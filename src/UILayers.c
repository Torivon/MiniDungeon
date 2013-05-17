#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "Menu.h"
#include "UILayers.h"
#include "Utils.h"
#include "Adventure.h"

#define WINDOW_ROW_HEIGHT 16

void InitializeTextLayer(TextLayer *textLayer, GRect frame, GFont font)
{
	text_layer_init(textLayer, frame);
	text_layer_set_text_color(textLayer, GColorWhite);
	text_layer_set_background_color(textLayer, GColorClear);
	text_layer_set_font(textLayer, font);
}

//******* MENU LAYERS *******//

TextLayer menuLayers[MAX_MENU_ENTRIES];
TextLayer menuDescLayer;
bool menuLayersInitialized = false;

GRect baseMenuFrame = {.origin = {.x = 92, .y = 7}, .size = {.w = 45, .h = WINDOW_ROW_HEIGHT}};
GRect menuDescFrame = {.origin = {.x = 10, .y = 107}, .size = {.w = 144-20, .h = 20}};

void RemoveMenuLayers(void)
{
	int i;
	for(i = 0; i < MAX_MENU_ENTRIES; ++i)
	{
		layer_remove_from_parent(&menuLayers[i].layer);
	}
	layer_remove_from_parent(&menuDescLayer.layer);		
}

void InitializeMenuLayers(Window *window)
{
	int i;
	for(i = 0; i < MAX_MENU_ENTRIES; ++i)
	{
		if(!menuLayersInitialized)
		{
			GRect menuFrame = baseMenuFrame;
			menuFrame.origin.y += i * WINDOW_ROW_HEIGHT;
			InitializeTextLayer(&menuLayers[i], menuFrame, fonts_get_system_font(FONT_KEY_GOTHIC_14));
		}
		layer_add_child(&window->layer, &menuLayers[i].layer);
		layer_set_hidden(&menuLayers[i].layer, true);
	}

	if(!menuLayersInitialized)
	{
		// Init the text layer used to show the menu description
		InitializeTextLayer(&menuDescLayer, menuDescFrame, fonts_get_system_font(FONT_KEY_GOTHIC_14));
		
		menuLayersInitialized = true;
	}
	
	layer_add_child(&window->layer, &menuDescLayer.layer);
}

void SetMenuHighlight(int menuItem, bool selected)
{
	if(selected)
	{
		text_layer_set_text_color(&menuLayers[menuItem], GColorBlack);
		text_layer_set_background_color(&menuLayers[menuItem], GColorWhite);
	}
	else
	{
		text_layer_set_text_color(&menuLayers[menuItem], GColorWhite);
		text_layer_set_background_color(&menuLayers[menuItem], GColorClear);
	}
}

void SetMenuDescription(const char *desc)
{
	text_layer_set_text(&menuDescLayer, desc ? desc : "");	
}

void ShowMenuLayer(int index, const char *text)
{
	text_layer_set_text(&menuLayers[index], text);
	layer_set_hidden(&menuLayers[index].layer, false);
}

void HideMenuLayer(int index)
{
	layer_set_hidden(&menuLayers[index].layer, true);
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
BmpContainer backgroundImage;
static bool backgroundLoaded = false;

void UnloadBackgroundImage(void)
{
	if(!backgroundLoaded)
		return;
	bmp_deinit_container(&backgroundImage);
	backgroundLoaded = false;
}

void RemoveBackgroundImage()
{
	layer_remove_from_parent(&backgroundImage.layer.layer);
}

void LoadBackgroundImage(Window *window, int id)
{
	int resourceId = id;
	if(!backgroundLoaded)
	{
		bmp_init_container(resourceId, &backgroundImage);
		backgroundLoaded = true;
	}
	
	layer_add_child(&window->layer, &backgroundImage.layer.layer);		
}

//******** Main part of the screen *********//
// mainImage will hold the main piece of artwork for each state, including monster art during battle.
// mainTextLayers and mainNumberLayers will be used for various things requiring names and numbers.
//		Monster name/health
//		Floor/Number
//		item name / count

BmpContainer mainImage;
GRect mainFrame = {.origin = {.x = 5, .y = 25}, .size = {.w = 80, .h = 80}};
static bool mainImageLoaded = false;
static int mainImageResourceLoaded = -1;

TextLayer mainTextLayers[MAX_MAIN_TEXT_LAYERS];
GRect mainTextBaseFrame = {.origin = {.x = 6, .y = 7}, .size = {.w = 80, .h = WINDOW_ROW_HEIGHT}};
	
TextLayer mainNumberLayers[MAX_MAIN_TEXT_LAYERS];
GRect mainNumberBaseFrame = {.origin = {.x = 54, .y = 7}, .size = {.w = 30, .h = WINDOW_ROW_HEIGHT}};

bool mainLayersInitialized = false;

void RemoveMainLayers(void)
{
	int i;
	for(i = 0; i < MAX_MAIN_TEXT_LAYERS; ++i)
	{
		layer_remove_from_parent(&mainTextLayers[i].layer);
		layer_remove_from_parent(&mainNumberLayers[i].layer);
	}
}

void InitializeMainLayers(Window *window)
{
	int i;
	for(i = 0; i < MAX_MAIN_TEXT_LAYERS; ++i)
	{
		if(!mainLayersInitialized)
		{
			GRect localFrame = mainTextBaseFrame;
			localFrame.origin.y += i * WINDOW_ROW_HEIGHT;
			InitializeTextLayer(&mainTextLayers[i], localFrame, fonts_get_system_font(FONT_KEY_GOTHIC_14));
			localFrame = mainNumberBaseFrame;
			localFrame.origin.y += i * WINDOW_ROW_HEIGHT;
			InitializeTextLayer(&mainNumberLayers[i], localFrame, fonts_get_system_font(FONT_KEY_GOTHIC_14));
			text_layer_set_text_alignment(&mainNumberLayers[i], GTextAlignmentRight);
		}

		layer_add_child(&window->layer, &mainTextLayers[i].layer);
		layer_set_hidden(&mainTextLayers[i].layer, true);
		layer_add_child(&window->layer, &mainNumberLayers[i].layer);
		layer_set_hidden(&mainNumberLayers[i].layer, true);
	}
	mainLayersInitialized = true;
}

void ShowMainWindowRow(int index, const char *text, const char *number)
{
	if(index >= MAX_MAIN_TEXT_LAYERS)
		return;
		
	text_layer_set_text(&mainTextLayers[index], text);
	layer_set_hidden(&mainTextLayers[index].layer, false);

	text_layer_set_text(&mainNumberLayers[index], number);
	layer_set_hidden(&mainNumberLayers[index].layer, false);
}

void RemoveMainBmpImage(void)
{
	layer_remove_from_parent(&mainImage.layer.layer);
}

void UnloadMainBmpImage(void)
{
	if(!mainImageLoaded)
		return;
	layer_remove_from_parent(&mainImage.layer.layer);
	bmp_deinit_container(&mainImage);
	mainImageLoaded = false;
	mainImageResourceLoaded = -1;
}

void LoadMainBmpImage(Window *window, int id)
{
	int resourceId = id;
	
	if(!window)
		return;
		
	if(mainImageLoaded)
	{
		if(mainImageResourceLoaded == resourceId)
		{
			layer_add_child(&window->layer, &mainImage.layer.layer);
			return; // already loaded the correct one.
		}
		UnloadMainBmpImage();
	}
	bmp_init_container(resourceId, &mainImage);
	layer_set_frame(&mainImage.layer.layer, mainFrame);
	layer_add_child(&window->layer, &mainImage.layer.layer);
	mainImageLoaded = true;
	mainImageResourceLoaded = resourceId;
}

//******* CLOCK *********//

TextLayer clockLayer; // The clock
GRect clockFrame = {.origin = {.x = 85, .y = 127}, .size = {.w = 144-85, .h = 168-127}};
bool clockLayerInitialized = false;

void UpdateClock(void)
{
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

	PblTm currentTime;

	get_time(&currentTime);

	string_format_time(timeText, sizeof(timeText), time_format, &currentTime);

	text_layer_set_text(&clockLayer, timeText);
}

void RemoveClockLayer(void)
{
	layer_remove_from_parent(&clockLayer.layer);
}

void InitializeClockLayer(Window *window)
{
	if(!clockLayerInitialized)
	{
		// Init the text layer used to show the time
		InitializeTextLayer(&clockLayer, clockFrame, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
		clockLayerInitialized = true;
		UpdateClock();
	}
	layer_add_child(&window->layer, &clockLayer.layer);
}

// *********** Level **********//

TextLayer levelLayer;
GRect levelFrame = {.origin = {.x = 10, .y = 127}, .size = {.w = 144-85, .h = 168-127}};
bool levelLayerInitialized = false;

void UpdateLevelLayerText(int level)
{
	static char levelText[] = "00";

	IntToString(levelText, 2, level);
	text_layer_set_text(&levelLayer, levelText);
}

void RemoveLevelLayer(void)
{
	layer_remove_from_parent(&levelLayer.layer);
}

void InitializeLevelLayer(Window *window)
{
	if(!levelLayerInitialized)
	{
		InitializeTextLayer(&levelLayer, levelFrame, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
		levelLayerInitialized = true;
	}
	layer_add_child(&window->layer, &levelLayer.layer);
}

// ********** Health *********//

TextLayer currentHealthLayer;
GRect currentHealthFrame = {.origin = {.x = 42, .y = 126}, .size = {.w = 50, .h = 168-130}};

TextLayer maxHealthLayer;
GRect maxHealthFrame = {.origin = {.x = 42, .y = 143}, .size = {.w = 50, .h = 168-140}};
bool healthLayersInitialized = false;
void UpdateHealthText(int current, int max)
{
	static char currentHealthText[] = "0000"; // Needs to be static because it's used by the system later.
	static char maxHealthText[] = "0000"; // Needs to be static because it's used by the system later.

	IntToString(currentHealthText, 4, current);
	text_layer_set_text(&currentHealthLayer, currentHealthText);

	IntToString(maxHealthText, 4, max);
	text_layer_set_text(&maxHealthLayer, maxHealthText);
}

void RemoveHealthLayer(void)
{
	layer_remove_from_parent(&currentHealthLayer.layer);
	layer_remove_from_parent(&maxHealthLayer.layer);
}

void InitializeHealthLayer(Window *window)
{
	if(!healthLayersInitialized)
	{
		InitializeTextLayer(&currentHealthLayer, currentHealthFrame, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
		InitializeTextLayer(&maxHealthLayer, maxHealthFrame, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
		
		healthLayersInitialized = true;
	}

	layer_add_child(&window->layer, &currentHealthLayer.layer);
	layer_add_child(&window->layer, &maxHealthLayer.layer);
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

void InitializeWindow(Window *window, const char *name, bool animated)
{
	window_init(window, name);
	window_stack_push(window, animated);
	window_set_fullscreen(window, true); // Do I want full screen?
	window_set_background_color(window, GColorBlack);		
}

void InitializeMenuWindow(Window *window, const char *name, bool animated, WindowHandler init, WindowHandler deinit, WindowHandler appear, WindowHandler disappear)
{
	InitializeWindow(window, name, animated);
	window->window_handlers.load = init;
	window->window_handlers.unload = deinit;
	window->window_handlers.appear = appear;
	window->window_handlers.disappear = disappear;
#if OVERRIDE_BACK_BUTTON	
	window->overrides_back_button = true;
#endif
	
	SetMenuClickConfigProvider(window);
}

//************************ Exit confirmation window ****************************//

void ExitWindow_SelectSingleClickHandler(ClickRecognizerRef recognizer, Window *window)
{
	ShowAdventureWindow();
}

void ExitWindowClickConfigProvider(ClickConfig **clickConfig, Window *window)
{
	clickConfig[BUTTON_ID_SELECT]->click.handler = (ClickHandler) ExitWindow_SelectSingleClickHandler;
}

GRect exitFrame = {.origin = {.x = 0, .y = 50}, .size = {.w = 144, .h = 168-50}};
GRect yesFrame = {.origin = {.x = 5, .y = 30}, .size = {.w = 50, .h = 20}};
GRect noFrame = {.origin = {.x = 115, .y = 78}, .size = {.w = 24, .h = 20}};

void InitializeConfirmationWindow(Window *window, TextLayer *exitText, TextLayer *yesText, TextLayer *noText)
{
	InitializeWindow(window, "Exit", true);

	InitializeTextLayer(exitText, exitFrame, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text(exitText, "Exit?");
	text_layer_set_text_alignment(exitText, GTextAlignmentCenter);
	layer_add_child(&window->layer, &exitText->layer);

	InitializeTextLayer(yesText, yesFrame, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text(yesText, "Yes");
	layer_add_child(&window->layer, &yesText->layer);

	InitializeTextLayer(noText, noFrame, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text(noText, "No");
	layer_add_child(&window->layer, &noText->layer);
}

Window confirmationWindow;
static TextLayer exitText;
static TextLayer yesText;
static TextLayer noText;

void InitializeExitConfirmationWindow(void)
{
	InitializeConfirmationWindow(&confirmationWindow, &exitText, &yesText, &noText);
	window_set_click_config_provider(&confirmationWindow, (ClickConfigProvider) ExitWindowClickConfigProvider);
}
