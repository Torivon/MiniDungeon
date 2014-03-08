#include "pebble.h"

#include "LocationInternal.h"
#include "Location.h"
#include "Logging.h"
#include "Utils.h"

FixedClass *GetLocationFixedClass(Location *location, bool dungeonFixed)
{
	switch(location->type)
	{
		case LOCATIONTYPE_FIXED:
			return location->fixedclass;
		case LOCATIONTYPE_PATH:
			return NULL;
		case LOCATIONTYPE_DUNGEON:
			if(!location->dungeonclass)
				return NULL;
			if(dungeonFixed)
				return location->dungeonclass->fixedclass;
			else
				return NULL;
		default:
			return NULL;
	}
}
	
PathClass *GetLocationPathClass(Location *location, bool dungeonFixed)
{
	switch(location->type)
	{
		case LOCATIONTYPE_FIXED:
			return NULL;
		case LOCATIONTYPE_PATH:
			return location->pathclass;
		case LOCATIONTYPE_DUNGEON:
			if(!location->dungeonclass)
				return NULL;
			if(dungeonFixed)
				return NULL;
			else
				return location->dungeonclass->pathclass;
		default:
			return NULL;
	}
}

int GetLocationBaseLevel(Location *location, int floor)
{
	if(location->type == LOCATIONTYPE_DUNGEON && location->dungeonclass && location->dungeonclass->levelIncreaseRate)
	{
		return location->baseLevel + floor / location->dungeonclass->levelIncreaseRate;
	}
	
	return location->baseLevel;
}

int GetLocationBackgroundImageId(Location *location, bool dungeonFixed)
{
	int index;
	FixedClass *fixedclass = NULL;
	PathClass *pathclass = NULL;
	
	if((fixedclass = GetLocationFixedClass(location, dungeonFixed)))
		return fixedclass->backgroundImage;
	
	if((pathclass = GetLocationPathClass(location, dungeonFixed)))
	{
		if(pathclass->numberOfBackgroundImages > LOCATION_MAX_BACKGROUND_IMAGES)
			return -1;
		
		if(pathclass->numberOfBackgroundImages == 1)
		{
			return pathclass->backgroundImages[0];
		}
		else
		{
			index = Random(pathclass->numberOfBackgroundImages);
			return pathclass->backgroundImages[index];
		}
	}
	
	return -1;
}

int GetLocationMonsterIndex(Location *location, bool dungeonFixed, uint floor)
{
	int index;
	FixedClass *fixedclass = NULL;
	PathClass *pathclass = NULL;
	
	if((fixedclass = GetLocationFixedClass(location, dungeonFixed)))
		return location->fixedclass->monster;
	
	if((pathclass = GetLocationPathClass(location, dungeonFixed)))
	{
		if(pathclass->numberOfMonsters > PATH_CLASS_MAX_MONSTERS)
			return -1;
		
		if(pathclass->numberOfMonsters == 1)
		{
			return pathclass->monsters[0];
		}
		else
		{
			int max = ((pathclass->numberOfMonsters - 1) * (GetLocationBaseLevel(location, floor) - 1))/(pathclass->monsterUnlockLevel - 1) + 1;
			if(max > pathclass->numberOfMonsters)
				max = pathclass->numberOfMonsters;
			index = Random(max);
			return pathclass->monsters[index];
		}
	}
	
	return -1;
}

int GetLocationEncounterChance(Location *location, bool dungeonFixed)
{
	PathClass *pathclass = NULL;
	if((pathclass = GetLocationPathClass(location, dungeonFixed)))
		return pathclass->encounterChance;
	
	return 0;
}

int GetLocationMonsterCount(Location *location, bool dungeonFixed)
{	
	PathClass *pathclass = NULL;
	FixedClass *fixedclass = NULL;
	if((pathclass = GetLocationPathClass(location, dungeonFixed)))
		return pathclass->numberOfMonsters;
	
	if((fixedclass = GetLocationFixedClass(location, dungeonFixed)))
		return fixedclass->monster ? 1 : 0;
	
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

uint8_t GetLocationNumberOfFloors(Location *location)
{
	if(location->type != LOCATIONTYPE_DUNGEON)
		return 0;
	
	return location->dungeonclass->numberOfFloors;
}

bool LocationAllowsShop(Location *location, bool dungeonFixed)
{
	FixedClass *fixedclass = GetLocationFixedClass(location, dungeonFixed);
	
	if(!fixedclass)
		return false;
	
	return fixedclass->allowShop;
}