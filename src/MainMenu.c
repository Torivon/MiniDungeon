#include "pebble.h"

#include "Adventure.h"
#include "Character.h"
#include "Battle.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "Shop.h"
#include "UILayers.h"

// **************** TEST MENU ******************//

void DoNothing(void)
{
}

#if ALLOW_TEST_MENU

void ForceEvent(void)
{
	PopMenu();
	ComputeRandomEvent(true);
}

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
	.mainImageId = -1
};

void ShowTestMenu(void)
{
	PushNewMenu(&testMenuDef);
}
#endif

//************* Main Menu *****************//

static bool vibration = true;

void DrawOptionsMenu(void)
{
	ShowMainWindowRow(0, "Options", "");
	ShowMainWindowRow(1, "Vibration", vibration ? "On" : "Off");
}

void ToggleVibration(void)
{
	vibration = !vibration;
	DrawOptionsMenu();
}

bool GetVibration(void)
{
	return vibration;
}

void SetVibration(bool enable)
{
	vibration = enable;
}

void OptionsMenuAppear(Window *window);

MenuDefinition optionsMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Quit", .description = "Return to main menu", .menuFunction = PopMenu},
		{.text = "Toggle", .description = "Toggle Vibration", .menuFunction = ToggleVibration},
	},
	.appear = OptionsMenuAppear,
	.mainImageId = -1
};

void OptionsMenuAppear(Window *window)
{
	MenuAppear(window);
	DrawOptionsMenu();
}

void ShowOptionsMenu(void)
{
	PushNewMenu(&optionsMenuDef);
}

void MainMenuWindowAppear(Window *window);

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
	.mainImageId = RESOURCE_ID_IMAGE_REST
};

void MainMenuWindowAppear(Window *window)
{
	MenuAppear(window);
	ShowMainWindowRow(0, "Paused", "");
}

void ShowMainMenu(void)
{
	INFO_LOG("Entering main menu. Game paused.");
	PushNewMenu(&mainMenuDef);
}
