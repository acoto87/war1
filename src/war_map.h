#pragma once

#include "war_types.h"

WarMap* wmap_createMap(WarContext *context, s32 levelInfoIndex);
void wmap_freeMap(WarContext* context, WarMap* map);

void wmap_enterMap(WarContext* context);
void wmap_updateMap(WarContext* context);
void wmap_leaveMap(WarContext* context);

void wmap_addEntityToSelection(WarContext* context, WarEntityId id);
void wmap_removeEntityFromSelection(WarContext* context, WarEntityId id);
void wmap_clearSelection(WarContext* context);

vec2 wmap_vec2ScreenToMapCoordinates(WarContext* context, vec2 v);
vec2 wmap_vec2ScreenToMinimapCoordinates(WarContext* context, vec2 v);
rect wmap_rectScreenToMapCoordinates(WarContext* context, rect r);
vec2 wmap_vec2MapToScreenCoordinates(WarContext* context, vec2 v);
rect wmap_rectMapToScreenCoordinates(WarContext* context, rect r);
vec2 wmap_vec2MapToTileCoordinates(vec2 v);
vec2 wmap_vec2TileToMapCoordinates(vec2 v, bool centeredInTile);
vec2 wmap_vec2MinimapToViewportCoordinates(WarContext* context, vec2 v);

WarColor wmap_getMapTileAverage(WarResource* levelVisual, WarResource* tileset, s32 x, s32 y);
void wmap_updateMinimapTile(WarContext* context, WarResource* levelVisual, WarResource* tileset, s32 x, s32 y);
s32 wmap_getMapTileIndex(WarContext* context, s32 x, s32 y);
void wmap_setMapTileIndex(WarContext* context, s32 x, s32 y, s32 tile);

WarMapTile* wmap_getMapTileState(WarMap* map, s32 x, s32 y);
void wmap_setMapTileState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState tileState);
void wmap_setUnitMapTileState(WarMap* map, WarEntity* entity, WarMapTileState tileState);
bool wmap_isTileInState(WarMap* map, s32 x, s32 y, WarMapTileState state);
bool wmap_isAnyTileInStates(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state);
bool wmap_isAnyUnitTileInStates(WarMap* map, WarEntity* entity, WarMapTileState state);
bool wmap_areAllTilesInState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state);
bool wmap_areAllUnitTilesInState(WarMap* map, WarEntity* entity, WarMapTileState state);

#define isUnitPartiallyVisible(map, entity) wmap_isAnyUnitTileInStates(map, entity, MAP_TILE_STATE_VISIBLE)
#define isUnitVisible(map, entity) wmap_areAllUnitTilesInState(map, entity, MAP_TILE_STATE_VISIBLE)
#define isUnitPartiallyFog(map, entity) wmap_isAnyUnitTileInStates(map, entity, MAP_TILE_STATE_FOG)
#define isUnitFog(map, entity) wmap_areAllUnitTilesInState(map, entity, MAP_TILE_STATE_FOG)
#define isUnitPartiallyUnkown(map, entity) wmap_isAnyUnitTileInStates(map, entity, MAP_TILE_STATE_UNKOWN)
#define isUnitUnknown(map, entity) wmap_areAllUnitTilesInState(map, entity, MAP_TILE_STATE_UNKOWN)

#define isTileUnkown(map, x, y) wmap_isTileInState(map, x, y, MAP_TILE_STATE_UNKOWN)
#define isTileFog(map, x, y) wmap_isTileInState(map, x, y, MAP_TILE_STATE_FOG)
#define isTileVisible(map, x, y) wmap_isTileInState(map, x, y, MAP_TILE_STATE_VISIBLE)

void wui_changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type);

WarCampaignMapType wmap_getCampaignMapTypeByLevelInfoIndex(s32 levelInfoIndex);

f32 wmap_getMapScaledSpeed(WarContext* context, f32 t);
f32 wmap_getMapScaledTime(WarContext* context, f32 t);

#define getMapScrollSpeed(speedValue) ((f32)(100 + (speedValue) * 50))
