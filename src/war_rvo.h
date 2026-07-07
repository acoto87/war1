#pragma once

#include <stdbool.h>

#include "war_math.h"

#define RVO_MAX_NB 16
#define RVO_RING_N 16
#define RVO_MAX_CANDIDATES (1 + RVO_RING_N*2 + 1) // 34

#define RVO_TIME_HORIZON 2.0f // seconds
#define RVO_COLLISION_W  1.5f
#define RVO_DEVIATION_W  1.0f

typedef struct {
    vec2 position;
    vec2 velocity;
    f32  radius;
} WarRvoNeighbour;

// Main entry point called from the move state.
// Returns the best velocity found, guaranteed to have magnitude <= maxSpeed.
vec2 wrvo_computeNewVelocity(
    vec2  prefVel,
    vec2  posA,
    vec2  vA,
    f32   radiusA,
    f32   maxSpeed,
    f32   dt,
    const WarRvoNeighbour* neighbours,
    s32   numNeighbours,
    // Debug output (all optional, may be NULL):
    vec2* outCandidates,                  // [out] size RVO_MAX_CANDIDATES
    s32*  outNumCandidates,               // [out]
    s32*  outBestIndex,                   // [out] index in outCandidates
    bool* outCandidateHadCollision);      // [out] per-candidate penalty flag

// Collect moving units near `centrePixel` within `radiusPx`.
// Returns count (capped at RVO_MAX_NB).
s32 wrvo_gatherNeighbours(
    WarContext* context,
    s32         selfId,
    vec2        centrePixel,
    f32         radiusPx,
    WarRvoNeighbour out[RVO_MAX_NB]);
