#pragma once
	
typedef enum 
{
	LOCATIONTYPE_FIXED = 0,
	LOCATIONTYPE_PATH = 1,
	LOCATIONTYPE_DUNGEON = 2,
} LocationType;

typedef enum
{
	LOCATIONUPDATE_COMPUTERANDOM = 0,
	LOCATIONUPDATE_DONOTHING,
	LOCATIONUPDATE_FULLREFRESH,
} LocationUpdateReturnType;

typedef struct Location Location;

typedef void (*LocationFunction)(void);
typedef bool (*PrerequisiteFunction)(void);

#define LOCATION_MAX_MONSTERS 4
#define LOCATION_MAX_BACKGROUND_IMAGES 4
#define LOCATION_MAX_ADJACENT_LOCATIONS 4
	
#define PATH_CLASS_MAX_MONSTERS 10

int GetLocationBackgroundImageId(Location *location, bool dungeonFixed);
int GetLocationMonsterIndex(Location *location, bool dungeonFixed, uint floor);
int GetLocationEncounterChance(Location *location, bool dungeonFixed);
int GetLocationMonsterCount(Location *location, bool dungeonFixed);
const char *GetLocationName(Location *location);

size_t SizeOfLocation(void);
Location *GetLocationByIndex(Location *locationList, int index);
int GetLocationLength(Location *location);
int GetLocationBaseLevel(Location *location, int floor);

int GetAdjacentLocationIndex(Location *location, int index);

LocationType GetLocationType(Location *location);
int GetDestinationOfPath(Location *location, int lastIndex);

void RunArrivalFunction(Location *location);

uint8_t GetLocationNumberOfFloors(Location *location);

bool LocationAllowsShop(Location *location, bool dungeonFixed);

