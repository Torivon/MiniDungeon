#pragma once

bool GetVibration(void);
void SetVibration(bool enable);
bool GetFastMode(void);
void SetFastMode(bool enable);
bool GetWorkerApp(void);
void SetWorkerApp(bool enable);
bool GetWorkerCanLaunch(void);
void SetWorkerCanLaunch(bool enable);

void DrawOptionsMenu(void);

void ShowOptionsMenu(void);

bool OptionsMenuIsVisible(void);

bool GetUseOldAssets(void);
void SetUseOldAssets(bool enable);
