#pragma once

#include "MiniDungeon.h"

typedef void (*MenuFunction)(void);

typedef struct
{
	const char *text;
	const char *description;
	MenuFunction menuFunction;
} MenuEntry;

typedef struct
{
	MenuEntry menuEntries[MAX_MENU_ENTRIES];
	WindowHandler init;
	WindowHandler deinit;
	WindowHandler appear;
	WindowHandler disappear;
	int currentSelection : 4;
	int animated : 1;
	int disableBackButton : 1;
	int mainImageId : 6;
	int floorImageId : 6;
	int useFloorImage : 1;
} MenuDefinition;

void SetCurrentMenu(MenuDefinition *menuDef);

void SetMenuClickConfigProvider(Window *window);

// Useful callback while building a new menu
void DoNothing(void);

void MenuInit(Window *window);
void MenuDeinit(Window *window);
void MenuAppear(Window *window);
void MenuDisappear(Window *window);
void PushNewMenu(MenuDefinition *menuDef);
void PopMenu(void);
