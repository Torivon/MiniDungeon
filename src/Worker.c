#include <pebble_worker.h>

#include "MiniDungeon.h"
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

#if ALLOW_SHOP
// These should add up to 100
int chances[] = 
{
	44,
	44,
	9,
	3
};
#else
// These should add up to 100
int chances[] = 
{
	40,
	50,
	10
};
#endif

// Returns an integer in the range [1,max]
int Random(int max)
{
	int result = (uint16_t)(rand() % max) + 1;
	return result;
}

static int baseChanceOfEvent = 35;
static int ticksSinceLastEvent = 0;

int ComputeRandomEvent(void)
{
	int result = Random(100);
	int i = 0;
	int acc = 0;
	int chanceOfEvent = baseChanceOfEvent;
	int event = -1;
#if EVENT_CHANCE_SCALING
	if(ticksSinceLastEvent > 20)
	{
		chanceOfEvent += (ticksSinceLastEvent - 20) * 2;
	}
#endif
	
	if(result > chanceOfEvent)
		return -1;
		
	result = Random(100);
	
	do
	{
		acc += chances[i];
		if(acc >= result)
		{
			event = i;
			break;
		}
		++i;      
    } while (i < 4);
	return event;
}

static void TriggerEvent(int event)
{
	SendMessageToApp(TRIGGER_EVENT, event, 0, 0);
	ticksSinceLastEvent = 0;	
}

static void AppMessageHandler(uint16_t type, AppWorkerMessage *data)
{
	switch(type)
	{
		case APP_SEND_TICK_COUNT:
		{
#if EVENT_CHANCE_SCALING
			ticksSinceLastEvent = data->data0;
#endif
			appAlive = true;
			break;
		}
		case PAUSE_WORKER_APP:
		{
			handlingTicks = false;
			break;
		}
		case UNPAUSE_WORKER_APP:
		{
			handlingTicks = true;
			break;
		}
		case APP_DYING:
		{
			appAlive = false;
			handlingTicks = true;
			break;
		}
		case APP_AWAKE:
		{
			appAlive = true;
			TriggerEvent(lastEvent);
			break;
		}
	}
}

void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) 
{
	if(handlingTicks)
	{
#if EVENT_CHANCE_SCALING
		++ticksSinceLastEvent;
#endif
		lastEvent = ComputeRandomEvent();
		if(lastEvent > -1)
		{
			if(appAlive)
			{
				TriggerEvent(lastEvent);
			}
			else
			{
				worker_launch_app();
				handlingTicks = false;
			}
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