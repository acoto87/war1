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
    return state;
}

void changeNextState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    stateMachine->nextState = state;
}

void enterState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);

    switch (state->type)
    {
        case WAR_STATE_IDLE:
        {
            setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, false);
            break;
        }
    
        case WAR_STATE_MOVE:
        {
            s32 currentIndex = state->move.currentIndex;
            s32 nextIndex = state->move.nextIndex;
            WarMapPath* path = &state->move.path;

            assert(path->nodes.count > 1);
            assert(inRange(currentIndex, 0, path->nodes.count));
            assert(inRange(nextIndex, 0, path->nodes.count));

            vec2 currentNode = path->nodes.items[currentIndex];
            vec2 nextNode = path->nodes.items[nextIndex];

            if (getTileValue(map->finder, nextNode.x, nextNode.y))
            {
                vec2 lastNode = path->nodes.items[path->nodes.count - 1];

                WarMapPath newPath = findPath(map->finder, currentNode.x, currentNode.y, lastNode.x, lastNode.y);

                // remove the remaining nodes of the current path (make a RemoveRange function for lists)
                for(s32 i = path->nodes.count - 1; i > currentIndex; i--)
                    Vec2ListRemoveAt(&path->nodes, i);

                if (newPath.nodes.count > 0)
                {
                    // if a path was found subsitute the rest of the current path for the new one
                    for(s32 i = 1; i < newPath.nodes.count; i++)
                        Vec2ListAdd(&path->nodes, newPath.nodes.items[i]);

                    currentNode = path->nodes.items[currentIndex];
                    nextNode = path->nodes.items[nextIndex];
                }

                freePath(newPath);
            }

            if (path->nodes.count > 1)
            {
                setBlockTiles(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y);
                setBlockTiles(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);

                WarUnitDirection direction = getDirectionFromDiff(nextNode.x - currentNode.x, nextNode.y - currentNode.y);
                setUnitDirection(entity, direction);

                setAction(context, entity, WAR_ACTION_TYPE_WALK, true, false);
            }
            else
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState);
            }
            
            break;
        }

        case WAR_STATE_PATROL:
        {
            s32 currentIndex = state->patrol.currentIndex;
            s32 nextIndex = state->patrol.nextIndex;
            WarMapPath* path = &state->patrol.path;

            assert(path->nodes.count > 1);
            assert(inRange(currentIndex, 0, path->nodes.count));
            assert(inRange(nextIndex, 0, path->nodes.count));

            vec2 currentNode = path->nodes.items[currentIndex];
            vec2 nextNode = path->nodes.items[nextIndex];

            if (getTileValue(map->finder, nextNode.x, nextNode.y))
            {
                vec2 lastNode = path->nodes.items[path->nodes.count - 1];

                WarMapPath newPath = findPath(map->finder, currentNode.x, currentNode.y, lastNode.x, lastNode.y);

                // remove the remaining nodes of the current path (make a RemoveRange function for lists)
                for(s32 i = path->nodes.count - 1; i > currentIndex; i--)
                    Vec2ListRemoveAt(&path->nodes, i);

                if (newPath.nodes.count > 0)
                {
                    // if a path was found subsitute the rest of the current path for the new one
                    for(s32 i = 1; i < newPath.nodes.count; i++)
                        Vec2ListAdd(&path->nodes, newPath.nodes.items[i]);

                    currentNode = path->nodes.items[currentIndex];
                    nextNode = path->nodes.items[nextIndex];
                }

                freePath(newPath);
            }

            if (path->nodes.count > 1)
            {
                setBlockTiles(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y);
                setBlockTiles(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);

                WarUnitDirection direction = getDirectionFromDiff(nextNode.x - currentNode.x, nextNode.y - currentNode.y);
                setUnitDirection(entity, direction);

                setAction(context, entity, WAR_ACTION_TYPE_WALK, true, false);
            }
            else
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState);
            }

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
    
    switch (state->type)
    {
        case WAR_STATE_MOVE:
        {
            freePath(state->move.path);
            break;
        }

        case WAR_STATE_PATROL:
        {
            freePath(state->patrol.path);
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

    s32 currentIndex = state->move.currentIndex;
    s32 nextIndex = state->move.nextIndex;
    WarMapPath* path = &state->move.path;

    assert(path->nodes.count > 1);
    assert(inRange(currentIndex, 0, path->nodes.count));
    assert(inRange(nextIndex, 0, path->nodes.count));

    vec2 currentNode = path->nodes.items[currentIndex];
    vec2 nextNode = path->nodes.items[nextIndex];

    vec2 position = getUnitCenterPosition(entity);
    vec2 target = vec2TileToMapCoordinates(nextNode, true);

    vec2 direction = vec2Normalize(vec2Subv(target, position));
    vec2 step = vec2Mulf(direction, 20 * context->deltaTime);
    vec2 newPosition = vec2Addv(position, step);

    f32 distance = vec2Distance(newPosition, target);
    if (distance < 0.1f)
    {
        newPosition = target;

        vec2 unitSize = getUnitSize(entity);

        setFreeTiles(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y);
        setFreeTiles(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y);

        currentIndex++;
        nextIndex++;

        currentNode = path->nodes.items[currentIndex];

        setBlockTiles(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y);

        // if there is nodes left, check to see if the next one is occupied
        if (nextIndex < path->nodes.count)
        {
            nextNode = path->nodes.items[nextIndex];
            
            if (getTileValue(map->finder, (s32)nextNode.x, (s32)nextNode.y))
            {
                // get the destination position of current path
                s32 lastIndex = path->nodes.count - 1;
                vec2 lastNode = path->nodes.items[lastIndex];

                // find a new path from the current position to the destination
                WarMapPath newPath = findPath(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)lastNode.x, (s32)lastNode.y);
                
                // remove the remaining nodes of the current path (make a RemoveRange function for lists)
                for(s32 i = path->nodes.count - 1; i > currentIndex; i--)
                    Vec2ListRemoveAt(&path->nodes, i);

                if (newPath.nodes.count > 0)
                {
                    // if a path was found subsitute the rest of the current path for the new one
                    for(s32 i = 1; i < newPath.nodes.count; i++)
                        Vec2ListAdd(&path->nodes, newPath.nodes.items[i]);
                }

                freePath(newPath);
            }

            if (nextIndex < path->nodes.count)
            {
                nextNode = path->nodes.items[nextIndex];

                setBlockTiles(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);

                WarUnitDirection direction = getDirectionFromDiff(nextNode.x - currentNode.x, nextNode.y - currentNode.y);
                setUnitDirection(entity, direction);

                state->move.currentIndex = currentIndex;
                state->move.nextIndex = nextIndex;
            }
            else
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState);
            }
        }
        else
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState);
        }
    }

    setUnitCenterPosition(entity, newPosition);
}

void updatePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    s32 currentIndex = state->patrol.currentIndex;
    s32 nextIndex = state->patrol.nextIndex;
    s32 dir = state->patrol.dir;
    WarMapPath* path = &state->patrol.path;

    assert(path->nodes.count > 1);
    assert(inRange(currentIndex, 0, path->nodes.count));
    assert(inRange(nextIndex, 0, path->nodes.count));
    assert(inRange(dir, -1, 2));

    vec2 currentNode = path->nodes.items[currentIndex];
    vec2 nextNode = path->nodes.items[nextIndex];

    vec2 position = getUnitCenterPosition(entity);
    vec2 target = vec2TileToMapCoordinates(nextNode, true);

    vec2 direction = vec2Normalize(vec2Subv(target, position));
    vec2 step = vec2Mulf(direction, 20 * context->deltaTime);
    vec2 newPosition = vec2Addv(position, step);

    f32 distance = vec2Distance(newPosition, target);
    logDebug("%d, %f", entity->id, distance);
    if (distance < 0.2f)
    {
        newPosition = target;

        vec2 unitSize = getUnitSize(entity);

        setFreeTiles(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y);
        setFreeTiles(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y);

        currentIndex += dir;
        if (currentIndex == 0 || currentIndex == path->nodes.count - 1)
            dir *= -1;

        nextIndex += dir;

        currentNode = path->nodes.items[currentIndex];
        nextNode = path->nodes.items[nextIndex];

        setBlockTiles(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y);
        
        if (getTileValue(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // get the destination position of current path
            s32 lastIndex = dir > 0 ? path->nodes.count - 1 : 0;
            vec2 lastNode = path->nodes.items[lastIndex];

            // find a new path from the current position to the destination
            WarMapPath newPath = findPath(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)lastNode.x, (s32)lastNode.y);
            
            if (dir > 0)
            {
                // remove the remaining nodes of the current path (make a RemoveRange function for lists)
                for(s32 i = path->nodes.count - 1; i > currentIndex; i--)
                    Vec2ListRemoveAt(&path->nodes, i);

                if (newPath.nodes.count > 0)
                {
                    // if a path was found subsitute the rest of the current path for the new one
                    for(s32 i = 1; i < newPath.nodes.count; i++)
                        Vec2ListAdd(&path->nodes, newPath.nodes.items[i]);
                }
            }
            else
            {
                for(s32 i = nextIndex; i >= 0; i--)
                    Vec2ListRemoveAt(&path->nodes, i);

                if (newPath.nodes.count > 0)
                {
                    // if a path was found subsitute the rest of the current path for the new one
                    for(s32 i = 1; i < newPath.nodes.count; i++)
                        Vec2ListInsert(&path->nodes, 0, newPath.nodes.items[i]);
                }   
            }

            freePath(newPath);
        }

        if (inRange(nextIndex, 0, path->nodes.count))
        {
            nextNode = path->nodes.items[nextIndex];

            setBlockTiles(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y);

            WarUnitDirection direction = getDirectionFromDiff(nextNode.x - currentNode.x, nextNode.y - currentNode.y);
            setUnitDirection(entity, direction);

            state->patrol.currentIndex = currentIndex;
            state->patrol.nextIndex = nextIndex;
            state->patrol.dir = dir;
        }
        else
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState);
        }
    }

    setUnitCenterPosition(entity, newPosition);
}

void updateStateMachine(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    if (stateMachine->enabled)
    {
        // the enterState could potentially change state if it determine that is not ready to start the current state
        while (stateMachine->nextState)
        {
            leaveState(context, entity, stateMachine->currentState);
            
            stateMachine->currentState = stateMachine->nextState;
            stateMachine->nextState = NULL;

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
