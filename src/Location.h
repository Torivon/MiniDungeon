#pragma once
	
typedef enum 
{
	LOCATIONTYPE_FIXED = 0,
	LOCATIONTYPE_PATH = 1,
} LocationType;

typedef struct Location Location;

typedef void (*LocationFunction)(void);
typedef bool (*PrerequisiteFunction)(void);

#define LOCATION_MAX_MONSTERS 4
#define LOCATION_MAX_BACKGROUND_IMAGES 4
#define LOCATION_MAX_ADJACENT_LOCATIONS 4
	
#define PATH_CLASS_MAX_MONSTERS 10

int GetLocationBackgroundImageId(Location *location);
int GetLocationMonsterIndex(Location *location);
int GetLocationEncounterChance(Location *location);
const char *GetLocationName(Location *location);

size_t SizeOfLocation(void);
Location *GetLocationByIndex(Location *locationList, int index);
int GetLocationLength(Location *location);
int GetLocationBaseLevel(Location *location);

int GetAdjacentLocationIndex(Location *location, int index);

LocationType GetLocationType(Location *location);
int GetDestinationOfPath(Location *location, int lastIndex);

void RunArrivalFunction(Location *location);
