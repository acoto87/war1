#include "war_state_machine.h"

#include "war_actions.h"

#include "TracyC.h"

WarStateWait* wst_createWaitState(WarContext* context, WarEntity* entity, f32 waitTime)
{
    TracyCZoneN(ctx, "wst_createWaitState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_WAIT, entity->id);
    WarStateWait* state = (WarStateWait*)wst_deref(context, ref);
    state->waitEndGameTime = context->gameTime + waitTime;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveWaitState", true);

    if (!state->initialized)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    NOT_USED(state);

    WarMap* map = context->map;

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    TracyCZoneEnd(ctx);
}

void wst_updateWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateWaitState", true);

    WarStateWait* s = (WarStateWait*)state;

    if (!state->initialized)
    {
        WarMap* map = context->map;

        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        vec2 unitSize = wu_getUnitSize(context, entity);
        vec2 position = wmap_mapToTileCoordinatesV(transform->position);
        setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
        wact_setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, 1.0f);

        state->initialized = true;
        TracyCZoneEnd(ctx);
        return;
    }

    if (context->gameTime >= s->waitEndGameTime)
    {
        wst_popState(context, entity);
    }

    TracyCZoneEnd(ctx);
}


void wst_updateWaitStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateWaitStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateWait*      states  = storage->wait;
    bool*             occupied = storage->occupied[WAR_STATE_WAIT];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateWait*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->depth == 0 || sm->stack[sm->depth - 1].type != WAR_STATE_WAIT || sm->stack[sm->depth - 1].idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateWaitState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
