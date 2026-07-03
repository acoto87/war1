#include "war_state_machine.h"

#include "TracyC.h"

WarState* wst_createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    TracyCZoneN(ctx, "wst_createPatrolState", true);

    WarState* state = wst_createState(context, entity, WAR_STATE_PATROL);
    state->patrol.dir = 1;
    Vec2ListInit(&state->patrol.positions, wm_globalAllocator());
    Vec2ListAddRange(&state->patrol.positions, positionCount, positions);

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterPatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterPatrolState", true);

    if (state->patrol.positions.count <= 1)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    WarState* moveState = wst_createMoveState(context, entity, state->patrol.positions.count, Vec2ListToArray(&state->patrol.positions));
    moveState->nextState = state;
    wst_changeNextState(context, entity, moveState, false, true);

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

    Vec2List positions = state->patrol.positions;

    // if the unit isn't where is suppose to be, then there must have been a problem in the move, so abort and go idle
    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 actualPosition = wmap_mapToTileCoordinatesV(transform->position);
    vec2 shouldBeAt = positions.items[positions.count - 1];

    f32 distance = vec2_distance(actualPosition, shouldBeAt);
    if (distance >= MOVE_EPSILON)
    {
        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);

        TracyCZoneEnd(ctx);
        return;
    }

    // otherwise, reverse the positions list and go to the move state again
    state->patrol.dir *= -1;
    Vec2ListReverse(&state->patrol.positions);

    vec2* positionsToMove = Vec2ListToArray(&state->patrol.positions);

    WarState* moveState = wst_createMoveState(context, entity, state->patrol.positions.count, positionsToMove);
    moveState->nextState = state;
    wst_changeNextState(context, entity, moveState, false, true);

    wm_free(positionsToMove);

    TracyCZoneEnd(ctx);
}

void wst_freePatrolState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freePatrolState", true);

    NOT_USED(context);

    Vec2ListFree(&state->patrol.positions);

    TracyCZoneEnd(ctx);
}
