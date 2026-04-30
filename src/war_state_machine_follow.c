#include "war_state_machine.h"

#include "war_units.h"

WarState* wst_createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile, s32 distance)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_FOLLOW);
    state->follow.targetEntityId = targetEntityId;
    state->follow.targetTile = targetTile;
    state->follow.distance = distance;
    return state;
}

void wst_enterFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_leaveFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updateFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    vec2 start = wu_getUnitCenterPosition(entity, true);
    vec2 end = state->follow.targetTile;

    if (state->follow.targetEntityId)
    {
        WarEntity* targetEntity = we_findEntity(context, (WarEntityId)state->follow.targetEntityId);

        if (isUnit(targetEntity))
        {
            // if the target entity is an unit the instead of using the tile where
            // the player click, use a point on the target unit that is closer to
            // the following unit
            end = wu_unitPointOnTarget(entity, targetEntity);
        }
        else
        {
            end = wu_getUnitCenterPosition(targetEntity, true);
        }
    }

    f32 distance = vec2DistanceInTiles(start, end);

    // if the unit is already in distance, go to idle
    if (distance <= state->follow.distance)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* waitState = wst_createWaitState(context, entity, wmap_getMapScaledTime(context, MOVE_WAIT_TIME));
            waitState->nextState = state;
            wst_changeNextState(context, entity, waitState, false, true);
        }

        return;
    }

    WarMapPath path = wpath_findPath(map->finder, (s32)start.x, (s32)start.y, (s32)end.x, (s32)end.y);

    // if there is no path to the target, go to idle
    if (path.nodes.count <= 1)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }

        wpath_freePath(path);
        return;
    }

    WarState* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, path.nodes.items[0], path.nodes.items[1]));
    moveState->nextState = state;
    wst_changeNextState(context, entity, moveState, false, true);

    wpath_freePath(path);
}

void wst_freeFollowState(WarContext* context, WarState* state)
{
    NOT_USED(context);
    NOT_USED(state);
}
