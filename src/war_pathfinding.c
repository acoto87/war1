typedef struct
{
    s32 x, y;
    s32 level;
    s32 parent;
} WarMapNode;

internal bool equalsMapNode(const WarMapNode node1, const WarMapNode node2)
{
    return node1.x == node2.x && node1.y == node2.y;
}

shlDeclareList(WarMapNodeList, WarMapNode)
shlDefineList(WarMapNodeList, WarMapNode, equalsMapNode, (WarMapNode){0})

internal const s32 dirC = 8;
internal const s32 dirX[] = {  0,  1, 1, 1, 0, -1, -1, -1 };
internal const s32 dirY[] = { -1, -1, 0, 1, 1,  1,  0, -1 };

internal WarMapNode createNode(s32 x, s32 y)
{
    return (WarMapNode){x, y, 0, -1};
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
    setValueTiles(finder, startX, startY, width, height, 0);
}

void setBlockTiles(WarPathFinder finder, s32 startX, s32 startY, s32 width, s32 height)
{
    setValueTiles(finder, startX, startY, width, height, 1);
}

u16 getTileValue(WarPathFinder finder, s32 x, s32 y)
{
    assert(inRange(x, 0, finder.width));
    assert(inRange(y, 0, finder.height));
    return finder.data[y * finder.width + x];
}

void freePath(WarMapPath path)
{
    Vec2ListFree(&path.nodes);
}

WarMapPath findPath(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
    WarMapNodeList nodes;
    WarMapNodeListInit(&nodes);

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
                if (finder.data[yy * finder.width + xx] == 0 || equalsMapNode(newNode, endNode))
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
    Vec2ListInit(&path.nodes);

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

    if (newPath.nodes.count > 0)
    {
        s32 minIndex = min(fromIndex, toIndex);
        s32 maxIndex = max(fromIndex, toIndex);

        // remove the nodes in the range [fromIndex, toIndex] or [toIndex, fromIndex] from current to last remaining nodes of the current path 
        // TODO: make a RemoveRange function for lists
        for(s32 i = maxIndex; i >= minIndex; i--)
            Vec2ListRemoveAt(&path->nodes, i);

        // if a path was found subsitute the portion of the path with the new one
        for(s32 i = 0; i < newPath.nodes.count; i++)
            Vec2ListInsert(&path->nodes, minIndex + i, newPath.nodes.items[i]);

        result = true;
    }

    freePath(newPath);
    return result;
}
