inline bool isEmpty(WarPathFinder finder, s32 x, s32 y)
{
    assert(inRange(x, 0, finder.width));
    assert(inRange(y, 0, finder.height));
    return finder.data[y * finder.width + x] == PATH_FINDER_DATA_EMPTY;
}

inline bool isStatic(WarPathFinder finder, s32 x, s32 y)
{
    assert(inRange(x, 0, finder.width));
    assert(inRange(y, 0, finder.height));
    return finder.data[y * finder.width + x] == PATH_FINDER_DATA_STATIC;
}

inline bool isDynamic(WarPathFinder finder, s32 x, s32 y)
{
    assert(inRange(x, 0, finder.width));
    assert(inRange(y, 0, finder.height));
    u16 value = finder.data[y * finder.width + x];
    return value > PATH_FINDER_DATA_EMPTY && value < PATH_FINDER_DATA_STATIC;
}

inline bool isDynamicOrEntity(WarPathFinder finder, s32 x, s32 y, WarEntityId id)
{
    assert(inRange(x, 0, finder.width));
    assert(inRange(y, 0, finder.height));
    return finder.data[y * finder.width + x] == id;
}

inline WarMapPath clonePath(WarMapPath path)
{
    WarMapPath clone = (WarMapPath){0};
    vec2ListInit(&clone.nodes, vec2ListDefaultOptions);
    vec2ListAddRange(&clone.nodes, path.nodes.count, path.nodes.items);
    return clone;
}

typedef struct
{
    s32 x, y; // the coordinates of the node
    s32 level; // the length of the path from the start to this node
    s32 parent; // the previous node in the path from start to end passing through this node
    s32 gScore; // the cost from the start to this node
    s32 fScore; // the cost from start to end passing through this node
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

shlDeclareList(WarMapNodeList, WarMapNode)
shlDefineList(WarMapNodeList, WarMapNode)

shlDeclareBinaryHeap(WarMapNodeHeap, WarMapNode)
shlDefineBinaryHeap(WarMapNodeHeap, WarMapNode)

#define WarMapNodeListDefaultOptions (WarMapNodeListOptions){WarMapNodeEmpty, equalsMapNode, NULL}
#define WarMapNodeHeapDefaultOptions (WarMapNodeHeapOptions){WarMapNodeEmpty, equalsMapNode, compareFScore, NULL}

internal const s32 dirC = 8;
internal const s32 dirX[] = {  0,  1, 1, 1, 0, -1, -1, -1 };
internal const s32 dirY[] = { -1, -1, 0, 1, 1,  1,  0, -1 };

internal WarMapNode createNode(s32 x, s32 y)
{
    return (WarMapNode){x, y, 0, -1, INT32_MAX, INT32_MAX};
}

WarPathFinder initPathFinder(PathFindingType type, s32 width, s32 height, u16 data[])
{
    WarPathFinder finder = (WarPathFinder){0};
    finder.type = type;
    finder.width = width;
    finder.height = height;
    finder.data = (u16*)xcalloc(width * height, sizeof(u16));
    memcpy(finder.data, data, width * height * sizeof(u16));
    return finder;
}

internal void setValueTiles(WarPathFinder finder, s32 startX, s32 startY, s32 width, s32 height, u16 value)
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

void setFreeTiles(WarPathFinder finder, s32 startX, s32 startY, s32 width, s32 height)
{
    setValueTiles(finder, startX, startY, width, height, PATH_FINDER_DATA_EMPTY);
}

void setDynamicEntity(WarPathFinder finder, s32 startX, s32 startY, s32 width, s32 height, WarEntityId id)
{
    setValueTiles(finder, startX, startY, width, height, id);
}

void setStaticEntity(WarPathFinder finder, s32 startX, s32 startY, s32 width, s32 height)
{
    setValueTiles(finder, startX, startY, width, height, PATH_FINDER_DATA_STATIC);
}

void freePath(WarMapPath path)
{
    vec2ListFree(&path.nodes);
}

internal WarMapPath bfs(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
    WarMapNodeList nodes;
    WarMapNodeListInit(&nodes, WarMapNodeListDefaultOptions);

    WarMapNode startNode = createNode(startX, startY);
    WarMapNode endNode = createNode(endX, endY);

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
            if (inRange(xx, 0, finder.width) && inRange(yy, 0, finder.height))
            {
                WarMapNode newNode = createNode(xx, yy);
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

    // The set of nodes already evaluated
    WarMapNodeList closedSet;
    WarMapNodeListInit(&closedSet, WarMapNodeListDefaultOptions);

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
        WarMapNodeListAdd(&closedSet, current);

        if (equalsMapNode(current, endNode))
            break;

        for(s32 d = 0; d < dirC; d++)
        {
            s32 xx = current.x + dirX[d];
            s32 yy = current.y + dirY[d];
            if (inRange(xx, 0, finder.width) && inRange(yy, 0, finder.height))
            {
                WarMapNode neighbor = createNode(xx, yy);
                if (isStatic(finder, xx, yy))
                    continue;

                // Ignore the neighbor which is already evaluated.
                // OPTIMIZE
                if (WarMapNodeListContains(&closedSet, neighbor))
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
                neighbor.parent = closedSet.count - 1;
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
        s32 index = closedSet.count - 1;
        WarMapNode node = closedSet.items[index];

        // if the last node is not the end position, look for the most closest one and go there
        if (!equalsMapNode(node, endNode))
        {
            s32 minDistanceToEnd = nodeDistanceSqr(endNode, node);
            s32 minDistanceFromStart = nodeDistanceSqr(startNode, node);
            for(s32 k = index - 1; k >= 0; k--)
            {
                s32 distanceToEnd = nodeDistanceSqr(endNode, closedSet.items[k]);
                if (distanceToEnd < minDistanceToEnd)
                {
                    minDistanceToEnd = distanceToEnd;
                    minDistanceFromStart = nodeDistanceSqr(startNode, closedSet.items[k]);
                    index = k;
                }
                else if(distanceToEnd == minDistanceToEnd)
                {
                    s32 distanceFromStart = nodeDistanceSqr(startNode, closedSet.items[k]);
                    if (distanceFromStart < minDistanceFromStart)
                    {
                        minDistanceToEnd = distanceToEnd;
                        minDistanceFromStart = distanceFromStart;
                        index = k;
                    }
                }
            }

            node = closedSet.items[index];
        }

        while (index >= 0)
        {
            node = closedSet.items[index];
            vec2ListAdd(&path.nodes, vec2i(node.x, node.y));

            index = node.parent;
        }

        vec2ListReverse(&path.nodes);
    }

    WarMapNodeHeapFree(&openSet);
    WarMapNodeListFree(&closedSet);

    return path;
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
