WarState* createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    WarState* state = createState(context, entity, WAR_STATE_PATROL);
    state->patrol.dir = 1;
    vec2ListInit(&state->patrol.positions, vec2ListDefaultOptions);
    vec2ListAddRange(&state->patrol.positions, positionCount, positions);
    return state;
}

void enterPatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);
    
    if (state->patrol.positions.count <= 1)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }
        
        return;
    }

    WarState* moveState = createMoveState(context, entity, state->patrol.positions.count, vec2ListToArray(&state->patrol.positions));
    moveState->nextState = state;
    changeNextState(context, entity, moveState, false, true);
}

void leavePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updatePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    vec2List positions = state->patrol.positions;

    // if the unit isn't where is suppose to be, then there must have been a problem in the move, so abort and go idle
    vec2 actualPosition = vec2MapToTileCoordinates(entity->transform.position);
    vec2 shouldBeAt = positions.items[positions.count - 1];

    f32 distance = vec2Distance(actualPosition, shouldBeAt);
    if (distance >= MOVE_EPSILON)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);

        return;
    }

    // otherwise, reverse the positions list and go to the move state again
    state->patrol.dir *= -1;
    vec2ListReverse(&state->patrol.positions);

    vec2* positionsToMove = vec2ListToArray(&state->patrol.positions);

    WarState* moveState = createMoveState(context, entity, state->patrol.positions.count, positionsToMove);
    moveState->nextState = state;
    changeNextState(context, entity, moveState, false, true);

    free(positionsToMove);
}

void freePatrolState(WarState* state)
{
    vec2ListFree(&state->patrol.positions);
}