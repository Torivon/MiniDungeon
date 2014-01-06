#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Menu.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"

	 
// Called once per minute
void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) 
{
	UpdateClock();
	UpdateAdventure();
}

void ResetGame(void)
{
#if ALLOW_STAT_SHOP
	ResetStatPointsPurchased();
#endif
	InitializeCharacter();
	ResetFloor();
	ClearInventory();
}

void handle_init() {
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Start MiniDungeon");
	time_t now = time(NULL);
	struct tm *current_time = localtime(&now);
	
	srand(now);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Srand");
	InitializeExitConfirmationWindow();
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "exit window initialized");
	
	handle_minute_tick(NULL, MINUTE_UNIT);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "First handle second");
	
	ResetGame();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Reset Game");
	ShowAdventureWindow();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Show adventure window");
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

void handle_deinit() {
	UnloadBackgroundImage();
	UnloadMainBmpImage();
	UnloadTextLayers();
}

// The main event/run loop for our app
int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
