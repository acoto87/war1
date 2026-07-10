#include <math.h>
#include <float.h>

#include "war.h"
#include "war_rvo.h"

// -----------------------------------------------------------------------
// Time-to-collision under the RVO assumption.
// "candidate" is the candidate velocity being evaluated for agent A.
// "velocityA"    is A's *current* (last-frame) velocity.
// "velocityB"    is B's current velocity.
// Returns the earliest positive time at which the two discs would touch,
// or FLT_MAX if no collision is predicted.
// -----------------------------------------------------------------------
static f32 wrvo_tcc(
    vec2 positionA, vec2 velocityA,
    vec2 positionB, vec2 velocityB,
    f32  radius,
    vec2 candidate)
{
    // relPos: position of A relative to B
    vec2 relPos = { positionA.x - positionB.x, positionA.y - positionB.y };

    // Under RVO, the effective relative velocity is 2*candidate − (velocityA + velocityB).
    // This accounts for B also steering away symmetrically.
    vec2 relVel = {
        2.0f*candidate.x - velocityA.x - velocityB.x,
        2.0f*candidate.y - velocityA.y - velocityB.y
    };

    f32 a = relVel.x*relVel.x + relVel.y*relVel.y;
    f32 b = relPos.x*relVel.x + relPos.y*relVel.y;
    f32 c = relPos.x*relPos.x + relPos.y*relPos.y - radius*radius;

    if (c < 0.0f)  return 0.0f;      // already overlapping: immediate collision
    if (a < 1e-8f) return FLT_MAX;   // zero relative motion

    f32 disc = b*b - a*c;
    if (disc < 0.0f) return FLT_MAX; // paths don't intersect

    f32 t = (-b - sqrtf(disc)) / a;
    return (t >= 0.0f) ? t : FLT_MAX;
}

// -----------------------------------------------------------------------
// Score a candidate velocity.
// Lower score = better (less collision risk, less deviation from goal).
// -----------------------------------------------------------------------
static f32 wrvo_score(
    vec2 candidate, f32 maxSpeed,
    vec2 preferredVelocity,
    vec2 positionA, vec2 velocityA, f32 radiusA,
    const WarRvoNeighbour* neighbours, s32 numNeighbours)
{
    // Deviation from preferred velocity (Euclidean distance in velocity space).
    f32 dx = candidate.x - preferredVelocity.x;
    f32 dy = candidate.y - preferredVelocity.y;
    f32 s  = RVO_DEVIATION_W * sqrtf(dx*dx + dy*dy);

    // Collision penalties.
    for (s32 i = 0; i < numNeighbours; i++)
    {
        f32 r = radiusA + neighbours[i].radius;
        f32 t = wrvo_tcc(positionA, velocityA, neighbours[i].position, neighbours[i].velocity, r, candidate);
        if (t < RVO_TIME_HORIZON)
        {
            s += (RVO_COLLISION_W * maxSpeed) * (RVO_TIME_HORIZON - t);
        }
    }

    return s;
}

vec2 wrvo_computeNewVelocity(
    vec2  preferredVelocity,
    vec2  positionA,
    vec2  velocityA,
    f32   radiusA,
    f32   maxSpeed,
    f32   dt,
    const WarRvoNeighbour* neighbours,
    s32   numNeighbours,
    // Debug output (all optional, may be NULL):
    vec2* outCandidates,
    s32*  outNumCandidates,
    s32*  outBestIndex,
    bool* outCandidateHadCollision)
{
    NOT_USED(dt); // not needed for scoring, kept for API consistency with ORCA variant

    // ---- Build candidate set ----
    vec2 candidates[RVO_MAX_CANDIDATES];
    s32  numCandidates = 0;

    // Preferred velocity (clip to maxSpeed).
    f32 preferredLength = vec2_length(preferredVelocity);
    if (preferredLength > maxSpeed && preferredLength > 1e-6f)
    {
        f32 inv = maxSpeed / preferredLength;
        candidates[numCandidates++] = vec2f(preferredVelocity.x*inv, preferredVelocity.y*inv);
    }
    else
    {
        candidates[numCandidates++] = preferredVelocity;
    }

    // Rings at maxSpeed and maxSpeed*0.5.
    f32 speeds[2] = { maxSpeed, maxSpeed * 0.5f };
    for (s32 ring = 0; ring < 2; ring++)
    {
        f32 speed = speeds[ring];

        for (s32 k = 0; k < RVO_RING_N; k++)
        {
            f32 a = (f32)k * (2.0f * PI / (f32)RVO_RING_N);
            candidates[numCandidates++] = vec2f(cosf(a)*speed, sinf(a)*speed);
        }
    }

    // Zero velocity (full stop).
    candidates[numCandidates++] = vec2f(0.0f, 0.0f);

    // ---- Mirror to out-params if requested ----
    if (outCandidates)
    {
        for (s32 i = 0; i < numCandidates; i++)
            outCandidates[i] = candidates[i];
    }
    if (outNumCandidates)
        *outNumCandidates = numCandidates;

    // ---- Score and pick ----
    s32 bestIndex = 0;
    f32 bestScore = wrvo_score(candidates[0], maxSpeed, preferredVelocity, positionA, velocityA, radiusA, neighbours, numNeighbours);

    for (s32 i = 1; i < numCandidates; i++)
    {
        f32 s = wrvo_score(candidates[i], maxSpeed, preferredVelocity, positionA, velocityA, radiusA, neighbours, numNeighbours);
        if (s < bestScore)
        {
            bestScore = s;
            bestIndex = i;
        }
    }

    // ---- Per-candidate collision-penalty bookkeeping (optional) ----
    if (outCandidateHadCollision)
    {
        for (s32 i = 0; i < numCandidates; i++)
        {
            bool hadCollision = false;
            for (s32 n = 0; n < numNeighbours; n++)
            {
                f32 r = radiusA + neighbours[n].radius;
                f32 t = wrvo_tcc(positionA, velocityA,
                                neighbours[n].position, neighbours[n].velocity,
                                r, candidates[i]);
                if (t < RVO_TIME_HORIZON)
                {
                    hadCollision = true;
                    break;
                }
            }
            outCandidateHadCollision[i] = hadCollision;
        }
    }

    if (outBestIndex)
        *outBestIndex = bestIndex;

    return candidates[bestIndex];
}

// Collect moving units near `centrePixel` within `radiusPx`.
// Returns count (capped at RVO_MAX_NB).
s32 wrvo_gatherNeighbours(
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

                vec2 oPos = wu_getUnitCenterPosition(context, other);
                f32  dx   = oPos.x - centrePixel.x;
                f32  dy   = oPos.y - centrePixel.y;
                if (dx*dx + dy*dy > radiusPx*radiusPx) continue;

                // Read the neighbour's last RVO velocity (zero if not moving).
                vec2 oVel = VEC2_ZERO;

                WarStateMove* moveState = wst_getMoveState(context, other);
                if (moveState)
                {
                    oVel = moveState->rvoVelocity;
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
