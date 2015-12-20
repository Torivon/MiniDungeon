#include "pebble.h"

#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "Menu.h"
#include "MiniDungeon.h"
#include "Monsters.h"
#include "OptionsMenu.h"
#include "Adventure.h"
#include "UILayers.h"
#include "Utils.h"

static bool battleCleanExit = true;

typedef void (*BattleAfterExitCallback)(void);

void RemoveConfirmationWindow(void);

void CloseBattleWindow(void)
{
	INFO_LOG("Ending battle.");
	battleCleanExit = true;
	PopMenu();
}

bool ClosingWhileInBattle(void)
{
	return !battleCleanExit;
}

static int currentFloor = 1;

void ResetFloor(void)
{
	currentFloor = 1;
}

void IncrementFloor(void)
{
	++currentFloor;
}

int GetCurrentFloor(void)
{
	return currentFloor;
}

void SetCurrentFloor(int newFloor)
{
	currentFloor = newFloor;
}

MonsterDef *currentMonster;
int currentMonsterHealth;

int GetCurrentMonsterHealth(void)
{
	return currentMonsterHealth;
}

int ApplyDefense(int baseDamage, int defense)
{
	int multiplier;
	int totalDamage;
	if(defense < 5)
		multiplier = (-30*defense)/5 + 100;
	else if(defense < 10)
		multiplier = (-20*(defense-5))/5 + 70;
	else if(defense < 15)
		multiplier = (-15*(defense-10))/5 + 50;
	else
		multiplier = (-10*(defense-15))/5 + 35;
	
	totalDamage = baseDamage * multiplier / 100;
	
	if(totalDamage <= 0)
		totalDamage = 1;
	
	return totalDamage;
}

void MonsterAttack(void)
{
	int baseDamage;
	int damageToDeal;
	bool useMagicAttack = (currentMonster->allowMagicAttack && currentMonster->allowPhysicalAttack) ? Random(2) - 1 : currentMonster->allowMagicAttack;
	baseDamage = ComputePlayerHealth(currentFloor)/GetMonsterPowerDivisor(currentMonster->powerLevel);
	damageToDeal = ApplyDefense(baseDamage, useMagicAttack ? GetCharacter()->stats.magicDefense : GetCharacter()->stats.defense);

	if(DealPlayerDamage(damageToDeal))
	{
		CloseBattleWindow();
		ShowEndWindow();
	}
}

const char  *UpdateMonsterHealthText(void)
{
	static char monsterHealthText[] = "00000"; // Needs to be static because it's used by the system later.

	IntToString(monsterHealthText, 5, currentMonster ? currentMonsterHealth : 0);
	return monsterHealthText;
}

void BattleUpdate(void)
{
	if(currentMonsterHealth <= 0)
	{
		INFO_LOG("Player wins.");
		CloseBattleWindow();
		GrantGold(currentFloor * currentMonster->goldScale);
		if(currentFloor >= 20)
		{
			ShowEndWindow();
		}
		else if(GrantExperience(currentFloor))
		{
			LevelUp();
		}
		return;
	}

	MonsterAttack();
}

void DamageCurrentMonster(int strength, int level, int defense, int baseMultiplier, int bonusMultiplier)
{
	int base = baseMultiplier * (2 + strength + 4*((strength * level)/8 + (strength * strength)/64 + (level * level)/64));
	int damageToDeal = ApplyDefense(base, defense);

	damageToDeal = damageToDeal * bonusMultiplier / 100;
	
	currentMonsterHealth -= damageToDeal;
	ShowMainWindowRow(0, currentMonster->name, UpdateMonsterHealthText());
	BattleUpdate();
}

void AttackCurrentMonster(void)
{
	DamageCurrentMonster(GetCharacter()->stats.strength, GetCharacter()->level, GetMonsterDefense(currentMonster->defenseLevel), 1, 100);
}

void UseFireOnCurrentMonster(void)
{
	DamageCurrentMonster(GetCharacter()->stats.magic, GetCharacter()->level, GetMonsterDefense(currentMonster->magicDefenseLevel), 3, currentMonster->extraFireDefenseMultiplier);
}

void UseIceOnCurrentMonster(void)
{
	DamageCurrentMonster(GetCharacter()->stats.magic, GetCharacter()->level, GetMonsterDefense(currentMonster->magicDefenseLevel), 3, currentMonster->extraIceDefenseMultiplier);
}

void UseLightningOnCurrentMonster(void)
{
	DamageCurrentMonster(GetCharacter()->stats.magic, GetCharacter()->level, GetMonsterDefense(currentMonster->magicDefenseLevel), 3, currentMonster->extraLightningDefenseMultiplier);
}

void AttemptToRun(void)
{
	int runCheck = Random(3);
			
	if(runCheck == 3 && currentFloor < 20) // if floor is >= 20 you are fighting the dragon
	{
		INFO_LOG("Player runs.");
		CloseBattleWindow();
		IncrementEscapes();
		return;
	}

	BattleUpdate();
}

void ActivateFireScroll(void)
{
	if(AttemptToConsumeFireScroll())
	{
		PopMenu();
		UseFireOnCurrentMonster();
	}
}

void ActivateIceScroll(void)
{
	if(AttemptToConsumeIceScroll())
	{
		PopMenu();
		UseIceOnCurrentMonster();
	}
}

void ActivateLightningScroll(void)
{
	if(AttemptToConsumeLightningScroll())
	{
		PopMenu();
		UseLightningOnCurrentMonster();
	}
}

void ShowItemBattleMenu(void);

void BattleWindowAppear(Window *window);
void BattleWindowInit(Window *window);

MenuDefinition battleMainMenuDef = 
{
	.menuEntries = 
	{
		{"Attack", "Attack with your sword.", AttackCurrentMonster},
		{"Item", "Use an item", ShowItemBattleMenu},
		{NULL, NULL, NULL},
		{"Progress", "Character advancement", ShowProgressMenu},
		{NULL, NULL, NULL},
		{"Run", "Try to run away", AttemptToRun},
	},
	.init = BattleWindowInit,
	.appear = BattleWindowAppear,
	.disableBackButton = true,
	.mainImageId = -1
};

void ShowMainBattleMenu(void)
{
	INFO_LOG("Entering battle.");
	PushNewMenu(&battleMainMenuDef);
}

void BattleWindowAppear(Window *window)
{
	MenuAppear(window);
	ShowMainWindowRow(0, currentMonster->name, UpdateMonsterHealthText());
}

void ActivateCombatPotion(void)
{
	if(AttemptToUsePotion())
	{
		PopMenu();
		BattleUpdate();
	}
}

void ActivateCombatFullPotion(void)
{
	if(AttemptToUseFullPotion())
	{
		PopMenu();
		BattleUpdate();
	}
}

void ItemBattleMenuAppear(Window *window);

MenuDefinition itemBattleMenuDef = 
{
	.menuEntries = 
	{
		{"Quit", "Return to battle menu", PopMenu},
		{"Drink", "Heal 50% of your health", ActivateCombatPotion},
		{"Drink", "Heal 100% of your health", ActivateCombatFullPotion},
		{"Throw", "Deal fire damage", ActivateFireScroll},
		{"Throw", "Deal ice damage", ActivateIceScroll},
		{"Throw", "Deal lightning damage", ActivateLightningScroll}
	},
	.appear = ItemBattleMenuAppear,
	.mainImageId = -1
};

void ItemBattleMenuAppear(Window *window)
{
	MenuAppear(window);
	ShowAllItemCounts();
}

void ShowItemBattleMenu(void)
{
	PushNewMenu(&itemBattleMenuDef);
}

int ComputeMonsterHealth(int level)
{
	int baseHealth = 20 + ((level-1)*(level)/2) + ((level-1)*(level)*(level+1)/(6*2));
	return ScaleMonsterHealth(currentMonster, baseHealth);
}

static bool forcedBattle = false;
static int forcedBattleMonsterType = -1;
static int forcedBattleMonsterHealth = 0;
void ResumeBattle(int currentMonster, int currentMonsterHealth)
{
	if(currentMonster >= 0 && currentMonster < MonsterTypeCount() && currentMonsterHealth > 0)
	{
		forcedBattle = true;
		forcedBattleMonsterType = currentMonster;
		forcedBattleMonsterHealth = currentMonsterHealth;
	}
}

bool IsBattleForced(void)
{
	return forcedBattle;
}

void BattleInit(void)
{
	currentMonster = NULL;
	if(forcedBattle)
	{
		DEBUG_LOG("Starting forced battle with (%d,%d)", forcedBattleMonsterType, forcedBattleMonsterHealth);
		currentMonster = GetFixedMonster(forcedBattleMonsterType);
		currentMonsterHealth = forcedBattleMonsterHealth;	
		forcedBattle = false;
	}
	
	if(!currentMonster)
	{
		currentMonster = GetRandomMonster(currentFloor);
		currentMonsterHealth = ComputeMonsterHealth(currentFloor);
	}
	
	if(GetUseOldAssets())
	{
		battleMainMenuDef.mainImageId = currentMonster->imageBWId;
	}
	else
	{
#if defined(PBL_COLOR)
		battleMainMenuDef.mainImageId = currentMonster->imageId;
		battleMainMenuDef.floorImageId = RESOURCE_ID_IMAGE_BATTLE_FLOOR;
		battleMainMenuDef.useFloorImage = true;
#else
		battleMainMenuDef.mainImageId = currentMonster->imageId;
#endif
	}
	battleCleanExit = false;
}

void BattleWindowInit(Window *window)
{
	MenuInit(window);
	BattleInit();
}

void ShowBattleWindow(void)
{
	ShowMainBattleMenu();
}
