#pragma once

#include "war_ai.h"
#include "war_campaigns.h"
#include "war_cheats.h"
#include "war_entities.h"
#include "war_resources.h"

#define isHumanPlayer(player) ((player)->race == WAR_RACE_HUMANS)
#define isOrcPlayer(player) ((player)->race == WAR_RACE_ORCS)
#define isNeutralPlayer(player) ((player)->race == WAR_RACE_NEUTRAL)

#define isFeatureAllowed(player, feature) ((player)->features[(feature)/2])
#define setFeatureAllowed(player, feature, allowed) ((player)->features[(feature/2)] = (allowed))

#define incrementUpgradeLevel(player, upgrade) ((player)->upgrades[(upgrade)/2].level++)
#define hasAnyUpgrade(player, upgrade) \
    ((player)->upgrades[(upgrade)/2].allowed > 0 && \
     (player)->upgrades[(upgrade)/2].level > 0)
#define hasRemainingUpgrade(player, upgrade) \
    ((player)->upgrades[(upgrade)/2].level < (player)->upgrades[(upgrade)/2].allowed)
#define getUpgradeLevel(player, upgrade) \
    ((player)->upgrades[(upgrade)/2].level)
#define checkUpgradeLevel(player, upgrade) \
    ((player)->upgrades[(upgrade)/2].level <= (player)->upgrades[(upgrade)/2].allowed)
#define setUpgradeAllowed(player, upgrade, value) \
    ((player)->upgrades[(upgrade)/2].allowed = (value))

#define imageResourceRefFromPlayer(player, hIdx, oIdx) imageResourceRef((player)->race == WAR_RACE_HUMANS ? (hIdx) : (oIdx))
#define spriteResourceRefFromPlayer(player, hIdx, oIdx, spriteIndex) wspr_createSpriteResourceRef((player)->race == WAR_RACE_HUMANS ? (hIdx) : (oIdx), 1, arrayArg(s32, (spriteIndex)))

// Reserved resource indices for editor-loaded custom .w1m maps.
// These are above the normal DATA.WAR range (0-582).
#define WAR_CUSTOM_LEVEL_INFO_INDEX  583
#define WAR_CUSTOM_VISUAL_INDEX      584
#define WAR_CUSTOM_PASSABLE_INDEX    585

struct _WarMapTile
{
    WarMapTileState state;
    WarFogPieceType type;
    WarFogBoundaryType boundary;
};

struct _WarUpgrade
{
    s32 allowed;
    s32 level;
};

struct _WarPlayerInfo
{
    u8 index;
    WarRace race;
    s32 gold;
    s32 wood;
    bool godMode;
    bool features[MAX_FEATURES_COUNT];
    WarUpgrade upgrades[MAX_UPGRADES_COUNT];
    WarAI* ai;
};

struct _WarMapSettings
{
    WarMapSpeed gameSpeed;
    s32 musicVol;
    s32 sfxVol;
    WarMapSpeed mouseScrollSpeed;
    WarMapSpeed keyScrollSpeed;
};

struct _WarMap
{
    bool playing;
    bool custom;
    WarLevelResult result;
    WarMenuState menuState;

    s32 levelInfoIndex;
    f32 objectivesTime;

    // scroll
    bool isScrolling;
    bool wasScrolling;

    WarMapSettings settings;
    WarMapSettings settings2;

    // viewport in map coordinates,
    // this is the portion of the map that the player see
    rect viewport;

    // panels of the ui, in screen coordinates
    rect leftTopPanel;
    rect leftBottomPanel;
    rect topPanel;
    rect bottomPanel;
    rect rightPanel;
    rect mapPanel;
    rect minimapPanel;
    rect menuPanel;
    rect messagePanel;
    rect saveLoadPanel;

    WarSprite sprite;
    WarSprite minimapSprite;
    WarSprite blackSprite;

    WarMapTilesetType tilesetType;
    WarMapTile tiles[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];

    WarEntityManager entityManager;
    WarEntityIdList selectedEntities;

    WarEntity* forest;
    WarEntity* wall;
    WarEntity* road;
    WarEntity* ruin;

    bool editingTrees;
    bool editingWalls;
    bool editingRoads;
    bool editingRuins;
    bool editingRainOfFire;
    bool addingUnit;
    WarUnitType addingUnitType;

    bool hurryUp;
    bool fowEnabled;

    WarPathFinder finder;
    WarUnitCommand command;
    WarFlashStatus flashStatus;
    WarCheatStatus cheatStatus;
    WarPlayerInfo players[MAX_PLAYERS_COUNT];

    char hudStatusText[256];
    s32 hudStatusHighlightIndex;
    s32 hudStatusHighlightCount;
    s32 hudStatusGold;
    s32 hudStatusWood;

    WarUnitCommandData commandSlots[6];
    bool commandSlotActive[6];

    char commandTexts[4][32];
    s32 commandTextHighlightIndex[4];
    s32 commandTextHighlightCount[4];
    bool commandTextVisible[4];
};

WarMap* wmap_createMap(WarContext *context, s32 levelInfoIndex);
void wmap_freeMap(WarContext* context, WarMap* map);

bool wmap_loadCustomMap(WarContext* context, StringView mapPath);

void wmap_enterMap(WarContext* context);
void wmap_updateMap(WarContext* context);
void wmap_leaveMap(WarContext* context);
void wmap_renderMap(WarContext* context);

void wmap_addEntityToSelection(WarContext* context, WarEntityId id);
void wmap_removeEntityFromSelection(WarContext* context, WarEntityId id);
void wmap_clearSelection(WarContext* context);

vec2 wmap_getDirFromArrowKeys(WarContext* context);
vec2 wmap_getDirFromMousePos(WarContext* context);
vec2 wmap_screenToMapCoordinatesV(WarContext* context, vec2 v);
vec2 wmap_screenToMinimapCoordinatesV(WarContext* context, vec2 v);
rect wmap_screenToMapCoordinatesR(WarContext* context, rect r);
vec2 wmap_mapToScreenCoordinatesV(WarContext* context, vec2 v);
rect wmap_mapToScreenCoordinatesR(WarContext* context, rect r);
vec2 wmap_mapToTileCoordinatesV(vec2 v);
vec2 wmap_tileToMapCoordinatesV(vec2 v, bool centeredInTile);
vec2 wmap_minimapToViewportCoordinatesV(WarContext* context, vec2 v);

WarColor wmap_getMapTileAverage(WarResource* levelVisual, WarResource* tileset, s32 x, s32 y);
void wmap_updateMinimapTile(WarContext* context, WarResource* levelVisual, WarResource* tileset, s32 x, s32 y);
s32 wmap_getMapTileIndex(WarContext* context, s32 x, s32 y);
void wmap_setMapTileIndex(WarContext* context, s32 x, s32 y, s32 tile);

WarMapTile* wmap_getMapTileState(WarMap* map, s32 x, s32 y);
void wmap_setMapTileState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState tileState);
void wmap_setUnitMapTileState(WarContext* context, WarMap* map, WarEntity* entity, WarMapTileState tileState);
bool wmap_isTileInState(WarMap* map, s32 x, s32 y, WarMapTileState state);
bool wmap_isAnyTileInStates(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state);
bool wmap_isAnyUnitTileInStates(WarContext* context, WarMap* map, WarEntity* entity, WarMapTileState state);
bool wmap_areAllTilesInState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state);
bool wmap_areAllUnitTilesInState(WarContext* context, WarMap* map, WarEntity* entity, WarMapTileState state);

bool wmap_isUnitPartiallyVisible(WarContext* context, WarMap* map, WarEntity* entity);
bool wmap_isUnitVisible(WarContext* context, WarMap* map, WarEntity* entity);
bool wmap_isUnitPartiallyFog(WarContext* context, WarMap* map, WarEntity* entity);
bool wmap_isUnitFog(WarContext* context, WarMap* map, WarEntity* entity);
bool wmap_isUnitPartiallyUnkown(WarContext* context, WarMap* map, WarEntity* entity);
bool wmap_isUnitUnknown(WarContext* context, WarMap* map, WarEntity* entity);

bool wmap_isTileUnkown(WarMap* map, s32 x, s32 y);
bool wmap_isTileFog(WarMap* map, s32 x, s32 y);
bool wmap_isTileVisible(WarMap* map, s32 x, s32 y);

void wui_changeCursorType(WarContext* context, WarCursorType type);

WarCampaignMapType wmap_getCampaignMapTypeByLevelInfoIndex(s32 levelInfoIndex);

f32 wmap_getMapScaledSpeed(WarContext* context, f32 t);
f32 wmap_getMapScaledTime(WarContext* context, f32 t);

#define getMapScrollSpeed(speedValue) ((f32)(100 + (speedValue) * 50))
