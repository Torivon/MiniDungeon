#pragma once
	
#include "Location.h"

typedef struct Location
{
	char *name;
	LocationType type;

	uint length : 8; // For a path, how many minutes it takes to traverse.
	uint baseLevel : 8; // This determines the level of monsters encountered
	uint encounterChance : 8; // Should be a number in the range [0-100]. This is the chance in any given minute that a monter will attack.
	uint numberOfAdjacentLocations : 3;
	uint numberOfMonsters : 3;
	uint numberOfBackgroundImages : 3;
	
	uint16_t adjacentLocations[LOCATION_MAX_ADJACENT_LOCATIONS]; // For a town or dungeon, these must be paths leading away. For a path, only two can be set and they are the endpoints
	
	LocationFunction arrivalFunction; // Run when you arrive at a new location
	PrerequisiteFunction prerequisiteFunction; // Determines whether you can enter the location

	uint16_t monsters[LOCATION_MAX_MONSTERS]; // The set of random monsters that can appear

	uint16_t backgroundImages[LOCATION_MAX_BACKGROUND_IMAGES]; // resource ids	
} Location;
