WarState* createState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarState* state = (WarState*)xcalloc(1, sizeof(WarState));
    state->type = type;
    state->entityId = entity->id;
    return state;
}

WarState* createIdleState(WarContext* context, WarEntity* entity, bool lookAround)
{
    WarState* state = createState(context, entity, WAR_STATE_IDLE);
    state->updateFrequency = 1.0f;
    state->nextUpdateTime = 0;
    state->idle.lookAround = lookAround;
    return state;
}

WarState* createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    WarState* state = createState(context, entity, WAR_STATE_MOVE);
    state->updateFrequency = SECONDS_PER_FRAME;
    state->nextUpdateTime = 0;

    vec2ListInit(&state->move.positions, vec2ListDefaultOptions);
    vec2ListAddRange(&state->move.positions, positionCount, positions);

    return state;
}

WarState* createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    WarState* state = createState(context, entity, WAR_STATE_PATROL);
    state->updateFrequency = SECONDS_PER_FRAME;
    state->nextUpdateTime = 0;
    state->patrol.dir = 1;

    vec2ListInit(&state->patrol.positions, vec2ListDefaultOptions);
    vec2ListAddRange(&state->patrol.positions, positionCount, positions);

    return state;
}

WarState* createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, s32 distance)
{
    WarState* state = createState(context, entity, WAR_STATE_FOLLOW);
    state->updateFrequency = SECONDS_PER_FRAME;
    state->nextUpdateTime = 0;
    state->follow.targetEntityId = targetEntityId;
    state->follow.distance = distance;
    return state;
}

WarState* createAttackState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId)
{
    WarState* state = createState(context, entity, WAR_STATE_ATTACK);
    state->updateFrequency = SECONDS_PER_FRAME;
    state->nextUpdateTime = 0;
    state->attack.targetEntityId = targetEntityId;
    return state;
}

WarState* createWaitState(WarContext* context, WarEntity* entity, f32 waitTime)
{
    WarState* state = createState(context, entity, WAR_STATE_WAIT);
    state->updateFrequency = SECONDS_PER_FRAME;
    state->nextUpdateTime = 0;
    state->wait.waitTime = waitTime;
    return state;
}

void changeNextState(WarContext* context, WarEntity* entity, WarState* state, bool leaveState, bool enterState)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    stateMachine->nextState = state;
    stateMachine->leaveState = leaveState;
    stateMachine->enterState = enterState;
}

bool changeStateNextState(WarContext* context, WarEntity* entity, WarState* state)
{
    if(state->nextState)
    {
        changeNextState(context, entity, state->nextState, true, false);
        state->nextState = NULL;
        return true;
    }

    return false;
}

WarState* getState(WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* state = stateMachine->currentState;
    while (state)
    {
        if (state->type == type)
            return state;

        state = state->nextState;
    }
    return NULL;
}

WarState* getDirectState(WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* state = stateMachine->currentState;
    return state && state->type == type ? state : NULL;
}

WarState* getNextState(WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* state = stateMachine->nextState;
    return state && state->type == type ? state : NULL;
}

#define getIdleState(entity) getDirectState(entity, WAR_STATE_IDLE)
#define getMoveState(entity) getState(entity, WAR_STATE_MOVE)
#define getPatrolState(entity) getState(entity, WAR_STATE_PATROL)

bool hasState(WarEntity* entity, WarStateType type)
{
    return getState(entity, type) != NULL;
}

bool hasDirectState(WarEntity* entity, WarStateType type)
{
    return getDirectState(entity, type) != NULL;
}

bool hasNextState(WarEntity* entity, WarStateType type)
{
    return getNextState(entity, type) != NULL;
}

#define isIdle(entity) hasDirectState(entity, WAR_STATE_IDLE)
#define isMoving(entity) hasState(entity, WAR_STATE_MOVE)
#define isPatrolling(entity) hasState(entity, WAR_STATE_PATROL)
#define isAttacking(entity) hasState(entity, WAR_STATE_ATTACK)

void freeState(WarState* state)
{
    switch (state->type)
    {
        case WAR_STATE_MOVE:
        {
            vec2ListFree(&state->move.positions);
            freePath(state->move.path);
            break;
        }

        case WAR_STATE_PATROL:
        {
            vec2ListFree(&state->patrol.positions);
            break;
        }
    }

    if (state->nextState)
        freeState(state->nextState);

    free(state);
}

void enterState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);

    switch (state->type)
    {
        case WAR_STATE_IDLE:
        case WAR_STATE_WAIT:
        {
            vec2 position = vec2MapToTileCoordinates(entity->transform.position);
            setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
            setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, 1.0f);
            break;
        }
    
        case WAR_STATE_MOVE:
        {
            if (state->move.positions.count <= 1)
            {
                if (!changeStateNextState(context, entity, state))
                {
                    WarState* idleState = createIdleState(context, entity, true);
                    changeNextState(context, entity, idleState, true, true);
                }

                return;
            }

            state->move.positionIndex = 0;
            state->move.pathNodeIndex = 0;
            state->move.waitCount = 0;

            vec2 start = state->move.positions.items[state->move.positionIndex];
            vec2 end = state->move.positions.items[state->move.positionIndex + 1];

            WarMapPath path = state->move.path = findPath(map->finder, start.x, start.y, end.x, end.y);

            // if the is no path to the next position, go to idle
            if (path.nodes.count <= 1)
            {
                if (!changeStateNextState(context, entity, state))
                {
                    WarState* idleState = createIdleState(context, entity, true);
                    changeNextState(context, entity, idleState, true, true);
                }

                freePath(path);
                return;
            }

            vec2 currentNode = path.nodes.items[state->move.pathNodeIndex];
            setDynamicEntity(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y, entity->id);

            vec2 nextNode = path.nodes.items[state->move.pathNodeIndex + 1];
            setDynamicEntity(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y, entity->id);

            setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);

            // level 0 -> 1.0f, level 1 -> 0.9f, level 2 -> 0.8f
            f32 scale = 1 - entity->unit.level * 0.1f;
            setAction(context, entity, WAR_ACTION_TYPE_WALK, true, scale);
            break;
        }

        case WAR_STATE_PATROL:
        {
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
            break;
        }

        case WAR_STATE_FOLLOW:
        {
            // all the logic is already in the update function, do nothing here
            break;
        }

        case WAR_STATE_ATTACK:
        {
            // all the logic is already in the update function, do nothing here
            break;
        }

        default:
        {
            logError("Unkown state %d for entity %d", state->type, entity->id);
            break;
        }
    }
}

void leaveState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (!state)
        return;
    
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);

    switch (state->type)
    {
        case WAR_STATE_IDLE:
        case WAR_STATE_WAIT:
        {
            vec2 position = vec2MapToTileCoordinates(entity->transform.position);
            setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
            break;
        }

        case WAR_STATE_MOVE:
        {
            WarMapPath* path = &state->move.path;

            if (inRange(state->move.pathNodeIndex, 0, path->nodes.count))
            {
                vec2 currentNode = path->nodes.items[state->move.pathNodeIndex];
                if (isDynamicOfEntity(map->finder, currentNode.x, currentNode.y, entity->id))
                    setFreeTiles(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y);
            }

            if (inRange(state->move.pathNodeIndex + 1, 0, path->nodes.count))
            {
                vec2 nextNode = path->nodes.items[state->move.pathNodeIndex + 1];
                if (isDynamicOfEntity(map->finder, nextNode.x, nextNode.y, entity->id))
                    setFreeTiles(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);
            }

            break;
        }

        case WAR_STATE_PATROL:
        {
            break;
        }
    }

    freeState(state);
}

void updateIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (state->idle.lookAround)
    {
        if (chance(20))
        {
            WarUnitComponent* unit = &entity->unit;
            unit->direction += randomi(-1, 2);
            if (unit->direction < 0)
                unit->direction = WAR_DIRECTION_NORTH_WEST;
            else if(unit->direction >= WAR_DIRECTION_COUNT)
                unit->direction = WAR_DIRECTION_NORTH;
        }
    }

    // for(s32 i = 0; i < map->entities.count; i++)
    // {
    //     WarEntity* other = context->map->entities[i];
    //     if (other && other->type == WAR_ENTITY_TYPE_UNIT)
    //     {
    //         WarUnitComponent* unit = &other->unit;
    //         if (isDudeUnit(unit->type) && isEnemy(unit->player))
    //         {
                
    //         }
    //     }
    // }
}

void updateMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarMapPath* path = &state->move.path;

    assert(path->nodes.count > 1);
    assert(inRange(state->move.pathNodeIndex, 0, path->nodes.count - 1));

    WarUnitStats stats = getUnitStats(entity->unit.type);
    vec2 unitSize = getUnitSize(entity);

    vec2 currentNode = path->nodes.items[state->move.pathNodeIndex];
    vec2 nextNode = path->nodes.items[state->move.pathNodeIndex + 1];

    // if this unit is waiting
    if (state->move.waitCount > 0)
    {
        if (!isEmpty(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // wait for a number of times before re-route
            if (state->move.waitCount < MOVE_WAIT_INTENTS)
            {
                state->move.waitCount++;

                WarState* waitState = createWaitState(context, entity, MOVE_WAIT_TIME);
                waitState->nextState = state;
                changeNextState(context, entity, waitState, false, true);

                return;
            }

            state->move.waitCount = 0;

            // if there is no re-routing possible, go to idle
            if (!reRoutePath(map->finder, path, state->move.pathNodeIndex, path->nodes.count - 1))
            {
                if (!changeStateNextState(context, entity, state))
                {
                    WarState* idleState = createIdleState(context, entity, true);
                    changeNextState(context, entity, idleState, true, true);
                }
                
                return;
            }

            nextNode = path->nodes.items[state->move.pathNodeIndex + 1];
        }
        else
        {
            state->move.waitCount = 0;
        }

        setDynamicEntity(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y, entity->id);
        setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);

        // level 0 -> 1.0f, level 1 -> 0.9f, level 2 -> 0.8f
        f32 scale = 1 - entity->unit.level * 0.1f;
        setAction(context, entity, WAR_ACTION_TYPE_WALK, true, scale);
    }

    vec2 position = getUnitCenterPosition(entity);
    vec2 target = vec2TileToMapCoordinates(nextNode, true);

    vec2 direction = vec2Normalize(vec2Subv(target, position));
    f32 speed = stats.speeds[entity->unit.level] * context->globalSpeed;
    vec2 step = vec2Mulf(direction, speed * context->deltaTime);
    vec2 newPosition = vec2Addv(position, step);

    setUnitCenterPosition(entity, newPosition);

    f32 distance = vec2Distance(newPosition, target);
    if (distance < MOVE_EPSILON)
    {
        newPosition = target;
        setUnitCenterPosition(entity, newPosition);

        setFreeTiles(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y);
        setFreeTiles(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y);

        state->move.pathNodeIndex++;

        // if there is no more path nodes to check, go to idle
        if (state->move.pathNodeIndex >= path->nodes.count - 1)
        {
            state->move.positionIndex++;

            // if this is no more segments, go to idle
            if (state->move.positionIndex >= state->move.positions.count - 1)
            {
                if (!changeStateNextState(context, entity, state))
                {
                    WarState* idleState = createIdleState(context, entity, true);
                    changeNextState(context, entity, idleState, true, true);
                }

                return;
            }

            // free the previous path and check if there is a new one
            freePath(*path);

            vec2 start = state->move.positions.items[state->move.positionIndex];
            vec2 end = state->move.positions.items[state->move.positionIndex + 1];

            *path = findPath(map->finder, (s32)start.x, (s32)start.y, (s32)end.x, (s32)end.y);

            // if there is no path for the next segment, go to idle
            if (path->nodes.count <= 1)
            {
                if (!changeStateNextState(context, entity, state))
                {
                    WarState* idleState = createIdleState(context, entity, true);
                    changeNextState(context, entity, idleState, true, true);
                }

                return;
            }

            state->move.pathNodeIndex = 0;
        }

        currentNode = path->nodes.items[state->move.pathNodeIndex];
        setDynamicEntity(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y, entity->id);

        nextNode = path->nodes.items[state->move.pathNodeIndex + 1];

        // if the next node is occupied, check if the unit have to wait to re-route if necessary
        if (!isEmpty(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // if the next node is occupied but is the last one, don't wait to re-route, go idle
            if (state->move.pathNodeIndex + 1 == path->nodes.count - 1)
            {
                if (!changeStateNextState(context, entity, state))
                {
                    WarState* idleState = createIdleState(context, entity, true);
                    changeNextState(context, entity, idleState, true, true);
                }

                return;
            }

            state->move.waitCount++;

            WarState* waitState = createWaitState(context, entity, MOVE_WAIT_TIME);
            waitState->nextState = state;
            changeNextState(context, entity, waitState, false, true);
            
            return;
        }

        setDynamicEntity(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y, entity->id);
        setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
    }
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

void updateFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarEntity* targetEntity = findEntity(context, state->follow.targetEntityId);

    // if the entity to follow doesn't exists, go to idle
    if (!targetEntity)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }

        return;
    }

    vec2 start = vec2MapToTileCoordinates(entity->transform.position);
    vec2 end = vec2MapToTileCoordinates(targetEntity->transform.position);
    f32 distance = vec2DistanceInTiles(start, end);

    // if the unit is already in distance, go to idle
    if (distance <= state->follow.distance)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* waitState = createWaitState(context, entity, true, MOVE_WAIT_TIME);
            waitState->nextState = state;
            changeNextState(context, entity, waitState, false, true);
        }

        return;
    }

    WarMapPath path = state->move.path = findPath(map->finder, start.x, start.y, end.x, end.y);

    // if there is no path to the target, go to idle
    if (path.nodes.count <= 1)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }

        freePath(path);
        return;
    }

    WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, path.nodes.items[0], path.nodes.items[1]));
    moveState->nextState = state;
    changeNextState(context, entity, moveState, false, true);

    freePath(path);
}

void updateWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    state->wait.waitTime -= context->deltaTime;

    if (state->wait.waitTime < 0)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }
    }
}

void updateAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);
    WarUnitStats stats = getUnitStats(entity->unit.type);

    WarEntity* targetEntity = findEntity(context, state->attack.targetEntityId);

    // if the entity to attack doesn't exists, go idle
    if (!targetEntity)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    vec2 targetPosition = vec2MapToTileCoordinates(targetEntity->transform.position);
    f32 distance = vec2DistanceInTiles(targetPosition, position);
    
    // if the unit is not in range to attack, chase it
    if (distance > stats.range)
    {
        WarState* followState = createFollowState(context, entity, state->attack.targetEntityId, stats.range);
        followState->nextState = state;
        changeNextState(context, entity, followState, false, true);

        return;
    }

    setDynamicEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
    setUnitDirectionFromDiff(entity, targetPosition.x - position.x, targetPosition.y - position.y);
    setAction(context, entity, WAR_ACTION_TYPE_ATTACK, false, 1.0f);
}

void updateStateMachine(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    if (stateMachine->enabled)
    {
        // the enterState could potentially change state if it determine that is not ready to start the current state
        while (stateMachine->nextState)
        {
            if (stateMachine->leaveState)
                leaveState(context, entity, stateMachine->currentState);
            
            stateMachine->currentState = stateMachine->nextState;
            stateMachine->nextState = NULL;

            if (stateMachine->enterState)
                enterState(context, entity, stateMachine->currentState);
        }

        WarState* currentState = stateMachine->currentState;

        if (context->time >= currentState->nextUpdateTime)
        {
            switch (currentState->type)
            {
                case WAR_STATE_IDLE:
                {
                    updateIdleState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_MOVE:
                {
                    updateMoveState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_PATROL:
                {
                    updatePatrolState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_FOLLOW:
                {
                    updateFollowState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_WAIT:
                {
                    updateWaitState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_ATTACK:
                {
                    updateAttackState(context, entity, currentState);
                    break;
                }
            
                default:
                {
                    logError("Unkown state %d for entity %d", currentState->type, entity->id);
                    break;
                }
            }

            currentState->nextUpdateTime = context->time + currentState->updateFrequency;
        }
    }
}
