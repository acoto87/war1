#include "war_state_machine.h"

#include "war_actions.h"
#include "war_units.h"

WarState* wst_createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    WarState* state = wst_createState(context, entity, WAR_STATE_MOVE);
    vec2ListInit(&state->wcmd_move.positions, vec2ListDefaultOptions);
    vec2ListAddRange(&state->wcmd_move.positions, positionCount, positions);
    return state;
}

void wst_enterMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = wu_getUnitSize(entity);

    if (state->wcmd_move.positions.count <= 1)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }

        return;
    }

    state->wcmd_move.positionIndex = 0;
    state->wcmd_move.pathNodeIndex = 0;
    state->wcmd_move.waitCount = 0;

    vec2 start = state->wcmd_move.positions.items[state->wcmd_move.positionIndex];
    vec2 end = state->wcmd_move.positions.items[state->wcmd_move.positionIndex + 1];

    WarMapPath path = state->wcmd_move.path = wpath_findPath(map->finder, (s32)start.x, (s32)start.y, (s32)end.x, (s32)end.y);

    // if the is no path to the next position, go to idle
    if (path.nodes.count <= 1)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }

        return;
    }

    vec2 currentNode = path.nodes.items[state->wcmd_move.pathNodeIndex];
    setDynamicEntity(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

    vec2 nextNode = path.nodes.items[state->wcmd_move.pathNodeIndex + 1];
    setDynamicEntity(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

    wu_setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
    wact_setAction(context, entity, WAR_ACTION_TYPE_WALK, false, wu_getUnitActionScale(entity));
}

void wst_leaveMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = wu_getUnitSize(entity);
    WarMapPath* path = &state->wcmd_move.path;

    if (inRange(state->wcmd_move.pathNodeIndex, 0, path->nodes.count))
    {
        vec2 currentNode = path->nodes.items[state->wcmd_move.pathNodeIndex];
        if (isDynamicOfEntity(map->finder, (s32)currentNode.x, (s32)currentNode.y, entity->id))
            setFreeTiles(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y);
    }

    if (inRange(state->wcmd_move.pathNodeIndex + 1, 0, path->nodes.count))
    {
        vec2 nextNode = path->nodes.items[state->wcmd_move.pathNodeIndex + 1];
        if (isDynamicOfEntity(map->finder, (s32)nextNode.x, (s32)nextNode.y, entity->id))
            setFreeTiles(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y);
    }
}

void wst_updateMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarMapPath* path = &state->wcmd_move.path;

    assert(path->nodes.count > 1);
    assert(inRange(state->wcmd_move.pathNodeIndex, 0, path->nodes.count - 1));

    WarUnitStats stats = wu_getUnitStats(entity->unit.type);
    vec2 unitSize = wu_getUnitSize(entity);

    vec2 currentNode = path->nodes.items[state->wcmd_move.pathNodeIndex];
    vec2 nextNode = path->nodes.items[state->wcmd_move.pathNodeIndex + 1];

    if (state->wcmd_move.checkForAttacks)
    {
        WarEntity* enemy = we_getAttacker(context, entity);
        if (enemy && wu_areEnemies(context, entity, enemy) && wu_canAttack(context, entity, enemy))
        {
            vec2 enemyPosition = wu_getUnitPosition(enemy, true);
            WarState* attackState = wst_createAttackState(context, entity, enemy->id, enemyPosition);
            wst_changeNextState(context, entity, attackState, true, true);

            return;
        }
    }

    // if this unit is waiting
    if (state->wcmd_move.waitCount > 0)
    {
        if (!isEmpty(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // wait for a number of times before re-route
            if (state->wcmd_move.waitCount < MOVE_WAIT_INTENTS)
            {
                state->wcmd_move.waitCount++;

                WarState* waitState = wst_createWaitState(context, entity, wmap_getMapScaledTime(context, MOVE_WAIT_TIME));
                waitState->nextState = state;
                wst_changeNextState(context, entity, waitState, false, true);

                return;
            }

            state->wcmd_move.waitCount = 0;

            // if there is no re-routing possible, go to idle
            if (!wpath_reRoutePath(map->finder, path, state->wcmd_move.pathNodeIndex, path->nodes.count - 1))
            {
                if (!wst_changeStateNextState(context, entity, state))
                {
                    WarState* idleState = wst_createIdleState(context, entity, true);
                    wst_changeNextState(context, entity, idleState, true, true);
                }

                return;
            }

            nextNode = path->nodes.items[state->wcmd_move.pathNodeIndex + 1];
        }
        else
        {
            state->wcmd_move.waitCount = 0;
        }

        setDynamicEntity(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
        wu_setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
        wact_setAction(context, entity, WAR_ACTION_TYPE_WALK, false, wu_getUnitActionScale(entity));
    }

    vec2 position = wu_getUnitCenterPosition(entity, false);
    vec2 target = wmap_vec2TileToMapCoordinates(nextNode, true);

    vec2 direction = vec2Subv(target, position);
    f32 directionLength = vec2Length(direction);

    f32 speed = wmap_getMapScaledSpeed(context, (f32)stats.speeds[entity->unit.speed]);
    vec2 step = vec2Mulf(vec2Normalize(direction), speed * context->deltaTime);
    f32 stepLength = vec2Length(step);

    if (directionLength < stepLength)
    {
        step = direction;
    }

    vec2 newPosition = vec2Addv(position, step);
    wu_setUnitCenterPosition(entity, newPosition, false);

    f32 distance = vec2Distance(newPosition, target);
    if (distance < MOVE_EPSILON)
    {
        newPosition = target;
        wu_setUnitCenterPosition(entity, newPosition, false);

        setFreeTiles(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y);
        setFreeTiles(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y);

        state->wcmd_move.pathNodeIndex++;

        // if there is no more path nodes to check, go to idle
        if (state->wcmd_move.pathNodeIndex >= path->nodes.count - 1)
        {
            state->wcmd_move.positionIndex++;

            // if this is no more segments, go to idle
            if (state->wcmd_move.positionIndex >= state->wcmd_move.positions.count - 1)
            {
                if (!wst_changeStateNextState(context, entity, state))
                {
                    WarState* idleState = wst_createIdleState(context, entity, true);
                    wst_changeNextState(context, entity, idleState, true, true);
                }

                return;
            }

            // free the previous path and check if there is a new one
            wpath_freePath(*path);

            vec2 start = state->wcmd_move.positions.items[state->wcmd_move.positionIndex];
            vec2 end = state->wcmd_move.positions.items[state->wcmd_move.positionIndex + 1];

            *path = wpath_findPath(map->finder, (s32)start.x, (s32)start.y, (s32)end.x, (s32)end.y);

            // if there is no path for the next segment, go to idle
            if (path->nodes.count <= 1)
            {
                if (!wst_changeStateNextState(context, entity, state))
                {
                    WarState* idleState = wst_createIdleState(context, entity, true);
                    wst_changeNextState(context, entity, idleState, true, true);
                }

                return;
            }

            state->wcmd_move.pathNodeIndex = 0;
        }

        currentNode = path->nodes.items[state->wcmd_move.pathNodeIndex];
        setDynamicEntity(map->finder, (s32)currentNode.x, (s32)currentNode.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

        nextNode = path->nodes.items[state->wcmd_move.pathNodeIndex + 1];

        // if the next node is occupied, check if the unit have to wait to re-route if necessary
        if (!isEmpty(map->finder, (s32)nextNode.x, (s32)nextNode.y))
        {
            // if the next node is occupied but is the last one, don't wait to re-route, go idle
            if (state->wcmd_move.pathNodeIndex + 1 == path->nodes.count - 1)
            {
                if (!wst_changeStateNextState(context, entity, state))
                {
                    WarState* idleState = wst_createIdleState(context, entity, true);
                    wst_changeNextState(context, entity, idleState, true, true);
                }

                return;
            }

            state->wcmd_move.waitCount++;

            WarState* waitState = wst_createWaitState(context, entity, wmap_getMapScaledTime(context, MOVE_WAIT_TIME));
            waitState->nextState = state;
            wst_changeNextState(context, entity, waitState, false, true);

            return;
        }

        setDynamicEntity(map->finder, (s32)nextNode.x, (s32)nextNode.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
        wu_setUnitDirectionFromDiff(entity, nextNode.x - currentNode.x, nextNode.y - currentNode.y);
    }
}

void wst_freeMoveState(WarContext* context, WarState* state)
{
    vec2ListFree(&state->wcmd_move.positions);
    wpath_freePath(state->wcmd_move.path);
}
