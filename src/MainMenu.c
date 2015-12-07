#include "pebble.h"

#include "Adventure.h"
#include "Character.h"
#include "Battle.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "OptionsMenu.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"

// **************** TEST MENU ******************//

void DoNothing(void)
{
}

#if ALLOW_TEST_MENU

void ForceNewFloor(void)
{
	PopMenu();
	ShowNewFloorWindow();
}

void ForceItemGain(void)
{
	PopMenu();
	ShowItemGainWindow();
}

void ForceBattle(void)
{
	PopMenu();
	ShowBattleWindow();
}

void ForceShop(void)
{
	PopMenu();
	ShowShopWindow();
}

MenuDefinition testMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Quit", .description = "", .menuFunction = PopMenu},
		{.text = "Random", .description = "", .menuFunction = ForceEvent},
		{.text = "NewFloor", .description = "", .menuFunction = ForceNewFloor},
		{.text = "Item", .description = "", .menuFunction = ForceItemGain},
		{.text = "Battle", .description = "", .menuFunction = ForceBattle},
		{.text = "Shop", .description = "", .menuFunction = ForceShop},
	},
	.mainImageId = -1,
	.floorImageId = -1
};

void ShowTestMenu(void)
{
	PushNewMenu(&testMenuDef);
}

void ForceDragonSetup(void)
{
	int i = 0;
	SetCurrentFloor(20);
	GrantGold(5000);
	for(i = 0; i < 20; ++i)
	{
		LevelUpData();
	}
	LevelUp();
}

MenuDefinition testMenu2Def = 
{
	.menuEntries = 
	{
		{"Quit", "", PopMenu},
		{"Dragon", "", ForceDragonSetup},
	},
	.mainImageId = -1
};

void ShowTestMenu2(void)
{
	PushNewMenu(&testMenu2Def);
}
#endif

//************* Application Stats *****************//

#if ALLOW_TEST_MENU

static size_t minMemoryFree = (size_t)-1;

void InfoWindowAppear(Window *window);

void UpdateMinFreeMemory()
{
	size_t bytesFree = heap_bytes_free();
	if(bytesFree < minMemoryFree)
		minMemoryFree = bytesFree;
}

MenuDefinition infoMenuDef = 
{
	.menuEntries = 
	{
		{"Quit", "", PopMenu},
	},
	.appear = InfoWindowAppear,
	.mainImageId = -1,
	.floorImageId = -1
};

const char *UpdateFreeText(void)
{
	static char freeText[] = "0000000"; // Needs to be static because it's used by the system later.
	size_t bytesFree = heap_bytes_free();
	IntToString(freeText, 7, bytesFree);
	return freeText;
}

const char *UpdateMinFreeText(void)
{
	static char minFreeText[] = "0000000"; // Needs to be static because it's used by the system later.
	IntToString(minFreeText, 7, minMemoryFree);
	return minFreeText;
}

void InfoWindowAppear(Window *window)
{
	MenuAppear(window);
	
	ShowMainWindowRow(0, "Info", "");
	ShowMainWindowRow(1, UpdateFreeText(), "Free");
	ShowMainWindowRow(2, UpdateMinFreeText(), "Min");
}

void ShowInfoMenu(void)
{
	PushNewMenu(&infoMenuDef);
}
#endif

//************* Main Menu *****************//

static bool mainMenuVisible = false;

void MainMenuWindowAppear(Window *window);
void MainMenuWindowDisappear(Window *window);

MenuDefinition mainMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Quit", .description = "Return to adventure", .menuFunction = PopMenu},
#if ENABLE_ITEMS
		{.text = "Items", .description = "Items Owned", .menuFunction = ShowMainItemMenu},
#else
		{0},
#endif
		{.text = "Progress", .description = "Character advancement", .menuFunction = ShowProgressMenu},
		{.text = "Stats", .description = "Character Stats", .menuFunction = ShowStatMenu},
		{.text = "Options", .description = "Open the options menu", .menuFunction = ShowOptionsMenu},
		{.text = "Reset", .description = "Reset the game", .menuFunction = ResetGame},
	},
	.appear = MainMenuWindowAppear,
	.disappear = MainMenuWindowDisappear,
	.mainImageId = RESOURCE_ID_IMAGE_REST,
	.floorImageId = RESOURCE_ID_IMAGE_BATTLE_FLOOR
};

void UpdateBatteryText(BatteryChargeState chargeState, char *buffer, int count)
{
	DEBUG_VERBOSE_LOG("Updating battery text");
	if (chargeState.is_charging) {
		snprintf(buffer, count, "Chg");
	} else if(chargeState.charge_percent == 100) {
		snprintf(buffer, count, "Full");
	} else {
		IntToPercent(buffer, count, chargeState.charge_percent);
	}	
	
	DEBUG_VERBOSE_LOG("Drawing new battery info");
}

void ShowPauseRow(BatteryChargeState chargeState)
{
	static char s_battery_buffer[5] = "0000";

	UpdateBatteryText(chargeState, s_battery_buffer, sizeof(s_battery_buffer));
	ShowMainWindowRow(0, "Paused",  s_battery_buffer);
}

void BatteryHandler(BatteryChargeState charge)
{
	DEBUG_VERBOSE_LOG("BatteryHandler called");
	if(mainMenuVisible)
		ShowPauseRow(charge);
}

void MainMenuWindowAppear(Window *window)
{
	mainMenuVisible = true;
	MenuAppear(window);
	ShowPauseRow(battery_state_service_peek());
	battery_state_service_subscribe(BatteryHandler);
}

void MainMenuWindowDisappear(Window *window)
{
	mainMenuVisible = false;
	battery_state_service_unsubscribe();
	MenuDisappear(window);
}

void ShowMainMenu(void)
{
	INFO_LOG("Entering main menu. Game paused.");
	PushNewMenu(&mainMenuDef);
}
