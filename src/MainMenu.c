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
		{"Quit", "", PopMenu},
		{"Random", "", ForceEvent},
		{"NewFloor", "", ForceNewFloor},
		{"Item", "", ForceItemGain},
		{"Battle", "", ForceBattle},
		{"Shop", "", ForceShop},
	},
	.mainImageId = -1
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

//************* Main Menu *****************//

void MainMenuWindowAppear(Window *window);

MenuDefinition mainMenuDef = 
{
	.menuEntries = 
	{
		{"Quit", "Return to adventure", PopMenu},
		{"Items", "Items Owned", ShowMainItemMenu},
		{"Progress", "Character advancement", ShowProgressMenu},
		{"Stats", "Character Stats", ShowStatMenu},
		{"Options", "Open the options menu", ShowOptionsMenu},
	},
	.appear = MainMenuWindowAppear,
	.mainImageId = RESOURCE_ID_IMAGE_REST,
	.floorImageId = RESOURCE_ID_IMAGE_BATTLE_FLOOR,
	.useFloorImage = true
};

void MainMenuWindowAppear(Window *window)
{
	static char s_battery_buffer[5] = "0000";
	BatteryChargeState charge_state = battery_state_service_peek();
	MenuAppear(window);

	if (charge_state.is_charging) {
		snprintf(s_battery_buffer, sizeof(s_battery_buffer), "Chg");
	} else if(charge_state.charge_percent == 100) {
		snprintf(s_battery_buffer, sizeof(s_battery_buffer), "Full");
	} else {
		IntToPercent(s_battery_buffer, sizeof(s_battery_buffer), charge_state.charge_percent);
	}
	ShowMainWindowRow(0, "Paused",  s_battery_buffer);
}

void ShowMainMenu(void)
{
	INFO_LOG("Entering main menu. Game paused.");
	PushNewMenu(&mainMenuDef);
}
