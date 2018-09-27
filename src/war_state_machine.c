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
    state->move.index = 0;
    return state;
}

WarState* createPatrolState(WarContext* context, WarEntity* entity, WarMapPath path)
{
    WarState* state = createState(context, entity, WAR_STATE_PATROL);
    state->updateFrequency = SECONDS_PER_FRAME;
    state->nextUpdateTime = 0;
    state->patrol.path = path;
    state->patrol.index = 0;
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
    switch (state->type)
    {
        case WAR_STATE_IDLE:
        {
            setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, false);
            break;
        }
    
        case WAR_STATE_MOVE:
        {
            s32 index = state->move.index;
            WarMapPath path = state->move.path;

            assert(path.nodes.count > 0);
            assert(inRange(index, 0, path.nodes.count));

            vec2 position = getUnitCenterPosition(entity);
            vec2 target = vec2TileToMapCoordinates(path.nodes.items[index], true);
            WarUnitDirection direction = getDirectionFromDiff(target.x - position.x, target.y - position.y);
            setUnitDirection(entity, direction);

            setAction(context, entity, WAR_ACTION_TYPE_WALK, true, false);
            break;
        }

        case WAR_STATE_PATROL:
        {
            s32 index = state->move.index;
            WarMapPath path = state->move.path;

            assert(path.nodes.count > 0);
            assert(inRange(index, 0, path.nodes.count));

            vec2 position = getUnitCenterPosition(entity);
            vec2 target = vec2TileToMapCoordinates(path.nodes.items[index], true);
            WarUnitDirection direction = getDirectionFromDiff(target.x - position.x, target.y - position.y);
            setUnitDirection(entity, direction);

            setAction(context, entity, WAR_ACTION_TYPE_WALK, true, false);
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

    s32 index = state->move.index;
    WarMapPath* path = &state->move.path;

    assert(path->nodes.count > 0);
    assert(inRange(index, 0, path->nodes.count));

    vec2 currentNode = path->nodes.items[index];

    // if there is nodes left, check to see if the next one is occupied
    if (index < path->nodes.count - 1)
    {
        s32 nextNodeIndex = index + 1;
        vec2 nextNode = path->nodes.items[nextNodeIndex];
        
        if (getTileValue(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // what this code tries to do is minimize the path finding operations by just searching alternative paths
            // for the blocked part of the current part, but it gives weird behaviours, like when the alternative route
            // go around a building but giving not the shortest route to the destiny, because it has to go to the calculated
            // non-blocked next node.
            // 
            // so, for now is better to search for an alternative path directly to the destination.
            // 
            // nextNodeIndex++;
            // nextNode = path.nodes.items[nextNodeIndex];

            // // if the next node is block, search for the next that isn't
            // while (nextNodeIndex < path.nodes.count && getTileValue(map->finder, (s32)nextNode.x, (s32)nextNode.y))
            // {
            //     nextNodeIndex++;
            //     nextNode = path.nodes.items[nextNodeIndex];
            // }

            // if (nextNodeIndex < path.nodes.count)
            // {
            //     // if we found one, re-route the path from the current node to this non-blocked node
            //     WarMapPath newPath = findPath(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)nextNode.x, (s32)nextNode.y);

            //     if (newPath.nodes.count > 0)
            //     {
            //         // if there is a path, delete the blocked nodes
            //         for(s32 i = nextNodeIndex; i >= index; i--)
            //             Vec2ListRemoveAt(&path.nodes, i);
                    
            //         // and insert the new nodes into the current path
            //         for(s32 i = newPath.nodes.count - 1; i >= 0; i--)
            //             Vec2ListInsert(&path.nodes, index, newPath.nodes.items[i]);
            //     }
            //     else
            //     {
            //         // if there is no path to the next node, then there is no path to the destination, stay idle in the position
            //         WarState* idleState = createIdleState(context, entity, true);
            //         changeNextState(context, entity, idleState);
            //     }

            //     freePath(newPath);
            // }
            // else
            // {
            //     // if there is not non-blocked node, then there is no path to the destination, stay idle in the position
            //     WarState* idleState = createIdleState(context, entity, true);
            //     changeNextState(context, entity, idleState);
            // }
            
            // get the destination position of current path
            vec2 lastNode = path->nodes.items[path->nodes.count - 1];
            logDebug("Re-routing for entity %d to: ", entity->id);
            vec2Print(lastNode);

            // find a new path from the current position to the destination
            WarMapPath newPath = findPath(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)lastNode.x, (s32)lastNode.y);

            logDebug("current path");
            
            for(s32 i = 0; i < path->nodes.count; i++)
                printf("(%d, %d), ", (s32)path->nodes.items[i].x, (s32)path->nodes.items[i].y);

            printf("\n");
            
            // remove the remaining nodes of the current path (make a RemoveRange function for lists)
            for(s32 i = path->nodes.count - 1; i > index; i--)
                Vec2ListRemoveAt(&path->nodes, i);

            if (newPath.nodes.count > 0)
            {
                // if a path was found subsitute the rest of the current path for the new one
                for(s32 i = 1; i < newPath.nodes.count; i++)
                    Vec2ListAdd(&path->nodes, newPath.nodes.items[i]);

                freePath(newPath);
            }

            logDebug("new path");
            
            for(s32 i = 0; i < path->nodes.count; i++)
                printf("(%d, %d), ", (s32)path->nodes.items[i].x, (s32)path->nodes.items[i].y);

            printf("\n\n");
        }
    }

    vec2 position = getUnitCenterPosition(entity);
    vec2 target = vec2TileToMapCoordinates(currentNode, true);

    vec2 direction = vec2Normalize(vec2Subv(target, position));
    vec2 step = vec2Mulf(direction, 20 * context->deltaTime);
    vec2 newPosition = vec2Addv(position, step);

    f32 distance = vec2Distance(newPosition, target);
    if (distance < 0.1f)
    {
        newPosition = target;

        index++;

        if (index < path->nodes.count)
        {
            target = vec2TileToMapCoordinates(path->nodes.items[index], true);
            
            WarUnitDirection direction = getDirectionFromDiff(target.x - newPosition.x, target.y - newPosition.y);
            setUnitDirection(entity, direction);

            state->move.index = index;
        }
        else
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState);
        }
    }

    vec2 unitSize = getUnitSize(entity);
    
    position = vec2MapToTileCoordinates(position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    
    setUnitCenterPosition(entity, newPosition);
    
    newPosition = vec2MapToTileCoordinates(newPosition);
    setBlockTiles(map->finder, (s32)newPosition.x, (s32)newPosition.y, (s32)unitSize.x, (s32)unitSize.y);
}

void updatePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    s32 index = state->patrol.index;
    s32 dir = state->patrol.dir;
    WarMapPath* path = &state->patrol.path;

    assert(path->nodes.count > 0);
    assert(inRange(index, 0, path->nodes.count));
    assert(inRange(dir, -1, 2));

    vec2 currentNode = path->nodes.items[index];

    // if there is nodes left, check to see if the next one is occupied
    if (index < path->nodes.count - 1)
    {
        s32 nextNodeIndex = index + 1;
        vec2 nextNode = path->nodes.items[nextNodeIndex];
        
        if (getTileValue(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // what this code tries to do is minimize the path finding operations by just searching alternative paths
            // for the blocked part of the current part, but it gives weird behaviours, like when the alternative route
            // go around a building but giving not the shortest route to the destiny, because it has to go to the calculated
            // non-blocked next node.
            // 
            // so, for now is better to search for an alternative path directly to the destination.
            // 
            // nextNodeIndex++;
            // nextNode = path.nodes.items[nextNodeIndex];

            // // if the next node is block, search for the next that isn't
            // while (nextNodeIndex < path.nodes.count && getTileValue(map->finder, (s32)nextNode.x, (s32)nextNode.y))
            // {
            //     nextNodeIndex++;
            //     nextNode = path.nodes.items[nextNodeIndex];
            // }

            // if (nextNodeIndex < path.nodes.count)
            // {
            //     // if we found one, re-route the path from the current node to this non-blocked node
            //     WarMapPath newPath = findPath(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)nextNode.x, (s32)nextNode.y);

            //     if (newPath.nodes.count > 0)
            //     {
            //         // if there is a path, delete the blocked nodes
            //         for(s32 i = nextNodeIndex; i >= index; i--)
            //             Vec2ListRemoveAt(&path.nodes, i);
                    
            //         // and insert the new nodes into the current path
            //         for(s32 i = newPath.nodes.count - 1; i >= 0; i--)
            //             Vec2ListInsert(&path.nodes, index, newPath.nodes.items[i]);
            //     }
            //     else
            //     {
            //         // if there is no path to the next node, then there is no path to the destination, stay idle in the position
            //         WarState* idleState = createIdleState(context, entity, true);
            //         changeNextState(context, entity, idleState);
            //     }

            //     freePath(newPath);
            // }
            // else
            // {
            //     // if there is not non-blocked node, then there is no path to the destination, stay idle in the position
            //     WarState* idleState = createIdleState(context, entity, true);
            //     changeNextState(context, entity, idleState);
            // }
            
            // get the destination position of current path
            vec2 lastNode = path->nodes.items[path->nodes.count - 1];
            logDebug("Re-routing for entity %d to: ", entity->id);
            vec2Print(lastNode);

            // find a new path from the current position to the destination
            WarMapPath newPath = findPath(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)lastNode.x, (s32)lastNode.y);

            logDebug("current path");
            
            for(s32 i = 0; i < path->nodes.count; i++)
                printf("(%d, %d), ", (s32)path->nodes.items[i].x, (s32)path->nodes.items[i].y);

            printf("\n");
            
            // remove the remaining nodes of the current path (make a RemoveRange function for lists)
            for(s32 i = path->nodes.count - 1; i > index; i--)
                Vec2ListRemoveAt(&path->nodes, i);

            if (newPath.nodes.count > 0)
            {
                // if a path was found subsitute the rest of the current path for the new one
                for(s32 i = 1; i < newPath.nodes.count; i++)
                    Vec2ListAdd(&path->nodes, newPath.nodes.items[i]);

                freePath(newPath);
            }

            logDebug("new path");
            
            for(s32 i = 0; i < path->nodes.count; i++)
                printf("(%d, %d), ", (s32)path->nodes.items[i].x, (s32)path->nodes.items[i].y);

            printf("\n\n");
        }
    }

    vec2 position = getUnitCenterPosition(entity);
    vec2 target = vec2TileToMapCoordinates(path->nodes.items[index], true);

    vec2 direction = vec2Normalize(vec2Subv(target, position));
    vec2 step = vec2Mulf(direction, 20 * context->deltaTime);
    vec2 newPosition = vec2Addv(position, step);

    f32 distance = vec2Distance(newPosition, target);
    if (distance < 0.1f)
    {
        newPosition = target;

        index += dir;

        if (!inRange(index, 0, path->nodes.count))
        {
            dir *= -1;
            index += dir;
        }

        target = vec2TileToMapCoordinates(path->nodes.items[index], true);
        
        WarUnitDirection direction = getDirectionFromDiff(target.x - newPosition.x, target.y - newPosition.y);
        setUnitDirection(entity, direction);

        state->patrol.dir = dir;
        state->patrol.index = index;
    }

    vec2 unitSize = getUnitSize(entity);
    
    position = vec2MapToTileCoordinates(position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    
    setUnitCenterPosition(entity, newPosition);
    
    newPosition = vec2MapToTileCoordinates(newPosition);
    setBlockTiles(map->finder, (s32)newPosition.x, (s32)newPosition.y, (s32)unitSize.x, (s32)unitSize.y);
}

void updateStateMachine(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    if (stateMachine->enabled)
    {
        if (stateMachine->nextState)
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
