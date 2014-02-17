#include "pebble.h"

#include "LocationInternal.h"
#include "Location.h"
#include "Logging.h"
#include "Utils.h"

int GetLocationBackgroundImageId(Location *location)
{
	int index;
	
	if(location->numberOfBackgroundImages > LOCATION_MAX_BACKGROUND_IMAGES)
		return -1;
	
	if(location->numberOfBackgroundImages == 1)
	{
		return location->backgroundImages[0];
	}
	else
	{
		index = Random(location->numberOfBackgroundImages);
		return location->backgroundImages[index];
	}
}

int GetLocationMonsterIndex(Location *location)
{
	int index;
	
	if(location->numberOfMonsters > LOCATION_MAX_MONSTERS)
		return -1;
	
	if(location->numberOfMonsters == 1)
	{
		return location->monsters[0];
	}
	else
	{
		index = Random(location->numberOfMonsters);
		return location->monsters[index];
	}
}

int GetLocationEncounterChance(Location *location)
{
	return location->encounterChance;
}

const char *GetLocationName(Location* location)
{
	return location->name;
}

int GetLocationLength(Location *location)
{
	return location->length;
}

int GetLocationBaseLevel(Location *location)
{
	return location->baseLevel;
}

size_t SizeOfLocation(void)
{
	return sizeof(Location);
}

Location *GetLocationByIndex(Location *locationList, int index)
{
	if(!locationList)
	{
		DEBUG_LOG("No location list passed in.");
		return NULL;
	}
	
	return &locationList[index];
}

int GetAdjacentLocationIndex(Location *location, int index)
{
	if(index < 0 || index >= location->numberOfAdjacentLocations)
		return -1;
	
	return location->adjacentLocations[index];
}

LocationType GetLocationType(Location *location)
{
	return location->type;
}

int GetDestinationOfPath(Location *location, int lastIndex)
{
	if(!location)
		return -1;
	
	if(location->numberOfAdjacentLocations != 2)
		return -1;
	
	if(location->adjacentLocations[0] == lastIndex)
		return location->adjacentLocations[1];

	if(location->adjacentLocations[1] == lastIndex)
		return location->adjacentLocations[0];
	
	return -1;
}
