#include "war_entities.h"

#include <assert.h>

bool we_hasRuinPieceAtPosition(WarContext* context, WarEntity* ruins, s32 x, s32 y)
{
    WarRuinComponent* ruin = we_getRuinComponent(context, ruins);
    assert(ruin);

    WarRuinPieceList* pieces = &ruin->pieces;
    for (s32 i = 0; i < pieces->count; i++)
    {
        WarRuinPiece* piece = &pieces->items[i];
        if (piece->tilex == x && piece->tiley == y)
            return true;
    }

    return false;
}

WarRuinPiece* we_getRuinPieceAtPosition(WarContext* context, WarEntity* ruins, s32 x, s32 y)
{
    WarRuinComponent* ruin = we_getRuinComponent(context, ruins);
    assert(ruin);

    WarRuinPieceList* pieces = &ruin->pieces;
    for (s32 i = 0; i < pieces->count; i++)
    {
        WarRuinPiece* piece = &pieces->items[i];
        if (piece->tilex == x && piece->tiley == y)
            return piece;
    }

    return NULL;
}

void we_determineRuinTypes(WarContext* context, WarEntity* entity)
{
    assert(entity);
    assert(entity->type == WAR_ENTITY_TYPE_RUIN);

    WarMap* map = context->map;

    WarRuinComponent* ruinComp = we_getRuinComponent(context, entity);
    assert(ruinComp);

    WarRuinPieceList* pieces = &ruinComp->pieces;

    const s32 dirC = 8;
    const s32 dirX[] = { -1,  0,  1, 1, 1, 0, -1, -1 };
    const s32 dirY[] = { -1, -1, -1, 0, 1, 1,  1,  0 };

    S32List invalidPieces;
    S32ListInit(&invalidPieces, wm_frameAllocator());

    for(s32 i = 0; i < pieces->count; i++)
    {
        WarRuinPiece* pi = &pieces->items[i];

        s32 index = 0;

        for (s32 d = 0; d < dirC; d++)
        {
            s32 xx = pi->tilex + dirX[d];
            s32 yy = pi->tiley + dirY[d];

            if (!wpath_isInside(map->finder, xx, yy) || we_hasRuinPieceAtPosition(context, entity, xx, yy))
            {
                index = index | (1 << d);
            }
        }

        pi->type = ruinTileTypeMap[index];

        if (pi->type == WAR_RUIN_PIECE_NONE)
            S32ListAdd(&invalidPieces, i);
    }

    for (s32 i = invalidPieces.count - 1; i >= 0; i--)
        WarRuinPieceListRemoveAt(pieces, invalidPieces.items[i]);

    S32ListFree(&invalidPieces);
}

WarEntity* we_createRuins(WarContext* context)
{
    WarMap* map = context->map;

    WarRuinPieceList pieces;
    WarRuinPieceListInit(&pieces, wm_globalAllocator());

    WarEntity *entity = we_createEntity(context, WAR_ENTITY_TYPE_RUIN, true);
    we_addRuinComponent(context, entity, pieces);
    we_addSpriteComponent(context, entity, WAR_SPRITE_COMPONENT_INIT(
        .sprite = map->sprite
    ));

    return entity;
}

void we_addRuinsPieces(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 dim)
{
    assert(entity);
    assert(entity->type == WAR_ENTITY_TYPE_RUIN);

    WarRuinComponent* ruinComp = we_getRuinComponent(context, entity);
    assert(ruinComp);

    WarRuinPieceList* pieces = &ruinComp->pieces;

    for(s32 yy = 0; yy < dim; yy++)
    {
        for(s32 xx = 0; xx < dim; xx++)
        {
            if (!we_hasRuinPieceAtPosition(context, entity, x + xx, y + yy))
                WarRuinPieceListAdd(pieces, createRuinPiece(x + xx, y + yy));
        }
    }
}

void we_removeRuinPiece(WarContext* context, WarEntity* entity, WarRuinPiece* piece)
{
    WarRuinComponent* ruinComp = we_getRuinComponent(context, entity);
    assert(ruinComp);

    WarRuinPieceList* pieces = &ruinComp->pieces;
    WarRuinPieceListRemove(pieces, *piece, we_equalsRuinPiece);
}
