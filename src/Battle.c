#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "MainMenu.h"
#include "Menu.h"
#include "MiniDungeon.h"
#include "Monsters.h"
#include "OptionsMenu.h"
#include "Story.h"
#include "UILayers.h"
#include "Utils.h"

bool gUpdateBattle = false;
static BattleState gBattleState = {0};

static bool battleCleanExit = true;

typedef void (*BattleAfterExitCallback)(void);

typedef void (*BattleActionCommand)(CharacterData *character, MonsterDef *monster);

void CloseBattleWindow(void)
{
	INFO_LOG("Ending battle.");
	battleCleanExit = true;
	gUpdateBattle = false;
	PopMenu();
}

bool ClosingWhileInBattle(void)
{
	return !battleCleanExit;
}

MonsterDef *currentMonster;

int GetCurrentMonsterHealth(void)
{
	return gBattleState.currentMonsterHealth;
}

bool ReadyToAct(uint16_t actionBar)
{
	return actionBar >= 100;
}

bool PlayerReadyToAct(void)
{
	return ReadyToAct(gBattleState.playerActionBar);
}

bool MonsterReadyToAct(void)
{
	return ReadyToAct(gBattleState.monsterActionBar);
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

void MonsterAttack(CharacterData *character, MonsterDef *monster)
{
	int baseDamage;
	int damageToDeal;
	bool useMagicAttack = (monster->allowMagicAttack && monster->allowPhysicalAttack) ? Random(2) : monster->allowMagicAttack;
	baseDamage = ComputePlayerHealth(GetCurrentBaseLevel())/GetMonsterPowerDivisor(monster->powerLevel);
	damageToDeal = ApplyDefense(baseDamage, useMagicAttack ? character->stats.magicDefense : character->stats.defense);

	DealPlayerDamage(damageToDeal);
}

const char  *UpdateMonsterHealthText(void)
{
	static char monsterHealthText[] = "00000"; // Needs to be static because it's used by the system later.

	IntToString(monsterHealthText, 5, currentMonster ? gBattleState.currentMonsterHealth : 0);
	return monsterHealthText;
}

// returns true if battle should end
void PerformPlayerAction(BattleActionCommand playerAction)
{
	if(playerAction)
	{
		playerAction(GetCharacter(), currentMonster);
	}

	gBattleState.playerActionBar = 0;
	RefreshMenuAppearance();
}

// returns true if the battle should end
void PerformMonsterAction(BattleActionCommand monsterAction)
{
	if(monsterAction)
	{
		monsterAction(GetCharacter(), currentMonster);
	}

	gBattleState.monsterActionBar = 0;
}

void DamageCurrentMonster(int strength, int level, int defense, int baseMultiplier, int bonusMultiplier)
{
	int base = baseMultiplier * (2 + strength + 4*((strength * level)/8 + (strength * strength)/64 + (level * level)/64));
	int damageToDeal = ApplyDefense(base, defense);

	damageToDeal = damageToDeal * bonusMultiplier / 100;
	
	if(damageToDeal > gBattleState.currentMonsterHealth)
		gBattleState.currentMonsterHealth = 0;
	else
		gBattleState.currentMonsterHealth -= damageToDeal;
	ShowMainWindowRow(0, currentMonster->name, UpdateMonsterHealthText());
}

void AttackCurrentMonster(CharacterData *character, MonsterDef *monster)
{
	DamageCurrentMonster(character->stats.strength, character->level, GetMonsterDefense(monster->defenseLevel), 1, 100);
}

void QueuePlayerAction_Attack(void)
{
	PerformPlayerAction(AttackCurrentMonster);
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

void AttemptToRun(CharacterData *character, MonsterDef *monster)
{
	int runCheck = Random(3) + 1;
			
	if(runCheck == 3 && !currentMonster->preventRun)
	{
		INFO_LOG("Player runs.");
		CloseBattleWindow();
		IncrementEscapes();
	}
}

void QueuePlayerAction_Run(void)
{
	PerformPlayerAction(AttemptToRun);
}

#if ENABLE_ITEMS

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

void ActivateCombatPotion(void)
{
	if(AttemptToUsePotion())
	{
		PopMenu();
	}
}

void ActivateCombatFullPotion(void)
{
	if(AttemptToUseFullPotion())
	{
		PopMenu();
	}
}

void ItemBattleMenuAppear(Window *window);

MenuDefinition itemBattleMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Quit", .description = "Return to battle menu", .menuFunction = PopMenu},
		{.text = "Drink", .description = "Heal 50% of your health", .menuFunction = ActivateCombatPotion},
		{.text = "Drink", .description = "Heal 100% of your health", .menuFunction = ActivateCombatFullPotion},
		{.text = "Throw", .description = "Deal fire damage", .menuFunction = ActivateFireScroll},
		{.text = "Throw", .description = "Deal ice damage", .menuFunction = ActivateIceScroll},
		{.text = "Throw", .description = "Deal lightning damage", .menuFunction = ActivateLightningScroll}
	},
	.appear = ItemBattleMenuAppear,
	.mainImageId = -1,
	.floorImageId = -1
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

#endif

void BattleWindowAppear(Window *window);
void BattleWindowDisappear(Window *window);
void BattleWindowInit(Window *window);

const char *AttackMenuText(void)
{
	if(PlayerReadyToAct())
	{
		return "Attack";
	}
	
	return NULL;
}

const char *AttackMenuDescription(void)
{
	return "Attack with your sword.";
}

const char *ProgressMenuText(void)
{
	if(PlayerReadyToAct())
	{
		return "Progress";
	}
	
	return NULL;	
}

const char *ProgressMenuDescription(void)
{
	return "Character advancement";
}

const char *RunMenuText(void)
{
	if(PlayerReadyToAct())
	{
		return "Run";
	}
	
	return NULL;	
}

const char *RunMenuDescription(void)
{
	return "Try to run away";
}

MenuDefinition battleMainMenuDef = 
{
	.menuEntries = 
	{
		{.useFunctions = true, .textFunction = AttackMenuText, .descriptionFunction = AttackMenuDescription, .menuFunction = QueuePlayerAction_Attack},
#if ENABLE_ITEMS
		{.text = "Item", .description = "Use an item", .menuFunction = ShowItemBattleMenu},
#else
		{.text = NULL, .description = NULL, .menuFunction = NULL},
#endif
		{.text = NULL, .description = NULL, .menuFunction = NULL},
		{.useFunctions = true, .textFunction = ProgressMenuText, .descriptionFunction = ProgressMenuDescription, .menuFunction = ShowProgressMenu},
		{.text = NULL, .description = NULL, .menuFunction = NULL},
		{.useFunctions = true, .textFunction = RunMenuText, .descriptionFunction = RunMenuDescription, .menuFunction = QueuePlayerAction_Run},
	},
	.init = BattleWindowInit,
	.appear = BattleWindowAppear,
	.disappear = BattleWindowDisappear,
	.disableBackButton = true,
	.mainImageId = -1,
	.floorImageId = -1
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
	gUpdateBattle = true;
}

void BattleWindowDisappear(Window *window)
{
	MenuDisappear(window);
	gUpdateBattle = false;
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
	if(currentMonster >= 0 && currentMonsterHealth > 0)
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
		gBattleState.currentMonsterHealth = forcedBattleMonsterHealth;	
		forcedBattle = false;
	}
	
	if(!currentMonster)
	{
		currentMonster = GetRandomMonster();
		gBattleState.currentMonsterHealth = ComputeMonsterHealth(GetCurrentBaseLevel());
	}
	
	battleMainMenuDef.mainImageId = currentMonster->imageId;
#if defined(PBL_COLOR)
	battleMainMenuDef.floorImageId = RESOURCE_ID_IMAGE_BATTLE_FLOOR;
#endif
	battleCleanExit = false;
}

void BattleWindowInit(Window *window)
{
	MenuInit(window);
	BattleInit();
}

void ShowBattleWindow(void)
{
	if(CurrentLocationAllowsCombat())
		ShowMainBattleMenu();
}

void UpdateBattle(void)
{
	if(!gUpdateBattle)
		return;
	
	if(PlayerIsDead())
	{
		CloseBattleWindow();
		ShowEndWindow();
	}
	
	if(gBattleState.currentMonsterHealth <= 0)
	{
		INFO_LOG("Player wins.");
		CloseBattleWindow();
		GrantGold(GetCurrentBaseLevel() * currentMonster->goldScale);
		if(GrantExperience(GetCurrentBaseLevel()))
		{
			LevelUp();
		}
		return;
	}

	// If the enemy gets to act out of turn, remove this
	if(PlayerReadyToAct())
		return;
	
	// TODO: Speed should be determined by player/monster stats
	gBattleState.playerActionBar += 10;
	gBattleState.monsterActionBar += 6;

	// If the monster is ready to act and his bar is higher than the player's, go.
	// Otherwise, the player gets to act, then the monster will get to go next time around.
	if(MonsterReadyToAct() && gBattleState.monsterActionBar > gBattleState.playerActionBar)
	{
		PerformMonsterAction(MonsterAttack);
	}
	
	if(PlayerReadyToAct())
	{
		if(GetVibration())
			vibes_short_pulse();
		RefreshMenuAppearance();
	}
}
