#pragma once

#define MAX_MAIN_TEXT_LAYERS 6

void SetMenuHighlight(int menuItem, bool selected);
void SetMenuDescription(const char *desc);
void ShowMenuLayer(int index, const char *text);
void HideMenuLayer(int index);
void HideAllMenuLayers(void);

void LoadMainBmpImage(Window *window, int resourceId, int backgroundId);
void UnloadMainBmpImage(void);
void UnloadBackgroundImage(void);
void UnloadTextLayers(void);

void ShowMainWindowRow(int index, const char *text, const char *number);

void UpdateClock(void);
void UpdateLevelLayerText(int level);

void UpdateHealthText(int currentHealth, int maxHealth);

TextLayer * InitializeTextLayer(GRect frame, GFont font);

Window * InitializeWindow(const char *name, bool animated);
Window * InitializeMenuWindow(void *menuWindow, const char *name, bool animated, WindowHandler init, WindowHandler deinit, WindowHandler appear, WindowHandler disappear, ClickConfigProvider clicker);

void WindowAppear(Window *window);
void WindowDisappear(Window *window);
