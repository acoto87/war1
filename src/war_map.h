#pragma once

WarMap* createMap(WarContext *context, s32 levelInfoIndex);
void freeMap(WarContext* context, WarMap* map);

void enterMap(WarContext* context);
void updateMap(WarContext* context);
void leaveMap(WarContext* context);

void addEntityToSelection(WarContext* context, WarEntityId id);
void removeEntityFromSelection(WarContext* context, WarEntityId id);
void clearSelection(WarContext* context);

vec2 vec2ScreenToMapCoordinates(WarContext* context, vec2 v);
vec2 vec2ScreenToMinimapCoordinates(WarContext* context, vec2 v);
rect rectScreenToMapCoordinates(WarContext* context, rect r);
vec2 vec2MapToScreenCoordinates(WarContext* context, vec2 v);
rect rectMapToScreenCoordinates(WarContext* context, rect r);
vec2 vec2MapToTileCoordinates(vec2 v);
vec2 vec2TileToMapCoordinates(vec2 v, bool centeredInTile);
vec2 vec2MinimapToViewportCoordinates(WarContext* context, vec2 v);

u8Color getMapTileAverage(WarResource* levelVisual, WarResource* tileset, s32 x, s32 y);
void updateMinimapTile(WarContext* context, WarResource* levelVisual, WarResource* tileset, s32 x, s32 y);
s32 getMapTileIndex(WarContext* context, s32 x, s32 y);
void setMapTileIndex(WarContext* context, s32 x, s32 y, s32 tile);

WarMapTile* getMapTileState(WarMap* map, s32 x, s32 y);
void setMapTileState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState tileState);
void setUnitMapTileState(WarMap* map, WarEntity* entity, WarMapTileState tileState);
bool isTileInState(WarMap* map, s32 x, s32 y, WarMapTileState state);
bool isAnyTileInStates(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state);
bool isAnyUnitTileInStates(WarMap* map, WarEntity* entity, WarMapTileState state);
bool areAllTilesInState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state);
bool areAllUnitTilesInState(WarMap* map, WarEntity* entity, WarMapTileState state);

#define isUnitPartiallyVisible(map, entity) isAnyUnitTileInStates(map, entity, MAP_TILE_STATE_VISIBLE)
#define isUnitVisible(map, entity) areAllUnitTilesInState(map, entity, MAP_TILE_STATE_VISIBLE)
#define isUnitPartiallyFog(map, entity) isAnyUnitTileInStates(map, entity, MAP_TILE_STATE_FOG)
#define isUnitFog(map, entity) areAllUnitTilesInState(map, entity, MAP_TILE_STATE_FOG)
#define isUnitPartiallyUnkown(map, entity) isAnyUnitTileInStates(map, entity, MAP_TILE_STATE_UNKOWN)
#define isUnitUnknown(map, entity) areAllUnitTilesInState(map, entity, MAP_TILE_STATE_UNKOWN)

#define isTileUnkown(map, x, y) isTileInState(map, x, y, MAP_TILE_STATE_UNKOWN)
#define isTileFog(map, x, y) isTileInState(map, x, y, MAP_TILE_STATE_FOG)
#define isTileVisible(map, x, y) isTileInState(map, x, y, MAP_TILE_STATE_VISIBLE)

void changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type);

WarCampaignMapType getCampaignMapTypeByLevelInfoIndex(s32 levelInfoIndex);

f32 getMapScaledSpeed(WarContext* context, f32 t);
f32 getMapScaledTime(WarContext* context, f32 t);

#define getMapScrollSpeed(speedValue) ((f32)(100 + (speedValue) * 50))
