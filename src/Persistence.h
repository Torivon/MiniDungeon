#pragma once

#define CURRENT_DATA_VERSION 8

#define NUMBER_OF_PERSISTED_GAMES 2

enum
{
	PERSISTED_IS_DATA_SAVED = 0,
	PERSISTED_CURRENT_DATA_VERSION,
	PERSISTED_MAX_KEY_USED,
	PERSISTED_VIBRATION,
	PERSISTED_WORKER_APP,
	PERSISTED_WORKER_CAN_LAUNCH,
	PERSISTED_CURRENT_GAME,
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

inline bool IsPersistedDataCurrent(void)
{
	bool dataSaved = persist_read_bool(PERSISTED_IS_DATA_SAVED);
	int savedVersion;
	if(!dataSaved)
		return true;
	
	savedVersion = persist_read_int(PERSISTED_CURRENT_DATA_VERSION);
	
	return savedVersion == CURRENT_DATA_VERSION;
}

inline int ComputeGamePersistedDataOffset(int storyNumber)
{
	return PERSISTED_GLOBAL_DATA_COUNT + storyNumber * PERSISTED_GAME_DATA_COUNT;
}
	
inline bool IsPersistedGameDataCurrent(int storyNumber, int storyDataVersion)
{
	int offset = ComputeGamePersistedDataOffset(storyNumber);
	bool dataSaved = persist_read_bool(PERSISTED_GAME_IS_DATA_SAVED + offset);
	int savedVersion;
	if(!dataSaved)
		return true;
	
	savedVersion = persist_read_int(PERSISTED_GAME_CURRENT_DATA_VERSION + offset);
	
	return savedVersion == storyDataVersion;
}

bool SavePersistedData(void);
bool LoadPersistedData(void);
