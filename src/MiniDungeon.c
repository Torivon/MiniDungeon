#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "OptionsMenu.h"
#include "Persistence.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"
#include "WorkerControl.h"
	 
static bool hasFocus = true;

bool HasFocus(void)
{
	return hasFocus;
}

// Called once per minute
void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) 
{
	DEBUG_LOG("Main App tick");
	if(!hasFocus)
		return;
	
	UpdateClock();
	UpdateAdventure();
}

void focus_handler(bool in_focus) {
	hasFocus = in_focus;
	if(hasFocus)
	{
		UpdateClock();
		SetUpdateDelay();
		INFO_LOG("Gained focus.");
	}
	else
	{
		INFO_LOG("Lost focus.");
	}
}

void InitializeGameData(void)
{
	if(!LoadPersistedData())
		ResetGame();
}

void ResetGame(void)
{
#if ALLOW_STAT_SHOP
	ResetStatPointsPurchased();
#endif
	InitializeCharacter();
	ResetFloor();
	ClearInventory();
	
	SavePersistedData();
}

void handle_init() {
	
	INFO_LOG("Starting MiniDungeon");
	time_t now = time(NULL);
	
	app_focus_service_subscribe(focus_handler);

	srand(now);
	DEBUG_LOG("Srand");
	
	handle_minute_tick(NULL, MINUTE_UNIT);
	DEBUG_LOG("First handle second");
	
#if ALLOW_WORKER_APP
	app_worker_message_subscribe(WorkerMessageHandler);
#endif
	
	InitializeGameData();

#if ALLOW_WORKER_APP
	AppAwake();
#endif
	
	DEBUG_LOG("InitializeGameData");
	ShowAdventureWindow();
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

void handle_deinit() 
{
	INFO_LOG("Cleaning up on exit.");
	SavePersistedData();
	UnloadBackgroundImage();
	UnloadMainBmpImage();
	UnloadTextLayers();
	tick_timer_service_unsubscribe();
	app_focus_service_unsubscribe();
#if ALLOW_WORKER_APP		
	AppDying(ClosingWhileInBattle());
	app_worker_message_unsubscribe();
#endif
}

// The main event/run loop for our app
int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
