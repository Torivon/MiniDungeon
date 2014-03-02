#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "Menu.h"
#include "Persistence.h"
#include "Shop.h"
#include "Story.h"
#include "TitleMenu.h"
#include "UILayers.h"
#include "Utils.h"

	 
// Called once per minute
void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) 
{
	UpdateClock();
	UpdateAdventure();
}

void handle_init() {
	
	INFO_LOG("Starting MiniAdventure");
	
	SeedRandom();
	DEBUG_LOG("Srand");
	
	handle_minute_tick(NULL, MINUTE_UNIT);
	DEBUG_LOG("First handle second");
	
	// Just here so that the health and level fields are always filled in.
	InitializeCharacter();
	
	ShowTitleMenu();
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

void handle_deinit() 
{
	INFO_LOG("Cleaning up on exit.");
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
