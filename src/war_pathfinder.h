#pragma once

#include "shl/binary_heap.h"
#include "shl/list.h"
#include "shl/map.h"

#include "war.h"
#include "war_common.h"
#include "war_fwd.h"
#include "war_math.h"
#include "war_collections.h"

struct _WarMapPath
{
    vec2 nodes[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
    s32 count;
};

struct _WarMapFlowField
{
    s32 cost[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
    u8 dirs[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
};

struct _WarPathFinder
{
    u16 data[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
    WarMapFlowField* fields[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
};

WarPathFinder wpath_initPathFinder(u16 data[MAP_TILES_WIDTH * MAP_TILES_HEIGHT]);
bool wpath_isInside(s32 x, s32 y);
void wpath_astar(WarPathFinder* finder, s32 startX, s32 startY, s32 endX, s32 endY, WarMapPath* path);
void wpath_flowField(WarPathFinder* finder, s32 x, s32 y, WarMapFlowField* flowField);
WarMapFlowField* wpath_computeFlowField(WarPathFinder* finder, s32 x, s32 y);
WarMapFlowField* wpath_getFlowField(WarPathFinder* finder, s32 x, s32 y);
vec2 wpath_flowFieldSample(WarMapFlowField* flowField, s32 x, s32 y);
vec2 wpath_findEmptyTile(WarPathFinder* finder, vec2 tile);
bool wpath_isTileAccesible(WarPathFinder* finder, vec2 tile);
u16 wpath_getTileValue(WarPathFinder* finder, s32 x, s32 y);
WarPathFinderDataType wpath_getTileValueType(WarPathFinder* finder, s32 x, s32 y);
WarEntityId wpath_getTileEntityId(WarPathFinder* finder, s32 x, s32 y);
void wpath_setTilesValue(WarPathFinder* finder, s32 startX, s32 startY, s32 width, s32 height, u16 value);
void wpath_setFreeTiles(WarPathFinder* finder, s32 startX, s32 startY, s32 width, s32 height);
void wpath_setStaticEntity(WarPathFinder* finder, s32 startX, s32 startY, s32 width, s32 height, WarEntityId id);
bool wpath_isEmpty(WarPathFinder* finder, s32 x, s32 y);
bool wpath_isStatic(WarPathFinder* finder, s32 x, s32 y);
