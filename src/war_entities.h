#pragma once

#include "shl/memzone.h"
#include "shl/set.h"

#include "common.h"
#include "war.h"
#include "war_animations.h"
#include "war_audio.h"
#include "war_font.h"
#include "war_state_machine.h"
#include "war_units.h"
#include "war_resources.h"

#define isEntityOfType(entity, entityType) ((entity)->type == (entityType))

struct _WarRoadPiece
{
    WarRoadPieceType type;
    s32 tilex, tiley;
    u8 player;
};

#define WarRoadPieceEmpty (WarRoadPiece){0}
#define createRoadPiece(x, y, player) ((WarRoadPiece){0, (x), (y), (u8)(player)})

bool equalsRoadPiece(const WarRoadPiece r1, const WarRoadPiece r2);

shlDeclareList(WarRoadPieceList, WarRoadPiece)

#define WarRoadPieceListDefaultOptions (WarRoadPieceListOptions){WarRoadPieceEmpty, equalsRoadPiece, NULL}

struct _WarWallPiece
{
    WarWallPieceType type;
    s32 hp;
    s32 maxhp;
    s32 tilex, tiley;
    u8 player;
};

#define WarWallPieceEmpty (WarWallPiece){0}
#define createWallPiece(x, y, player) ((WarWallPiece){0, 0, 0, (x), (y), (u8)(player)})

bool equalsWallPiece(const WarWallPiece w1, const WarWallPiece w2);

shlDeclareList(WarWallPieceList, WarWallPiece)

#define WarWallPieceListDefaultOptions (WarWallPieceListOptions){WarWallPieceEmpty, equalsWallPiece, NULL}

struct _WarRuinPiece
{
    WarRuinPieceType type;
    s32 tilex, tiley;
};

#define WarRuinPieceEmpty (WarRuinPiece){0}
#define createRuinPiece(x, y) ((WarRuinPiece){0, (x), (y)})

bool equalsRuinPiece(const WarRuinPiece r1, const WarRuinPiece r2);

shlDeclareList(WarRuinPieceList, WarRuinPiece)

#define WarRuinPieceListDefaultOptions (WarRuinPieceListOptions){WarRuinPieceEmpty, equalsRuinPiece, NULL}

struct _WarTree
{
    WarTreeTileType type;
    s32 tilex, tiley;
    s32 amount;
};

#define WarTreeEmpty (WarTree){0}
#define createTree(x, y, amount) ((WarTree){0, (x), (y), (amount)})

bool equalsTree(const WarTree t1, const WarTree t2);
s32 compareTreesByPosition(const WarTree t1, const WarTree t2);

shlDeclareList(WarTreeList, WarTree)

#define WarTreeListDefaultOptions (WarTreeListOptions){WarTreeEmpty, equalsTree, NULL}

shlDeclareList(WarEntityIdList, WarEntityId)

#define WarEntityIdListDefaultOptions (WarEntityIdListOptions){0, equalsEntityId, NULL}

shlDeclareSet(WarEntityIdSet, WarEntityId)

#define WarEntityIdSetDefaultOptions (WarEntityIdSetOptions){0, hashEntityId, equalsEntityId, NULL}

enum _WarEntityType
{
    WAR_ENTITY_TYPE_NONE,
    WAR_ENTITY_TYPE_IMAGE,
    WAR_ENTITY_TYPE_UNIT,
    WAR_ENTITY_TYPE_ROAD,
    WAR_ENTITY_TYPE_WALL,
    WAR_ENTITY_TYPE_RUIN,
    WAR_ENTITY_TYPE_FOREST,
    WAR_ENTITY_TYPE_TEXT,
    WAR_ENTITY_TYPE_RECT,
    WAR_ENTITY_TYPE_BUTTON,
    WAR_ENTITY_TYPE_CURSOR,
    WAR_ENTITY_TYPE_AUDIO,
    WAR_ENTITY_TYPE_PROJECTILE,
    WAR_ENTITY_TYPE_RAIN_OF_FIRE,
    WAR_ENTITY_TYPE_POISON_CLOUD,
    WAR_ENTITY_TYPE_SIGHT,
    WAR_ENTITY_TYPE_MINIMAP,
    WAR_ENTITY_TYPE_ANIMATION,

    WAR_ENTITY_TYPE_COUNT
};

enum _WarCursorType
{
    WAR_CURSOR_ARROW = 263,
    WAR_CURSOR_INVALID = 264,
    WAR_CURSOR_YELLOW_CROSSHAIR = 265,
    WAR_CURSOR_RED_CROSSHAIR = 266,
    WAR_CURSOR_YELLOW_CROSSHAIR_2 = 267,
    WAR_CURSOR_MAGNIFYING_GLASS = 268,
    WAR_CURSOR_GREEN_CROSSHAIR = 269,
    WAR_CURSOR_WATCH = 270,
    WAR_CURSOR_ARROW_UP = 271,
    WAR_CURSOR_ARROW_UP_RIGHT = 272,
    WAR_CURSOR_ARROW_RIGHT = 273,
    WAR_CURSOR_ARROW_BOTTOM_RIGHT = 274,
    WAR_CURSOR_ARROW_BOTTOM = 275,
    WAR_CURSOR_ARROW_BOTTOM_LEFT = 276,
    WAR_CURSOR_ARROW_LEFT = 277,
    WAR_CURSOR_ARROW_UP_LEFT = 278
};

//
// Components
//

struct _WarTransformComponent
{
    bool enabled;
    vec2 position;
    vec2 rotation;
    vec2 scale;
};

struct _WarSpriteComponent
{
    bool enabled;
    s32 resourceIndex;
    s32 frameIndex;
    WarSprite sprite;
};

struct _WarUnitComponent
{
    bool enabled;
    WarUnitType type;
    WarUnitDirection direction;

    // position in tiles
    s32 tilex, tiley;
    // size in tiles
    s32 sizex, sizey;

    // index of the player this unit belongs to
    u8 player;

    // the units that can carry resources are
    // peasants, peons, goldmines and trees
    WarResourceKind resourceKind;
    s32 amount;

    // indicate if the unit is building something
    bool building;
    f32 buildPercent;

    // hit points, magic and armor
    s32 maxhp;
    s32 hp;
    s32 maxMana;
    s32 mana;
    s32 armor;
    s32 range;
    s32 minDamage;
    s32 rndDamage;
    s32 decay;
    bool invisible;
    bool invulnerable;
    bool hasBeenSeen;

    // index of the array of speeds of the unit
    s32 speed;

    // the current action index and per-unit mutable action states
    WarUnitActionType actionType;
    WarUnitAction actions[WAR_ACTION_TYPE_COUNT];

    // time remainder (in seconds) until mana is affected
    f32 manaTime;
    // time remainder (in seconds) until the unit invisiblity ceases
    f32 invisibilityTime;
    // time remainder (in seconds) until the unit invulnerability ceases
    f32 invulnerabilityTime;
};

struct _WarAnimationsComponent
{
    bool enabled;
    WarSpriteAnimationList animations;
};

struct _WarRoadComponent
{
    bool enabled;
    WarRoadPieceList pieces;
};

struct _WarWallComponent
{
    bool enabled;
    WarWallPieceList pieces;
};

struct _WarRuinComponent
{
    bool enabled;
    WarRuinPieceList pieces;
};

struct _WarForestComponent
{
    bool enabled;
    WarTreeList trees;
};

struct _WarStateMachineComponent
{
    bool enabled;
    WarState* currentState;
    WarState* nextState;
    bool wst_leaveState;
    bool wst_enterState;
};

struct _WarUIComponent
{
    bool enabled;
    String name;
};

struct _WarTextComponent
{
    bool enabled;
    String text;
    s32 fontIndex;
    f32 fontSize;
    f32 lineHeight;
    WarColor fontColor;
    WarColor highlightColor;
    s32 highlightIndex;
    s32 highlightCount;
    vec2 boundings;
    WarTextAlignment horizontalAlign;
    WarTextAlignment verticalAlign;
    WarTextAlignment lineAlign;
    WarTextWrapping wrapping;
    WarTextTrimming trimming;
    bool multiline;
};

struct _WarRectComponent
{
    bool enabled;
    vec2 size;
    WarColor color;
};

struct _WarButtonComponent
{
    bool enabled;
    bool interactive;
    bool hot;
    bool active;
    WarKeys hotKey;
    s32 highlightIndex;
    s32 highlightCount;
    String tooltip;
    s32 gold;
    s32 wood;
    WarSprite normalSprite;
    WarSprite pressedSprite;
    WarClickHandler clickHandler;
};

struct _WarCursorComponent
{
    bool enabled;
    WarCursorType type;
    vec2 hot;
};

struct _WarProjectileComponent
{
    bool enabled;
    WarProjectileType type;
    WarEntityId sourceEntityId;
    WarEntityId targetEntityId;
    vec2 origin;
    vec2 target;
    s32 speed;
};

struct _WarPoisonCloudComponent
{
    bool enabled;
    vec2 position;
    f32 time; // time in seconds left of the spell
    f32 damageTime; // time in seconds left to inflict damage
    String animName;
};

struct _WarSightComponent
{
    bool enabled;
    vec2 position;
    f32 time; // time in seconds left of the spell
};

typedef void (*WarRenderFunc)(struct _WarContext* context, struct _WarEntity* entity);
typedef s32 (*WarRenderCompareFunc)(const struct _WarEntity* e1, const struct _WarEntity* e2);

struct _WarEntity
{
    bool enabled;
    WarEntityId id;
    WarEntityType type;
    WarTransformComponent transform;
    WarSpriteComponent sprite;
    WarRoadComponent road;
    WarWallComponent wall;
    WarRuinComponent ruin;
    WarForestComponent forest;
    WarUnitComponent unit;
    WarStateMachineComponent stateMachine;
    WarAnimationsComponent animations;
    WarUIComponent ui;
    WarTextComponent text;
    WarRectComponent rect;
    WarButtonComponent button;
    WarAudioComponent audio;
    WarCursorComponent cursor;
    WarProjectileComponent projectile;
    WarPoisonCloudComponent poisonCloud;
    WarSightComponent sight;
};

bool equalsEntity(const WarEntity* e1, const WarEntity* e2);

static inline void freeEntity(WarEntity* e)
{
    wm_free(e);
}

shlDeclareList(WarEntityList, WarEntity*)

#define WarEntityListDefaultOptions (WarEntityListOptions){NULL, equalsEntity, freeEntity}
#define WarEntityListNonFreeOptions (WarEntityListOptions){NULL, equalsEntity}

uint32_t hashEntityType(const WarEntityType type);
bool equalsEntityType(const WarEntityType t1, const WarEntityType t2);

static inline void freeEntityList(WarEntityList* list)
{
    WarEntityListFree(list);
}

shlDeclareMap(WarEntityMap, WarEntityType, WarEntityList*)

uint32_t hashUnitType(const WarUnitType type);
bool equalsUnitType(const WarUnitType t1, const WarUnitType t2);

shlDeclareMap(WarUnitMap, WarUnitType, WarEntityList*)

shlDeclareMap(WarEntityIdMap, WarEntityId, WarEntity*)

struct _WarEntityManager
{
    s32 staticEntityId;

    WarEntityList entities;
    WarEntityMap entitiesByType;
    WarUnitMap unitsByType;
    WarEntityIdMap entitiesById;
    WarEntityList uiEntities;
};

void we_addTransformComponent(WarContext* context, WarEntity* entity, vec2 position);
void we_removeTransformComponent(WarContext* context, WarEntity* entity);

void we_addSpriteComponent(WarContext* context, WarEntity* entity, WarSprite sprite);
void we_addSpriteComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef spriteResourceRef);
void we_removeSpriteComponent(WarContext* context, WarEntity* entity);

void we_addUnitComponent(WarContext* context,
                      WarEntity* entity,
                      WarUnitType type,
                      s32 x,
                      s32 y,
                      u8 player,
                      WarResourceKind resourceKind,
                      u32 amount);
void we_removeUnitComponent(WarContext* context, WarEntity* entity);

void we_addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces);
void we_removeRoadComponent(WarContext* context, WarEntity* entity);

void we_addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces);
void we_removeWallComponent(WarContext* context, WarEntity* entity);

void we_addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces);
void we_removeRuinComponent(WarContext* context, WarEntity* entity);

void we_addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees);
void we_removeForestComponent(WarContext* context, WarEntity* entity);

void we_addStateMachineComponent(WarContext* context, WarEntity* entity);
void we_removeStateMachineComponent(WarContext* context, WarEntity* entity);

void we_addAnimationsComponent(WarContext* context, WarEntity* entity);
void we_removeAnimationsComponent(WarContext* context, WarEntity* entity);

void we_addUIComponent(WarContext* context, WarEntity* entity, String name);
void we_removeUIComponent(WarContext* context, WarEntity* entity);

void we_addTextComponent(WarContext* context, WarEntity* entity, s32 fontIndex, f32 fontSize, String text);
void we_removeTextComponent(WarContext* context, WarEntity* entity);

void we_addRectComponent(WarContext* context, WarEntity* entity, vec2 size, WarColor color);
void we_removeRectComponent(WarContext* context, WarEntity* entity);

void we_addButtonComponent(WarContext* context, WarEntity* entity, WarSprite normalSprite, WarSprite pressedSprite);
void we_addButtonComponentFromResource(WarContext* context,
                                    WarEntity* entity,
                                    WarSpriteResourceRef normalRef,
                                    WarSpriteResourceRef pressedRef);
void we_removeButtonComponent(WarContext* context, WarEntity* entity);

void we_addAudioComponent(WarContext* context, WarEntity* entity, WarAudioType type, s32 resourceIndex, bool loop);
void we_removeAudioComponent(WarContext* context, WarEntity* entity);

void we_addCursorComponent(WarContext* context, WarEntity* entity, WarCursorType type, vec2 hot);
void we_removeCursorComponent(WarContext* context, WarEntity* entity);

void we_addProjectileComponent(WarContext* context, WarEntity* entity, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target, s32 speed);
void we_removeProjectileComponent(WarContext* context, WarEntity* entity);

void we_addPoisonCloudComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time);
void we_removePoisonCloudComponent(WarContext* context, WarEntity* entity);

void we_addSightComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time);
void we_removeSightComponent(WarContext* context, WarEntity* entity);

// Roads
WarEntity* we_createRoad(WarContext* context);
void we_addRoadPiece(WarEntity* entity, s32 x, s32 y, s32 player);
void we_addRoadPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct);
bool we_hasRoadPieceAtPosition(WarEntity* entity, s32 x, s32 y);
WarRoadPiece* we_getRoadPieceAtPosition(WarEntity* entity, s32 x, s32 y);
void we_removeRoadPiece(WarEntity* entity, WarRoadPiece* piece);
void we_determineRoadTypes(WarContext* context, WarEntity* entity);

// Walls
WarEntity* we_createWall(WarContext* context);
WarWallPiece* we_addWallPiece(WarEntity* entity, s32 x, s32 y, s32 player);
void we_addWallPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct);
bool we_hasWallPieceAtPosition(WarEntity* entity, s32 x, s32 y);
WarWallPiece* we_getWallPieceAtPosition(WarEntity* entity, s32 x, s32 y);
void we_removeWallPiece(WarEntity* entity, WarWallPiece* piece);
void we_determineWallTypes(WarContext* context, WarEntity* entity);
void we_takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage);

// Ruins
WarEntity* we_createRuins(WarContext* context);
void we_addRuinsPieces(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 dim);
bool we_hasRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y);
WarRuinPiece* we_getRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y);
void we_determineRuinTypes(WarContext* context, WarEntity* entity);

// Trees
bool we_hasTreeAtPosition(WarEntity* forest, s32 x, s32 y);
WarTree* we_getTreeAtPosition(WarEntity* forest, s32 x, s32 y);
void we_determineTreeTiles(WarContext* context, WarEntity* forest);
void we_determineAllTreeTiles(WarContext* context);
WarTree* we_findAccesibleTree(WarContext* context, WarEntity* forest, vec2 position);
void we_plantTree(WarContext* context, WarEntity* forest, s32 x, s32 y);
bool we_validTree(WarContext* context, WarEntity* forest, WarTree* tree);
s32 we_chopTree(WarContext* context, WarEntity* forest, WarTree* tree, s32 amount);

// Entities
WarEntity* we_createEntity(WarContext* context, WarEntityType type, bool addToMap);
WarEntity* we_createUnit(WarContext* context, WarUnitType type,
                      s32 x, s32 y, u8 player,
                      WarResourceKind resourceKind, u32 amount,
                      bool addToMap);
WarEntity* we_createDude(WarContext* context, WarUnitType type, s32 x, s32 y, u8 player, bool isGoingToTrain);
WarEntity* we_createBuilding(WarContext* context, WarUnitType type, s32 x, s32 y, u8 player, bool isGoingToBuild);
WarEntity* we_findEntity(WarContext* context, WarEntityId id);
WarEntity* we_findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type);
WarEntity* we_findUIEntity(WarContext* context, StringView name);
WarEntity* we_findEntityUnderCursor(WarContext* context, bool includeTrees, bool includeWalls);
void we_removeEntity(WarContext* context, WarEntity* entity);
void we_removeEntityById(WarContext* context, WarEntityId id);
void we_renderEntity(WarContext* context, WarEntity* entity);
void we_renderEntitiesOfType(WarContext* context, WarEntityType type);
void we_renderUnitSelection(WarContext* context);
bool we_isStaticEntity(WarEntity* entity);

void we_initEntityManager(WarContext* context, WarEntityManager* manager);
WarEntityManager* we_getEntityManager(WarContext* context);
WarEntityList* we_getEntities(WarContext* context);
WarEntityList* we_getEntitiesOfType(WarContext* context, WarEntityType type);
WarEntityList* we_getUnitsOfType(WarContext* context, WarUnitType type);
WarEntityList* we_getUIEntities(WarContext* context);

void we_increaseUpgradeLevel(WarContext* context, WarPlayerInfo* player, WarUpgradeType upgrade);
bool we_enoughPlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
bool we_decreasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
void we_increasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
bool we_increaseUnitHp(WarContext* context, WarEntity* entity, s32 hp);
bool we_decreaseUnitHp(WarContext* context, WarEntity* entity, s32 hp);
bool we_decreaseUnitMana(WarContext* context, WarEntity* entity, s32 mana);
void we_increaseUnitMana(WarContext* context, WarEntity* entity, s32 mana);
bool we_enoughFarmFood(WarContext* context, WarPlayerInfo* player);
bool we_checkFarmFood(WarContext* context, WarPlayerInfo* player);
bool we_checkRectToBuild(WarContext* context, s32 x, s32 y, s32 w, s32 h);
bool we_checkTileToBuild(WarContext* context, WarUnitType buildingToBuild, s32 x, s32 y);
bool we_checkTileToBuildRoadOrWall(WarContext* context, s32 x, s32 y);
WarEntityList* we_getNearUnits(WarContext* context, vec2 tilePosition, s32 distance);
WarEntity* we_getNearEnemy(WarContext* context, WarEntity* entity);
bool we_isBeingAttackedBy(WarEntity* entity, WarEntity* other);
bool we_isBeingAttacked(WarContext* context, WarEntity* entity);
WarEntity* we_getAttacker(WarContext* context, WarEntity* entity);
WarEntity* we_getAttackTarget(WarContext* context, WarEntity* entity);
s32 we_getTotalDamage(s32 minDamage, s32 rndDamage, s32 armor);
void we_takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage);
void we_rangeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity);
void we_rangeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece);
void we_meleeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity);
void we_meleeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece);
s32 mine(WarContext* context, WarEntity* goldmine, s32 amount);
