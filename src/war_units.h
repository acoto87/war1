#pragma once

#include <assert.h>

#include "war_log.h"
#include "war_color.h"
#include "war_types.h"
#include "war.h"
#include "war_commands.h"

// Brace-only StringView initializer for use in file-scope const arrays.
// Unlike WSV_LITERAL, this does not use a compound literal cast so it
// satisfies -Wpedantic's "initializer element is not constant" requirement.
#define WSV_INIT(s) { s, sizeof(s) - 1 }

StringView features[MAX_FEATURES_COUNT * 2] = {
	// Units. 0 - 6
	WSV_INIT("unit-footman"),             WSV_INIT("unit-grunt"),
	WSV_INIT("unit-peasant"),             WSV_INIT("unit-peon"),
	WSV_INIT("unit-human-catapult"),      WSV_INIT("unit-orc-catapult"),
	WSV_INIT("unit-knight"),              WSV_INIT("unit-raider"),
	WSV_INIT("unit-archer"),              WSV_INIT("unit-spearman"),
	WSV_INIT("unit-conjurer"),            WSV_INIT("unit-warlock"),
	WSV_INIT("unit-cleric"),              WSV_INIT("unit-necrolyte"),
	// Constructing buildings. 7 - 14
	WSV_INIT("unit-human-farm"),          WSV_INIT("unit-orc-farm"),
	WSV_INIT("unit-human-barracks"),      WSV_INIT("unit-orc-barracks"),
	WSV_INIT("unit-human-church"),        WSV_INIT("unit-orc-temple"),
	WSV_INIT("unit-human-tower"),         WSV_INIT("unit-orc-tower"),
	WSV_INIT("unit-human-town-hall"),     WSV_INIT("unit-orc-town-hall"),
	WSV_INIT("unit-human-lumber-mill"),   WSV_INIT("unit-orc-lumber-mill"),
	WSV_INIT("unit-human-stable"),        WSV_INIT("unit-orc-kennel"),
	WSV_INIT("unit-human-blacksmith"),    WSV_INIT("unit-orc-blacksmith"),
	// Cleric/Necrolyte spells. 15 - 17
	WSV_INIT("upgrade-healing"),          WSV_INIT("upgrade-raise-dead"),
	WSV_INIT("upgrade-holy-vision"),      WSV_INIT("upgrade-dark-vision"),
	WSV_INIT("upgrade-invisibility"),     WSV_INIT("upgrade-unholy-armor"),
	// Conjurer/Warlock spells. 18 - 20
	WSV_INIT("upgrade-scorpion"),         WSV_INIT("upgrade-spider"),
	WSV_INIT("upgrade-rain-of-fire"),     WSV_INIT("upgrade-poison-cloud"),
	WSV_INIT("upgrade-water-elemental"),  WSV_INIT("upgrade-daemon"),
	// Roads and walls. 21 - 22
	WSV_INIT("unit-road"),                WSV_INIT("unit-wall")
};

StringView upgradeNames[MAX_UPGRADES_COUNT * 2] =
{
    // Basic upgrades
    WSV_INIT("upgrade-spear"),        WSV_INIT("upgrade-arrow"),
    WSV_INIT("upgrade-axe"),          WSV_INIT("upgrade-sword"),
    WSV_INIT("upgrade-wolves"),       WSV_INIT("upgrade-horse"),
    // Spells and summons
    WSV_INIT("upgrade-spider"),       WSV_INIT("upgrade-scorpion"),
    WSV_INIT("upgrade-poison-cloud"), WSV_INIT("upgrade-rain-of-fire"),
    WSV_INIT("upgrade-daemon"),       WSV_INIT("upgrade-water-elemental"),
    WSV_INIT("upgrade-raise-dead"),   WSV_INIT("upgrade-healing"),
    WSV_INIT("upgrade-dark-vision"),  WSV_INIT("upgrade-far-seeing"),
    WSV_INIT("upgrade-unholy-armor"), WSV_INIT("upgrade-invisibility"),
    // Shield upgrades
    WSV_INIT("upgrade-orc-shield"),   WSV_INIT("upgrade-human-shield")
};

typedef enum
{
    WAR_PORTRAIT_FOOTMAN,
    WAR_PORTRAIT_GRUNT,
    WAR_PORTRAIT_CONJURER,
    WAR_PORTRAIT_WARLOCK,
    WAR_PORTRAIT_PEASANT,
    WAR_PORTRAIT_PEON,
    WAR_PORTRAIT_CATAPULT_HUMANS,
    WAR_PORTRAIT_CATAPULT_ORCS,
    WAR_PORTRAIT_KNIGHT,
    WAR_PORTRAIT_RAIDER,
    WAR_PORTRAIT_ARCHER,
    WAR_PORTRAIT_SPEARMAN,
    WAR_PORTRAIT_CLERIC,
    WAR_PORTRAIT_NECROLYTE,
    WAR_PORTRAIT_FARM_HUMANS,
    WAR_PORTRAIT_FARM_ORCS,
    WAR_PORTRAIT_BARRACKS_HUMANS,
    WAR_PORTRAIT_BARRACKS_ORCS,
    WAR_PORTRAIT_TOWER_HUMANS,
    WAR_PORTRAIT_TOWER_ORCS,
    WAR_PORTRAIT_TOWNHALL_HUMANS,
    WAR_PORTRAIT_TOWNHALL_ORCS,
    WAR_PORTRAIT_LUMBERMILL_HUMANS,
    WAR_PORTRAIT_LUMBERMILL_ORCS,
    WAR_PORTRAIT_STABLE,
    WAR_PORTRAIT_KENNEL,
    WAR_PORTRAIT_BLACKSMITH_HUMANS,
    WAR_PORTRAIT_BLACKSMITH_ORCS,
    WAR_PORTRAIT_CHURCH,
    WAR_PORTRAIT_TEMPLE,
    WAR_PORTRAIT_GOLDMINE,
    WAR_PORTRAIT_STORMWIND,
    WAR_PORTRAIT_BLACKROCK,
    WAR_PORTRAIT_MOVE_HUMANS,
    WAR_PORTRAIT_MOVE_ORCS,
    WAR_PORTRAIT_REPAIR,
    WAR_PORTRAIT_HARVEST,
    WAR_PORTRAIT_BUILD_BASIC,
    WAR_PORTRAIT_BUILD_ADVANCED,
    WAR_PORTRAIT_DELIVER,
    WAR_PORTRAIT_CANCEL,
    WAR_PORTRAIT_WALL,
    WAR_PORTRAIT_ROAD,
    WAR_PORTRAIT_UNKOWN,
    WAR_PORTRAIT_OGRE,
    WAR_PORTRAIT_SPIDER,
    WAR_PORTRAIT_SLIME,
    WAR_PORTRAIT_FIRE_ELEMENTAL,
    WAR_PORTRAIT_SCORPION,
    WAR_PORTRAIT_SKELETON,
    WAR_PORTRAIT_DEAD,
    WAR_PORTRAIT_DAEMON,
    WAR_PORTRAIT_WATER_ELEMENTAL,
    WAR_PORTRAIT_LOTHAR,
    WAR_PORTRAIT_MEDIVH,
    WAR_PORTRAIT_GRIZELDA,
    WAR_PORTRAIT_GARONA,
    WAR_PORTRAIT_WOUNDED,
    WAR_PORTRAIT_BRIGAND,
    WAR_PORTRAIT_HOLY_LANCE,
    WAR_PORTRAIT_ELEMENTAL_BLAST,
    WAR_PORTRAIT_SHADOW_SPEAR,
    WAR_PORTRAIT_FIREBALL,
    WAR_PORTRAIT_SWORD_1,
    WAR_PORTRAIT_SWORD_2,
    WAR_PORTRAIT_SWORD_3,
    WAR_PORTRAIT_AXE_1,
    WAR_PORTRAIT_AXE_2,
    WAR_PORTRAIT_AXE_3,
    WAR_PORTRAIT_WOLVES_1,
    WAR_PORTRAIT_WOLVES_2,
    WAR_PORTRAIT_ARROW_1,
    WAR_PORTRAIT_ARROW_2,
    WAR_PORTRAIT_ARROW_3,
    WAR_PORTRAIT_SPEAR_1,
    WAR_PORTRAIT_SPEAR_2,
    WAR_PORTRAIT_SPEAR_3,
    WAR_PORTRAIT_HORSE_1,
    WAR_PORTRAIT_HORSE_2,
    WAR_PORTRAIT_SHIELD_1_HUMANS,
    WAR_PORTRAIT_SHIELD_2_HUMANS,
    WAR_PORTRAIT_SHIELD_3_HUMANS,
    WAR_PORTRAIT_SHIELD_1_ORCS,
    WAR_PORTRAIT_SHIELD_2_ORCS,
    WAR_PORTRAIT_SHIELD_3_ORCS,
    WAR_PORTRAIT_HEALING,
    WAR_PORTRAIT_FAR_SIGHT,
    WAR_PORTRAIT_INVISIBILITY,
    WAR_PORTRAIT_RAIN_OF_FIRE,
    WAR_PORTRAIT_RAISE_DEAD,
    WAR_PORTRAIT_DARK_VISION,
    WAR_PORTRAIT_UNHOLY_ARMOR,
    WAR_PORTRAIT_POISON_CLOUD
} WarUnitPortraits;

typedef struct
{
    WarUnitType type;
    s32 resourceIndex;
    s32 portraitFrameIndex;
    s32 sizex;
    s32 sizey;
    StringView name;
} WarUnitData;

const WarUnitData unitsData[] =
{
    // units
    { WAR_UNIT_FOOTMAN,             279, WAR_PORTRAIT_FOOTMAN,            1, 1, WSV_INIT("FOOTMAN")         },
    { WAR_UNIT_GRUNT,               280, WAR_PORTRAIT_GRUNT,              1, 1, WSV_INIT("GRUNT")           },
    { WAR_UNIT_PEASANT,             281, WAR_PORTRAIT_PEASANT,            1, 1, WSV_INIT("PEASANT")         },
    { WAR_UNIT_PEON,                282, WAR_PORTRAIT_PEON,               1, 1, WSV_INIT("PEON")            },
    { WAR_UNIT_CATAPULT_HUMANS,     283, WAR_PORTRAIT_CATAPULT_HUMANS,    1, 1, WSV_INIT("CATAPULT")        },
    { WAR_UNIT_CATAPULT_ORCS,       284, WAR_PORTRAIT_CATAPULT_ORCS,      1, 1, WSV_INIT("CATAPULT")        },
    { WAR_UNIT_KNIGHT,              285, WAR_PORTRAIT_KNIGHT,             1, 1, WSV_INIT("KNIGHT")          },
    { WAR_UNIT_RAIDER,              286, WAR_PORTRAIT_RAIDER,             1, 1, WSV_INIT("RAIDER")          },
    { WAR_UNIT_ARCHER,              287, WAR_PORTRAIT_ARCHER,             1, 1, WSV_INIT("ARCHER")          },
    { WAR_UNIT_SPEARMAN,            288, WAR_PORTRAIT_SPEARMAN,           1, 1, WSV_INIT("SPEARMAN")        },
    { WAR_UNIT_CONJURER,            289, WAR_PORTRAIT_CONJURER,           1, 1, WSV_INIT("CONJURER")        },
    { WAR_UNIT_WARLOCK,             290, WAR_PORTRAIT_WARLOCK,            1, 1, WSV_INIT("WARLOCK")         },
    { WAR_UNIT_CLERIC,              291, WAR_PORTRAIT_CLERIC,             1, 1, WSV_INIT("CLERIC")          },
    { WAR_UNIT_NECROLYTE,           292, WAR_PORTRAIT_NECROLYTE,          1, 1, WSV_INIT("NECROLYTE")       },
    { WAR_UNIT_MEDIVH,              293, WAR_PORTRAIT_MEDIVH,             1, 1, WSV_INIT("MEDIVH")          },
    { WAR_UNIT_LOTHAR,              294, WAR_PORTRAIT_LOTHAR,             1, 1, WSV_INIT("LOTHAR")          },
    { WAR_UNIT_WOUNDED,             295, WAR_PORTRAIT_WOUNDED,            1, 1, WSV_INIT("WOUNDED")         },
    { WAR_UNIT_GRIZELDA,            296, WAR_PORTRAIT_GRIZELDA,           1, 1, WSV_INIT("GRIZELDA")        },
    { WAR_UNIT_GARONA,              296, WAR_PORTRAIT_GARONA,             1, 1, WSV_INIT("GARONA")          },
    { WAR_UNIT_OGRE,                297, WAR_PORTRAIT_OGRE,               1, 1, WSV_INIT("OGRE")            },
    { WAR_UNIT_SPIDER,              298, WAR_PORTRAIT_SPIDER,             1, 1, WSV_INIT("SPIDER")          },
    { WAR_UNIT_SLIME,               299, WAR_PORTRAIT_SLIME,              1, 1, WSV_INIT("SLIME")           },
    { WAR_UNIT_FIRE_ELEMENTAL,      300, WAR_PORTRAIT_FIRE_ELEMENTAL,     1, 1, WSV_INIT("FIRE ELEM")       },
    { WAR_UNIT_SCORPION,            301, WAR_PORTRAIT_SCORPION,           1, 1, WSV_INIT("SCORPION")        },
    { WAR_UNIT_BRIGAND,             302, WAR_PORTRAIT_BRIGAND,            1, 1, WSV_INIT("BRIGAND")         },
    { WAR_UNIT_THE_DEAD,            303, WAR_PORTRAIT_DEAD,               1, 1, WSV_INIT("THE DEAD")        },
    { WAR_UNIT_SKELETON,            304, WAR_PORTRAIT_SKELETON,           1, 1, WSV_INIT("SKELETON")        },
    { WAR_UNIT_DAEMON,              305, WAR_PORTRAIT_DAEMON,             1, 1, WSV_INIT("DAEMON")          },
    { WAR_UNIT_WATER_ELEMENTAL,     306, WAR_PORTRAIT_WATER_ELEMENTAL,    1, 1, WSV_INIT("WATER ELEM")      },

    // buildings
    { WAR_UNIT_FARM_HUMANS,         307, WAR_PORTRAIT_FARM_HUMANS,        2, 2, WSV_INIT("FARM")            },
    { WAR_UNIT_FARM_ORCS,           308, WAR_PORTRAIT_FARM_ORCS,          2, 2, WSV_INIT("FARM")            },
    { WAR_UNIT_BARRACKS_HUMANS,     309, WAR_PORTRAIT_BARRACKS_HUMANS,    3, 3, WSV_INIT("BARRACKS")        },
    { WAR_UNIT_BARRACKS_ORCS,       310, WAR_PORTRAIT_BARRACKS_ORCS,      3, 3, WSV_INIT("BARRACKS")        },
    { WAR_UNIT_CHURCH,              311, WAR_PORTRAIT_CHURCH,             3, 3, WSV_INIT("CHURCH")          },
    { WAR_UNIT_TEMPLE,              312, WAR_PORTRAIT_TEMPLE,             3, 3, WSV_INIT("TEMPLE")          },
    { WAR_UNIT_TOWER_HUMANS,        313, WAR_PORTRAIT_TOWER_HUMANS,       2, 2, WSV_INIT("TOWER")           },
    { WAR_UNIT_TOWER_ORCS,          314, WAR_PORTRAIT_TOWER_ORCS,         2, 2, WSV_INIT("TOWER")           },
    { WAR_UNIT_TOWNHALL_HUMANS,     315, WAR_PORTRAIT_TOWNHALL_HUMANS,    3, 3, WSV_INIT("TOWN HALL")       },
    { WAR_UNIT_TOWNHALL_ORCS,       316, WAR_PORTRAIT_TOWNHALL_ORCS,      3, 3, WSV_INIT("TOWN HALL")       },
    { WAR_UNIT_LUMBERMILL_HUMANS,   317, WAR_PORTRAIT_LUMBERMILL_HUMANS,  3, 3, WSV_INIT("MILL")            },
    { WAR_UNIT_LUMBERMILL_ORCS,     318, WAR_PORTRAIT_LUMBERMILL_ORCS,    3, 3, WSV_INIT("MILL")            },
    { WAR_UNIT_STABLE,              319, WAR_PORTRAIT_STABLE,             3, 3, WSV_INIT("STABLES")         },
    { WAR_UNIT_KENNEL,              320, WAR_PORTRAIT_KENNEL,             3, 3, WSV_INIT("KENNEL")          },
    { WAR_UNIT_BLACKSMITH_HUMANS,   321, WAR_PORTRAIT_BLACKSMITH_HUMANS,  2, 2, WSV_INIT("BLACKSMITH")      },
    { WAR_UNIT_BLACKSMITH_ORCS,     322, WAR_PORTRAIT_BLACKSMITH_ORCS,    2, 2, WSV_INIT("BLACKSMITH")      },
    { WAR_UNIT_STORMWIND,           323, WAR_PORTRAIT_STORMWIND,          4, 4, WSV_INIT("STORMWIND")       },
    { WAR_UNIT_BLACKROCK,           324, WAR_PORTRAIT_BLACKROCK,          4, 4, WSV_INIT("BLACKROCK")       },

    // neutral
    { WAR_UNIT_GOLDMINE,            325, WAR_PORTRAIT_GOLDMINE,           3, 3, WSV_INIT("GOLD MINE")       },

    // corpses
    { WAR_UNIT_HUMAN_CORPSE,        326, 0,                               1, 1, WSV_INIT("CORPSE")          },
    { WAR_UNIT_ORC_CORPSE,          326, 0,                               1, 1, WSV_INIT("CORPSE")          }
};

typedef struct
{
    WarUnitType type;
    s32 carryingWoodResource;
    s32 carryingGoldResource;
} WarWorkerData;

const WarWorkerData workersData[] =
{
    // units                              wood gold
    { WAR_UNIT_PEASANT,                   327, 329 },
    { WAR_UNIT_PEON,                      328, 330 },
};

typedef struct
{
    WarUnitType type;
    s32 buildingResource;
} WarBuildingData;

const WarBuildingData buildingsData[] =
{
    { WAR_UNIT_FARM_HUMANS,               331 },
    { WAR_UNIT_FARM_ORCS,                 332 },
    { WAR_UNIT_BARRACKS_HUMANS,           333 },
    { WAR_UNIT_BARRACKS_ORCS,             334 },
    { WAR_UNIT_CHURCH,                    335 },
    { WAR_UNIT_TEMPLE,                    336 },
    { WAR_UNIT_TOWER_HUMANS,              337 },
    { WAR_UNIT_TOWER_ORCS,                338 },
    { WAR_UNIT_TOWNHALL_HUMANS,           339 },
    { WAR_UNIT_TOWNHALL_ORCS,             340 },
    { WAR_UNIT_LUMBERMILL_HUMANS,         341 },
    { WAR_UNIT_LUMBERMILL_ORCS,           342 },
    { WAR_UNIT_STABLE,                    343 },
    { WAR_UNIT_KENNEL,                    344 },
    { WAR_UNIT_BLACKSMITH_HUMANS,         345 },
    { WAR_UNIT_BLACKSMITH_ORCS,           346 },
};

typedef struct
{
    WarRoadPieceType type;
    s32 tileIndexForest;
    s32 tileIndexSwamp;
} WarRoadData;

const WarRoadData roadsData[] =
{
    { WAR_ROAD_PIECE_LEFT,                56, 57 },
    { WAR_ROAD_PIECE_TOP,                 57, 58 },
    { WAR_ROAD_PIECE_RIGHT,               58, 59 },
    { WAR_ROAD_PIECE_BOTTOM,              59, 60 },
    { WAR_ROAD_PIECE_BOTTOM_LEFT,         60, 61 },
    { WAR_ROAD_PIECE_VERTICAL,            61, 62 },
    { WAR_ROAD_PIECE_BOTTOM_RIGHT,        62, 63 },
    { WAR_ROAD_PIECE_T_LEFT,              63, 64 },
    { WAR_ROAD_PIECE_T_BOTTOM,            64, 65 },
    { WAR_ROAD_PIECE_T_RIGHT,             65, 66 },
    { WAR_ROAD_PIECE_CROSS,               66, 67 },
    { WAR_ROAD_PIECE_TOP_LEFT,            67, 68 },
    { WAR_ROAD_PIECE_HORIZONTAL,          68, 69 },
    { WAR_ROAD_PIECE_T_TOP,               69, 70 },
    { WAR_ROAD_PIECE_TOP_RIGHT,           70, 71 },
};

// this is a map of the configurations of the neighbors of a tile to a road tile type.
// each index represent one configuration of the 16 possibles and the value at that index
// is the tile the tree should get based on that configuration.
const WarRoadPieceType roadTileTypeMap[16] =
{
    // 0-15
    0, 3, 0, 4, 1, 5, 11, 7, 2, 6, 12, 8, 14, 9, 13, 10,
};

typedef struct
{
    WarWallPieceType type;
    s32 tileForest;
    s32 tileDamagedForest;
    s32 tileDestroyedForest;
    s32 tileSwamp;
    s32 tileDamagedSwamp;
    s32 tileDestroyedSwamp;
} WarWallData;

const WarWallData wallsData[] =
{
    { WAR_WALL_PIECE_LEFT,                18, 30, 37, 18, 26, 37 },
    { WAR_WALL_PIECE_TOP,                 21, 23, 34, 21, 23, 34 },
    { WAR_WALL_PIECE_RIGHT,               18, 30, 37, 18, 26, 37 },
    { WAR_WALL_PIECE_BOTTOM,              11, 40, 34, 11, 40, 34 },
    { WAR_WALL_PIECE_BOTTOM_LEFT,         10, 22, 36, 10, 22, 36 },
    { WAR_WALL_PIECE_VERTICAL,            21, 23, 34, 21, 23, 34 },
    { WAR_WALL_PIECE_BOTTOM_RIGHT,        12, 24, 38, 12, 24, 38 },
    { WAR_WALL_PIECE_T_LEFT,              13, 25, 36, 13, 25, 36 },
    { WAR_WALL_PIECE_T_BOTTOM,            14, 26, 39, 14, 26, 39 },
    { WAR_WALL_PIECE_T_RIGHT,             15, 27, 38, 15, 27, 38 },
    { WAR_WALL_PIECE_CROSS,               16, 28, 39, 16, 28, 39 },
    { WAR_WALL_PIECE_TOP_LEFT,            17, 29, 33, 17, 29, 33 },
    { WAR_WALL_PIECE_HORIZONTAL,          18, 30, 37, 18, 30, 37 },
    { WAR_WALL_PIECE_T_TOP,               19, 31, 37, 19, 31, 37 },
    { WAR_WALL_PIECE_TOP_RIGHT,           20, 32, 35, 20, 32, 35 },
};

// this is a map of the configurations of the neighbors of a tile to a wall tile type.
// each index represent one configuration of the 16 possibles and the value at that index
// is the tile the tree should get based on that configuration.
const WarWallPieceType wallTileTypeMap[16] =
{
    // 0-15
    0, 3, 0, 4, 1, 5, 11, 7, 2, 6, 12, 8, 14, 9, 13, 10,
};

#define WAR_WALL_GOLD_COST 100
#define WAR_WALL_WOOD_COST 0
#define WAR_WALL_BUILD_TIME __bts(200)
#define WAR_WALL_MAX_HP 60

#define WAR_ROAD_GOLD_COST 50
#define WAR_ROAD_WOOD_COST 0

#define GOD_MODE_MIN_DAMAGE 255

typedef struct
{
    WarRuinPieceType type;
    s32 tileIndexForest;
    s32 tileIndexSwamp;
} WarRuinData;

const WarRuinData ruinsData[] =
{
    { WAR_RUIN_PIECE_TOP_LEFT,            41, 42 },
    { WAR_RUIN_PIECE_TOP,                 42, 43 },
    { WAR_RUIN_PIECE_TOP_RIGHT,           43, 44 },
    { WAR_RUIN_PIECE_LEFT,                44, 45 },
    { WAR_RUIN_PIECE_CENTER,              45, 46 },
    { WAR_RUIN_PIECE_RIGHT,               46, 47 },
    { WAR_RUIN_PIECE_BOTTOM_LEFT,         47, 48 },
    { WAR_RUIN_PIECE_BOTTOM,              48, 49 },
    { WAR_RUIN_PIECE_BOTTOM_RIGHT,        49, 50 },
    { WAR_RUIN_PIECE_TOP_LEFT_INSIDE,     50, 51 },
    { WAR_RUIN_PIECE_TOP_RIGHT_INSIDE,    51, 52 },
    { WAR_RUIN_PIECE_BOTTOM_LEFT_INSIDE,  53, 54 },
    { WAR_RUIN_PIECE_BOTTOM_RIGHT_INSIDE, 52, 53 },
    { WAR_RUIN_PIECE_DIAG_1,              55, 56 },
    { WAR_RUIN_PIECE_DIAG_2,              54, 55 },
};

// this is a map of the configurations of the neighbors of a tile to a ruin tile type.
// each index represent one configuration of the 256 possibles and the value at that index
// is the tile the tree should get based on that configuration.
const WarRuinPieceType ruinTileTypeMap[256] =
{
    // 0-15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 16-31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 32-47
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0,
    // 48-63
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 4, 4,
    // 64-79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 80-95
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 96-111
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 112-127
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 4, 4,
    // 128-143
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 144-159
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 160-175
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 176-191
    0, 0, 0, 9, 0, 0, 0, 9, 1, 1, 1, 14, 1, 1, 4, 11,
    // 192-207
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 208-223
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 224-239
    3, 3, 3, 6, 3, 3, 3, 6, 3, 3, 3, 6, 3, 3, 15, 10,
    // 240-255
    3, 3, 3, 6, 3, 3, 3, 6, 2, 2, 2, 12, 2, 2, 13, 5,
};

typedef struct
{
    WarTreeTileType type;
    s32 tileIndexForest;
    s32 tileIndexSwamp;
} WarTreeData;

const WarTreeData treesData[] =
{
    { WAR_TREE_NONE,                 0,  0 },
    { WAR_TREE_TOP_LEFT,            73, 74 },
    { WAR_TREE_TOP,                 75, 76 },
    { WAR_TREE_TOP_RIGHT,           76, 77 },
    { WAR_TREE_LEFT,                72, 73 },
    { WAR_TREE_CENTER,              94, 95 },
    { WAR_TREE_RIGHT,               78, 79 },
    { WAR_TREE_BOTTOM_LEFT,         71, 72 },
    { WAR_TREE_BOTTOM,              93, 94 },
    { WAR_TREE_BOTTOM_RIGHT,        79, 80 },
    { WAR_TREE_TOP_LEFT_INSIDE,     80, 81 },
    { WAR_TREE_TOP_RIGHT_INSIDE,    81, 82 },
    { WAR_TREE_BOTTOM_LEFT_INSIDE,  77, 78 },
    { WAR_TREE_BOTTOM_RIGHT_INSIDE, 74, 75 },
    { WAR_TREE_TOP_END,             90, 91 },
    { WAR_TREE_BOTTOM_END,          92, 93 },
    { WAR_TREE_VERTICAL,            91, 92 },
    { WAR_TREE_DIAG_1,              83, 84 },
    { WAR_TREE_DIAG_2,              82, 83 },
    { WAR_TREE_CHOPPED,             95, 96 },
};

// this is a map of the configurations of the neighbors of a tile to a tree tile type.
// each index represent one configuration of the 256 possibles and the value at that index
// is the tile the tree should get based on that configuration.
const WarTreeTileType treeTileTypeMap[256] =
{
    // 0-15
    0, 0, 15, 15, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 7, 7,
    // 16-31
    0, 0, 15, 15, 0, 0, 15, 15, 0, 0, 15, 15, 0, 0, 7, 7,
    // 32-47
    14, 14, 16, 16, 14, 14, 16, 16, 0, 0, 0, 0, 14, 14, 4, 4,
    // 48-63
    14, 14, 16, 16, 14, 14, 16, 16, 1, 1, 4, 4, 1, 1, 4, 4,
    // 64-79
    0, 0, 15, 15, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 7, 7,
    // 80-95
    0, 0, 15, 15, 0, 0, 15, 15, 0, 0, 7, 15, 0, 0, 7, 7,
    // 96-111
    14, 14, 16, 16, 14, 14, 16, 16, 1, 0, 4, 0, 14, 1, 4, 4,
    // 112-127
    14, 14, 16, 16, 14, 14, 16, 16, 1, 1, 4, 4, 1, 1, 4, 4,
    // 128-143
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 8, 0, 0, 8, 8,
    // 144-159
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 8, 9, 0, 0, 8, 8,
    // 160-175
    0, 14, 0, 6, 0, 3, 6, 6, 2, 0, 0, 0, 0, 14, 0, 5,
    // 176-191
    0, 14, 0, 6, 0, 14, 0, 6, 0, 1, 0, 18, 0, 14, 5, 11,
    // 192-207
    0, 0, 15, 9, 0, 0, 15, 9, 0, 0, 0, 0, 0, 0, 7, 8,
    // 208-223
    0, 0, 9, 9, 0, 0, 15, 9, 0, 0, 15, 9, 0, 0, 7, 8,
    // 224-239
    3, 3, 6, 6, 3, 3, 6, 6, 2, 2, 0, 5, 3, 2, 17, 10,
    // 240-255
    3, 3, 6, 6, 3, 3, 6, 6, 2, 2, 5, 12, 2, 2, 13, 5,
};

// this is a map of the configurations of the neighbors of a tile to a ruin tile type.
// each index represent one configuration of the 256 possibles and the value at that index
// is the tile the tree should get based on that configuration.
const WarFogPieceType fogTileTypeMap[256] =
{
    // 0-15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 16-31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 32-47
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0,
    // 48-63
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 4, 4,
    // 64-79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 80-95
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 96-111
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
    // 112-127
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 4, 4,
    // 128-143
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 144-159
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 160-175
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 176-191
    0, 0, 0, 9, 0, 0, 0, 9, 1, 1, 1, 0, 1, 1, 4, 0,
    // 192-207
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 208-223
    0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 7, 8,
    // 224-239
    3, 3, 3, 6, 3, 3, 3, 6, 3, 3, 3, 6, 3, 3, 0, 0,
    // 240-255
    3, 3, 3, 6, 3, 3, 3, 6, 2, 2, 0, 0, 2, 2, 0, 5,
};

typedef struct
{
    WarUnitType type;
    s32 range;
    s32 sight;
    s32 armor;
    s32 hp;
    s32 mana;
    s32 minDamage;
    s32 rndDamage;
    s32 buildTime;
    s32 goldCost;
    s32 woodCost;
    s32 decay;
    f32 speeds[3];
} WarUnitStats;

// Move speeds (equivalent for orcs):
// Scorpion, Knight (upgrade 2)             = 1.629 bps = 1.629 bps * 16 px = 26.064 pxs = 26 pxs
// Knight (upgrade 1)                       = 1.379 bps = 1.379 bps * 16 px = 22.064 pxs = 22 pxs
// Peasant, Archer, Knight, Lothar, Medivh  = 1.224 bps = 1.224 bps * 16 px = 19.584 pxs = 19 pxs
// Elemental                                = 1.121 bps = 1.121 bps * 16 px = 17.936 pxs = 18 pxs
// Footman                                  = 1.046 bps = 1.046 bps * 16 px = 16.736 pxs = 17 pxs
// Cleric, Conjurer, Griselda, Garona       = 0.874 bps = 0.874 bps * 16 px = 13.984 pxs = 14 pxs
// Catapult                                 = 0.562 bps = 0.562 bps * 16 px =  8.992 pxs =  9 pxs

// Build times in the document Warcraft: Orcs & Humans Insider's Guide have some inconsistencies
// or at least I couldn't understanding it. So I'm taking the formula from the statement (also in that document)
// that the TownHall which have 1000 build units last aprox. 80 seconds
//
// Unit build times (equivalent for orcs):
// FOOTMAN,                      600 program cycles = 48s
// PEASANT,                      750 program cycles = 60s
// CATAPULT_HUMANS              1000 program cycles = 80s
// KNIGHT,                       800 program cycles = 64s
// ARCHER,                       700 program cycles = 56s
// CONJURER,                     900 program cycles = 72s
// CLERIC,                       800 program cycles = 64s
//
// Building build times (equivalent for orcs):
// FARM_HUMANS,                 1000 program cycles =  80s
// BARRACKS_HUMANS,             1500 program cycles = 120s
// CHURCH,                      2000 program cycles = 160s
// TOWER_HUMANS,                2000 program cycles = 160s
// TOWNHALL_HUMANS,             1000 program cycles =  80s
// LUMBERMILL_HUMANS,           1500 program cycles = 120s
// STABLE,                     1500 program cycles = 120s
// BLACKSMITH_HUMANS,           1500 program cycles = 120s
//
// build time in seconds
#define __bts(t) ((t)*80/1000)

const WarUnitStats unitStats[] =
{
    // unit type                range  sight  armor   hp   mana   min D.  rnd D.       build    gold    lumber   decay    speed in pixels x seconds
    { WAR_UNIT_FOOTMAN,           1,     2,     2,     60,   -1,     1,      9,    __bts(600),    400,      0,     -1,   { 16.736f, 16.736f, 16.736f } },
    { WAR_UNIT_GRUNT,             1,     2,     2,     60,   -1,     1,      9,    __bts(600),    400,      0,     -1,   { 16.736f, 16.736f, 16.736f } },
    { WAR_UNIT_PEASANT,           1,     2,     0,     40,   -1,     0,      0,    __bts(750),    400,      0,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_PEON,              1,     2,     0,     40,   -1,     0,      0,    __bts(750),    400,      0,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_CATAPULT_HUMANS,   8,     5,     0,    120,   -1,     0,    255,    __bts(1000),   900,    200,     -1,   {  8.992f,  8.992f,  8.992f } },
    { WAR_UNIT_CATAPULT_ORCS,     8,     5,     0,    120,   -1,     0,    255,    __bts(1000),   900,    200,     -1,   {  8.992f,  8.992f,  8.992f } },
    { WAR_UNIT_KNIGHT,            1,     4,     5,     90,   -1,     1,     13,    __bts(800),    850,      0,     -1,   { 19.584f, 22.064f, 26.064f } },
    { WAR_UNIT_RAIDER,            1,     4,     5,     90,   -1,     1,     13,    __bts(800),    850,      0,     -1,   { 19.584f, 22.064f, 26.064f } },
    { WAR_UNIT_ARCHER,            5,     5,     1,     60,   -1,     4,      0,    __bts(700),    450,     50,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_SPEARMAN,          4,     4,     1,     60,   -1,     5,      0,    __bts(700),    450,     50,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_CONJURER,          3,     4,     0,     40,  255,     6,      0,    __bts(900),    900,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_WARLOCK,           2,     3,     0,     40,  255,     6,      0,    __bts(900),    900,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_CLERIC,            1,     3,     0,     40,  255,     6,      0,    __bts(800),    700,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_NECROLYTE,         2,     3,     0,     40,  255,     6,      0,    __bts(800),    700,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_MEDIVH,            5,     5,     0,    110,   -1,    10,      0,            -1,     -1,     -1,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_LOTHAR,            1,     2,     5,     50,   -1,     1,     15,            -1,     -1,     -1,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_GRIZELDA,          1,     2,     0,     30,   -1,    -1,     -1,            -1,     -1,     -1,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_GARONA,            1,     2,     0,     30,   -1,    -1,     -1,            -1,     -1,     -1,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_OGRE,              1,     2,     3,     60,   -1,     1,     12,            -1,     -1,     -1,     -1,   { 17.936f, 17.936f, 17.936f } },
    { WAR_UNIT_SPIDER,            1,     2,     0,     30,  255,     1,      3,            -1,     -1,     -1,     45,   { 26.064f, 26.064f, 26.064f } },
    { WAR_UNIT_SLIME,             1,     2,    10,    150,   -1,     1,      0,            -1,     -1,     -1,     -1,   { 26.064f, 26.064f, 26.064f } },
    { WAR_UNIT_FIRE_ELEMENTAL,    1,     2,     0,    200,   -1,     0,     40,            -1,     -1,     -1,     -1,   { 17.936f, 17.936f, 17.936f } },
    { WAR_UNIT_SCORPION,          1,     2,     0,     30,  255,     3,      0,            -1,     -1,     -1,     45,   { 26.064f, 26.064f, 26.064f } },
    { WAR_UNIT_BRIGAND,           1,     2,     1,     40,   -1,     1,      9,            -1,     -1,     -1,     -1,   { 16.736f, 16.736f, 16.736f } },
    { WAR_UNIT_THE_DEAD,          1,     2,     2,     30,  255,     1,      9,            -1,     -1,     -1,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_SKELETON,          1,     2,     1,     40,   -1,     1,      4,            -1,     -1,     -1,     45,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_DAEMON,            1,     2,     0,    300,  255,     0,     65,            -1,     -1,     -1,     45,   { 17.936f, 17.936f, 17.936f } },
    { WAR_UNIT_WATER_ELEMENTAL,   3,     4,     0,    250,  255,    40,      0,            -1,     -1,     -1,     45,   { 17.936f, 17.936f, 17.936f } },
};

typedef struct
{
    WarUnitType type;
    s32 armor;
    s32 sight;
    s32 hp;
    s32 buildTime;
    s32 goldCost;
    s32 woodCost;
} WarBuildingStats;

const WarBuildingStats buildingStats[] =
{
    // building type                    armor    sight     hp            build     gold    lumber
    { WAR_UNIT_FARM_HUMANS,               0,      2,       400,     __bts(1000),    500,    300 },
    { WAR_UNIT_FARM_ORCS,                 0,      2,       400,     __bts(1000),    500,    300 },
    { WAR_UNIT_BARRACKS_HUMANS,           0,      3,       800,     __bts(1500),    600,    500 },
    { WAR_UNIT_BARRACKS_ORCS,             0,      3,       800,     __bts(1500),    600,    500 },
    { WAR_UNIT_CHURCH,                    0,      3,       700,     __bts(2000),    800,    500 },
    { WAR_UNIT_TEMPLE,                    0,      3,       700,     __bts(2000),    800,    500 },
    { WAR_UNIT_TOWER_HUMANS,              0,      2,       900,     __bts(2000),   1400,    300 },
    { WAR_UNIT_TOWER_ORCS,                0,      2,       900,     __bts(2000),   1400,    300 },
    { WAR_UNIT_TOWNHALL_HUMANS,           0,      3,      2500,     __bts(1000),    400,    400 },
    { WAR_UNIT_TOWNHALL_ORCS,             0,      3,      2500,     __bts(1000),    400,    400 },
    { WAR_UNIT_LUMBERMILL_HUMANS,         0,      3,       600,     __bts(1500),    600,    500 },
    { WAR_UNIT_LUMBERMILL_ORCS,           0,      3,       600,     __bts(1500),    600,    500 },
    { WAR_UNIT_STABLE,                    0,      3,       500,     __bts(1500),   1000,    400 },
    { WAR_UNIT_KENNEL,                    0,      3,       500,     __bts(1500),   1000,    400 },
    { WAR_UNIT_BLACKSMITH_HUMANS,         0,      2,       800,     __bts(1500),    900,    400 },
    { WAR_UNIT_BLACKSMITH_ORCS,           0,      2,       800,     __bts(1500),    900,    400 },
    { WAR_UNIT_STORMWIND,                 0,      4,      5000,              -1,     -1,     -1 },
    { WAR_UNIT_BLACKROCK,                 0,      4,      5000,              -1,     -1,     -1 },
    { WAR_UNIT_GOLDMINE,                  0,      3,     25500,              -1,     -1,     -1 },
};

typedef struct
{
    WarUpgradeType type;
    s32 maxLevelAllowed;
    s32 frameIndices[2];
} WarUpgradeData;

const WarUpgradeData upgradesData[] =
{
    { WAR_UPGRADE_ARROWS,           2, { WAR_PORTRAIT_ARROW_2,          WAR_PORTRAIT_ARROW_3         } },
    { WAR_UPGRADE_SPEARS,           2, { WAR_PORTRAIT_SPEAR_2,          WAR_PORTRAIT_SPEAR_3         } },
    { WAR_UPGRADE_SWORDS,           2, { WAR_PORTRAIT_SWORD_2,          WAR_PORTRAIT_SWORD_3         } },
    { WAR_UPGRADE_AXES,             2, { WAR_PORTRAIT_AXE_2,            WAR_PORTRAIT_AXE_3           } },
    { WAR_UPGRADE_HORSES,           2, { WAR_PORTRAIT_HORSE_1,          WAR_PORTRAIT_HORSE_2         } },
    { WAR_UPGRADE_WOLVES,           2, { WAR_PORTRAIT_WOLVES_1,         WAR_PORTRAIT_WOLVES_2        } },
    { WAR_UPGRADE_SCORPIONS,        1, { WAR_PORTRAIT_SCORPION,         WAR_PORTRAIT_SCORPION        } },
    { WAR_UPGRADE_SPIDERS,          1, { WAR_PORTRAIT_SPIDER,           WAR_PORTRAIT_SPIDER          } },
    { WAR_UPGRADE_RAIN_OF_FIRE,     1, { WAR_PORTRAIT_RAIN_OF_FIRE,     WAR_PORTRAIT_RAIN_OF_FIRE    } },
    { WAR_UPGRADE_POISON_CLOUD,     1, { WAR_PORTRAIT_POISON_CLOUD,     WAR_PORTRAIT_POISON_CLOUD    } },
    { WAR_UPGRADE_WATER_ELEMENTAL,  1, { WAR_PORTRAIT_WATER_ELEMENTAL,  WAR_PORTRAIT_WATER_ELEMENTAL } },
    { WAR_UPGRADE_DAEMON,           1, { WAR_PORTRAIT_DAEMON,           WAR_PORTRAIT_DAEMON          } },
    { WAR_UPGRADE_HEALING,          1, { WAR_PORTRAIT_HEALING,          WAR_PORTRAIT_HEALING         } },
    { WAR_UPGRADE_RAISE_DEAD,       1, { WAR_PORTRAIT_RAISE_DEAD,       WAR_PORTRAIT_RAISE_DEAD      } },
    { WAR_UPGRADE_FAR_SIGHT,        1, { WAR_PORTRAIT_FAR_SIGHT,        WAR_PORTRAIT_FAR_SIGHT       } },
    { WAR_UPGRADE_DARK_VISION,      1, { WAR_PORTRAIT_DARK_VISION,      WAR_PORTRAIT_DARK_VISION     } },
    { WAR_UPGRADE_INVISIBILITY,     1, { WAR_PORTRAIT_INVISIBILITY,     WAR_PORTRAIT_INVISIBILITY    } },
    { WAR_UPGRADE_UNHOLY_ARMOR,     1, { WAR_PORTRAIT_UNHOLY_ARMOR,     WAR_PORTRAIT_UNHOLY_ARMOR    } },
    { WAR_UPGRADE_SHIELD,           2, { WAR_PORTRAIT_SHIELD_2_HUMANS,  WAR_PORTRAIT_SHIELD_3_HUMANS } },
};

typedef struct
{
    WarUpgradeType type;
    s32 buildTime;
    s32 goldCost[2];
} WarUpgradeStats;

const WarUpgradeStats upgradeStats[] =
{
    { WAR_UPGRADE_ARROWS,           __bts(1600), {  750, 1500 } },
    { WAR_UPGRADE_SPEARS,           __bts(1600), {  750, 1500 } },
    { WAR_UPGRADE_SWORDS,           __bts(1750), {  750, 1500 } },
    { WAR_UPGRADE_AXES,             __bts(1750), {  750, 1500 } },
    { WAR_UPGRADE_HORSES,           __bts(1750), {  750, 1500 } },
    { WAR_UPGRADE_WOLVES,           __bts(1750), {  750, 1500 } },
    { WAR_UPGRADE_SCORPIONS,        __bts(1750), {  750,    0 } },
    { WAR_UPGRADE_SPIDERS,          __bts(1750), {  750,    0 } },
    { WAR_UPGRADE_RAIN_OF_FIRE,     __bts(1750), { 1500,    0 } },
    { WAR_UPGRADE_POISON_CLOUD,     __bts(1750), { 1500,    0 } },
    { WAR_UPGRADE_WATER_ELEMENTAL,  __bts(1750), { 3000,    0 } },
    { WAR_UPGRADE_DAEMON,           __bts(1750), { 3000,    0 } },
    { WAR_UPGRADE_HEALING,          __bts(1600), {  750,    0 } },
    { WAR_UPGRADE_RAISE_DEAD,       __bts(1600), {  750,    0 } },
    { WAR_UPGRADE_FAR_SIGHT,        __bts(1600), { 1500,    0 } },
    { WAR_UPGRADE_DARK_VISION,      __bts(1600), { 1500,    0 } },
    { WAR_UPGRADE_INVISIBILITY,     __bts(1600), { 3000,    0 } },
    { WAR_UPGRADE_UNHOLY_ARMOR,     __bts(1600), { 3000,    0 } },
    { WAR_UPGRADE_SHIELD,           __bts(1600), {  750, 1500 } },
};

typedef struct
{
    WarSpellType type;
    s32 portraitFrameIndex;
} WarSpellData;

const WarSpellData spellData[] =
{
    // summons
    { WAR_SUMMON_SPIDER,            WAR_PORTRAIT_SPIDER             },
    { WAR_SUMMON_SCORPION,          WAR_PORTRAIT_SCORPION           },
    { WAR_SUMMON_DAEMON,            WAR_PORTRAIT_DAEMON             },
    { WAR_SUMMON_WATER_ELEMENTAL,   WAR_PORTRAIT_WATER_ELEMENTAL    },

    // spells
    { WAR_SPELL_HEALING,            WAR_PORTRAIT_HEALING            },
    { WAR_SPELL_FAR_SIGHT,          WAR_PORTRAIT_FAR_SIGHT          },
    { WAR_SPELL_INVISIBILITY,       WAR_PORTRAIT_INVISIBILITY       },
    { WAR_SPELL_RAIN_OF_FIRE,       WAR_PORTRAIT_RAIN_OF_FIRE       },
    { WAR_SPELL_RAISE_DEAD,         WAR_PORTRAIT_RAISE_DEAD         },
    { WAR_SPELL_DARK_VISION,        WAR_PORTRAIT_DARK_VISION        },
    { WAR_SPELL_UNHOLY_ARMOR,       WAR_PORTRAIT_UNHOLY_ARMOR       },
    { WAR_SPELL_POISON_CLOUD,       WAR_PORTRAIT_POISON_CLOUD       },
};

typedef struct
{
    WarSpellType type;
    s32 manaCost;
    f32 time;
    s32 range;
} WarSpellStats;

const WarSpellStats spellStats[] =
{
    // summons
    { WAR_SUMMON_SPIDER,             51,  0,  0 },
    { WAR_SUMMON_SCORPION,           51,  0,  0 },
    { WAR_SUMMON_DAEMON,            255,  0,  0 },
    { WAR_SUMMON_WATER_ELEMENTAL,   250,  0,  0 },

    // spells
    { WAR_SPELL_HEALING,              6,  0,  6 },
    { WAR_SPELL_FAR_SIGHT,           70, 10,  0 },
    { WAR_SPELL_INVISIBILITY,       200, 50,  6 },
    { WAR_SPELL_RAIN_OF_FIRE,        25,  0, 12 },
    { WAR_SPELL_RAISE_DEAD,          50,  0,  6 },
    { WAR_SPELL_DARK_VISION,         50, 10,  0 },
    { WAR_SPELL_UNHOLY_ARMOR,       100, 13,  6 },
    { WAR_SPELL_POISON_CLOUD,       100, 10, 12 },
};

typedef struct
{
    WarSpellType type;
    s32 mappedType;
} WarSpellMapping;

const WarSpellMapping spellMappings[] =
{
    // summon
    { WAR_SUMMON_SPIDER,            WAR_UNIT_SPIDER             },
    { WAR_SUMMON_SCORPION,          WAR_UNIT_SCORPION           },
    { WAR_SUMMON_DAEMON,            WAR_UNIT_DAEMON             },
    { WAR_SUMMON_WATER_ELEMENTAL,   WAR_UNIT_WATER_ELEMENTAL    },
};

typedef struct
{
    WarUnitCommandType type;
    WarClickHandler clickHandler;
    WarKeys hotKey;
    s32 highlightIndex;
    s32 highlightCount;
    StringView tooltip;
    StringView tooltip2;
} WarUnitCommandBaseData;

const WarUnitCommandBaseData commandsBaseData[] =
{
    { WAR_COMMAND_NONE,                     NULL,                   WAR_KEY_NONE,      -1, 1, WSV_INIT(""),                             WSV_INIT("") },

    // unit commands
    { WAR_COMMAND_MOVE,                     move,                   WAR_KEY_M,          0, 1, WSV_INIT("MOVE"),                         WSV_INIT("") },
    { WAR_COMMAND_STOP,                     stop,                   WAR_KEY_S,          0, 1, WSV_INIT("STOP"),                         WSV_INIT("") },
    { WAR_COMMAND_HARVEST,                  harvest,                WAR_KEY_H,          0, 1, WSV_INIT("HARVEST LUMBER/MINE GOLD"),     WSV_INIT("") },
    { WAR_COMMAND_DELIVER,                  deliver,                WAR_KEY_T,         16, 1, WSV_INIT("RETURN GOODS TO TOWN HALL"),    WSV_INIT("") },
    { WAR_COMMAND_REPAIR,                   repair,                 WAR_KEY_R,          0, 1, WSV_INIT("REPAIR"),                       WSV_INIT("") },
    { WAR_COMMAND_BUILD_BASIC,              buildBasic,             WAR_KEY_B,          0, 1, WSV_INIT("BUILD BASIC STRUCTURE"),        WSV_INIT("") },
    { WAR_COMMAND_BUILD_ADVANCED,           buildAdvanced,          WAR_KEY_A,          6, 1, WSV_INIT("BUILD ADVANCED STRUCTURE"),     WSV_INIT("") },
    { WAR_COMMAND_ATTACK,                   attack,                 WAR_KEY_A,          0, 1, WSV_INIT("ATTACK"),                       WSV_INIT("") },

    // train commands
    { WAR_COMMAND_TRAIN_FOOTMAN,            trainFootman,           WAR_KEY_F,          6, 1, WSV_INIT("TRAIN FOOTMAN"),                WSV_INIT("TRAINING A FOOTMAN")        },
    { WAR_COMMAND_TRAIN_GRUNT,              trainGrunt,             WAR_KEY_U,          8, 1, WSV_INIT("TRAIN GRUNT"),                  WSV_INIT("TRAINING A GRUNT")          },
    { WAR_COMMAND_TRAIN_PEASANT,            trainPeasant,           WAR_KEY_P,          6, 1, WSV_INIT("TRAIN PEASANT"),                WSV_INIT("TRAINING A PEASANT")        },
    { WAR_COMMAND_TRAIN_PEON,               trainPeon,              WAR_KEY_P,          6, 1, WSV_INIT("TRAIN PEON"),                   WSV_INIT("TRAINING A PEON")           },
    { WAR_COMMAND_TRAIN_CATAPULT_HUMANS,    trainHumanCatapult,     WAR_KEY_T,          8, 1, WSV_INIT("BUILD CATAPULT"),               WSV_INIT("TRAINING A CATAPULT CREW")  },
    { WAR_COMMAND_TRAIN_CATAPULT_ORCS,      trainOrcCatapult,       WAR_KEY_T,          8, 1, WSV_INIT("BUILD CATAPULT"),               WSV_INIT("TRAINING A CATAPULT CREW")  },
    { WAR_COMMAND_TRAIN_KNIGHT,             trainKnight,            WAR_KEY_K,          6, 1, WSV_INIT("TRAIN KNIGHT"),                 WSV_INIT("TRAINING A KNIGHT")         },
    { WAR_COMMAND_TRAIN_RAIDER,             trainRaider,            WAR_KEY_R,          6, 1, WSV_INIT("TRAIN RAIDER"),                 WSV_INIT("TRAINING A RAIDER")         },
    { WAR_COMMAND_TRAIN_ARCHER,             trainArcher,            WAR_KEY_A,          6, 1, WSV_INIT("TRAIN ARCHER"),                 WSV_INIT("TRAINING AN ARCHER")        },
    { WAR_COMMAND_TRAIN_SPEARMAN,           trainSpearman,          WAR_KEY_S,          6, 1, WSV_INIT("TRAIN SPEARMAN"),               WSV_INIT("TRAINING A SPEARMAN")       },
    { WAR_COMMAND_TRAIN_CONJURER,           trainConjurer,          WAR_KEY_T,          0, 1, WSV_INIT("TRAIN CONJURER"),               WSV_INIT("TRAINING A CONJURER")       },
    { WAR_COMMAND_TRAIN_WARLOCK,            trainWarlock,           WAR_KEY_T,          0, 1, WSV_INIT("TRAIN WARLOCK"),                WSV_INIT("TRAINING A WARLOCK")        },
    { WAR_COMMAND_TRAIN_CLERIC,             trainCleric,            WAR_KEY_T,          0, 1, WSV_INIT("TRAIN CLERIC"),                 WSV_INIT("TRAINING A CLERIC")         },
    { WAR_COMMAND_TRAIN_NECROLYTE,          trainNecrolyte,         WAR_KEY_T,          0, 1, WSV_INIT("TRAIN NECROLYTE"),              WSV_INIT("TRAINING A NECROLYTE")      },

    // spell commands
    { WAR_COMMAND_SPELL_HEALING,            castHeal,               WAR_KEY_H,          0, 1, WSV_INIT("HEALING"),                      WSV_INIT("")                          },
    { WAR_COMMAND_SPELL_POISON_CLOUD,       castPoisonCloud,        WAR_KEY_P,          9, 1, WSV_INIT("CLOUD OF POISON"),              WSV_INIT("")                          },
    { WAR_COMMAND_SPELL_FAR_SIGHT,          castFarSight,           WAR_KEY_F,          0, 1, WSV_INIT("FAR SEEING"),                   WSV_INIT("")                          },
    { WAR_COMMAND_SPELL_DARK_VISION,        castDarkVision,         WAR_KEY_D,          0, 1, WSV_INIT("DARK VISION"),                  WSV_INIT("")                          },
    { WAR_COMMAND_SPELL_INVISIBILITY,       castInvisibility,       WAR_KEY_I,          0, 1, WSV_INIT("INVISIBILITY"),                 WSV_INIT("")                          },
    { WAR_COMMAND_SPELL_UNHOLY_ARMOR,       castUnHolyArmor,        WAR_KEY_U,          0, 1, WSV_INIT("UNHOLY ARMOR"),                 WSV_INIT("")                          },
    { WAR_COMMAND_SPELL_RAIN_OF_FIRE,       castRainOfFire,         WAR_KEY_R,          0, 1, WSV_INIT("RAIN OF FIRE"),                 WSV_INIT("")                          },
    { WAR_COMMAND_SPELL_RAISE_DEAD,         castRaiseDead,          WAR_KEY_R,          0, 1, WSV_INIT("RAISE DEAD"),                   WSV_INIT("")                          },

    // summons
    { WAR_COMMAND_SUMMON_SCORPION,          summonScorpion,         WAR_KEY_O,          9, 1, WSV_INIT("SUMMON SCORPIONS"),             WSV_INIT("")                          },
    { WAR_COMMAND_SUMMON_SPIDER,            summonSpider,           WAR_KEY_R,         12, 1, WSV_INIT("SUMMON SPIDERS"),               WSV_INIT("")                          },
    { WAR_COMMAND_SUMMON_WATER_ELEMENTAL,   summonWaterElemental,   WAR_KEY_W,          7, 1, WSV_INIT("SUMMON WATER ELEMENTAL"),       WSV_INIT("")                          },
    { WAR_COMMAND_SUMMON_DAEMON,            summonDaemon,           WAR_KEY_D,          7, 1, WSV_INIT("SUMMON DAEMON"),                WSV_INIT("")                          },

    // build commands
    { WAR_COMMAND_BUILD_FARM_HUMANS,        buildFarmHumans,        WAR_KEY_F,          6, 1, WSV_INIT("BUILD FARM"),                   WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_FARM_ORCS,          buildFarmOrcs,          WAR_KEY_F,          6, 1, WSV_INIT("BUILD FARM"),                   WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_BARRACKS_HUMANS,    buildBarracksHumans,    WAR_KEY_B,          6, 1, WSV_INIT("BUILD BARRACKS"),               WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_BARRACKS_ORCS,      buildBarracksOrcs,      WAR_KEY_B,          6, 1, WSV_INIT("BUILD BARRACKS"),               WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_CHURCH,             buildChurch,            WAR_KEY_U,          8, 1, WSV_INIT("BUILD CHURCH"),                 WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_TEMPLE,             buildTemple,            WAR_KEY_E,          7, 1, WSV_INIT("BUILD TEMPLE"),                 WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_TOWER_HUMANS,       buildTowerHumans,       WAR_KEY_T,          6, 1, WSV_INIT("BUILD TOWER"),                  WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_TOWER_ORCS,         buildTowerOrcs,         WAR_KEY_T,          6, 1, WSV_INIT("BUILD TOWER"),                  WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_TOWNHALL_HUMANS,    buildTownHallHumans,    WAR_KEY_H,         11, 1, WSV_INIT("BUILD TOWN HALL"),              WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_TOWNHALL_ORCS,      buildTownHallOrcs,      WAR_KEY_H,         11, 1, WSV_INIT("BUILD TOWN HALL"),              WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_LUMBERMILL_HUMANS,  buildLumbermillHumans,  WAR_KEY_L,          6, 1, WSV_INIT("BUILD LUMBER MILL"),            WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_LUMBERMILL_ORCS,    buildLumbermillOrcs,    WAR_KEY_L,          6, 1, WSV_INIT("BUILD LUMBER MILL"),            WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_STABLE,             buildStable,            WAR_KEY_S,          6, 1, WSV_INIT("BUILD STABLES"),                WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_KENNEL,             buildKennel,            WAR_KEY_K,          6, 1, WSV_INIT("BUILD KENNEL"),                 WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_BLACKSMITH_HUMANS,  buildBlacksmithHumans,  WAR_KEY_B,          6, 1, WSV_INIT("BUILD BLACKSMITH"),             WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_BLACKSMITH_ORCS,    buildBlacksmithOrcs,    WAR_KEY_B,          6, 1, WSV_INIT("BUILD BLACKSMITH"),             WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_ROAD,               buildRoad,              WAR_KEY_R,          6, 1, WSV_INIT("BUILD ROAD"),                   WSV_INIT("")                          },
    { WAR_COMMAND_BUILD_WALL,               buildWall,              WAR_KEY_W,          6, 1, WSV_INIT("BUILD WALL"),                   WSV_INIT("")                          },

    // upgrades
    { WAR_COMMAND_UPGRADE_SWORDS,           upgradeSwords,          WAR_KEY_W,          9, 1, WSV_INIT("UPGRADE SWORD STRENGTH"),       WSV_INIT("RESEARCHING WEAPONRY")      },
    { WAR_COMMAND_UPGRADE_AXES,             upgradeAxes,            WAR_KEY_A,          8, 1, WSV_INIT("UPGRADE AXE STRENGTH"),         WSV_INIT("RESEARCHING WEAPONRY")      },
    { WAR_COMMAND_UPGRADE_SHIELD_HUMANS,    upgradeHumanShields,    WAR_KEY_H,          9, 1, WSV_INIT("UPGRADE SHIELD STRENGTH"),      WSV_INIT("RESEARCHING ARMOR")         },
    { WAR_COMMAND_UPGRADE_SHIELD_ORCS,      upgradeOrcsShields,     WAR_KEY_H,          9, 1, WSV_INIT("UPGRADE SHIELD STRENGTH"),      WSV_INIT("RESEARCHING ARMOR")         },
    { WAR_COMMAND_UPGRADE_ARROWS,           upgradeArrows,          WAR_KEY_U,          0, 1, WSV_INIT("UPGRADE ARROW STRENGTH"),       WSV_INIT("RESEARCHING WEAPONRY")      },
    { WAR_COMMAND_UPGRADE_SPEARS,           upgradeSpears,          WAR_KEY_U,          0, 1, WSV_INIT("UPGRADE SPEAR STRENGTH"),       WSV_INIT("RESEARCHING WEAPONRY")      },
    { WAR_COMMAND_UPGRADE_HORSES,           upgradeHorses,          WAR_KEY_B,          0, 1, WSV_INIT("BREED FASTER HORSES"),          WSV_INIT("BREADING BETTER STOCK")     },
    { WAR_COMMAND_UPGRADE_WOLVES,           upgradeWolves,          WAR_KEY_B,          0, 1, WSV_INIT("BREED FASTER WOLVES"),          WSV_INIT("BREADING BETTER STOCK")     },
    { WAR_COMMAND_UPGRADE_SCORPION,         upgradeScorpions,       WAR_KEY_M,          9, 1, WSV_INIT("RESEARCH MINOR SUMMONING"),     WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_SPIDER,           upgradeSpiders,         WAR_KEY_M,          9, 1, WSV_INIT("RESEARCH MINOR SUMMONING"),     WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_RAIN_OF_FIRE,     upgradeRainOfFire,      WAR_KEY_R,          9, 1, WSV_INIT("RESEARCH RAIN OF FIRE"),        WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_POISON_CLOUD,     upgradePoisonCloud,     WAR_KEY_P,         18, 1, WSV_INIT("RESEARCH CLOUD OF POISON"),     WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_WATER_ELEMENTAL,  upgradeWaterElemental,  WAR_KEY_A,         10, 1, WSV_INIT("RESEARCH MAJOR SUMMONING"),     WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_DAEMON,           upgradeDaemon,          WAR_KEY_A,         10, 1, WSV_INIT("RESEARCH MAJOR SUMMONING"),     WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_HEALING,          upgradeHealing,         WAR_KEY_H,          9, 1, WSV_INIT("RESEARCH HEALING"),             WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_RAISE_DEAD,       upgradeRaiseDead,       WAR_KEY_R,          9, 1, WSV_INIT("RESEARCH RAISING DEAD"),        WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_FAR_SIGHT,        upgradeFarSight,        WAR_KEY_F,          9, 1, WSV_INIT("RESEARCH FAR SEEING"),          WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_DARK_VISION,      upgradeDarkVision,      WAR_KEY_D,          9, 1, WSV_INIT("RESEARCH DARK VISION"),         WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_INVISIBILITY,     upgradeInvisibility,    WAR_KEY_I,          9, 1, WSV_INIT("RESEARCH INVISIBILITY"),        WSV_INIT("RESEARCHING NEW SPELL")     },
    { WAR_COMMAND_UPGRADE_UNHOLY_ARMOR,     upgradeUnholyArmor,     WAR_KEY_U,          9, 1, WSV_INIT("RESEARCH UNHOLY ARMOR"),        WSV_INIT("RESEARCHING NEW SPELL")     },

    // cancel
    { WAR_COMMAND_CANCEL,                   cancel,                 WAR_KEY_ESC,        0, 3, WSV_INIT("ESC - CANCEL"),                 WSV_INIT("")                          }
};

typedef struct
{
    WarUnitCommandType type;
    s32 mappedType;
} WarUnitCommandMapping;

const WarUnitCommandMapping commandMappings[] =
{
    // train commands
    { WAR_COMMAND_TRAIN_FOOTMAN,            WAR_UNIT_FOOTMAN            },
    { WAR_COMMAND_TRAIN_GRUNT,              WAR_UNIT_GRUNT              },
    { WAR_COMMAND_TRAIN_PEASANT,            WAR_UNIT_PEASANT            },
    { WAR_COMMAND_TRAIN_PEON,               WAR_UNIT_PEON               },
    { WAR_COMMAND_TRAIN_CATAPULT_HUMANS,    WAR_UNIT_CATAPULT_HUMANS    },
    { WAR_COMMAND_TRAIN_CATAPULT_ORCS,      WAR_UNIT_CATAPULT_ORCS      },
    { WAR_COMMAND_TRAIN_KNIGHT,             WAR_UNIT_KNIGHT             },
    { WAR_COMMAND_TRAIN_RAIDER,             WAR_UNIT_RAIDER             },
    { WAR_COMMAND_TRAIN_ARCHER,             WAR_UNIT_ARCHER             },
    { WAR_COMMAND_TRAIN_SPEARMAN,           WAR_UNIT_SPEARMAN           },
    { WAR_COMMAND_TRAIN_CONJURER,           WAR_UNIT_CONJURER           },
    { WAR_COMMAND_TRAIN_WARLOCK,            WAR_UNIT_WARLOCK            },
    { WAR_COMMAND_TRAIN_CLERIC,             WAR_UNIT_CLERIC             },
    { WAR_COMMAND_TRAIN_NECROLYTE,          WAR_UNIT_NECROLYTE          },

    // build commands
    { WAR_COMMAND_BUILD_FARM_HUMANS,        WAR_UNIT_FARM_HUMANS        },
    { WAR_COMMAND_BUILD_FARM_ORCS,          WAR_UNIT_FARM_ORCS          },
    { WAR_COMMAND_BUILD_BARRACKS_HUMANS,    WAR_UNIT_BARRACKS_HUMANS    },
    { WAR_COMMAND_BUILD_BARRACKS_ORCS,      WAR_UNIT_BARRACKS_ORCS      },
    { WAR_COMMAND_BUILD_CHURCH,             WAR_UNIT_CHURCH             },
    { WAR_COMMAND_BUILD_TEMPLE,             WAR_UNIT_TEMPLE             },
    { WAR_COMMAND_BUILD_TOWER_HUMANS,       WAR_UNIT_TOWER_HUMANS       },
    { WAR_COMMAND_BUILD_TOWER_ORCS,         WAR_UNIT_TOWER_ORCS         },
    { WAR_COMMAND_BUILD_TOWNHALL_HUMANS,    WAR_UNIT_TOWNHALL_HUMANS    },
    { WAR_COMMAND_BUILD_TOWNHALL_ORCS,      WAR_UNIT_TOWNHALL_ORCS      },
    { WAR_COMMAND_BUILD_LUMBERMILL_HUMANS,  WAR_UNIT_LUMBERMILL_HUMANS  },
    { WAR_COMMAND_BUILD_LUMBERMILL_ORCS,    WAR_UNIT_LUMBERMILL_ORCS    },
    { WAR_COMMAND_BUILD_STABLE,             WAR_UNIT_STABLE             },
    { WAR_COMMAND_BUILD_KENNEL,             WAR_UNIT_KENNEL             },
    { WAR_COMMAND_BUILD_BLACKSMITH_HUMANS,  WAR_UNIT_BLACKSMITH_HUMANS  },
    { WAR_COMMAND_BUILD_BLACKSMITH_ORCS,    WAR_UNIT_BLACKSMITH_ORCS    },

    // upgrades
    { WAR_COMMAND_UPGRADE_SWORDS,           WAR_UPGRADE_SWORDS          },
    { WAR_COMMAND_UPGRADE_AXES,             WAR_UPGRADE_AXES            },
    { WAR_COMMAND_UPGRADE_SHIELD_HUMANS,    WAR_UPGRADE_SHIELD          },
    { WAR_COMMAND_UPGRADE_SHIELD_ORCS,      WAR_UPGRADE_SHIELD          },
    { WAR_COMMAND_UPGRADE_ARROWS,           WAR_UPGRADE_ARROWS          },
    { WAR_COMMAND_UPGRADE_SPEARS,           WAR_UPGRADE_SPEARS          },
    { WAR_COMMAND_UPGRADE_HORSES,           WAR_UPGRADE_HORSES          },
    { WAR_COMMAND_UPGRADE_WOLVES,           WAR_UPGRADE_WOLVES          },
    { WAR_COMMAND_UPGRADE_SCORPION,         WAR_UPGRADE_SCORPIONS       },
    { WAR_COMMAND_UPGRADE_SPIDER,           WAR_UPGRADE_SPIDERS         },
    { WAR_COMMAND_UPGRADE_RAIN_OF_FIRE,     WAR_UPGRADE_RAIN_OF_FIRE    },
    { WAR_COMMAND_UPGRADE_POISON_CLOUD,     WAR_UPGRADE_POISON_CLOUD    },
    { WAR_COMMAND_UPGRADE_WATER_ELEMENTAL,  WAR_UPGRADE_WATER_ELEMENTAL },
    { WAR_COMMAND_UPGRADE_DAEMON,           WAR_UPGRADE_DAEMON          },
    { WAR_COMMAND_UPGRADE_HEALING,          WAR_UPGRADE_HEALING         },
    { WAR_COMMAND_UPGRADE_RAISE_DEAD,       WAR_UPGRADE_RAISE_DEAD      },
    { WAR_COMMAND_UPGRADE_FAR_SIGHT,        WAR_UPGRADE_FAR_SIGHT       },
    { WAR_COMMAND_UPGRADE_DARK_VISION,      WAR_UPGRADE_DARK_VISION     },
    { WAR_COMMAND_UPGRADE_INVISIBILITY,     WAR_UPGRADE_INVISIBILITY    },
    { WAR_COMMAND_UPGRADE_UNHOLY_ARMOR,     WAR_UPGRADE_UNHOLY_ARMOR    },

    // summon commands
    { WAR_COMMAND_SUMMON_SPIDER,            WAR_SUMMON_SPIDER           },
    { WAR_COMMAND_SUMMON_SCORPION,          WAR_SUMMON_SCORPION         },
    { WAR_COMMAND_SUMMON_DAEMON,            WAR_SUMMON_DAEMON           },
    { WAR_COMMAND_SUMMON_WATER_ELEMENTAL,   WAR_SUMMON_WATER_ELEMENTAL  },

    // spells
    { WAR_COMMAND_SPELL_HEALING,            WAR_SPELL_HEALING           },
    { WAR_COMMAND_SPELL_FAR_SIGHT,          WAR_SPELL_FAR_SIGHT         },
    { WAR_COMMAND_SPELL_INVISIBILITY,       WAR_SPELL_INVISIBILITY      },
    { WAR_COMMAND_SPELL_RAIN_OF_FIRE,       WAR_SPELL_RAIN_OF_FIRE      },
    { WAR_COMMAND_SPELL_RAISE_DEAD,         WAR_SPELL_RAISE_DEAD        },
    { WAR_COMMAND_SPELL_DARK_VISION,        WAR_SPELL_DARK_VISION       },
    { WAR_COMMAND_SPELL_UNHOLY_ARMOR,       WAR_SPELL_UNHOLY_ARMOR      },
    { WAR_COMMAND_SPELL_POISON_CLOUD,       WAR_SPELL_POISON_CLOUD      },
};

typedef struct
{
    WarUnitCommandType type;
    s32 gold;
    s32 wood;
    s32 frameIndex; // always from resource 361
    WarKeys hotKey;
    s32 highlightIndex;
    s32 highlightCount;
    StringView tooltip;
    WarClickHandler clickHandler;
} WarUnitCommandData;

WarUnitData wun_getUnitData(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(unitsData);
    while (index < length && unitsData[index].type != type)
        index++;

    assert(index < length);
    return unitsData[index];
}

WarWorkerData wun_getWorkerData(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(workersData);
    while (index < length && workersData[index].type != type)
        index++;

    assert(index < length);
    return workersData[index];
}

WarBuildingData wun_getBuildingData(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(buildingsData);
    while (index < length && buildingsData[index].type != type)
        index++;

    assert(index < length);
    return buildingsData[index];
}

WarRoadData wun_getRoadData(WarRoadPieceType type)
{
    s32 index = 0;
    s32 length = arrayLength(roadsData);
    while (index < length && roadsData[index].type != type)
        index++;

    assert(index < length);
    return roadsData[index];
}

WarWallData wun_getWallData(WarWallPieceType type)
{
    s32 index = 0;
    s32 length = arrayLength(wallsData);
    while (index < length && wallsData[index].type != type)
        index++;

    assert(index < length);
    return wallsData[index];
}

WarRuinData wun_getRuinData(WarRuinPieceType type)
{
    s32 index = 0;
    s32 length = arrayLength(ruinsData);
    while (index < length && ruinsData[index].type != type)
        index++;

    assert(index < length);
    return ruinsData[index];
}

WarTreeData wun_getTreeData(WarTreeTileType type)
{
    s32 index = 0;
    s32 length = arrayLength(treesData);
    while (index < length && treesData[index].type != type)
        index++;

    assert(index < length);
    return treesData[index];
}

WarUpgradeData wun_getUpgradeData(WarUpgradeType type)
{
    s32 index = 0;
    s32 length = arrayLength(upgradesData);
    while (index < length && upgradesData[index].type != type)
        index++;

    assert(index < length);
    return upgradesData[index];
}

WarSpellData wun_getSpellData(WarSpellType type)
{
    s32 index = 0;
    s32 length = arrayLength(spellData);
    while (index < length && spellData[index].type != type)
        index++;

    assert(index < length);
    return spellData[index];
}

WarUnitStats wun_getUnitStats(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(unitStats);
    while (index < length && unitStats[index].type != type)
        index++;

    assert(index < length);
    return unitStats[index];
}

WarBuildingStats wun_getBuildingStats(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(buildingStats);
    while (index < length && buildingStats[index].type != type)
        index++;

    assert(index < length);
    return buildingStats[index];
}

WarUpgradeStats wun_getUpgradeStats(WarUpgradeType type)
{
    s32 index = 0;
    s32 length = arrayLength(upgradeStats);
    while (index < length && upgradeStats[index].type != type)
        index++;

    assert(index < length);
    return upgradeStats[index];
}

WarSpellStats wun_getSpellStats(WarSpellType type)
{
    s32 index = 0;
    s32 length = arrayLength(spellStats);
    while (index < length && spellStats[index].type != type)
        index++;

    assert(index < length);
    return spellStats[index];
}

WarSpellMapping wun_getSpellMapping(WarSpellType type)
{
    s32 index = 0;
    s32 length = arrayLength(spellMappings);
    while (index < length && spellMappings[index].type != type)
        index++;

    assert(index < length);
    return spellMappings[index];
}

WarUnitCommandBaseData wun_getCommandBaseData(WarUnitCommandType type)
{
    s32 index = 0;
    s32 length = arrayLength(commandsBaseData);
    while (index < length && commandsBaseData[index].type != type)
        index++;

    assert(index < length);
    return commandsBaseData[index];
}

WarUnitCommandMapping wun_getCommandMapping(WarUnitCommandType type)
{
    s32 index = 0;
    s32 length = arrayLength(commandMappings);
    while (index < length && commandMappings[index].type != type)
        index++;

    assert(index < length);
    return commandMappings[index];
}

WarUnitCommandMapping wun_getCommandMappingFromUnitType(WarUnitType unitType)
{
    s32 index = 0;
    s32 length = index + 30;
    while (index < length && commandMappings[index].mappedType != unitType)
        index++;

    assert(index < length);
    return commandMappings[index];
}

WarUnitCommandMapping wun_getCommandMappingFromUpgradeType(WarUpgradeType upgradeType)
{
    s32 index = 30;
    s32 length = index + 20;
    while (index < length && commandMappings[index].mappedType != upgradeType)
        index++;

    assert(index < length);
    return commandMappings[index];
}

WarUnitCommandMapping wun_getCommandMappingFromSpellType(WarSpellType spellType)
{
    s32 index = 50;
    s32 length = arrayLength(commandMappings);
    while (index < length && commandMappings[index].mappedType != spellType)
        index++;

    assert(index < length);
    return commandMappings[index];
}

#define isUnit(entity) ((entity)->type == WAR_ENTITY_TYPE_UNIT)
#define isUnitOfType(entity, unitType) (isUnit(entity) && (entity)->unit.type == (unitType))
#define isRoad(entity) ((entity)->type == WAR_ENTITY_TYPE_ROAD)
#define isWall(entity) ((entity)->type == WAR_ENTITY_TYPE_WALL)
#define isRuin(entity) ((entity)->type == WAR_ENTITY_TYPE_RUIN)

bool wun_isDudeUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_PEASANT:
        case WAR_UNIT_PEON:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_MEDIVH:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_WOUNDED:
        case WAR_UNIT_GRIZELDA:
        case WAR_UNIT_GARONA:
        case WAR_UNIT_OGRE:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_SLIME:
        case WAR_UNIT_FIRE_ELEMENTAL:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_BRIGAND:
        case WAR_UNIT_THE_DEAD:
        case WAR_UNIT_SKELETON:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_WATER_ELEMENTAL:
            return true;

        default:
            return false;
    }
}

bool wun_isBuildingUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FARM_HUMANS:
        case WAR_UNIT_FARM_ORCS:
        case WAR_UNIT_BARRACKS_HUMANS:
        case WAR_UNIT_BARRACKS_ORCS:
        case WAR_UNIT_CHURCH:
        case WAR_UNIT_TEMPLE:
        case WAR_UNIT_TOWER_HUMANS:
        case WAR_UNIT_TOWER_ORCS:
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_TOWNHALL_ORCS:
        case WAR_UNIT_LUMBERMILL_HUMANS:
        case WAR_UNIT_LUMBERMILL_ORCS:
        case WAR_UNIT_STABLE:
        case WAR_UNIT_KENNEL:
        case WAR_UNIT_BLACKSMITH_HUMANS:
        case WAR_UNIT_BLACKSMITH_ORCS:
        case WAR_UNIT_STORMWIND:
        case WAR_UNIT_BLACKROCK:
        case WAR_UNIT_GOLDMINE:
            return true;

        default:
            return false;
    }
}

bool wun_isWorkerUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_PEASANT:
        case WAR_UNIT_PEON:
            return true;

        default:
            return false;
    }
}

bool wun_isWarriorUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_MEDIVH:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_WOUNDED:
        case WAR_UNIT_OGRE:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_SLIME:
        case WAR_UNIT_FIRE_ELEMENTAL:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_BRIGAND:
        case WAR_UNIT_THE_DEAD:
        case WAR_UNIT_SKELETON:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_WATER_ELEMENTAL:
            return true;

        default:
            return false;
    }
}

bool wun_isRangeUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_MEDIVH:
        case WAR_UNIT_WATER_ELEMENTAL:
            return true;

        default:
            return false;
    }
}

bool wun_isMeleeUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_OGRE:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_SLIME:
        case WAR_UNIT_FIRE_ELEMENTAL:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_BRIGAND:
        case WAR_UNIT_SKELETON:
        case WAR_UNIT_DAEMON:
            return true;

        default:
            return false;
    }
}

bool wun_isFistUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_OGRE:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_SLIME:
        case WAR_UNIT_FIRE_ELEMENTAL:
        case WAR_UNIT_SCORPION:
            return true;

        default:
            return false;
    }
}

bool wun_isSwordUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_BRIGAND:
        case WAR_UNIT_SKELETON:
        case WAR_UNIT_DAEMON:
            return true;

        default:
            return false;
    }
}

bool wun_isMagicUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_WATER_ELEMENTAL:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_THE_DEAD:
            return true;

        default:
            return false;
    }
}

bool wun_isCorpseUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_HUMAN_CORPSE:
        case WAR_UNIT_ORC_CORPSE:
            return true;

        default:
            return false;
    }
}

bool wun_isCatapultUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
            return true;

        default:
            return false;
    }
}

bool wun_isConjurerOrWarlockUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
            return true;

        default:
            return false;
    }
}

bool wun_isClericOrNecrolyteUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
            return true;

        default:
            return false;
    }
}

bool wun_isSummonUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_WATER_ELEMENTAL:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_THE_DEAD:
            return true;

        default:
            return false;
    }
}


bool wun_isDudeUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isDudeUnitType(entity->unit.type);
}

bool wun_isBuildingUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isBuildingUnitType(entity->unit.type);
}

bool wun_isWorkerUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isWorkerUnitType(entity->unit.type);
}

bool wun_isWarriorUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isWarriorUnitType(entity->unit.type);
}

bool wun_isRangeUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isRangeUnitType(entity->unit.type);
}

bool wun_isMeleeUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isMeleeUnitType(entity->unit.type);
}

bool wun_isFistUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isFistUnitType(entity->unit.type);
}

bool wun_isSwordUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isSwordUnitType(entity->unit.type);
}

bool wun_isMagicUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isMagicUnitType(entity->unit.type);
}

bool wun_isCorpseUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isCorpseUnitType(entity->unit.type);
}

bool wun_isCatapultUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isCatapultUnitType(entity->unit.type);
}

bool wun_isConjurerOrWarlockUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isConjurerOrWarlockUnitType(entity->unit.type);
}

bool wun_isClericOrNecrolyteUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isClericOrNecrolyteUnitType(entity->unit.type);
}

bool wun_isSummonUnit(WarEntity* entity)
{
    return isUnit(entity) && wun_isSummonUnitType(entity->unit.type);
}

bool wun_isSkeletonUnit(WarEntity* entity)
{
    return isUnit(entity) && entity->unit.type == WAR_UNIT_SKELETON;
}

WarRace wun_getUnitTypeRace(WarUnitType type)
{
    switch (type)
    {
        // units
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_PEASANT:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_WATER_ELEMENTAL:
        case WAR_UNIT_HUMAN_CORPSE:
        // buildings
        case WAR_UNIT_FARM_HUMANS:
        case WAR_UNIT_BARRACKS_HUMANS:
        case WAR_UNIT_CHURCH:
        case WAR_UNIT_TOWER_HUMANS:
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_LUMBERMILL_HUMANS:
        case WAR_UNIT_STABLE:
        case WAR_UNIT_BLACKSMITH_HUMANS:
        case WAR_UNIT_STORMWIND:
            return WAR_RACE_HUMANS;

        // units
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_PEON:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_GRIZELDA:
        case WAR_UNIT_GARONA:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_ORC_CORPSE:
        // buildings
        case WAR_UNIT_FARM_ORCS:
        case WAR_UNIT_BARRACKS_ORCS:
        case WAR_UNIT_TEMPLE:
        case WAR_UNIT_TOWER_ORCS:
        case WAR_UNIT_TOWNHALL_ORCS:
        case WAR_UNIT_LUMBERMILL_ORCS:
        case WAR_UNIT_KENNEL:
        case WAR_UNIT_BLACKSMITH_ORCS:
        case WAR_UNIT_BLACKROCK:
            return WAR_RACE_ORCS;

        default:
            return WAR_RACE_NEUTRAL;
    }
}

WarRace wun_getUnitRace(WarEntity* entity)
{
    if (!isUnit(entity))
        return WAR_RACE_NEUTRAL;

    return wun_getUnitTypeRace(entity->unit.type);
}

#define isHumanUnit(entity) (wun_getUnitRace(entity) == WAR_RACE_HUMANS)
#define isOrcUnit(entity) (wun_getUnitRace(entity) == WAR_RACE_ORCS)
#define isNeutralUnit(entity) (wun_getUnitRace(entity) == WAR_RACE_NEUTRAL)

WarUnitType wun_getUnitTypeForRace(WarUnitType type, WarRace race);

WarProjectileType wun_getProjectileType(WarUnitType type)
{
    assert(wun_isRangeUnitType(type));

    switch (type)
    {
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        {
            return WAR_PROJECTILE_ARROW;
        }

        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        {
            return WAR_PROJECTILE_CATAPULT;
        }

        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        {
            return WAR_PROJECTILE_FIREBALL;
        }

        case WAR_UNIT_WATER_ELEMENTAL:
        {
            return WAR_PROJECTILE_WATER_ELEMENTAL;
        }

        default:
        {
            // unreachable
            logWarning("Invalid unit firing a projectile: %d", type);
            return WAR_PROJECTILE_ARROW;
        }
    }
}

bool wun_isFriendlyUnit(WarContext* context, WarEntity* entity);
bool wun_isEnemyUnit(WarContext* context, WarEntity* entity);
bool wun_areEnemies(WarContext* context, WarEntity* entity, WarEntity* other);
bool wun_canAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity);

WarUnitType wun_getTownHallOfRace(WarRace race)
{
    switch (race)
    {
        case WAR_RACE_HUMANS: return WAR_UNIT_TOWNHALL_HUMANS;
        case WAR_RACE_ORCS: return WAR_UNIT_TOWNHALL_ORCS;
        default: return WAR_UNIT_TOWNHALL_HUMANS;
    }
}

WarUnitType wun_getProducerUnitOfType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_PEASANT:
            return WAR_UNIT_TOWNHALL_HUMANS;
        case WAR_UNIT_PEON:
            return WAR_UNIT_TOWNHALL_ORCS;

        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_ARCHER:
            return WAR_UNIT_BARRACKS_HUMANS;

        case WAR_UNIT_GRUNT:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_SPEARMAN:
            return WAR_UNIT_BARRACKS_ORCS;

        case WAR_UNIT_CONJURER:
            return WAR_UNIT_TOWER_HUMANS;
        case WAR_UNIT_WARLOCK:
            return WAR_UNIT_TOWER_ORCS;
        case WAR_UNIT_CLERIC:
            return WAR_UNIT_CHURCH;
        case WAR_UNIT_NECROLYTE:
            return WAR_UNIT_TEMPLE;

        default:
        {
            logWarning("There is not producer unit for unit type %d", type);
            return -1;
        }
    }
}

vec2 wun_getUnitSize(WarEntity* entity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    return vec2i(unit->sizex, unit->sizey);
}

vec2 wun_getUnitFrameSize(WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    return vec2i(sprite->sprite.frameWidth, sprite->sprite.frameHeight);
}

rect wun_getUnitFrameRect(WarEntity* entity)
{
    return rectv(VEC2_ZERO, wun_getUnitFrameSize(entity));
}

vec2 wun_getUnitSpriteSize(WarEntity* entity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    return vec2i(unit->sizex * MEGA_TILE_WIDTH, unit->sizey * MEGA_TILE_HEIGHT);
}

rect wun_getUnitSpriteRect(WarEntity* entity)
{
    vec2 frameSize = wun_getUnitFrameSize(entity);
    vec2 unitSize = wun_getUnitSpriteSize(entity);
    vec2 pos = vec2Half(vec2Subv(frameSize, unitSize));
    return rectv(pos, unitSize);
}

vec2 wun_getUnitSpriteCenter(WarEntity* entity)
{
    vec2 frameSize = wun_getUnitFrameSize(entity);
    vec2 unitSize = wun_getUnitSpriteSize(entity);
    vec2 pos = vec2Half(vec2Subv(frameSize, unitSize));
    return vec2Addv(pos, vec2Half(unitSize));
}

rect wun_getUnitRect(WarEntity* entity)
{
    assert(isUnit(entity));

    return rectv(entity->transform.position, wun_getUnitSpriteSize(entity));
}

vec2 wun_getUnitPosition(WarEntity* entity, bool inTiles)
{
    vec2 position = entity->transform.position;
    return inTiles ? wmap_vec2MapToTileCoordinates(position) : position;
}

vec2 wun_getUnitCenterPosition(WarEntity* entity, bool inTiles)
{
    WarTransformComponent* transform = &entity->transform;
    vec2 spriteSize = wun_getUnitSpriteSize(entity);
    vec2 unitCenter = vec2Half(spriteSize);
    vec2 position = vec2Addv(transform->position, unitCenter);
    return inTiles ? wmap_vec2MapToTileCoordinates(position) : position;
}

void wun_setUnitPosition(WarEntity* entity, vec2 position, bool inTiles)
{
    if (inTiles)
    {
        position = wmap_vec2TileToMapCoordinates(position, true);
    }

    entity->transform.position = position;
}

void wun_setUnitCenterPosition(WarEntity* entity, vec2 position, bool inTiles)
{
    if (inTiles)
    {
        position = wmap_vec2TileToMapCoordinates(position, true);
    }

    WarTransformComponent* transform = &entity->transform;
    vec2 spriteSize = wun_getUnitSpriteSize(entity);
    vec2 unitCenter = vec2Half(spriteSize);
    transform->position = vec2Subv(position, unitCenter);
}

WarUnitDirection wun_getUnitDirection(WarEntity* entity)
{
    assert(isUnit(entity));

    return entity->unit.direction;
}

WarUnitDirection wun_getDirectionFromDiff(f32 x, f32 y)
{
    if (x < 0 && y < 0)
        return WAR_DIRECTION_NORTH_WEST;
    if (x == 0 && y < 0)
        return WAR_DIRECTION_NORTH;
    if (x > 0 && y < 0)
        return WAR_DIRECTION_NORTH_EAST;

    if (x < 0 && y == 0)
        return WAR_DIRECTION_WEST;
    if (x > 0 && y == 0)
        return WAR_DIRECTION_EAST;

    if (x < 0 && y > 0)
        return WAR_DIRECTION_SOUTH_WEST;
    if (x == 0 && y > 0)
        return WAR_DIRECTION_SOUTH;
    if (x > 0 && y > 0)
        return WAR_DIRECTION_SOUTH_EAST;

    return WAR_DIRECTION_NORTH;
}

void wun_setUnitDirection(WarEntity* entity, WarUnitDirection direction)
{
    assert(isUnit(entity));

    entity->unit.direction = direction;
}

void wun_setUnitDirectionFromDiff(WarEntity* entity, f32 dx, f32 dy)
{
    assert(isUnit(entity));

    WarUnitDirection direction = wun_getDirectionFromDiff(dx, dy);
    wun_setUnitDirection(entity, direction);
}

f32 wun_getUnitActionScale(WarEntity* entity)
{
    assert(isUnit(entity));

    // this is the scale of the animation, for walking
    // the lower the less time is the transition between the frames
    // thus the animation goes faster.
    //
    // speed 0 -> 1.0f
    // speed 1 -> 0.9f
    // speed 2 -> 0.8f
    return 1 - entity->unit.speed * 0.1f;
}

vec2 wun_unitPointOnTarget(WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));
    assert(isUnit(targetEntity));

    vec2 position = wun_getUnitCenterPosition(entity, true);

    vec2 targetPosition = wmap_vec2MapToTileCoordinates(targetEntity->transform.position);
    vec2 unitSize = wun_getUnitSize(targetEntity);
    rect unitRect = rectv(targetPosition, unitSize);

    return getClosestPointOnRect(position, unitRect);
}

s32 wun_entityTileDistance(WarEntity* entity, vec2 targetPosition)
{
    assert(isUnit(entity));

    vec2 position = wun_getUnitCenterPosition(entity, true);
    f32 distance = vec2DistanceInTiles(position, targetPosition);
    return (s32)distance;
}

bool wun_tileInRange(WarEntity* entity, vec2 targetTile, s32 range)
{
    assert(range >= 0);

    s32 distance = wun_entityTileDistance(entity, targetTile);
    return distance <= range;
}

s32 wun_unitDistanceInTiles(WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));
    assert(isUnit(targetEntity));

    vec2 pointOnTarget = wun_unitPointOnTarget(entity, targetEntity);
    return wun_entityTileDistance(entity, pointOnTarget);
}

bool wun_unitInRange(WarEntity* entity, WarEntity* targetEntity, s32 range)
{
    assert(isUnit(entity));
    assert(isUnit(targetEntity));
    assert(range >= 0);

    s32 distance = wun_unitDistanceInTiles(entity, targetEntity);
    return distance <= range;
}

bool wun_isCarryingResources(WarEntity* entity)
{
    assert(entity);
    assert(isUnit(entity));

    switch (entity->unit.resourceKind)
    {
        case WAR_RESOURCE_GOLD: return entity->unit.amount == UNIT_MAX_CARRY_WOOD;
        case WAR_RESOURCE_WOOD: return entity->unit.amount == UNIT_MAX_CARRY_GOLD;
        default: return false;
    }
}

s32 wun_getUnitSightRange(WarEntity* entity)
{
    assert(isUnit(entity));

    s32 sight = 0;

    if (wun_isBuildingUnit(entity))
    {
        WarBuildingStats stats = wun_getBuildingStats(entity->unit.type);
        sight = stats.sight;
    }
    else
    {
        WarUnitStats stats = wun_getUnitStats(entity->unit.type);
        sight = stats.sight;
    }

    return sight;
}

bool wun_displayUnitOnMinimap(WarEntity* entity);
WarColor wun_getUnitColorOnMinimap(WarEntity* entity);

s32 wun_getTotalNumberOfDudes(WarContext* context, u8 player);
s32 wun_getTotalNumberOfBuildings(WarContext* context, u8 player, bool alreadyBuilt);
s32 wun_getNumberOfBuildingsOfType(WarContext* context, u8 player, WarUnitType unitType, bool alreadyBuilt);
s32 wun_getNumberOfUnitsOfType(WarContext* context, u8 player, WarUnitType unitType);
s32 wun_getTotalNumberOfUnits(WarContext* context, u8 player);

#define playerHasUnit(context, player, unitType) (wun_getNumberOfUnitsOfType(context, player, unitType) > 0)
#define playerHasBuilding(context, player, unitType) (wun_getNumberOfBuildingsOfType(context, player, unitType, true) > 0)

#define isValidUnitType(type) inRange(type, WAR_UNIT_FOOTMAN, WAR_UNIT_COUNT)

void wun_getUnitCommands(WarContext* context, WarEntity* entity, WarUnitCommandType commands[]);
void wun_getBuildBasicCommands(WarContext* context, WarEntity* entity, WarUnitCommandType commands[]);
void wun_getBuildAdvancedCommands(WarContext* context, WarEntity* entity, WarUnitCommandType commands[]);
WarUnitCommandData wun_getUnitCommandData(WarContext* context, WarEntity* entity, WarUnitCommandType commandType);
