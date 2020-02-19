WarState* createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    WarState* state = createState(context, entity, WAR_STATE_MOVE);
    vec2ListInit(&state->move.positions, vec2ListDefaultOptions);
    vec2ListAddRange(&state->move.positions, positionCount, positions);
    return state;
}

void enterMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);

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

        return;
    }

    vec2 currentNode = path.nodes.items[state->move.pathNodeIndex];
    setDynamicEntity(map->finder, currentNode.x, currentNode.y, unitSize.x, unitSize.y, entity->id);

    vec2 nextNode = path.nodes.items[state->move.pathNodeIndex + 1];
    setDynamicEntity(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y, entity->id);

    setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
    setAction(context, entity, WAR_ACTION_TYPE_WALK, false, getUnitActionScale(entity));
}

void leaveMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);
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

    if (state->move.checkForAttacks)
    {
        WarEntity* enemy = getAttacker(context, entity);
        if (enemy && areEnemies(context, entity, enemy) && canAttack(context, entity, enemy))
        {
            vec2 enemyPosition = getUnitPosition(enemy, true);
            WarState* attackState = createAttackState(context, entity, enemy->id, enemyPosition);
            changeNextState(context, entity, attackState, true, true);

            return;
        }
    }

    // if this unit is waiting
    if (state->move.waitCount > 0)
    {
        if (!isEmpty(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // wait for a number of times before re-route
            if (state->move.waitCount < MOVE_WAIT_INTENTS)
            {
                state->move.waitCount++;

                WarState* waitState = createWaitState(context, entity, getMapScaledTime(context, MOVE_WAIT_TIME));
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
        setAction(context, entity, WAR_ACTION_TYPE_WALK, false, getUnitActionScale(entity));
    }

    vec2 position = getUnitCenterPosition(entity, false);
    vec2 target = vec2TileToMapCoordinates(nextNode, true);

    vec2 direction = vec2Subv(target, position);
    f32 directionLength = vec2Length(direction);

    f32 speed = getMapScaledSpeed(context, stats.speeds[entity->unit.speed]);
    vec2 step = vec2Mulf(vec2Normalize(direction), speed * context->deltaTime);
    f32 stepLength = vec2Length(step);

    if (directionLength < stepLength)
    {
        step = direction;
    }

    vec2 newPosition = vec2Addv(position, step);
    setUnitCenterPosition(entity, newPosition, false);

    f32 distance = vec2Distance(newPosition, target);
    if (distance < MOVE_EPSILON)
    {
        newPosition = target;
        setUnitCenterPosition(entity, newPosition, false);

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

            WarState* waitState = createWaitState(context, entity, getMapScaledTime(context, MOVE_WAIT_TIME));
            waitState->nextState = state;
            changeNextState(context, entity, waitState, false, true);

            return;
        }

        setDynamicEntity(map->finder, nextNode.x, nextNode.y, unitSize.x, unitSize.y, entity->id);
        setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
    }
}

void freeMoveState(WarState* state)
{
    vec2ListFree(&state->move.positions);
    freePath(state->move.path);
}