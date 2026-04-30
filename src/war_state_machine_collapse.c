#include "war_state_machine.h"

#include "shl/wstr.h"

#include "war_animations.h"

WarState* wst_createCollapseState(WarContext* context, WarEntity* entity)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_COLLAPSE);
    return state;
}

void wst_enterCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = wu_getUnitSize(entity);
    vec2 position = wmap_mapToTileCoordinatesV(entity->transform.position);

    wanim_removeAnimation(context, entity, wsv_fromCString("littleDamage"));
    wanim_removeAnimation(context, entity, wsv_fromCString("hugeDamage"));

    // disable the sprite component to just render the animation
    entity->sprite.enabled = false;

    WarSpriteAnimation* collapseAnim = wanim_createCollapseAnimation(context, entity, wstr_fromCString("collapse"));

    setDelay(state, wmap_getMapScaledTime(context, wanim_getAnimationDuration(collapseAnim)));

    WarEntity* ruins = map->ruin;
    we_addRuinsPieces(context, ruins, (s32)position.x, (s32)position.y, (s32)unitSize.x);
    we_determineRuinTypes(context, ruins);

    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    wmap_removeEntityFromSelection(context, entity->id);
}

void wst_leaveCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updateCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    we_removeEntityById(context, entity->id);
}

void wst_freeCollapseState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}
