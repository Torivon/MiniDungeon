#pragma once

void ToggleVibration(void);
const char *UpdateVibrationText(void);

bool AdventureWindowIsVisible(void);

void ShowAdventureWindow(void);

void SetUpdateDelay(void);
void UpdateAdventure(void);

void RefreshAdventure(void);
void ResetGame(void);

extern bool gUpdateAdventure;
