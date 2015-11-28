#include <pebble.h>
#include "Adventure.h"
#include "Logging.h"
#include "MainMenu.h"
#include "OptionsMenu.h"
#include "WorkerControl.h"

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

void AppDying(void)
{
	if(WorkerIsRunning())
		SendMessageToWorker(APP_DYING, 0, 0, 0);
}

void AppAwake(void)
{
	if(launch_reason() == APP_LAUNCH_WORKER && WorkerIsRunning())
		SendMessageToWorker(APP_AWAKE, 0, 0, 0);
}

void UnpauseWorkerApp(void)
{
	SendMessageToWorker(UNPAUSE_WORKER_APP, 0, 0, 0);
}

void PauseWorkerApp(void)
{
	SendMessageToWorker(PAUSE_WORKER_APP, 0, 0, 0);
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

void WorkerMessageHandler(uint16_t type, AppWorkerMessage *data)
{
	switch(type)
	{
		case WORKER_LAUNCHED:
		{
			workerHandlingUpdates = true;
			SendMessageToWorker(APP_SEND_TICK_COUNT, GetTickCount(), 0, 0);
			if(AdventureWindowIsVisible())
			{
				UnpauseWorkerApp();
			}
			else
			{
				PauseWorkerApp();
				if(OptionsMenuIsVisible())
					DrawOptionsMenu();
			}
				
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
			workerHandlingUpdates = false;
			SetTickCount(data->data0);
			break;
		}
	}
}
