#include "war_state_machine.h"

#include "shl/wstr.h"

#include "war_animations.h"

#include "TracyC.h"

WarStateCollapse* wst_createCollapseState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_createCollapseState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_COLLAPSE, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateCollapse* state = (WarStateCollapse*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterCollapseState", true);

    WarMap* map = context->map;
    assert(map);

    vec2 unitSize = wu_getUnitSize(context, entity);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 position = wmap_mapToTileCoordinatesV(transform->position);

    wanim_removeAnimation(context, entity, wsv_fromCString("littleDamage"));
    wanim_removeAnimation(context, entity, wsv_fromCString("hugeDamage"));

    we_disableComponent(context, entity, COMP_SPRITE);

    WarSpriteAnimation collapseAnim = wanim_createCollapseAnimation(context, entity, wstr_fromCString("collapse"));

    state->nextUpdateGameTime = context->gameTime + wmap_getMapScaledTime(context, wanim_getAnimationDuration(&collapseAnim));

    WarEntity* ruins = map->editing.ruin;
    we_addRuinsPieces(context, ruins, (s32)position.x, (s32)position.y, (s32)unitSize.x);
    we_determineRuinTypes(context, ruins);

    wpath_setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    wmap_removeEntityFromSelection(context, entity->id);

    TracyCZoneEnd(ctx);
}

void wst_updateCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateCollapseState", true);

    NOT_USED(state);

    we_removeEntityById(context, entity->id);

    TracyCZoneEnd(ctx);
}


void wst_updateCollapseStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateCollapseStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateCollapse*      states  = storage->collapse;
    bool*             occupied = storage->occupied[WAR_STATE_COLLAPSE];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateCollapse*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateCollapseState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
