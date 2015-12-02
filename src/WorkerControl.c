#include <pebble.h>
#include "Adventure.h"
#include "Logging.h"
#include "MainMenu.h"
#include "OptionsMenu.h"
#include "WorkerControl.h"

static bool workerReady = false;

void SendMessageToWorker(uint8_t type, uint16_t data0, uint16_t data1, uint16_t data2)
{
	AppWorkerMessage msg_data = {
		.data0 = data0,
		.data1 = data1,
		.data2 = data2
	};
	app_worker_send_message(type, &msg_data);
}

void AppDying(bool closingWhileInBattle)
{
	DEBUG_LOG("AppDying(%s)", closingWhileInBattle ? "true" : "false");
	if(WorkerIsRunning())
		SendMessageToWorker(APP_DYING, GetTickCount(), closingWhileInBattle, 0);
}

void AppAwake(void)
{
	workerReady = false;
	if(WorkerIsRunning())
		SendMessageToWorker(APP_AWAKE, 0, 0, 0);
}

AppWorkerResult LaunchWorkerApp()
{
	DEBUG_LOG("Launching worker app");
	return app_worker_launch();
}

AppWorkerResult KillWorkerApp()
{
	bool running = WorkerIsRunning();
	
	if(!running)
		return false;
	
	return app_worker_kill();
}

void AttemptToLaunchWorkerApp()
{
#if ALLOW_WORKER_APP
	if(WorkerIsRunning())
	{
		DEBUG_LOG("WorkerApp already running");
		SetWorkerApp(true);
	}
	else
	{
		LaunchWorkerApp();
		SetWorkerApp(false);
	}
#endif
}

void AttemptToKillWorkerApp()
{
#if ALLOW_WORKER_APP
	if(WorkerIsRunning())
	{
		KillWorkerApp();
	}
	else
	{
		SetWorkerApp(false);
	}
#endif	
}

bool WorkerIsRunning(void)
{
#if ALLOW_WORKER_APP
	return app_worker_is_running();
#else
	return false;
#endif
}

bool WorkerIsReady(void)
{
#if ALLOW_WORKER_APP
	return workerReady;
#else
	return true;
#endif
}

void SendEventChances(int baseChance, int *chances, int chanceCount)
{
	int i;
	SendMessageToWorker(APP_SEND_BASE_EVENT_CHANCE, baseChance, 0, 0);
	for(i = 0; i < chanceCount; ++i)
	{
		SendMessageToWorker(APP_SEND_EVENT_CHANCE, chances[i], 0, 0);
	}
	SendMessageToWorker(APP_SEND_EVENT_END, 0, 0, 0);
}

void SendWorkerCanLaunch(void)
{
	SendMessageToWorker(APP_SEND_WORKER_CAN_LAUNCH, GetWorkerCanLaunch(), 0, 0);
}


void WorkerMessageHandler(uint16_t type, AppWorkerMessage *data)
{
	switch(type)
	{
		case WORKER_ACKNOWLEDGE_BASE_EVENT_CHANCE:
		{
			DEBUG_VERBOSE_LOG("WorkerApp received base chance %d", data->data0);
			break;
		}
		case WORKER_ACKNOWLEDGE_EVENT_CHANCE:
		{
			DEBUG_VERBOSE_LOG("WorkerApp received event chance[%d]=%d", data->data0, data->data1);
			break;			
		}
		case WORKER_ACKNOWLEDGE_EVENT_END:
		{
			DEBUG_VERBOSE_LOG("WorkerApp received all event chances");
			break;			
		}
		case WORKER_LAUNCHED:
		{
			DEBUG_LOG("Worker Launched. Sending events.");
			SendEventChances(GetBaseChanceOfEvent(), GetEventChances(), GetEventCount());
			SendWorkerCanLaunch();
			SetWorkerApp(true);
			SetFastMode(false);
			break;
		}
		case TRIGGER_EVENT:
		{
			if(AdventureWindowIsVisible())
				ExecuteEvent(data->data0);
			break;
		}
		case WORKER_DYING:
		{
			DEBUG_LOG("Worker dying");
			SetTickCount(data->data0);
			SetWorkerApp(false);
			if(OptionsMenuIsVisible())
				DrawOptionsMenu();
			break;
		}
		case WORKER_READY:
		{
			DEBUG_LOG("Worker ready to run events");
			workerReady = true;
			break;
		}
	}
}
