#include "war_state_machine.h"

#include "shl/wstr.h"

#include "war_animations.h"

#include "TracyC.h"

WarStateCollapse* wst_createCollapseState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_createCollapseState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_COLLAPSE, entity->id);
    WarStateCollapse* state = (WarStateCollapse*)wst_deref(context, ref);

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterCollapseState", true);

    WarMap* map = context->map;
    vec2 unitSize = wu_getUnitSize(context, entity);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 position = wmap_mapToTileCoordinatesV(transform->position);

    wanim_removeAnimation(context, entity, wsv_fromCString("littleDamage"));
    wanim_removeAnimation(context, entity, wsv_fromCString("hugeDamage"));

    we_disableComponent(context, entity, COMP_SPRITE);

    WarSpriteAnimation collapseAnim = wanim_createCollapseAnimation(context, entity, wstr_fromCString("collapse"));

    state->delay = wmap_getMapScaledTime(context, wanim_getAnimationDuration(&collapseAnim));

    WarEntity* ruins = map->editing.ruin;
    we_addRuinsPieces(context, ruins, (s32)position.x, (s32)position.y, (s32)unitSize.x);
    we_determineRuinTypes(context, ruins);

    setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    wmap_removeEntityFromSelection(context, entity->id);

    TracyCZoneEnd(ctx);
}

void wst_leaveCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveCollapseState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateCollapseState", true);

    NOT_USED(context);
    NOT_USED(state);

    we_removeEntityById(context, entity->id);

    TracyCZoneEnd(ctx);
}

void wst_freeCollapseState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeCollapseState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
