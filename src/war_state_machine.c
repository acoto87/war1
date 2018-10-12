WarState* createState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;

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

WarState* createMoveState(WarContext* context, WarEntity* entity, WarMapPath path)
{
    WarState* state = createState(context, entity, WAR_STATE_MOVE);
    state->updateFrequency = SECONDS_PER_FRAME;
    state->nextUpdateTime = 0;
    state->move.path = path;
    state->move.currentIndex = 0;
    state->move.nextIndex = 1;
    state->move.waitCount = 0;
    return state;
}

WarState* createPatrolState(WarContext* context, WarEntity* entity, WarMapPath path)
{
    WarState* state = createState(context, entity, WAR_STATE_PATROL);
    state->updateFrequency = SECONDS_PER_FRAME;
    state->nextUpdateTime = 0;
    state->patrol.path = path;
    state->patrol.currentIndex = 0;
    state->patrol.nextIndex = 1;
    state->patrol.dir = 1;
    state->patrol.waitCount = 0;
    return state;
}

WarState* createWaitState(WarContext* context, WarEntity* entity, f32 waitTime, WarState* nextState)
{
    WarState* state = createState(context, entity, WAR_STATE_WAIT);
    state->updateFrequency = SECONDS_PER_FRAME;
    state->nextUpdateTime = 0;
    state->wait.waitTime = waitTime;
    state->wait.nextState = nextState;
    return state;
}

void changeNextState(WarContext* context, WarEntity* entity, WarState* state, bool leaveState, bool enterState)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    stateMachine->nextState = state;
    stateMachine->leaveState = leaveState;
    stateMachine->enterState = enterState;
}

bool hasState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    return stateMachine->currentState && stateMachine->currentState->type == type;
}

bool hasNextState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    return stateMachine->nextState && stateMachine->nextState->type == type;
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
            setStaticEntity(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

            setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, false);
            break;
        }
    
        case WAR_STATE_MOVE:
        {
            s32 currentIndex = state->move.currentIndex;
            s32 nextIndex = state->move.nextIndex;
            s32 waitCount = state->move.waitCount;
            WarMapPath* path = &state->move.path;

            assert(path->nodes.count > 1);
            assert(inRange(currentIndex, 0, path->nodes.count));
            assert(inRange(nextIndex, 0, path->nodes.count));

            vec2 currentNode = path->nodes.items[currentIndex];
            setDynamicEntity(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y);

            vec2 nextNode = path->nodes.items[nextIndex];
            if (waitCount == 0)
                setDynamicEntity(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y);

            setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
            setAction(context, entity, WAR_ACTION_TYPE_WALK, true, false);
            break;
        }

        case WAR_STATE_PATROL:
        {
            s32 currentIndex = state->patrol.currentIndex;
            s32 nextIndex = state->patrol.nextIndex;
            s32 waitCount = state->patrol.waitCount;
            WarMapPath* path = &state->patrol.path;

            assert(path->nodes.count > 1);
            assert(inRange(currentIndex, 0, path->nodes.count));
            assert(inRange(nextIndex, 0, path->nodes.count));

            vec2 currentNode = path->nodes.items[currentIndex];
            setDynamicEntity(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y);

            vec2 nextNode = path->nodes.items[nextIndex];
            if (waitCount == 0)
                setDynamicEntity(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y);

            setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
            // setAction(context, entity, WAR_ACTION_TYPE_WALK, true, false);
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

            if (inRange(state->move.currentIndex, 0, path->nodes.count))
            {
                vec2 currentNode = path->nodes.items[state->move.currentIndex];
                setFreeTiles(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y);
            }

            if (inRange(state->move.nextIndex, 0, path->nodes.count))
            {
                vec2 nextNode = path->nodes.items[state->move.nextIndex];
                setFreeTiles(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);
            }

            freePath(*path);
            break;
        }

        case WAR_STATE_PATROL:
        {
            WarMapPath* path = &state->patrol.path;

            if (inRange(state->patrol.currentIndex, 0, path->nodes.count))
            {
                vec2 currentNode = path->nodes.items[state->patrol.currentIndex];
                setFreeTiles(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y);
            }

            if (inRange(state->patrol.nextIndex, 0, path->nodes.count))
            {
                vec2 nextNode = path->nodes.items[state->patrol.nextIndex];
                setFreeTiles(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);
            }

            freePath(*path);
            break;
        }
    }

    free(state);
}

void updateIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (state->idle.lookAround)
    {
        if (chance(50))
        {
            WarUnitComponent* unit = &entity->unit;
            unit->direction += random(-1, 2);
            if (unit->direction < 0)
                unit->direction = WAR_DIRECTION_NORTH_WEST;
            else if(unit->direction >= WAR_DIRECTION_COUNT)
                unit->direction = WAR_DIRECTION_NORTH;

            unit->direction = WAR_DIRECTION_EAST;
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
    assert(inRange(state->move.currentIndex, 0, path->nodes.count));
    assert(inRange(state->move.nextIndex, 0, path->nodes.count));

    WarUnitStats stats = getUnitStats(entity->unit.type);
    vec2 unitSize = getUnitSize(entity);

    vec2 currentNode = path->nodes.items[state->move.currentIndex];
    vec2 nextNode = path->nodes.items[state->move.nextIndex];


    // if this unit is waiting
    if (state->move.waitCount > 0)
    {
        if (!isEmpty(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // wait for a number of times before re-route
            if (state->move.waitCount < MOVE_WAIT_INTENTS)
            {
                state->move.waitCount++;

                WarState* waitState = createWaitState(context, entity, MOVE_WAIT_TIME, state);
                changeNextState(context, entity, waitState, false, true);
                return;
            }

            state->move.waitCount = 0;

            // if there is no re-routing possible, go to idle
            if (!reRoutePath(map->finder, path, state->move.currentIndex, path->nodes.count - 1))
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);
                return;
            }

            nextNode = path->nodes.items[state->move.nextIndex];
        }
        else
        {
            state->move.waitCount = 0;
        }

        setDynamicEntity(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);
        setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
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

        state->move.currentIndex++;
        state->move.nextIndex++;

        // if there is no more path nodes to check, go to idle
        if (state->move.nextIndex >= path->nodes.count)
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
            return;
        }

        currentNode = path->nodes.items[state->move.currentIndex];
        setDynamicEntity(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y);

        nextNode = path->nodes.items[state->move.nextIndex];

        // if the next node is occupied, check if the unit have to wait to re-route if necessary
        if (!isEmpty(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            state->move.waitCount++;

            WarState* waitState = createWaitState(context, entity, MOVE_WAIT_TIME, state);
            changeNextState(context, entity, waitState, false, true);
            return;
        }

        setDynamicEntity(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);
        setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
    }
}

void updatePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarMapPath* path = &state->patrol.path;

    assert(path->nodes.count > 1);
    assert(inRange(state->patrol.currentIndex, 0, path->nodes.count));
    assert(inRange(state->patrol.nextIndex, 0, path->nodes.count));

    WarUnitStats stats = getUnitStats(entity->unit.type);

    vec2 currentNode = path->nodes.items[state->patrol.currentIndex];
    vec2 nextNode = path->nodes.items[state->patrol.nextIndex];

    vec2 unitSize = getUnitSize(entity);

    // if this unit is waiting
    if (state->patrol.waitCount > 0)
    {
        if (!isEmpty(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // wait for a number of times before re-route
            if (state->patrol.waitCount < MOVE_WAIT_INTENTS)
            {
                state->patrol.waitCount++;

                WarState* waitState = createWaitState(context, entity, MOVE_WAIT_TIME, state);
                changeNextState(context, entity, waitState, false, true);
                return;
            }

            state->patrol.waitCount = 0;

            // if there is no re-routing possible, go to idle
            if (!reRoutePath(map->finder, path, state->patrol.currentIndex, state->patrol.dir > 0 ? path->nodes.count - 1 : 0))
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);
                return;
            }

            nextNode = path->nodes.items[state->patrol.nextIndex];
        }
        else
        {
            state->patrol.waitCount = 0;
        }

        setDynamicEntity(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);
        setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
    }

    vec2 position = getUnitCenterPosition(entity);
    vec2 target = vec2TileToMapCoordinates(nextNode, true);

    vec2 direction = vec2Normalize(vec2Subv(target, position));
    vec2 step = vec2Mulf(direction, stats.speeds[0] * context->deltaTime);
    vec2 newPosition = vec2Addv(position, step);

    setUnitCenterPosition(entity, newPosition);

    f32 distance = vec2Distance(newPosition, target);
    if (distance < MOVE_EPSILON)
    {
        newPosition = target;
        setUnitCenterPosition(entity, newPosition);

        setFreeTiles(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y);
        setFreeTiles(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y);

        state->patrol.currentIndex += state->patrol.dir;
        if (state->patrol.currentIndex == 0 || state->patrol.currentIndex == path->nodes.count - 1)
            state->patrol.dir *= -1;

        state->patrol.nextIndex += state->patrol.dir;

        currentNode = path->nodes.items[state->patrol.currentIndex];
        setDynamicEntity(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y);

        nextNode = path->nodes.items[state->patrol.nextIndex];

        // if the next node is occupied, check if the unit have to wait to re-route if necessary
        if (!isEmpty(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            state->patrol.waitCount++;

            WarState* waitState = createWaitState(context, entity, MOVE_WAIT_TIME, state);
            changeNextState(context, entity, waitState, false, true);
            return;
        }

        setDynamicEntity(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);
        setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
    }
}

void updateWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    state->wait.waitTime -= context->deltaTime;

    if (state->wait.waitTime < 0)
    {
        if (state->wait.nextState)
        {
            changeNextState(context, entity, state->wait.nextState, true, true);
        }
        else
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }
    }
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

                case WAR_STATE_WAIT:
                {
                    updateWaitState(context, entity, currentState);
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
