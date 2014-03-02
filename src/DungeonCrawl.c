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
	DUNGEON_FLOOR_1,
	DUNGEON_FLOOR_1_END,
	DUNGEON_FLOOR_2,
	DUNGEON_FLOOR_2_END,
	DUNGEON_FLOOR_3,
	DUNGEON_FLOOR_3_END,
	DUNGEON_FLOOR_4,
	DUNGEON_FLOOR_4_END,
	DUNGEON_FLOOR_5,
	DUNGEON_FLOOR_5_END,
	DUNGEON_FLOOR_6,
	DUNGEON_FLOOR_6_END,
	DUNGEON_FLOOR_7,
	DUNGEON_FLOOR_7_END,
	DUNGEON_FLOOR_8,
	DUNGEON_FLOOR_8_END,
	DUNGEON_FLOOR_9,
	DUNGEON_FLOOR_9_END,
	DUNGEON_FLOOR_10,
	DUNGEON_FLOOR_10_END,
	DUNGEON_FLOOR_11,
	DUNGEON_FLOOR_11_END,
	DUNGEON_FLOOR_12,
	DUNGEON_FLOOR_12_END,
	DUNGEON_FLOOR_13,
	DUNGEON_FLOOR_13_END,
	DUNGEON_FLOOR_14,
	DUNGEON_FLOOR_14_END,
	DUNGEON_FLOOR_15,
	DUNGEON_FLOOR_15_END,
	DUNGEON_FLOOR_16,
	DUNGEON_FLOOR_16_END,
	DUNGEON_FLOOR_17,
	DUNGEON_FLOOR_17_END,
	DUNGEON_FLOOR_18,
	DUNGEON_FLOOR_18_END,
	DUNGEON_FLOOR_19,
	DUNGEON_FLOOR_19_END,
	DUNGEON_FLOOR_20,
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
	.encounterChance = 25,
	.numberOfBackgroundImages = 4,
	.backgroundImages = {RESOURCE_ID_IMAGE_DUNGEONSTRAIGHT, RESOURCE_ID_IMAGE_DUNGEONLEFT, RESOURCE_ID_IMAGE_DUNGEONRIGHT, RESOURCE_ID_IMAGE_DUNGEONDEADEND},	
};

static FixedClass DungeonStairsClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_NEWFLOOR,
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

static Location locationList[] = 
{
	{
		.name = "Dungeon Entrance",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_1},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 1",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_ENTRANCE, DUNGEON_FLOOR_1_END},
		.length = 30,
		.baseLevel = 1,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 1 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_2},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 2",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_1_END, DUNGEON_FLOOR_2_END},
		.length = 30,
		.baseLevel = 2,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 2 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_3},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 3",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_2_END, DUNGEON_FLOOR_3_END},
		.length = 30,
		.baseLevel = 3,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 3 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_4},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 4",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_3_END, DUNGEON_FLOOR_4_END},
		.length = 30,
		.baseLevel = 4,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 4 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_5},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 5",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_4_END, DUNGEON_FLOOR_5_END},
		.length = 30,
		.baseLevel = 5,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 5 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_6},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 6",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_5_END, DUNGEON_FLOOR_6_END},
		.length = 30,
		.baseLevel = 6,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 6 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_7},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 7",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_6_END, DUNGEON_FLOOR_7_END},
		.length = 30,
		.baseLevel = 7,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 7 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_8},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 8",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_7_END, DUNGEON_FLOOR_8_END},
		.length = 30,
		.baseLevel = 8,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 8 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_9},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 9",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_8_END, DUNGEON_FLOOR_9_END},
		.length = 30,
		.baseLevel = 9,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 9 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_10},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 10",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_9_END, DUNGEON_FLOOR_10_END},
		.length = 30,
		.baseLevel = 10,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 10 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_11},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 11",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_10_END, DUNGEON_FLOOR_11_END},
		.length = 30,
		.baseLevel = 11,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 11 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_12},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 12",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_11_END, DUNGEON_FLOOR_12_END},
		.length = 30,
		.baseLevel = 12,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 12 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_13},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 13",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_12_END, DUNGEON_FLOOR_13_END},
		.length = 30,
		.baseLevel = 13,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 13 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_14},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 14",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_10_END, DUNGEON_FLOOR_11_END},
		.length = 30,
		.baseLevel = 14,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 14 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_15},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 15",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_14_END, DUNGEON_FLOOR_15_END},
		.length = 30,
		.baseLevel = 15,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 15 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_16},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 16",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_15_END, DUNGEON_FLOOR_16_END},
		.length = 30,
		.baseLevel = 16,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 16 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_17},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 17",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_16_END, DUNGEON_FLOOR_17_END},
		.length = 30,
		.baseLevel = 17,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 17 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_18},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 18",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_17_END, DUNGEON_FLOOR_18_END},
		.length = 30,
		.baseLevel = 18,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 18 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_19},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 19",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_18_END, DUNGEON_FLOOR_19_END},
		.length = 30,
		.baseLevel = 19,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Floor 19 Stairs",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_FLOOR_20},
		.fixedclass = &DungeonStairsClass,
	},
	{
		.name = "Floor 20",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DUNGEON_FLOOR_19_END, DUNGEON_DRAGONS_ROOM},
		.length = 30,
		.baseLevel = 20,
		.pathclass = &DungeonFloorClass,
	},
	{
		.name = "Dragon's Room",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DUNGEON_TREASURE_ROOM},
		.fixedclass = &DragonsRoomClass,
		.baseLevel = 20,
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

StoryState dungeonCrawlStoryState = {0};

void InitializeDungeonCrawl(void)
{
	dungeonCrawlStoryState.needsSaving = true;
	dungeonCrawlStoryState.persistedStoryState.currentLocationIndex = 0;
	dungeonCrawlStoryState.persistedStoryState.currentLocationDuration = 0;
	dungeonCrawlStoryState.persistedStoryState.currentPathDestination = 0;
	dungeonCrawlStoryState.persistedStoryState.mostRecentMonster = 0;
	
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
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
#if INCLUDE_DUNGEON_CRAWL
	dungeonCrawlStory.numberOfLocations = sizeof(locationList)/sizeof(Location);
	dungeonCrawlStory.numberOfMonsters = sizeof(monsters)/sizeof(MonsterDef);
	RegisterStory(&dungeonCrawlStory, &dungeonCrawlStoryState);
	DEBUG_LOG("Initialized locationList size = %d", sizeof(locationList));
	ShowAdventureWindow();
#endif	
}

#endif