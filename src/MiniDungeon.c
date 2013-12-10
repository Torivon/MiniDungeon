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
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) 
{
	(void)ctx;
	(void)t;

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

void handle_init(AppContextRef ctx) {
	(void)ctx;
	PblTm currentTime;
	unsigned int unixTime;

	resource_init_current_app(&APP_RESOURCES);

	get_time(&currentTime);
	
	srand(time(NULL));
	
	InitializeExitConfirmationWindow();
	
	handle_minute_tick(ctx, NULL);

	ResetGame();
	ShowAdventureWindow();
}

void handle_deinit(AppContextRef ctx) {
	(void)ctx;

	UnloadBackgroundImage();
	UnloadMainBmpImage();
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		// Handle app start
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,

		// Handle time updates
		.tick_info = {
			.tick_handler = &handle_minute_tick,
			.tick_units = MINUTE_UNIT
			}
	};
	app_event_loop(params, &handlers);
}
