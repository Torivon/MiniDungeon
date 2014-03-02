#include "pebble.h"

#include "LocationInternal.h"
#include "Location.h"
#include "Logging.h"
#include "Utils.h"

int GetLocationBackgroundImageId(Location *location)
{
	int index;
	
	if(location->type == LOCATIONTYPE_FIXED && location->fixedclass)
		return location->fixedclass->backgroundImage;
	
	if(location->type == LOCATIONTYPE_PATH && location->pathclass)
	{
		if(location->pathclass->numberOfBackgroundImages > LOCATION_MAX_BACKGROUND_IMAGES)
			return -1;
		
		if(location->pathclass->numberOfBackgroundImages == 1)
		{
			return location->pathclass->backgroundImages[0];
		}
		else
		{
			index = Random(location->pathclass->numberOfBackgroundImages);
			return location->pathclass->backgroundImages[index];
		}
	}
	
	return -1;
}

int GetLocationMonsterIndex(Location *location)
{
	int index;
	
	if(location->type == LOCATIONTYPE_FIXED && location->fixedclass)
		return location->fixedclass->monster;
	
	if(location->type == LOCATIONTYPE_PATH && location->pathclass)
	{
		if(location->pathclass->numberOfMonsters > PATH_CLASS_MAX_MONSTERS)
			return -1;
		
		if(location->pathclass->numberOfMonsters == 1)
		{
			return location->pathclass->monsters[0];
		}
		else
		{
			int max = ((location->pathclass->numberOfMonsters - 1) * (location->baseLevel - 1))/(location->pathclass->monsterUnlockLevel - 1) + 1;
			index = Random(max);
			return location->pathclass->monsters[index];
		}
	}
	
	return -1;
}

int GetLocationEncounterChance(Location *location)
{
	if(location->type == LOCATIONTYPE_PATH && location->pathclass)
		return location->pathclass->encounterChance;
	
	return 0;
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
	
	if(index < 0)
		return NULL;
	
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

void RunArrivalFunction(Location *location)
{
	if(location && location->type == LOCATIONTYPE_FIXED && location->fixed_ArrivalFunction)
	{
		location->fixed_ArrivalFunction();
	}
}
