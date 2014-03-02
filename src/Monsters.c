#include "pebble.h"

#include "Logging.h"
#include "Monsters.h"
#include "Utils.h"

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
