#include <pebble_worker.h>

#include "MiniDungeon.h"
#include "Utils.h"
#include "WorkerControl.h"

void SendMessageToApp(uint8_t type, uint16_t data0, uint16_t data1, uint16_t data2)
{
	AppWorkerMessage msg_data = {
		.data0 = data0,
		.data1 = data1,
		.data2 = data2
	};
	app_worker_send_message(type, &msg_data);
}

static bool handlingTicks = false;
static bool appAlive = false;
static int lastEvent = -1;
static bool forcedDelay = false; // Make sure we don't trigger an event while the app is still closing
static bool workerCanLaunch = false;

int importedChances[10];
int chanceCount = 0;
bool chancesComplete = false;

static int baseChanceOfEvent = 0;
static int ticksSinceLastEvent = 0;

static void TriggerEvent(int event)
{
	if(event > -1)
	{
		SendMessageToApp(TRIGGER_EVENT, event, 0, 0);
		ticksSinceLastEvent = 0;
		lastEvent = -1;
	}
}

void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) 
{	
	if(handlingTicks)
	{
		if(forcedDelay)
		{
			forcedDelay = false;
			return;
		}
#if EVENT_CHANCE_SCALING
		++ticksSinceLastEvent;
#endif
		lastEvent = ComputeRandomEvent_inline(baseChanceOfEvent, ticksSinceLastEvent, importedChances, chanceCount, FAST_MODE_IN_BACKGROUND);
		if(lastEvent > -1)
		{
			if(appAlive)
			{
				TriggerEvent(lastEvent);
			}
			else
			{
				if(workerCanLaunch)
					worker_launch_app();
				handlingTicks = false;
			}
		}
	}
}

static void AppMessageHandler(uint16_t type, AppWorkerMessage *data)
{
	switch(type)
	{
		case APP_DYING:
		{
			appAlive = false;
			handlingTicks = !data->data1; // Don't handle ticks while in combat
#if EVENT_CHANCE_SCALING
			ticksSinceLastEvent = data->data0;
#endif
			forcedDelay = true;
			break;
		}
		case APP_AWAKE:
		{
			appAlive = true;
			handlingTicks = false;
			TriggerEvent(lastEvent);
			break;
		}
		case APP_SEND_BASE_EVENT_CHANCE:
		{
			baseChanceOfEvent = data->data0;
			break;
		}
		case APP_SEND_EVENT_CHANCE:
		{
			if(chancesComplete)
				break;
			importedChances[chanceCount] = data->data0;
			++chanceCount;
			break;
		}
		case APP_SEND_EVENT_END:
		{
			chancesComplete = true;
			SendMessageToApp(WORKER_READY, 0, 0, 0);
			break;
		}
		case APP_SEND_WORKER_CAN_LAUNCH:
		{
			workerCanLaunch = data->data0;
			break;
		}
	}
}

static void init() 
{
	// Initialize your worker here
	SendMessageToApp(WORKER_LAUNCHED, 0, 0, 0);
	app_worker_message_subscribe(AppMessageHandler);
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void deinit() {
	// Deinitialize your worker here
	SendMessageToApp(WORKER_DYING, ticksSinceLastEvent, 0, 0);
	app_worker_message_unsubscribe();
	tick_timer_service_unsubscribe();
}

int main(void) {
	init();
	worker_event_loop();
	deinit();
}