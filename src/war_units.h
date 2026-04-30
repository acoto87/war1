#pragma once

#include <assert.h>
#include "common.h"
#include "war_math.h"
#include "war_log.h"
#include "war.h"
#include "war_color.h"

enum _WarRace
{
    WAR_RACE_NEUTRAL,
    WAR_RACE_HUMANS,
    WAR_RACE_ORCS
};

enum _WarUnitDirection
{
    WAR_DIRECTION_NORTH,
    WAR_DIRECTION_NORTH_EAST,
    WAR_DIRECTION_EAST,
    WAR_DIRECTION_SOUTH_EAST,
    WAR_DIRECTION_SOUTH,
    WAR_DIRECTION_SOUTH_WEST,
    WAR_DIRECTION_WEST,
    WAR_DIRECTION_NORTH_WEST,

    WAR_DIRECTION_COUNT
};

#define directionByIndex(i) ((WarUnitDirection)(WAR_DIRECTION_NORTH + i))

enum _WarUnitType
{
    // units
    WAR_UNIT_FOOTMAN,
    WAR_UNIT_GRUNT,
    WAR_UNIT_PEASANT,
    WAR_UNIT_PEON,
    WAR_UNIT_CATAPULT_HUMANS,
    WAR_UNIT_CATAPULT_ORCS,
    WAR_UNIT_KNIGHT,
    WAR_UNIT_RAIDER,
    WAR_UNIT_ARCHER,
    WAR_UNIT_SPEARMAN,
    WAR_UNIT_CONJURER,
    WAR_UNIT_WARLOCK,
    WAR_UNIT_CLERIC,
    WAR_UNIT_NECROLYTE,
    WAR_UNIT_MEDIVH,
    WAR_UNIT_LOTHAR,
    WAR_UNIT_WOUNDED,
    WAR_UNIT_GRIZELDA,
    WAR_UNIT_GARONA,
    WAR_UNIT_OGRE,
    WAR_UNIT_SPIDER,
    WAR_UNIT_SLIME,
    WAR_UNIT_FIRE_ELEMENTAL,
    WAR_UNIT_SCORPION,
    WAR_UNIT_BRIGAND,
    WAR_UNIT_THE_DEAD,
    WAR_UNIT_SKELETON,
    WAR_UNIT_DAEMON,
    WAR_UNIT_WATER_ELEMENTAL,
    WAR_UNIT_DRAGON_CYCLOPS_GIANT,
    WAR_UNIT_26,
    WAR_UNIT_30,

    // buildings
    WAR_UNIT_FARM_HUMANS,
    WAR_UNIT_FARM_ORCS,
    WAR_UNIT_BARRACKS_HUMANS,
    WAR_UNIT_BARRACKS_ORCS,
    WAR_UNIT_CHURCH,
    WAR_UNIT_TEMPLE,
    WAR_UNIT_TOWER_HUMANS,
    WAR_UNIT_TOWER_ORCS,
    WAR_UNIT_TOWNHALL_HUMANS,
    WAR_UNIT_TOWNHALL_ORCS,
    WAR_UNIT_LUMBERMILL_HUMANS,
    WAR_UNIT_LUMBERMILL_ORCS,
    WAR_UNIT_STABLE,
    WAR_UNIT_KENNEL,
    WAR_UNIT_BLACKSMITH_HUMANS,
    WAR_UNIT_BLACKSMITH_ORCS,
    WAR_UNIT_STORMWIND,
    WAR_UNIT_BLACKROCK,

    // neutral
    WAR_UNIT_GOLDMINE,

	WAR_UNIT_51,

    // others
    WAR_UNIT_HUMAN_CORPSE,
    WAR_UNIT_ORC_CORPSE,

    WAR_UNIT_COUNT
};

enum _WarFeatureType
{
    WAR_FEATURE_UNIT_FOOTMAN,
    WAR_FEATURE_UNIT_GRUNT,
    WAR_FEATURE_UNIT_PEASANT,
    WAR_FEATURE_UNIT_PEON,
    WAR_FEATURE_UNIT_CATAPULT_HUMANS,
    WAR_FEATURE_UNIT_CATAPULT_ORCS,
    WAR_FEATURE_UNIT_KNIGHT,
    WAR_FEATURE_UNIT_RAIDER,
    WAR_FEATURE_UNIT_ARCHER,
    WAR_FEATURE_UNIT_SPEARMAN,
    WAR_FEATURE_UNIT_CONJURER,
    WAR_FEATURE_UNIT_WARLOCK,
    WAR_FEATURE_UNIT_CLERIC,
    WAR_FEATURE_UNIT_NECROLYTE,
    WAR_FEATURE_UNIT_FARM_HUMANS,
    WAR_FEATURE_UNIT_FARM_ORCS,
    WAR_FEATURE_UNIT_BARRACKS_HUMANS,
    WAR_FEATURE_UNIT_BARRACKS_ORCS,
    WAR_FEATURE_UNIT_CHURCH,
    WAR_FEATURE_UNIT_TEMPLE,
    WAR_FEATURE_UNIT_TOWER_HUMANS,
    WAR_FEATURE_UNIT_TOWER_ORCS,
    WAR_FEATURE_UNIT_TOWN_HALL_HUMANS,
    WAR_FEATURE_UNIT_TOWN_HALL_ORCS,
    WAR_FEATURE_UNIT_LUMBER_MILL_HUMANS,
    WAR_FEATURE_UNIT_LUMBER_MILL_ORCS,
    WAR_FEATURE_UNIT_STABLE,
    WAR_FEATURE_UNIT_KENNEL,
    WAR_FEATURE_UNIT_BLACKSMITH_HUMANS,
    WAR_FEATURE_UNIT_BLACKSMITH_ORCS,
    WAR_FEATURE_SPELL_HEALING,
    WAR_FEATURE_SPELL_RAISE_DEAD,
    WAR_FEATURE_SPELL_FAR_SIGHT,
    WAR_FEATURE_SPELL_DARK_VISION,
    WAR_FEATURE_SPELL_INVISIBILITY,
    WAR_FEATURE_SPELL_UNHOLY_ARMOR,
    WAR_FEATURE_SPELL_SCORPION,
    WAR_FEATURE_SPELL_SPIDER,
    WAR_FEATURE_SPELL_RAIN_OF_FIRE,
    WAR_FEATURE_SPELL_POISON_CLOUD,
    WAR_FEATURE_SPELL_WATER_ELEMENTAL,
    WAR_FEATURE_SPELL_DAEMON,
    WAR_FEATURE_UNIT_ROAD,
    WAR_FEATURE_UNIT_WALL,
};

enum _WarUpgradeType
{
    WAR_UPGRADE_ARROWS,
    WAR_UPGRADE_SPEARS,
    WAR_UPGRADE_SWORDS,
    WAR_UPGRADE_AXES,
    WAR_UPGRADE_HORSES,
    WAR_UPGRADE_WOLVES,
    WAR_UPGRADE_SCORPIONS,
    WAR_UPGRADE_SPIDERS,
    WAR_UPGRADE_RAIN_OF_FIRE,
    WAR_UPGRADE_POISON_CLOUD,
    WAR_UPGRADE_WATER_ELEMENTAL,
    WAR_UPGRADE_DAEMON,
    WAR_UPGRADE_HEALING,
    WAR_UPGRADE_RAISE_DEAD,
    WAR_UPGRADE_FAR_SIGHT,
    WAR_UPGRADE_DARK_VISION,
    WAR_UPGRADE_INVISIBILITY,
    WAR_UPGRADE_UNHOLY_ARMOR,
    WAR_UPGRADE_SHIELD,
};

enum _WarSpellType
{
    // spells
    WAR_SPELL_HEALING,
    WAR_SPELL_FAR_SIGHT,
    WAR_SPELL_INVISIBILITY,
    WAR_SPELL_RAIN_OF_FIRE,
    WAR_SPELL_POISON_CLOUD,
    WAR_SPELL_RAISE_DEAD,
    WAR_SPELL_DARK_VISION,
    WAR_SPELL_UNHOLY_ARMOR,

    // summons
    WAR_SUMMON_SPIDER,
    WAR_SUMMON_SCORPION,
    WAR_SUMMON_DAEMON,
    WAR_SUMMON_WATER_ELEMENTAL,
};

#include "war_commands.h"

enum _WarResourceKind
{
    WAR_RESOURCE_NONE,
    WAR_RESOURCE_GOLD,
    WAR_RESOURCE_WOOD
};

enum _WarRoadPieceType
{
    WAR_ROAD_PIECE_LEFT,
    WAR_ROAD_PIECE_TOP,
    WAR_ROAD_PIECE_RIGHT,
    WAR_ROAD_PIECE_BOTTOM,
    WAR_ROAD_PIECE_BOTTOM_LEFT,
    WAR_ROAD_PIECE_VERTICAL,
    WAR_ROAD_PIECE_BOTTOM_RIGHT,
    WAR_ROAD_PIECE_T_LEFT,
    WAR_ROAD_PIECE_T_BOTTOM,
    WAR_ROAD_PIECE_T_RIGHT,
    WAR_ROAD_PIECE_CROSS,
    WAR_ROAD_PIECE_TOP_LEFT,
    WAR_ROAD_PIECE_HORIZONTAL,
    WAR_ROAD_PIECE_T_TOP,
    WAR_ROAD_PIECE_TOP_RIGHT,
};

enum _WarWallPieceType
{
    WAR_WALL_PIECE_LEFT,
    WAR_WALL_PIECE_TOP,
    WAR_WALL_PIECE_RIGHT,
    WAR_WALL_PIECE_BOTTOM,
    WAR_WALL_PIECE_BOTTOM_LEFT,
    WAR_WALL_PIECE_VERTICAL,
    WAR_WALL_PIECE_BOTTOM_RIGHT,
    WAR_WALL_PIECE_T_LEFT,
    WAR_WALL_PIECE_T_BOTTOM,
    WAR_WALL_PIECE_T_RIGHT,
    WAR_WALL_PIECE_CROSS,
    WAR_WALL_PIECE_TOP_LEFT,
    WAR_WALL_PIECE_HORIZONTAL,
    WAR_WALL_PIECE_T_TOP,
    WAR_WALL_PIECE_TOP_RIGHT,
};

enum _WarRuinPieceType
{
    WAR_RUIN_PIECE_NONE,
    WAR_RUIN_PIECE_TOP_LEFT,
    WAR_RUIN_PIECE_TOP,
    WAR_RUIN_PIECE_TOP_RIGHT,
    WAR_RUIN_PIECE_LEFT,
    WAR_RUIN_PIECE_CENTER,
    WAR_RUIN_PIECE_RIGHT,
    WAR_RUIN_PIECE_BOTTOM_LEFT,
    WAR_RUIN_PIECE_BOTTOM,
    WAR_RUIN_PIECE_BOTTOM_RIGHT,
    WAR_RUIN_PIECE_TOP_LEFT_INSIDE,
    WAR_RUIN_PIECE_TOP_RIGHT_INSIDE,
    WAR_RUIN_PIECE_BOTTOM_LEFT_INSIDE,
    WAR_RUIN_PIECE_BOTTOM_RIGHT_INSIDE,
    WAR_RUIN_PIECE_DIAG_1,
    WAR_RUIN_PIECE_DIAG_2,
};

enum _WarTreeTileType
{
    WAR_TREE_NONE,
    WAR_TREE_TOP_LEFT,
    WAR_TREE_TOP,
    WAR_TREE_TOP_RIGHT,
    WAR_TREE_LEFT,
    WAR_TREE_CENTER,
    WAR_TREE_RIGHT,
    WAR_TREE_BOTTOM_LEFT,
    WAR_TREE_BOTTOM,
    WAR_TREE_BOTTOM_RIGHT,
    WAR_TREE_TOP_LEFT_INSIDE,
    WAR_TREE_TOP_RIGHT_INSIDE,
    WAR_TREE_BOTTOM_LEFT_INSIDE,
    WAR_TREE_BOTTOM_RIGHT_INSIDE,
    WAR_TREE_TOP_END,
    WAR_TREE_BOTTOM_END,
    WAR_TREE_VERTICAL,
    WAR_TREE_DIAG_1,
    WAR_TREE_DIAG_2,
    WAR_TREE_CHOPPED,
};

enum _WarFogPieceType
{
    WAR_FOG_PIECE_NONE,
    WAR_FOG_PIECE_TOP_LEFT,
    WAR_FOG_PIECE_TOP,
    WAR_FOG_PIECE_TOP_RIGHT,
    WAR_FOG_PIECE_LEFT,
    WAR_FOG_PIECE_CENTER,
    WAR_FOG_PIECE_RIGHT,
    WAR_FOG_PIECE_BOTTOM_LEFT,
    WAR_FOG_PIECE_BOTTOM,
    WAR_FOG_PIECE_BOTTOM_RIGHT
};

enum _WarFogBoundaryType
{
    WAR_FOG_BOUNDARY_NONE,
    WAR_FOG_BOUNDARY_UNKOWN,
    WAR_FOG_BOUNDARY_FOG,
};

extern const StringView features[];

extern const StringView upgradeNames[];

enum _WarUnitPortraits
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
};

struct _WarUnitData
{
    WarUnitType type;
    s32 resourceIndex;
    s32 portraitFrameIndex;
    s32 sizex;
    s32 sizey;
    StringView name;
};

extern const WarUnitData unitsData[];

struct _WarWorkerData
{
    WarUnitType type;
    s32 carryingWoodResource;
    s32 carryingGoldResource;
};

extern const WarWorkerData workersData[];

struct _WarBuildingData
{
    WarUnitType type;
    s32 buildingResource;
};

extern const WarBuildingData buildingsData[];

struct _WarRoadData
{
    WarRoadPieceType type;
    s32 tileIndexForest;
    s32 tileIndexSwamp;
};

extern const WarRoadData roadsData[];

// this is a map of the configurations of the neighbors of a tile to a road tile type.
// each index represent one configuration of the 16 possibles and the value at that index
// is the tile the tree should get based on that configuration.
extern const WarRoadPieceType roadTileTypeMap[16];

struct _WarWallData
{
    WarWallPieceType type;
    s32 tileForest;
    s32 tileDamagedForest;
    s32 tileDestroyedForest;
    s32 tileSwamp;
    s32 tileDamagedSwamp;
    s32 tileDestroyedSwamp;
};

extern const WarWallData wallsData[];

// this is a map of the configurations of the neighbors of a tile to a wall tile type.
// each index represent one configuration of the 16 possibles and the value at that index
// is the tile the tree should get based on that configuration.
extern const WarWallPieceType wallTileTypeMap[16];

#define __bts(t) ((t)*80/1000)

#define WAR_WALL_GOLD_COST 100
#define WAR_WALL_WOOD_COST 0
#define WAR_WALL_BUILD_TIME __bts(200)
#define WAR_WALL_MAX_HP 60

#define WAR_ROAD_GOLD_COST 50
#define WAR_ROAD_WOOD_COST 0

#define GOD_MODE_MIN_DAMAGE 255

struct _WarRuinData
{
    WarRuinPieceType type;
    s32 tileIndexForest;
    s32 tileIndexSwamp;
};

extern const WarRuinData ruinsData[];

// this is a map of the configurations of the neighbors of a tile to a ruin tile type.
// each index represent one configuration of the 256 possibles and the value at that index
// is the tile the tree should get based on that configuration.
extern const WarRuinPieceType ruinTileTypeMap[256];

struct _WarTreeData
{
    WarTreeTileType type;
    s32 tileIndexForest;
    s32 tileIndexSwamp;
};

extern const WarTreeData treesData[];

// this is a map of the configurations of the neighbors of a tile to a tree tile type.
// each index represent one configuration of the 256 possibles and the value at that index
// is the tile the tree should get based on that configuration.
extern const WarTreeTileType treeTileTypeMap[256];

// this is a map of the configurations of the neighbors of a tile to a ruin tile type.
// each index represent one configuration of the 256 possibles and the value at that index
// is the tile the tree should get based on that configuration.
extern const WarFogPieceType fogTileTypeMap[256];

struct _WarUnitStats
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
};

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

extern const WarUnitStats unitStats[];

struct _WarBuildingStats
{
    WarUnitType type;
    s32 armor;
    s32 sight;
    s32 hp;
    s32 buildTime;
    s32 goldCost;
    s32 woodCost;
};

extern const WarBuildingStats buildingStats[];

struct _WarUpgradeData
{
    WarUpgradeType type;
    s32 maxLevelAllowed;
    s32 frameIndices[2];
};

extern const WarUpgradeData upgradesData[];

struct _WarUpgradeStats
{
    WarUpgradeType type;
    s32 buildTime;
    s32 goldCost[2];
};

extern const WarUpgradeStats upgradeStats[];

struct _WarSpellData
{
    WarSpellType type;
    s32 portraitFrameIndex;
};

extern const WarSpellData spellData[];

struct _WarSpellStats
{
    WarSpellType type;
    s32 manaCost;
    f32 time;
    s32 range;
};

extern const WarSpellStats spellStats[];

struct _WarSpellMapping
{
    WarSpellType type;
    s32 mappedType;
};

extern const WarSpellMapping spellMappings[];

struct _WarUnitCommandBaseData
{
    WarUnitCommandType type;
    WarClickHandler clickHandler;
    WarKeys hotKey;
    s32 highlightIndex;
    s32 highlightCount;
    StringView tooltip;
    StringView tooltip2;
};

extern const WarUnitCommandBaseData commandsBaseData[];

struct _WarUnitCommandMapping
{
    WarUnitCommandType type;
    s32 mappedType;
};

extern const WarUnitCommandMapping commandMappings[];

struct _WarUnitCommandData
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
};

WarUnitData wu_getUnitData(WarUnitType type);
WarWorkerData wu_getWorkerData(WarUnitType type);
WarBuildingData wu_getBuildingData(WarUnitType type);
WarRoadData wu_getRoadData(WarRoadPieceType type);
WarWallData wu_getWallData(WarWallPieceType type);
WarRuinData wu_getRuinData(WarRuinPieceType type);
WarTreeData wu_getTreeData(WarTreeTileType type);
WarUpgradeData wu_getUpgradeData(WarUpgradeType type);
WarSpellData wu_getSpellData(WarSpellType type);
WarUnitStats wu_getUnitStats(WarUnitType type);
WarBuildingStats wu_getBuildingStats(WarUnitType type);
WarUpgradeStats wu_getUpgradeStats(WarUpgradeType type);
WarSpellStats wu_getSpellStats(WarSpellType type);
WarSpellMapping wu_getSpellMapping(WarSpellType type);
WarUnitCommandBaseData wu_getCommandBaseData(WarUnitCommandType type);
WarUnitCommandMapping wu_getCommandMapping(WarUnitCommandType type);
WarUnitCommandMapping wu_getCommandMappingFromUnitType(WarUnitType unitType);
WarUnitCommandMapping wu_getCommandMappingFromUpgradeType(WarUpgradeType upgradeType);
WarUnitCommandMapping wu_getCommandMappingFromSpellType(WarSpellType spellType);

#define isUnit(entity) ((entity)->type == WAR_ENTITY_TYPE_UNIT)
#define isUnitOfType(entity, unitType) (isUnit(entity) && (entity)->unit.type == (unitType))
#define isRoad(entity) ((entity)->type == WAR_ENTITY_TYPE_ROAD)
#define isWall(entity) ((entity)->type == WAR_ENTITY_TYPE_WALL)
#define isRuin(entity) ((entity)->type == WAR_ENTITY_TYPE_RUIN)

bool wu_isDudeUnitType(WarUnitType type);
bool wu_isBuildingUnitType(WarUnitType type);
bool wu_isWorkerUnitType(WarUnitType type);
bool wu_isWarriorUnitType(WarUnitType type);
bool wu_isRangeUnitType(WarUnitType type);
bool wu_isMeleeUnitType(WarUnitType type);
bool wu_isFistUnitType(WarUnitType type);
bool wu_isSwordUnitType(WarUnitType type);
bool wu_isMagicUnitType(WarUnitType type);
bool wu_isCorpseUnitType(WarUnitType type);
bool wu_isCatapultUnitType(WarUnitType type);
bool wu_isConjurerOrWarlockUnitType(WarUnitType type);
bool wu_isClericOrNecrolyteUnitType(WarUnitType type);
bool wu_isSummonUnitType(WarUnitType type);

// Entity-level unit-type queries (bodies defined in war_entities.h after WarEntity is complete)
bool wu_isDudeUnit(WarEntity* entity);
bool wu_isBuildingUnit(WarEntity* entity);
bool wu_isWorkerUnit(WarEntity* entity);
bool wu_isWarriorUnit(WarEntity* entity);
bool wu_isRangeUnit(WarEntity* entity);
bool wu_isMeleeUnit(WarEntity* entity);
bool wu_isFistUnit(WarEntity* entity);
bool wu_isSwordUnit(WarEntity* entity);
bool wu_isMagicUnit(WarEntity* entity);
bool wu_isCorpseUnit(WarEntity* entity);
bool wu_isCatapultUnit(WarEntity* entity);
bool wu_isConjurerOrWarlockUnit(WarEntity* entity);
bool wu_isClericOrNecrolyteUnit(WarEntity* entity);
bool wu_isSummonUnit(WarEntity* entity);
bool wu_isSkeletonUnit(WarEntity* entity);

WarRace wu_getUnitTypeRace(WarUnitType type);

WarRace wu_getUnitRace(WarEntity* entity);

#define isHumanUnit(entity) (wu_getUnitRace(entity) == WAR_RACE_HUMANS)
#define isOrcUnit(entity) (wu_getUnitRace(entity) == WAR_RACE_ORCS)
#define isNeutralUnit(entity) (wu_getUnitRace(entity) == WAR_RACE_NEUTRAL)

WarUnitType wu_getUnitTypeForRace(WarUnitType type, WarRace race);

enum _WarProjectileType
{
    WAR_PROJECTILE_ARROW,
    WAR_PROJECTILE_CATAPULT,
    WAR_PROJECTILE_FIREBALL,
    WAR_PROJECTILE_FIREBALL_2,
    WAR_PROJECTILE_WATER_ELEMENTAL,
    WAR_PROJECTILE_RAIN_OF_FIRE
};

WarProjectileType wu_getProjectileType(WarUnitType type);

bool wu_isFriendlyUnit(WarContext* context, WarEntity* entity);
bool wu_isEnemyUnit(WarContext* context, WarEntity* entity);
bool wu_areEnemies(WarContext* context, WarEntity* entity, WarEntity* other);
bool wu_canAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity);

WarUnitType wu_getTownHallOfRace(WarRace race);

WarUnitType wu_getProducerUnitOfType(WarUnitType type);

// Entity geometry / property functions (bodies defined in war_entities.h)
vec2 wu_getUnitSize(WarEntity* entity);
vec2 wu_getUnitFrameSize(WarEntity* entity);
rect wu_getUnitFrameRect(WarEntity* entity);
vec2 wu_getUnitSpriteSize(WarEntity* entity);
rect wu_getUnitSpriteRect(WarEntity* entity);
vec2 wu_getUnitSpriteCenter(WarEntity* entity);
rect wu_getUnitRect(WarEntity* entity);
vec2 wu_getUnitPosition(WarEntity* entity, bool inTiles);
vec2 wu_getUnitCenterPosition(WarEntity* entity, bool inTiles);
void wu_setUnitPosition(WarEntity* entity, vec2 position, bool inTiles);
void wu_setUnitCenterPosition(WarEntity* entity, vec2 position, bool inTiles);
WarUnitDirection wu_getUnitDirection(WarEntity* entity);

WarUnitDirection wu_getDirectionFromDiff(f32 x, f32 y);

void wu_setUnitDirection(WarEntity* entity, WarUnitDirection direction);
void wu_setUnitDirectionFromDiff(WarEntity* entity, f32 dx, f32 dy);
f32 wu_getUnitActionScale(WarEntity* entity);
vec2 wu_unitPointOnTarget(WarEntity* entity, WarEntity* targetEntity);
s32 wu_entityTileDistance(WarEntity* entity, vec2 targetPosition);
bool wu_tileInRange(WarEntity* entity, vec2 targetTile, s32 range);
s32 wu_unitDistanceInTiles(WarEntity* entity, WarEntity* targetEntity);
bool wu_unitInRange(WarEntity* entity, WarEntity* targetEntity, s32 range);
bool wu_isCarryingResources(WarEntity* entity);
s32 wu_getUnitSightRange(WarEntity* entity);

bool wu_displayUnitOnMinimap(WarEntity* entity);
WarColor wu_getUnitColorOnMinimap(WarEntity* entity);

s32 wu_getTotalNumberOfDudes(WarContext* context, u8 player);
s32 wu_getTotalNumberOfBuildings(WarContext* context, u8 player, bool alreadyBuilt);
s32 wu_getNumberOfBuildingsOfType(WarContext* context, u8 player, WarUnitType unitType, bool alreadyBuilt);
s32 wu_getNumberOfUnitsOfType(WarContext* context, u8 player, WarUnitType unitType);
s32 wu_getTotalNumberOfUnits(WarContext* context, u8 player);

#define playerHasUnit(context, player, unitType) (wu_getNumberOfUnitsOfType(context, player, unitType) > 0)
#define playerHasBuilding(context, player, unitType) (wu_getNumberOfBuildingsOfType(context, player, unitType, true) > 0)

#define isValidUnitType(type) inRange(type, WAR_UNIT_FOOTMAN, WAR_UNIT_COUNT)

void wu_getUnitCommands(WarContext* context, WarEntity* entity, WarUnitCommandType commands[]);
WarUnitCommandData wu_getUnitCommandData(WarContext* context, WarEntity* entity, WarUnitCommandType commandType);
