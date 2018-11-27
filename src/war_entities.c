void addTransformComponent(WarContext* context, WarEntity* entity, vec2 position)
{
    entity->transform = (WarTransformComponent){0};
    entity->transform.enabled = true;
    entity->transform.position = position;
    entity->transform.rotation = VEC2_ZERO;
    entity->transform.scale = VEC2_ONE;
}

void removeTransformComponent(WarContext* context, WarEntity* entity)
{
    entity->transform = (WarTransformComponent){0};
}

void addSpriteComponent(WarContext* context, WarEntity* entity, WarSprite sprite)
{
    entity->sprite = (WarSpriteComponent){0};
    entity->sprite.enabled = true;
    entity->sprite.frameIndex = 0;
    entity->sprite.resourceIndex = 0;
    entity->sprite.sprite = sprite;
}

void addSpriteComponentFromResource(WarContext* context, WarEntity* entity, s32 resourceIndex)
{
    WarSprite sprite = createSpriteFromResourceIndex(context, resourceIndex);
    addSpriteComponent(context, entity, sprite);
    entity->sprite.resourceIndex = resourceIndex;
}

void removeSpriteComponent(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    for (s32 i = 0; i < sprite->sprite.framesCount; i++)
    {
        u8* data = sprite->sprite.frames[i].data;
        if (data)
            free(data);
    }

    nvgDeleteImage(context->gfx, sprite->sprite.image);

    *sprite = (WarSpriteComponent){0};
}

void addUnitComponent(WarContext* context, WarEntity* entity, WarUnitType type, s32 x, s32 y, u8 player, WarResourceKind resourceKind, u32 amount)
{
    WarUnitsData unitData = getUnitsData(type);

    entity->unit = (WarUnitComponent){0};
    entity->unit.enabled = true;
    entity->unit.type = type;
    entity->unit.direction = rand() % WAR_DIRECTION_COUNT;
    entity->unit.tilex = x;
    entity->unit.tiley = y;
    entity->unit.sizex = unitData.sizex;
    entity->unit.sizey = unitData.sizey;
    entity->unit.player = player;
    entity->unit.resourceKind = resourceKind;
    entity->unit.amount = amount;
    entity->unit.level = 0;
    entity->unit.hp = 0;
    entity->unit.maxhp = 0;
    entity->unit.armour = 0;
    entity->unit.range = 0;
    entity->unit.minDamage = 0;
    entity->unit.rndDamage = 0;
    entity->unit.decay = 0;
    entity->unit.actionIndex = 0;

    WarUnitActionListInit(&entity->unit.actions, WarUnitActionListDefaultOptions);
}

void removeUnitComponent(WarContext* context, WarEntity* entity)
{
    WarUnitActionListFree(&entity->unit.actions);
    entity->unit = (WarUnitComponent){0};
}

void addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces)
{
    entity->road = (WarRoadComponent){0};
    entity->road.enabled = true;
    entity->road.pieces = pieces;
}

void removeRoadComponent(WarContext* context, WarEntity* entity)
{
    WarRoadPieceListFree(&entity->road.pieces);
    entity->road = (WarRoadComponent){0};
}

void addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces)
{
    entity->wall = (WarWallComponent){0};
    entity->wall.enabled = true;
    entity->wall.pieces = pieces;
}

void removeWallComponent(WarContext* context, WarEntity* entity)
{
    WarWallPieceListFree(&entity->wall.pieces);
    entity->wall = (WarWallComponent){0};
}

void addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces)
{
    entity->ruin = (WarRuinComponent){0};
    entity->ruin.enabled = true;
    entity->ruin.pieces = pieces;
}

void removeRuinComponent(WarContext* context, WarEntity* entity)
{
    WarRuinPieceListFree(&entity->ruin.pieces);
    entity->ruin = (WarRuinComponent){0};
}

void addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees)
{
    entity->forest = (WarForestComponent){0};
    entity->forest.enabled = true;
    entity->forest.trees = trees;
}

void removeForestComponent(WarContext* context, WarEntity* entity)
{
    WarTreeListFree(&entity->forest.trees);
    entity->forest = (WarForestComponent){0};
}

void addStateMachineComponent(WarContext* context, WarEntity* entity)
{
    entity->stateMachine = (WarStateMachineComponent){0};
    entity->stateMachine.enabled = true;
    entity->stateMachine.currentState = NULL;
    entity->stateMachine.nextState = NULL;
}

void removeStateMachineComponent(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;

    if (stateMachine->currentState)
        leaveState(context, entity, stateMachine->currentState);

    if (stateMachine->nextState)
        leaveState(context, entity, stateMachine->nextState);

    entity->stateMachine = (WarStateMachineComponent){0};
}

void addAnimationsComponent(WarContext* context, WarEntity* entity)
{
    entity->animations = (WarAnimationsComponent){0};
    entity->animations.enabled = true;
    WarSpriteAnimationListInit(&entity->animations.animations, WarSpriteAnimationListDefaultOptions);
}

void removeAnimationsComponent(WarContext* context, WarEntity* entity)
{
    WarSpriteAnimationListFree(&entity->animations.animations);
    entity->animations = (WarAnimationsComponent){0};
}

#define isNeutral(player) (player == 4)
#define isEnemy(player) (player != 0 && !isNeutral(player))
#define isHuman(player) (player == 0)

// Entities
WarEntity* createEntity(WarContext* context, WarEntityType type)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* entity = (WarEntity *)xcalloc(1, sizeof(WarEntity));
    entity->id = ++context->staticEntityId;
    entity->type = type;
    entity->enabled = true;

    // transform component
    entity->transform = (WarTransformComponent){0};
    entity->sprite = (WarSpriteComponent){0};
    entity->unit = (WarUnitComponent){0};
    entity->road = (WarRoadComponent){0};
    entity->wall = (WarWallComponent){0};
    entity->forest = (WarForestComponent){0};
    entity->stateMachine = (WarStateMachineComponent){0};
    entity->animations = (WarAnimationsComponent){0};

    WarEntityListAdd(&map->entities, entity);

    return entity;
}

void determineRoadTypes(WarEntity* entity)
{
    assert(entity->type == WAR_ENTITY_TYPE_ROAD);

    WarRoadPieceList* pieces = &entity->road.pieces;

    s32 count = pieces->count;
    for(s32 i = 0; i < count; i++)
    {
        WarRoadPiece* pi = &pieces->items[i];

        bool left = false, 
             top = false, 
             right = false, 
             bottom = false;

        for(s32 j = 0; j < count; j++)
        {
            if (i == j) continue;

            WarRoadPiece* pj = &pieces->items[j];

            if (pj->tilex == pi->tilex - 1 && pj->tiley == pi->tiley)
                left = true;
            else if(pj->tilex == pi->tilex + 1 && pj->tiley == pi->tiley)
                right = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley - 1)
                top = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley + 1)
                bottom = true;
        }

        // Endpieces
        if (top && !bottom && !left && !right)
            pi->type = WAR_ROAD_PIECE_BOTTOM;
        if (!top && bottom && !left && !right)
            pi->type = WAR_ROAD_PIECE_TOP;
        if (!top && !bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_LEFT;
        if (!top && !bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_RIGHT;

        // Corner pieces
        if (top && !bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_TOP_LEFT;
        if (!top && bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_BOTTOM_LEFT;
        if (top && !bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_TOP_RIGHT;
        if (!top && bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_BOTTOM_RIGHT;

        // Middle pieces
        if (!top && !bottom && left && right)
            pi->type = WAR_ROAD_PIECE_HORIZONTAL;
        if (top && bottom && !left && !right)
            pi->type = WAR_ROAD_PIECE_VERTICAL;

        // Quad piece
        if (top && bottom && left && right)
            pi->type = WAR_ROAD_PIECE_CROSS;

        // T-Corners
        if (top && bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_T_RIGHT;
        if (top && bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_T_LEFT;
        if (!top && bottom && left && right)
            pi->type = WAR_ROAD_PIECE_T_TOP;
        if (top && !bottom && left && right)
            pi->type = WAR_ROAD_PIECE_T_BOTTOM;
    }
}

void addRoadPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct)
{
    assert(entity->type == WAR_ENTITY_TYPE_ROAD);

    WarRoadPieceList* pieces = &entity->road.pieces;

    s32 x1 = construct->x1;
    s32 y1 = construct->y1;
    s32 x2 = construct->x2;
    s32 y2 = construct->y2;
    u8 player = construct->player;

    s32 dx = x2 - x1;
    dx = sign(dx);

    s32 dy = y2 - y1;
    dy = sign(dy);
    
    s32 x = x1;
    s32 y = y1;

    WarRoadPiece piece;

    while (x != x2)
    {
        WarRoadPieceListAdd(pieces, createRoadPiece(x, y, player));
        x += dx;
    }

    WarRoadPieceListAdd(pieces, createRoadPiece(x, y, player));

    while (y != y2)
    {
        WarRoadPieceListAdd(pieces, createRoadPiece(x, y, player));
        y += dy;
    }

    WarRoadPieceListAdd(pieces, createRoadPiece(x, y, player));
}

WarEntity* createRoad(WarContext* context)
{
    WarMap* map = context->map;

    WarRoadPieceList pieces;
    WarRoadPieceListInit(&pieces, WarRoadPieceListDefaultOptions);

    WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_ROAD);
    addRoadComponent(context, entity, pieces);
    addSpriteComponent(context, entity, map->sprite);

    return entity;
}

void determineWallTypes(WarEntity* entity)
{
    assert(entity->type == WAR_ENTITY_TYPE_WALL);

    WarWallPieceList *pieces = &entity->wall.pieces;

    s32 count = pieces->count;
    for(s32 i = 0; i < count; i++)
    {
        WarWallPiece* pi = &pieces->items[i];

        bool left = false, 
             top = false, 
             right = false, 
             bottom = false;

        for(s32 j = 0; j < count; j++)
        {
            if (i == j) continue;

            WarWallPiece* pj = &pieces->items[j];

            if (pj->tilex == pi->tilex - 1 && pj->tiley == pi->tiley)
                left = true;
            else if(pj->tilex == pi->tilex + 1 && pj->tiley == pi->tiley)
                right = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley - 1)
                top = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley + 1)
                bottom = true;
        }

        // Endpieces
        if (top && !bottom && !left && !right)
            pi->type = WAR_ROAD_PIECE_BOTTOM;
        if (!top && bottom && !left && !right)
            pi->type = WAR_ROAD_PIECE_TOP;
        if (!top && !bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_LEFT;
        if (!top && !bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_RIGHT;

        // Corner pieces
        if (top && !bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_BOTTOM_RIGHT;
        if (!top && bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_TOP_RIGHT;
        if (top && !bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_BOTTOM_LEFT;
        if (!top && bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_TOP_LEFT;

        // Middle pieces
        if (!top && !bottom && left && right)
            pi->type = WAR_ROAD_PIECE_HORIZONTAL;
        if (top && bottom && !left && !right)
            pi->type = WAR_ROAD_PIECE_VERTICAL;

        // Quad piece
        if (top && bottom && left && right)
            pi->type = WAR_ROAD_PIECE_CROSS;

        // T-Corners
        if (top && bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_T_RIGHT;
        if (top && bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_T_LEFT;
        if (!top && bottom && left && right)
            pi->type = WAR_ROAD_PIECE_T_TOP;
        if (top && !bottom && left && right)
            pi->type = WAR_ROAD_PIECE_T_BOTTOM;
    }
}

WarWallPiece* getWallPiece(WarEntity* entity, vec2 position)
{
    assert(isWall(entity));

    WarWallComponent* wall = &entity->wall;
    
    for(s32 i = 0; i < wall->pieces.count; i++)
    {
        WarWallPiece* piece = &wall->pieces.items[i];
        if (piece->tilex == (s32)position.x && piece->tiley == (s32)position.y)
        {
            return piece;
        }
    }
    
    return NULL;
}

void addWallPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct)
{
    assert(entity->type == WAR_ENTITY_TYPE_WALL);

    WarWallPieceList *pieces = &entity->wall.pieces;

    s32 x1 = construct->x1;
    s32 y1 = construct->y1;
    s32 x2 = construct->x2;
    s32 y2 = construct->y2;
    u8 player = construct->player;

    s32 dx = x2 - x1;
    dx = sign(dx);

    s32 dy = y2 - y1;
    dy = sign(dy);
    
    s32 x = x1;
    s32 y = y1;

    WarWallPiece piece;

    while (x != x2)
    {
        WarWallPieceListAdd(pieces, createWallPiece(x, y, player));
        x += dx;
    }

    WarWallPieceListAdd(pieces, createWallPiece(x, y, player));

    while (y != y2)
    {
        WarWallPieceListAdd(pieces, createWallPiece(x, y, player));
        y += dy;
    }

    WarWallPieceListAdd(pieces, createWallPiece(x, y, player));
}

WarEntity* createWall(WarContext* context)
{
    WarMap* map = context->map;

    WarWallPieceList pieces;
    WarWallPieceListInit(&pieces, WarWallPieceListDefaultOptions);

    WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_WALL);
    addWallComponent(context, entity, pieces);
    addSpriteComponent(context, entity, map->sprite);

    return entity;
}

void determineRuinTypes(WarEntity* entity)
{
    WarRuinComponent* ruin = &entity->ruin;
    WarRuinPieceList* pieces = &ruin->pieces;
    s32 count = pieces->count;
    for(s32 i = 0; i < count; i++)
    {
        WarRuinPiece* pi = &pieces->items[i];

        bool topLeft = false,
             top = false,
             topRight = false,
             left = false,
             right = false,
             bottomLeft = false,
             bottom = false,
             bottomRight = false;

        for(s32 j = 0; j < count; j++)
        {
            if (i == j) continue;

            WarRuinPiece* pj = &pieces->items[j];

            if (pj->tilex == pi->tilex - 1 && pj->tiley == pi->tiley - 1)
                topLeft = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley - 1)
                top = true;
            else if (pj->tilex == pi->tilex + 1 && pj->tiley == pi->tiley - 1)
                topRight = true;
            else if (pj->tilex == pi->tilex - 1 && pj->tiley == pi->tiley)
                left = true;
            else if(pj->tilex == pi->tilex + 1 && pj->tiley == pi->tiley)
                right = true;
            else if (pj->tilex == pi->tilex - 1 && pj->tiley == pi->tiley + 1)
                bottomLeft = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley + 1)
                bottom = true;
            else if (pj->tilex == pi->tilex + 1 && pj->tiley == pi->tiley + 1)
                bottomRight = true;
        }

        // Corners
        if (!topLeft && !top && !left && right && bottom && bottomRight)
            pi->type = WAR_RUIN_PIECE_TOP_LEFT;
        if (!topRight && !top && !right && left && bottom && bottomLeft)
            pi->type = WAR_RUIN_PIECE_TOP_RIGHT;
        if (!bottomLeft && !bottom && !left && right && top && topRight)
            pi->type = WAR_RUIN_PIECE_BOTTOM_LEFT;
        if (!bottomRight && !bottom && !right && left && top && topLeft)
            pi->type = WAR_RUIN_PIECE_BOTTOM_RIGHT;

        // Edges
        if (!top && left && right && bottom)
            pi->type = WAR_RUIN_PIECE_TOP;
        if (!left && top && bottom && right)
            pi->type = WAR_RUIN_PIECE_LEFT;
        if (!right && top && bottom && left)
            pi->type = WAR_RUIN_PIECE_RIGHT;
        if (!bottom && left && right && top)
            pi->type = WAR_RUIN_PIECE_BOTTOM;

        // Insides
        if (!bottomRight && top && left && right && bottom && topLeft)
            pi->type = WAR_RUIN_PIECE_TOP_LEFT_INSIDE;
        if (!bottomLeft && top && right && left && bottom && topRight)
            pi->type = WAR_RUIN_PIECE_TOP_RIGHT_INSIDE;
        if (!topRight && top && left && right && bottom && bottomLeft)
            pi->type = WAR_RUIN_PIECE_BOTTOM_LEFT_INSIDE;
        if (!topLeft && top && left && right && bottom && bottomRight)
            pi->type = WAR_RUIN_PIECE_BOTTOM_RIGHT_INSIDE;

        // Diagonals
        if (!topRight && !bottomLeft && topLeft && bottomRight)
            pi->type = WAR_RUIN_PIECE_DIAG_1;
        if (!topLeft && !bottomRight && topRight && bottomLeft)
            pi->type = WAR_RUIN_PIECE_DIAG_2;

        // Center
        if (topLeft && top && topRight && left && right && bottomLeft && bottom && bottomRight)
            pi->type = WAR_RUIN_PIECE_CENTER;
        if (!topLeft && !top && !topRight && !left && !right && !bottomLeft && !bottom && !bottomRight)
            pi->type = WAR_RUIN_PIECE_CENTER;
    }
}

WarEntity* createRuins(WarContext* context)
{
    WarMap* map = context->map;

    WarRuinPieceList pieces;
    WarRuinPieceListInit(&pieces, WarRuinPieceListDefaultOptions);

    WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_RUIN);
    addRuinComponent(context, entity, pieces);
    addSpriteComponent(context, entity, map->sprite);

    return entity;
}

void addRuinsPieces(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 dim)
{
    assert(entity);
    assert(entity->type == WAR_ENTITY_TYPE_RUIN);

    WarRuinPieceList* pieces = &entity->ruin.pieces;

    for(s32 yy = 0; yy < dim; yy++)
    {
        for(s32 xx = 0; xx < dim; xx++)
            WarRuinPieceListAdd(pieces, createRuinPiece(x + xx, y + yy));
    }

    determineRuinTypes(entity);
}

s32 findEntityIndex(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    for (s32 i = 0; i < map->entities.count; i++)
    {
        if (map->entities.items[i]->id == id)
            return i;
    }

    return -1;
}

WarEntity* findEntity(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    for (s32 i = 0; i < map->entities.count; i++)
    {
        if (map->entities.items[i]->id == id)
            return map->entities.items[i];
    }

    return NULL;
}

WarEntity* findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* result = NULL;
    f32 minDst = INT32_MAX;

    for (s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* target = map->entities.items[i];
        if (isUnitOfType(target, type))
        {
            s32 dst = unitDistanceInTiles(entity, target);
            if (dst < minDst)
            {
                result = target;
                minDst = dst;
            }
        }
    }

    return result;
}

void determineTreeTiles(WarContext* context, WarEntity* forest)
{
    assert(forest);
    assert(forest->type == WAR_ENTITY_TYPE_FOREST);

    WarMap* map = context->map;

    WarTreeList* trees = &forest->forest.trees;
    
    for (s32 i = 0; i < trees->count; i++)
    {
        WarTree* ti = &trees->items[i];

        bool topLeft = ti->tilex == 0 || ti->tiley == 0,
             top = ti->tiley == 0,
             topRight = ti->tilex == MAP_TILES_WIDTH - 1 || ti->tiley == 0,
             left = ti->tilex == 0,
             right = ti->tilex == MAP_TILES_WIDTH - 1,
             bottomLeft = ti->tilex == 0 || ti->tiley == MAP_TILES_HEIGHT - 1,
             bottom = ti->tiley == MAP_TILES_HEIGHT - 1,
             bottomRight = ti->tilex == MAP_TILES_WIDTH - 1 || ti->tiley == MAP_TILES_HEIGHT - 1;

        for (s32 j = 0; j < trees->count; j++)
        {
            if (i == j) continue;

            WarTree* tj = &trees->items[j];

            // if the tree is chopped down treat it as an empty
            if (tj->amount == 0) continue;

            if (tj->tilex == ti->tilex - 1 && tj->tiley == ti->tiley - 1)
                topLeft = true;
            else if (tj->tilex == ti->tilex && tj->tiley == ti->tiley - 1)
                top = true;
            else if (tj->tilex == ti->tilex + 1 && tj->tiley == ti->tiley - 1)
                topRight = true;
            else if (tj->tilex == ti->tilex - 1 && tj->tiley == ti->tiley)
                left = true;
            else if(tj->tilex == ti->tilex + 1 && tj->tiley == ti->tiley)
                right = true;
            else if (tj->tilex == ti->tilex - 1 && tj->tiley == ti->tiley + 1)
                bottomLeft = true;
            else if (tj->tilex == ti->tilex && tj->tiley == ti->tiley + 1)
                bottom = true;
            else if (tj->tilex == ti->tilex + 1 && tj->tiley == ti->tiley + 1)
                bottomRight = true;
        }
        
        WarTreeTileType type = WAR_TREE_NONE;

        if (!bottom)
        {
            if (left && right)
                type = WAR_TREE_BOTTOM;
            else if(!left)
                type = WAR_TREE_BOTTOM_LEFT;
            else if(!right)
                type = WAR_TREE_BOTTOM_RIGHT;
            else
                type == WAR_TREE_BOTTOM_END;
        }
        else
        {
            if (left && right && bottomLeft && bottomRight)
            {
                if (top)
                {
                    if (topRight && topLeft)
                    {
                        type = WAR_TREE_CENTER;
                    }
                    else if (!topLeft)
                    {
                        type = WAR_TREE_BOTTOM_RIGHT_INSIDE;
                    }
                    else if (!topRight)
                    {
                        type = WAR_TREE_BOTTOM_LEFT_INSIDE;
                    }
                    else
                    {
                    }
                }
                else
                {
                    type = WAR_TREE_CENTER;
                }
            }
            else if (left && right && bottomLeft && !bottomRight)
            {
                type = WAR_TREE_TOP_LEFT_INSIDE;
            }
            else if (left && right && !bottomLeft && bottomRight)
            {
                type = WAR_TREE_TOP_RIGHT_INSIDE;
            }
            else if (left && right && !bottomLeft && !bottomRight)
            {

            }
            else if (!left)
            {
                if (bottomLeft)
                {
                    type = WAR_TREE_BOTTOM_LEFT_INSIDE;
                }
                else
                {
                    type = WAR_TREE_LEFT;
                }
            }
            else if (!right)
            {
                if (bottomRight)
                {
                    type = WAR_TREE_BOTTOM_RIGHT_INSIDE;
                }
                else
                {
                    type = WAR_TREE_RIGHT;
                }
            }
        }

        if (type != WAR_TREE_NONE)
        {
            WarTreesData data = getTreesData(type);

            WarResource* levelInfo = getOrCreateResource(context, map->levelInfoIndex);
            assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

            WarResource* levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
            assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

            WarMapTilesetType tilesetType = map->tilesetType;
            assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

            s32 prevTileIndex = levelVisual->levelVisual.data[ti->tiley * MAP_TILES_WIDTH + ti->tilex];
            s32 newTileIndex = (tilesetType == MAP_TILESET_FOREST) ? data.tileIndexForest : data.tileIndexSwamp;

            if (prevTileIndex != newTileIndex)
                logDebug("different tile index for tree (%d, %d), prev: %d, new: %d", ti->tilex, ti->tiley, prevTileIndex, newTileIndex);

            levelVisual->levelVisual.data[ti->tiley * MAP_TILES_WIDTH + ti->tilex] = newTileIndex;

            if (!top && isInside(map->finder, ti->tilex, ti->tiley - 1))
            {
                if (left && right)
                {
                    type = WAR_TREE_TOP;
                }
                else if (!left)
                {
                    type = WAR_TREE_TOP_LEFT;
                }
                else if (!right)
                {
                    type = WAR_TREE_TOP_RIGHT;
                }
                else
                {
                    type = WAR_TREE_TOP;
                }

                data = getTreesData(type);
                s32 newTileIndex = (tilesetType == MAP_TILESET_FOREST) ? data.tileIndexForest : data.tileIndexSwamp;
                levelVisual->levelVisual.data[(ti->tiley - 1) * MAP_TILES_WIDTH + ti->tilex] = newTileIndex;
            }
        }
    }
}

WarTree* getTreeAtPosition(WarEntity* forest, vec2 position)
{
    s32 x = (s32)position.x;
    s32 y = (s32)position.y;
    WarTreeList* trees = &forest->forest.trees;
    for (s32 i = 0; i < trees->count; i++)
    {
        WarTree* tree = &trees->items[i];
        if (tree->tilex == x && tree->tiley == y)
            return tree;
    }

    return NULL;
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

        WarTree* tree = getTreeAtPosition(forest, position);
        if (tree)
        {
            if (isPositionAccesible(map->finder, position) && tree->amount > 0)
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

void removeEntityById(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    s32 index = findEntityIndex(context, id);
    assert(index >= 0);

    WarEntity* entity = map->entities.items[index];

    removeTransformComponent(context, entity);
    removeSpriteComponent(context, entity);
    removeRoadComponent(context, entity);
    removeWallComponent(context, entity);
    removeRuinComponent(context, entity);
    removeForestComponent(context, entity);
    removeUnitComponent(context, entity);
    removeStateMachineComponent(context, entity);
    removeAnimationsComponent(context, entity);

    WarEntityListRemoveAt(&map->entities, index);
}

// Render entities
void renderImage(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;

    if (sprite->enabled)
    {
        WarSpriteFrame* frame = getSpriteFrame(context, sprite);

        nvgTranslate(gfx, -frame->dx, -frame->dy);
        nvgTranslate(gfx, transform.position.x, transform.position.y);
        renderSprite(context, &sprite->sprite, VEC2_ZERO, VEC2_ONE);
    }
}

void renderRoad(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarRoadComponent* road = &entity->road;

    // the roads are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled)
    {
        if (road->enabled)
        {
            WarRoadPieceList* pieces = &road->pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, road->pieces.count);

            for (s32 i = 0; i < pieces->count; i++)
            {
                // get the index of the tile in the spritesheet of the map,
                // corresponding to the current tileset type (forest, swamp)
                WarRoadsData roadsData = getRoadsData(pieces->items[i].type);

                s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? roadsData.tileIndexForest : roadsData.tileIndexSwamp;
                
                // the position in the world of the road piece tile
                s32 x = pieces->items[i].tilex;
                s32 y = pieces->items[i].tiley;

                // coordinates in pixels of the road piece tile
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                nvgSave(gfx);
                nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

                nvgRestore(gfx);
            }

            nvgEndImageBatch(gfx, batch);
        }
    }
}

void renderWall(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarWallComponent* wall = &entity->wall;

    // the walls are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled)
    {
        if (wall->enabled)
        {
            WarWallPieceList* pieces = &wall->pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, wall->pieces.count);

            for (s32 i = 0; i < pieces->count; i++)
            {
                WarWallPiece* piece = &pieces->items[i];

                // get the index of the tile in the spritesheet of the map,
                // corresponding to the current tileset type (forest, swamp)
                WarWallsData wallsData = getWallsData(piece->type);

                s32 tileIndex = 0;

                s32 hpPercent = percentabi(piece->hp, piece->maxhp);
                if (hpPercent <= 0)
                {
                    tileIndex = (tilesetType == MAP_TILESET_FOREST)
                        ? wallsData.tileDestroyedForest : wallsData.tileDestroyedSwamp;
                }
                else if(hpPercent < 50)
                {
                    tileIndex = (tilesetType == MAP_TILESET_FOREST)
                        ? wallsData.tileDamagedForest : wallsData.tileDamagedSwamp;
                }
                else
                {
                    tileIndex = (tilesetType == MAP_TILESET_FOREST)
                        ? wallsData.tileForest : wallsData.tileSwamp;
                }

                // the position in the world of the wall piece tile
                s32 x = piece->tilex;
                s32 y = piece->tiley;

                // coordinates in pixels of the wall piece tile
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                nvgSave(gfx);
                nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

                nvgRestore(gfx);
            }

            nvgEndImageBatch(gfx, batch);
        }
    }
}

void renderRuin(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarRuinComponent* ruin = &entity->ruin;

    // the walls are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled)
    {
        if (ruin->enabled)
        {
            WarRuinPieceList* pieces = &ruin->pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, ruin->pieces.count);

            for (s32 i = 0; i < ruin->pieces.count; i++)
            {
                // get the index of the tile in the spritesheet of the map,
                // corresponding to the current tileset type (forest, swamp)
                WarRuinsData ruinsData = getRuinsData(pieces->items[i].type);
                
                s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? ruinsData.tileIndexForest : ruinsData.tileIndexSwamp;
                
                // the position in the world of the road piece tile
                s32 x = pieces->items[i].tilex;
                s32 y = pieces->items[i].tiley;

                // coordinates in pixels of the road piece tile
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                nvgSave(gfx);
                nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

                nvgRestore(gfx);
            }

            nvgEndImageBatch(gfx, batch);
        }
    }
}

void renderForest(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarForestComponent* forest = &entity->forest;

    // the wood are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    // if (sprite->enabled)
    // {
    //     if (forest->enabled)
    //     {
    //         WarTreeList* trees = &forest->trees;
    //         for (s32 i = 0; i < trees->count; i++)
    //         {
    //             WarTree* tree = &trees->items[i];

    //             WarTreesData data = getTreesData(tree->type);

    //             // this is the index in the map sprite of the chopped down tree tile
    //             s32 tileIndex = (tilesetType == MAP_TILESET_FOREST) 
    //                 ? data.tileIndexForest : data.tileIndexSwamp;

    //             // only render the sprite of chooped wood if the entity hasn't any wood left
    //             // the sprite of the trees are already in the map description
    //             // if (tree->amount == 0)
    //             {
    //                 tileIndex = (tilesetType == MAP_TILESET_FOREST) 
    //                     ? CHOPPED_DOWN_TREE_FOREST_TILE_INDEX : CHOPPED_DOWN_TREE_SWAMP_TILE_INDEX;
    //             }

    //             // the position in the world of the wood tile
    //             s32 x = tree->tilex;
    //             s32 y = tree->tiley;

    //             // coordinates in pixels of the road piece tile
    //             s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
    //             s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

    //             nvgSave(gfx);
    //             nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

    //             rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
    //             rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
    //             renderSubSprite(context, &sprite->sprite, rs, rd, VEC2_ONE);

    //             nvgRestore(gfx);
    //         }            
    //     }
    // }
}

void renderUnit(WarContext* context, WarEntity* entity, bool selected)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarUnitComponent* unit = &entity->unit;
    WarAnimationsComponent* animations = &entity->animations;

    // size of the original sprite
    vec2 frameSize = getUnitFrameSize(entity);

    // size of the unit
    vec2 unitSize = getUnitSpriteSize(entity);

    // position of the unit in the map
    vec2 position = transform->position;

    // scale of the unit: this is modified by animations when the animation indicates that it
    // should flip horizontally or vertically or both
    vec2 scale = transform->scale;

    nvgTranslate(gfx, -halff(frameSize.x), -halff(frameSize.y));
    nvgTranslate(gfx, halff(unitSize.x), halff(unitSize.y));
    nvgTranslate(gfx, position.x, position.y);

#ifdef DEBUG_RENDER_UNIT_INFO
    nvgFillRect(gfx, getUnitFrameRect(entity), nvgRGBA(0, 0, 128, 128));
    nvgFillRect(gfx, getUnitSpriteRect(entity), NVG_GRAY_TRANSPARENT);
    nvgFillRect(gfx, rectv(getUnitSpriteCenter(entity), VEC2_ONE), nvgRGB(255, 0, 0));
#endif

#ifdef DEBUG_RENDER_UNIT_STATS
    rect spriteRect = getUnitSpriteRect(entity);

    char debugText[50];

    if (unit->hp == 0)
        sprintf(debugText, "hp: dead");
    else
        sprintf(debugText, "hp: %d", percentabi(unit->hp, unit->maxhp));

    nvgFontSize(gfx, 5.0f);
    nvgFontFace(gfx, "roboto-r");
    nvgFillColor(gfx, nvgRGBA(200, 200, 200, 255));
    nvgTextAlign(gfx, NVG_ALIGN_LEFT);
    nvgText(gfx, spriteRect.x, spriteRect.y, debugText, NULL);
#endif

    if (sprite->enabled)
    {
        nvgSave(gfx);

        WarSpriteFrame* frame = getSpriteFrame(context, sprite);
        updateSpriteImage(context, &sprite->sprite, frame->data);
        renderSprite(context, &sprite->sprite, VEC2_ZERO, scale);

        nvgRestore(gfx);

        if (selected)
        {
            rect selr = rectf(halff(frameSize.x - unitSize.x), halff(frameSize.y - unitSize.y), unitSize.x, unitSize.y);
            nvgStrokeRect(gfx, selr, NVG_GREEN_SELECTION, 1.0f);
        }
    }

    if (animations->enabled)
    {
        for (s32 i = 0; i < animations->animations.count; i++)
        {
            WarSpriteAnimation* anim = animations->animations.items[i];
            if (anim->status == WAR_ANIM_STATUS_RUNNING)
            {
                s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
                animFrameIndex = clamp(animFrameIndex, 0, anim->frames.count - 1);

                s32 spriteFrameIndex = anim->frames.items[animFrameIndex];
                assert(spriteFrameIndex >= 0 && spriteFrameIndex < anim->sprite.framesCount);

                // size of the original sprite
                vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

                nvgSave(gfx);

                nvgTranslate(gfx, anim->offset.x, anim->offset.y);
                nvgScale(gfx, anim->scale.x, anim->scale.y);

#ifdef DEBUG_RENDER_UNIT_ANIMATIONS
                nvgFillRect(gfx, rectv(VEC2_ZERO, animFrameSize), NVG_GRAY_TRANSPARENT);
#endif

                WarSpriteFrame frame = anim->sprite.frames[spriteFrameIndex];
                updateSpriteImage(context, &anim->sprite, frame.data);
                renderSprite(context, &anim->sprite, VEC2_ZERO, VEC2_ONE);

                nvgRestore(gfx);
            }
        }
    }
}

void renderEntity(WarContext* context, WarEntity* entity, bool selected)
{
    NVGcontext* gfx = context->gfx;

    if (entity->id && entity->enabled)
    {
        nvgSave(gfx);

        switch (entity->type)
        {
            case WAR_ENTITY_TYPE_IMAGE:
            {
                renderImage(context, entity);
                break;
            }

            case WAR_ENTITY_TYPE_UNIT:
            {
                renderUnit(context, entity, selected);
                break;
            }

            case WAR_ENTITY_TYPE_ROAD:
            {
                renderRoad(context, entity);
                break;
            }

            case WAR_ENTITY_TYPE_WALL:
            {
                renderWall(context, entity);
                break;
            }

            case WAR_ENTITY_TYPE_RUIN:
            {
                renderRuin(context, entity);
                break;
            }

            case WAR_ENTITY_TYPE_FOREST:
            {
                renderForest(context, entity);
                break;
            }

            default:
            {
                logError("Entity of type %d can't be rendered.\n", entity->type);
                break;
            }
        }

        nvgRestore(gfx);
    }
}

inline s32 getTotalDamage(s32 minDamage, s32 rndDamage, s32 armour)
{
    return minDamage + maxi(rndDamage - armour, 0);
}

void takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage)
{
    assert(isUnit(entity));

    WarUnitComponent *unit = &entity->unit;

    // Minimal damage + [Random damage - Enemy's Armour]
    s32 damage = getTotalDamage(minDamage, rndDamage, unit->armour);
    unit->hp -= damage;
    unit->hp = maxi(unit->hp, 0);

    if (unit->hp == 0)
    {
        if (isBuildingUnit(entity))
        {
            WarState* collapseState = createCollapseState(context, entity);
            changeNextState(context, entity, collapseState, true, true);
        }
        else
        {
            WarState* deathState = createDeathState(context, entity);
            deathState->delay = __frameCountToSeconds(108);
            changeNextState(context, entity, deathState, true, true);
        }
    }
    else if (isBuildingUnit(entity))
    {
        if (!isDamaged(entity))
        {
            WarState* damagedState = createDamagedState(context, entity);
            changeNextState(context, entity, damagedState, true, true);
        }
    }
}

void takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage)
{
    assert(isWall(entity));

    // Minimal damage + [Random damage - Enemy's Armour]
    s32 damage = getTotalDamage(minDamage, rndDamage, 0);
    piece->hp -= damage;
    piece->hp = maxi(piece->hp, 0);
}

void chopTree(WarContext* context, WarEntity* forest, WarTree* tree, s32 amount)
{
    assert(forest);
    assert(forest->type == WAR_ENTITY_TYPE_FOREST);

    WarMap* map = context->map;

    tree->amount -= amount;
    tree->amount = maxi(tree->amount, 0);
    if (tree->amount == 0)
    {
        // determine the tree types when any tree of this forest is chopped down
        determineTreeTiles(context, forest);

        setFreeTiles(map->finder, tree->tilex, tree->tiley, 1, 1);
    }
}