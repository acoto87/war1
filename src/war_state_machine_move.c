#include <float.h>

#include "TracyC.h"

#include "war_state_machine.h"
#include "war_actions.h"
#include "war_units.h"
#include "war_rvo.h"
#include "war_map.h"

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

void wst_leaveMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveMoveState", true);

    if (!state->initialized)
    {
        TracyCZoneEnd(ctx);
        return;
    }

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


static void updateArrivalDecay(WarContext* context, WarEntity* entity, WarStateMove* state)
{
    TracyCZoneN(ctx, "UpdateArrivalDecay", 1);

    assert(entity && wu_isUnit(entity));
    assert(state->base.type == WAR_STATE_MOVE);

    if (state->waypointsIndex >= state->waypointsCount - 1)
    {
        vec2 position = wu_getUnitCenterPosition(context, entity, false);
        vec2 goalPosition = state->waypoints[state->waypointsCount - 1];

        f32 distToGoalSq = vec2_distanceSqr(position, goalPosition);

#define RVO_SETTLE_PROGRESS_SQ    16.0f                    // must improve by 4 px to count
#define RVO_SETTLE_GOAL_RADIUS    (MEGA_TILE_WIDTH * 2.0f) // 32 px = 2 tiles
#define RVO_SETTLE_TIME_THRESHOLD 1.0f                     // game-seconds before giving up

        // Reset the timer whenever we get meaningfully closer to the goal.
        if (distToGoalSq < state->closestGoalDistSq - RVO_SETTLE_PROGRESS_SQ)
        {
            state->closestGoalDistSq = distToGoalSq;
            state->settleTimer       = 0.0f;
        }
        else
        {
            state->settleTimer += context->gameDeltaTime;
        }

        // Settle within the goal radius AND stuck for long enough -> idle.
        if (distToGoalSq <= RVO_SETTLE_GOAL_RADIUS * RVO_SETTLE_GOAL_RADIUS &&
            state->settleTimer >= RVO_SETTLE_TIME_THRESHOLD)
        {
            if (!wst_changeStateNextState(context, entity, (WarStateBase*)state))
            {
                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
            }
        }
    }

    TracyCZoneEnd(ctx);
}

static void updatePreferredVelocity(WarContext* context, WarEntity* entity, WarStateMove* state)
{
    TracyCZoneN(ctx, "UpdatePreferredVelocity", 1);

    assert(entity && wu_isUnit(entity));
    assert(state->base.type == WAR_STATE_MOVE);

    WarMap* map = context->map;
    assert(map);

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    vec2 position = wu_getUnitCenterPosition(context, entity, false);
    vec2 tile = wmap_mapToTileCoordinatesV(position);

    vec2 nextPosition = state->waypoints[state->waypointsIndex + 1];
    vec2 nextTile = wmap_mapToTileCoordinatesV(nextPosition);

    WarMapFlowField* flowField = wpath_getFlowField(&map->finder, (s32)nextTile.x, (s32)nextTile.y);
    if (flowField)
    {
        vec2 direction = wpath_flowFieldSample(flowField, (s32)tile.x, (s32)tile.y);

        // If the flow field is zero (a.k.a. reaching to the goal tile),
        // fall back to a direct vector to the next waypoint.
        if (vec2_lengthSqr(direction) < 0.0001f)
        {
            direction = vec2_subv(nextPosition, position);
            direction = vec2_normalize(direction);
        }

        f32 speed = (f32)stats->speeds[unit->speed];
        vec2 preferredVelocity = vec2_mulf(direction, speed);

        if (state->waypointsIndex >= state->waypointsCount - 1)
        {
#define RVO_ARRIVAL_SLOWDOWN_RADIUS (MEGA_TILE_WIDTH * 3.0f)  // 48 px = 3 tiles
#define RVO_ARRIVAL_MIN_SCALE       0.25f

            vec2 goalPosition = state->waypoints[state->waypointsCount - 1];
            f32 distToGoalSq = vec2_distanceSqr(position, goalPosition);
            if (distToGoalSq < RVO_ARRIVAL_SLOWDOWN_RADIUS * RVO_ARRIVAL_SLOWDOWN_RADIUS)
            {
                f32 distToGoal = sqrtf(distToGoalSq);
                f32 scale = distToGoal / RVO_ARRIVAL_SLOWDOWN_RADIUS;
                scale = MAX(scale, RVO_ARRIVAL_MIN_SCALE);
                preferredVelocity = vec2_mulf(preferredVelocity, scale);
            }
        }

        state->rvoPreferredVelocity = preferredVelocity;
    }
    else
    {
            if (!wst_changeStateNextState(context, entity, (WarStateBase*)state))
            {
                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
            }
    }

    TracyCZoneEnd(ctx);
}

static void updateAdjustedVelocity(WarContext* context, WarEntity* entity, WarStateMove* state)
{
    TracyCZoneN(ctx, "UpdateAdjustedVelocity", 1);

    assert(entity && wu_isUnit(entity));
    assert(state->base.type == WAR_STATE_MOVE);

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    vec2 position = wu_getUnitCenterPosition(context, entity, false);
    f32 speed = (f32)stats->speeds[unit->speed];

#define SEARCH_RADIUS_PX (MEGA_TILE_WIDTH * 5.0f) // ~5 tiles
#define MY_RADIUS_PX (MEGA_TILE_WIDTH * 0.45f)    // ~0.5 tiles

    WarRvoNeighbour neighbours[RVO_MAX_NB];
    s32 numNeighbours = wrvo_gatherNeighbours(context, entity->id, position, SEARCH_RADIUS_PX, neighbours);

    vec2  candidates[RVO_MAX_CANDIDATES];
    bool  hadCollision[RVO_MAX_CANDIDATES] = { 0 };
    s32   numCandidates = 0;
    s32   bestIndex     = 0;

    vec2 adjustedVelocity = wrvo_computeNewVelocity(
        state->rvoPreferredVelocity,
        position, state->rvoVelocity, MY_RADIUS_PX,
        speed,
        context->gameDeltaTime,
        neighbours, numNeighbours,
        candidates, &numCandidates, &bestIndex, hadCollision
    );

    state->rvoAdjustedVelocity = adjustedVelocity;
    state->rvoPosition         = position;
    state->rvoRadius           = MY_RADIUS_PX;
    state->rvoNumCandidates    = numCandidates;
    state->rvoBestIndex        = bestIndex;
    for (s32 i = 0; i < numCandidates; i++)
    {
        state->rvoCandidates[i]            = candidates[i];
        state->rvoCandidateHadCollision[i] = hadCollision[i];
    }

    TracyCZoneEnd(ctx);
}

static void updatePosition(WarContext* context, WarEntity* entity, WarStateMove* state)
{
    TracyCZoneN(ctx, "UpdatePosition", 1);

    assert(entity && wu_isUnit(entity));
    assert(state->base.type == WAR_STATE_MOVE);

    vec2 position = wu_getUnitCenterPosition(context, entity, false);
    vec2 nextPosition = state->waypoints[state->waypointsIndex + 1];

    // Overshoot clamp: if step would carry us past the waypoint, snap to it.
    vec2 step = vec2_mulf(state->rvoAdjustedVelocity, context->gameDeltaTime);
    vec2 toNext = vec2_subv(nextPosition, position);
    if (vec2_lengthSqr(step) > vec2_lengthSqr(toNext))
    {
        step = toNext;
    }

    vec2 newPosition = vec2_addv(position, step);
    wu_setUnitDirection(context, entity, wu_getDirectionFromDiff(state->rvoAdjustedVelocity.x, state->rvoAdjustedVelocity.y));
    wact_setAction(context, entity, WAR_ACTION_TYPE_WALK, false, wu_getUnitActionScale(context, entity));
    wu_setUnitCenterPosition(context, entity, newPosition, false);

    f32 distanceSq = vec2_distanceSqr(newPosition, nextPosition);
    if (distanceSq < MOVE_EPSILON * MOVE_EPSILON)
    {
        newPosition = nextPosition;
        wu_setUnitCenterPosition(context, entity, newPosition, false);

        state->waypointsIndex++;

        if (state->waypointsIndex >= state->waypointsCount - 1)
        {
            state->rvoVelocity = VEC2_ZERO;
            state->settleTimer = 0.0f;
            state->closestGoalDistSq = FLT_MAX;

            state->rvoPreferredVelocity = VEC2_ZERO;
            state->rvoPosition = VEC2_ZERO;
            state->rvoRadius = 0.0f;
            state->rvoNumCandidates = 0;
            state->rvoBestIndex = 0;
            memset(state->rvoCandidates, 0, sizeof(state->rvoCandidates));
            memset(state->rvoCandidateHadCollision, 0, sizeof(state->rvoCandidateHadCollision));

            if (!wst_changeStateNextState(context, entity, (WarStateBase*)state))
            {
                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
            }
        }
    }

    TracyCZoneEnd(ctx);
}

void wst_updateMoveStates(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateMovement", 1);

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);

    WarStateStorage* storage = &manager->stateStorage;
    WarStateMove* moveStates = storage->move;
    bool* occupied = storage->occupied[WAR_STATE_MOVE];

    // Initialize newly-created MOVE states before the first RVO pass.
    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        if (state->base.initialized) continue;

        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        if (!sm || sm->currentRef.type != WAR_STATE_MOVE || sm->currentRef.idx != i) continue;

        state->base.initialized = true;

        if (state->waypointsCount <= 1)
        {
            if (!wst_changeStateNextState(context, entity, (WarStateBase*)state))
            {
                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
            }
            continue;
        }

        state->waypointsIndex = 0;
        state->rvoVelocity = VEC2_ZERO;
        state->settleTimer = 0.0f;
        state->closestGoalDistSq = FLT_MAX;
    }

    // 0. Reset the RVO velocity for all units to their current velocity.
    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        if (!sm || sm->currentRef.type != WAR_STATE_MOVE || sm->currentRef.idx != i) continue;
        state->rvoPreferredVelocity = VEC2_ZERO;
        state->rvoPosition = VEC2_ZERO;
        state->rvoRadius = 0.0f;
        state->rvoNumCandidates = 0;
        state->rvoBestIndex = 0;
        memset(state->rvoCandidates, 0, sizeof(state->rvoCandidates));
        memset(state->rvoCandidateHadCollision, 0, sizeof(state->rvoCandidateHadCollision));
    }

    // 0.5. Check for attacks before doing any RVO calculations.
    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        if (!sm || sm->currentRef.type != WAR_STATE_MOVE || sm->currentRef.idx != i) continue;

        if (state->checkForAttacks)
        {
            WarEntity* enemy = we_getAttacker(context, entity);
            if (enemy && wu_areEnemies(context, entity, enemy) && wu_canAttack(context, entity, enemy))
            {
                vec2 enemyPosition = wu_getUnitPosition(context, enemy, true);
                WarStateAttack* attackState = wst_createAttackState(context, entity, enemy->id, enemyPosition);
                wst_changeNextState(context, entity, (WarStateBase*)attackState, true);
            }
        }
    }

    // 1. Update already-arrived units to zero velocity.
    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        if (!sm || sm->currentRef.type != WAR_STATE_MOVE || sm->currentRef.idx != i) continue;
        updateArrivalDecay(context, entity, state);
    }

    // 2. Update preferred velocities for all units.
    for(s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        if (!sm || sm->currentRef.type != WAR_STATE_MOVE || sm->currentRef.idx != i) continue;
        updatePreferredVelocity(context, entity, state);
    }

    // 3. Update spatial grid with the new preferred velocities and positions of the units.
    wgrid_rebuildIfDirty(context);

    // 4. Update adjusted velocities from RVO calculations for all units.
    for(s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        if (!sm || sm->currentRef.type != WAR_STATE_MOVE || sm->currentRef.idx != i) continue;
        updateAdjustedVelocity(context, entity, state);
    }

    // 5. Update positions based on the adjusted velocities.
    for(s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        if (!sm || sm->currentRef.type != WAR_STATE_MOVE || sm->currentRef.idx != i) continue;
        updatePosition(context, entity, state);
    }

    // 6. Update rvoVelocity for all units (after position updates).
    for(s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        if (!sm || sm->currentRef.type != WAR_STATE_MOVE || sm->currentRef.idx != i) continue;
        state->rvoVelocity = state->rvoAdjustedVelocity;
    }

    // 7. Rebuild the spatial grid again to reflect the new positions of the units.
    wgrid_rebuildIfDirty(context);

    TracyCZoneEnd(ctx);
}
