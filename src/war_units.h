#pragma once

#include <assert.h>
#include "common.h"
#include "war_math.h"
#include "war_log.h"
#include "war.h"
#include "war_color.h"
#include "war_commands.h"

#define directionByIndex(i) ((WarUnitDirection)(WAR_DIRECTION_NORTH + i))

extern const StringView features[];
extern const StringView upgradeNames[];

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

u32 wu_hashUnitType(const WarUnitType type);
bool wu_equalsUnitType(const WarUnitType t1, const WarUnitType t2);

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
