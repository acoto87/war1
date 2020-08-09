bool hasRoadPieceAtPosition(WarEntity* road, s32 x, s32 y)
{
    WarRoadPieceList* pieces = &road->road.pieces;
    for (s32 i = 0; i < pieces->count; i++)
    {
        WarRoadPiece* piece = &pieces->items[i];
        if (piece->tilex == x && piece->tiley == y)
            return true;
    }

    return false;
}

WarRoadPiece* getRoadPieceAtPosition(WarEntity* road, s32 x, s32 y)
{
    WarRoadPieceList* pieces = &road->road.pieces;
    for (s32 i = 0; i < pieces->count; i++)
    {
        WarRoadPiece* piece = &pieces->items[i];
        if (piece->tilex == x && piece->tiley == y)
            return piece;
    }

    return NULL;
}

void determineRoadTypes(WarContext* context, WarEntity* entity)
{
    assert(isRoad(entity));

    WarMap* map = context->map;

    WarRoadPieceList* pieces = &entity->road.pieces;

    const s32 dirC = 4;
    const s32 dirX[] = {  0, 1, 0, -1 };
    const s32 dirY[] = { -1, 0, 1,  0 };

    s32 count = pieces->count;
    for(s32 i = 0; i < count; i++)
    {
        WarRoadPiece* pi = &pieces->items[i];

        s32 index = 0;

        for (s32 d = 0; d < dirC; d++)
        {
            s32 xx = pi->tilex + dirX[d];
            s32 yy = pi->tiley + dirY[d];

            if (!isInside(map->finder, xx, yy) || hasRoadPieceAtPosition(entity, xx, yy))
            {
                index = index | (1 << d);
            }
        }

        pi->type = roadTileTypeMap[index];
    }
}

void addRoadPiece(WarEntity* entity, s32 x, s32 y, s32 player)
{
    WarRoadPieceList* pieces = &entity->road.pieces;
    WarRoadPieceListAdd(pieces, createRoadPiece(x, y, player));
}

void addRoadPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct)
{
    assert(isRoad(entity));

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

void removeRoadPiece(WarEntity* entity, WarRoadPiece* piece)
{
    WarRoadPieceList* pieces = &entity->road.pieces;
    WarRoadPieceListRemove(pieces, *piece);
}

WarEntity* createRoad(WarContext* context)
{
    WarMap* map = context->map;

    WarRoadPieceList pieces;
    WarRoadPieceListInit(&pieces, WarRoadPieceListDefaultOptions);

    WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_ROAD, true);
    addRoadComponent(context, entity, pieces);
    addSpriteComponent(context, entity, map->sprite);

    return entity;
}