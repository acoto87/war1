#include <float.h>

#include "TracyC.h"

#include "war_state_machine.h"
#include "war_actions.h"
#include "war_units.h"
#include "war_rvo.h"

// Collect moving units near `centrePixel` within `radiusPx`.
// Returns count (capped at RVO_MAX_NB).
static s32 rvo_gatherNeighbours(
    WarContext* context,
    s32         selfId,
    vec2        centrePixel,
    f32         radiusPx,
    WarRvoNeighbour out[RVO_MAX_NB])
{
    WarMap*     map  = context->map;
    WarEntityManager* entityManager = &map->entityManager;
    WarMapGrid* grid = &map->grid;
    s32         count = 0;

    vec2 ct    = wmap_mapToTileCoordinatesV(centrePixel);
    f32  rt    = radiusPx / (f32)MEGA_TILE_WIDTH;
    s32  gxMin = MAX(0, (s32)floorf((ct.x - rt) / MAP_GRID_TILE_SIZE));
    s32  gxMax = MIN(MAP_GRID_TILES_WIDTH - 1, (s32)floorf((ct.x + rt) / MAP_GRID_TILE_SIZE));
    s32  gyMin = MAX(0, (s32)floorf((ct.y - rt) / MAP_GRID_TILE_SIZE));
    s32  gyMax = MIN(MAP_GRID_TILES_HEIGHT - 1, (s32)floorf((ct.y + rt) / MAP_GRID_TILE_SIZE));

    for (s32 gy = gyMin; gy <= gyMax && count < RVO_MAX_NB; gy++)
    {
        for (s32 gx = gxMin; gx <= gxMax && count < RVO_MAX_NB; gx++)
        {
            for (s32 idx = grid->head[gy * MAP_GRID_TILES_WIDTH + gx];
                 idx >= 0 && count < RVO_MAX_NB;
                 idx = grid->next[idx])
            {
                WarEntity* other = &entityManager->entities[idx];
                if (other->id == selfId) continue;
                if (!we_isComponentEnabled(context, other, COMP_UNIT)) continue;

                vec2 oPos = wu_getUnitCenterPosition(context, other, false);
                f32  dx   = oPos.x - centrePixel.x;
                f32  dy   = oPos.y - centrePixel.y;
                if (dx*dx + dy*dy > radiusPx*radiusPx) continue;

                // Read the neighbour's last RVO velocity (zero if not moving).
                vec2 oVel = VEC2_ZERO;

                WarState* moveState = wst_getMoveState(context, other);
                if (moveState)
                {
                    oVel = moveState->move.rvoVelocity;
                }

                out[count].position = oPos;
                out[count].velocity = oVel;
                out[count].radius   = (f32)MEGA_TILE_WIDTH * 0.45f;
                count++;
            }
        }
    }

    return count;
}

WarState* wst_createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[])
{
    TracyCZoneN(ctx, "wst_createMoveState", true);

    WarState* state = wst_createState(context, entity, WAR_STATE_MOVE);
    memcpy(state->move.waypoints, positions, positionCount * sizeof(vec2));
    state->move.waypointsCount = positionCount;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterMoveState", true);

    // WarMap* map = context->map;

    if (state->move.waypointsCount <= 1)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    state->move.waypointsIndex = 0;

    state->move.rvoVelocity       = VEC2_ZERO;
    state->move.settleTimer       = 0.0f;
    state->move.closestGoalDistSq = FLT_MAX;

    // state->move.rvoPreferredVelocity = VEC2_ZERO;
    // state->move.rvoPosition          = VEC2_ZERO;
    // state->move.rvoRadius            = 0.0f;
    // state->move.rvoNumCandidates     = 0;
    // state->move.rvoBestIndex         = 0;
    // for (s32 i = 0; i < RVO_MAX_CANDIDATES; i++)
    // {
    //     state->move.rvoCandidates[i]            = VEC2_ZERO;
    //     state->move.rvoCandidateHadCollision[i] = false;
    // }

    // vec2 nextPosition = state->move.waypoints[state->move.waypointsIndex + 1];

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

    NOT_USED(context);
    NOT_USED(entity);

    state->move.rvoVelocity = VEC2_ZERO;

    // state->move.rvoPreferredVelocity = VEC2_ZERO;
    // state->move.rvoPosition          = VEC2_ZERO;
    // state->move.rvoRadius            = 0.0f;
    // state->move.rvoNumCandidates     = 0;
    // state->move.rvoBestIndex         = 0;
    // for (s32 i = 0; i < RVO_MAX_CANDIDATES; i++)
    // {
    //     state->move.rvoCandidates[i]            = VEC2_ZERO;
    //     state->move.rvoCandidateHadCollision[i] = false;
    // }

    TracyCZoneEnd(ctx);
}

void wst_updateMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateMoveState", true);

    NOT_USED(state);

//     WarMap* map = context->map;

//     assert(state->move.waypointsCount > 1);
//     assert(inRange(state->move.waypointsIndex, 0, state->move.waypointsCount - 1));

//     WarUnitComponent* unit = we_getUnitComponent(context, entity);
//     assert(unit);

//     const WarUnitStats* stats = wu_getUnitStats(unit->type);

//     vec2 nextPosition = state->move.waypoints[state->move.waypointsIndex + 1];

//     if (state->move.checkForAttacks)
//     {
//         WarEntity* enemy = we_getAttacker(context, entity);
//         if (enemy && wu_areEnemies(context, entity, enemy) && wu_canAttack(context, entity, enemy))
//         {
//             vec2 enemyPosition = wu_getUnitPosition(context, enemy, true);
//             WarState* attackState = wst_createAttackState(context, entity, enemy->id, enemyPosition);
//             wst_changeNextState(context, entity, attackState, true, true);

//             TracyCZoneEnd(ctx);
//             return;
//         }
//     }

//     WarMapFlowField* flowField = wpath_getFlowField(&map->finder, (s32)nextPosition.x, (s32)nextPosition.y);
//     assert(flowField);

//     vec2 position = wu_getUnitCenterPosition(context, entity, false);
//     vec2 tile = wmap_mapToTileCoordinatesV(position);

//     // ── 1. CROWD SETTLING: detect lack of progress toward the final goal ─────
//     // The final commanded destination (last element of the positions list, stored in tile coords).

//     vec2 nextGoal = wmap_tileToMapCoordinatesV(nextPosition, true);

//     f32 gx           = position.x - nextGoal.x;
//     f32 gy           = position.y - nextGoal.y;
//     f32 distToGoalSq = gx*gx + gy*gy;

//     // Reset the timer whenever we get meaningfully closer to the goal.
// #define RVO_SETTLE_PROGRESS_SQ      16.0f   // must improve by 4 px to count
// #define RVO_SETTLE_GOAL_RADIUS      (MEGA_TILE_WIDTH * 2.0f)  // 32 px = 2 tiles
// #define RVO_SETTLE_TIME_THRESHOLD   0.75f   // game-seconds before giving up

//     if (distToGoalSq < state->move.closestGoalDistSq - RVO_SETTLE_PROGRESS_SQ)
//     {
//         state->move.closestGoalDistSq = distToGoalSq;
//         state->move.settleTimer       = 0.0f;
//     }
//     else
//     {
//         state->move.settleTimer += context->gameDeltaTime;
//     }

//     // Settle: within the goal radius AND stuck for long enough -> idle.
//     if (distToGoalSq <= RVO_SETTLE_GOAL_RADIUS * RVO_SETTLE_GOAL_RADIUS &&
//         state->move.settleTimer >= RVO_SETTLE_TIME_THRESHOLD)
//     {
//         if (!wst_changeStateNextState(context, entity, state))
//         {
//             WarState* idleState = wst_createIdleState(context, entity, true);
//             wst_changeNextState(context, entity, idleState, true, true);
//         }

//         TracyCZoneEnd(ctx);
//         return;
//     }

//     // ── 2. PREFERRED VELOCITY WITH ARRIVAL DECELERATION ──────────────────────
//     // Scale down preferredVelocity as the unit nears the final goal. The scoring function
//     // will then find that the zero-velocity candidate (full stop) or a
//     // low-speed candidate wins over the full-speed one — naturally encouraging
//     // the unit to coast to a halt inside the crowd rather than fighting it.
// #define RVO_ARRIVAL_SLOWDOWN_RADIUS (MEGA_TILE_WIDTH * 3.0f)  // 48 px = 3 tiles
// #define RVO_ARRIVAL_MIN_SCALE       0.25f

//     vec2 direction = wpath_flowFieldSample(flowField, (s32)tile.x, (s32)tile.y);
//     wu_setUnitDirection(context, entity, wu_getDirectionFromDiff(direction.x, direction.y));

//     f32 speed = (f32)stats->speeds[unit->speed];
//     vec2 preferredVelocity = vec2_mulf(direction, speed);

//     f32 distToGoal = sqrtf(distToGoalSq);
//     if (distToGoal < RVO_ARRIVAL_SLOWDOWN_RADIUS)
//     {
//         f32 scale = distToGoal / RVO_ARRIVAL_SLOWDOWN_RADIUS;
//         scale = MAX(scale, RVO_ARRIVAL_MIN_SCALE);
//         preferredVelocity = vec2_mulf(preferredVelocity, scale);
//     }

//     // ── 3. RVO STEERING ───────────────────────────────────────────────────────
//     const f32 SEARCH_RADIUS_PX = 80.0f;    // ~5 tiles
//     f32       myRadius         = (f32)MEGA_TILE_WIDTH * 0.45f;

//     WarRvoNeighbour neighbours[RVO_MAX_NB];
//     s32 numNeighbours = rvo_gatherNeighbours(context, entity->id, position, SEARCH_RADIUS_PX, neighbours);

//     logDebug("RVO: unit %d pos=(%.2f, %.2f) goal=(%.2f, %.2f) distToGoal=%.2f numNeighbours=%d",
//         entity->id,
//         position.x, position.y,
//         nextGoal.x, nextGoal.y,
//         distToGoal,
//         numNeighbours
//     );

//     vec2  candidates[RVO_MAX_CANDIDATES];
//     bool  hadCollision[RVO_MAX_CANDIDATES] = { 0 };
//     s32   numCandidates = 0;
//     s32   bestIndex     = 0;

//     vec2 newVelocity = wrvo_computeNewVelocity(
//         preferredVelocity,
//         position, state->move.rvoVelocity, myRadius,
//         speed,
//         context->gameDeltaTime,
//         neighbours, numNeighbours,
//         candidates, &numCandidates, &bestIndex, hadCollision
//     );

//     logDebug("RVO: unit %d pos=(%.2f, %.2f) vel=(%.2f, %.2f) prefVel=(%.2f, %.2f) newVel=(%.2f, %.2f) bestIdx=%d numCand=%d",
//         entity->id,
//         position.x, position.y,
//         state->move.rvoVelocity.x, state->move.rvoVelocity.y,
//         preferredVelocity.x, preferredVelocity.y,
//         newVelocity.x, newVelocity.y,
//         bestIndex, numCandidates
//     );

//     state->move.rvoVelocity          = newVelocity;
//     state->move.rvoPreferredVelocity = preferredVelocity;
//     state->move.rvoPosition          = position;
//     state->move.rvoRadius            = myRadius;
//     state->move.rvoNumCandidates     = numCandidates;
//     state->move.rvoBestIndex         = bestIndex;
//     for (s32 i = 0; i < numCandidates; i++)
//     {
//         state->move.rvoCandidates[i]            = candidates[i];
//         state->move.rvoCandidateHadCollision[i] = hadCollision[i];
//     }

//     // Overshoot clamp: if step would carry us past the waypoint, snap to it.
//     vec2 step = vec2_mulf(newVelocity, context->gameDeltaTime);
//     // if (vec2_lenSq(step) > vec2_lenSq(direction)) step = direction;

//     vec2 newPosition = vec2_addv(position, step);
//     wu_setUnitCenterPosition(context, entity, newPosition, false);

//     f32 distance = vec2_distance(newPosition, nextPosition);
//     if (distance < MOVE_EPSILON)
//     {
//         newPosition = nextPosition;
//         wu_setUnitCenterPosition(context, entity, newPosition, false);

//         state->move.waypointsIndex++;

//         if (state->move.waypointsIndex >= state->move.waypointsCount - 1)
//         {
//             if (!wst_changeStateNextState(context, entity, state))
//             {
//                 WarState* idleState = wst_createIdleState(context, entity, true);
//                 wst_changeNextState(context, entity, idleState, true, true);
//             }

//             TracyCZoneEnd(ctx);
//             return;
//         }
//     }

    TracyCZoneEnd(ctx);
}

void wst_freeMoveState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeMoveState", true);
    NOT_USED(context);
    NOT_USED(state);
    TracyCZoneEnd(ctx);
}
