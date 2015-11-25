#include "pebble.h"

#include "Adventure.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "MiniDungeon.h"
#include "Slideshow.h"
#include "UILayers.h"
	
#include "DungeonCrawl.h"
#include "DragonQuest.h"

void TitleMenuWindowAppear(Window *window)
{
	MenuAppear(window);
	ShowMainWindowRow(0, "MiniAdventure", "");
}

static MenuDefinition titleMenuDef = 
{
	.menuEntries = 
	{
		{.text = NULL, .description = NULL, .menuFunction = NULL},
#if INCLUDE_DUNGEON_CRAWL
		{.text = "Dungeon", .description = "A simple dungeon crawl", .menuFunction = LaunchDungeonCrawl},
#else
		{.text = NULL, .description = NULL, .menuFunction = NULL},
#endif
#if INCLUDE_DRAGON_QUEST
		{.text = "Quest", .description = "Quest to kill a dragon", .menuFunction = LaunchDragonQuest},
#else
		{.text = NULL, .description = NULL, .menuFunction = NULL},
#endif
#if INCLUDE_SLIDESHOW
		{.text = "Slideshow", .description = "Slideshow of all art", .menuFunction = LaunchSlideshow},
#else
		{.text = NULL, .description = NULL, .menuFunction = NULL},
#endif
	},
	.appear = TitleMenuWindowAppear,
	.mainImageId = RESOURCE_ID_IMAGE_TITLE,
	.floorImageId = -1
};

void ShowTitleMenu(void)
{
	INFO_LOG("Entering title menu.");
	PushNewMenu(&titleMenuDef);
}
