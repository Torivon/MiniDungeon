#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

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
	Window window;
	bool inUse;
	MenuDefinition *menu;
} MenuWindow;

MenuWindow menuWindows[MAX_MENU_WINDOWS];

void MenuInit(Window *window)
{
}

void MenuDeinit(Window *window)
{
	MenuWindow *menuWindow = window->user_data;
	if(menuWindow)
	{
		menuWindow->inUse = false;
		menuWindow->menu = NULL;
	}
}

void MenuAppear(Window *window)
{
	int i;
	bool setSelected = false;
	MenuWindow *menuWindow = window->user_data;
	if(menuWindow)
	{
		SetCurrentMenu(menuWindow->menu);
	}	
	WindowAppear(window);
	if(!currentMenuDef)
	{
		HideAllMenuLayers();
		SetMenuDescription(NULL);
		return;
	}

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
		LoadMainBmpImage(window, menuWindow->menu->mainImageId);
	}
}

void MenuDisappear(Window *window)
{
	WindowDisappear(window);
}

void SetCurrentMenu(MenuDefinition *menuDef)
{
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
	
		InitializeMenuWindow(&newMenuWindow->window, "Menu", currentMenuDef->animated, 
			currentMenuDef->init ? currentMenuDef->init : MenuInit,
			currentMenuDef->deinit ? currentMenuDef->deinit : MenuDeinit,
			currentMenuDef->appear ? currentMenuDef->appear : MenuAppear,
			currentMenuDef->disappear ? currentMenuDef->disappear : MenuDisappear);

		newMenuWindow->window.user_data = newMenuWindow;
	}
}

// ******** CLICK **********//

void SelectSingleClickHandler(ClickRecognizerRef recognizer, Window *window)
{
	MenuEntry *currentEntry;
	if(!currentMenuDef)
		return;

	currentEntry = &currentMenuDef->menuEntries[currentMenuDef->currentSelection];
	if(!MenuEntryIsActive(currentEntry))
		return;

	currentEntry->menuFunction();
}

void IterateMenuEntries(int direction, int limit)
{
	int iterator,newSelection;

	if(!currentMenuDef)
		return;

	iterator = newSelection = currentMenuDef->currentSelection;
  
	while(iterator != limit)
	{
		MenuEntry *entry;
		iterator += direction;
		entry = &currentMenuDef->menuEntries[iterator];
		if(MenuEntryIsActive(entry))
		{
			newSelection = iterator;
			break;
		}
	}

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
	IterateMenuEntries(-1, 0);
}

void DownSingleClickHandler(ClickRecognizerRef recognizer, Window *window)
{
	IterateMenuEntries(1, MAX_MENU_ENTRIES-1);
}

#if OVERRIDE_BACK_BUTTON
void BackSingleClickHandler(ClickRecognizerRef recognizer, Window *window)
{
	if(currentMenuDef && currentMenuDef->disableBackButton)
		return;
		
	PopMenu();
}

void BackLongClickHandler(ClickRecognizerRef recognizer, Window *window)
{
	window_stack_pop_all(true);
}
#endif

void MenuClickConfigProvider(ClickConfig **clickConfig, Window *window)
{
	(void)window;

	clickConfig[BUTTON_ID_SELECT]->click.handler = (ClickHandler) SelectSingleClickHandler;

	clickConfig[BUTTON_ID_UP]->click.handler = (ClickHandler) UpSingleClickHandler;

	clickConfig[BUTTON_ID_DOWN]->click.handler = (ClickHandler) DownSingleClickHandler;
	
#if OVERRIDE_BACK_BUTTON
	clickConfig[BUTTON_ID_BACK]->click.handler = (ClickHandler) BackSingleClickHandler;
	
	clickConfig[BUTTON_ID_BACK]->long_click.handler = (ClickHandler) BackLongClickHandler;
#endif
}

void SetMenuClickConfigProvider(Window *window)
{
	window_set_click_config_provider(window, (ClickConfigProvider) MenuClickConfigProvider);
}
