#pragma once

#include "shl/binary_heap.h"
#include "shl/list.h"
#include "shl/map.h"

#include "common.h"
#include "war_fwd.h"
#include "war_math.h"

struct _WarMapNode
{
    s32 id;     // id of the node
    s32 x, y;   // the coordinates of the node
    s32 level;  // the length of the path from the start to this node
    s32 parent; // the previous node in the path from start to end passing through this node
    s32 gScore; // the cost from the start to this node
    s32 fScore; // the cost from start to end passing through this node
};

struct _WarMapPath
{
    vec2List nodes;
};

struct _WarPathFinder
{
    PathFindingType type;
    s32 width, height;
    u16* data;
};

// NOTE: this value is the distance squared to avoid dynamic units
// so if the dynamic entity at a distance squared less than this value,
// treat it like a static one, so the unit can get around it and the risk of
// overlapping units is less
#define DISTANCE_SQR_AVOID_DYNAMIC_POSITIONS 2.0f

#define WarMapNodeEmpty (WarMapNode){0}

bool equalsMapNode(const WarMapNode node1, const WarMapNode node2);
s32 compareFScore(const WarMapNode node1, const WarMapNode node2);

u32 hashMapNode(const s32 key);
bool equalsMapNodeId(const s32 key1, const s32 key2);

shlDeclareList(WarMapNodeList, WarMapNode)
shlDeclareBinaryHeap(WarMapNodeHeap, WarMapNode)
shlDeclareMap(WarMapNodeMap, s32, WarMapNode)

#define WarMapNodeListDefaultOptions (WarMapNodeListOptions){WarMapNodeEmpty, equalsMapNode}
#define WarMapNodeHeapDefaultOptions (WarMapNodeHeapOptions){WarMapNodeEmpty, equalsMapNode, compareFScore}
#define WarMapNodeMapDefaultOptions (WarMapNodeMapOptions){WarMapNodeEmpty, hashMapNode, equalsMapNodeId}

u16 wpath_getTileValue(WarPathFinder finder, s32 x, s32 y);
void wpath_setTilesValue(WarPathFinder finder, s32 startX, s32 startY, s32 width, s32 height, u16 value);

#define setFreeTiles(finder, startX, startY, width, height) wpath_setTilesValue(finder, startX, startY, width, height, PATH_FINDER_DATA_EMPTY)
#define setDynamicEntity(finder, startX, startY, width, height, id) wpath_setTilesValue(finder, startX, startY, width, height, (id << 4) | PATH_FINDER_DATA_DYNAMIC)
#define setStaticEntity(finder, startX, startY, width, height, id) wpath_setTilesValue(finder, startX, startY, width, height, (id << 4) | PATH_FINDER_DATA_STATIC)

#define getTileValueType(finder, x, y) (wpath_getTileValue(finder, x, y) & 0x000F)
#define getTileEntityId(finder, x, y) ((wpath_getTileValue(finder, x, y) & 0xFFF0) >> 4)
#define isEmpty(finder, x, y) (getTileValueType(finder, x, y) == PATH_FINDER_DATA_EMPTY)
#define isStatic(finder, x, y) (getTileValueType(finder, x, y) == PATH_FINDER_DATA_STATIC)
#define isDynamic(finder, x,  y) (getTileValueType(finder, x, y) == PATH_FINDER_DATA_DYNAMIC)
#define isDynamicOfEntity(finder, x, y, id) (getTileEntityId(finder, x, y) == id)

WarPathFinder wpath_initPathFinder(WarContext* context, PathFindingType type, s32 width, s32 height, u16 data[]);
bool wpath_isInside(WarPathFinder finder, s32 x, s32 y);
WarMapPath wpath_findPath(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY);
bool wpath_reRoutePath(WarPathFinder finder, WarMapPath* path, s32 fromIndex, s32 toIndex);
bool wpath_pathExists(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY);
void wpath_freePath(WarMapPath path);

vec2 wpath_findEmptyPosition(WarPathFinder finder, vec2 position);
bool wpath_isPositionAccesible(WarPathFinder finder, vec2 position);
