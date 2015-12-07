#include <pebble_worker.h>

#include "../src/Events.h"
#include "../src/MiniDungeon.h"
#include "../src/Utils.h"
#include "../src/WorkerControl.h"
#include "Worker_Persistence.h"

#if ALLOW_WORKER_APP

void SendMessageToApp(uint8_t type, uint16_t data0, uint16_t data1, uint16_t data2)
{
#if ALLOW_WORKER_APP_MESSAGES
	AppWorkerMessage msg_data = {
		.data0 = data0,
		.data1 = data1,
		.data2 = data2
	};
	app_worker_send_message(type, &msg_data);
#endif
}

static bool handlingTicks = false;
static int lastEvent = -1;
static bool forcedDelay = false; // Make sure we don't trigger an event while the app is still closing
static bool appAlive = true; 
static bool error = false;

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
	
	if(GetClosedInBattle())
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
		lastEvent = ComputeRandomEvent_inline(GetBaseChanceOfEvent(), ticksSinceLastEvent, GetEventChances(), GetEventCount(), FAST_MODE_IN_BACKGROUND);
		if(lastEvent > -1)
		{
			if(GetWorkerCanLaunch())
				worker_launch_app();
			handlingTicks = false;
		}
	}
	else
	{
		if(lastEvent > -1)
		{
			if(GetWorkerCanLaunch())
				worker_launch_app();
		}
	}
}

static void InitializeState(int ticks)
{
	handlingTicks = !GetClosedInBattle(); // Don't handle ticks while in combat
#if EVENT_CHANCE_SCALING
	ticksSinceLastEvent = ticks;
#endif
	forcedDelay = true;
	lastEvent = -1;
	appAlive = false;
	error = false;	
}

static void AppMessageHandler(uint16_t type, AppWorkerMessage *data)
{
	switch(type)
	{
		case APP_DYING:
		{
			SetClosedInBattle(data->data1);
			InitializeState(data->data0);
			break;
		}
		case APP_AWAKE:
		{
			SendMessageToApp(WORKER_SEND_STATE1, handlingTicks, lastEvent, ticksSinceLastEvent);
			SendMessageToApp(WORKER_SEND_STATE2, GetClosedInBattle(), forcedDelay, appAlive);
			if(error)
				SendMessageToApp(WORKER_SEND_ERROR, 0, 0, 0);
			handlingTicks = false;
			appAlive = true;
			TriggerEvent(lastEvent);
			break;
		}
		case APP_SEND_WORKER_CAN_LAUNCH:
		{
			SetWorkerCanLaunch(data->data0);
			break;
		}
	}
}

static void init() 
{
	// Initialize your worker here
	time_t now = time(NULL);
	srand(now);
	if(!LoadWorkerData())
		worker_launch_app();
#if ALLOW_WORKER_APP_LISTENING
	app_worker_message_subscribe(AppMessageHandler);
#endif
	InitializeState(0);
	SendMessageToApp(WORKER_LAUNCHED, 0, 0, 0);
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void deinit() {
	// Deinitialize your worker here
	SendMessageToApp(WORKER_DYING, ticksSinceLastEvent, 0, 0);
#if ALLOW_WORKER_APP_LISTENING
	app_worker_message_unsubscribe();
#endif
	tick_timer_service_unsubscribe();
}


int main(void) {
	init();
	worker_event_loop();
	deinit();
}
#endif