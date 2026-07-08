#include <float.h>

#include "TracyC.h"

#include "war_state_machine.h"
#include "war_actions.h"
#include "war_units.h"
#include "war_rvo.h"

WarStateMove* wst_createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    TracyCZoneN(ctx, "wst_createMoveState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_MOVE, entity->id);
    WarStateMove* state = (WarStateMove*)wst_deref(context, ref);
    memcpy(state->waypoints, positions, positionCount * sizeof(vec2));
    state->waypointsCount = positionCount;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterMoveState", true);

    WarStateMove* s = (WarStateMove*)state;

    // WarMap* map = context->map;

    if (s->waypointsCount <= 1)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    s->waypointsIndex = 0;

    s->rvoVelocity       = VEC2_ZERO;
    s->settleTimer       = 0.0f;
    s->closestGoalDistSq = FLT_MAX;

    // s->rvoPreferredVelocity = VEC2_ZERO;
    // s->rvoPosition          = VEC2_ZERO;
    // s->rvoRadius            = 0.0f;
    // s->rvoNumCandidates     = 0;
    // s->rvoBestIndex         = 0;
    // for (s32 i = 0; i < RVO_MAX_CANDIDATES; i++)
    // {
    //     s->rvoCandidates[i]            = VEC2_ZERO;
    //     s->rvoCandidateHadCollision[i] = false;
    // }

    // vec2 nextPosition = s->waypoints[s->waypointsIndex + 1];

    // WarMapFlowField* flowField = wpath_getFlowField(&map->finder, (s32)nextPosition.x, (s32)nextPosition.y);
    // if (!flowField)
    // {
    //     flowField = wpath_computeFlowField(&map->finder, (s32)nextPosition.x, (s32)nextPosition.y);

    //     if (!flowField)
    //     {
    //         if (!wst_changeStateNextState(context, entity, state))
    //         {
    //             WarState* idleState = wst_createIdleState(context, entity, true);
    //             wst_changeNextState(context, entity, idleState, true, true);
    //         }

    //         TracyCZoneEnd(ctx);
    //         return;
    //     }
    // }

    // wact_setAction(context, entity, WAR_ACTION_TYPE_WALK, false, wu_getUnitActionScale(context, entity));

    TracyCZoneEnd(ctx);
}

void wst_leaveMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveMoveState", true);

    WarStateMove* s = (WarStateMove*)state;

    NOT_USED(context);
    NOT_USED(entity);

    s->rvoVelocity = VEC2_ZERO;

    // s->rvoPreferredVelocity = VEC2_ZERO;
    // s->rvoPosition          = VEC2_ZERO;
    // s->rvoRadius            = 0.0f;
    // s->rvoNumCandidates     = 0;
    // s->rvoBestIndex         = 0;
    // for (s32 i = 0; i < RVO_MAX_CANDIDATES; i++)
    // {
    //     s->rvoCandidates[i]            = VEC2_ZERO;
    //     s->rvoCandidateHadCollision[i] = false;
    // }

    TracyCZoneEnd(ctx);
}

void wst_updateMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateMoveState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_freeMoveState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeMoveState", true);
    NOT_USED(context);
    NOT_USED(state);
    TracyCZoneEnd(ctx);
}
