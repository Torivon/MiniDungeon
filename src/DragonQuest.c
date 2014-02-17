#include "pebble.h"

#include "Adventure.h"
#include "Location.h"
#include "LocationInternal.h"
#include "Story.h"
#include "MiniDungeon.h"

#if INCLUDE_DRAGON_QUEST	

Story dragonQuestStory = 
{
	locationList = 
	{
		{
			.name = "Home Town",
			.type = LOCATIONTYPE_FIXED,
			.adjacentLocation1 = 1
		},
		{
			.name = "Forest Road",
			.type = LOCATIONTYPE_PATH,
			.adjacentLocation1 = 0,
			.adjacentLocation2 = 2,
			.length = 30,
		},
		{
			.name = "Dungeon Entrance",
			.type = LOCATIONTYPE_FIXED,
			.adjacentLocation1 = 1,
		},
		{
			.name = "Dungeon Path",
			.type = LOCATIONTYPE_PATH,
			.adjacentLocation1 = 1,
		},
	}	
}

StoryState dragonQuestStoryState = {0};

#endif

void LaunchDragonQuestStory(void)
{
#if INCLUDE_DRAGON_QUEST
	RegisterStory(&dragonQuestStory, &dragonQuestStoryState);
	ShowAdventureWindow();
#endif
}
