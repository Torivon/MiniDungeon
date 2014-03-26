#include "pebble.h"

#include "Adventure.h"
#include "Battle.h"
#include "Character.h"
#include "DragonQuest.h"
#include "Items.h"
#include "Location.h"
#include "LocationInternal.h"
#include "Logging.h"
#include "MiniDungeon.h"
#include "Monsters.h"
#include "Story.h"
	
#if INCLUDE_DRAGON_QUEST	

enum
{
	VILLAGE1 = 0,
	VILLAGE1HOUSE,
	VILLAGE2,
	VILLAGE2HOUSE,
	ROAD1,
	CELLAR1,
	CAVE1,
	FOREST1,
	FORESTCAVE1,
	RUINEDVILLAGE1,
	FOREST2,
	WATERFALLBASE,
	WATERFALLCAVE,
	WATERFALLTOP,
	ROAD2,
	VILLAGE3,
	DESERT1,
	OASIS1,
	DESERT2,
	PYRAMID_EXTERIOR,
	PYRAMID_INTERIOR,
	SARCOPHAGUS,
	BOAT1,
	BOATFIXED,
	BOAT2,
	VILLAGE4,
	MOUNTAIN1,
	TOMB_ENTRANCE1,
	TOMB_INTERIOR1,
	TREASURE_ROOM1,
	ROAD3,
	CAVE_ENTRANCE1,
	CAVE2,
	CAVE_ENTRANCE2,
	ROAD4,
	TOMB_ENTRANCE2,
	TOMB_INTERIOR2,
	TREASURE_ROOM2,
	BOAT3,
	BEACH,
	MOUNTAIN2,
	CAVE_ENTRANCE3,
	DRAGON_CAVE,
	DRAGON_ROOM,
};

static PathClass RoadClass =
{
	.numberOfMonsters = 0,
	.monsters = {0},
	.monsterUnlockLevel = 2,
	.encounterChance = 0,
	.numberOfBackgroundImages = 3,
	.backgroundImages = {RESOURCE_ID_IMAGE_ROAD1, RESOURCE_ID_IMAGE_ROAD2, RESOURCE_ID_IMAGE_ROAD3},	
};

static PathClass BoatClass =
{
	.numberOfMonsters = 0,
	.monsters = {0},
	.monsterUnlockLevel = 2,
	.encounterChance = 0,
	.numberOfBackgroundImages = 1,
	.backgroundImages = {RESOURCE_ID_IMAGE_BOAT},	
};

static PathClass CaveClass =
{
	.numberOfMonsters = 0,
	.monsters = {0},
	.monsterUnlockLevel = 2,
	.encounterChance = 0,
	.numberOfBackgroundImages = 3,
	.backgroundImages = {RESOURCE_ID_IMAGE_CAVE1, RESOURCE_ID_IMAGE_CAVE2, RESOURCE_ID_IMAGE_CAVE3},
};

static PathClass RuinsPathClass =
{
	.numberOfMonsters = 0,
	.monsters = {0},
	.monsterUnlockLevel = 2,
	.encounterChance = 0,
	.numberOfBackgroundImages = 3,
	.backgroundImages = {RESOURCE_ID_IMAGE_DUNGEONSTRAIGHT, RESOURCE_ID_IMAGE_DUNGEONLEFT, RESOURCE_ID_IMAGE_DUNGEONRIGHT},
};

static PathClass ForestClass =
{
	.numberOfMonsters = 0,
	.monsters = {0},
	.monsterUnlockLevel = 2,
	.encounterChance = 0,
	.numberOfBackgroundImages = 3,
	.backgroundImages = {RESOURCE_ID_IMAGE_FOREST1, RESOURCE_ID_IMAGE_FOREST2, RESOURCE_ID_IMAGE_FOREST3},
};

static PathClass DesertClass =
{
	.numberOfMonsters = 0,
	.monsters = {0},
	.monsterUnlockLevel = 2,
	.encounterChance = 0,
	.numberOfBackgroundImages = 3,
	.backgroundImages = {RESOURCE_ID_IMAGE_DESERT1, RESOURCE_ID_IMAGE_DESERT2, RESOURCE_ID_IMAGE_DESERT3},
};

static PathClass MountainClass =
{
	.numberOfMonsters = 0,
	.monsters = {0},
	.monsterUnlockLevel = 2,
	.encounterChance = 0,
	.numberOfBackgroundImages = 3,
	.backgroundImages = {RESOURCE_ID_IMAGE_MOUNTAIN1, RESOURCE_ID_IMAGE_MOUNTAIN2, RESOURCE_ID_IMAGE_MOUNTAIN3},
};

static FixedClass BoatFixedClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_BOAT,
	.allowShop = false,
};

static FixedClass VillageClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_VILLAGE_EXTERIOR,
	.allowShop = true,
};

static FixedClass RuinedVillageClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_RUINED_VILLAGE_EXTERIOR,
	.allowShop = false,
};

static FixedClass WaterfallBaseClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_WATERFALL_BASE,
	.allowShop = false,
};

static FixedClass WaterfallTopClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_WATERFALL_TOP,
	.allowShop = false,
};

static FixedClass HouseClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_VILLAGE_INTERIOR,
	.allowShop = false,
};

static FixedClass CellarClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_CELLAR,
	.allowShop = false,
};

static FixedClass ForestCaveClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_FOREST_CAVE,
	.allowShop = false,
};

static FixedClass CaveFixedClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_CAVE1,
	.allowShop = false,
};

static FixedClass RuinsFixedClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_NEWFLOOR,
	.allowShop = false,
};

static FixedClass OasisClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_OASIS,
	.allowShop = false,
};

static FixedClass PyramidClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_PYRAMID_EXTERIOR,
	.allowShop = false,
};

static FixedClass SarcophagusClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_SARCOPHAGUS,
	.allowShop = false,
};

static FixedClass TombClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_TOMB_ENTRANCE,
	.allowShop = false,
};

static FixedClass TreasureRoomClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_TREASURE_ROOM,
	.allowShop = false,
};

static FixedClass BeachClass =
{
	.backgroundImage = RESOURCE_ID_IMAGE_BEACH,
	.allowShop = false,
};

static DungeonClass CaveDungeonClass =
{
	.numberOfFloors = 3,
	.levelIncreaseRate = 1,
	.pathclass = &CaveClass,
	.fixedclass = &CaveFixedClass,
};

static DungeonClass RuinsDungeonClass =
{
	.numberOfFloors = 3,
	.levelIncreaseRate = 1,
	.pathclass = &RuinsPathClass,
	.fixedclass = &RuinsFixedClass,
};

static MonsterDef monsters[] =
{
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
	{ //VILLAGE1
		.name = "Village 1",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {VILLAGE1HOUSE},
		.fixedclass = &VillageClass,
	},
	{ //VILLAGE1HOUSE
		.name = "Village 1 House",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {VILLAGE1, CELLAR1},
		.fixedclass = &HouseClass,
	},
	{ //VILLAGE2
		.name = "Village 2",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 4,
		.adjacentLocations = {VILLAGE2HOUSE, ROAD1, FOREST2, ROAD2},
		.fixedclass = &VillageClass,
	},
	{ //VILLAGE2HOUSE
		.name = "Village 2 House",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {VILLAGE2},
		.fixedclass = &HouseClass,
	},
	{ //ROAD1
		.name = "Road",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {RUINEDVILLAGE1, VILLAGE2},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &RoadClass,
	},
	{ //CELLAR1
		.name = "Cellar",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {VILLAGE1HOUSE, CAVE1},
		.fixedclass = &CellarClass,
	},
	{ //CAVE1
		.name = "Cave",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {CELLAR1, FORESTCAVE1},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &CaveClass,
	},
	{ //FOREST1
		.name = "Forest",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {FORESTCAVE1, RUINEDVILLAGE1},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &ForestClass,
	},
	{ //FORESTCAVE1
		.name = "Forest Cave",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {FOREST1, CAVE1},
		.fixedclass = &ForestCaveClass,
	},
	{ //RUINEDVILLAGE1
		.name = "Village 1",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {ROAD1, FOREST1},
		.fixedclass = &RuinedVillageClass,
	},
	{ //FOREST2
		.name = "Forest2",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {VILLAGE2, WATERFALLBASE},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &ForestClass,
	},
	{ //WATERFALLBASE
		.name = "Waterfall Base",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {FOREST2, WATERFALLCAVE},
		.fixedclass = &WaterfallBaseClass,
	},
	{ //WATERFALLCAVE
		.name = "Waterfall Cave",
		.type = LOCATIONTYPE_DUNGEON,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {WATERFALLBASE, WATERFALLTOP},
		.length = 2,
		.baseLevel = 1,
		.dungeonclass = &CaveDungeonClass,
	},
	{ //WATERFALLTOP
		.name = "Waterfall Top",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {WATERFALLCAVE, WATERFALLBASE},
		.fixedclass = &WaterfallTopClass,
	},
	{ //ROAD2
		.name = "Road",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {VILLAGE2, VILLAGE3},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &RoadClass,
	},
	{ //VILLAGE3
		.name = "Village 3",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 3,
		.adjacentLocations = {ROAD2, DESERT1, BOAT1},
		.fixedclass = &VillageClass,
	},
	{ //DESERT1
		.name = "Desert1",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {VILLAGE3, OASIS1},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &DesertClass,
	},
	{ //OASIS1
		.name = "Oasis",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DESERT1, DESERT2},
		.fixedclass = &OasisClass,
	},
	{ //DESERT2
		.name = "Desert2",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {OASIS1, PYRAMID_EXTERIOR},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &DesertClass,
	},
	{ //PYRAMID_EXTERIOR
		.name = "Pyramid",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {DESERT2, PYRAMID_INTERIOR},
		.fixedclass = &PyramidClass,
	},
	{ //PYRAMID_INTERIOR
		.name = "Pyramid Inside",
		.type = LOCATIONTYPE_DUNGEON,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {PYRAMID_EXTERIOR, SARCOPHAGUS},
		.length = 2,
		.baseLevel = 1,
		.dungeonclass = &RuinsDungeonClass,
	},
	{ //SARCOPHAGUS
		.name = "Sarcophagus",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {PYRAMID_INTERIOR},
		.fixedclass = &SarcophagusClass,
	},
	{ //BOAT1
		.name = "Boat1",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {VILLAGE3, BOATFIXED},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &BoatClass,
	},
	{ //BOATFIXED
		.name = "BoatFixed",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {BOAT1, BOAT2},
		.fixedclass = &BoatFixedClass,
	},
	{ //BOAT2
		.name = "Boat2",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {BOATFIXED, VILLAGE4},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &BoatClass,
	},
	{ //VILLAGE4
		.name = "Village 4",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 4,
		.adjacentLocations = {BOAT2, MOUNTAIN1, ROAD3, BOAT3},
		.fixedclass = &VillageClass,
	},
	{ //MOUNTAIN1
		.name = "Mountain1",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {VILLAGE4, TOMB_ENTRANCE1},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &MountainClass,
	},
	{ //TOMB_ENTRANCE1
		.name = "Tomb1",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {MOUNTAIN1, TOMB_INTERIOR1},
		.fixedclass = &TombClass,
	},
	{ //TOMB_INTERIOR1
		.name = "Tomb1 Inside",
		.type = LOCATIONTYPE_DUNGEON,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {TOMB_ENTRANCE1, TREASURE_ROOM1},
		.length = 2,
		.baseLevel = 1,
		.dungeonclass = &RuinsDungeonClass,
	},
	{ //TREASURE_ROOM1
		.name = "Treasure Room1",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {TOMB_INTERIOR1},
		.fixedclass = &TreasureRoomClass,
	},
	{ //ROAD3
		.name = "Road",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {VILLAGE4, CAVE_ENTRANCE1},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &RoadClass,
	},
	{ //CAVE_ENTRANCE1
		.name = "Cave south",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {ROAD3, CAVE2},
		.fixedclass = &ForestCaveClass,
	},
	{ //CAVE2
		.name = "Cave",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {CAVE_ENTRANCE1, CAVE_ENTRANCE2},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &CaveClass,
	},
	{ //CAVE_ENTRANCE2
		.name = "Cave north",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {CAVE2, ROAD4},
		.fixedclass = &ForestCaveClass,
	},
	{ //ROAD4
		.name = "Road",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {CAVE_ENTRANCE2, TOMB_ENTRANCE2},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &RoadClass,
	},
	{ //TOMB_ENTRANCE2
		.name = "Tomb2",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {ROAD4, TOMB_INTERIOR2},
		.fixedclass = &TombClass,
	},
	{ //TOMB_INTERIOR
		.name = "Tomb2 Inside",
		.type = LOCATIONTYPE_DUNGEON,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {TOMB_ENTRANCE2, TREASURE_ROOM2},
		.length = 2,
		.baseLevel = 1,
		.dungeonclass = &RuinsDungeonClass,
	},
	{ //TREASURE_ROOM
		.name = "Treasure Room2",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {TOMB_INTERIOR2},
		.fixedclass = &TreasureRoomClass,
	},
	{ //BOAT3
		.name = "Boat3",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {VILLAGE4, BEACH},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &BoatClass,
	},
	{ //BEACH
		.name = "Beach",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {BOAT3, MOUNTAIN2},
		.fixedclass = &BeachClass,
	},
	{ //MOUNTAIN2
		.name = "Mountain2",
		.type = LOCATIONTYPE_PATH,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {BEACH, CAVE_ENTRANCE3},
		.length = 2,
		.baseLevel = 1,
		.pathclass = &MountainClass,
	},
	{ //CAVE_ENTRANCE3
		.name = "Cave south",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {MOUNTAIN2, DRAGON_CAVE},
		.fixedclass = &ForestCaveClass,
	},
	{ //DRAGON_CAVE
		.name = "Dragon Cave",
		.type = LOCATIONTYPE_DUNGEON,
		.numberOfAdjacentLocations = 2,
		.adjacentLocations = {CAVE_ENTRANCE3, DRAGON_ROOM},
		.length = 2,
		.baseLevel = 1,
		.dungeonclass = &CaveDungeonClass,
	},
	{ //DRAGON_ROOM
		.name = "Dragon's Room",
		.type = LOCATIONTYPE_FIXED,
		.numberOfAdjacentLocations = 1,
		.adjacentLocations = {DRAGON_CAVE},
		.fixedclass = &TreasureRoomClass,
	},
};

StoryState dragonQuestStoryState = {0};

void InitializeDragonQuest(void)
{
	dragonQuestStoryState.needsSaving = true;
	dragonQuestStoryState.persistedStoryState.currentLocationIndex = VILLAGE1HOUSE;
	dragonQuestStoryState.persistedStoryState.currentLocationDuration = 0;
	dragonQuestStoryState.persistedStoryState.currentPathDestination = 0;
	dragonQuestStoryState.persistedStoryState.mostRecentMonster = 0;
	
#if ENABLE_ITEMS
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
	AddItem(ITEM_TYPE_POTION);
#endif
}

Story dragonQuestStory = 
{
	.gameNumber = DRAGON_QUEST_INDEX,
	.gameDataVersion = 5,
	.locationList = locationList,
	.monsterList = monsters,
	.initializeStory = InitializeDragonQuest,
};

void LaunchDragonQuest(void)
{
	dragonQuestStory.numberOfLocations = sizeof(locationList)/sizeof(Location);
	dragonQuestStory.numberOfMonsters = sizeof(monsters)/sizeof(MonsterDef);
	RegisterStory(&dragonQuestStory, &dragonQuestStoryState);
	DEBUG_LOG("Initialized locationList size = %d", sizeof(locationList));
	ShowAdventureWindow();
}

#endif
