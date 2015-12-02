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

bool WorkerIsReady(void);

// Worker message types
enum
{
	WORKER_LAUNCHED = 0,
	WORKER_DYING,
	WORKER_READY,
	TRIGGER_EVENT,
	WORKER_ACKNOWLEDGE_BASE_EVENT_CHANCE,
	WORKER_ACKNOWLEDGE_EVENT_CHANCE,
	WORKER_ACKNOWLEDGE_EVENT_END,

	APP_DYING,
	APP_AWAKE,
	APP_SEND_BASE_EVENT_CHANCE,
	APP_SEND_EVENT_CHANCE,
	APP_SEND_EVENT_END,
	APP_SEND_WORKER_CAN_LAUNCH
};