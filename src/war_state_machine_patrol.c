#include <string.h>

#include "war_state_machine.h"

#include "TracyC.h"

WarStatePatrol* wst_createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    TracyCZoneN(ctx, "wst_createPatrolState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_PATROL, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStatePatrol* state = (WarStatePatrol*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->dir = 1;
    state->waypointsIndex = 0;
    state->waypointsCount = MIN(positionCount, 64);
    memcpy(state->waypoints, positions, state->waypointsCount * sizeof(vec2));

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterPatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterPatrolState", true);

    WarStatePatrol* s = (WarStatePatrol*)state;

    if (s->waypointsCount <= 1)
    {
        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_NO_DESTINATION);
        TracyCZoneEnd(ctx);
        return;
    }

    WarStateMove* moveState = wst_createMoveState(context, entity, s->waypointsCount, s->waypoints, true);
    wst_pushState(context, entity, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_COMPLETION);

    TracyCZoneEnd(ctx);
}

void wst_updatePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updatePatrolState", true);

    WarStatePatrol* s = (WarStatePatrol*)state;

    // if the unit isn't where is suppose to be, then there must have been a problem in the move, so abort and go idle
    vec2 actualPosition = wu_getUnitCenterPosition(context, entity);
    vec2 shouldBeAt = s->waypoints[s->waypointsCount - 1];

    f32 distanceSq = vec2_distanceSqr(actualPosition, shouldBeAt);
    if (distanceSq >= MOVE_EPSILON * MOVE_EPSILON)
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);

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

    WarStateMove* moveState = wst_createMoveState(context, entity, s->waypointsCount, s->waypoints, true);
    wst_pushState(context, entity, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_COMPLETION);

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

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updatePatrolState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
