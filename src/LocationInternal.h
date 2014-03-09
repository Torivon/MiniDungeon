#pragma once
	
#include "Location.h"

typedef struct PathClass
{
	uint numberOfMonsters : 8;
	uint encounterChance : 8; // Should be a number in the range [0-100]. This is the chance in any given minute that a monter will attack.
	uint numberOfBackgroundImages : 8;
	uint monsterUnlockLevel : 8; // When the base level of the location is greater than or equal to this, all monster types are available.
	uint16_t monsters[PATH_CLASS_MAX_MONSTERS]; // The set of random monsters that can appear
	int backgroundImages[LOCATION_MAX_BACKGROUND_IMAGES]; // resource ids		
} PathClass;

typedef struct FixedClass
{
	uint16_t monster; // A fixed location only has one monster, to be used if the arrival function is ShowBattleWindow
	int backgroundImage; // resource id
	bool allowShop;
} FixedClass;

// The dungeon class interprets Location's length as per floor
typedef struct DungeonClass
{
	uint8_t numberOfFloors;
	uint8_t levelIncreaseRate; // For every x floors, add 1 to the baseLevel.
	PathClass *pathclass;
	FixedClass *fixedclass;
} DungeonClass;

typedef struct Location
{
	char *name;
	uint type : 2;
	uint numberOfAdjacentLocations : 3;
	
	uint8_t length; // For a path, how many minutes it takes to traverse.
	uint8_t baseLevel; // This determines the level of monsters encountered

	uint16_t adjacentLocations[LOCATION_MAX_ADJACENT_LOCATIONS]; // For a town or dungeon, these can be paths leading away or fixed locations. For a path, only two can be set and they are the endpoints
	
	union
	{
		LocationFunction fixed_ArrivalFunction; // Run when you arrive at a new location
		PrerequisiteFunction path_PrerequisiteFunction; // Determines whether you can enter the location
	};

	union
	{
		PathClass *pathclass;
		FixedClass *fixedclass;
		DungeonClass *dungeonclass;
	};
} Location;