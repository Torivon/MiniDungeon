#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "Shop.h"
#include "Story.h"
#include "UILayers.h"
#include "Utils.h"

int updateDelay = 0;
bool adventureWindowVisible = false;

void AdventureWindowAppear(Window *window);
void AdventureWindowDisappear(Window *window);
void LoadLocationImage(void);

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
}

void FollowFirstPath(void)
{
	DEBUG_LOG("Trying to follow first path");
	if(!IsCurrentLocationPath())
	{
		SetNewLocation(GetCurrentAdjacentLocationIndex(0));
		RefreshAdventure();
	}
}

void EndPath(void)
{
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
 
MenuDefinition adventureMenuDef = 
{
	.menuEntries = 
	{
		{"Main", "Open the main menu", ShowMainMenu},
		{"Leave", "Follow a path", FollowFirstPath}, // Debug: If this is a fixed location, travel along path 0
		{"Shop", "Visit a shop", ShowShopWindow}, // Debug: This should not be here on paths
#if ALLOW_TEST_MENU
		{NULL, NULL, NULL},
		{NULL, NULL, NULL},
		{"", "", ShowTestMenu}
#endif
	},
	.appear = AdventureWindowAppear,
	.disappear = AdventureWindowDisappear,
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

void ShowAdventureWindow(void)
{
	INFO_LOG("Adventure Window");
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
