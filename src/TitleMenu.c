#include "pebble.h"

#include "Adventure.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "MiniDungeon.h"
#include "UILayers.h"
	
#include "DungeonCrawl.h"
#include "DragonQuest.h"

void TitleMenuWindowAppear(Window *window)
{
	MenuAppear(window);
	ShowMainWindowRow(0, "Paused", "");
}

static MenuDefinition titleMenuDef = 
{
	.menuEntries = 
	{
		{NULL, NULL, NULL},
#if INCLUDE_DUNGEON_CRAWL
		{"Dungeon", "A simple dungeon crawl", LaunchDungeonCrawl},
#else
		{NULL, NULL, NULL},
#endif
#if INCLUDE_DRAGON_QUEST
		{"Quest", "Quest to kill a dragon", LaunchDragonQuest},
#else
		{NULL, NULL, NULL},
#endif
	},
	.appear = TitleMenuWindowAppear,
	.mainImageId = RESOURCE_ID_IMAGE_REST
};

void ShowTitleMenu(void)
{
	INFO_LOG("Entering title menu.");
	PushNewMenu(&titleMenuDef);
}
