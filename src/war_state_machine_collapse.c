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

    if (!state->initialized)
    {
        WarMap* map = context->map;
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

        state->initialized = true;
        TracyCZoneEnd(ctx);
        return;
    }

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

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->depth == 0 || sm->stack[sm->depth - 1].type != WAR_STATE_COLLAPSE || sm->stack[sm->depth - 1].idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateCollapseState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
