bool hasRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y)
{
    WarRuinPieceList* pieces = &ruins->ruin.pieces;
    for (s32 i = 0; i < pieces->count; i++)
    {
        WarRuinPiece* piece = &pieces->items[i];
        if (piece->tilex == x && piece->tiley == y)
            return true;
    }

    return false;
}

WarRuinPiece* getRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y)
{
    WarRuinPieceList* pieces = &ruins->ruin.pieces;
    for (s32 i = 0; i < pieces->count; i++)
    {
        WarRuinPiece* piece = &pieces->items[i];
        if (piece->tilex == x && piece->tiley == y)
            return piece;
    }

    return NULL;
}

void determineRuinTypes(WarContext* context, WarEntity* entity)
{
    assert(entity);
    assert(entity->type == WAR_ENTITY_TYPE_RUIN);

    WarMap* map = context->map;

    WarRuinPieceList* pieces = &entity->ruin.pieces;

    const s32 dirC = 8;
    const s32 dirX[] = { -1,  0,  1, 1, 1, 0, -1, -1 };
    const s32 dirY[] = { -1, -1, -1, 0, 1, 1,  1,  0 };

    s32List invalidPieces;
    s32ListInit(&invalidPieces, s32ListDefaultOptions);

    for(s32 i = 0; i < pieces->count; i++)
    {
        WarRuinPiece* pi = &pieces->items[i];

        s32 index = 0;

        for (s32 d = 0; d < dirC; d++)
        {
            s32 xx = pi->tilex + dirX[d];
            s32 yy = pi->tiley + dirY[d];
            
            if (!isInside(map->finder, xx, yy) || hasRuinPieceAtPosition(entity, xx, yy))
            {
                index = index | (1 << d);
            }
        }
        
        pi->type = ruinTileTypeMap[index];

        if (pi->type == WAR_RUIN_PIECE_NONE)
            s32ListAdd(&invalidPieces, i);
    }
    
    for (s32 i = invalidPieces.count - 1; i >= 0; i--)
        WarRuinPieceListRemoveAt(pieces, invalidPieces.items[i]);

    s32ListFree(&invalidPieces);
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
        {
            if (!hasRuinPieceAtPosition(entity, x + xx, y + yy))
                WarRuinPieceListAdd(pieces, createRuinPiece(x + xx, y + yy));
        }
    }
}

void removeRuinPiece(WarEntity* entity, WarRuinPiece* piece)
{
    WarRuinPieceList* pieces = &entity->ruin.pieces;
    WarRuinPieceListRemove(pieces, *piece);
}