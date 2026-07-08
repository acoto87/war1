#include "war_state_machine.h"

#include "TracyC.h"

WarStatePatrol* wst_createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    TracyCZoneN(ctx, "wst_createPatrolState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_PATROL, entity->id);
    WarStatePatrol* state = (WarStatePatrol*)wst_deref(context, ref);
    state->dir = 1;
    Vec2ListInit(&state->positions, wm_globalAllocator());
    Vec2ListAddRange(&state->positions, positionCount, positions);

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterPatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterPatrolState", true);

    WarStatePatrol* s = (WarStatePatrol*)state;

    if (s->positions.count <= 1)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    WarStateMove* moveState = wst_createMoveState(context, entity, s->positions.count, Vec2ListToArray(&s->positions));
    wst_chainNext(context, (WarStateBase*)moveState, (WarStateBase*)state);
    wst_changeNextState(context, entity, (WarStateBase*)moveState, false, true);

    TracyCZoneEnd(ctx);
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

    Vec2List positions = s->positions;

    // if the unit isn't where is suppose to be, then there must have been a problem in the move, so abort and go idle
    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 actualPosition = wmap_mapToTileCoordinatesV(transform->position);
    vec2 shouldBeAt = positions.items[positions.count - 1];

    f32 distance = vec2_distance(actualPosition, shouldBeAt);
    if (distance >= MOVE_EPSILON)
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);

        TracyCZoneEnd(ctx);
        return;
    }

    // otherwise, reverse the positions list and go to the move state again
    s->dir *= -1;
    Vec2ListReverse(&s->positions);

    vec2* positionsToMove = Vec2ListToArray(&s->positions);

    WarStateMove* moveState = wst_createMoveState(context, entity, s->positions.count, positionsToMove);
    wst_chainNext(context, (WarStateBase*)moveState, (WarStateBase*)state);
    wst_changeNextState(context, entity, (WarStateBase*)moveState, false, true);

    wm_free(positionsToMove);

    TracyCZoneEnd(ctx);
}

void wst_freePatrolState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freePatrolState", true);

    NOT_USED(context);

    WarStatePatrol* s = (WarStatePatrol*)state;

    Vec2ListFree(&s->positions);

    TracyCZoneEnd(ctx);
}
