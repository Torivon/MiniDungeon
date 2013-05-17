#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "MainMenu.h"
#include "Menu.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"

const char *UpdateFloorText(void)
{
	static char floorText[] = "00"; // Needs to be static because it's used by the system later.
	IntToString(floorText, 2, GetCurrentFloor());
	return floorText;
}

int updateDelay = 0;
bool adventureWindowVisible = false;

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
		{"", "", ShowTestMenu}
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
	
	if(!adventureWindow)
		return;

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

	LoadMainBmpImage(adventureWindow, adventureMenuDef.mainImageId);
}

void AdventureWindowAppear(Window *window)
{
	MenuAppear(window);
	ShowMainWindowRow(0, "Floor", UpdateFloorText());
	adventureWindow = window;
	UpdateCharacterHealth();
	UpdateCharacterLevel();
	updateDelay = 1;
	adventureWindowVisible = true;
}

void AdventureWindowDisappear(Window *window)
{
	adventureWindowVisible = false;
	MenuDisappear(window);
	adventureWindow = NULL;
}

void ShowAdventureWindow(void)
{
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
#if EVENT_CHANCE_SCALING
static int ticksSinceLastEvent = 0;
#endif

bool ComputeRandomEvent(bool fastMode)
{
	int result = Random(100);
	int i = 0;
	int acc = 0;
	int chanceOfEvent = baseChanceOfEvent;
#if EVENT_CHANCE_SCALING
	if(ticksSinceLastEvent > 20)
	{
		chanceOfEvent += (ticksSinceLastEvent - 20) * 2;
	}
#endif
	
	if(!fastMode && result > chanceOfEvent)
		return false;
		
	result = Random(100);
	
	do
	{
		acc += entries[i].weight;
		if(acc >= result)
		{
			if(GetVibration())
				vibes_short_pulse();
			if(entries[i].windowFunction)
				entries[i].windowFunction();
#if EVENT_CHANCE_SCALING
			ticksSinceLastEvent = 0;
#endif
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
#if EVENT_CHANCE_SCALING
	++ticksSinceLastEvent;
#endif
	if(updateDelay && !GetFastMode())
	{
		--updateDelay;
		return;
	}
 
	ComputeRandomEvent(GetFastMode());
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
