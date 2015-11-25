#pragma once

#include "MiniDungeon.h"

typedef void (*MenuFunction)(void);
typedef const char *(*MenuTextFunction)(void);

typedef struct
{
	bool useFunctions;
	union
	{
		const char *text;
		MenuTextFunction textFunction;
	};
	union
	{
		const char *description;
		MenuTextFunction descriptionFunction;
	};
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
	int mainImageId; //resourceid
	int floorImageId; //resourceid For now, the floor image will not be drawn if there is no main image.
} MenuDefinition;

void SetCurrentMenu(MenuDefinition *menuDef);

void SetMenuClickConfigProvider(Window *window);

// Useful callback while building a new menu
void DoNothing(void);

void RefreshMenuAppearance(void);

void MenuInit(Window *window);
void MenuDeinit(Window *window);
void MenuAppear(Window *window);
void MenuDisappear(Window *window);
void PushNewMenu(MenuDefinition *menuDef);
void PopMenu(void);
