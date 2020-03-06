bool hasWallPieceAtPosition(WarEntity* wall, s32 x, s32 y)
{
    WarWallPieceList* pieces = &wall->wall.pieces;
    for (s32 i = 0; i < pieces->count; i++)
    {
        WarWallPiece* piece = &pieces->items[i];
        if (piece->tilex == x && piece->tiley == y)
            return true;
    }

    return false;
}

WarWallPiece* getWallPieceAtPosition(WarEntity* wall, s32 x, s32 y)
{
    WarWallPieceList* pieces = &wall->wall.pieces;
    for (s32 i = 0; i < pieces->count; i++)
    {
        WarWallPiece* piece = &pieces->items[i];
        if (piece->tilex == x && piece->tiley == y)
            return piece;
    }

    return NULL;
}

void determineWallTypes(WarContext* context, WarEntity* entity)
{
    assert(entity->type == WAR_ENTITY_TYPE_WALL);

    WarMap* map = context->map;

    WarWallPieceList* pieces = &entity->wall.pieces;

    const s32 dirC = 4;
    const s32 dirX[] = {  0, 1, 0, -1 };
    const s32 dirY[] = { -1, 0, 1,  0 };

    s32 count = pieces->count;
    for(s32 i = 0; i < count; i++)
    {
        WarWallPiece* pi = &pieces->items[i];

        s32 index = 0;

        for (s32 d = 0; d < dirC; d++)
        {
            s32 xx = pi->tilex + dirX[d];
            s32 yy = pi->tiley + dirY[d];

            if (!isInside(map->finder, xx, yy) || hasWallPieceAtPosition(entity, xx, yy))
            {
                index = index | (1 << d);
            }
        }

        pi->type = wallTileTypeMap[index];
    }
}

WarWallPiece* addWallPiece(WarEntity* entity, s32 x, s32 y, s32 player)
{
    WarWallPieceList* pieces = &entity->wall.pieces;
    WarWallPieceListAdd(pieces, createWallPiece(x, y, player));
    return &pieces->items[pieces->count - 1];
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

void removeWallPiece(WarEntity* entity, WarWallPiece* piece)
{
    WarWallPieceList* pieces = &entity->wall.pieces;
    WarWallPieceListRemove(pieces, *piece);
}

WarEntity* createWall(WarContext* context)
{
    WarMap* map = context->map;

    WarWallPieceList pieces;
    WarWallPieceListInit(&pieces, WarWallPieceListDefaultOptions);

    WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_WALL, true);
    addWallComponent(context, entity, pieces);
    addSpriteComponent(context, entity, map->sprite);

    return entity;
}
