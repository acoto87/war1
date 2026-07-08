#include <string.h>

#include "war_state_machine.h"

#include "TracyC.h"

WarStatePatrol* wst_createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    TracyCZoneN(ctx, "wst_createPatrolState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_PATROL, entity->id);
    WarStatePatrol* state = (WarStatePatrol*)wst_deref(context, ref);
    state->dir = 1;
    state->waypointsIndex = 0;
    state->waypointsCount = MIN(positionCount, 64);
    memcpy(state->waypoints, positions, state->waypointsCount * sizeof(vec2));

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leavePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leavePatrolState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updatePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updatePatrolState", true);

    WarStatePatrol* s = (WarStatePatrol*)state;

    if (!state->initialized)
    {
        state->initialized = true;

        if (s->waypointsCount <= 1)
        {
            if (!wst_changeStateNextState(context, entity, state))
            {
                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
            }
            TracyCZoneEnd(ctx);
            return;
        }

        WarStateMove* moveState = wst_createMoveState(context, entity, s->waypointsCount, s->waypoints);
        wst_chainNext(context, (WarStateBase*)moveState, (WarStateBase*)state);
        wst_changeNextState(context, entity, (WarStateBase*)moveState, false);
        TracyCZoneEnd(ctx);
        return;
    }

    // if the unit isn't where is suppose to be, then there must have been a problem in the move, so abort and go idle
    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 actualPosition = wmap_mapToTileCoordinatesV(transform->position);
    vec2 shouldBeAt = s->waypoints[s->waypointsCount - 1];

    f32 distance = vec2_distance(actualPosition, shouldBeAt);
    if (distance >= MOVE_EPSILON)
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, (WarStateBase*)idleState, true);

        TracyCZoneEnd(ctx);
        return;
    }

    // otherwise, reverse the waypoints array and go to the move state again
    s->dir *= -1;

    // reverse the waypoints in-place
    for (s32 i = 0; i < s->waypointsCount / 2; i++)
    {
        vec2 temp = s->waypoints[i];
        s->waypoints[i] = s->waypoints[s->waypointsCount - 1 - i];
        s->waypoints[s->waypointsCount - 1 - i] = temp;
    }

    WarStateMove* moveState = wst_createMoveState(context, entity, s->waypointsCount, s->waypoints);
    wst_chainNext(context, (WarStateBase*)moveState, (WarStateBase*)state);
    wst_changeNextState(context, entity, (WarStateBase*)moveState, false);

    TracyCZoneEnd(ctx);
}


void wst_updatePatrolStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updatePatrolStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStatePatrol*      states  = storage->patrol;
    bool*             occupied = storage->occupied[WAR_STATE_PATROL];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStatePatrol*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->currentRef.type != WAR_STATE_PATROL || sm->currentRef.idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updatePatrolState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
