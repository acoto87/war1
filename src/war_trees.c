bool hasTreeAtPosition(WarEntity* forest, s32 x, s32 y)
{
    WarTreeList* trees = &forest->forest.trees;
    for (s32 i = 0; i < trees->count; i++)
    {
        WarTree* tree = &trees->items[i];
        if (tree->tilex == x && tree->tiley == y)
            return true;
    }

    return false;
}

WarTree* getTreeAtPosition(WarEntity* forest, s32 x, s32 y)
{
    WarTreeList* trees = &forest->forest.trees;
    for (s32 i = 0; i < trees->count; i++)
    {
        WarTree* tree = &trees->items[i];
        if (tree->tilex == x && tree->tiley == y)
            return tree;
    }

    return NULL;
}

void determineTreeTiles(WarContext* context, WarEntity* forest)
{
    assert(forest);
    assert(forest->type == WAR_ENTITY_TYPE_FOREST);

    WarMap* map = context->map;

    WarTreeList* trees = &forest->forest.trees;

    const s32 dirC = 8;
    const s32 dirX[] = { -1,  0,  1, 1, 1, 0, -1, -1 };
    const s32 dirY[] = { -1, -1, -1, 0, 1, 1,  1,  0 };

    s32List invalidTrees;
    s32ListInit(&invalidTrees, s32ListDefaultOptions);

    for (s32 i = 0; i < trees->count; i++)
    {
        WarTree* ti = &trees->items[i];
        if (ti->type == WAR_TREE_CHOPPED)
            continue;

        s32 index = 0;

        for (s32 d = 0; d < dirC; d++)
        {
            s32 xx = ti->tilex + dirX[d];
            s32 yy = ti->tiley + dirY[d];
            if (isInside(map->finder, xx, yy))
            {
                WarEntityId entityId = getTileEntityId(map->finder, xx, yy);
                WarEntity* entity = findEntity(context, entityId);
                if (entity && entity->type == WAR_ENTITY_TYPE_FOREST)
                {
                    WarTree* tree = getTreeAtPosition(entity, xx, yy);
                    if (tree && tree->amount > 0)
                        index = index | (1 << d);
                }
            }
            else
            {
                index = index | (1 << d);
            }
        }

        ti->type = treeTileTypeMap[index];

        if (ti->type == WAR_TREE_NONE)
            s32ListAdd(&invalidTrees, i);
    }

    for (s32 i = invalidTrees.count - 1; i >= 0; i--)
        WarTreeListRemoveAt(trees, invalidTrees.items[i]);

    s32ListFree(&invalidTrees);
}

void determineAllTreeTiles(WarContext* context)
{
    WarEntityList* forests = getEntitiesOfType(context, WAR_ENTITY_TYPE_FOREST);
    for (s32 i = 0; i < forests->count; i++)
    {
        WarEntity* entity = forests->items[i];
        if (entity)
        {
            determineTreeTiles(context, entity);
        }
    }
}

WarTree* findAccesibleTree(WarContext* context, WarEntity* forest, vec2 position)
{
    WarMap* map = context->map;

    WarTree* result = NULL;

    vec2List positions;
    vec2ListInit(&positions, vec2ListDefaultOptions);
    vec2ListAdd(&positions, position);

    for (s32 i = 0; i < positions.count; i++)
    {
        position = positions.items[i];

        WarTree* tree = getTreeAtPosition(forest, position.x, position.y);
        if (tree)
        {
            bool isTreeAccessible = isPositionAccesible(map->finder, position);
            bool isTreeVisibleOrFog =
                isTileVisible(map, (s32)position.x, (s32)position.y) ||
                isTileFog(map, (s32)position.x, (s32)position.y);

            if (isTreeAccessible && isTreeVisibleOrFog && tree->amount > 0)
            {
                result = tree;
                break;
            }
        }

        for (s32 d = 0; d < dirC; d++)
        {
            s32 xx = (s32)position.x + dirX[d];
            s32 yy = (s32)position.y + dirY[d];
            if (isInside(map->finder, xx, yy))
            {
                vec2 newPosition = vec2i(xx, yy);
                if (!vec2ListContains(&positions, newPosition))
                    vec2ListAdd(&positions, newPosition);
            }
        }
    }

    vec2ListFree(&positions);

    return result;
}

void plantTree(WarContext* context, WarEntity* forest, s32 x, s32 y)
{
    assert(forest);
    assert(forest->type == WAR_ENTITY_TYPE_FOREST);

    WarMap* map = context->map;

    // if the position is not empty, there can't be tree there
    if (!isEmpty(map->finder, x, y))
        return;

    // only plant a tree in the top border of the map, if there are one below
    if (!isInside(map->finder, x, y - 1))
    {
        WarTree* belowTree = getTreeAtPosition(forest, x, y + 1);
        if (belowTree)
        {
            WarTree tree1 = createTree(x, y, TREE_MAX_WOOD);
            WarTreeListAdd(&forest->forest.trees, tree1);
            setStaticEntity(map->finder, x, y, 1, 1, forest->id);
        }
    }
    else
    {
        WarTree tree1 = createTree(x, y, TREE_MAX_WOOD);
        WarTreeListAdd(&forest->forest.trees, tree1);
        setStaticEntity(map->finder, x, y, 1, 1, forest->id);

        if (isEmpty(map->finder, x, y - 1))
        {
            WarTree tree2 = createTree(x, y - 1, TREE_MAX_WOOD);
            WarTreeListAdd(&forest->forest.trees, tree2);
            setStaticEntity(map->finder, x, y - 1, 1, 1, forest->id);
        }
    }
}

bool validTree(WarContext* context, WarEntity* forest, WarTree* tree)
{
    WarTreeList* trees = &forest->forest.trees;
    for (s32 i = 0; i < trees->count; i++)
    {
        WarTree* tree2 = &trees->items[i];
        if (tree2->tilex == tree->tilex)
        {
            if (tree2->tiley == tree->tiley - 1 ||
                tree2->tiley == tree->tiley + 1)
            {
                return true;
            }
        }
    }

    return false;
}

void takeTreeDown(WarContext* context, WarEntity* forest, WarTree* tree)
{
    WarMap* map = context->map;
    assert(map);

    assert(forest);
    assert(forest->type == WAR_ENTITY_TYPE_FOREST);
    assert(tree);

    WarTreeData data = getTreeData(WAR_TREE_CHOPPED);
    s32 choppedTileIndex = map->tilesetType == MAP_TILESET_FOREST
        ? data.tileIndexForest : data.tileIndexSwamp;

    WarTree choppedTree = *tree;

    setFreeTiles(map->finder, choppedTree.tilex, choppedTree.tiley, 1, 1);
    setMapTileIndex(context, choppedTree.tilex, choppedTree.tiley, choppedTileIndex);
    WarTreeListRemove(&forest->forest.trees, choppedTree);

    WarTree* aboveTree = getTreeAtPosition(forest, choppedTree.tilex, choppedTree.tiley - 1);
    if (aboveTree && !validTree(context, forest, aboveTree))
        takeTreeDown(context, forest, aboveTree);

    WarTree* belowTree = getTreeAtPosition(forest, choppedTree.tilex, choppedTree.tiley + 1);
    if (belowTree && !validTree(context, forest, belowTree))
        takeTreeDown(context, forest, belowTree);
}

s32 chopTree(WarContext* context, WarEntity* forest, WarTree* tree, s32 amount)
{
    if (tree->amount < amount)
        amount = tree->amount;

    tree->amount -= amount;
    tree->amount = max(tree->amount, 0);

    if (tree->amount == 0)
    {
        takeTreeDown(context, forest, tree);
        determineTreeTiles(context, forest);
    }

    return amount;
}

WarEntity* createForest(WarContext* context)
{
    WarMap* map = context->map;

    WarTreeList trees;
    WarTreeListInit(&trees, WarTreeListDefaultOptions);

    WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_FOREST, true);
    addSpriteComponent(context, entity, map->sprite);
    addForestComponent(context, entity, trees);

    return entity;
}
