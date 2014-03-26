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
void handle_time_tick(struct tm* tick_time, TimeUnits units_changed) 
{
	if(gUpdateBattle && (units_changed & SECOND_UNIT))
	{
		UpdateBattle();
	}
	
	if(units_changed & MINUTE_UNIT)
	{
		UpdateClock();
		if(gUpdateAdventure)
			UpdateAdventure();
	}
}

void handle_init() {
	
	INFO_LOG("Starting MiniAdventure");
	
	SeedRandom();
	DEBUG_LOG("Srand");
	
	handle_time_tick(NULL, SECOND_UNIT);
	DEBUG_LOG("First handle second");
	
	// Just here so that the health and level fields are always filled in.
	InitializeCharacter();
	
	ShowTitleMenu();
	tick_timer_service_subscribe(SECOND_UNIT, &handle_time_tick);
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
