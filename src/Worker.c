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
static int lastEvent = -1;
static bool forcedDelay = false; // Make sure we don't trigger an event while the app is still closing
static bool workerCanLaunch = false;
static bool closingWhileInBattle = false;
static bool appAlive = true; 
static bool error = false;

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
	if(appAlive)
		return;
	
	if(closingWhileInBattle)
		return;
		
	if(!handlingTicks && lastEvent == -1)
	{
		error = true;
		worker_launch_app();
	}

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
			if(workerCanLaunch)
				worker_launch_app();
			handlingTicks = false;
		}
	}
	else
	{
		if(lastEvent > -1)
		{
			if(workerCanLaunch)
				worker_launch_app();
		}
	}
}

static void AppMessageHandler(uint16_t type, AppWorkerMessage *data)
{
	switch(type)
	{
		case APP_DYING:
		{
			closingWhileInBattle = data->data1;
			handlingTicks = !closingWhileInBattle; // Don't handle ticks while in combat
#if EVENT_CHANCE_SCALING
			ticksSinceLastEvent = data->data0;
#endif
			forcedDelay = true;
			lastEvent = -1;
			appAlive = false;
			error = false;
			break;
		}
		case APP_AWAKE:
		{
			SendMessageToApp(WORKER_SEND_STATE1, handlingTicks, lastEvent, ticksSinceLastEvent);
			SendMessageToApp(WORKER_SEND_STATE2, closingWhileInBattle, forcedDelay, appAlive);
			if(error)
				SendMessageToApp(WORKER_SEND_ERROR, 0, 0, 0);
			handlingTicks = false;
			appAlive = true;
			TriggerEvent(lastEvent);
			break;
		}
		case APP_SEND_BASE_EVENT_CHANCE:
		{
			baseChanceOfEvent = data->data0;
			SendMessageToApp(WORKER_ACKNOWLEDGE_BASE_EVENT_CHANCE, baseChanceOfEvent, 0, 0);
			break;
		}
		case APP_SEND_EVENT_CHANCE:
		{
			if(chancesComplete)
				break;
			importedChances[chanceCount] = data->data0;
			SendMessageToApp(WORKER_ACKNOWLEDGE_EVENT_CHANCE, chanceCount, data->data0, 0);
			++chanceCount;
			break;
		}
		case APP_SEND_EVENT_END:
		{
			SendMessageToApp(WORKER_ACKNOWLEDGE_EVENT_END, 0, 0, 0);
			chancesComplete = true;
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
	time_t now = time(NULL);
	srand(now);
	app_worker_message_subscribe(AppMessageHandler);
	SendMessageToApp(WORKER_LAUNCHED, 0, 0, 0);
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