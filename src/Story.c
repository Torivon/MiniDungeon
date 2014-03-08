#include "pebble.h"

#include "Utils.h"
#include "Location.h"
#include "Logging.h"
#include "Monsters.h"
#include "Story.h"
	
static const Story *currentStory = NULL;
static StoryState *currentStoryState = NULL;

void RegisterStory(const Story *story, StoryState *storyState)
{
	if(story)
	{
		currentStory = story;
		currentStoryState = storyState;
	}
}

void ClearCurrentStory(void)
{
	currentStory = NULL;
	currentStoryState = NULL;
}

const Story *GetCurrentStory(void)
{
	return currentStory;
}

static int GetNumberOfMonsters(MonsterDef *monsterList)
{
	return currentStory->numberOfMonsters;
}

MonsterDef *GetMonsterByIndex(int index)
{
	if(!currentStory)
		return NULL;
	
	if(index < 0 || index > GetNumberOfMonsters(currentStory->monsterList))
		return NULL;

	currentStoryState->persistedStoryState.mostRecentMonster = index;
	return &currentStory->monsterList[index];
}

Location *GetCurrentLocation(void)
{
	if(currentStoryState->persistedStoryState.currentLocationIndex < 0 || currentStoryState->persistedStoryState.currentLocationIndex > currentStory->numberOfLocations)
	{
		DEBUG_LOG("Current location index out of bounds (%d/%d).", currentStoryState->persistedStoryState.currentLocationIndex, currentStory->numberOfLocations);
		return NULL;
	}

	return GetLocationByIndex(currentStory->locationList, currentStoryState->persistedStoryState.currentLocationIndex);
}

int GetCurrentBackgroundImage(void)
{
	return GetLocationBackgroundImageId(GetCurrentLocation(), currentStoryState->persistedStoryState.dungeonFixed);
}

MonsterDef *GetRandomMonster(void)
{
	return GetMonsterByIndex(GetLocationMonsterIndex(GetCurrentLocation(), currentStoryState->persistedStoryState.dungeonFixed, currentStoryState->persistedStoryState.dungeonFloor));
}

MonsterDef *GetFixedMonster(int index)
{
	return GetMonsterByIndex(index);
}

int GetCurrentLocationEncounterChance(void)
{
	return GetLocationEncounterChance(GetCurrentLocation(), currentStoryState->persistedStoryState.dungeonFixed);
}

int GetCurrentBaseLevel(void)
{
	return GetLocationBaseLevel(GetCurrentLocation(), currentStoryState->persistedStoryState.dungeonFloor);
}

const char *GetCurrentLocationName(void)
{
	return GetLocationName(GetCurrentLocation());
}

const char *GetCurrentLocationFloor(void)
{
	if(IsCurrentLocationDungeon())
	{
		static char floorText[] = "00"; // Needs to be static because it's used by the system later.
		IntToString(floorText, 2, currentStoryState->persistedStoryState.dungeonFloor);
		return floorText;
	}
	
	return "";
}

void IncrementCurrentDuration(void)
{
	++currentStoryState->persistedStoryState.currentLocationDuration;
}

void CurrentStoryStateNeedsSaving(void)
{
	currentStoryState->needsSaving = true;
}

int GetCurrentDuration(void)
{
	return currentStoryState->persistedStoryState.currentLocationDuration;
}

int GetCurrentLocationLength(void)
{
	return GetLocationLength(GetCurrentLocation());
}

const char *GetAdjacentLocationName(uint16_t index)
{
	Location *adjacentLocation;
	int realIndex = GetCurrentAdjacentLocationIndex(index);
	
	if(IsCurrentLocationDungeon())
	{
		if(!currentStoryState->persistedStoryState.dungeonFixed)
			return NULL;
		
		if(index == 0)
			return "Previous";
		else if(index == 1)
			return "Next";
		else
			return NULL;
	}
	
	if(realIndex == -1)
		return NULL;
	
	adjacentLocation = GetLocationByIndex(currentStory->locationList, realIndex);
	
	if(!adjacentLocation)
		return NULL;
	
	return GetLocationName(adjacentLocation);
}

bool IsCurrentLocationPath(void)
{
	return GetLocationType(GetCurrentLocation()) == LOCATIONTYPE_PATH;
}

bool IsCurrentLocationDungeon(void)
{
	return GetLocationType(GetCurrentLocation()) == LOCATIONTYPE_DUNGEON;
}

bool IsCurrentLocationFixed(void)
{
	return GetLocationType(GetCurrentLocation()) == LOCATIONTYPE_FIXED;
}

bool CurrentLocationAllowsShop(void)
{
	return LocationAllowsShop(GetCurrentLocation(), currentStoryState->persistedStoryState.dungeonFixed);
}

bool CurrentLocationAllowsCombat(void)
{
	Location *location = GetCurrentLocation();
	if(!location)
		return false;
	
	if(!GetLocationEncounterChance(location, currentStoryState->persistedStoryState.dungeonFixed))
		return false;
	
	if(!GetLocationMonsterCount(location, currentStoryState->persistedStoryState.dungeonFixed))
		return false;
	
	return true;
}

void SetNewLocation(int index)
{
	int lastIndex = currentStoryState->persistedStoryState.currentLocationIndex;
	Location *newLocation = GetLocationByIndex(currentStory->locationList, index);

	DEBUG_LOG("Trying to find new location with index %d.", index);

	if(!newLocation)
	{
		DEBUG_LOG("Failed to find new location, aborting.");
		return;
	}
	
	DEBUG_LOG("Setting new location index %d.", index);
	currentStoryState->needsSaving = true;
	currentStoryState->persistedStoryState.currentLocationIndex = index;
	currentStoryState->persistedStoryState.currentLocationDuration = 0;
	if(GetLocationType(newLocation) == LOCATIONTYPE_PATH)
	{
		currentStoryState->persistedStoryState.currentPathDestination = GetDestinationOfPath(newLocation, lastIndex);
	}
	if(GetLocationType(newLocation) == LOCATIONTYPE_DUNGEON)
	{
		currentStoryState->persistedStoryState.currentPathDestination = GetDestinationOfPath(newLocation, lastIndex);
		if(GetAdjacentLocationIndex(newLocation, 0) == currentStoryState->persistedStoryState.currentPathDestination)
			currentStoryState->persistedStoryState.dungeonFloor = GetLocationNumberOfFloors(newLocation);
		else
			currentStoryState->persistedStoryState.dungeonFloor = 1;
		currentStoryState->persistedStoryState.dungeonFixed = false;
	}
	RunArrivalFunction(newLocation);
}

int GetCurrentDestinationIndex(void)
{
	return currentStoryState->persistedStoryState.currentPathDestination;
}

int GetCurrentAdjacentLocationIndex(int index)
{
	Location *currentLocation = GetCurrentLocation();
	if(!currentLocation)
		return -1;
	
	return GetAdjacentLocationIndex(currentLocation, index);
}

void InitializeCurrentStory(void)
{
	if(currentStory && currentStory->initializeStory)
	{
		currentStory->initializeStory();
	}
}

StoryState *GetCurrentStoryState(void)
{
	return currentStoryState;
}

static void EndPath(void)
{
	SetNewLocation(GetCurrentDestinationIndex());
}

static LocationUpdateReturnType UpdatePath(void)
{
	IncrementCurrentDuration();
	DEBUG_LOG("Time in current location: %d/%d.", GetCurrentDuration(), GetCurrentLocationLength());
	if(GetCurrentDuration() >= GetCurrentLocationLength())
	{
		EndPath();
		return LOCATIONUPDATE_FULLREFRESH;
	}
	
	return LOCATIONUPDATE_COMPUTERANDOM;
}

static void EndDungeonPath(void)
{
	Location *location = GetCurrentLocation();
	bool forward;
	if(currentStoryState->persistedStoryState.currentPathDestination == GetCurrentAdjacentLocationIndex(0))
		forward = false;
	else
		forward = true;
	
	if(forward && currentStoryState->persistedStoryState.dungeonFloor == GetLocationNumberOfFloors(location))
	{
		SetNewLocation(GetCurrentDestinationIndex());
	}
	else if(!forward && currentStoryState->persistedStoryState.dungeonFloor == 1)
	{
		SetNewLocation(GetCurrentDestinationIndex());
	}
	else
	{
		currentStoryState->persistedStoryState.dungeonFixed = true;
		currentStoryState->persistedStoryState.currentLocationDuration = 0;
		if(!forward)
			--currentStoryState->persistedStoryState.dungeonFloor;
	}
}

static LocationUpdateReturnType UpdateDungeon(void)
{
	if(currentStoryState->persistedStoryState.dungeonFixed)
	{
		return LOCATIONUPDATE_DONOTHING;
	}
	else
	{
		IncrementCurrentDuration();
		DEBUG_LOG("Time in current location: %d/%d.", GetCurrentDuration(), GetCurrentLocationLength());
		if(GetCurrentDuration() >= GetCurrentLocationLength())
		{
			EndDungeonPath();
			return LOCATIONUPDATE_FULLREFRESH;
		}
		
		return LOCATIONUPDATE_COMPUTERANDOM;
	}
}

LocationUpdateReturnType UpdateCurrentLocation(void)
{
	Location *location = GetCurrentLocation();
	if(!location)
		return LOCATIONUPDATE_DONOTHING;
	
	switch(GetLocationType(location))
	{
		case LOCATIONTYPE_PATH:
			return UpdatePath();
			break;
		case LOCATIONTYPE_FIXED:
			return LOCATIONUPDATE_DONOTHING;
			break;
		case LOCATIONTYPE_DUNGEON:
			return UpdateDungeon();
			break;
		default:
			return LOCATIONUPDATE_DONOTHING;
	}
}

void TravelToAdjacentLocationByIndex(int index)
{
	if(IsCurrentLocationFixed())
	{
		SetNewLocation(GetCurrentAdjacentLocationIndex(index));
	}
	else if(IsCurrentLocationDungeon() && currentStoryState->persistedStoryState.dungeonFixed)
	{
		if(index != 0 && index != 1)
			return;
		
		if(index == 1)
			++currentStoryState->persistedStoryState.dungeonFloor;
		
		currentStoryState->persistedStoryState.dungeonFixed = false;
		currentStoryState->persistedStoryState.currentPathDestination = GetCurrentAdjacentLocationIndex(index);
		currentStoryState->persistedStoryState.currentLocationDuration = 0;
	}
}