#pragma once

void ShowBattleWindow(void);

bool ClosingWhileInBattle(void);
int GetCurrentMonsterHealth(void);

void ResumeBattle(int currentMonster, int currentMonsterHealth);
bool IsBattleForced(void);
