#pragma once

// Game tuning 
#define STAT_POINTS_PER_LEVEL 2
#define XP_FOR_NEXT_LEVEL 5 * level

// Feature tuning
#define PAD_WITH_SPACES 1
#define DISABLE_MENU_BMPS 0
	
#define ENABLE_ITEMS 0
#define ENABLE_SHOPS 0

// Publish 0 turns on the test menu
#define PUBLISH 1
	
// Stories to include
#define INCLUDE_DUNGEON_CRAWL 1
#define INCLUDE_DRAGON_QUEST 1

#define INCLUDE_SLIDESHOW 0

enum
{
	DUNGEON_CRAWL_INDEX = 0,
	DRAGON_QUEST_INDEX = 1,
};

// Logging
#define DEBUG_LOGGING 0 // DEBUG_LOGGING 1 turns on DEBUG_LOG. DEBUG_LOGGING 2 turns on DEBUG_VERBOSE_LOG also.
#define ERROR_LOGGING 0 // ERROR_LOG should be used for actual incorrect operation.
#define WARNING_LOGGING 0 // WARNING_LOG should be used strange edge cases that are not expected
#define INFO_LOGGING 0 // INFO_LOG should be used to log game flow.
#define PROFILE_LOGGING 0 // PROFILE_LOGGING shows up as DEBUG_VERBOSE_LOG, but should be used for timing

// Features to turn off to make space
#define BOUNDS_CHECKING 1

// Set up the test menu based on previous choices
#if PUBLISH
	#define ALLOW_TEST_MENU 0
	#define FAST_MODE_IN_BACKGROUND false
#else
	#define ALLOW_TEST_MENU 1
	#define FAST_MODE_IN_BACKGROUND true
#endif

// This needs to be large enough to handle the maximum size of the window stack
#define MAX_MENU_WINDOWS 7
// This determines how many text rows there are in the interface
#define MAX_MENU_ENTRIES 6

#define MAX_EVENT_COUNT 10
	
#define ALLOW_WORKER_APP 0
#define ALLOW_WORKER_APP_MESSAGES 0
#define ALLOW_WORKER_APP_LISTENING 0

void ResetGame(void);
bool HasFocus(void);
