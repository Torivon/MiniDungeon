#pragma once

#define MAX_MAIN_TEXT_LAYERS 6

void SetMenuHighlight(int menuItem, bool selected);
void SetMenuDescription(const char *desc);
void ShowMenuLayer(int index, const char *text);
void HideMenuLayer(int index);
void HideAllMenuLayers(void);

void LoadMainBmpImage(Window *window, int resourceId);
void UnloadMainBmpImage(void);
void UnloadBackgroundImage(void);

void ShowMainWindowRow(int index, const char *text, const char *number);

void UpdateClock(void);
void UpdateLevelLayerText(int level);

void UpdateHealthText(int currentHealth, int maxHealth);

void InitializeTextLayer(TextLayer *textLayer, GRect frame, GFont font);

void InitializeConfirmationWindow(Window *window, TextLayer *exitText, TextLayer *yesText, TextLayer *noText);
void InitializeExitConfirmationWindow(void);
void InitializeWindow(Window *window, const char *name, bool animated);
void InitializeMenuWindow(Window *window, const char *name, bool animated, WindowHandler init, WindowHandler deinit, WindowHandler appear, WindowHandler disappear);

void WindowAppear(Window *window);
void WindowDisappear(Window *window);
