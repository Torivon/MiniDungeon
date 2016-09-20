#pragma once

#include <pebble_worker.h>

#define CURRENT_DATA_VERSION 4
enum
{
    PERSISTED_IS_DATA_SAVED = 0,
    PERSISTED_CURRENT_DATA_VERSION,
    PERSISTED_MAX_KEY_USED,
    PERSISTED_CHARACTER_DATA,
    PERSISTED_CURRENT_FLOOR,
    PERSISTED_ITEM_DATA,
    PERSISTED_STAT_POINTS_PURCHASED,
    PERSISTED_VIBRATION,
    PERSISTED_FAST_MODE,
    PERSISTED_WORKER_APP,
    PERSISTED_WORKER_CAN_LAUNCH,
    
    PERSISTED_IN_COMBAT,
    PERSISTED_MONSTER_TYPE,
    PERSISTED_MONSTER_HEALTH,
    
    PERSISTED_USE_OLD_ASSETS,
    
    // This needs to always be last
    PERSISTED_DATA_COUNT
};

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

bool LoadWorkerData(void);
bool GetWorkerCanLaunch(void);
void SetWorkerCanLaunch(bool enable);
bool GetClosedInBattle(void);
void SetClosedInBattle(bool enable);
