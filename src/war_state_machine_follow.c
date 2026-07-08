#include "war_state_machine.h"

#include "war_units.h"

#include "TracyC.h"

WarStateFollow* wst_createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile, s32 distance)
{
    TracyCZoneN(ctx, "wst_createFollowState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_FOLLOW, entity->id);
    WarStateFollow* state = (WarStateFollow*)wst_deref(context, ref);
    state->targetEntityId = targetEntityId;
    state->targetTile = targetTile;
    state->distance = distance;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterFollowState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_leaveFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveFollowState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateFollowState", true);

    WarStateFollow* s = (WarStateFollow*)state;

    WarMap* map = context->map;

    vec2 start = wu_getUnitCenterPosition(context, entity, true);
    vec2 end = s->targetTile;

    if (s->targetEntityId)
    {
        WarEntity* targetEntity = we_findEntity(context, (WarEntityId)s->targetEntityId);
        if (!targetEntity)
        {
            // if the target entity doesn't exist anymore, go to idle
            if (!wst_changeStateNextState(context, entity, state))
            {
                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
            }

            TracyCZoneEnd(ctx);
            return;
        }

        if (wu_isUnit(targetEntity))
        {
            // if the target entity is an unit the instead of using the tile where
            // the player click, use a point on the target unit that is closer to
            // the following unit
            end = wu_unitPointOnTarget(context, entity, targetEntity);
        }
        else
        {
            end = wu_getUnitCenterPosition(context, targetEntity, true);
        }
    }

    f32 distance = vec2_distanceInTiles(start, end);

    // if the unit is already in distance, go to idle
    if (distance <= s->distance)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarStateWait* waitState = wst_createWaitState(context, entity, wmap_getMapScaledTime(context, MOVE_WAIT_TIME));
            wst_chainNext(context, (WarStateBase*)waitState, (WarStateBase*)state);
            wst_changeNextState(context, entity, (WarStateBase*)waitState, false, true);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    WarMapPath path = wpath_findPath(&map->finder, (s32)start.x, (s32)start.y, (s32)end.x, (s32)end.y);

    // if there is no path to the target, go to idle
    if (path.nodes.count <= 1)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
        }

        Vec2ListFree(&path.nodes);
        TracyCZoneEnd(ctx);
        return;
    }

    WarStateMove* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, path.nodes.items[0], path.nodes.items[1]));
    wst_chainNext(context, (WarStateBase*)moveState, (WarStateBase*)state);
    wst_changeNextState(context, entity, (WarStateBase*)moveState, false, true);

    Vec2ListFree(&path.nodes);

    TracyCZoneEnd(ctx);
}

void wst_freeFollowState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeFollowState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
