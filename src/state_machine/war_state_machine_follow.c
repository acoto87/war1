#include "../war_state_machine.h"

WarState* createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, s32 distance)
{
    WarState* state = createState(context, entity, WAR_STATE_FOLLOW);
    state->follow.targetEntityId = targetEntityId;
    state->follow.distance = distance;
    return state;
}

void enterFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void leaveFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
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

    vec2 start = getUnitCenterPosition(entity, true);
    vec2 end = getUnitCenterPosition(targetEntity, true);
    f32 distance = vec2DistanceInTiles(start, end);

    // if the unit is already in distance, go to idle
    if (distance <= state->follow.distance)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* waitState = createWaitState(context, entity, MOVE_WAIT_TIME);
            waitState->nextState = state;
            changeNextState(context, entity, waitState, false, true);
        }

        return;
    }

    WarMapPath path = findPath(map->finder, start.x, start.y, end.x, end.y);

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

void freeFollowState(WarState* state)
{
}