#include "pebble.h"

#include "Logging.h"
#include "Menu.h"
#include "UILayers.h"
#include "Utils.h"

bool MenuEntryIsActive(MenuEntry *entry)
{
	if(!entry)
		return false;

	return entry->text && entry->menuFunction;
}

MenuDefinition *currentMenuDef = NULL;

//*************** Menu Windows ******************//

// I have a set of window structs here so that I can have only as many as needed 
// for the depth of stack, instead of one for each possible menu

typedef struct
{
	Window *window;
	bool inUse;
	MenuDefinition *menu;
} MenuWindow;

MenuWindow menuWindows[MAX_MENU_WINDOWS];

void MenuInit(Window *window)
{
	DEBUG_LOG("Menu init");
}

void MenuDeinit(Window *window)
{
	MenuWindow *menuWindow = window_get_user_data(window);
	if(menuWindow)
	{
		menuWindow->inUse = false;
		menuWindow->menu = NULL;
	}
	window_destroy(window);
}

void MenuAppear(Window *window)
{
	int i;
	bool setSelected = false;
	
	DEBUG_VERBOSE_LOG("MenuAppear");
	
	MenuWindow *menuWindow = window_get_user_data(window);
	if(menuWindow)
	{
		SetCurrentMenu(menuWindow->menu);
	}
	WindowAppear(window);
	if(!currentMenuDef)
	{
		DEBUG_VERBOSE_LOG("No menu");
		HideAllMenuLayers();
		SetMenuDescription(NULL);
		return;
	}

	DEBUG_VERBOSE_LOG("Set highlight and visibility");
	currentMenuDef->currentSelection = -1;
	for(i = 0; i < MAX_MENU_ENTRIES; ++i)
	{
		MenuEntry *entry = &currentMenuDef->menuEntries[i];
		if(MenuEntryIsActive(entry))
		{
			ShowMenuLayer(i, entry->text);
			if(setSelected)
			{
				SetMenuHighlight(i, false);
			}
			else
			{
				SetMenuHighlight(i, true);
				setSelected = true;
				currentMenuDef->currentSelection = i;
				SetMenuDescription(entry->description);
			}
		}
		else
		{
			HideMenuLayer(i);
		}
	}

	if(menuWindow && menuWindow->menu && menuWindow->menu->mainImageId != -1)
	{
		DEBUG_VERBOSE_LOG("Load bitmap");
		LoadMainBmpImage(window, menuWindow->menu->mainImageId, menuWindow->menu->useFloorImage ? menuWindow->menu->floorImageId : -1);
	}
}

void MenuDisappear(Window *window)
{
	WindowDisappear(window);
}

void SetCurrentMenu(MenuDefinition *menuDef)
{	
	DEBUG_VERBOSE_LOG("SetCurrentMenu");
	currentMenuDef = menuDef;
}

void PopMenu(void)
{
	window_stack_pop(currentMenuDef ? currentMenuDef->animated : true);
}

void PushNewMenu(MenuDefinition *menuDef)
{
	SetCurrentMenu(menuDef);
	if(currentMenuDef)
	{
		int i = 0;

		MenuWindow *newMenuWindow = NULL;
		
		for(i = 0; i < MAX_MENU_WINDOWS; ++i)
		{
			if(!menuWindows[i].inUse)
			{	
				newMenuWindow = &menuWindows[i];
				break;
			}
		}
		
		if(!newMenuWindow)
		{
			window_stack_pop_all(true);
			return;
		}

		newMenuWindow->inUse = true;
		newMenuWindow->menu = currentMenuDef;
	
		newMenuWindow->window = InitializeMenuWindow(newMenuWindow, "Menu", currentMenuDef->animated, 
			currentMenuDef->init ? currentMenuDef->init : MenuInit,
			currentMenuDef->deinit ? currentMenuDef->deinit : MenuDeinit,
			currentMenuDef->appear ? currentMenuDef->appear : MenuAppear,
			currentMenuDef->disappear ? currentMenuDef->disappear : MenuDisappear);
	}
}

// ******** CLICK **********//

void SelectSingleClickHandler(ClickRecognizerRef recognizer, Window *window)
{
	DEBUG_LOG("Single Click Select");
	MenuEntry *currentEntry;
	if(!currentMenuDef)
		return;

	currentEntry = &currentMenuDef->menuEntries[currentMenuDef->currentSelection];
	if(!MenuEntryIsActive(currentEntry))
		return;

	currentEntry->menuFunction();
	DEBUG_LOG("End Single Click Select");
}

void IterateMenuEntries(int direction)
{
	int iterator,newSelection;

	if(!currentMenuDef)
		return;

	iterator = newSelection = currentMenuDef->currentSelection;
	
	if(currentMenuDef->currentSelection == -1)
		return;

	do
	{
		MenuEntry *entry;
		iterator += direction;
		if(iterator > MAX_MENU_ENTRIES-1)
			iterator = 0;
		else if(iterator < 0)
			iterator = MAX_MENU_ENTRIES-1;

		entry = &currentMenuDef->menuEntries[iterator];
		if(MenuEntryIsActive(entry))
		{
			newSelection = iterator;
			break;
		}
	}
	while(iterator != currentMenuDef->currentSelection);

	if(newSelection != currentMenuDef->currentSelection)
	{
		MenuEntry *entry = &currentMenuDef->menuEntries[newSelection];
		SetMenuHighlight(currentMenuDef->currentSelection, false);
		SetMenuHighlight(newSelection, true);
		currentMenuDef->currentSelection = newSelection;
		SetMenuDescription(entry->description);
	}
}

void UpSingleClickHandler(ClickRecognizerRef recognizer, Window *window)
{
	IterateMenuEntries(-1);
}

void DownSingleClickHandler(ClickRecognizerRef recognizer, Window *window)
{
	IterateMenuEntries(1);
}

void BackSingleClickHandler(ClickRecognizerRef recognizer, Window *window)
{
	if(currentMenuDef && currentMenuDef->disableBackButton)
		return;
		
	PopMenu();
}

void MenuClickConfigProvider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)SelectSingleClickHandler);
	window_single_click_subscribe(BUTTON_ID_UP,(ClickHandler)UpSingleClickHandler);
	window_single_click_subscribe(BUTTON_ID_DOWN,(ClickHandler)DownSingleClickHandler);

	window_single_click_subscribe(BUTTON_ID_BACK, (ClickHandler)BackSingleClickHandler);
}

void SetMenuClickConfigProvider(Window *window)
{
	window_set_click_config_provider(window, (ClickConfigProvider) MenuClickConfigProvider);
}
