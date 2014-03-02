#pragma once
	
#include "Location.h"

typedef struct Location
{
	char *name;
	LocationType type;
	
	int numberOfAdjacentLocations;
	int adjacentLocations[LOCATION_MAX_ADJACENT_LOCATIONS]; // For a town or dungeon, these must be paths leading away. For a path, only two can be set and they are the endpoints
	
	int length; // For a path, how many minutes it takes to traverse.
	int baseLevel; // This determines the level of monsters encountered
	
	LocationFunction arrivalFunction; // Run when you arrive at a new location
	PrerequisiteFunction prerequisiteFunction; // Determines whether you can enter the location
	
	int numberOfMonsters;
	int monsters[LOCATION_MAX_MONSTERS]; // The set of random monsters that can appear
	int encounterChance; // Should be a number in the range [0-100]. This is the chance in any given minute that a monter will attack.
	
	int numberOfBackgroundImages;
	int backgroundImages[LOCATION_MAX_BACKGROUND_IMAGES]; // resource ids	
} Location;

