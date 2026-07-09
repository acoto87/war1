#include <assert.h>

#include "war_alloc.h"
#include "war_pathfinder.h"
#include "war_collections.h"

static const s32 wpath_dirC = 8;
static const s32 wpath_dirX[] = {  0,  1, 1, 1, 0, -1, -1, -1 };
static const s32 wpath_dirY[] = { -1, -1, 0, 1, 1,  1,  0, -1 };

typedef struct _WarMapNode
{
    s32 id;     // id of the node
    s32 x, y;   // the coordinates of the node
    s32 level;  // the length of the path from the start to this node
    s32 parent; // the previous node in the path from start to end passing through this node
    s32 gScore; // the cost from the start to this node
    s32 fScore; // the cost from start to end passing through this node
} WarMapNode;

static s32 manhattanDistance(const WarMapNode node1, const WarMapNode node2)
{
    s32 xx = node1.x - node2.x;
    s32 yy = node1.y - node2.y;
    return ABS(xx) + ABS(yy);
}

static s32 nodeDistanceSqr(const WarMapNode node1, const WarMapNode node2)
{
    s32 xx = node1.x - node2.x;
    s32 yy = node1.y - node2.y;
    return xx * xx + yy * yy;
}

static bool equalsMapNode(const WarMapNode node1, const WarMapNode node2)
{
    return node1.x == node2.x && node1.y == node2.y;
}

static s32 compareFScore(const WarMapNode node1, const WarMapNode node2)
{
    return node1.fScore - node2.fScore;
}

static u32 hashMapNode(const s32 key)
{
    return (u32)key;
}

static bool equalsMapNodeId(const s32 key1, const s32 key2)
{
    return key1 == key2;
}

shlDeclareList(WarMapNodeList, WarMapNode)
shlDefineList(WarMapNodeList, WarMapNode)

shlDeclareBinaryHeap(WarMapNodeHeap, WarMapNode)
shlDefineBinaryHeap(WarMapNodeHeap, WarMapNode)

shlDeclareMap(WarMapNodeMap, s32, WarMapNode)
shlDefineMap(WarMapNodeMap, s32, WarMapNode)

static WarMapNode createNode(s32 x, s32 y)
{
    return (WarMapNode){y * MAP_TILES_WIDTH + x, x, y, 0, -1, INT32_MAX, INT32_MAX};
}

void wpath_astar(WarPathFinder* finder, s32 startX, s32 startY, s32 endX, s32 endY, WarMapPath* path)
{
    assert(finder);
    assert(inRange(startX, 0, MAP_TILES_WIDTH));
    assert(inRange(startY, 0, MAP_TILES_HEIGHT));
    assert(inRange(endX, 0, MAP_TILES_WIDTH));
    assert(inRange(endY, 0, MAP_TILES_HEIGHT));
    assert(path);

    TracyCZoneN(ctx, "Astar", 1);

    // The set of currently discovered nodes that are not evaluated yet.
    WarMapNodeHeap openSet;
    WarMapNodeHeapInit(&openSet, wm_frameAllocator(), compareFScore);

    // The set of nodes already evaluated (this could be a simple boolean array to mark the visited nodes)
    WarMapNodeMap closedSet;
    WarMapNodeMapInit(&closedSet, wm_frameAllocator(), hashMapNode, equalsMapNodeId);

    WarMapNode startNode = createNode(startX, startY);
    WarMapNode endNode = createNode(endX, endY);

    // The cost of going from start to start is zero.
    startNode.gScore = 0;

    // The cost of going from start to end is infinity.
    endNode.gScore = INT32_MAX;

    // For the first node, the fScore that value is completely heuristic.
    startNode.fScore = manhattanDistance(startNode, endNode);

    // For the last node, the fScore is 0
    endNode.fScore = 0;

    // Initially, only the start node is known.
    WarMapNodeHeapPush(&openSet, startNode);

    while (openSet.count > 0)
    {
        // the node in openSet having the lowest fScore value
        WarMapNode current = WarMapNodeHeapPop(&openSet);
        WarMapNodeMapSet(&closedSet, current.id, current);

        if (equalsMapNode(current, endNode))
            break;

        for(s32 d = 0; d < wpath_dirC; d++)
        {
            s32 xx = current.x + wpath_dirX[d];
            s32 yy = current.y + wpath_dirY[d];
            if (wpath_isInside(xx, yy))
            {
                // if the neighbor tile is occupied by a static entity,
                // don't consider it so that the unit is able to surround it
                if (wpath_isStatic(finder, xx, yy))
                    continue;

                WarMapNode neighbor = createNode(xx, yy);

                // Ignore the neighbor which is already evaluated.
                if (WarMapNodeMapContains(&closedSet, neighbor.id))
                    continue;

                // The distance from start -> current node -> neighbor
                s32 gScore = current.gScore + 1 /* cost from current to neighbor, can be a little higher for diagonals */;

                // < 0 indicates that this node need to be inserted into the heap
                s32 index = WarMapNodeHeapIndexOf(&openSet, neighbor, equalsMapNode);

                // if the node is already in the heap, check to update its gScore if necessary
                if (index >= 0)
                {
                    neighbor = openSet.items[index];

                    // going from the current node through this neighbor is not the best way, skip it
                    if (gScore >= neighbor.gScore)
                        continue;
                }

                // This path is the best until now. Record it!
                neighbor.parent = current.id;
                neighbor.gScore = gScore;
                neighbor.fScore = neighbor.gScore + manhattanDistance(neighbor, endNode);

                if (index >= 0)
                    WarMapNodeHeapUpdate(&openSet, index, neighbor);
                else
                    WarMapNodeHeapPush(&openSet, neighbor);
            }
        }
    }

    // only process the path if has at least two points
    if (closedSet.count > 1)
    {
        WarMapNode node = {0};

        // if the last node is not in the collection of processed nodes,
        // then the node is unreachable, look for the closest one and go there
        if (!WarMapNodeMapContains(&closedSet, endNode.id))
        {
            s32 minDistanceToEnd = INT32_MAX;
            s32 minDistanceFromStart = INT32_MAX;

            for(s32 k = 0; k < closedSet.capacity; k++)
            {
                if (closedSet.entries[k].active)
                {
                    s32 distanceToEnd = nodeDistanceSqr(endNode, closedSet.entries[k].value);
                    if (distanceToEnd < minDistanceToEnd)
                    {
                        node = closedSet.entries[k].value;
                        minDistanceToEnd = distanceToEnd;
                        minDistanceFromStart = nodeDistanceSqr(startNode, closedSet.entries[k].value);
                    }
                    else if(distanceToEnd == minDistanceToEnd)
                    {
                        s32 distanceFromStart = nodeDistanceSqr(startNode, closedSet.entries[k].value);
                        if (distanceFromStart < minDistanceFromStart)
                        {
                            node = closedSet.entries[k].value;
                            minDistanceToEnd = distanceToEnd;
                            minDistanceFromStart = distanceFromStart;
                        }
                    }
                }
            }
        }
        else
        {
            node = WarMapNodeMapGet(&closedSet, endNode.id);
        }

        path->count = 0;

        while (node.parent >= 0)
        {
            path->nodes[path->count++] = vec2i(node.x, node.y);
            node = WarMapNodeMapGet(&closedSet, node.parent);
        }

        path->nodes[path->count++] = vec2i(startNode.x, startNode.y);

        for(s32 i = 0, j = path->count - 1; i < j; i++, j--)
        {
            vec2 temp = path->nodes[i];
            path->nodes[i] = path->nodes[j];
            path->nodes[j] = temp;
        }
    }

    WarMapNodeHeapFree(&openSet);
    WarMapNodeMapFree(&closedSet);

    TracyCZoneEnd(ctx);
}

void wpath_flowField(WarPathFinder* finder, s32 x, s32 y, WarMapFlowField* flowField)
{
    assert(finder);
    assert(inRange(x, 0, MAP_TILES_WIDTH));
    assert(inRange(y, 0, MAP_TILES_HEIGHT));
    assert(flowField);

    TracyCZoneN(ctx, "FlowField", 1);

    // The set of currently discovered nodes that are not evaluated yet.
    WarMapNodeHeap openSet;
    WarMapNodeHeapInit(&openSet, wm_frameAllocator(), compareFScore);

    // The set of nodes already evaluated (this could be a simple boolean array to mark the visited nodes)
    WarMapNodeMap closedSet;
    WarMapNodeMapInit(&closedSet, wm_frameAllocator(), hashMapNode, equalsMapNodeId);

    WarMapNode startNode = createNode(x, y);

    // The cost of going from start to start is zero.
    startNode.gScore = 0;

    // For the first node, the fScore is the same as the gScore
    startNode.fScore = 0;

    // Initially, only the start node is known.
    WarMapNodeHeapPush(&openSet, startNode);

    while (openSet.count > 0)
    {
        // the node in openSet having the lowest fScore value
        WarMapNode current = WarMapNodeHeapPop(&openSet);
        WarMapNodeMapSet(&closedSet, current.id, current);

        for(s32 d = 0; d < wpath_dirC; d++)
        {
            s32 xx = current.x + wpath_dirX[d];
            s32 yy = current.y + wpath_dirY[d];
            if (wpath_isInside(xx, yy))
            {
                if (!wpath_isStatic(finder, xx, yy))
                {
                    WarMapNode neighbor = createNode(xx, yy);

                    // Ignore the neighbor which is already evaluated.
                    if (WarMapNodeMapContains(&closedSet, neighbor.id))
                        continue;

                    // The cost from start -> current node -> neighbor
                    // cost from current to neighbor, can be a little higher for diagonals
                    s32 gScore = current.gScore + ((d % 2 != 0) ? 141 : 100);

                    // < 0 indicates that this node need to be inserted into the heap
                    s32 index = WarMapNodeHeapIndexOf(&openSet, neighbor, equalsMapNode);

                    // if the node is already in the heap, check to update its gScore if necessary
                    if (index >= 0)
                    {
                        neighbor = openSet.items[index];

                        // going from the current node through this neighbor is not the best way, skip it
                        if (gScore >= neighbor.gScore)
                            continue;
                    }

                    // This path is the best until now. Record it!
                    neighbor.parent = current.id;
                    neighbor.gScore = gScore;
                    neighbor.fScore = gScore;

                    if (index >= 0)
                        WarMapNodeHeapUpdate(&openSet, index, neighbor);
                    else
                        WarMapNodeHeapPush(&openSet, neighbor);
                }
            }
        }
    }

    if (closedSet.count > 1)
    {
        for (s32 k = 0; k < closedSet.capacity; k++)
        {
            if (closedSet.entries[k].active)
            {
                WarMapNode node = closedSet.entries[k].value;
                if (node.parent >= 0)
                {
                    WarMapNode parent = WarMapNodeMapGet(&closedSet, node.parent);
                    s32 dx = parent.x - node.x;
                    s32 dy = parent.y - node.y;

                    for(u8 d = 0; d < wpath_dirC; d++)
                    {
                        if (dx == wpath_dirX[d] && dy == wpath_dirY[d])
                        {
                            flowField->cost[node.y * MAP_TILES_WIDTH + node.x] = node.gScore;
                            flowField->dirs[node.y * MAP_TILES_WIDTH + node.x] = d;
                            break;
                        }
                    }
                }
            }
        }
    }

    WarMapNodeHeapFree(&openSet);
    WarMapNodeMapFree(&closedSet);

    TracyCZoneEnd(ctx);
}

WarPathFinder wpath_initPathFinder(u16 data[MAP_TILES_WIDTH * MAP_TILES_HEIGHT])
{
    WarPathFinder finder = (WarPathFinder){0};
    memset(finder.data, 0, sizeof(finder.data));

    // 128 -> wood, 64 -> water, 16 -> bridge, 0 -> empty
    for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        switch (data[i])
        {
            case 128:
            case 64:
                finder.data[i] = PATH_FINDER_DATA_STATIC;
                break;

            default:
                finder.data[i] = PATH_FINDER_DATA_EMPTY;
                break;
        }
    }

    return finder;
}

bool wpath_isInside(s32 x, s32 y)
{
    return inRange(x, 0, MAP_TILES_WIDTH) && inRange(y, 0, MAP_TILES_HEIGHT);
}

WarMapFlowField* wpath_computeFlowField(WarPathFinder* finder, s32 x, s32 y)
{
    assert(inRange(x, 0, MAP_TILES_WIDTH));
    assert(inRange(y, 0, MAP_TILES_HEIGHT));

    TracyCZoneN(ctx, "ComputeFlowField", 1);

    s32 fieldIndex = y * MAP_TILES_WIDTH + x;
    if (fieldIndex < 0 || fieldIndex >= MAP_TILES_WIDTH * MAP_TILES_HEIGHT)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarMapFlowField* flowField = finder->fields[fieldIndex];
    if (!flowField)
    {
        flowField = (WarMapFlowField*)wm_alloc(sizeof(WarMapFlowField));
        finder->fields[fieldIndex] = flowField;
    }

    for (s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        flowField->cost[i] = INT32_MAX;
        flowField->dirs[i] = WAR_DIRECTION_COUNT;
    }

    wpath_flowField(finder, x, y, flowField);

    TracyCZoneEnd(ctx);

    return flowField;
}

WarMapFlowField* wpath_getFlowField(WarPathFinder* finder, s32 x, s32 y)
{
    assert(inRange(x, 0, MAP_TILES_WIDTH));
    assert(inRange(y, 0, MAP_TILES_HEIGHT));

    s32 fieldIndex = y * MAP_TILES_WIDTH + x;

    if (fieldIndex < 0 || fieldIndex >= MAP_TILES_WIDTH * MAP_TILES_HEIGHT)
    {
        return NULL;
    }

    return finder->fields[fieldIndex];
}

WarMapFlowField* wpath_ensureFlowField(WarPathFinder* finder, s32 x, s32 y)
{
    assert(inRange(x, 0, MAP_TILES_WIDTH));
    assert(inRange(y, 0, MAP_TILES_HEIGHT));

    s32 fieldIndex = y * MAP_TILES_WIDTH + x;

    if (fieldIndex < 0 || fieldIndex >= MAP_TILES_WIDTH * MAP_TILES_HEIGHT)
    {
        return NULL;
    }

    if (!finder->fields[fieldIndex])
    {
        return wpath_computeFlowField(finder, x, y);
    }

    return finder->fields[fieldIndex];
}

vec2 wpath_flowFieldSample(WarMapFlowField* flowField, s32 x, s32 y)
{
    assert(inRange(x, 0, MAP_TILES_WIDTH));
    assert(inRange(y, 0, MAP_TILES_HEIGHT));

    s32 fieldIndex = y * MAP_TILES_WIDTH + x;

    if (fieldIndex < 0 || fieldIndex >= MAP_TILES_WIDTH * MAP_TILES_HEIGHT)
    {
        return vec2i(0, 0);
    }

    u8 dir = flowField->dirs[fieldIndex];
    if (dir < 0 || dir >= WAR_DIRECTION_COUNT)
    {
        return vec2i(0, 0);
    }

    // TODO: Do bilinear interpolation to smooth the flow field, so the unit doesn't move in a grid-like way
    vec2 result = vec2i(wpath_dirX[dir], wpath_dirY[dir]);
    return vec2_normalize(result);
}

vec2 wpath_findEmptyTile(WarPathFinder* finder, vec2 tile)
{
    if (wpath_isEmpty(finder, (s32)tile.x, (s32)tile.y))
        return tile;

    // TODO: BFS, see if we can statically allocate this list
    Vec2List positions;
    Vec2ListInit(&positions, wm_frameAllocator());
    Vec2ListAdd(&positions, tile);

    for(s32 i = 0; i < positions.count; i++)
    {
        vec2 currentPosition = positions.items[i];
        if (wpath_isEmpty(finder, (s32)currentPosition.x, (s32)currentPosition.y))
        {
            tile = currentPosition;
            break;
        }

        for(s32 d = 0; d < wpath_dirC; d++)
        {
            s32 xx = (s32)currentPosition.x + wpath_dirX[d];
            s32 yy = (s32)currentPosition.y + wpath_dirY[d];
            if (inRange(xx, 0, MAP_TILES_WIDTH) && inRange(yy, 0, MAP_TILES_HEIGHT))
            {
                vec2 newPosition = vec2i(xx, yy);
                if (!Vec2ListContains(&positions, newPosition, equalsVec2))
                    Vec2ListAdd(&positions, newPosition);
            }
        }
    }

    Vec2ListFree(&positions);

    return tile;
}

bool wpath_isTileAccesible(WarPathFinder* finder, vec2 tile)
{
    for(s32 d = 0; d < wpath_dirC; d++)
    {
        s32 xx = (s32)tile.x + wpath_dirX[d];
        s32 yy = (s32)tile.y + wpath_dirY[d];
        if (inRange(xx, 0, MAP_TILES_WIDTH) && inRange(yy, 0, MAP_TILES_HEIGHT))
        {
            if (wpath_isEmpty(finder, xx, yy))
                return true;
        }
    }

    return false;
}

u16 wpath_getTileValue(WarPathFinder* finder, s32 x, s32 y)
{
    assert(inRange(x, 0, MAP_TILES_WIDTH));
    assert(inRange(y, 0, MAP_TILES_HEIGHT));
    return finder->data[y * MAP_TILES_WIDTH + x];
}

void wpath_setTilesValue(WarPathFinder* finder, s32 startX, s32 startY, s32 width, s32 height, u16 value)
{
    assert(finder);

    if (!inRange(startX, 0, MAP_TILES_WIDTH) || !inRange(startY, 0, MAP_TILES_HEIGHT))
        return;

    if (startX + width >= MAP_TILES_WIDTH)
        width = MAP_TILES_WIDTH - startX;

    if (startY + height >= MAP_TILES_HEIGHT)
        height = MAP_TILES_HEIGHT - startY;

    s32 endX = startX + width;
    s32 endY = startY + height;

    for(s32 y = startY; y < endY; y++)
    {
        for(s32 x = startX; x < endX; x++)
        {
            finder->data[y * MAP_TILES_WIDTH + x] = value;
        }
    }
}

void wpath_setFreeTiles(WarPathFinder* finder, s32 startX, s32 startY, s32 width, s32 height)
{
    wpath_setTilesValue(finder, startX, startY, width, height, PATH_FINDER_DATA_EMPTY);
}

void wpath_setStaticEntity(WarPathFinder* finder, s32 startX, s32 startY, s32 width, s32 height, WarEntityId id)
{
    wpath_setTilesValue(finder, startX, startY, width, height, (id << 4) | PATH_FINDER_DATA_STATIC);
}

WarPathFinderDataType wpath_getTileValueType(WarPathFinder* finder, s32 x, s32 y)
{
    return (WarPathFinderDataType)(wpath_getTileValue(finder, x, y) & 0x000F);
}

WarEntityId wpath_getTileEntityId(WarPathFinder* finder, s32 x, s32 y)
{
    return (WarEntityId)((wpath_getTileValue(finder, x, y) & 0xFFF0) >> 4);
}

bool wpath_isEmpty(WarPathFinder* finder, s32 x, s32 y)
{
    return wpath_getTileValueType(finder, x, y) == PATH_FINDER_DATA_EMPTY;
}

bool wpath_isStatic(WarPathFinder* finder, s32 x, s32 y)
{
    return wpath_getTileValueType(finder, x, y) == PATH_FINDER_DATA_STATIC;
}
