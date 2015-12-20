#include "pebble.h"

#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "Monsters.h"
#include "OptionsMenu.h"
#include "Persistence.h"
#include "Shop.h"
#include "WorkerControl.h"

void ClearPersistedData(void)
{
	if(persist_exists(PERSISTED_IS_DATA_SAVED))
	{
		int maxKey = persist_read_int(PERSISTED_MAX_KEY_USED);
		int i;
		for(i = 0; i <= maxKey; ++i)
		{
			persist_delete(i);
		}
	}
}

bool SavePersistedData(void)
{
	CharacterData *characterData;

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

	if(GetSizeOfItemsOwned() > PERSIST_DATA_MAX_LENGTH )
	{
		ERROR_LOG("Item data is too big to save (%d).", GetSizeOfItemsOwned());
		return false;
	}

	ProfileLogStart("SavePersistedData");
	INFO_LOG("Saving persisted data.");
	DEBUG_VERBOSE_LOG("Saving meta data");
	persist_write_bool(PERSISTED_IS_DATA_SAVED, true);
	persist_write_int(PERSISTED_CURRENT_DATA_VERSION, CURRENT_DATA_VERSION);
	persist_write_int(PERSISTED_MAX_KEY_USED, MAX_PERSISTED_KEY);
	
	DEBUG_VERBOSE_LOG("Saving character");
	characterData = GetCharacter();
	persist_write_data(PERSISTED_CHARACTER_DATA, characterData, sizeof(CharacterData));
	
	DEBUG_VERBOSE_LOG("Saving floor");
	persist_write_int(PERSISTED_CURRENT_FLOOR, GetCurrentFloor());
	
	DEBUG_VERBOSE_LOG("Saving item data");
	persist_write_data(PERSISTED_ITEM_DATA, GetItemsOwned(), GetSizeOfItemsOwned());
	
	DEBUG_VERBOSE_LOG("Saving stat points");
	persist_write_int(PERSISTED_STAT_POINTS_PURCHASED, GetStatPointsPurchased());

	DEBUG_VERBOSE_LOG("Saving option data");
	persist_write_bool(PERSISTED_VIBRATION, GetVibration());
	persist_write_bool(PERSISTED_FAST_MODE, GetFastMode());
	persist_write_bool(PERSISTED_WORKER_APP, GetWorkerApp());
	persist_write_bool(PERSISTED_WORKER_CAN_LAUNCH, GetWorkerCanLaunch());
	persist_write_bool(PERSISTED_USE_OLD_ASSETS, GetUseOldAssets());

	DEBUG_VERBOSE_LOG("Saving combat data");
	persist_write_bool(PERSISTED_IN_COMBAT, ClosingWhileInBattle());
	persist_write_int(PERSISTED_MONSTER_TYPE, GetMostRecentMonster());
	persist_write_int(PERSISTED_MONSTER_HEALTH, GetCurrentMonsterHealth());
	INFO_LOG("Done saving persisted data.");
	ProfileLogStop("SavePersistedData");
	
	return true;
}

bool LoadPersistedData(void)
{
	CharacterData *characterData;
	int floor = 0;
	bool useWorkerApp = false;
	if(!persist_exists(PERSISTED_IS_DATA_SAVED) || !persist_read_bool(PERSISTED_IS_DATA_SAVED))
		return false;
		
	if(!IsPersistedDataCurrent())
	{
		WARNING_LOG("Persisted data does not match current version, clearing.");
		ClearPersistedData();
		return false;
	}

	ProfileLogStart("LoadPersistedData");
	INFO_LOG("Loading persisted data.");
	characterData = GetCharacter();
	persist_read_data(PERSISTED_CHARACTER_DATA, characterData, sizeof(CharacterData));
	floor = persist_read_int(PERSISTED_CURRENT_FLOOR);
	SetCurrentFloor(floor);
	persist_read_data(PERSISTED_ITEM_DATA, GetItemsOwned(), GetSizeOfItemsOwned());
	SetStatPointsPurchased(persist_read_int(PERSISTED_STAT_POINTS_PURCHASED));
	SetVibration(persist_read_bool(PERSISTED_VIBRATION));
	SetFastMode(persist_read_bool(PERSISTED_FAST_MODE));
	if(persist_exists(PERSISTED_USE_OLD_ASSETS))
		SetUseOldAssets(persist_read_bool(PERSISTED_USE_OLD_ASSETS));
	useWorkerApp = persist_read_bool(PERSISTED_WORKER_APP);
	if(useWorkerApp)
	{
		AttemptToLaunchWorkerApp();
	}
	else
	{
		// If the user has launched the worker app outside of MiniDungeon,
		// they want it on.
		if(WorkerIsRunning())
			SetWorkerApp(true);
	}
	SetWorkerCanLaunch(persist_read_bool(PERSISTED_WORKER_CAN_LAUNCH));

	if(persist_read_bool(PERSISTED_IN_COMBAT))
	{
		int currentMonster = persist_read_int(PERSISTED_MONSTER_TYPE);
		int currentMonsterHealth = persist_read_int(PERSISTED_MONSTER_HEALTH);
		ResumeBattle(currentMonster, currentMonsterHealth);
	}
	ProfileLogStop("LoadPersistedData");
	if(characterData->level == 0)
	{
		// Something bad happened to the data, possible due to a watch crash
		ERROR_LOG("Persisted data was broken somehow, clearing");
		ClearPersistedData();
		return false;
	}
	return true;
}