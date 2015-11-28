#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "OptionsMenu.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"
#include "WorkerControl.h"

const char *UpdateFloorText(void)
{
	static char floorText[] = "00"; // Needs to be static because it's used by the system later.
	IntToString(floorText, 2, GetCurrentFloor());
	return floorText;
}

int updateDelay = 0;
bool adventureWindowVisible = false;

bool AdventureWindowIsVisible(void)
{
	return adventureWindowVisible;
}

void AdventureWindowAppear(Window *window);
void AdventureWindowDisappear(Window *window);

MenuDefinition adventureMenuDef = 
{
	.menuEntries = 
	{
		{"Main", "Open the main menu", ShowMainMenu},
#if ALLOW_TEST_MENU
		{NULL, NULL, NULL},
		{NULL, NULL, NULL},
		{NULL, NULL, NULL},
		{NULL, NULL, NULL},
		{"TEST", "Test features", ShowTestMenu}
#endif
	},
	.appear = AdventureWindowAppear,
	.disappear = AdventureWindowDisappear,
	.animated = true,
	.mainImageId = RESOURCE_ID_IMAGE_DUNGEONRIGHT
};

Window *adventureWindow = NULL;

void LoadRandomDungeonImage(void)
{
#if ALLOW_RANDOM_DUNGEON_GRAPHICS		
	int result;
#endif
	
#if ALLOW_RANDOM_DUNGEON_GRAPHICS		
	result = Random(12);
	if(result < 6)
		adventureMenuDef.mainImageId = RESOURCE_ID_IMAGE_DUNGEONSTRAIGHT;
	else if(result < 9)
		adventureMenuDef.mainImageId = RESOURCE_ID_IMAGE_DUNGEONLEFT;
	else if(result < 12)
		adventureMenuDef.mainImageId = RESOURCE_ID_IMAGE_DUNGEONRIGHT;
	else
		adventureMenuDef.mainImageId = RESOURCE_ID_IMAGE_DUNGEONDEADEND;
#endif

	if(adventureWindow)
		LoadMainBmpImage(adventureWindow, adventureMenuDef.mainImageId, -1);
}

void AdventureWindowAppear(Window *window)
{
	INFO_LOG("Back to the adventure.");
	DEBUG_LOG("Adventure appear floor %d",GetCurrentFloor());
	MenuAppear(window);
	ShowMainWindowRow(0, "Floor", UpdateFloorText());
	adventureWindow = window;
	UpdateCharacterHealth();
	UpdateCharacterLevel();
	updateDelay = 1;
	adventureWindowVisible = true;
	if(WorkerIsRunning())
	{
		UnpauseWorkerApp();
	}
}

void AdventureWindowDisappear(Window *window)
{
	adventureWindowVisible = false;
	if(WorkerIsRunning())
	{
		PauseWorkerApp();
	}
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

#if ALLOW_SHOP
// These should add up to 100
RandomTableEntry entries[] = 
{
	{ShowItemGainWindow, 44},
	{ShowBattleWindow, 44},
	{ShowNewFloorWindow, 9},
	{ShowShopWindow, 3}
};
#else
// These should add up to 100
RandomTableEntry entries[] = 
{
	{ShowItemGainWindow, 40},
	{ShowBattleWindow, 50},
	{ShowNewFloorWindow, 10}
};
#endif

static int baseChanceOfEvent = 35;
static int ticksSinceLastEvent = 0;

int GetTickCount(void)
{
	return ticksSinceLastEvent;
}

void SetTickCount(int ticks)
{
	ticksSinceLastEvent = ticks;
}

void ExecuteEvent(int i)
{
	if(i == -1)
		return;
	
	if(GetVibration())
		vibes_short_pulse();
	if(entries[i].windowFunction)
		entries[i].windowFunction();
#if EVENT_CHANCE_SCALING
	ticksSinceLastEvent = 0;
#endif
	
}

int ComputeRandomEvent(bool fastMode)
{
	int result = Random(100);
	int i = 0;
	int acc = 0;
	int chanceOfEvent = baseChanceOfEvent;
	int event = -1;
#if EVENT_CHANCE_SCALING
	if(ticksSinceLastEvent > 20)
	{
		chanceOfEvent += (ticksSinceLastEvent - 20) * 2;
	}
#endif
	
	if(!fastMode && result > chanceOfEvent)
		return -1;
		
	result = Random(100);
	
	do
	{
		acc += entries[i].weight;
		if(acc >= result)
		{
			event = i;
			break;
		}
		++i;      
    } while (i < 4);
	return event;
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

#if EVENT_CHANCE_SCALING
	++ticksSinceLastEvent;
#endif
	if(updateDelay && !GetFastMode())
	{
		--updateDelay;
		return;
	}

	if(!WorkerIsHandlingUpdates())
		ExecuteEvent(ComputeRandomEvent(GetFastMode()));
	LoadRandomDungeonImage();
}

void NewFloorMenuInit(Window *window);
void NewFloorMenuAppear(Window *window);

MenuDefinition newFloorMenuDef = 
{
	.menuEntries = 
	{
		{"Ok", "Return to adventuring", PopMenu}
	},
	.init = NewFloorMenuInit,
	.appear = NewFloorMenuAppear,
	.mainImageId = RESOURCE_ID_IMAGE_NEWFLOOR
};

void NewFloorMenuInit(Window *window)
{
	MenuInit(window);
	IncrementFloor();
}

void NewFloorMenuAppear(Window *window)
{
	MenuAppear(window);
	ShowMainWindowRow(0, "New Floor", UpdateFloorText());
}

void ShowNewFloorWindow(void)
{
	PushNewMenu(&newFloorMenuDef);
}
