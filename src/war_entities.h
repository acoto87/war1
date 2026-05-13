#pragma once

#include "shl/memzone.h"
#include "shl/list.h"
#include "shl/set.h"
#include "shl/map.h"

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

struct _WarWallPiece
{
    WarWallPieceType type;
    s32 hp;
    s32 maxhp;
    s32 tilex, tiley;
    u8 player;
};

struct _WarRuinPiece
{
    WarRuinPieceType type;
    s32 tilex, tiley;
};

struct _WarTree
{
    WarTreeTileType type;
    s32 tilex, tiley;
    s32 amount;
};

#define WarRoadPieceEmpty (WarRoadPiece){0}
#define WarWallPieceEmpty (WarWallPiece){0}
#define WarRuinPieceEmpty (WarRuinPiece){0}
#define WarTreeEmpty (WarTree){0}

#define createRoadPiece(x, y, player) ((WarRoadPiece){0, (x), (y), (u8)(player)})
#define createWallPiece(x, y, player) ((WarWallPiece){0, 0, 0, (x), (y), (u8)(player)})
#define createRuinPiece(x, y) ((WarRuinPiece){0, (x), (y)})
#define createTree(x, y, amount) ((WarTree){0, (x), (y), (amount)})

shlDeclareList(WarRoadPieceList, WarRoadPiece)
shlDeclareList(WarWallPieceList, WarWallPiece)
shlDeclareList(WarRuinPieceList, WarRuinPiece)
shlDeclareList(WarTreeList, WarTree)
shlDeclareList(WarEntityIdList, WarEntityId)
shlDeclareSet(WarEntityIdSet, WarEntityId)
shlDeclareList(WarEntityList, WarEntity*)
shlDeclareMap(WarEntityMap, WarEntityType, WarEntityList*)
shlDeclareMap(WarUnitMap, WarUnitType, WarEntityList*)
shlDeclareMap(WarEntityIdMap, WarEntityId, WarEntity*)

bool we_equalsRoadPiece(const WarRoadPiece r1, const WarRoadPiece r2);
bool we_equalsWallPiece(const WarWallPiece w1, const WarWallPiece w2);
bool we_equalsRuinPiece(const WarRuinPiece r1, const WarRuinPiece r2);
bool we_equalsTree(const WarTree t1, const WarTree t2);
bool we_equalsEntity(const WarEntity* e1, const WarEntity* e2);
bool we_equalsEntityType(const WarEntityType t1, const WarEntityType t2);
bool we_equalsEntityId(const WarEntityId id1, const WarEntityId id2);

u32 we_hashEntityType(const WarEntityType type);
u32 we_hashEntityId(const WarEntityId id);

void we_freeEntity(WarEntity* e);
void we_freeEntityList(WarEntityList* list);

#define WarRoadPieceListDefaultOptions (WarRoadPieceListOptions){WarRoadPieceEmpty, we_equalsRoadPiece, NULL}
#define WarWallPieceListDefaultOptions (WarWallPieceListOptions){WarWallPieceEmpty, we_equalsWallPiece, NULL}
#define WarRuinPieceListDefaultOptions (WarRuinPieceListOptions){WarRuinPieceEmpty, we_equalsRuinPiece, NULL}
#define WarTreeListDefaultOptions (WarTreeListOptions){WarTreeEmpty, we_equalsTree, NULL}
#define WarEntityIdListDefaultOptions (WarEntityIdListOptions){0, we_equalsEntityId, NULL}
#define WarEntityIdSetDefaultOptions (WarEntityIdSetOptions){0, we_hashEntityId, we_equalsEntityId, NULL}
#define WarEntityListDefaultOptions (WarEntityListOptions){NULL, we_equalsEntity, we_freeEntity}
#define WarEntityListNonFreeOptions (WarEntityListOptions){NULL, we_equalsEntity}

struct _WarTransformComponent
{
    vec2 position;
    vec2 rotation;
    vec2 scale;
};

#define WAR_TRANSFORM_COMPONENT_INIT_CONST(...) { \
    .position = {0},                               \
    .rotation = {0},                               \
    .scale    = {1, 1},                            \
    __VA_ARGS__                                    \
}
#define WAR_TRANSFORM_COMPONENT_INIT(...) ((WarTransformComponent)WAR_TRANSFORM_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarSpriteComponent
{
    s32 resourceIndex;
    s32 frameIndex;
    WarSprite sprite;
};

#define WAR_SPRITE_COMPONENT_INIT_CONST(...) { \
    .resourceIndex = 0,                         \
    .frameIndex    = 0,                         \
    .sprite        = {0},                       \
    __VA_ARGS__                                 \
}
#define WAR_SPRITE_COMPONENT_INIT(...) ((WarSpriteComponent)WAR_SPRITE_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarUnitComponent
{
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

#define WAR_UNIT_COMPONENT_INIT_CONST(...) {                  \
    .type                = 0,                              \
    .direction           = 0,                              \
    .tilex               = 0,                              \
    .tiley               = 0,                              \
    .sizex               = 1,                              \
    .sizey               = 1,                              \
    .player              = 0,                              \
    .resourceKind        = WAR_RESOURCE_NONE,              \
    .amount              = 0,                              \
    .building            = false,                          \
    .buildPercent        = 0,                              \
    .maxhp               = 0,                              \
    .hp                  = 0,                              \
    .maxMana             = 0,                              \
    .mana                = 0,                              \
    .armor               = 0,                              \
    .range               = 0,                              \
    .minDamage           = 0,                              \
    .rndDamage           = 0,                              \
    .decay               = 0,                              \
    .invisible           = false,                          \
    .invulnerable        = false,                          \
    .hasBeenSeen         = false,                          \
    .speed               = 0,                              \
    .actionType          = WAR_ACTION_TYPE_IDLE,           \
    .manaTime            = 0,                              \
    .invisibilityTime    = 0,                              \
    .invulnerabilityTime = 0,                              \
    __VA_ARGS__                                            \
}
#define WAR_UNIT_COMPONENT_INIT(...) ((WarUnitComponent)WAR_UNIT_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarAnimationsComponent
{
    WarSpriteAnimationList animations;
};

struct _WarRoadComponent
{
    WarRoadPieceList pieces;
};

struct _WarWallComponent
{
    WarWallPieceList pieces;
};

struct _WarRuinComponent
{
    WarRuinPieceList pieces;
};

struct _WarForestComponent
{
    WarTreeList trees;
};

struct _WarStateMachineComponent
{
    WarState* currentState;
    WarState* nextState;
    bool leaveState;
    bool enterState;
};

#define WAR_STATE_MACHINE_COMPONENT_INIT_CONST(...) { \
    .currentState   = NULL,                            \
    .nextState      = NULL,                            \
    .leaveState = false,                               \
    .enterState = false,                               \
    __VA_ARGS__                                        \
}
#define WAR_STATE_MACHINE_COMPONENT_INIT(...) ((WarStateMachineComponent)WAR_STATE_MACHINE_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarUIComponent
{
    String name;
};

struct _WarTextComponent
{
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

#define WAR_TEXT_COMPONENT_INIT_CONST(...) { \
    .fontIndex       = 0,                                 \
    .fontSize        = 10,                                \
    .lineHeight      = 0,                                 \
    .fontColor       = FONT_NORMAL_COLOR_INIT,            \
    .highlightColor  = FONT_HIGHLIGHT_COLOR_INIT,         \
    .highlightIndex  = NO_HIGHLIGHT,                      \
    .highlightCount  = 0,                                 \
    .boundings       = {0},                               \
    .horizontalAlign = WAR_TEXT_ALIGN_LEFT,               \
    .verticalAlign   = WAR_TEXT_ALIGN_TOP,                \
    .lineAlign       = WAR_TEXT_ALIGN_LEFT,               \
    .wrapping        = WAR_TEXT_WRAP_NONE,                \
    .trimming        = WAR_TEXT_TRIM_NONE,                \
    .multiline       = false,                             \
    __VA_ARGS__                                           \
}
#define WAR_TEXT_COMPONENT_INIT(...) ((WarTextComponent)WAR_TEXT_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarRectComponent
{
    vec2 size;
    WarColor color;
};

#define WAR_RECT_COMPONENT_INIT_CONST(...) { \
    .size  = {0},                             \
    .color = {0},                             \
    __VA_ARGS__                               \
}
#define WAR_RECT_COMPONENT_INIT(...) ((WarRectComponent)WAR_RECT_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarButtonComponent
{
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

#define WAR_BUTTON_COMPONENT_INIT_CONST(...) { \
    .interactive    = true,                    \
    .hot            = false,                   \
    .active         = false,                   \
    .hotKey         = WAR_KEY_NONE,            \
    .highlightIndex = NO_HIGHLIGHT,            \
    .highlightCount = 0,                       \
    .tooltip        = {0},                     \
    .gold           = 0,                       \
    .wood           = 0,                       \
    .normalSprite   = {0},                     \
    .pressedSprite  = {0},                     \
    .clickHandler   = NULL,                    \
    __VA_ARGS__                                \
}
#define WAR_BUTTON_COMPONENT_INIT(...) ((WarButtonComponent)WAR_BUTTON_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarCursorComponent
{
    WarCursorType type;
    vec2 hot;
};

#define WAR_CURSOR_COMPONENT_INIT_CONST(...) { \
    .type = WAR_CURSOR_ARROW,                   \
    .hot  = {0},                                \
    __VA_ARGS__                                 \
}
#define WAR_CURSOR_COMPONENT_INIT(...) ((WarCursorComponent)WAR_CURSOR_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarProjectileComponent
{
    WarProjectileType type;
    WarEntityId sourceEntityId;
    WarEntityId targetEntityId;
    vec2 origin;
    vec2 target;
    s32 speed;
};

#define WAR_PROJECTILE_COMPONENT_INIT_CONST(...) { \
    .type           = 0,                            \
    .sourceEntityId = 0,                            \
    .targetEntityId = 0,                            \
    .origin         = {0},                          \
    .target         = {0},                          \
    .speed          = 0,                            \
    __VA_ARGS__                                     \
}
#define WAR_PROJECTILE_COMPONENT_INIT(...) ((WarProjectileComponent)WAR_PROJECTILE_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarPoisonCloudComponent
{
    vec2 position;
    f32 time; // time in seconds left of the spell
    f32 damageTime; // time in seconds left to inflict damage
    String animName;
};

#define WAR_POISON_CLOUD_COMPONENT_INIT_CONST(...) { \
    .position   = {0},                               \
    .time       = 0,                                 \
    .damageTime = 0,                                 \
    .animName   = {0},                               \
    __VA_ARGS__                                      \
}
#define WAR_POISON_CLOUD_COMPONENT_INIT(...) ((WarPoisonCloudComponent)WAR_POISON_CLOUD_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarSightComponent
{
    vec2 position;
    f32 time; // time in seconds left of the spell
};

#define WAR_SIGHT_COMPONENT_INIT_CONST(...) { \
    .position = {0},                           \
    .time     = 0,                             \
    __VA_ARGS__                                \
}
#define WAR_SIGHT_COMPONENT_INIT(...) ((WarSightComponent)WAR_SIGHT_COMPONENT_INIT_CONST(__VA_ARGS__))

struct _WarEntity
{
    WarEntityId id;
    u16 type;                             // WarEntityType cast to u16
    u16 components[MAX_COMPONENTS_COUNT]; // INVALID_COMP_INDEX = absent
};
static_assert(sizeof(WarEntity) == 64, "WarEntity must be 64 bytes");

#define DEFINE_COMPONENT_STORAGE(name, type) typedef struct \
{ \
    bool enabled[MAX_ENTITIES_COUNT]; \
    type dense[MAX_ENTITIES_COUNT]; \
    WarEntityId owners[MAX_ENTITIES_COUNT]; \
    s32 count; \
} name

DEFINE_COMPONENT_STORAGE(WarTransformStorage, WarTransformComponent);
DEFINE_COMPONENT_STORAGE(WarSpriteStorage, WarSpriteComponent);
DEFINE_COMPONENT_STORAGE(WarUnitStorage, WarUnitComponent);
DEFINE_COMPONENT_STORAGE(WarAnimationsStorage, WarAnimationsComponent);
DEFINE_COMPONENT_STORAGE(WarRoadStorage, WarRoadComponent);
DEFINE_COMPONENT_STORAGE(WarWallStorage, WarWallComponent);
DEFINE_COMPONENT_STORAGE(WarRuinStorage, WarRuinComponent);
DEFINE_COMPONENT_STORAGE(WarForestStorage, WarForestComponent);
DEFINE_COMPONENT_STORAGE(WarStateMachineStorage, WarStateMachineComponent);
DEFINE_COMPONENT_STORAGE(WarUIStorage, WarUIComponent);
DEFINE_COMPONENT_STORAGE(WarTextStorage, WarTextComponent);
DEFINE_COMPONENT_STORAGE(WarRectStorage, WarRectComponent);
DEFINE_COMPONENT_STORAGE(WarButtonStorage, WarButtonComponent);
DEFINE_COMPONENT_STORAGE(WarAudioStorage, WarAudioComponent);
DEFINE_COMPONENT_STORAGE(WarCursorStorage, WarCursorComponent);
DEFINE_COMPONENT_STORAGE(WarProjectileStorage, WarProjectileComponent);
DEFINE_COMPONENT_STORAGE(WarPoisonCloudStorage, WarPoisonCloudComponent);
DEFINE_COMPONENT_STORAGE(WarSightStorage, WarSightComponent);

struct _WarEntityManager
{
    // --- Entity pool (flat array, slot 0 reserved/empty) ---
    WarEntity entities[MAX_ENTITIES_COUNT];
    s32 entityCount;
    s32 nextEntityId; // monotonic; replaces staticEntityId

    // --- Secondary indices (kept for O(1) typed queries) ---
    WarEntityMap   entitiesByType; // WarEntityType  -> WarEntityList*
    WarUnitMap     unitsByType;    // WarUnitType    -> WarEntityList*
    WarEntityIdMap entitiesById;   // WarEntityId    -> WarEntity*
    WarEntityList  uiEntities;     // subset of entities with COMP_UI present

    // --- Per-component dense storage ---
    WarTransformStorage    transforms;
    WarSpriteStorage       sprites;
    WarUnitStorage         units;
    WarAnimationsStorage   animations;
    WarRoadStorage         roads;
    WarWallStorage         walls;
    WarRuinStorage         ruins;
    WarForestStorage       forests;
    WarStateMachineStorage stateMachines;
    WarUIStorage           uis;
    WarTextStorage         texts;
    WarRectStorage         rects;
    WarButtonStorage       buttons;
    WarAudioStorage        audios;
    WarCursorStorage       cursors;
    WarProjectileStorage   projectiles;
    WarPoisonCloudStorage  poisonClouds;
    WarSightStorage        sights;
};

bool we_isComponentEnabled(WarContext* context, WarEntity* entity, WarComponentType componentType);
void we_setComponentEnabled(WarContext* context, WarEntity* entity, WarComponentType componentType, bool enabled);
void we_enableComponent(WarContext* context, WarEntity* entity, WarComponentType componentType);
void we_disableComponent(WarContext* context, WarEntity* entity, WarComponentType componentType);

WarTransformComponent*    we_getTransformComponent(WarContext* context, const WarEntity* entity);
WarSpriteComponent*       we_getSpriteComponent(WarContext* context, const WarEntity* entity);
WarUnitComponent*         we_getUnitComponent(WarContext* context, const WarEntity* entity);
WarAnimationsComponent*   we_getAnimationsComponent(WarContext* context, const WarEntity* entity);
WarRoadComponent*         we_getRoadComponent(WarContext* context, const WarEntity* entity);
WarWallComponent*         we_getWallComponent(WarContext* context, const WarEntity* entity);
WarRuinComponent*         we_getRuinComponent(WarContext* context, const WarEntity* entity);
WarForestComponent*       we_getForestComponent(WarContext* context, const WarEntity* entity);
WarStateMachineComponent* we_getStateMachineComponent(WarContext* context, const WarEntity* entity);
WarUIComponent*           we_getUIComponent(WarContext* context, const WarEntity* entity);
WarTextComponent*         we_getTextComponent(WarContext* context, const WarEntity* entity);
WarRectComponent*         we_getRectComponent(WarContext* context, const WarEntity* entity);
WarButtonComponent*       we_getButtonComponent(WarContext* context, const WarEntity* entity);
WarAudioComponent*        we_getAudioComponent(WarContext* context, const WarEntity* entity);
WarCursorComponent*       we_getCursorComponent(WarContext* context, const WarEntity* entity);
WarProjectileComponent*   we_getProjectileComponent(WarContext* context, const WarEntity* entity);
WarPoisonCloudComponent*  we_getPoisonCloudComponent(WarContext* context, const WarEntity* entity);
WarSightComponent*        we_getSightComponent(WarContext* context, const WarEntity* entity);

WarTransformComponent* we_addTransformComponent(WarContext* context, WarEntity* entity, WarTransformComponent params);
void we_removeTransformComponent(WarContext* context, WarEntity* entity);

WarSpriteComponent* we_addSpriteComponent(WarContext* context, WarEntity* entity, WarSpriteComponent params);
WarSpriteComponent* we_addSpriteComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef spriteResourceRef);
void we_removeSpriteComponent(WarContext* context, WarEntity* entity);

WarUnitComponent* we_addUnitComponent(WarContext* context, WarEntity* entity, WarUnitComponent params);
void we_removeUnitComponent(WarContext* context, WarEntity* entity);

WarRoadComponent* we_addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces);
void we_removeRoadComponent(WarContext* context, WarEntity* entity);

WarWallComponent* we_addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces);
void we_removeWallComponent(WarContext* context, WarEntity* entity);

WarRuinComponent* we_addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces);
void we_removeRuinComponent(WarContext* context, WarEntity* entity);

WarForestComponent* we_addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees);
void we_removeForestComponent(WarContext* context, WarEntity* entity);

WarStateMachineComponent* we_addStateMachineComponent(WarContext* context, WarEntity* entity);
void we_removeStateMachineComponent(WarContext* context, WarEntity* entity);

WarAnimationsComponent* we_addAnimationsComponent(WarContext* context, WarEntity* entity);
void we_removeAnimationsComponent(WarContext* context, WarEntity* entity);

WarUIComponent* we_addUIComponent(WarContext* context, WarEntity* entity, String name);
void we_removeUIComponent(WarContext* context, WarEntity* entity);

WarTextComponent* we_addTextComponent(WarContext* context, WarEntity* entity, WarTextComponent params);
void we_removeTextComponent(WarContext* context, WarEntity* entity);

WarRectComponent* we_addRectComponent(WarContext* context, WarEntity* entity, WarRectComponent params);
void we_removeRectComponent(WarContext* context, WarEntity* entity);

WarButtonComponent* we_addButtonComponent(WarContext* context, WarEntity* entity, WarButtonComponent params);
WarButtonComponent* we_addButtonComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef normalRef, WarSpriteResourceRef pressedRef);
void we_removeButtonComponent(WarContext* context, WarEntity* entity);

WarAudioComponent* we_addAudioComponent(WarContext* context, WarEntity* entity, WarAudioComponent params);
void we_removeAudioComponent(WarContext* context, WarEntity* entity);

WarCursorComponent* we_addCursorComponent(WarContext* context, WarEntity* entity, WarCursorComponent params);
void we_removeCursorComponent(WarContext* context, WarEntity* entity);

WarProjectileComponent* we_addProjectileComponent(WarContext* context, WarEntity* entity, WarProjectileComponent params);
void we_removeProjectileComponent(WarContext* context, WarEntity* entity);

WarPoisonCloudComponent* we_addPoisonCloudComponent(WarContext* context, WarEntity* entity, WarPoisonCloudComponent params);
void we_removePoisonCloudComponent(WarContext* context, WarEntity* entity);

WarSightComponent* we_addSightComponent(WarContext* context, WarEntity* entity, WarSightComponent params);
void we_removeSightComponent(WarContext* context, WarEntity* entity);

// Roads
WarEntity* we_createRoad(WarContext* context);
void we_addRoadPiece(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 player);
void we_addRoadPiecesFromConstruct(WarContext* context, WarEntity* entity, WarLevelConstruct *construct);
bool we_hasRoadPieceAtPosition(WarContext* context, WarEntity* entity, s32 x, s32 y);
WarRoadPiece* we_getRoadPieceAtPosition(WarContext* context, WarEntity* entity, s32 x, s32 y);
void we_removeRoadPiece(WarContext* context, WarEntity* entity, WarRoadPiece* piece);
void we_determineRoadTypes(WarContext* context, WarEntity* entity);

// Walls
WarEntity* we_createWall(WarContext* context);
WarWallPiece* we_addWallPiece(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 player);
void we_addWallPiecesFromConstruct(WarContext* context, WarEntity* entity, WarLevelConstruct *construct);
bool we_hasWallPieceAtPosition(WarContext* context, WarEntity* entity, s32 x, s32 y);
WarWallPiece* we_getWallPieceAtPosition(WarContext* context, WarEntity* entity, s32 x, s32 y);
void we_removeWallPiece(WarContext* context, WarEntity* entity, WarWallPiece* piece);
void we_determineWallTypes(WarContext* context, WarEntity* entity);
void we_takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage);

// Ruins
WarEntity* we_createRuins(WarContext* context);
void we_addRuinsPieces(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 dim);
bool we_hasRuinPieceAtPosition(WarContext* context, WarEntity* ruins, s32 x, s32 y);
WarRuinPiece* we_getRuinPieceAtPosition(WarContext* context, WarEntity* ruins, s32 x, s32 y);
void we_removeRuinPiece(WarContext* context, WarEntity* entity, WarRuinPiece* piece);
void we_determineRuinTypes(WarContext* context, WarEntity* entity);

// Trees
bool we_hasTreeAtPosition(WarContext* context, WarEntity* forest, s32 x, s32 y);
WarTree* we_getTreeAtPosition(WarContext* context, WarEntity* forest, s32 x, s32 y);
void we_determineTreeTiles(WarContext* context, WarEntity* forest);
void we_determineAllTreeTiles(WarContext* context);
WarTree* we_findAccesibleTree(WarContext* context, WarEntity* forest, vec2 position);
void we_plantTree(WarContext* context, WarEntity* forest, s32 x, s32 y);
bool we_validTree(WarContext* context, WarEntity* forest, WarTree* tree);
s32 we_chopTree(WarContext* context, WarEntity* forest, WarTree* tree, s32 amount);

// Entities
WarEntity* we_createEntity(WarContext* context, WarEntityType type, bool addToMap);

typedef struct {
    WarUnitType type;
    s32 x;
    s32 y;
    u8 player;
    WarResourceKind resourceKind;
    u32 amount;
    bool addToMap;
    bool isGoingToTrain;
    bool isGoingToBuild;
} CreateUnitArgs;

#define CREATE_UNIT_ARGS_INIT_CONST(...) { \
    .type = 0, \
    .x = 0, \
    .y = 0, \
    .player = 0, \
    .resourceKind = WAR_RESOURCE_NONE, \
    .amount = 0, \
    .addToMap = true, \
    .isGoingToTrain = false, \
    .isGoingToBuild = false, \
    __VA_ARGS__ \
}
#define CREATE_UNIT_ARGS_INIT(...) (&(CreateUnitArgs)CREATE_UNIT_ARGS_INIT_CONST(__VA_ARGS__))

WarEntity* we_createUnit(WarContext* context, const CreateUnitArgs* args);
WarEntity* we_createDude(WarContext* context, const CreateUnitArgs* args);
WarEntity* we_createBuilding(WarContext* context, const CreateUnitArgs* args);

WarEntity* we_findEntity(WarContext* context, WarEntityId id);
WarEntity* we_findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type);
WarEntity* we_findUIEntity(WarContext* context, StringView name);
WarEntity* we_findEntityUnderCursor(WarContext* context, bool includeTrees, bool includeWalls);
void we_removeEntity(WarContext* context, WarEntity* entity);
void we_removeEntityById(WarContext* context, WarEntityId id);
void we_renderEntity(WarContext* context, WarEntity* entity);
void we_renderEntitiesOfType(WarContext* context, WarEntityType type);
void we_renderUnitSelection(WarContext* context);

void we_initEntityManager(WarContext* context, WarEntityManager* manager);
WarEntityManager* we_getEntityManager(WarContext* context);
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
bool we_isBeingAttackedBy(WarContext* context, WarEntity* entity, WarEntity* other);
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
