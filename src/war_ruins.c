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