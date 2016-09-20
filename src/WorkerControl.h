#pragma once

AppWorkerResult LaunchWorkerApp();
AppWorkerResult KillWorkerApp();
void AttemptToKillWorkerApp();
void AttemptToLaunchWorkerApp();

bool WorkerIsRunning(void);
void WorkerMessageHandler(uint16_t type, AppWorkerMessage *data);
void AppDying(bool closingWhileInBattle);
void AppAwake(void);
void SendWorkerCanLaunch(void);

// This needs to be kept in sync with "../worker_src/Worker.h"
// Worker message types
enum
{
	WORKER_LAUNCHED = 0,
	WORKER_DYING,
	TRIGGER_EVENT,
	WORKER_SEND_STATE1,
	WORKER_SEND_STATE2,
	WORKER_SEND_ERROR,

	APP_DYING,
	APP_AWAKE,
	APP_SEND_WORKER_CAN_LAUNCH
};