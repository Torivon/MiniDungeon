#pragma once
#include "MiniDungeon.h"
void ShowMainMenu(void);
void ShowTestMenu(void);
void ShowTestMenu2(void);

#if ALLOW_TEST_MENU
void UpdateMinFreeMemory();
#else
#define UpdateMinFreeMemory()
#endif
