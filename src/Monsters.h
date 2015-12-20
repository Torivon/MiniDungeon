#pragma once

#include "Character.h"

typedef struct
{
	const char *name;
	int imageId : 6;
	int imageBWId : 6;
	int extraFireDefenseMultiplier : 11;
	int extraIceDefenseMultiplier : 11;
	int extraLightningDefenseMultiplier : 11;
	bool allowMagicAttack : 1;
	bool allowPhysicalAttack : 1;
	int powerLevel : 3;
	int healthLevel : 3;
	int defenseLevel : 3;
	int magicDefenseLevel : 3;
	int goldScale : 5;
} MonsterDef;

int GetMostRecentMonster(void);

int GetMonsterDefense(int defenseLevel);
int ScaleMonsterHealth(MonsterDef *monster, int baseHealth);
int GetMonsterPowerDivisor(int powerLevel);

int MonsterTypeCount(void);
MonsterDef *GetRandomMonster(int floor);
MonsterDef *GetFixedMonster(int index);
