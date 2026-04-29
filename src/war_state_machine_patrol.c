#include "war_state_machine.h"

WarState* wst_createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    WarState* state = wst_createState(context, entity, WAR_STATE_PATROL);
    state->patrol.dir = 1;
    vec2ListInit(&state->patrol.positions, vec2ListDefaultOptions);
    vec2ListAddRange(&state->patrol.positions, positionCount, positions);
    return state;
}

void wst_enterPatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (state->patrol.positions.count <= 1)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }

        return;
    }

    WarState* moveState = wst_createMoveState(context, entity, state->patrol.positions.count, vec2ListToArray(&state->patrol.positions));
    moveState->nextState = state;
    wst_changeNextState(context, entity, moveState, false, true);
}

void wst_leavePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updatePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    vec2List positions = state->patrol.positions;

    // if the unit isn't where is suppose to be, then there must have been a problem in the move, so abort and go idle
    vec2 actualPosition = wmap_vec2MapToTileCoordinates(entity->transform.position);
    vec2 shouldBeAt = positions.items[positions.count - 1];

    f32 distance = vec2Distance(actualPosition, shouldBeAt);
    if (distance >= MOVE_EPSILON)
    {
        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);

        return;
    }

    // otherwise, reverse the positions list and go to the move state again
    state->patrol.dir *= -1;
    vec2ListReverse(&state->patrol.positions);

    vec2* positionsToMove = vec2ListToArray(&state->patrol.positions);

    WarState* moveState = wst_createMoveState(context, entity, state->patrol.positions.count, positionsToMove);
    moveState->nextState = state;
    wst_changeNextState(context, entity, moveState, false, true);

    wm_free(positionsToMove);
}

void wst_freePatrolState(WarContext* context, WarState* state)
{
    vec2ListFree(&state->patrol.positions);
}
