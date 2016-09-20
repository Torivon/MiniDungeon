#pragma once

#define BUILD_CLOUDPEBBLE_WORKER_APP 0

#if BUILD_CLOUDPEBBLE_WORKER_APP
#define BUILD_WORKER_FILES
#endif

// Game tuning 
#define STAT_POINTS_PER_LEVEL 2
#define XP_FOR_NEXT_LEVEL 5 * level

// Feature tuning
#define PAD_WITH_SPACES 1
#define DISABLE_MENU_BMPS 0

// Publish 0 turns on the test menu
#define PUBLISH 1
	
// Logging
#define DEBUG_LOGGING 0 // DEBUG_LOGGING 1 turns on DEBUG_LOG. DEBUG_LOGGING 2 turns on DEBUG_VERBOSE_LOG also.
#define ERROR_LOGGING 1 // ERROR_LOG should be used for actual incorrect operation.
#define WARNING_LOGGING 1 // WARNING_LOG should be used strange edge cases that are not expected
#define INFO_LOGGING 1 // INFO_LOG should be used to log game flow.
#define PROFILE_LOGGING 0 // PROFILE_LOGGING shows up as DEBUG_VERBOSE_LOG, but should be used for timing

// Features to turn off to make space
#define ALLOW_RANDOM_DUNGEON_GRAPHICS 1
#define BOUNDS_CHECKING 1
#define EVENT_CHANCE_SCALING 1

#define ALLOW_SHOP 1
#if ALLOW_SHOP
	#define ALLOW_ITEM_SHOP 1
	#define ALLOW_STAT_SHOP 1
#else
	#define ALLOW_ITEM_SHOP 0
	#define ALLOW_STAT_SHOP 0
#endif	

// Set up the test menu based on previous choices
#if PUBLISH
	#define ALLOW_TEST_MENU 0
	#define FAST_MODE_IN_BACKGROUND false
#else
	#define ALLOW_TEST_MENU 1
	#define FAST_MODE_IN_BACKGROUND true
#endif

// This needs to be large enough to handle the maximum size of the window stack
#define MAX_MENU_WINDOWS 4
// This determines how many text rows there are in the interface
#define MAX_MENU_ENTRIES 6

#define MAX_EVENT_COUNT 10
	
#define ALLOW_WORKER_APP 1
#define ALLOW_WORKER_APP_MESSAGES 1
#define ALLOW_WORKER_APP_LISTENING 1

void ResetGame(void);
int HasFocus(void);
