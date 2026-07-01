#include <assert.h>

#include "war_pathfinder.h"
#include "war_collections.h"

static const s32 wpath_dirC = 8;
static const s32 wpath_dirX[] = {  0,  1, 1, 1, 0, -1, -1, -1 };
static const s32 wpath_dirY[] = { -1, -1, 0, 1, 1,  1,  0, -1 };

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

shlDefineList(WarMapNodeList, WarMapNode)
shlDefineBinaryHeap(WarMapNodeHeap, WarMapNode)
shlDefineMap(WarMapNodeMap, s32, WarMapNode)

static WarMapNode createNode(s32 x, s32 y)
{
    return (WarMapNode){y * MAP_TILES_WIDTH + x, x, y, 0, -1, INT32_MAX, INT32_MAX};
}

static WarMapPath bfs(WarPathFinder* finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
    WarMapNodeList nodes;
    WarMapNodeListInit(&nodes, wm_frameAllocator());

    WarMapNode startNode = createNode(startX, startY);
    WarMapNode endNode = createNode(endX, endY);

    WarMapNodeListAdd(&nodes, startNode);

    s32 i;
    for(i = 0; i < nodes.count; i++)
    {
        WarMapNode node = nodes.items[i];
        if (equalsMapNode(node, endNode))
            break;

        for(s32 d = 0; d < wpath_dirC; d++)
        {
            s32 xx = node.x + wpath_dirX[d];
            s32 yy = node.y + wpath_dirY[d];
            if (wpath_isInside(xx, yy))
            {
                WarMapNode newNode = createNode(xx, yy);
                if (isEmpty(finder, xx, yy) || equalsMapNode(newNode, endNode))
                {
                    if (!WarMapNodeListContains(&nodes, newNode, equalsMapNode))
                    {
                        newNode.parent = i;
                        newNode.level = node.level + 1;
                        WarMapNodeListAdd(&nodes, newNode);
                    }
                }
            }
        }
    }

    WarMapPath path = (WarMapPath){0};
    Vec2ListInit(&path.nodes, wm_globalAllocator());

    if (i < nodes.count)
    {
        WarMapNode node = nodes.items[i];
        Vec2ListAdd(&path.nodes, vec2i(node.x, node.y));

        while (node.parent >= 0)
        {
            node = nodes.items[node.parent];
            Vec2ListAdd(&path.nodes, vec2i(node.x, node.y));
        }

        Vec2ListReverse(&path.nodes);
    }

    WarMapNodeListFree(&nodes);

    return path;
}

static WarMapPath astar(WarPathFinder* finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
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
                // if the neighbor position is occupied by a static entity,
                // don't consider it so that the unit is able to surround it
                if (isStatic(finder, xx, yy))
                    continue;

                WarMapNode neighbor = createNode(xx, yy);

                // if the neighbor position is a occupied by a dynamic entity (another unit moving),
                // there is a chance that when the unit gets there the position is empty
                // but only consider it in the path when that position is far away from the start,
                // because when that position is close enough, the risk of overlaping units is greater
                if (isDynamic(finder, xx, yy))
                {
                    s32 distance = nodeDistanceSqr(startNode, neighbor);
                    if (distance < DISTANCE_SQR_AVOID_DYNAMIC_POSITIONS)
                        continue;
                }

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

    WarMapPath path = (WarMapPath){0};
    Vec2ListInit(&path.nodes, wm_globalAllocator());

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

        while (node.parent >= 0)
        {
            Vec2ListAdd(&path.nodes, vec2i(node.x, node.y));
            node = WarMapNodeMapGet(&closedSet, node.parent);
        }

        Vec2ListAdd(&path.nodes, vec2i(node.x, node.y));
        Vec2ListReverse(&path.nodes);
    }

    WarMapNodeHeapFree(&openSet);
    WarMapNodeMapFree(&closedSet);

    TracyCZoneEnd(ctx);
    return path;
}

static void computeFlowField(WarPathFinder* finder, s32 x, s32 y, WarMapFlowField* flowField)
{
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
                if (isEmpty(finder, xx, yy))
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
}

WarPathFinder wpath_initPathFinder(PathFindingType type, u16 data[MAP_TILES_WIDTH * MAP_TILES_HEIGHT])
{
    WarPathFinder finder = (WarPathFinder){0};
    finder.type = type;
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

WarMapPath wpath_findPath(WarPathFinder* finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
    TracyCZoneN(ctx, "FindPath", 1);

    WarMapPath path;
    switch (finder->type)
    {
        case PATH_FINDING_BFS: path = bfs(finder, startX, startY, endX, endY); break;
        case PATH_FINDING_ASTAR: path = astar(finder, startX, startY, endX, endY); break;
        default:
        {
            logWarning("Unkown path finding type %d, defaulting to %d", finder->type, PATH_FINDING_ASTAR);
            path = astar(finder, startX, startY, endX, endY);
            break;
        }
    }

    TracyCZoneEnd(ctx);
    return path;
}

WarMapFlowField wpath_computeFlowField(WarPathFinder* finder, s32 x, s32 y)
{
    TracyCZoneN(ctx, "ComputeFlowField", 1);

    WarMapFlowField flowField = (WarMapFlowField){0};

    for (s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        flowField.cost[i] = INT32_MAX;
        flowField.dirs[i] = WAR_DIRECTION_COUNT;
    }

    computeFlowField(finder, x, y, &flowField);

    TracyCZoneEnd(ctx);
    return flowField;
}

bool wpath_reRoutePath(WarPathFinder* finder, WarMapPath* path, s32 fromIndex, s32 toIndex)
{
    assert(inRange(fromIndex, 0, path->nodes.count));
    assert(inRange(toIndex, 0, path->nodes.count));
    assert(fromIndex != toIndex);

    bool result = false;

    vec2 fromNode = path->nodes.items[fromIndex];
    vec2 toNode = path->nodes.items[toIndex];

    // find a new path from the current position to the destination
    WarMapPath newPath = wpath_findPath(finder, (s32)fromNode.x, (s32)fromNode.y, (s32)toNode.x, (s32)toNode.y);

    if (newPath.nodes.count > 1)
    {
        s32 minIndex = MIN(fromIndex, toIndex);
        s32 maxIndex = MAX(fromIndex, toIndex);

        // remove the nodes in the range [fromIndex, toIndex] or [toIndex, fromIndex] from current to last remaining nodes of the current path
        Vec2ListRemoveAtRange(&path->nodes, minIndex, maxIndex - minIndex + 1);

        // if a path was found subsitute the portion of the path with the new one
        Vec2ListInsertRange(&path->nodes, minIndex, newPath.nodes.count, newPath.nodes.items);

        result = true;
    }

    Vec2ListFree(&newPath.nodes);

    return result;
}

vec2 wpath_findEmptyPosition(WarPathFinder* finder, vec2 position)
{
    if (isEmpty(finder, (s32)position.x, (s32)position.y))
        return position;

    Vec2List positions;
    Vec2ListInit(&positions, wm_frameAllocator());
    Vec2ListAdd(&positions, position);

    for(s32 i = 0; i < positions.count; i++)
    {
        vec2 currentPosition = positions.items[i];
        if (isEmpty(finder, (s32)currentPosition.x, (s32)currentPosition.y))
        {
            position = currentPosition;
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

    return position;
}

bool wpath_isPositionAccesible(WarPathFinder* finder, vec2 position)
{
    for(s32 d = 0; d < wpath_dirC; d++)
    {
        s32 xx = (s32)position.x + wpath_dirX[d];
        s32 yy = (s32)position.y + wpath_dirY[d];
        if (inRange(xx, 0, MAP_TILES_WIDTH) && inRange(yy, 0, MAP_TILES_HEIGHT))
        {
            if (isEmpty(finder, xx, yy))
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
