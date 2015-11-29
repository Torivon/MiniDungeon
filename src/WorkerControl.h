#pragma once

AppWorkerResult LaunchWorkerApp();
AppWorkerResult KillWorkerApp();
bool WorkerIsHandlingUpdates(void);
bool WorkerIsRunning(void);
void WorkerMessageHandler(uint16_t type, AppWorkerMessage *data);
void ActivateWorkerAppLaunchCheck(void);
void WorkerAppLaunchCheck(void);
void AppDying(bool closingWhileInBattle);
void AppAwake(void);
void SetWorkerReady(bool ready);

// Worker message types
enum
{
	WORKER_HANDLE_UPDATES = 0,
	WORKER_LAUNCHED,
	WORKER_DYING,
	WORKER_READY,
	TRIGGER_EVENT,
	WORKER_ACKNOWLEDGE_BASE_CHANCE,
	WORKER_ACKNOWLEDGE_EVENT_CHANCE,
	WORKER_PREVIOUS_STATE,

	APP_DYING,
	APP_AWAKE,
	APP_SEND_BASE_EVENT_CHANCE,
	APP_SEND_EVENT_CHANCE,
	APP_SEND_EVENT_END
};