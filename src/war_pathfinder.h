#pragma once

#include "war_types.h"

// this value is the distance squared to avoid dynamic units
// so if the dynamic entity at a distance squared less than this value, 
// treat it like a static one, so the unit can get around it and the risk of
// overlapping units is less
#define DISTANCE_SQR_AVOID_DYNAMIC_POSITIONS 2.0f

const s32 dirC = 8;
const s32 dirX[] = {  0,  1, 1, 1, 0, -1, -1, -1 };
const s32 dirY[] = { -1, -1, 0, 1, 1,  1,  0, -1 };

u16 getTileValue(WarPathFinder finder, s32 x, s32 y);
void setTilesValue(WarPathFinder finder, s32 startX, s32 startY, s32 width, s32 height, u16 value);

#define setFreeTiles(finder, startX, startY, width, height) setTilesValue(finder, startX, startY, width, height, PATH_FINDER_DATA_EMPTY)
#define setDynamicEntity(finder, startX, startY, width, height, id) setTilesValue(finder, startX, startY, width, height, (id << 4) | PATH_FINDER_DATA_DYNAMIC)
#define setStaticEntity(finder, startX, startY, width, height, id) setTilesValue(finder, startX, startY, width, height, (id << 4) | PATH_FINDER_DATA_STATIC)

#define getTileValueType(finder, x, y) (getTileValue(finder, x, y) & 0x000F)
#define getTileEntityId(finder, x, y) ((getTileValue(finder, x, y) & 0xFFF0) >> 4)
#define isEmpty(finder, x, y) (getTileValueType(finder, x, y) == PATH_FINDER_DATA_EMPTY)
#define isStatic(finder, x, y) (getTileValueType(finder, x, y) == PATH_FINDER_DATA_STATIC)
#define isDynamic(finder, x,  y) (getTileValueType(finder, x, y) == PATH_FINDER_DATA_DYNAMIC)
#define isDynamicOfEntity(finder, x, y, id) (getTileEntityId(finder, x, y) == id)

WarPathFinder initPathFinder(PathFindingType type, s32 width, s32 height, u16 data[]);
bool isInside(WarPathFinder finder, s32 x, s32 y);
WarMapPath findPath(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY);
bool reRoutePath(WarPathFinder finder, WarMapPath* path, s32 fromIndex, s32 toIndex);
bool pathExists(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY);
void freePath(WarMapPath path);

vec2 findEmptyPosition(WarPathFinder finder, vec2 position);
bool isPositionAccesible(WarPathFinder finder, vec2 position);
