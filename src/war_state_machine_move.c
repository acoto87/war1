#include <float.h>
#include <string.h>

#include "TracyC.h"

#include "war_state_machine.h"
#include "war_actions.h"
#include "war_units.h"
#include "war_rvo.h"
#include "war_map.h"

#define MOVE_PROGRESS_DISTANCE_PX          4.0f
#define MOVE_PATH_REFRESH_TIME             0.5f
#define MOVE_AVOIDANCE_RECOVERY_TIME       1.5f
#define MOVE_BLOCKED_TIME                  3.0f
#define MOVE_LOW_VELOCITY_PX_PER_SECOND    1.0f
#define MOVE_BASE_RVO_RADIUS_PX            (MEGA_TILE_WIDTH * 0.45f)
#define MOVE_RECOVERY_RVO_RADIUS_PX        (MEGA_TILE_WIDTH * 0.60f)

static void resetMoveProgress(WarContext* context, WarEntity* entity, WarStateMove* state)
{
    state->progress = (WarMoveProgress)
    {
        .bestDistanceSq = FLT_MAX
    };
    state->progressGoalPosition = VEC2_ZERO;
    state->progressWaypointIndex = -1;

    if (state->waypointsIndex >= state->waypointsCount - 1)
    {
        return;
    }

    vec2 position = wu_getUnitCenterPosition(context, entity);
    vec2 goalPosition = state->waypoints[state->waypointsIndex + 1];

    state->progress.bestDistanceSq = vec2_distanceSqr(position, goalPosition);
    state->progressGoalPosition = goalPosition;
    state->progressWaypointIndex = state->waypointsIndex;
}

WarStateMove* wst_createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[], bool checkForAttacks)
{
    TracyCZoneN(ctx, "wst_createMoveState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_MOVE, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateMove* state = (WarStateMove*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    const s32 count = MIN(positionCount, arrayLength(state->waypoints));
    memcpy(state->waypoints, positions, count * sizeof(vec2));
    state->waypointsIndex = 0;
    state->waypointsCount = count;
    state->checkForAttacks = checkForAttacks;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterMoveState", true);

    WarStateMove* s = (WarStateMove*)state;

    if (s->waypointsCount <= 1)
    {
        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_NO_DESTINATION);
        TracyCZoneEnd(ctx);
        return;
    }

    s->waypointsIndex = 0;
    s->rvoVelocity = VEC2_ZERO;
    resetMoveProgress(context, entity, s);

    TracyCZoneEnd(ctx);
}

void wst_exitMoveState(WarContext* context, WarEntity* entity, WarState* state, WarStateExitReason reason)
{
    TracyCZoneN(ctx, "wst_exitMoveState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(reason);

    WarStateMove* s = (WarStateMove*)state;
    s->rvoVelocity = VEC2_ZERO;

    TracyCZoneEnd(ctx);
}

static void updateMoveProgressAndRecovery(WarContext* context, WarEntity* entity, WarStateMove* state)
{
    TracyCZoneN(ctx, "UpdateMoveProgressAndRecovery", 1);

    assert(entity && wu_isUnit(entity));
    assert(state->base.type == WAR_STATE_MOVE);

    if (state->waypointsIndex >= state->waypointsCount - 1)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    vec2 position = wu_getUnitCenterPosition(context, entity);
    vec2 goalPosition = state->waypoints[state->waypointsIndex + 1];
    bool goalChanged = state->progressWaypointIndex != state->waypointsIndex ||
                       vec2_distanceSqr(state->progressGoalPosition, goalPosition) >= MOVE_EPSILON * MOVE_EPSILON;

    if (goalChanged)
    {
        resetMoveProgress(context, entity, state);
        TracyCZoneEnd(ctx);
        return;
    }

    f32 distanceSq = vec2_distanceSqr(position, goalPosition);
    f32 distance = sqrtf(distanceSq);
    f32 bestDistance = sqrtf(state->progress.bestDistanceSq);
    if (bestDistance - distance >= MOVE_PROGRESS_DISTANCE_PX)
    {
        state->progress.bestDistanceSq = distanceSq;
        state->progress.noProgressTime = 0.0f;
        state->progress.lowVelocityTime = 0.0f;
        state->progress.recoveryAttempt = 0;
        TracyCZoneEnd(ctx);
        return;
    }

    state->progress.noProgressTime += context->gameDeltaTime;

    f32 velocitySq = vec2_lengthSqr(state->rvoAdjustedVelocity);
    if (velocitySq <= MOVE_LOW_VELOCITY_PX_PER_SECOND * MOVE_LOW_VELOCITY_PX_PER_SECOND)
    {
        state->progress.lowVelocityTime += context->gameDeltaTime;
    }
    else
    {
        state->progress.lowVelocityTime = 0.0f;
    }

    WarMap* map = context->map;
    assert(map);

    if (state->progress.recoveryAttempt < 1 && state->progress.noProgressTime >= MOVE_PATH_REFRESH_TIME)
    {
        vec2 goalTile = wmap_mapToTileCoordinatesV(goalPosition);
        wpath_computeFlowField(&map->finder, (s32)goalTile.x, (s32)goalTile.y);
        state->progress.recoveryAttempt = 1;
    }

    if (state->progress.recoveryAttempt < 2 && state->progress.noProgressTime >= MOVE_AVOIDANCE_RECOVERY_TIME)
    {
        state->progress.recoveryAttempt = 2;
    }

    if (state->progress.noProgressTime >= MOVE_BLOCKED_TIME)
    {
        state->progress.recoveryAttempt = 3;
        state->rvoPreferredVelocity = VEC2_ZERO;
        state->rvoAdjustedVelocity = VEC2_ZERO;
        state->rvoVelocity = VEC2_ZERO;
        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_BLOCKED);
    }

    TracyCZoneEnd(ctx);
}

static void updatePreferredVelocity(WarContext* context, WarEntity* entity, WarStateMove* state)
{
    TracyCZoneN(ctx, "UpdatePreferredVelocity", 1);

    assert(entity && wu_isUnit(entity));
    assert(state->base.type == WAR_STATE_MOVE);

    if (state->waypointsIndex >= state->waypointsCount - 1)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    WarMap* map = context->map;
    assert(map);

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    vec2 position = wu_getUnitCenterPosition(context, entity);
    vec2 tile = wmap_mapToTileCoordinatesV(position);

    vec2 nextPosition = state->waypoints[state->waypointsIndex + 1];
    vec2 nextTile = wmap_mapToTileCoordinatesV(nextPosition);

    WarMapFlowField* flowField = wpath_ensureFlowField(&map->finder, (s32)nextTile.x, (s32)nextTile.y);
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

        // Slow down when approaching the final waypoint to avoid overshooting.
        if (state->waypointsIndex >= state->waypointsCount - 2)
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
        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_NO_PATH);
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

    vec2 position = wu_getUnitCenterPosition(context, entity);
    f32 speed = (f32)stats->speeds[unit->speed];

#define SEARCH_RADIUS_PX (MEGA_TILE_WIDTH * 5.0f) // ~5 tiles

    f32 radius = state->progress.recoveryAttempt >= 2 ?
                 MOVE_RECOVERY_RVO_RADIUS_PX : MOVE_BASE_RVO_RADIUS_PX;

    WarRvoNeighbour neighbours[RVO_MAX_NB];
    s32 numNeighbours = wrvo_gatherNeighbours(context, entity->id, position, SEARCH_RADIUS_PX, neighbours);

    vec2  candidates[RVO_MAX_CANDIDATES];
    bool  hadCollision[RVO_MAX_CANDIDATES] = { 0 };
    s32   numCandidates = 0;
    s32   bestIndex     = 0;

    vec2 adjustedVelocity = wrvo_computeNewVelocity(
        state->rvoPreferredVelocity,
        position, state->rvoVelocity, radius,
        speed,
        context->gameDeltaTime,
        neighbours, numNeighbours,
        candidates, &numCandidates, &bestIndex, hadCollision
    );

    state->rvoAdjustedVelocity = adjustedVelocity;
    state->rvoPosition         = position;
    state->rvoRadius           = radius;
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

    if (state->waypointsIndex >= state->waypointsCount - 1)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    vec2 position = wu_getUnitCenterPosition(context, entity);
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
    wu_setUnitCenterPosition(context, entity, newPosition);

    f32 distanceSq = vec2_distanceSqr(newPosition, nextPosition);
    if (distanceSq < MOVE_EPSILON * MOVE_EPSILON)
    {
        newPosition = nextPosition;
        wu_setUnitCenterPosition(context, entity, newPosition);

        state->waypointsIndex++;

        if (state->waypointsIndex >= state->waypointsCount - 1)
        {
            state->rvoVelocity = VEC2_ZERO;

            state->rvoPreferredVelocity = VEC2_ZERO;
            state->rvoPosition = VEC2_ZERO;
            state->rvoRadius = 0.0f;
            state->rvoNumCandidates = 0;
            state->rvoBestIndex = 0;
            memset(state->rvoCandidates, 0, sizeof(state->rvoCandidates));
            memset(state->rvoCandidateHadCollision, 0, sizeof(state->rvoCandidateHadCollision));

            wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);
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

    // 1. Reset the RVO velocity for all units to their current velocity.
    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;

        state->rvoPreferredVelocity = VEC2_ZERO;
        state->rvoPosition = VEC2_ZERO;
        state->rvoRadius = 0.0f;
        state->rvoNumCandidates = 0;
        state->rvoBestIndex = 0;
        memset(state->rvoCandidates, 0, sizeof(state->rvoCandidates));
        memset(state->rvoCandidateHadCollision, 0, sizeof(state->rvoCandidateHadCollision));
    }

    // 2. Check for attacks before doing any RVO calculations.
    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;

        if (state->checkForAttacks)
        {
            WarEntity* enemy = we_getAttacker(context, entity);
            if (enemy && wu_areEnemies(context, entity, enemy) && wu_canAttack(context, entity, enemy) &&
                wst_canSubmitTransition(context, entity, WAR_INTERRUPT_AUTONOMOUS))
            {
                vec2 enemyPosition = wu_getUnitPosition(context, enemy);
                WarStateAttack* attackState = wst_createAttackState(context, entity, enemy->id, enemyPosition);
                wst_pushState(context, entity, (WarStateBase*)attackState, WAR_TRANSITION_CAUSE_AUTONOMOUS);
            }
        }
    }

    // 3. Update preferred velocities for all units.
    for(s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;

        updatePreferredVelocity(context, entity, state);
    }

    // 4. Update adjusted velocities from RVO calculations for all units.
    for(s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;

        updateAdjustedVelocity(context, entity, state);
    }

    // 5. Update positions based on the adjusted velocities.
    for(s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;

        updatePosition(context, entity, state);
    }

    // 6. Track progress and apply staged stuck recovery after movement.
    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;

        updateMoveProgressAndRecovery(context, entity, state);
    }

    // 7. Update rvoVelocity for all units (after position and recovery updates).
    for(s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;
        WarStateMove* state = &moveStates[i];
        WarEntity* entity = we_findEntity(context, state->base.entityId);
        if (!entity || !wu_isUnit(entity)) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;

        state->rvoVelocity = state->rvoAdjustedVelocity;
    }

    TracyCZoneEnd(ctx);
}
