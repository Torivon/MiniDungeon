#include "pebble.h"

#include "Logging.h"
#include "Monsters.h"
#include "Utils.h"

int mostRecentMonster = -1;

int GetMostRecentMonster(void)
{
	return mostRecentMonster;
}
	
MonsterDef Rat = 
{
	.name = "Rat",
	.imageId = RESOURCE_ID_IMAGE_RAT,
	.imageBWId = RESOURCE_ID_IMAGE_RAT_OLD,
	.extraFireDefenseMultiplier = 100,
	.extraIceDefenseMultiplier = 100,
	.extraLightningDefenseMultiplier = 100,
	.powerLevel = 0,
	.healthLevel = 0,
	.defenseLevel = 0,
	.magicDefenseLevel = 0,
	.allowPhysicalAttack = true,
	.goldScale = 0
};

MonsterDef Goblin = 
{
	.name = "Goblin",
	.imageId = RESOURCE_ID_IMAGE_GOBLIN,
	.imageBWId = RESOURCE_ID_IMAGE_GOBLIN_OLD,
	.extraFireDefenseMultiplier = 100,
	.extraIceDefenseMultiplier = 200,
	.extraLightningDefenseMultiplier = 100,
	.powerLevel = 1,
	.healthLevel = 1,
	.defenseLevel = 1,
	.magicDefenseLevel = 0,
	.allowPhysicalAttack = true,
	.goldScale = 1
};

MonsterDef Wizard = 
{
	.name = "Wizard",
	.imageId = RESOURCE_ID_IMAGE_WIZARD,
	.imageBWId = RESOURCE_ID_IMAGE_WIZARD_OLD,
	.extraFireDefenseMultiplier = 100,
	.extraIceDefenseMultiplier = 100,
	.extraLightningDefenseMultiplier = 100,
	.powerLevel = 1,
	.healthLevel = 1,
	.defenseLevel = 1,
	.magicDefenseLevel = 3,
	.allowMagicAttack = true,
	.goldScale = 2
};

MonsterDef Zombie = 
{
	.name = "Zombie",
	.imageId = RESOURCE_ID_IMAGE_ZOMBIE,
	.imageBWId = RESOURCE_ID_IMAGE_ZOMBIE_OLD,
	.extraFireDefenseMultiplier = 300,
	.extraIceDefenseMultiplier = 100,
	.extraLightningDefenseMultiplier = 100,
	.powerLevel = 2,
	.healthLevel = 1,
	.defenseLevel = 1,
	.magicDefenseLevel = 1,
	.allowPhysicalAttack = true,
	.goldScale = 1
};

MonsterDef Turtle = 
{
	.name = "Turtle",
	.imageId = RESOURCE_ID_IMAGE_TURTLE,
	.imageBWId = RESOURCE_ID_IMAGE_TURTLE_OLD,
	.extraFireDefenseMultiplier = 100,
	.extraIceDefenseMultiplier = 100,
	.extraLightningDefenseMultiplier = 600,
	.powerLevel = 1,
	.healthLevel = 2,
	.defenseLevel = 3,
	.magicDefenseLevel = 1,
	.allowPhysicalAttack = true,
	.goldScale = 0	
};

MonsterDef Lich = 
{
	.name = "Lich",
	.imageId = RESOURCE_ID_IMAGE_LICH,
	.imageBWId = RESOURCE_ID_IMAGE_LICH_OLD,
	.extraFireDefenseMultiplier = 100,
	.extraIceDefenseMultiplier = 600,
	.extraLightningDefenseMultiplier = 100,
	.powerLevel = 2,
	.healthLevel = 2,
	.defenseLevel = 3,
	.magicDefenseLevel = 1,
	.allowMagicAttack = true,
	.goldScale = 3
};

MonsterDef Dragon = 
{
	.name = "Dragon",
	.imageId = RESOURCE_ID_IMAGE_DRAGON,
	.imageBWId = RESOURCE_ID_IMAGE_DRAGON_OLD,
	.extraFireDefenseMultiplier = 100,
	.extraIceDefenseMultiplier = 100,
	.extraLightningDefenseMultiplier = 100,
	.powerLevel = 2,
	.healthLevel = 2,
	.defenseLevel = 2,
	.magicDefenseLevel = 2,
	.allowPhysicalAttack = true,
	.allowMagicAttack = true,
	.goldScale = 10	
};

int defenseLevelMap[] = 
{
	0,
	5,
	10,
	20
};

int GetMonsterDefense(int defenseLevel)
{
	if(defenseLevel < 4)
		return defenseLevelMap[defenseLevel];
	
	return 0;
}

int ScaleMonsterHealth(MonsterDef *monster, int baseHealth)
{
	switch(monster->healthLevel)
	{
		case 0:
			return baseHealth/2;
		case 2:
			return baseHealth*2;
		case 1:
		default:
			return baseHealth;
	}
}

int powerLevelMap[] = 
{
	10,
	7,
	4
};

int GetMonsterPowerDivisor(int powerLevel)
{
	if(powerLevel < 3)
		return powerLevelMap[powerLevel];
		
	return 10;
}

MonsterDef *randomMonsterMap[] = 
{
	&Dragon,
	&Rat,
	&Goblin,
	&Wizard,
	&Zombie,
	&Turtle,
	&Lich,
};

int MonsterTypeCount(void)
{
	int count = sizeof(randomMonsterMap) / sizeof(MonsterDef*);
	return count;
}

MonsterDef *GetRandomMonster(int floor)
{
	int limit;
	if(floor >= 20)
    {
        mostRecentMonster = 0;
    }
    else
    {
        limit = floor >= 12 ? 6 : (floor + 1) / 2;
        
        mostRecentMonster = Random(limit);
    }
	return randomMonsterMap[mostRecentMonster];
}

MonsterDef *GetFixedMonster(int index)
{
	if(index >= 0 && index < MonsterTypeCount())
	{
		mostRecentMonster = index;
		return randomMonsterMap[index];
	}
	
	ERROR_LOG("Monster type %d is out of range.", index);
	return NULL;
}
