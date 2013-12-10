#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Menu.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"

	 
// Called once per seconds
void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) 
{
	if(! tick_time->tm_sec %30)
		return;
	if(units_changed & MINUTE_UNIT)
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
	time_t now = time(NULL);
	struct tm *current_time = localtime(&now);
	
	srand(now);
	
	InitializeExitConfirmationWindow();
	
	handle_second_tick(current_time, MINUTE_UNIT);
	tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
	

	ResetGame();
	ShowAdventureWindow();
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
