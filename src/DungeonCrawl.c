#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "DungeonCrawl.h"
#include "Items.h"
#include "Location.h"
#include "LocationInternal.h"
#include "Logging.h"
#include "MiniDungeon.h"
#include "Monsters.h"
#include "Story.h"
	
#if INCLUDE_DUNGEON_CRAWL

enum
{
	DUNGEON_ENTRANCE = 0,
	DUNGEON_FULL,
	DUNGEON_DRAGONS_ROOM,
	DUNGEON_TREASURE_ROOM,
};

enum
{
	DUNGEON_RAT = 0,
	DUNGEON_GOBLIN,
	DUNGEON_WIZARD,
	DUNGEON_ZOMBIE,
	DUNGEON_TURTLE,
	DUNGEON_LICH,
	DUNGEON_DRAGON,
};

static PathClass DungeonFloorClass =
{
	.numberOfMonsters = 6,
	.monsters = {DUNGEON_RAT, DUNGEON_GOBLIN, DUNGEON_WIZARD, DUNGEON_ZOMBIE, DUNGEON_TURTLE, DUNGEON_LICH},
	.monsterUnlockLevel = 15,
	.encounterChance = 20,
	.numberOfBackgroundImages = 3,
	.backgroundImages = {RESOURCE_ID_IMAGE_DUNGEONSTRAIGHT, RESOURCE_ID_IMAGE_DUNGEONLEFT, RESOURCE_ID_IMAGE_DUNGEONRIGHT},	
};

static FixedClass DungeonStairsClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_NEWFLOOR,
	.allowShop = true,
};

static FixedClass DragonsRoomClass =
{
	.monster = DUNGEON_DRAGON,
	.backgroundImage = RESOURCE_ID_IMAGE_DUNGEONSTRAIGHT,
};

static FixedClass TreasureRoomClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_DUNGEONDEADEND,
};

static DungeonClass FullDungeonClass =
{
	.numberOfFloors = 20,
	.levelIncreaseRate = 1,
	.pathclass = &DungeonFloorClass,
	.fixedclass = &DungeonStairsClass,
};

static MonsterDef monsters[] =
{
	{
		.name = "Rat",
		.imageId = RESOURCE_ID_IMAGE_RAT,
		.extraFireDefenseMultiplier = 100,
		.extraIceDefenseMultiplier = 100,
		.extraLightningDefenseMultiplier = 100,
		.powerLevel = 0,
		.healthLevel = 0,
		.defenseLevel = 0,
		.magicDefenseLevel = 0,
		.allowPhysicalAttack = true,
		.goldScale = 1
	},

	{
		.name = "Goblin",
		.imageId = RESOURCE_ID_IMAGE_GOBLIN,
		.extraFireDefenseMultiplier = 100,
		.extraIceDefenseMultiplier = 200,
		.extraLightningDefenseMultiplier = 100,
		.powerLevel = 1,
		.healthLevel = 1,
		.defenseLevel = 1,
		.magicDefenseLevel = 0,
		.allowPhysicalAttack = true,
		.goldScale = 2
	},

	{
		.name = "Wizard",
		.imageId = RESOURCE_ID_IMAGE_WIZARD,
		.extraFireDefenseMultiplier = 100,
		.extraIceDefenseMultiplier = 100,
		.extraLightningDefenseMultiplier = 100,
		.powerLevel = 1,
		.healthLevel = 1,
		.defenseLevel = 1,
		.magicDefenseLevel = 3,
		.allowMagicAttack = true,
		.goldScale = 3
	},

	{
		.name = "Zombie",
		.imageId = RESOURCE_ID_IMAGE_ZOMBIE,
		.extraFireDefenseMultiplier = 300,
		.extraIceDefenseMultiplier = 100,
		.extraLightningDefenseMultiplier = 100,
		.powerLevel = 2,
		.healthLevel = 1,
		.defenseLevel = 1,
		.magicDefenseLevel = 1,
		.allowPhysicalAttack = true,
		.goldScale = 2
	},

	{
		.name = "Turtle",
		.imageId = RESOURCE_ID_IMAGE_TURTLE,
		.extraFireDefenseMultiplier = 100,
		.extraIceDefenseMultiplier = 100,
		.extraLightningDefenseMultiplier = 600,
		.powerLevel = 1,
		.healthLevel = 2,
		.defenseLevel = 3,
		.magicDefenseLevel = 1,
		.allowPhysicalAttack = true,
		.goldScale = 1
	},

	{
		.name = "Lich",
		.imageId = RESOURCE_ID_IMAGE_LICH,
		.extraFireDefenseMultiplier = 100,
		.extraIceDefenseMultiplier = 600,
		.extraLightningDefenseMultiplier = 100,
		.powerLevel = 2,
		.healthLevel = 2,
		.defenseLevel = 3,
		.magicDefenseLevel = 1,
		.allowMagicAttack = true,
		.goldScale = 5
	},

	{
		.name = "Dragon",
		.imageId = RESOURCE_ID_IMAGE_DRAGON,
		.extraFireDefenseMultiplier = 100,
		.extraIceDefenseMultiplier = 100,
		.extraLightningDefenseMultiplier = 100,
		.powerLevel = 2,
		.healthLevel = 2,
		.defenseLevel = 2,
		.magicDefenseLevel = 2,
		.allowPhysicalAttack = true,
		.allowMagicAttack = true,
		.goldScale = 10,
		.preventRun = true,
	},
};

static Location locationList[] =
{
	{
		.name = "Dungeon Entrance",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FULL},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Dungeon",
		.type = LOCATIONTYPE_DUNGEON,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_ENTRANCE, DUNGEON_DRAGONS_ROOM},
		.length = 30,
		.baseLevel = 1,
		.dungeonclass = &FullDungeonClass,
	},
	{
		.name = "Dragon's Room",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_TREASURE_ROOM},
		.fixedclass = &DragonsRoomClass,
		.baseLevel = 1,
		.fixed_ArrivalFunction = ShowBattleWindow,
	},
	{
		.name = "Treasure Room",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 0,
		.fixedclass = &TreasureRoomClass,
		.fixed_ArrivalFunction = ShowEndWindow,
	},
};

StoryState dungeonCrawlStoryState = {0};

void InitializeDungeonCrawl(void)
{
	dungeonCrawlStoryState.needsSaving = true;
	dungeonCrawlStoryState.persistedStoryState.currentLocationIndex = 0;
	dungeonCrawlStoryState.persistedStoryState.currentLocationDuration = 0;
	dungeonCrawlStoryState.persistedStoryState.currentPathDestination = 0;
	dungeonCrawlStoryState.persistedStoryState.mostRecentMonster = 0;

#if ENABLE_ITEMS
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
#endif
}

Story dungeonCrawlStory = 
{
	.gameNumber = DUNGEON_CRAWL_INDEX,
	.gameDataVersion = 1,
	.locationList = locationList,
	.monsterList = monsters,
	.initializeStory = InitializeDungeonCrawl,
};

void LaunchDungeonCrawl(void)
{
	dungeonCrawlStory.numberOfLocations = sizeof(locationList)/sizeof(Location);
	dungeonCrawlStory.numberOfMonsters = sizeof(monsters)/sizeof(MonsterDef);
	RegisterStory(&dungeonCrawlStory, &dungeonCrawlStoryState);
	DEBUG_LOG("Initialized locationList size = %d", sizeof(locationList));
	ShowAdventureWindow();
}

#endif