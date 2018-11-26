#include "war_pathfinder.h"

typedef struct
{
    // id of the node
    s32 id;

    // the coordinates of the node
    s32 x, y;

    // the length of the path from the start to this node
    s32 level;

    // the previous node in the path from start to end passing through this node
    s32 parent;

    // the cost from the start to this node
    s32 gScore;

    // the cost from start to end passing through this node
    s32 fScore;
} WarMapNode;

#define WarMapNodeEmpty (WarMapNode){0}

internal bool equalsMapNode(const WarMapNode node1, const WarMapNode node2)
{
    return node1.x == node2.x && node1.y == node2.y;
}

internal s32 compareFScore(const WarMapNode node1, const WarMapNode node2)
{
    return node1.fScore - node2.fScore;
}

internal s32 compareGScore(const WarMapNode node1, const WarMapNode node2)
{
    return node1.gScore - node2.gScore;
}

internal s32 manhattanDistance(const WarMapNode node1, const WarMapNode node2)
{
    return abs(node1.x - node2.x) + abs(node1.y - node2.y);
}

internal s32 nodeDistanceSqr(const WarMapNode node1, const WarMapNode node2)
{
    s32 xx = node1.x - node2.x;
    s32 yy = node1.y - node2.y;
    return xx * xx + yy * yy;
}

internal s32 hashMapNode(const s32 key)
{
    return key;
}

internal bool equalsMapNodeId(const s32 key1, const s32 key2)
{
    return key1 == key2;
}

shlDeclareList(WarMapNodeList, WarMapNode)
shlDefineList(WarMapNodeList, WarMapNode)

shlDeclareBinaryHeap(WarMapNodeHeap, WarMapNode)
shlDefineBinaryHeap(WarMapNodeHeap, WarMapNode)

shlDeclareMap(WarMapNodeMap, s32, WarMapNode)
shlDefineMap(WarMapNodeMap, s32, WarMapNode)

#define WarMapNodeListDefaultOptions (WarMapNodeListOptions){WarMapNodeEmpty, equalsMapNode, NULL}
#define WarMapNodeHeapDefaultOptions (WarMapNodeHeapOptions){WarMapNodeEmpty, equalsMapNode, compareFScore, NULL}
#define WarMapNodeMapDefaultOptions (WarMapNodeMapOptions){WarMapNodeEmpty, hashMapNode, equalsMapNodeId, NULL}

internal WarMapNode createNode(WarPathFinder finder, s32 x, s32 y)
{
    return (WarMapNode){y * finder.width + x, x, y, 0, -1, INT32_MAX, INT32_MAX};
}

u16 getTileValue(WarPathFinder finder, s32 x, s32 y)
{
    assert(inRange(x, 0, finder.width));
    assert(inRange(y, 0, finder.height));
    return finder.data[y * finder.width + x];
}

void setTilesValue(WarPathFinder finder, s32 startX, s32 startY, s32 width, s32 height, u16 value)
{
    if (!inRange(startX, 0, finder.width) || !inRange(startY, 0, finder.height))
        return;

    if (startX + width >= finder.width)
        width = finder.width - startX;

    if (startY + height >= finder.height)
        height = finder.height - startY;

    s32 endX = startX + width;
    s32 endY = startY + height;

    for(s32 y = startY; y < endY; y++)
    {
        for(s32 x = startX; x < endX; x++)
        {
            finder.data[y * finder.width + x] = value;
        }
    }
}

internal WarMapPath bfs(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
    WarMapNodeList nodes;
    WarMapNodeListInit(&nodes, WarMapNodeListDefaultOptions);

    WarMapNode startNode = createNode(finder, startX, startY);
    WarMapNode endNode = createNode(finder, endX, endY);

    WarMapNodeListAdd(&nodes, startNode);
    
    s32 i;
    for(i = 0; i < nodes.count; i++)
    {
        WarMapNode node = nodes.items[i];
        if (equalsMapNode(node, endNode))
            break;

        for(s32 d = 0; d < dirC; d++)
        {
            s32 xx = node.x + dirX[d];
            s32 yy = node.y + dirY[d];
            if (isInside(finder, xx, yy))
            {
                WarMapNode newNode = createNode(finder, xx, yy);
                if (isEmpty(finder, xx, yy) || equalsMapNode(newNode, endNode))
                {
                    if (!WarMapNodeListContains(&nodes, newNode))
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
    vec2ListInit(&path.nodes, vec2ListDefaultOptions);

    if (i < nodes.count)
    {
        WarMapNode node = nodes.items[i];
        vec2ListAdd(&path.nodes, vec2i(node.x, node.y));
        
        while (node.parent >= 0)
        {
            node = nodes.items[node.parent];
            vec2ListAdd(&path.nodes, vec2i(node.x, node.y));
        }

        vec2ListReverse(&path.nodes);
    }

    WarMapNodeListFree(&nodes);

    return path;
}

internal WarMapPath astar(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
    // The set of currently discovered nodes that are not evaluated yet.
    WarMapNodeHeap openSet;
    WarMapNodeHeapInit(&openSet, WarMapNodeHeapDefaultOptions);

    // The set of nodes already evaluated (this could be a simple boolean array to mark the visited nodes)
    WarMapNodeMap closedSet;
    WarMapNodeMapInit(&closedSet, WarMapNodeMapDefaultOptions);

    WarMapNode startNode = createNode(finder, startX, startY);
    WarMapNode endNode = createNode(finder, endX, endY);

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

        for(s32 d = 0; d < dirC; d++)
        {
            s32 xx = current.x + dirX[d];
            s32 yy = current.y + dirY[d];
            if (isInside(finder, xx, yy))
            {
                // if the neighbor position is occupied by a static entity, 
                // don't consider it so that the unit is able to surround it
                if (isStatic(finder, xx, yy))
                    continue;

                WarMapNode neighbor = createNode(finder, xx, yy);

                // if the neighbor position is a occupied by a dynamic entity (another unit moving),
                // there is a chance that when the unit gets there the position is empty
                // but only consider it in the path when that position is far away from the start,
                // because when that position is close enough, the risk of overlaping units is greater
                if (isDynamic(finder, xx, yy))
                {
                    f32 distance = nodeDistanceSqr(startNode, neighbor);
                    if (distance < DISTANCE_SQR_AVOID_DYNAMIC_POSITIONS)
                        continue;
                }

                // Ignore the neighbor which is already evaluated.
                if (WarMapNodeMapContains(&closedSet, neighbor.id))
                    continue;

                // The distance from start -> current node -> neighbor
                s32 gScore = current.gScore + 1 /* cost from current to neighbor, can be a little higher for diagonals */;

                // < 0 indicates that this node need to be inserted into the heap
                s32 index = WarMapNodeHeapIndexOf(&openSet, neighbor);

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
    vec2ListInit(&path.nodes, vec2ListDefaultOptions);

    // only process the path if has at least two points
    if (closedSet.count > 1)
    {
        WarMapNode node;

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
            vec2ListAdd(&path.nodes, vec2i(node.x, node.y));
            node = WarMapNodeMapGet(&closedSet, node.parent);
        }

        vec2ListAdd(&path.nodes, vec2i(node.x, node.y));
        vec2ListReverse(&path.nodes);
    }

    WarMapNodeHeapFree(&openSet);
    WarMapNodeMapFree(&closedSet);

    return path;
}

WarPathFinder initPathFinder(PathFindingType type, s32 width, s32 height, u16 data[])
{
    WarPathFinder finder = (WarPathFinder){0};
    finder.type = type;
    finder.width = width;
    finder.height = height;
    finder.data = (u16*)xcalloc(width * height, sizeof(u16));

    // 128 -> wood, 64 -> water, 16 -> bridge, 0 -> empty
    for(s32 i = 0; i < width * height; i++)
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

bool isInside(WarPathFinder finder, s32 x, s32 y)
{
    return inRange(x, 0, finder.width) && inRange(y, 0, finder.height);
}

WarMapPath findPath(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
    switch (finder.type)
    {
        case PATH_FINDING_BFS: return bfs(finder, startX, startY, endX, endY);
        case PATH_FINDING_ASTAR: return astar(finder, startX, startY, endX, endY);
        default:
        {
            logWarning("Unkown path finding type %d, defaulting to %d", finder.type, PATH_FINDING_ASTAR);
            return astar(finder, startX, startY, endX, endY);
        }
    }
}

bool reRoutePath(WarPathFinder finder, WarMapPath* path, s32 fromIndex, s32 toIndex)
{
    assert(inRange(fromIndex, 0, path->nodes.count));
    assert(inRange(toIndex, 0, path->nodes.count));
    assert(fromIndex != toIndex);

    bool result = false;

    vec2 fromNode = path->nodes.items[fromIndex];
    vec2 toNode = path->nodes.items[toIndex];

    // find a new path from the current position to the destination
    WarMapPath newPath = findPath(finder, (s32)fromNode.x, (s32)fromNode.y, (s32)toNode.x, (s32)toNode.y);

    if (newPath.nodes.count > 1)
    {
        s32 minIndex = min(fromIndex, toIndex);
        s32 maxIndex = max(fromIndex, toIndex);

        // remove the nodes in the range [fromIndex, toIndex] or [toIndex, fromIndex] from current to last remaining nodes of the current path 
        vec2ListRemoveAtRange(&path->nodes, minIndex, maxIndex - minIndex + 1);

        // if a path was found subsitute the portion of the path with the new one
        vec2ListInsertRange(&path->nodes, minIndex, newPath.nodes.count, newPath.nodes.items);

        result = true;
    }

    freePath(newPath);
    return result;
}

bool pathExists(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
    WarMapPath path = findPath(finder, startX, startY, endX, endY);
    bool result = path.nodes.count >= 2;
    freePath(path);
    return result;
}

void freePath(WarMapPath path)
{
    vec2ListFree(&path.nodes);
}

vec2 findEmptyPosition(WarPathFinder finder, vec2 position)
{
    if (isEmpty(finder, (s32)position.x, (s32)position.y))
        return position;

    vec2List positions;
    vec2ListInit(&positions, vec2ListDefaultOptions);
    vec2ListAdd(&positions, position);
    
    for(s32 i = 0; i < positions.count; i++)
    {
        vec2 currentPosition = positions.items[i];
        if (isEmpty(finder, (s32)currentPosition.x, (s32)currentPosition.y))
        {
            position = currentPosition;
            break;
        }

        for(s32 d = 0; d < dirC; d++)
        {
            s32 xx = currentPosition.x + dirX[d];
            s32 yy = currentPosition.y + dirY[d];
            if (inRange(xx, 0, finder.width) && inRange(yy, 0, finder.height))
            {
                vec2 newPosition = vec2i(xx, yy);
                if (!vec2ListContains(&positions, newPosition))
                    vec2ListAdd(&positions, newPosition);
            }
        }
    }

    vec2ListFree(&positions);

    return position;
}

bool isPositionAccesible(WarPathFinder finder, vec2 position)
{
    for(s32 d = 0; d < dirC; d++)
    {
        s32 xx = (s32)position.x + dirX[d];
        s32 yy = (s32)position.y + dirY[d];
        if (inRange(xx, 0, finder.width) && inRange(yy, 0, finder.height))
        {
            if (isEmpty(finder, xx, yy))
                return true;
        }
    }

    return false;
}
