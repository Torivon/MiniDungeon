#include "pebble.h"

#include "Logging.h"
#include "UILayers.h"
#include "Utils.h"

#define WINDOW_ROW_HEIGHT 16

TextLayer * InitializeTextLayer(GRect frame, GFont font)
{
	TextLayer *textLayer = text_layer_create(frame);
	text_layer_set_text_color(textLayer, GColorWhite);
	text_layer_set_background_color(textLayer, GColorClear);
	text_layer_set_font(textLayer, font);
	return textLayer;
}

//******* MENU LAYERS *******//

TextLayer *menuLayers[MAX_MENU_ENTRIES];
TextLayer *menuDescLayer;
bool menuLayersInitialized = false;

GRect baseMenuFrame = {.origin = {.x = 92, .y = 7}, .size = {.w = 45, .h = WINDOW_ROW_HEIGHT}};
GRect menuDescFrame = {.origin = {.x = 10, .y = 107}, .size = {.w = 144-20, .h = 20}};

void RemoveMenuLayers(void)
{
	int i;
	
	if(!menuLayersInitialized)
		return;
	
	for(i = 0; i < MAX_MENU_ENTRIES; ++i)
	{
		layer_remove_from_parent(text_layer_get_layer(menuLayers[i]));
	}
	layer_remove_from_parent(text_layer_get_layer(menuDescLayer));		
}

void InitializeMenuLayers(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	
	int i;
	for(i = 0; i < MAX_MENU_ENTRIES; ++i)
	{
		if(!menuLayersInitialized)
		{
			GRect menuFrame = baseMenuFrame;
			menuFrame.origin.y += i * WINDOW_ROW_HEIGHT;
			menuLayers[i] = InitializeTextLayer(menuFrame, fonts_get_system_font(FONT_KEY_GOTHIC_14));
			
		}
		layer_add_child(window_layer, text_layer_get_layer(menuLayers[i]));
		layer_set_hidden(text_layer_get_layer(menuLayers[i]), true);
	}

	if(!menuLayersInitialized)
	{
		// Init the text layer used to show the menu description
		menuDescLayer = InitializeTextLayer(menuDescFrame, fonts_get_system_font(FONT_KEY_GOTHIC_14));
		
		menuLayersInitialized = true;
	}
	
	layer_add_child(window_layer, text_layer_get_layer(menuDescLayer));
}

void SetMenuHighlight(int menuItem, bool selected)
{
	if(selected)
	{
		text_layer_set_text_color(menuLayers[menuItem], GColorBlack);
		text_layer_set_background_color(menuLayers[menuItem], GColorWhite);
	}
	else
	{
		text_layer_set_text_color(menuLayers[menuItem], GColorWhite);
		text_layer_set_background_color(menuLayers[menuItem], GColorClear);
	}
}

void SetMenuDescription(const char *desc)
{
	text_layer_set_text(menuDescLayer, desc ? desc : "");	
}

void ShowMenuLayer(int index, const char *text)
{
	text_layer_set_text(menuLayers[index], text);
	layer_set_hidden(text_layer_get_layer(menuLayers[index]), false);
}

void HideMenuLayer(int index)
{
	layer_set_hidden(text_layer_get_layer(menuLayers[index]), true);
}

void HideAllMenuLayers(void)
{
	int i;
	for(i = 0; i < MAX_MENU_ENTRIES; ++i)
	{
		HideMenuLayer(i);
	}
}

//******** Background *********//
// backgroundImage holds the frame for all other UI elements
BitmapLayer *backgroundImage;
GBitmap *backgroundBitmap;
static bool backgroundLoaded = false;

void UnloadBackgroundImage(void)
{
	if(!backgroundLoaded)
		return;
	bitmap_layer_destroy(backgroundImage);
	backgroundImage = NULL;
	gbitmap_destroy(backgroundBitmap);
	backgroundBitmap = NULL;
	backgroundLoaded = false;
}

void RemoveBackgroundImage()
{
	if(!backgroundLoaded)
		return;

	layer_remove_from_parent(bitmap_layer_get_layer(backgroundImage));
}

void LoadBackgroundImage(Window *window, int id)
{
	Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
	if(!backgroundLoaded)
	{
		backgroundBitmap = gbitmap_create_with_resource(id);
		backgroundImage = bitmap_layer_create(bounds);
		bitmap_layer_set_bitmap(backgroundImage, backgroundBitmap);
		bitmap_layer_set_alignment(backgroundImage, GAlignLeft);
		backgroundLoaded = true;
	}
	
	layer_add_child(window_layer, bitmap_layer_get_layer(backgroundImage));		
}

//******** Main part of the screen *********//
// mainImage will hold the main piece of artwork for each state, including monster art during battle.
// mainTextLayers and mainNumberLayers will be used for various things requiring names and numbers.
//		Monster name/health
//		Floor/Number
//		item name / count

GBitmap *mainImageBitmap;
BitmapLayer *mainImage;
GRect mainFrame = {.origin = {.x = 5, .y = 25}, .size = {.w = 80, .h = 80}};
static bool mainImageLoaded = false;
static int mainImageResourceLoaded = -1;

TextLayer *mainTextLayers[MAX_MAIN_TEXT_LAYERS];
GRect mainTextBaseFrame = {.origin = {.x = 6, .y = 7}, .size = {.w = 80, .h = WINDOW_ROW_HEIGHT}};
	
TextLayer *mainNumberLayers[MAX_MAIN_TEXT_LAYERS];
GRect mainNumberBaseFrame = {.origin = {.x = 54, .y = 7}, .size = {.w = 30, .h = WINDOW_ROW_HEIGHT}};

bool mainLayersInitialized = false;

void RemoveMainLayers(void)
{
	int i;
	
	if(!mainLayersInitialized)
		return;
	
	for(i = 0; i < MAX_MAIN_TEXT_LAYERS; ++i)
	{
		layer_remove_from_parent(text_layer_get_layer(mainTextLayers[i]));
		layer_remove_from_parent(text_layer_get_layer(mainNumberLayers[i]));
	}
}

void InitializeMainLayers(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	int i;
	for(i = 0; i < MAX_MAIN_TEXT_LAYERS; ++i)
	{
		if(!mainLayersInitialized)
		{
			GRect localFrame = mainTextBaseFrame;
			localFrame.origin.y += i * WINDOW_ROW_HEIGHT;
			mainTextLayers[i] = InitializeTextLayer(localFrame, fonts_get_system_font(FONT_KEY_GOTHIC_14));
			localFrame = mainNumberBaseFrame;
			localFrame.origin.y += i * WINDOW_ROW_HEIGHT;
			mainNumberLayers[i] = InitializeTextLayer(localFrame, fonts_get_system_font(FONT_KEY_GOTHIC_14));
			text_layer_set_text_alignment(mainNumberLayers[i], GTextAlignmentRight);
		}

		layer_add_child(window_layer, text_layer_get_layer(mainTextLayers[i]));
		layer_set_hidden(text_layer_get_layer(mainTextLayers[i]), true);
		layer_add_child(window_layer, text_layer_get_layer(mainNumberLayers[i]));
		layer_set_hidden(text_layer_get_layer(mainNumberLayers[i]), true);
	}
	mainLayersInitialized = true;
}

void ShowMainWindowRow(int index, const char *text, const char *number)
{
	if(index >= MAX_MAIN_TEXT_LAYERS)
		return;
		
	text_layer_set_text(mainTextLayers[index], text);
	layer_set_hidden(text_layer_get_layer(mainTextLayers[index]), false);

	text_layer_set_text(mainNumberLayers[index], number);
	layer_set_hidden(text_layer_get_layer(mainNumberLayers[index]), false);
}

void RemoveMainBmpImage(void)
{
	if(!mainImageLoaded)
		return;
	
	layer_remove_from_parent(bitmap_layer_get_layer(mainImage));
}

void UnloadMainBmpImage(void)
{
	if(!mainImageLoaded)
		return;
	
	DEBUG_LOG("Removing resourceId %d.", mainImageResourceLoaded);
	layer_remove_from_parent(bitmap_layer_get_layer(mainImage));
	bitmap_layer_destroy(mainImage);
	gbitmap_destroy(mainImageBitmap);
	mainImage = NULL;
	mainImageBitmap = NULL;
	mainImageLoaded = false;
	mainImageResourceLoaded = -1;
}



void LoadMainBmpImage(Window *window, int id)
{
	int resourceId = id;
	
	Layer *window_layer = window_get_root_layer(window);
	
#if DISABLE_MENU_BMPS
	return;
#endif
	
	if(!window)
	{
		DEBUG_LOG("Skipping image load due to window not yet available.");
		return;
	}
		
	if(mainImageLoaded)
	{
		if(mainImageResourceLoaded == resourceId)
		{
			DEBUG_LOG("Resource %d already loaded.", resourceId);
			layer_add_child(window_layer, bitmap_layer_get_layer(mainImage));
			return; // already loaded the correct one.
		}
		DEBUG_LOG("Unloading resourceId %d.", mainImageResourceLoaded);
		UnloadMainBmpImage();
	}
	
	DEBUG_LOG("Loading resourceId %d.", resourceId);

	mainImageBitmap = gbitmap_create_with_resource(resourceId);
	mainImage = bitmap_layer_create(mainFrame);
	bitmap_layer_set_bitmap(mainImage, mainImageBitmap);
	bitmap_layer_set_alignment(mainImage, GAlignCenter);
	layer_add_child(window_layer, bitmap_layer_get_layer(mainImage));
	mainImageLoaded = true;
	mainImageResourceLoaded = resourceId;
}

//******* CLOCK *********//

TextLayer *clockLayer; // The clock
GRect clockFrame = {.origin = {.x = 85, .y = 127}, .size = {.w = 144-85, .h = 168-127}};
bool clockLayerInitialized = false;

void UpdateClock(void)
{
	if(!clockLayerInitialized)
		return;

	static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.
	char *time_format;

	if (clock_is_24h_style()) 
	{
		time_format = "%R";
	}
	else 
	{
		time_format = "%I:%M";
	}

	time_t now = time(NULL);
	struct tm *current_time = localtime(&now);
	
	strftime(timeText, sizeof(timeText), time_format, current_time);

	text_layer_set_text(clockLayer, timeText);
}

void RemoveClockLayer(void)
{
	if(!clockLayerInitialized)
		return;

	layer_remove_from_parent(text_layer_get_layer(clockLayer));
}

void InitializeClockLayer(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	if(!clockLayerInitialized)
	{
		// Init the text layer used to show the time
		clockLayer = InitializeTextLayer(clockFrame, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
		clockLayerInitialized = true;
		UpdateClock();
	}
	layer_add_child(window_layer, text_layer_get_layer(clockLayer));
}

// *********** Level **********//

TextLayer *levelLayer;
GRect levelFrame = {.origin = {.x = 10, .y = 127}, .size = {.w = 144-85, .h = 168-127}};
bool levelLayerInitialized = false;

void UpdateLevelLayerText(int level)
{
	if(!levelLayerInitialized)
		return;
		
	static char levelText[] = "00";

	IntToString(levelText, 2, level);
	text_layer_set_text(levelLayer, levelText);
}

void RemoveLevelLayer(void)
{
	if(!levelLayerInitialized)
		return;
		
	layer_remove_from_parent(text_layer_get_layer(levelLayer));
}

void InitializeLevelLayer(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	if(!levelLayerInitialized)
	{
		levelLayer = InitializeTextLayer(levelFrame, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
		levelLayerInitialized = true;
	}
	layer_add_child(window_layer, text_layer_get_layer(levelLayer));
}

// ********** Health *********//

TextLayer *currentHealthLayer;
GRect currentHealthFrame = {.origin = {.x = 42, .y = 126}, .size = {.w = 50, .h = 168-130}};

TextLayer *maxHealthLayer;
GRect maxHealthFrame = {.origin = {.x = 42, .y = 143}, .size = {.w = 50, .h = 168-140}};
bool healthLayersInitialized = false;
void UpdateHealthText(int current, int max)
{
	if(!healthLayersInitialized)
		return;
		
	static char currentHealthText[] = "0000"; // Needs to be static because it's used by the system later.
	static char maxHealthText[] = "0000"; // Needs to be static because it's used by the system later.

	IntToString(currentHealthText, 4, current);
	text_layer_set_text(currentHealthLayer, currentHealthText);

	IntToString(maxHealthText, 4, max);
	text_layer_set_text(maxHealthLayer, maxHealthText);
}

void RemoveHealthLayer(void)
{
	if(!healthLayersInitialized)
		return;
		
	layer_remove_from_parent(text_layer_get_layer(currentHealthLayer));
	layer_remove_from_parent(text_layer_get_layer(maxHealthLayer));
}

void InitializeHealthLayer(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	if(!healthLayersInitialized)
	{
		currentHealthLayer = InitializeTextLayer(currentHealthFrame, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
		maxHealthLayer = InitializeTextLayer(maxHealthFrame, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
		
		healthLayersInitialized = true;
	}

	layer_add_child(window_layer, text_layer_get_layer(currentHealthLayer));
	layer_add_child(window_layer, text_layer_get_layer(maxHealthLayer));
}

//***************** Global Functions **************//

void WindowAppear(Window *window)
{
	LoadBackgroundImage(window, RESOURCE_ID_IMAGE_BACKGROUND);
	InitializeMainLayers(window);
	InitializeClockLayer(window);
	InitializeHealthLayer(window);
	InitializeLevelLayer(window);
	InitializeMenuLayers(window);	
}

void WindowDisappear(Window *window)
{
	(void)window;
	
	RemoveMainBmpImage();
	RemoveBackgroundImage();
	RemoveMainLayers();
	RemoveClockLayer();
	RemoveHealthLayer();
	RemoveLevelLayer();
	RemoveMenuLayers();
}

Window * InitializeWindow(const char *name, bool animated)
{
	DEBUG_LOG("Creating window %s",name);
	Window *window = window_create();
	window_set_fullscreen(window, true); // Do I want full screen?
	window_set_background_color(window, GColorBlack);
	DEBUG_LOG("Window %s created",name);
	return window;		
}

Window * InitializeMenuWindow(void *menuWindow, const char *name, bool animated, WindowHandler init, WindowHandler deinit, WindowHandler appear, WindowHandler disappear, ClickConfigProvider clicker)
{
	Window *window = InitializeWindow(name, animated);
	WindowHandlers handlers = {.load = init, .unload = deinit, .appear = appear, .disappear = disappear};
	window_set_window_handlers(window,handlers);
	
	window_set_click_config_provider(window, clicker);
	window_set_user_data(window,menuWindow);
	window_stack_push(window, animated);
	return window;
}

void UnloadTextLayers(void) {
	for(int i = 0; i < MAX_MENU_ENTRIES ; i++) {
		if(!menuLayers[i])
			continue;
		text_layer_destroy(menuLayers[i]);
	}
	text_layer_destroy(menuDescLayer);
	
	for(int i = 0; i < MAX_MAIN_TEXT_LAYERS; i++) {
		if(mainTextLayers[i])
			text_layer_destroy(mainTextLayers[i]);
		if(mainNumberLayers[i])
			text_layer_destroy(mainNumberLayers[i]);
	}
	text_layer_destroy(maxHealthLayer);
	text_layer_destroy(currentHealthLayer);
	text_layer_destroy(clockLayer);
	text_layer_destroy(levelLayer);
}
