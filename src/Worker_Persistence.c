#include <pebble_worker.h>

#include "../src/Persistence.h"
#include "Worker_Persistence.h"

#if ALLOW_WORKER_APP
static bool closedInBattle = false;
static bool workerCanLaunch = false;

bool GetClosedInBattle(void)
{
	return closedInBattle;
}

void SetClosedInBattle(bool enable)
{
	closedInBattle = enable;
}

bool GetWorkerCanLaunch(void)
{
	return workerCanLaunch;
}

void SetWorkerCanLaunch(bool enable)
{
	workerCanLaunch = enable;
}

bool LoadWorkerData(void)
{
	if(!persist_exists(PERSISTED_IS_DATA_SAVED) || !persist_read_bool(PERSISTED_IS_DATA_SAVED))
		return false;
		
	if(!IsPersistedDataCurrent())
	{
		return false;
	}

	SetWorkerCanLaunch(persist_read_bool(PERSISTED_WORKER_CAN_LAUNCH));

	SetClosedInBattle(persist_read_bool(PERSISTED_IN_COMBAT));
	return true;
}
#endif
