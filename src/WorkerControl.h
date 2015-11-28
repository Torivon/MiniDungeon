#pragma once

AppWorkerResult LaunchWorkerApp();
AppWorkerResult KillWorkerApp();
bool WorkerIsHandlingUpdates(void);
bool WorkerIsRunning(void);
void WorkerMessageHandler(uint16_t type, AppWorkerMessage *data);
void ActivateWorkerAppLaunchCheck(void);
void WorkerAppLaunchCheck(void);
void AppDying(void);
void AppAwake(void);

void UnpauseWorkerApp(void);
void PauseWorkerApp(void);

// Worker message types
enum
{
	WORKER_HANDLE_UPDATES = 0,
	WORKER_SEND_TICK_COUNT,
	WORKER_LAUNCHED,
	WORKER_DYING,
	TRIGGER_EVENT,
	PAUSE_WORKER_APP,
	UNPAUSE_WORKER_APP,
	APP_SEND_TICK_COUNT,
	APP_DYING,
	APP_AWAKE,
};