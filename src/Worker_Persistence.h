#pragma once
#include "MiniDungeon.h"

#ifdef BUILD_WORKER_FILES
#include <pebble_worker.h>

bool LoadWorkerData(void);
bool GetWorkerCanLaunch(void);
void SetWorkerCanLaunch(bool enable);
bool GetClosedInBattle(void);
void SetClosedInBattle(bool enable);
#endif
