#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "Persistence.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"
#include "WorkerControl.h"
	 
// Called once per minute
void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) 
{
#if ALLOW_WORKER_APP
	WorkerAppLaunchCheck();
#endif
	UpdateClock();
	UpdateAdventure();
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
	
	srand(now);
	DEBUG_LOG("Srand");
	
	handle_minute_tick(NULL, MINUTE_UNIT);
	DEBUG_LOG("First handle second");
	
	InitializeGameData();

#if ALLOW_WORKER_APP
	app_worker_message_subscribe(WorkerMessageHandler);
	AppAwake();
#endif
	DEBUG_LOG("InitializeGameData");
	ShowAdventureWindow();
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

void handle_deinit() 
{
	INFO_LOG("Cleaning up on exit.");
#if ALLOW_WORKER_APP
	WorkerAppLaunchCheck();
#endif
	SavePersistedData();
	UnloadBackgroundImage();
	UnloadMainBmpImage();
	UnloadTextLayers();
	tick_timer_service_unsubscribe();
#if ALLOW_WORKER_APP
	AppDying();
	app_worker_message_unsubscribe();
#endif
}

// The main event/run loop for our app
int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
