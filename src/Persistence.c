#include "pebble.h"

#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Monsters.h"
#include "Persistence.h"
#include "Shop.h"
#include "Story.h"
	
#define CURRENT_DATA_VERSION 3

#define NUMBER_OF_PERSISTED_GAMES 2

enum
{
	PERSISTED_IS_DATA_SAVED = 0,
	PERSISTED_CURRENT_DATA_VERSION,
	PERSISTED_MAX_KEY_USED,
	PERSISTED_VIBRATION,
	PERSISTED_GLOBAL_DATA_COUNT,
};

enum
{
	PERSISTED_GAME_IS_DATA_SAVED = 0,
	PERSISTED_GAME_CURRENT_DATA_VERSION,
	PERSISTED_GAME_CHARACTER_DATA,
	PERSISTED_GAME_STORY_DATA,
	PERSISTED_GAME_ITEM_DATA,
	PERSISTED_GAME_STAT_POINTS_PURCHASED,
	
	PERSISTED_GAME_IN_COMBAT,
	PERSISTED_GAME_MONSTER_TYPE,
	PERSISTED_GAME_MONSTER_HEALTH,
	
	// This needs to always be last
	PERSISTED_GAME_DATA_COUNT
};

#define PERSISTED_DATA_COUNT PERSISTED_GLOBAL_DATA_COUNT + NUMBER_OF_PERSISTED_GAMES * PERSISTED_GAME_DATA_COUNT
#define MAX_PERSISTED_KEY PERSISTED_DATA_COUNT - 1

int ComputeGamePersistedDataOffset(int storyNumber)
{
	return PERSISTED_GLOBAL_DATA_COUNT + storyNumber * PERSISTED_GAME_DATA_COUNT;
}
	
bool IsPersistedDataCurrent(void)
{
	bool dataSaved = persist_read_bool(PERSISTED_IS_DATA_SAVED);
	int savedVersion;
	if(!dataSaved)
		return true;
	
	savedVersion = persist_read_int(PERSISTED_CURRENT_DATA_VERSION);
	
	return savedVersion == CURRENT_DATA_VERSION;
}
	
bool IsPersistedGameDataCurrent(int storyNumber, int storyDataVersion)
{
	int offset = ComputeGamePersistedDataOffset(storyNumber);
	bool dataSaved = persist_read_bool(PERSISTED_GAME_IS_DATA_SAVED + offset);
	int savedVersion;
	if(!dataSaved)
		return true;
	
	savedVersion = persist_read_int(PERSISTED_GAME_CURRENT_DATA_VERSION + offset);
	
	return savedVersion == storyDataVersion;
}

void ClearPersistedData(void)
{
	if(persist_exists(PERSISTED_IS_DATA_SAVED))
	{
		DEBUG_LOG("Clearing persisted data.");
		int maxKey = persist_read_int(PERSISTED_MAX_KEY_USED);
		int i;
		for(i = 0; i <= maxKey; ++i)
		{
			persist_delete(i);
		}
	}
}
	
void ClearPersistedGameData(int gameNumber)
{
	int offset = ComputeGamePersistedDataOffset(gameNumber);
	if(persist_exists(PERSISTED_GAME_IS_DATA_SAVED + offset))
	{
		DEBUG_LOG("Clearing game persisted data.");
		int i;
		for(i = 0; i < PERSISTED_GAME_DATA_COUNT; ++i)
		{
			persist_delete(i + offset);
		}
	}
}
	
bool SavePersistedData(void)
{
	CharacterData *characterData;
	StoryState *currentStoryState;
	const Story *currentStory;
	
	if(!IsPersistedDataCurrent())
	{
		WARNING_LOG("Persisted data does not match current version, clearing.");
		ClearPersistedData();
	}
	
	if(sizeof(CharacterData) > PERSIST_DATA_MAX_LENGTH )
	{
		ERROR_LOG("CharacterData is too big to save (%d).", sizeof(CharacterData));
		return false;
	}

	if(sizeof(PersistedStoryState) > PERSIST_DATA_MAX_LENGTH )
	{
		ERROR_LOG("PersistedStoryState is too big to save (%d).", sizeof(PersistedStoryState));
		return false;
	}

	if(GetSizeOfItemsOwned() > PERSIST_DATA_MAX_LENGTH )
	{
		ERROR_LOG("Item data is too big to save (%d).", GetSizeOfItemsOwned());
		return false;
	}

	INFO_LOG("Saving global persisted data.");
	persist_write_bool(PERSISTED_IS_DATA_SAVED, true);
	persist_write_int(PERSISTED_CURRENT_DATA_VERSION, CURRENT_DATA_VERSION);
	persist_write_int(PERSISTED_MAX_KEY_USED, MAX_PERSISTED_KEY);
	persist_write_bool(PERSISTED_VIBRATION, GetVibration());
	
	currentStoryState = GetCurrentStoryState();
	currentStory = GetCurrentStory();
	if(currentStoryState && currentStory && currentStoryState->needsSaving)
	{
		int offset = ComputeGamePersistedDataOffset(currentStory->gameNumber);
		
		if(!IsPersistedGameDataCurrent(currentStory->gameNumber, currentStory->gameDataVersion))
		{
			WARNING_LOG("Game's persisted data does not match current version, clearing.");
			ClearPersistedGameData(currentStory->gameNumber);
		}
		
		INFO_LOG("Saving story persisted data.");
		
		persist_write_bool(PERSISTED_GAME_IS_DATA_SAVED + offset, true);
		persist_write_int(PERSISTED_GAME_CURRENT_DATA_VERSION + offset, currentStory->gameDataVersion);

		characterData = GetCharacter();
		persist_write_data(PERSISTED_GAME_CHARACTER_DATA + offset, characterData, sizeof(CharacterData));
		persist_write_data(PERSISTED_GAME_STORY_DATA + offset, &currentStoryState->persistedStoryState, sizeof(PersistedStoryState));
		persist_write_data(PERSISTED_GAME_ITEM_DATA + offset, GetItemsOwned(), GetSizeOfItemsOwned());
		persist_write_int(PERSISTED_GAME_STAT_POINTS_PURCHASED + offset, GetStatPointsPurchased());

		persist_write_bool(PERSISTED_GAME_IN_COMBAT + offset, ClosingWhileInBattle());
		persist_write_int(PERSISTED_GAME_MONSTER_TYPE + offset, currentStoryState->persistedStoryState.mostRecentMonster);
		persist_write_int(PERSISTED_GAME_MONSTER_HEALTH + offset, GetCurrentMonsterHealth());
		currentStoryState->needsSaving = false;
	}
	
	return true;
}

bool LoadPersistedData(void)
{
	CharacterData *characterData;
	StoryState *currentStoryState;
	const Story *currentStory;
	if(!persist_exists(PERSISTED_IS_DATA_SAVED) || !persist_read_bool(PERSISTED_IS_DATA_SAVED))
	{
		INFO_LOG("No saved data to load.");
		return false;
	}
		
	if(!IsPersistedDataCurrent())
	{
		WARNING_LOG("Persisted data does not match current version, clearing.");
		ClearPersistedData();
		return false;
	}
	INFO_LOG("Loading global persisted data.");
	SetVibration(persist_read_bool(PERSISTED_VIBRATION));

	currentStoryState = GetCurrentStoryState();
	currentStory = GetCurrentStory();
	
	if(currentStoryState && currentStory)
	{
		int offset = ComputeGamePersistedDataOffset(currentStory->gameNumber);
		INFO_LOG("Loading persisted data.");
		characterData = GetCharacter();
		persist_read_data(PERSISTED_GAME_CHARACTER_DATA + offset, characterData, sizeof(CharacterData));
		persist_read_data(PERSISTED_GAME_STORY_DATA + offset, &currentStoryState->persistedStoryState, sizeof(PersistedStoryState));
		persist_read_data(PERSISTED_GAME_ITEM_DATA + offset, GetItemsOwned(), GetSizeOfItemsOwned());
		SetStatPointsPurchased(persist_read_int(PERSISTED_GAME_STAT_POINTS_PURCHASED + offset));
		
		if(persist_read_bool(PERSISTED_GAME_IN_COMBAT + offset))
		{
			int currentMonster = persist_read_int(PERSISTED_GAME_MONSTER_TYPE + offset);
			int currentMonsterHealth = persist_read_int(PERSISTED_GAME_MONSTER_HEALTH + offset);
			INFO_LOG("Resuming battle in progress.");
			ResumeBattle(currentMonster, currentMonsterHealth);
		}
		if(characterData->level == 0)
		{
			// Something bad happened to the data, possible due to a watch crash
			ERROR_LOG("Persisted data was broken somehow, clearing");
			ClearPersistedGameData(currentStory->gameNumber);
			return false;
		}
	}
	
	return true;
}