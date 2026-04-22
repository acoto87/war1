#include "war_state_machine.h"

#include "war_animations.h"

WarState* createCollapseState(WarContext* context, WarEntity* entity)
{
    WarState* state = createState(context, entity, WAR_STATE_COLLAPSE);
    return state;
}

void enterCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);

    removeAnimation(context, entity, wsv_fromCString("littleDamage"));
    removeAnimation(context, entity, wsv_fromCString("hugeDamage"));

    // disable the sprite component to just render the animation
    entity->sprite.enabled = false;

    WarSpriteAnimation* collapseAnim = createCollapseAnimation(context, entity, wstr_fromCString("collapse"));

    setDelay(state, getMapScaledTime(context, getAnimationDuration(collapseAnim)));

    WarEntity* ruins = map->ruin;
    addRuinsPieces(context, ruins, (s32)position.x, (s32)position.y, (s32)unitSize.x);
    determineRuinTypes(context, ruins);

    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    removeEntityFromSelection(context, entity->id);
}

void leaveCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void updateCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    removeEntityById(context, entity->id);
}

void freeCollapseState(WarState* state)
{
    NOT_USED(state);
}
