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

typedef struct
{
    WarUnitType type;
    s32 resourceIndex;
    s32 sizex;
    s32 sizey;
} WarUnitsData;

const WarUnitsData unitsData[] = 
{
    // units
    { WAR_UNIT_FOOTMAN,                   279, 1, 1 },
    { WAR_UNIT_GRUNT,                     280, 1, 1 },
    { WAR_UNIT_PEASANT,                   281, 1, 1 },
    { WAR_UNIT_PEON,                      282, 1, 1 },
    { WAR_UNIT_CATAPULT_HUMANS,           283, 1, 1 },
    { WAR_UNIT_CATAPULT_ORCS,             284, 1, 1 },
    { WAR_UNIT_KNIGHT,                    285, 1, 1 },
    { WAR_UNIT_RAIDER,                    286, 1, 1 },
    { WAR_UNIT_ARCHER,                    287, 1, 1 },
    { WAR_UNIT_SPEARMAN,                  288, 1, 1 },
    { WAR_UNIT_CONJURER,                  289, 1, 1 },
    { WAR_UNIT_WARLOCK,                   290, 1, 1 },
    { WAR_UNIT_CLERIC,                    291, 1, 1 },
    { WAR_UNIT_NECROLYTE,                 292, 1, 1 },
    { WAR_UNIT_MEDIVH,                    293, 1, 1 },
    { WAR_UNIT_LOTHAR,                    294, 1, 1 },
    { WAR_UNIT_WOUNDED,                   295, 1, 1 },
    { WAR_UNIT_GRIZELDA,                  296, 1, 1 },
    { WAR_UNIT_GARONA,                    296, 1, 1 },
    { WAR_UNIT_OGRE,                      297, 1, 1 },
    { WAR_UNIT_SPIDER,                    298, 1, 1 },
    { WAR_UNIT_SLIME,                     299, 1, 1 },
    { WAR_UNIT_FIREELEMENTAL,             300, 1, 1 },
    { WAR_UNIT_SCORPION,                  301, 1, 1 },
    { WAR_UNIT_BRIGAND,                   302, 1, 1 },
    { WAR_UNIT_THE_DEAD,                  303, 1, 1 },
    { WAR_UNIT_SKELETON,                  304, 1, 1 },
    { WAR_UNIT_DAEMON,                    305, 1, 1 },
    { WAR_UNIT_WATERELEMENTAL,            306, 1, 1 },

    // buildings
    { WAR_UNIT_FARM_HUMANS,               307, 2, 2 },
    { WAR_UNIT_FARM_ORCS,                 308, 2, 2 },
    { WAR_UNIT_BARRACKS_HUMANS,           309, 3, 3 },
    { WAR_UNIT_BARRACKS_ORCS,             310, 3, 3 },
    { WAR_UNIT_CHURCH,                    311, 2, 2 },
    { WAR_UNIT_TEMPLE,                    312, 2, 2 },
    { WAR_UNIT_TOWER_HUMANS,              313, 2, 2 },
    { WAR_UNIT_TOWER_ORCS,                314, 2, 2 },
    { WAR_UNIT_TOWNHALL_HUMANS,           315, 3, 3 },
    { WAR_UNIT_TOWNHALL_ORCS,             316, 3, 3 },
    { WAR_UNIT_LUMBERMILL_HUMANS,         317, 3, 3 },
    { WAR_UNIT_LUMBERMILL_ORCS,           318, 3, 3 },
    { WAR_UNIT_STABLES,                   319, 3, 3 },
    { WAR_UNIT_KENNEL,                    320, 3, 3 },
    { WAR_UNIT_BLACKSMITH_HUMANS,         321, 3, 3 },
    { WAR_UNIT_BLACKSMITH_ORCS,           322, 3, 3 },
    { WAR_UNIT_STORMWIND,                 323, 4, 4 },
    { WAR_UNIT_BLACKROCK,                 324, 4, 4 },

    // neutral
    { WAR_UNIT_GOLDMINE,                  325, 3, 3 },

    // corpses
    { WAR_UNIT_HUMAN_CORPSE,              326, 1, 1 },
    { WAR_UNIT_ORC_CORPSE,                326, 1, 1 }
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

typedef struct
{
    WarUnitType type;
    s32 range;
    s32 armour;
    s32 hp;
    s32 minDamage;
    s32 rndDamage;
    s32 buildTime;
    s32 goldCost;
    s32 lumberCost;
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
    // unit type                range  armour   hp    min D.  rnd D.       build    gold    lumber   decay    speed in pixels x seconds
    { WAR_UNIT_FOOTMAN,           1,     2,     60,     1,      9,    __bts(600),    400,      0,     -1,   { 16.736f, 16.736f, 16.736f } },
    { WAR_UNIT_GRUNT,             1,     2,     60,     1,      9,    __bts(600),    400,      0,     -1,   { 16.736f, 16.736f, 16.736f } },
    { WAR_UNIT_PEASANT,           1,     0,     40,     0,      0,    __bts(750),    400,      0,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_PEON,              1,     0,     40,     0,      0,    __bts(750),    400,      0,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_CATAPULT_HUMANS,   8,     0,    120,     0,    255,    __bts(1000),   900,    200,     -1,   {  8.992f,  8.992f,  8.992f } },
    { WAR_UNIT_CATAPULT_ORCS,     8,     0,    120,     0,    255,    __bts(1000),   900,    200,     -1,   {  8.992f,  8.992f,  8.992f } },
    { WAR_UNIT_KNIGHT,            1,     5,     90,     1,     13,    __bts(800),    850,      0,     -1,   { 19.584f, 22.064f, 26.064f } },
    { WAR_UNIT_RAIDER,            1,     5,     90,     1,     13,    __bts(800),    850,      0,     -1,   { 19.584f, 22.064f, 26.064f } },
    { WAR_UNIT_ARCHER,            5,     1,     60,     4,      0,    __bts(700),    450,     50,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_SPEARMAN,          4,     1,     60,     5,      0,    __bts(700),    450,     50,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_CONJURER,          2,     0,     40,     6,      0,    __bts(900),    900,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_WARLOCK,           3,     0,     40,     6,      0,    __bts(900),    900,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_CLERIC,            2,     0,     40,     6,      0,    __bts(800),    700,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_NECROLYTE,         1,     0,     40,     6,      0,    __bts(800),    700,      0,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_MEDIVH,            5,     0,    110,    10,      0,            -1,     -1,     -1,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_LOTHAR,            1,     5,     50,     1,     15,            -1,     -1,     -1,     -1,   { 19.584f, 19.584f, 19.584f } },
    { WAR_UNIT_GRIZELDA,          1,     0,     30,    -1,     -1,            -1,     -1,     -1,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_GARONA,            1,     0,     30,    -1,     -1,            -1,     -1,     -1,     -1,   { 13.984f, 13.984f, 13.984f } },
    { WAR_UNIT_OGRE,              1,     3,     60,     1,     12,            -1,     -1,     -1,     -1,   { 17.936f, 17.936f, 17.936f } },
    { WAR_UNIT_SPIDER,            1,     0,     30,     1,      3,            -1,     -1,     -1,     45,   { 26.064f, 26.064f, 26.064f } },
    { WAR_UNIT_SLIME,             1,    10,    150,     1,      0,            -1,     -1,     -1,     -1,   { 26.064f, 26.064f, 26.064f } },
    { WAR_UNIT_FIREELEMENTAL,     1,     0,    200,     0,     40,            -1,     -1,     -1,     -1,   { 17.936f, 17.936f, 17.936f } },
    { WAR_UNIT_SCORPION,          1,     0,     30,     3,      0,            -1,     -1,     -1,     45,   { 26.064f, 26.064f, 26.064f } },
    { WAR_UNIT_BRIGAND,           1,     1,     40,     1,      9,            -1,     -1,     -1,     -1,   { 16.736f, 16.736f, 16.736f } },
    { WAR_UNIT_SKELETON,          1,     1,     40,     1,      4,            -1,     -1,     -1,     45,   {  8.992f,  8.992f,  8.992f } },
    { WAR_UNIT_DAEMON,            1,     0,    300,     0,     65,            -1,     -1,     -1,     45,   { 17.936f, 17.936f, 17.936f } },
    { WAR_UNIT_WATERELEMENTAL,    3,     0,    250,    40,      0,            -1,     -1,     -1,     45,   { 17.936f, 17.936f, 17.936f } },
};

typedef struct
{
    WarUnitType type;
    s32 armour;
    s32 hp;
    s32 buildTime;
    s32 goldCost;
    s32 lumberCost;
} WarBuildingStats;

const WarBuildingStats buildingStats[] = 
{
    // building type                    armour     hp            build     gold    lumber
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

inline WarUnitsData getUnitsData(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(unitsData);
    while (index < length && unitsData[index].type != type)
        index++;

    assert(index < length);
    return unitsData[index];
}

inline WarWorkerData getWorkerData(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(workersData);
    while (index < length && workersData[index].type != type)
        index++;

    assert(index < length);
    return workersData[index];
}

inline WarBuildingData getBuildingsData(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(buildingsData);
    while (index < length && buildingsData[index].type != type)
        index++;

    assert(index < length);
    return buildingsData[index];
}

inline WarRoadsData getRoadsData(WarRoadPieceType type)
{
    s32 index = 0;
    s32 length = arrayLength(roadsData);
    while (index < length && roadsData[index].type != type)
        index++;

    assert(index < length);
    return roadsData[index];
}

inline WarUnitStats getUnitStats(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(unitStats);
    while (index < length && unitStats[index].type != type)
        index++;

    assert(index < length);
    return unitStats[index];
}

inline WarBuildingStats getBuildingStats(WarUnitType type)
{
    s32 index = 0;
    s32 length = arrayLength(buildingStats);
    while (index < length && buildingStats[index].type != type)
        index++;

    assert(index < length);
    return buildingStats[index];
}

#define isUnit(entity) ((entity)->unit.enabled)

inline bool isDudeUnit(WarEntity* entity)
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
            return true;
    
        default:
            return false;
    }
}

inline bool isBuildingUnit(WarEntity* entity)
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

inline vec2 getUnitSize(WarEntity* entity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    return vec2i(unit->sizex, unit->sizey);
}

inline vec2 getUnitFrameSize(WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    return vec2i(sprite->sprite.frameWidth, sprite->sprite.frameHeight);
}

inline rect getUnitFrameRect(WarEntity* entity)
{
    return rectv(VEC2_ZERO, getUnitFrameSize(entity));
}

inline vec2 getUnitSpriteSize(WarEntity* entity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    return vec2i(unit->sizex * MEGA_TILE_WIDTH, unit->sizey * MEGA_TILE_HEIGHT);
}

inline rect getUnitSpriteRect(WarEntity* entity)
{
    vec2 frameSize = getUnitFrameSize(entity);
    vec2 unitSize = getUnitSpriteSize(entity);
    vec2 pos = vec2Half(vec2Subv(frameSize, unitSize));
    return rectv(pos, unitSize);
}

inline vec2 getUnitSpriteCenter(WarEntity* entity)
{
    vec2 frameSize = getUnitFrameSize(entity);
    vec2 unitSize = getUnitSpriteSize(entity);
    vec2 pos = vec2Half(vec2Subv(frameSize, unitSize));
    return vec2Addv(pos, vec2Half(unitSize));
}

inline vec2 getUnitCenterPosition(WarEntity* entity, bool inTiles)
{
    WarTransformComponent* transform = &entity->transform;
    vec2 spriteSize = getUnitSpriteSize(entity);
    vec2 unitCenter = vec2Half(spriteSize);
    vec2 position = vec2Addv(transform->position, unitCenter);
    return inTiles ? vec2MapToTileCoordinates(position) : position;
}

inline void setUnitCenterPosition(WarEntity* entity, vec2 position)
{
    WarTransformComponent* transform = &entity->transform;
    vec2 spriteSize = getUnitSpriteSize(entity);
    vec2 unitCenter = vec2Half(spriteSize);
    transform->position = vec2Subv(position, unitCenter);
}

inline WarUnitDirection getUnitDirection(WarEntity* entity)
{
    assert(isUnit(entity));

    return entity->unit.direction;
}

internal WarUnitDirection getDirectionFromDiff(f32 x, f32 y)
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

inline void setUnitDirection(WarEntity* entity, WarUnitDirection direction)
{
    assert(isUnit(entity));

    entity->unit.direction = direction;
}

inline void setUnitDirectionFromDiff(WarEntity* entity, f32 dx, f32 dy)
{
    assert(isUnit(entity));

    WarUnitDirection direction = getDirectionFromDiff(dx, dy);
    setUnitDirection(entity, direction);
}

inline f32 getUnitActionScale(WarEntity* entity)
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

vec2 getAttackPointOnTarget(WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));
    assert(isUnit(targetEntity));

    vec2 position = getUnitCenterPosition(entity, true);

    vec2 targetPosition = vec2MapToTileCoordinates(targetEntity->transform.position);
    vec2 unitSize = getUnitSize(targetEntity);
    rect unitRect = rectv(targetPosition, unitSize);
    
    return getClosestPointOnRect(position, unitRect);
}

bool inAttackRange(WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));
    assert(isUnit(targetEntity));

    WarUnitStats stats = getUnitStats(entity->unit.type);

    vec2 position = getUnitCenterPosition(entity, true);
    vec2 pointOnTarget = getAttackPointOnTarget(entity, targetEntity);
    f32 distance = vec2DistanceInTiles(position, pointOnTarget);
    return distance <= stats.range;
}