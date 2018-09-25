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

WarMapPath findPath(WarPathFinder finder, s32 startX, s32 startY, s32 endX, s32 endY)
{
    const s32 dirC = 8;
    const s32 dirX[] = {  0,  1, 1, 1, 0, -1, -1, -1 };
    const s32 dirY[] = { -1, -1, 0, 1, 1,  1,  0, -1 };

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
        {
            break;
        }

        for(s32 d = 0; d < dirC; d++)
        {
            s32 xx = node.x + dirX[d];
            s32 yy = node.y + dirY[d];
            WarMapNode newNode = createNode(xx, yy);
            if (inRange(newNode.x, 0, finder.width) && inRange(newNode.y, 0, finder.height))
            {
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

    if (i < nodes.count)
    {
        WarMapNode node = nodes.items[i];

        path.count = node.level;
        path.nodes = (WarMapNode*)xcalloc(path.count, sizeof(WarMapNode));
        
        for(s32 i = path.count - 1; i >= 0; i--)
        {
            path.nodes[i] = vec2i(node.x, node.y);

            if (node.parent >= 0)
                node = nodes.items[node.parent];
        }
    }

    WarMapNodeListFree(&nodes);

    return path;
}