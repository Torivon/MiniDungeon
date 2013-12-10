#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Menu.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"

#define MY_UUID { 0xA4, 0x20, 0x10, 0x83, 0x81, 0xDF, 0x4C, 0x5F, 0xA1, 0xFE, 0xF8, 0x43, 0x4B, 0xF7, 0x01, 0x49 }
PBL_APP_INFO(MY_UUID,
             "MiniDungeon", "Jonathan Panttaja",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_STANDARD_APP);
			 
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
