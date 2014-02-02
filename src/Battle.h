#pragma once

void ShowBattleWindow(void);

bool ClosingWhileInBattle(void);
int GetCurrentMonsterHealth(void);

void IncrementFloor(void);
int GetCurrentFloor(void);
void SetCurrentFloor(int);
void ResetFloor(void);

void ResumeBattle(int currentMonster, int currentMonsterHealth);
bool IsBattleForced(void);
