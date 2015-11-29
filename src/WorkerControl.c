#include <pebble.h>
#include "Adventure.h"
#include "Logging.h"
#include "MainMenu.h"
#include "OptionsMenu.h"
#include "WorkerControl.h"

static bool workerReady = false;

void SetWorkerReady(bool ready)
{
	workerReady = true;
}

void SendMessageToWorker(uint8_t type, uint16_t data0, uint16_t data1, uint16_t data2)
{
	AppWorkerMessage msg_data = {
		.data0 = data0,
		.data1 = data1,
		.data2 = data2
	};
	app_worker_send_message(type, &msg_data);
}

static bool workerHandlingUpdates = false;
static bool workerAppMaybeLaunching = false;

void AppDying(bool closingWhileInBattle)
{
	uint16_t test = (uint16_t)closingWhileInBattle;
	DEBUG_LOG("Original %d, cast %d, !cast %d", closingWhileInBattle, test, !test);
	if(WorkerIsRunning())
		SendMessageToWorker(APP_DYING, GetTickCount(), closingWhileInBattle, 0);
}

void AppAwake(void)
{
	if(WorkerIsRunning())
		SendMessageToWorker(APP_AWAKE, 0, 0, 0);
}

void ActivateWorkerAppLaunchCheck(void)
{
	workerAppMaybeLaunching = true;
}

void WorkerAppLaunchCheck(void)
{
	if(workerAppMaybeLaunching)
	{
		SetWorkerApp(WorkerIsRunning());
		workerAppMaybeLaunching = false;
	}		
}

AppWorkerResult LaunchWorkerApp()
{
	bool running = WorkerIsRunning();

	if(running)
		return false;
	
	return app_worker_launch();
}

AppWorkerResult KillWorkerApp()
{
	bool running = WorkerIsRunning();
	
	if(!running)
		return false;
	
	return app_worker_kill();
}

bool WorkerIsHandlingUpdates(void)
{
	return workerHandlingUpdates;
}

bool WorkerIsRunning(void)
{
#if ALLOW_WORKER_APP
	return app_worker_is_running();
#else
	return false;
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

void WorkerMessageHandler(uint16_t type, AppWorkerMessage *data)
{
	switch(type)
	{
		case WORKER_LAUNCHED:
		{
			workerHandlingUpdates = false;
			DEBUG_LOG("Worker Launched. Sending events.");
			SendEventChances(GetBaseChanceOfEvent(), GetEventChances(), GetEventCount());
			if(OptionsMenuIsVisible())
				DrawOptionsMenu();
				
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
			workerHandlingUpdates = false;
			SetTickCount(data->data0);
			SetWorkerApp(false);
			if(OptionsMenuIsVisible())
				DrawOptionsMenu();
			break;
		}
		case WORKER_ACKNOWLEDGE_BASE_CHANCE:
		{
			DEBUG_LOG("Worker got base chance of %d", data->data0);
			break;
		}
		case WORKER_ACKNOWLEDGE_EVENT_CHANCE:
		{
			DEBUG_LOG("Worker got event chance[%d]=%d", data->data0, data->data1);
			break;
		}
		case WORKER_READY:
		{
			DEBUG_LOG("Worker has event chances");
			workerReady = true;
			break;
		}
		case WORKER_PREVIOUS_STATE:
		{
			DEBUG_LOG("handlingTicks %d, appAlive %d, lastEvent %d", data->data0, data->data1, data->data2);
			break;
		}
	}
}
