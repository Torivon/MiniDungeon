#include "pebble.h"
	
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
		currentStoryState->currentLocationIndex = 0;
		currentStoryState->currentLocationDuration = 0;
	}
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

	return &currentStory->monsterList[index];
}

Location *GetCurrentLocation(void)
{
	if(currentStoryState->currentLocationIndex < 0 || currentStoryState->currentLocationIndex > currentStory->numberOfLocations)
	{
		DEBUG_LOG("Current location index out of bounds (%d/%d).", currentStoryState->currentLocationIndex, currentStory->numberOfLocations);
		return NULL;
	}

	return GetLocationByIndex(currentStory->locationList, currentStoryState->currentLocationIndex);
}

int GetCurrentBackgroundImage(void)
{
	return GetLocationBackgroundImageId(GetCurrentLocation());
}

MonsterDef *GetRandomMonster(void)
{
	return GetMonsterByIndex(GetLocationMonsterIndex(GetCurrentLocation()));
}

MonsterDef *GetFixedMonster(int index)
{
	return GetMonsterByIndex(index);
}

int GetCurrentLocationEncounterChance(void)
{
	return GetLocationEncounterChance(GetCurrentLocation());
}

int GetCurrentBaseLevel(void)
{
	return GetLocationBaseLevel(GetCurrentLocation());
}

const char *GetCurrentLocationName(void)
{
	return GetLocationName(GetCurrentLocation());
}

void IncrementCurrentDuration(void)
{
	++currentStoryState->currentLocationDuration;
}

int GetCurrentDuration(void)
{
	return currentStoryState->currentLocationDuration;
}

int GetCurrentLocationLength(void)
{
	return GetLocationLength(GetCurrentLocation());
}

bool IsCurrentLocationPath(void)
{
	return GetLocationType(GetCurrentLocation()) == LOCATIONTYPE_PATH;
}

void SetNewLocation(int index)
{
	int lastIndex = currentStoryState->currentLocationIndex;
	Location *newLocation = GetLocationByIndex(currentStory->locationList, index);

	DEBUG_LOG("Trying to find new location with index %d.", index);

	if(!newLocation)
	{
		DEBUG_LOG("Failed to find new location, aborting.");
		return;
	}
	
	DEBUG_LOG("Setting new location index %d.", index);
	currentStoryState->needsSaving = true;
	currentStoryState->currentLocationIndex = index;
	currentStoryState->currentLocationDuration = 0;
	if(GetLocationType(newLocation) == LOCATIONTYPE_PATH)
	{
		currentStoryState->currentPathDestination = GetDestinationOfPath(newLocation, lastIndex);
	}
}

int GetCurrentDestinationIndex(void)
{
	return currentStoryState->currentPathDestination;
}

int GetCurrentAdjacentLocationIndex(int index)
{
	Location *currentLocation = GetCurrentLocation();
	if(!currentLocation)
		return -1;
	
	return GetAdjacentLocationIndex(currentLocation, index);
}
