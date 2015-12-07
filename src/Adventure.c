#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Events.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "Persistence.h"
#include "OptionsMenu.h"
#include "Shop.h"
#include "Story.h"
#include "UILayers.h"
#include "Utils.h"
#include "WorkerControl.h"

bool gUpdateAdventure = false;

int updateDelay = 0;

void SetUpdateDelay(void)
{
	updateDelay = 1;
}

bool adventureWindowVisible = false;

bool AdventureWindowIsVisible(void)
{
	return adventureWindowVisible;
}

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
#if ENABLE_SHOPS
	ResetStatPointsPurchased();
#endif
	InitializeCharacter();
#if ENABLE_ITEMS
	ClearInventory();
#endif
	InitializeCurrentStory();
	
	SavePersistedData();
}

void RefreshAdventure(void)
{
	if(!adventureWindowVisible)
		return;
	
	DEBUG_VERBOSE_LOG("Refreshing adventure window.");
	ShowMainWindowRow(0, GetCurrentLocationName(), GetCurrentLocationFloor());
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
		TravelToAdjacentLocationByIndex(0);
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
		TravelToAdjacentLocationByIndex(1);
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
		TravelToAdjacentLocationByIndex(2);
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
		TravelToAdjacentLocationByIndex(3);
		RefreshAdventure();
	}
}

const char *ShopMenuEntryTextFunction(void)
{
	if(CurrentLocationAllowsShop())
		return "Shop";
	else
		return NULL;
}

const char *ShopMenuEntryDescriptionFunction(void)
{
	if(CurrentLocationAllowsShop())
		return "Visit a shop";
	else
		return NULL;
}

MenuDefinition adventureMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Main", .description = "Open the main menu", .menuFunction = ShowMainMenu},
#if ENABLE_SHOPS
		{.useFunctions = true, .textFunction = ShopMenuEntryTextFunction, .descriptionFunction = ShopMenuEntryDescriptionFunction, .menuFunction = ShowShopWindow},
#else
		{0},
#endif			
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
	.mainImageId = RESOURCE_ID_IMAGE_DUNGEONRIGHT,
	.floorImageId = -1
};

Window *adventureWindow = NULL;

void LoadLocationImage(void)
{
	adventureMenuDef.mainImageId = GetCurrentBackgroundImage();
	if(adventureWindow)
		LoadMainBmpImage(adventureWindow, adventureMenuDef.mainImageId, adventureMenuDef.floorImageId);
}

void AdventureWindowAppear(Window *window)
{
	INFO_LOG("Adventure window: %s", GetCurrentLocationName());
	MenuAppear(window);
	adventureWindow = window;
	adventureWindowVisible = true;
	gUpdateAdventure = true;
	RefreshAdventure();
}

void AdventureWindowDisappear(Window *window)
{
	DEBUG_LOG("Adventure disappear");
	adventureWindowVisible = false;
	gUpdateAdventure = false;
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
	LocationUpdateReturnType returnVal;
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
	
	returnVal = UpdateCurrentLocation();

	switch(returnVal)
	{
		case LOCATIONUPDATE_COMPUTERANDOM:
			ComputeRandomEvent();
			LoadLocationImage();
			break;
		case LOCATIONUPDATE_DONOTHING:
			break;
		case LOCATIONUPDATE_FULLREFRESH:
			if(GetVibration())
				vibes_short_pulse();

			RefreshAdventure();
			break;
	}
}
