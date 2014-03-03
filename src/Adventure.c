#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "Persistence.h"
#include "Shop.h"
#include "Story.h"
#include "UILayers.h"
#include "Utils.h"

int updateDelay = 0;
bool adventureWindowVisible = false;

void AdventureWindowAppear(Window *window);
void AdventureWindowDisappear(Window *window);
void AdventureWindowInit(Window *window);
void AdventureWindowDeinit(Window *window);
void LoadLocationImage(void);

void InitializeGameData(void)
{
	if(!LoadPersistedData())
		ResetGame();
}

void ResetGame(void)
{
	INFO_LOG("Resetting game.");
	ResetStatPointsPurchased();
	InitializeCharacter();
	ClearInventory();
	InitializeCurrentStory();
	
	SavePersistedData();
}

void RefreshAdventure(void)
{
	if(!adventureWindowVisible)
		return;
	
	DEBUG_VERBOSE_LOG("Refreshing adventure window.");
	ShowMainWindowRow(0, GetCurrentLocationName(), "");
	UpdateCharacterHealth();
	UpdateCharacterLevel();
	updateDelay = 1;
	LoadLocationImage();
	RefreshMenuAppearance();
}

const char *Path0Text(void)
{
	if(IsCurrentLocationPath())
		return NULL;

	return GetAdjacentLocationName(0);
}

void FollowPath0(void)
{
	DEBUG_LOG("Trying to follow first path");
	if(!IsCurrentLocationPath())
	{
		SetNewLocation(GetCurrentAdjacentLocationIndex(0));
		RefreshAdventure();
	}
}

const char *Path1Text(void)
{
	if(IsCurrentLocationPath())
		return NULL;

	return GetAdjacentLocationName(1);
}

void FollowPath1(void)
{
	DEBUG_LOG("Trying to follow second path");
	if(!IsCurrentLocationPath())
	{
		SetNewLocation(GetCurrentAdjacentLocationIndex(1));
		RefreshAdventure();
	}
}

const char *Path2Text(void)
{
	if(IsCurrentLocationPath())
		return NULL;

	return GetAdjacentLocationName(2);
}

void FollowPath2(void)
{
	DEBUG_LOG("Trying to follow first path");
	if(!IsCurrentLocationPath())
	{
		SetNewLocation(GetCurrentAdjacentLocationIndex(2));
		RefreshAdventure();
	}
}

const char *Path3Text(void)
{
	if(IsCurrentLocationPath())
		return NULL;

	return GetAdjacentLocationName(3);
}

void FollowPath3(void)
{
	DEBUG_LOG("Trying to follow first path");
	if(!IsCurrentLocationPath())
	{
		SetNewLocation(GetCurrentAdjacentLocationIndex(3));
		RefreshAdventure();
	}
}

void EndPath(void)
{
	if(GetVibration())
		vibes_short_pulse();
	SetNewLocation(GetCurrentDestinationIndex());
	RefreshAdventure();
}

bool UpdatePath(void)
{
	IncrementCurrentDuration();
	DEBUG_LOG("Time in current location: %d/%d.", GetCurrentDuration(), GetCurrentLocationLength());
	if(GetCurrentDuration() >= GetCurrentLocationLength())
	{
		EndPath();
		return true;
	}
	
	return false;
}

const char *ShopMenuEntryTextFunction(void)
{
	if(IsCurrentLocationPath())
		return NULL;
	else
		return "Shop";
}

const char *ShopMenuEntryDescriptionFunction(void)
{
	if(IsCurrentLocationPath())
		return NULL;
	else
		return "Visit a shop";
}

#if ALLOW_TEST_MENU
		{.text = "", .description = "", .menuFunction = ShowTestMenu}
#endif

MenuDefinition adventureMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Main", .description = "Open the main menu", .menuFunction = ShowMainMenu},
		{.useFunctions = true, .textFunction = ShopMenuEntryTextFunction, .descriptionFunction = ShopMenuEntryDescriptionFunction, .menuFunction = ShowShopWindow},
		{.useFunctions = true, .textFunction = Path0Text, .descriptionFunction = Path0Text, .menuFunction = FollowPath0},
		{.useFunctions = true, .textFunction = Path1Text, .descriptionFunction = Path1Text, .menuFunction = FollowPath1},
		{.useFunctions = true, .textFunction = Path2Text, .descriptionFunction = Path2Text, .menuFunction = FollowPath2},
		{.useFunctions = true, .textFunction = Path3Text, .descriptionFunction = Path3Text, .menuFunction = FollowPath3},
	},
	.appear = AdventureWindowAppear,
	.disappear = AdventureWindowDisappear,
	.init = AdventureWindowInit,
	.deinit = AdventureWindowDeinit,
	.animated = true,
	.mainImageId = RESOURCE_ID_IMAGE_DUNGEONRIGHT
};

Window *adventureWindow = NULL;

void LoadLocationImage(void)
{
	adventureMenuDef.mainImageId = GetCurrentBackgroundImage();
	if(adventureWindow)
		LoadMainBmpImage(adventureWindow, adventureMenuDef.mainImageId);
}

void AdventureWindowAppear(Window *window)
{
	INFO_LOG("Adventure window: %s", GetCurrentLocationName());
	MenuAppear(window);
	adventureWindow = window;
	adventureWindowVisible = true;
	RefreshAdventure();
}

void AdventureWindowDisappear(Window *window)
{
	adventureWindowVisible = false;
	MenuDisappear(window);
	adventureWindow = NULL;
}

void AdventureWindowInit(Window *window)
{
	MenuInit(window);
	DEBUG_LOG("InitializeGameData");
	InitializeGameData();	
}

void AdventureWindowDeinit(Window *window)
{
	SavePersistedData();
	ClearCurrentStory();
	MenuDeinit(window);
}

void ShowAdventureWindow(void)
{
	INFO_LOG("Adventure Window");
	CurrentStoryStateNeedsSaving();
	PushNewMenu(&adventureMenuDef);
}

typedef void (*ShowWindowFunction)(void);

typedef struct
{
	ShowWindowFunction windowFunction;
	int weight;
} RandomTableEntry;

// These should add up to 100
RandomTableEntry entries[] = 
{
	{ShowBattleWindow, 100},
};

bool ComputeRandomEvent(void)
{
	int result = Random(100) + 1;
	int i = 0;
	int acc = 0;
	int chanceOfEvent = GetCurrentLocationEncounterChance();
	
	if(result > chanceOfEvent)
		return false;
		
	result = Random(100) + 1;
	
	do
	{
		acc += entries[i].weight;
		if(acc >= result)
		{
			if(GetVibration())
				vibes_short_pulse();
			if(entries[i].windowFunction)
				entries[i].windowFunction();
			break;
		}
		++i;      
    } while (i < 4);
	return true;
}

void UpdateAdventure(void)
{
	if(!adventureWindowVisible)
		return;
	
	if(IsBattleForced())
	{
		INFO_LOG("Triggering forced battle.");
		ShowBattleWindow();
		return;
	}

	if(updateDelay)
	{
		--updateDelay;
		return;
	}
	
	if(IsCurrentLocationPath())
	{
		if(UpdatePath())
			return;
		ComputeRandomEvent();
		LoadLocationImage();
	}
}
