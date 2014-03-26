#pragma once

typedef struct BattleState
{
	uint16_t currentMonsterHealth;
	uint16_t monsterActionBar;
	uint16_t playerActionBar;
} BattleState;
	
void ShowBattleWindow(void);

bool ClosingWhileInBattle(void);
int GetCurrentMonsterHealth(void);

void ResumeBattle(int currentMonster, int currentMonsterHealth);
bool IsBattleForced(void);

void UpdateBattle(void);

extern bool gUpdateBattle;
