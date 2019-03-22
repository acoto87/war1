const char* features[MAX_FEATURES_COUNT * 2] = {
	// Units. 0 - 6
	"unit-footman",             "unit-grunt",
	"unit-peasant",             "unit-peon",
	"unit-human-catapult",      "unit-orc-catapult",
	"unit-knight",              "unit-raider",
	"unit-archer",              "unit-spearman",
	"unit-conjurer",            "unit-warlock",
	"unit-cleric",              "unit-necrolyte",
	// Constructing buildings. 7 - 14
	"unit-human-farm",          "unit-orc-farm",
	"unit-human-barracks",      "unit-orc-barracks",
	"unit-human-church",        "unit-orc-temple",
	"unit-human-tower",         "unit-orc-tower",
	"unit-human-town-hall",     "unit-orc-town-hall",
	"unit-human-lumber-mill",   "unit-orc-lumber-mill",
	"unit-human-stable",        "unit-orc-kennel",
	"unit-human-blacksmith",    "unit-orc-blacksmith",
	// Cleric/Necrolyte spells. 15 - 17
	"upgrade-healing",          "upgrade-raise-dead",
	"upgrade-holy-vision",      "upgrade-dark-vision",
	"upgrade-invisibility",     "upgrade-unholy-armor",
	// Conjurer/Warlock spells. 18 - 20
	"upgrade-scorpion",         "upgrade-spider",
	"upgrade-rain-of-fire",     "upgrade-poison-cloud",
	"upgrade-water-elemental",  "upgrade-daemon",
	// Roads and walls. 21 - 22
	"unit-road",                "unit-road",
	"unit-wall",                "unit-wall"
};

const char* upgradeNames[MAX_UPGRADES_COUNT * 2] = 
{
    // Basic upgrades
    "upgrade-spear",        "upgrade-arrow",
    "upgrade-axe",          "upgrade-sword",
    "upgrade-wolves",       "upgrade-horse",
    // Spells and summons
    "upgrade-spider",       "upgrade-scorpion",
    "upgrade-poison-cloud", "upgrade-rain-of-fire",
    "upgrade-daemon",       "upgrade-water-elemental",
    "upgrade-raise-dead",   "upgrade-healing",
    "upgrade-dark-vision",  "upgrade-far-seeing",
    "upgrade-unholy-armor", "upgrade-invisibility",
    // Shield upgrades
    "upgrade-orc-shield",   "upgrade-human-shield"
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
    WAR_PORTRAIT_STABLES,
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
    char name[50];
} WarUnitData;

const WarUnitData unitsData[] = 
{
    // units
    { WAR_UNIT_FOOTMAN,                   279,  0, 1, 1, "FOOTMAN" },
    { WAR_UNIT_GRUNT,                     280,  1, 1, 1, "GRUNT" },
    { WAR_UNIT_PEASANT,                   281,  4, 1, 1, "PEASANT" },
    { WAR_UNIT_PEON,                      282,  5, 1, 1, "PEON" },
    { WAR_UNIT_CATAPULT_HUMANS,           283,  6, 1, 1, "CATAPULT" },
    { WAR_UNIT_CATAPULT_ORCS,             284,  7, 1, 1, "CATAPULT" },
    { WAR_UNIT_KNIGHT,                    285,  8, 1, 1, "KNIGHT" },
    { WAR_UNIT_RAIDER,                    286,  9, 1, 1, "RAIDER" },
    { WAR_UNIT_ARCHER,                    287, 10, 1, 1, "ARCHER" },
    { WAR_UNIT_SPEARMAN,                  288, 11, 1, 1, "SPEARMAN" },
    { WAR_UNIT_CONJURER,                  289,  2, 1, 1, "CONJURER" },
    { WAR_UNIT_WARLOCK,                   290,  3, 1, 1, "WARLOCK" },
    { WAR_UNIT_CLERIC,                    291, 12, 1, 1, "CLERIC" },
    { WAR_UNIT_NECROLYTE,                 292, 13, 1, 1, "NECROLYTE" },
    { WAR_UNIT_MEDIVH,                    293, 54, 1, 1, "MEDIVH" },
    { WAR_UNIT_LOTHAR,                    294, 53, 1, 1, "LOTHAR" },
    { WAR_UNIT_WOUNDED,                   295, 57, 1, 1, "WOUNDED" },
    { WAR_UNIT_GRIZELDA,                  296, 55, 1, 1, "GRIZELDA" },
    { WAR_UNIT_GARONA,                    296, 56, 1, 1, "GARONA" },
    { WAR_UNIT_OGRE,                      297, 44, 1, 1, "OGRE" },
    { WAR_UNIT_SPIDER,                    298, 45, 1, 1, "SPIDER" },
    { WAR_UNIT_SLIME,                     299, 46, 1, 1, "SLIME" },
    { WAR_UNIT_FIREELEMENTAL,             300, 47, 1, 1, "FIRE ELEMENTAL" },
    { WAR_UNIT_SCORPION,                  301, 48, 1, 1, "SCORPION" },
    { WAR_UNIT_BRIGAND,                   302, 58, 1, 1, "BRIGAND" },
    { WAR_UNIT_THE_DEAD,                  303, 50, 1, 1, "THE DEAD" },
    { WAR_UNIT_SKELETON,                  304, 49, 1, 1, "SKELETON" },
    { WAR_UNIT_DAEMON,                    305, 51, 1, 1, "DAEMON" },
    { WAR_UNIT_WATER_ELEMENTAL,            306, 52, 1, 1, "WATER ELEMENTAL" },

    // buildings
    { WAR_UNIT_FARM_HUMANS,               307, 14, 2, 2, "FARM" },
    { WAR_UNIT_FARM_ORCS,                 308, 15, 2, 2, "FARM" },
    { WAR_UNIT_BARRACKS_HUMANS,           309, 16, 3, 3, "BARRACKS" },
    { WAR_UNIT_BARRACKS_ORCS,             310, 17, 3, 3, "BARRACKS" },
    { WAR_UNIT_CHURCH,                    311, 28, 3, 3, "CHURCH" },
    { WAR_UNIT_TEMPLE,                    312, 29, 3, 3, "TEMPLE" },
    { WAR_UNIT_TOWER_HUMANS,              313, 18, 2, 2, "TOWER" },
    { WAR_UNIT_TOWER_ORCS,                314, 19, 2, 2, "TOWER" },
    { WAR_UNIT_TOWNHALL_HUMANS,           315, 20, 3, 3, "TOWN HALL" },
    { WAR_UNIT_TOWNHALL_ORCS,             316, 21, 3, 3, "TOWN HALL" },
    { WAR_UNIT_LUMBERMILL_HUMANS,         317, 22, 3, 3, "LUMBER MILL" },
    { WAR_UNIT_LUMBERMILL_ORCS,           318, 23, 3, 3, "LUMBER MILL" },
    { WAR_UNIT_STABLES,                   319, 24, 3, 3, "STABLES" },
    { WAR_UNIT_KENNEL,                    320, 25, 3, 3, "KENNEL" },
    { WAR_UNIT_BLACKSMITH_HUMANS,         321, 26, 2, 2, "BLACKSMITH" },
    { WAR_UNIT_BLACKSMITH_ORCS,           322, 27, 2, 2, "BLACKSMITH" },
    { WAR_UNIT_STORMWIND,                 323, 31, 4, 4, "STORMWIND" },
    { WAR_UNIT_BLACKROCK,                 324, 32, 4, 4, "BLACKROCK" },

    // neutral
    { WAR_UNIT_GOLDMINE,                  325, 30, 3, 3, "GOLDMINE" },

    // corpses
    { WAR_UNIT_HUMAN_CORPSE,              326, 0, 1, 1, "CORPSE" },
    { WAR_UNIT_ORC_CORPSE,                326, 0, 1, 1, "CORPSE" }
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
    { WAR_UNIT_STABLES,                   343 },
    { WAR_UNIT_KENNEL,                    344 },
    { WAR_UNIT_BLACKSMITH_HUMANS,         345 },
    { WAR_UNIT_BLACKSMITH_ORCS,           346 },
};

typedef struct
{
    WarRoadPieceType type;
    s32 tileIndexForest;
    s32 tileIndexSwamp;
} WarRoadsData;

const WarRoadsData roadsData[] =
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
} WarWallsData;

const WarWallsData wallsData[] =
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

typedef struct
{
    WarRuinPieceType type;
    s32 tileIndexForest;
    s32 tileIndexSwamp;
} WarRuinsData;

const WarRuinsData ruinsData[] =
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
} WarTreesData;

const WarTreesData treesData[] =
{
    { WAR_TREE_NONE,                 0,  0 },
    { WAR_TREE_TOP_LEFT,            73, 42 },
    { WAR_TREE_TOP,                 75, 43 },
    { WAR_TREE_TOP_RIGHT,           76, 44 },
    { WAR_TREE_LEFT,                72, 45 },
    { WAR_TREE_CENTER,              94, 46 },
    { WAR_TREE_RIGHT,               78, 47 },
    { WAR_TREE_BOTTOM_LEFT,         71, 48 },
    { WAR_TREE_BOTTOM,              93, 49 },
    { WAR_TREE_BOTTOM_RIGHT,        79, 50 },
    { WAR_TREE_TOP_LEFT_INSIDE,     80, 51 },
    { WAR_TREE_TOP_RIGHT_INSIDE,    81, 52 },
    { WAR_TREE_BOTTOM_LEFT_INSIDE,  77, 54 },
    { WAR_TREE_BOTTOM_RIGHT_INSIDE, 74, 53 },
    { WAR_TREE_TOP_END,             90, 90 },
    { WAR_TREE_BOTTOM_END,          92, 92 },
    { WAR_TREE_VERTICAL,            91, 91 },
    { WAR_TREE_DIAG_1,              83, 56 },
    { WAR_TREE_DIAG_2,              82, 55 },
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

typedef struct
{
    WarUnitType type;
    s32 range;
    s32 armor;
    s32 hp;
    s32 magic;
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
// STABLES,                     1500 program cycles = 120s
// BLACKSMITH_HUMANS,           1500 program cycles = 120s
//
// build time in seconds
#define __bts(t) ((t)*80/1000)

const WarUnitStats unitStats[] = 
{
    // unit type                range  armor   hp   magic  min D.  rnd D.       build    gold    lumber   decay    speed in pixels x seconds
    { WAR_UNIT_FOOTMAN,           1,     2,     60,   -1,     1,      9,    __bts(600),    400,      0,     -1,   { 16.736f, 16.736f, 16.736f } },
    { WAR_UNIT_GRUNT,             1,     2,     60,   -1,     1,      9,    __bts(600),    400,      0,     -1,   { 16.736f, 16.736f, 16.736f } },
    { WAR_UNIT_PEASANT,           1,     0,     40,   -1,     0,      0,    __bts(750),    400,      0,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_PEON,              1,     0,     40,   -1,     0,      0,    __bts(750),    400,      0,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_CATAPULT_HUMANS,   8,     0,    120,   -1,     0,    255,    __bts(1000),   900,    200,     -1,   {  8.992f,  8.992f,  8.992f } },
    { WAR_UNIT_CATAPULT_ORCS,     8,     0,    120,   -1,     0,    255,    __bts(1000),   900,    200,     -1,   {  8.992f,  8.992f,  8.992f } },
    { WAR_UNIT_KNIGHT,            1,     5,     90,   -1,     1,     13,    __bts(800),    850,      0,     -1,   { 19.584f, 22.064f, 26.064f } },
    { WAR_UNIT_RAIDER,            1,     5,     90,   -1,     1,     13,    __bts(800),    850,      0,     -1,   { 19.584f, 22.064f, 26.064f } },
    { WAR_UNIT_ARCHER,            5,     1,     60,   -1,     4,      0,    __bts(700),    450,     50,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_SPEARMAN,          4,     1,     60,   -1,     5,      0,    __bts(700),    450,     50,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_CONJURER,          3,     0,     40,  255,     6,      0,    __bts(900),    900,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_WARLOCK,           2,     0,     40,  255,     6,      0,    __bts(900),    900,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_CLERIC,            1,     0,     40,  255,     6,      0,    __bts(800),    700,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_NECROLYTE,         2,     0,     40,  255,     6,      0,    __bts(800),    700,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_MEDIVH,            5,     0,    110,   -1,    10,      0,            -1,     -1,     -1,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_LOTHAR,            1,     5,     50,   -1,     1,     15,            -1,     -1,     -1,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_GRIZELDA,          1,     0,     30,   -1,    -1,     -1,            -1,     -1,     -1,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_GARONA,            1,     0,     30,   -1,    -1,     -1,            -1,     -1,     -1,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_OGRE,              1,     3,     60,   -1,     1,     12,            -1,     -1,     -1,     -1,   { 17.936f, 17.936f, 17.936f } },
    { WAR_UNIT_SPIDER,            1,     0,     30,   -1,     1,      3,            -1,     -1,     -1,     45,   { 26.064f, 26.064f, 26.064f } },
    { WAR_UNIT_SLIME,             1,    10,    150,   -1,     1,      0,            -1,     -1,     -1,     -1,   { 26.064f, 26.064f, 26.064f } },
    { WAR_UNIT_FIREELEMENTAL,     1,     0,    200,   -1,     0,     40,            -1,     -1,     -1,     -1,   { 17.936f, 17.936f, 17.936f } },
    { WAR_UNIT_SCORPION,          1,     0,     30,   -1,     3,      0,            -1,     -1,     -1,     45,   { 26.064f, 26.064f, 26.064f } },
    { WAR_UNIT_BRIGAND,           1,     1,     40,   -1,     1,      9,            -1,     -1,     -1,     -1,   { 16.736f, 16.736f, 16.736f } },
    { WAR_UNIT_SKELETON,          1,     1,     40,   -1,     1,      4,            -1,     -1,     -1,     45,   {  8.992f,  8.992f,  8.992f } },
    { WAR_UNIT_DAEMON,            1,     0,    300,   -1,     0,     65,            -1,     -1,     -1,     45,   { 17.936f, 17.936f, 17.936f } },
    { WAR_UNIT_WATER_ELEMENTAL,    3,     0,    250,   -1,    40,      0,            -1,     -1,     -1,     45,   { 17.936f, 17.936f, 17.936f } },
};

typedef struct
{
    WarUnitType type;
    s32 armor;
    s32 hp;
    s32 buildTime;
    s32 goldCost;
    s32 woodCost;
} WarBuildingStats;

const WarBuildingStats buildingStats[] = 
{
    // building type                    armor     hp            build     gold    lumber
    { WAR_UNIT_FARM_HUMANS,               0,       400,     __bts(1000),    500,    300 },
    { WAR_UNIT_FARM_ORCS,                 0,       400,     __bts(1000),    500,    300 },
    { WAR_UNIT_BARRACKS_HUMANS,           0,       800,     __bts(1500),    600,    500 },
    { WAR_UNIT_BARRACKS_ORCS,             0,       800,     __bts(1500),    600,    500 },
    { WAR_UNIT_CHURCH,                    0,       700,     __bts(2000),    800,    500 },
    { WAR_UNIT_TEMPLE,                    0,       700,     __bts(2000),    800,    500 },
    { WAR_UNIT_TOWER_HUMANS,              0,       900,     __bts(2000),   1400,    300 },
    { WAR_UNIT_TOWER_ORCS,                0,       900,     __bts(2000),   1400,    300 },
    { WAR_UNIT_TOWNHALL_HUMANS,           0,      2500,     __bts(1000),    400,    400 },
    { WAR_UNIT_TOWNHALL_ORCS,             0,      2500,     __bts(1000),    400,    400 },
    { WAR_UNIT_LUMBERMILL_HUMANS,         0,       600,     __bts(1500),    600,    500 },
    { WAR_UNIT_LUMBERMILL_ORCS,           0,       600,     __bts(1500),    600,    500 },
    { WAR_UNIT_STABLES,                   0,       500,     __bts(1500),   1000,    400 },
    { WAR_UNIT_KENNEL,                    0,       500,     __bts(1500),   1000,    400 },
    { WAR_UNIT_BLACKSMITH_HUMANS,         0,       800,     __bts(1500),    900,    400 },
    { WAR_UNIT_BLACKSMITH_ORCS,           0,       800,     __bts(1500),    900,    400 },
    { WAR_UNIT_STORMWIND,                 0,      5000,              -1,     -1,     -1 },
    { WAR_UNIT_BLACKROCK,                 0,      5000,              -1,     -1,     -1 },
    { WAR_UNIT_GOLDMINE,                  0,     25500,              -1,     -1,     -1 },
};

typedef struct
{
    WarUpgradeType type;
    s32 buildTime;
    s32 goldCost[2];
    s32 frameIndices[2];
} WarUpgradeStats;

const WarUpgradeStats upgradeStats[] =
{
    { WAR_UPGRADE_ARROWS,           __bts(120), {  750, 1500 }, { WAR_PORTRAIT_ARROW_2,          WAR_PORTRAIT_ARROW_3            } },
    { WAR_UPGRADE_SPEARS,           __bts(120), {  750, 1500 }, { WAR_PORTRAIT_SPEAR_2,          WAR_PORTRAIT_SPEAR_3            } },
    { WAR_UPGRADE_SWORDS,           __bts(140), {  750, 1500 }, { WAR_PORTRAIT_SWORD_2,          WAR_PORTRAIT_SWORD_3            } },
    { WAR_UPGRADE_AXES,             __bts(140), {  750, 1500 }, { WAR_PORTRAIT_AXE_2,            WAR_PORTRAIT_AXE_3              } },
    { WAR_UPGRADE_HORSES,           __bts(140), {  750, 1500 }, { WAR_PORTRAIT_HORSE_1,          WAR_PORTRAIT_HORSE_2            } },
    { WAR_UPGRADE_WOLVES,           __bts(140), {  750, 1500 }, { WAR_PORTRAIT_WOLVES_1,         WAR_PORTRAIT_WOLVES_2           } },
    { WAR_UPGRADE_SCORPIONS,        __bts(140), {  750,    0 }, { WAR_PORTRAIT_SCORPION,         WAR_PORTRAIT_SCORPION           } },
    { WAR_UPGRADE_SPIDERS,          __bts(140), {  750,    0 }, { WAR_PORTRAIT_SPIDER,           WAR_PORTRAIT_SPIDER             } },
    { WAR_UPGRADE_RAIN_OF_FIRE,     __bts(140), { 1500,    0 }, { WAR_PORTRAIT_RAIN_OF_FIRE,     WAR_PORTRAIT_RAIN_OF_FIRE       } },
    { WAR_UPGRADE_POISON_CLOUD,     __bts(140), { 1500,    0 }, { WAR_PORTRAIT_POISON_CLOUD,     WAR_PORTRAIT_POISON_CLOUD       } },
    { WAR_UPGRADE_WATER_ELEMENTAL,  __bts(140), { 3000,    0 }, { WAR_PORTRAIT_WATER_ELEMENTAL,  WAR_PORTRAIT_WATER_ELEMENTAL    } },
    { WAR_UPGRADE_DAEMON,           __bts(140), { 3000,    0 }, { WAR_PORTRAIT_DAEMON,           WAR_PORTRAIT_DAEMON             } },
    { WAR_UPGRADE_HEALING,          __bts(120), {  750,    0 }, { WAR_PORTRAIT_HEALING,          WAR_PORTRAIT_HEALING            } },
    { WAR_UPGRADE_RAISE_DEAD,       __bts(120), {  750,    0 }, { WAR_PORTRAIT_RAISE_DEAD,       WAR_PORTRAIT_RAISE_DEAD         } },
    { WAR_UPGRADE_FAR_SIGHT,        __bts(120), { 1500,    0 }, { WAR_PORTRAIT_FAR_SIGHT,        WAR_PORTRAIT_FAR_SIGHT          } },
    { WAR_UPGRADE_DARK_VISION,      __bts(120), { 1500,    0 }, { WAR_PORTRAIT_DARK_VISION,      WAR_PORTRAIT_DARK_VISION        } },
    { WAR_UPGRADE_INVISIBILITY,     __bts(120), { 3000,    0 }, { WAR_PORTRAIT_INVISIBILITY,     WAR_PORTRAIT_INVISIBILITY       } },
    { WAR_UPGRADE_UNHOLY_ARMOR,     __bts(120), { 3000,    0 }, { WAR_PORTRAIT_UNHOLY_ARMOR,     WAR_PORTRAIT_UNHOLY_ARMOR       } },
    { WAR_UPGRADE_SHIELD,           __bts(120), {  750, 1500 }, { WAR_PORTRAIT_SHIELD_2_HUMANS,  WAR_PORTRAIT_SHIELD_3_HUMANS    } },
};

typedef struct
{
    WarUnitCommandType type;
    s32 gold;
    s32 wood;
    s32 frameIndex; // always from resource 361
    char tooltip[100];
} WarUnitCommandData;

WarUnitData getUnitData(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(unitsData);
    while (index < length && unitsData[index].type != type)
        index++;

    assert(index < length);
    return unitsData[index];
}

WarWorkerData getWorkerData(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(workersData);
    while (index < length && workersData[index].type != type)
        index++;

    assert(index < length);
    return workersData[index];
}

WarBuildingData getBuildingsData(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(buildingsData);
    while (index < length && buildingsData[index].type != type)
        index++;

    assert(index < length);
    return buildingsData[index];
}

WarRoadsData getRoadsData(WarRoadPieceType type)
{
    s32 index = 0;
    s32 length = arrayLength(roadsData);
    while (index < length && roadsData[index].type != type)
        index++;

    assert(index < length);
    return roadsData[index];
}

WarWallsData getWallsData(WarWallPieceType type)
{
    s32 index = 0;
    s32 length = arrayLength(wallsData);
    while (index < length && wallsData[index].type != type)
        index++;

    assert(index < length);
    return wallsData[index];
}

WarRuinsData getRuinsData(WarRuinPieceType type)
{
    s32 index = 0;
    s32 length = arrayLength(ruinsData);
    while (index < length && ruinsData[index].type != type)
        index++;

    assert(index < length);
    return ruinsData[index];
}

WarTreesData getTreesData(WarTreeTileType type)
{
    s32 index = 0;
    s32 length = arrayLength(treesData);
    while (index < length && treesData[index].type != type)
        index++;

    assert(index < length);
    return treesData[index];
}

WarUnitStats getUnitStats(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(unitStats);
    while (index < length && unitStats[index].type != type)
        index++;

    assert(index < length);
    return unitStats[index];
}

WarBuildingStats getBuildingStats(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(buildingStats);
    while (index < length && buildingStats[index].type != type)
        index++;

    assert(index < length);
    return buildingStats[index];
}

WarUpgradeStats getUpgradeStats(WarUpgradeType type)
{
    s32 index = 0;
    s32 length = arrayLength(upgradeStats);
    while (index < length && upgradeStats[index].type != type)
        index++;

    assert(index < length);
    return upgradeStats[index];
}

#define isUnit(entity) ((entity)->type == WAR_ENTITY_TYPE_UNIT)
#define isUnitOfType(entity, unitType) (isUnit(entity) && (entity)->unit.type == (unitType))
#define isRoad(entity) ((entity)->type == WAR_ENTITY_TYPE_ROAD)
#define isWall(entity) ((entity)->type == WAR_ENTITY_TYPE_WALL)
#define isRuin(entity) ((entity)->type == WAR_ENTITY_TYPE_RUIN)

bool isDudeUnit(WarEntity* entity)
{
    if (!isUnit(entity))
        return false;

    switch (entity->unit.type)
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
        case WAR_UNIT_FIREELEMENTAL:
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

bool isMagicUnit(WarEntity* entity)
{
    if (!isUnit(entity))
        return false;

    switch (entity->unit.type)
    {
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
            return true;
    
        default:
            return false;
    }
}

bool isBuildingUnit(WarEntity* entity)
{
    if (!isUnit(entity))
        return false;

    switch (entity->unit.type)
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
        case WAR_UNIT_STABLES:
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

WarRace getUnitRace(WarEntity* entity)
{
    if (!isUnit(entity))
        return WAR_RACE_NEUTRAL;

    switch (entity->unit.type)
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
        case WAR_UNIT_WATER_ELEMENTAL:
        case WAR_UNIT_HUMAN_CORPSE:
        // buildings
        case WAR_UNIT_FARM_HUMANS:
        case WAR_UNIT_BARRACKS_HUMANS:
        case WAR_UNIT_CHURCH:
        case WAR_UNIT_TOWER_HUMANS:
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_LUMBERMILL_HUMANS:
        case WAR_UNIT_STABLES:
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
        case WAR_UNIT_FIREELEMENTAL:
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

WarUnitType getTownHallOfRace(WarRace race)
{
    switch (race)
    {
        case WAR_RACE_HUMANS: return WAR_UNIT_TOWNHALL_HUMANS;
        case WAR_RACE_ORCS: return WAR_UNIT_TOWNHALL_ORCS;
        default: return WAR_UNIT_TOWNHALL_HUMANS;
    }
}

vec2 getUnitSize(WarEntity* entity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    return vec2i(unit->sizex, unit->sizey);
}

vec2 getUnitFrameSize(WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    return vec2i(sprite->sprite.frameWidth, sprite->sprite.frameHeight);
}

rect getUnitFrameRect(WarEntity* entity)
{
    return rectv(VEC2_ZERO, getUnitFrameSize(entity));
}

vec2 getUnitSpriteSize(WarEntity* entity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    return vec2i(unit->sizex * MEGA_TILE_WIDTH, unit->sizey * MEGA_TILE_HEIGHT);
}

rect getUnitSpriteRect(WarEntity* entity)
{
    vec2 frameSize = getUnitFrameSize(entity);
    vec2 unitSize = getUnitSpriteSize(entity);
    vec2 pos = vec2Half(vec2Subv(frameSize, unitSize));
    return rectv(pos, unitSize);
}

vec2 getUnitSpriteCenter(WarEntity* entity)
{
    vec2 frameSize = getUnitFrameSize(entity);
    vec2 unitSize = getUnitSpriteSize(entity);
    vec2 pos = vec2Half(vec2Subv(frameSize, unitSize));
    return vec2Addv(pos, vec2Half(unitSize));
}

vec2 getUnitCenterPosition(WarEntity* entity, bool inTiles)
{
    WarTransformComponent* transform = &entity->transform;
    vec2 spriteSize = getUnitSpriteSize(entity);
    vec2 unitCenter = vec2Half(spriteSize);
    vec2 position = vec2Addv(transform->position, unitCenter);
    return inTiles ? vec2MapToTileCoordinates(position) : position;
}

void setUnitCenterPosition(WarEntity* entity, vec2 position, bool inTiles)
{
    if (inTiles)
        position = vec2TileToMapCoordinates(position, true);

    WarTransformComponent* transform = &entity->transform;
    vec2 spriteSize = getUnitSpriteSize(entity);
    vec2 unitCenter = vec2Half(spriteSize);
    transform->position = vec2Subv(position, unitCenter);
}

WarUnitDirection getUnitDirection(WarEntity* entity)
{
    assert(isUnit(entity));

    return entity->unit.direction;
}

WarUnitDirection getDirectionFromDiff(f32 x, f32 y)
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

void setUnitDirection(WarEntity* entity, WarUnitDirection direction)
{
    assert(isUnit(entity));

    entity->unit.direction = direction;
}

void setUnitDirectionFromDiff(WarEntity* entity, f32 dx, f32 dy)
{
    assert(isUnit(entity));

    WarUnitDirection direction = getDirectionFromDiff(dx, dy);
    setUnitDirection(entity, direction);
}

f32 getUnitActionScale(WarEntity* entity)
{
    assert(isUnit(entity));

    // this is the scale of the animation, for walking 
    // the lower the less time is the transition between the frames 
    // thus the animation goes faster.
    //
    // level 0 -> 1.0f
    // level 1 -> 0.9f
    // level 2 -> 0.8f
    return 1 - entity->unit.level * 0.1f;
}

vec2 unitPointOnTarget(WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));
    assert(isUnit(targetEntity));

    vec2 position = getUnitCenterPosition(entity, true);

    vec2 targetPosition = vec2MapToTileCoordinates(targetEntity->transform.position);
    vec2 unitSize = getUnitSize(targetEntity);
    rect unitRect = rectv(targetPosition, unitSize);
    
    return getClosestPointOnRect(position, unitRect);
}

s32 positionDistanceInTiles(WarEntity* entity, vec2 targetPosition)
{
    assert(isUnit(entity));

    vec2 position = getUnitCenterPosition(entity, true);
    f32 distance = vec2DistanceInTiles(position, targetPosition);
    return (s32)distance;
}

bool positionInRange(WarEntity* entity, vec2 targetPosition, s32 range)
{
    assert(range >= 0);

    s32 distance = positionDistanceInTiles(entity, targetPosition);
    return distance <= range;
}

s32 unitDistanceInTiles(WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));

    vec2 pointOnTarget = unitPointOnTarget(entity, targetEntity);
    return positionDistanceInTiles(entity, pointOnTarget);
}

bool unitInRange(WarEntity* entity, WarEntity* targetEntity, s32 range)
{
    assert(range >= 0);

    s32 distance = unitDistanceInTiles(entity, targetEntity);
    return distance <= range;
}

bool isCarryingResources(WarEntity* entity)
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

s32 getPlayerUnitTotalCount(WarContext* context, u8 player)
{
    WarMap* map = context->map;

    s32 count = 0;

    for (s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity && isUnit(entity))
        {
            if (entity->unit.player == player)
            {
                count++;
            }
        }
    }

    return count;
}

s32 getPlayerDudesCount(WarContext* context, u8 player)
{
    WarMap* map = context->map;

    s32 count = 0;

    for (s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity && isUnit(entity))
        {
            if (entity->unit.player == player && isDudeUnit(entity))
            {
                count++;
            }
        }
    }

    return count;
}

s32 getPlayerBuildingsCount(WarContext* context, u8 player)
{
    WarMap* map = context->map;

    s32 count = 0;

    for (s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity && isUnit(entity))
        {
            if (entity->unit.player == player && isBuildingUnit(entity))
            {
                count++;
            }
        }
    }

    return count;
}

s32 getPlayerUnitCount(WarContext* context, u8 player, WarUnitType unitType)
{
    WarMap* map = context->map;

    s32 count = 0;

    for (s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity && isUnit(entity))
        {
            if (entity->unit.player == player && 
                entity->unit.type == unitType)
            {
                count++;
            }
        }
    }

    return count;
}

bool playerHasUnit(WarContext* context, s32 player, WarUnitType unitType)
{
    return getPlayerUnitCount(context, player, unitType) > 0;
}

void getUnitCommands(WarContext* context, WarEntity* entity, WarUnitCommandType commands[])
{
    assert(entity);
    assert(isUnit(entity));

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    WarUnitComponent* unit = &entity->unit;

    if (player->race != getUnitRace(entity))
        return;

    switch (unit->type)
    {
        // units
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        {
            commands[0] = WAR_COMMAND_MOVE;
            commands[1] = WAR_COMMAND_STOP;
            commands[2] = WAR_COMMAND_ATTACK;

            break;
        }

        case WAR_UNIT_PEASANT:
        case WAR_UNIT_PEON:
        {
            commands[0] = WAR_COMMAND_MOVE;
            commands[1] = WAR_COMMAND_STOP;

            // if the unit is carrying, instead of harvest it should be deliver
            commands[2] = unit->resourceKind == WAR_RESOURCE_NONE
                ? WAR_COMMAND_HARVEST : WAR_COMMAND_DELIVER;

            commands[3] = WAR_COMMAND_REPAIR;
            commands[4] = WAR_COMMAND_BUILD_BASIC;

            // only if there is a lumber mill
            WarUnitType lumberMillType = isHuman(player)
                ? WAR_UNIT_LUMBERMILL_HUMANS : WAR_UNIT_LUMBERMILL_ORCS;
            if (playerHasUnit(context, player->index, lumberMillType))
            {
                commands[5] = WAR_COMMAND_BUILD_ADVANCED;
            }
            break;
        }

        case WAR_UNIT_CONJURER:
        {
            commands[0] = WAR_COMMAND_MOVE;
            commands[1] = WAR_COMMAND_STOP;
            commands[2] = WAR_COMMAND_ATTACK;

            // only if these spells are researshed
            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAIN_OF_FIRE) && 
                hasAnyUpgrade(player, WAR_UPGRADE_RAIN_OF_FIRE))
            {
                commands[3] = WAR_COMMAND_SPELL_RAIN_OF_FIRE;
            }

            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SCORPION) && 
                hasAnyUpgrade(player, WAR_UPGRADE_SCORPIONS))
            {
                commands[4] = WAR_COMMAND_SUMMON_SCORPION;
            }

            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_WATER_ELEMENTAL) && 
                hasAnyUpgrade(player, WAR_UPGRADE_WATER_ELEMENTAL))
            {
                commands[5] = WAR_COMMAND_SUMMON_WATER_ELEMENTAL;
            }
    
            break;
        }

        case WAR_UNIT_WARLOCK:
        {
            commands[0] = WAR_COMMAND_MOVE;
            commands[1] = WAR_COMMAND_STOP;
            commands[2] = WAR_COMMAND_ATTACK;

            // only if these spells are researshed
            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_POISON_CLOUD) && 
                hasAnyUpgrade(player, WAR_UPGRADE_POISON_CLOUD))
            {
                commands[3] = WAR_COMMAND_SPELL_POISON_CLOUD;
            }

            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SPIDER) && 
                hasAnyUpgrade(player, WAR_UPGRADE_SPIDERS))
            {
                commands[4] = WAR_COMMAND_SUMMON_SPIDER;
            }

            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DAEMON) && 
                hasAnyUpgrade(player, WAR_UPGRADE_DAEMON))
            {
                commands[5] = WAR_COMMAND_SUMMON_DAEMON;
            }

            break;
        }

        case WAR_UNIT_CLERIC:
        {
            commands[0] = WAR_COMMAND_MOVE;
            commands[1] = WAR_COMMAND_STOP;
            commands[2] = WAR_COMMAND_ATTACK;

            // only if these spells are researshed
            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_HEALING) && 
                hasAnyUpgrade(player, WAR_UPGRADE_HEALING))
            {
                commands[3] = WAR_COMMAND_SPELL_HEALING;
            }

            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_FAR_SIGHT) && 
                hasAnyUpgrade(player, WAR_UPGRADE_FAR_SIGHT))
            {
                commands[4] = WAR_COMMAND_SPELL_FAR_SIGHT;
            }

            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_INVISIBILITY) && 
                hasAnyUpgrade(player, WAR_UPGRADE_INVISIBILITY))
            {
                commands[5] = WAR_COMMAND_SPELL_INVISIBILITY;
            }

            break;
        }

        case WAR_UNIT_NECROLYTE:
        {
            commands[0] = WAR_COMMAND_MOVE;
            commands[1] = WAR_COMMAND_STOP;
            commands[2] = WAR_COMMAND_ATTACK;

            // only if these spells are researshed
            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAISE_DEAD) && 
                hasAnyUpgrade(player, WAR_UPGRADE_RAISE_DEAD))
            {
                commands[3] = WAR_COMMAND_SPELL_RAISE_DEAD;
            }

            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DARK_VISION) && 
                hasAnyUpgrade(player, WAR_UPGRADE_DARK_VISION))
            {
                commands[4] = WAR_COMMAND_SPELL_DARK_VISION;
            }

            if (isFeatureAllowed(player, WAR_FEATURE_SPELL_UNHOLY_ARMOR) && 
                hasAnyUpgrade(player, WAR_UPGRADE_UNHOLY_ARMOR))
            {
                commands[5] = WAR_COMMAND_SPELL_UNHOLY_ARMOR;
            }

            break;
        }

        // buildings
        case WAR_UNIT_BARRACKS_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_FOOTMAN;

                // only if there is a lumber mill
                if (playerHasUnit(context, player->index, WAR_UNIT_LUMBERMILL_HUMANS))
                {
                    commands[1] = WAR_COMMAND_TRAIN_ARCHER;
                }

                // only if there is a stable
                if (playerHasUnit(context, player->index, WAR_UNIT_BLACKSMITH_HUMANS) &&
                    playerHasUnit(context, player->index, WAR_UNIT_STABLES))
                {
                    commands[2] = WAR_COMMAND_TRAIN_KNIGHT;
                }

                // only if there is a blacksmith
                if (playerHasUnit(context, player->index, WAR_UNIT_BLACKSMITH_HUMANS))
                {
                    commands[3] = WAR_COMMAND_TRAIN_CATAPULT_HUMANS;
                }
            }

            break;
        }
        case WAR_UNIT_BARRACKS_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_GRUNT;

                // only if there is a lumber mill
                if (playerHasUnit(context, player->index, WAR_UNIT_LUMBERMILL_ORCS))
                {
                    commands[1] = WAR_COMMAND_TRAIN_SPEARMAN;
                }

                // only if there is a kennel
                if (playerHasUnit(context, player->index, WAR_UNIT_BLACKSMITH_ORCS) &&
                    playerHasUnit(context, player->index, WAR_UNIT_KENNEL))
                {
                    commands[2] = WAR_COMMAND_TRAIN_RAIDER;
                }

                // only if there is a blacksmith
                if (playerHasUnit(context, player->index, WAR_UNIT_BLACKSMITH_ORCS))
                {
                    commands[3] = WAR_COMMAND_TRAIN_CATAPULT_ORCS;
                }
            }

            break;
        }
        case WAR_UNIT_CHURCH:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_CLERIC;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_HEALING) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_HEALING))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_HEALING;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_FAR_SIGHT) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_FAR_SIGHT))
                {
                    commands[2] = WAR_COMMAND_UPGRADE_FAR_SIGHT;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_INVISIBILITY) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_INVISIBILITY))
                {
                    commands[3] = WAR_COMMAND_UPGRADE_INVISIBILITY;
                }
            }

            break;
        }
        case WAR_UNIT_TEMPLE:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_NECROLYTE;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAISE_DEAD) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_RAISE_DEAD))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_RAISE_DEAD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DARK_VISION) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_DARK_VISION))
                {
                    commands[2] = WAR_COMMAND_UPGRADE_DARK_VISION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_UNHOLY_ARMOR) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_UNHOLY_ARMOR))
                {
                    commands[3] = WAR_COMMAND_UPGRADE_UNHOLY_ARMOR;
                }
            }

            break;
        }
        case WAR_UNIT_TOWER_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_CONJURER;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SCORPION) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_SCORPIONS))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_SCORPION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAIN_OF_FIRE) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_RAIN_OF_FIRE))
                {
                    commands[2] = WAR_COMMAND_UPGRADE_RAIN_OF_FIRE;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_WATER_ELEMENTAL) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_WATER_ELEMENTAL))
                {
                    commands[3] = WAR_COMMAND_UPGRADE_WATER_ELEMENTAL;
                }
            }

            break;
        }
        case WAR_UNIT_TOWER_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_WARLOCK;
                
                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SPIDER) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_SPIDERS))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_SPIDER;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_POISON_CLOUD) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_POISON_CLOUD))
                {
                    commands[2] = WAR_COMMAND_UPGRADE_POISON_CLOUD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DAEMON) && 
                    hasRemainingUpgrade(player, WAR_UPGRADE_DAEMON))
                {
                    commands[3] = WAR_COMMAND_UPGRADE_DAEMON;
                }
            }

            break;
        }
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_TOWNHALL_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = unit->type == WAR_UNIT_TOWNHALL_HUMANS
                    ? WAR_COMMAND_TRAIN_PEASANT : WAR_COMMAND_TRAIN_PEON;

                commands[1] = WAR_COMMAND_BUILD_ROAD;

                // only if this is allowed
                if (isFeatureAllowed(player, WAR_FEATURE_UNIT_WALL))
                {
                    commands[2] = WAR_COMMAND_BUILD_WALL;
                }
            }

            break;
        }
        case WAR_UNIT_LUMBERMILL_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_ARROWS))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_ARROWS;
                }
            }

            break;
        }
        case WAR_UNIT_LUMBERMILL_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_SPEARS))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_SPEARS;
                }
            }
            
            break;
        }
        case WAR_UNIT_STABLES:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_HORSES))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_HORSES;
                }
            }
            
            break;
        }
        case WAR_UNIT_KENNEL:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_WOLVES))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_WOLVES;
                }
            }

            break;
        }
        case WAR_UNIT_BLACKSMITH_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_SWORDS))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_SWORDS;
                }

                if (hasRemainingUpgrade(player, WAR_UPGRADE_SHIELD))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_SHIELD_HUMANS;
                }
            }

            break;
        }
        case WAR_UNIT_BLACKSMITH_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_AXES))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_AXES;
                }

                if (hasRemainingUpgrade(player, WAR_UPGRADE_SHIELD))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_SHIELD_ORCS;
                }
            }

            break;
        }

        default:
        {
            logWarning("Commands for unit type %d not handled yet.\n", unit->type);
            break;
        }
    }
}

void getBuildBasicCommands(WarContext* context, WarEntity* entity, WarUnitCommandType commands[])
{
    assert(entity);
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    switch (unit->type)
    {
        case WAR_UNIT_PEASANT:
        {
            commands[0] = WAR_COMMAND_BUILD_FARM_HUMANS;
            commands[1] = WAR_COMMAND_BUILD_LUMBERMILL_HUMANS;
            commands[2] = WAR_COMMAND_BUILD_BARRACKS_HUMANS;
            commands[6] = WAR_COMMAND_CANCEL;
            break;
        }
        case WAR_UNIT_PEON:
        {
            commands[0] = WAR_COMMAND_BUILD_FARM_ORCS;
            commands[1] = WAR_COMMAND_BUILD_LUMBERMILL_ORCS;
            commands[2] = WAR_COMMAND_BUILD_BARRACKS_ORCS;
            commands[6] = WAR_COMMAND_CANCEL;
            break;
        }

        default:
        {
            logWarning("Trying to get build basic commands for unit of type: %d\n", unit->type);
            assert(false);
            break;
        }
    }
}

void getBuildAdvancedCommands(WarContext* context, WarEntity* entity, WarUnitCommandType commands[])
{
    assert(entity);
    assert(isUnit(entity));

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    WarUnitComponent* unit = &entity->unit;

    switch (unit->type)
    {
        case WAR_UNIT_PEASANT:
        {
            commands[0] = WAR_COMMAND_BUILD_BLACKSMITH_HUMANS;
            commands[1] = WAR_COMMAND_BUILD_CHURCH;
            commands[2] = WAR_COMMAND_BUILD_STABLE;

            if (playerHasUnit(context, player->index, WAR_UNIT_BLACKSMITH_HUMANS))
            {
                commands[3] = WAR_COMMAND_BUILD_TOWER_HUMANS;
            }

            commands[6] = WAR_COMMAND_CANCEL;
            break;
        }
        case WAR_UNIT_PEON:
        {
            commands[0] = WAR_COMMAND_BUILD_BARRACKS_ORCS;
            commands[1] = WAR_COMMAND_BUILD_TEMPLE;
            commands[2] = WAR_COMMAND_BUILD_KENNEL;
            
            if (playerHasUnit(context, player->index, WAR_UNIT_BLACKSMITH_ORCS))
            {
                commands[3] = WAR_COMMAND_BUILD_TOWER_ORCS;
            }

            commands[6] = WAR_COMMAND_CANCEL;
            break;
        }

        default:
        {
            logWarning("Trying to get build advanced commands for unit of type: %d\n", unit->type);
            assert(false);
            break;
        }
    }
}

WarUnitCommandData getUnitCommandData(WarContext* context, WarEntity* entity, WarUnitCommandType commandType)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitCommandData data = (WarUnitCommandData){0};
    data.type = commandType;

    switch (commandType)
    {
        // unit commands
        case WAR_COMMAND_MOVE:
        {
            strcpy(data.tooltip, "MOVE");
            data.frameIndex = isHuman(player) 
                ? WAR_PORTRAIT_MOVE_HUMANS : WAR_PORTRAIT_MOVE_ORCS;
            break;
        }
        case WAR_COMMAND_STOP:
        {
            strcpy(data.tooltip, "STOP");

            // check here upgrades for the shields
            if (getUpgradeLevel(player, WAR_UPGRADE_SHIELD) == 2)
            {
                data.frameIndex = isHuman(player)
                    ? WAR_PORTRAIT_SHIELD_3_HUMANS : WAR_PORTRAIT_SHIELD_3_ORCS;
            }
            else if (getUpgradeLevel(player, WAR_UPGRADE_SHIELD) == 1)
            {
                data.frameIndex = isHuman(player)
                    ? WAR_PORTRAIT_SHIELD_2_HUMANS : WAR_PORTRAIT_SHIELD_2_ORCS;
            }
            else
            {
                data.frameIndex = isHuman(player)
                    ? WAR_PORTRAIT_SHIELD_1_HUMANS : WAR_PORTRAIT_SHIELD_1_ORCS;
            }

            break;
        }
        case WAR_COMMAND_HARVEST:
        {
            strcpy(data.tooltip, "HARVEST LUMBER / MINE GOLD");
            data.frameIndex = WAR_PORTRAIT_HARVEST;
            break;
        }
        case WAR_COMMAND_DELIVER:
        {
            strcpy(data.tooltip, "RETURN GOODS TO TOWN HALL");
            data.frameIndex = WAR_PORTRAIT_DELIVER;
            break;
        }
        case WAR_COMMAND_REPAIR:
        {
            strcpy(data.tooltip, "REPAIR");
            data.frameIndex = WAR_PORTRAIT_REPAIR;
            break;
        }
        case WAR_COMMAND_BUILD_BASIC:
        {
            strcpy(data.tooltip, "BUILD BASIC STRUCTURES");
            data.frameIndex = WAR_PORTRAIT_BUILD_BASIC;
            break;
        }
        case WAR_COMMAND_BUILD_ADVANCED:
        {
            strcpy(data.tooltip, "BUILD ADVANCED STRUCTURES");
            data.frameIndex = WAR_PORTRAIT_BUILD_ADVANCED;
            break;
        }
        case WAR_COMMAND_ATTACK:
        {
            strcpy(data.tooltip, "ATTACK");

            if (entity && isUnit(entity))
            {
                WarUnitComponent* unit = &entity->unit;
                switch (unit->type)
                {
                    // units
                    case WAR_UNIT_FOOTMAN:
                    case WAR_UNIT_GRUNT:
                    case WAR_UNIT_KNIGHT:
                    case WAR_UNIT_RAIDER:
                    case WAR_UNIT_CATAPULT_HUMANS:
                    case WAR_UNIT_CATAPULT_ORCS:
                    {
                        // check here upgrades for the sword
                        WarUpgradeType swordAxeUpgrade = isHuman(player)
                            ? WAR_UPGRADE_SWORDS : WAR_UPGRADE_AXES;

                        if (getUpgradeLevel(player, swordAxeUpgrade) == 2)
                        {
                            data.frameIndex = isHuman(player)
                                ? WAR_PORTRAIT_SWORD_3 : WAR_PORTRAIT_AXE_3;
                        }
                        else if (getUpgradeLevel(player, swordAxeUpgrade) == 1)
                        {
                            data.frameIndex = isHuman(player)
                                ? WAR_PORTRAIT_SWORD_2 : WAR_PORTRAIT_AXE_2;
                        }
                        else
                        {
                            data.frameIndex = isHuman(player)
                                ? WAR_PORTRAIT_SWORD_1 : WAR_PORTRAIT_AXE_1;
                        }

                        break;
                    }

                    case WAR_UNIT_ARCHER:
                    case WAR_UNIT_SPEARMAN:
                    {
                        // check here upgrades for the arrows
                        WarUpgradeType arrowSpearUpgrade = isHuman(player)
                            ? WAR_UPGRADE_ARROWS : WAR_UPGRADE_SPEARS;

                        if (getUpgradeLevel(player, arrowSpearUpgrade) == 2)
                        {
                            data.frameIndex = isHuman(player)
                                ? WAR_PORTRAIT_ARROW_3 : WAR_PORTRAIT_SPEAR_3;
                        }
                        else if (getUpgradeLevel(player, arrowSpearUpgrade) == 1)
                        {
                            data.frameIndex = isHuman(player)
                                ? WAR_PORTRAIT_ARROW_2 : WAR_PORTRAIT_SPEAR_2;
                        }
                        else
                        {
                            data.frameIndex = isHuman(player)
                                ? WAR_PORTRAIT_ARROW_1 : WAR_PORTRAIT_SPEAR_1;
                        }

                        break;
                    }

                    case WAR_UNIT_CONJURER:
                    {
                        data.frameIndex = WAR_PORTRAIT_ELEMENTAL_BLAST;
                        break;
                    }

                    case WAR_UNIT_CLERIC:
                    {
                        data.frameIndex = WAR_PORTRAIT_HOLY_LANCE;
                        break;
                    }                    

                    case WAR_UNIT_NECROLYTE:
                    {
                        data.frameIndex = WAR_PORTRAIT_SHADOW_SPEAR;
                        break;
                    }

                    case WAR_UNIT_WARLOCK:
                    {
                        data.frameIndex = WAR_PORTRAIT_FIREBALL;
                        break;
                    }

                    default:
                    {
                        logWarning("Trying to get an attack command for unit of type: %d\n", unit->type);
                        break;
                    }
                }
            }
            else
            {
                // check here upgrades for the sword
                if (isHuman(player))
                {
                    if (getUpgradeLevel(player, WAR_UPGRADE_SWORDS) == 2)
                        data.frameIndex = WAR_PORTRAIT_SWORD_3;
                    else if (getUpgradeLevel(player, WAR_UPGRADE_SWORDS) == 1)
                        data.frameIndex = WAR_PORTRAIT_SWORD_2;
                    else
                        data.frameIndex = WAR_PORTRAIT_SWORD_1;
                }
                else if (isOrc(player))
                {
                    if (getUpgradeLevel(player, WAR_UPGRADE_AXES) == 2)
                        data.frameIndex = WAR_PORTRAIT_AXE_3;
                    else if (getUpgradeLevel(player, WAR_UPGRADE_AXES) == 1)
                        data.frameIndex = WAR_PORTRAIT_AXE_2;
                    else
                        data.frameIndex = WAR_PORTRAIT_AXE_1;
                }
                else
                {
                    logWarning("Trying to get a frame index for player race: %d\n", player->race);
                    assert(false);
                }
            }
            
            break;
        }
        
        case WAR_COMMAND_TRAIN_PEASANT:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_PEASANT);

            strcpy(data.tooltip, "TRAIN PEASANT");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_PEASANT;

            break;
        }

        case WAR_COMMAND_TRAIN_PEON:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_PEON);

            strcpy(data.tooltip, "TRAIN PEON");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_PEON;

            break;
        }

        case WAR_COMMAND_TRAIN_FOOTMAN:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_FOOTMAN);

            strcpy(data.tooltip, "TRAIN FOOTMAN");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_FOOTMAN;
            break;
        }

        case WAR_COMMAND_TRAIN_GRUNT:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_GRUNT);

            strcpy(data.tooltip, "TRAIN GRUNT");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_GRUNT;
            break;
        }

        case WAR_COMMAND_TRAIN_CATAPULT_HUMANS:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_CATAPULT_HUMANS);

            strcpy(data.tooltip, "TRAIN CATAPULT");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_CATAPULT_HUMANS;
            break;
        }

        case WAR_COMMAND_TRAIN_CATAPULT_ORCS:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_CATAPULT_ORCS);

            strcpy(data.tooltip, "TRAIN CATAPULT");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_CATAPULT_ORCS;
            break;
        }

        case WAR_COMMAND_TRAIN_KNIGHT:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_KNIGHT);

            strcpy(data.tooltip, "TRAIN KNIGHT");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_KNIGHT;
            break;
        }

        case WAR_COMMAND_TRAIN_RAIDER:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_RAIDER);

            strcpy(data.tooltip, "TRAIN RAIDER");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_RAIDER;
            break;
        }

        case WAR_COMMAND_TRAIN_ARCHER:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_ARCHER);

            strcpy(data.tooltip, "TRAIN ARCHER");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_ARCHER;
            break;
        }

        case WAR_COMMAND_TRAIN_SPEARMAN:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_SPEARMAN);

            strcpy(data.tooltip, "TRAIN SPEARMAN");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_SPEARMAN;
            break;
        }

        case WAR_COMMAND_TRAIN_CONJURER:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_CONJURER);

            strcpy(data.tooltip, "TRAIN CONJURER");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_CONJURER;
            break;
        }

        case WAR_COMMAND_TRAIN_WARLOCK:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_WARLOCK);

            strcpy(data.tooltip, "TRAIN WARLOCK");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_WARLOCK;
            break;
        }

        case WAR_COMMAND_TRAIN_CLERIC:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_CLERIC);

            strcpy(data.tooltip, "TRAIN CLERIC");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_CLERIC;
            break;
        }

        case WAR_COMMAND_TRAIN_NECROLYTE:
        {
            WarUnitStats stats = getUnitStats(WAR_UNIT_NECROLYTE);

            strcpy(data.tooltip, "TRAIN NECROLYTE");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_NECROLYTE;
            break;
        }

        case WAR_COMMAND_BUILD_FARM_HUMANS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_FARM_HUMANS);

            strcpy(data.tooltip, "TRAIN FARM");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_FARM_HUMANS;
            break;
        }

        case WAR_COMMAND_BUILD_FARM_ORCS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_FARM_ORCS);

            strcpy(data.tooltip, "TRAIN FARM");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_FARM_ORCS;
            break;
        }

        case WAR_COMMAND_BUILD_BARRACKS_HUMANS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_BARRACKS_HUMANS);

            strcpy(data.tooltip, "TRAIN BARRACKS");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_BARRACKS_HUMANS;
            break;
        }

        case WAR_COMMAND_BUILD_BARRACKS_ORCS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_BARRACKS_ORCS);

            strcpy(data.tooltip, "TRAIN BARRACKS");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_BARRACKS_ORCS;
            break;
        }

        case WAR_COMMAND_BUILD_CHURCH:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_CHURCH);

            strcpy(data.tooltip, "TRAIN CHURCH");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_CHURCH;
            break;
        }

        case WAR_COMMAND_BUILD_TEMPLE:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_TEMPLE);

            strcpy(data.tooltip, "TRAIN TEMPLE");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_TEMPLE;

            break;
        }

        case WAR_COMMAND_BUILD_TOWER_HUMANS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_TOWER_HUMANS);

            strcpy(data.tooltip, "TRAIN TOWER");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_TOWER_HUMANS;

            break;
        }

        case WAR_COMMAND_BUILD_TOWER_ORCS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_TOWER_ORCS);

            strcpy(data.tooltip, "TRAIN TOWER");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_TOWER_ORCS;

            break;
        }

        case WAR_COMMAND_BUILD_TOWNHALL_HUMANS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_TOWNHALL_HUMANS);

            strcpy(data.tooltip, "TRAIN TOWNHALL");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_TOWNHALL_HUMANS;
            break;
        }

        case WAR_COMMAND_BUILD_TOWNHALL_ORCS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_TOWNHALL_ORCS);

            strcpy(data.tooltip, "TRAIN TOWNHALL");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_TOWNHALL_ORCS;
            break;
        }

        case WAR_COMMAND_BUILD_LUMBERMILL_HUMANS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_LUMBERMILL_HUMANS);

            strcpy(data.tooltip, "TRAIN LUMBERMILL");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_LUMBERMILL_HUMANS;
            break;
        }

        case WAR_COMMAND_BUILD_LUMBERMILL_ORCS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_LUMBERMILL_ORCS);

            strcpy(data.tooltip, "TRAIN LUMBERMILL");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_LUMBERMILL_ORCS;
            break;
        }

        case WAR_COMMAND_BUILD_STABLE:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_STABLES);

            strcpy(data.tooltip, "TRAIN STABLES");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_STABLES;
            break;
        }

        case WAR_COMMAND_BUILD_KENNEL:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_KENNEL);

            strcpy(data.tooltip, "TRAIN KENNEL");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_KENNEL;
            break;
        }

        case WAR_COMMAND_BUILD_BLACKSMITH_HUMANS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_BLACKSMITH_HUMANS);

            strcpy(data.tooltip, "TRAIN BLACKSMITH");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_BLACKSMITH_HUMANS;
            break;
        }

        case WAR_COMMAND_BUILD_BLACKSMITH_ORCS:
        {
            WarBuildingStats stats = getBuildingStats(WAR_UNIT_BLACKSMITH_ORCS);

            strcpy(data.tooltip, "TRAIN BLACKSMITH");
            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = WAR_PORTRAIT_BLACKSMITH_ORCS;
            break;
        }

        case WAR_COMMAND_BUILD_ROAD:
        {
            strcpy(data.tooltip, "BUILD ROAD");
            data.gold = 0;
            data.wood = 0;
            data.frameIndex = WAR_PORTRAIT_ROAD;
            break;
        }

        case WAR_COMMAND_BUILD_WALL:
        {
            strcpy(data.tooltip, "BUILD WALL");
            data.gold = 0;
            data.wood = 0;
            data.frameIndex = WAR_PORTRAIT_WALL;
            break;
        }

        case WAR_COMMAND_UPGRADE_SWORDS:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_SWORDS));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_SWORDS);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_SWORDS);

            strcpy(data.tooltip, "UPGRADE SWORDS");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_AXES:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_AXES));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_AXES);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_AXES);

            strcpy(data.tooltip, "UPGRADE AXES");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_SHIELD_HUMANS:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_SHIELD));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_SHIELD);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_SHIELD);

            strcpy(data.tooltip, "UPGRADE SHIELD");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_SHIELD_ORCS:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_SHIELD));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_SHIELD);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_SHIELD);

            strcpy(data.tooltip, "UPGRADE SHIELD");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_ARROWS:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_ARROWS));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_ARROWS);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_ARROWS);

            strcpy(data.tooltip, "UPGRADE ARROWS");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_SPEARS:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_SPEARS));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_SPEARS);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_SPEARS);

            strcpy(data.tooltip, "UPGRADE SPEARS");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_HORSES:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_HORSES));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_HORSES);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_HORSES);

            strcpy(data.tooltip, "UPGRADE HORSES");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_WOLVES:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_WOLVES));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_WOLVES);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_WOLVES);

            strcpy(data.tooltip, "UPGRADE WOLVES");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_SCORPION:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_SCORPIONS));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_SCORPIONS);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_SCORPIONS);

            strcpy(data.tooltip, "UPGRADE SCORPIONS");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_SPIDER:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_SPIDERS));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_SPIDERS);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_SPIDERS);

            strcpy(data.tooltip, "UPGRADE SPIDERS");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_RAIN_OF_FIRE:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_RAIN_OF_FIRE));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_RAIN_OF_FIRE);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_RAIN_OF_FIRE);

            strcpy(data.tooltip, "UPGRADE RAIN_OF_FIRE");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_POISON_CLOUD:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_POISON_CLOUD));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_POISON_CLOUD);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_POISON_CLOUD);

            strcpy(data.tooltip, "UPGRADE POISON_CLOUD");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_WATER_ELEMENTAL:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_WATER_ELEMENTAL));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_WATER_ELEMENTAL);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_WATER_ELEMENTAL);

            strcpy(data.tooltip, "UPGRADE WATER_ELEMENTAL");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_DAEMON:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_DAEMON));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_DAEMON);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_DAEMON);

            strcpy(data.tooltip, "UPGRADE DAEMON");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_HEALING:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_HEALING));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_HEALING);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_HEALING);

            strcpy(data.tooltip, "UPGRADE HEALING");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_RAISE_DEAD:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_RAISE_DEAD));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_RAISE_DEAD);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_RAISE_DEAD);

            strcpy(data.tooltip, "UPGRADE RAISE_DEAD");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_FAR_SIGHT:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_FAR_SIGHT));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_FAR_SIGHT);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_FAR_SIGHT);

            strcpy(data.tooltip, "UPGRADE FAR_SIGHT");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_DARK_VISION:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_DARK_VISION));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_DARK_VISION);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_DARK_VISION);

            strcpy(data.tooltip, "UPGRADE DARK_VISION");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_INVISIBILITY:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_INVISIBILITY));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_INVISIBILITY);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_INVISIBILITY);

            strcpy(data.tooltip, "UPGRADE INVISIBILITY");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_UPGRADE_UNHOLY_ARMOR:
        {
            assert(hasRemainingUpgrade(player, WAR_UPGRADE_UNHOLY_ARMOR));

            WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_UNHOLY_ARMOR);
            s32 upgradeLevel = getUpgradeLevel(player, WAR_UPGRADE_UNHOLY_ARMOR);

            strcpy(data.tooltip, "UPGRADE UNHOLY_ARMOR");
            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = stats.frameIndices[upgradeLevel];
            break;
        }

        // cancel
        case WAR_COMMAND_CANCEL:
        {
            strcpy(data.tooltip, "CANCEL");
            data.frameIndex = WAR_PORTRAIT_CANCEL;
            break;
        }

        default:
        {
            logWarning("Unkown command type: %d\n", commandType);
            break;
        }
    }

    return data;
}
