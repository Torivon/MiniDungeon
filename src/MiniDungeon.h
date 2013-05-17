#pragma once

// Game tuning 
#define STAT_POINTS_PER_LEVEL 2
#define XP_FOR_NEXT_LEVEL 5 * level

// Feature tuning
#define PAD_WITH_SPACES 1

// Publish 0 turns on the test menu and ignores the official setting 
// The back button override is always on in test mode
#define PUBLISH 1
// Official 0 allows the back button override, while official 1 does not
#define OFFICIAL 1

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

// Set up the back button and test menu based on previous choices
#if PUBLISH
	#if OFFICIAL
		#define OVERRIDE_BACK_BUTTON 0
	#else
		#define OVERRIDE_BACK_BUTTON 1
	#endif
	#define ALLOW_TEST_MENU 0
#else
	// Overriding the back button is not officially supported, but works
	#define OVERRIDE_BACK_BUTTON 1
	#define ALLOW_TEST_MENU 1
#endif

// This needs to be large enough to handle the maximum size of the window stack
#define MAX_MENU_WINDOWS 4
// This determines how many text rows there are in the interface
#define MAX_MENU_ENTRIES 6

void ResetGame(void);
