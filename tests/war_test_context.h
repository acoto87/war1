#pragma once

#include "war.h"

#define WAR_TEST_TICK_RATE 30
#define WAR_TEST_DELTA_TIME (1.0f / WAR_TEST_TICK_RATE)

typedef struct WarTestContext
{
    WarContext* context;
    WarMap*     map;
    u64         simulationTick;
    f32         fixedDeltaTime;
} WarTestContext;

void wt_init(WarTestContext* test);
void wt_shutdown(WarTestContext* test);

void wt_updateGameTime(WarTestContext* test);
void wt_updateStateMachines(WarTestContext* test);
void wt_applyPendingTransitions(WarTestContext* test);
void wt_advanceTick(WarTestContext* test);
void wt_setUnitCenterPosition(WarTestContext* test, WarEntity* unit, vec2 position);
void wt_setUnitVelocity(WarTestContext* test, WarEntity* unit, vec2 velocity);
void wt_buildGrid(WarTestContext* test);

void wt_step(WarTestContext* test);
void wt_stepTicks(WarTestContext* test, u32 ticks);

WarEntity* wt_spawnUnit(WarTestContext* test, WarUnitType type, s32 player, vec2 position);
WarEntity* wt_spawnBuilding(WarTestContext* test, WarUnitType type, s32 player, vec2 position);

u8 wt_stateDepth(WarTestContext* test, WarEntity* entity);
WarStateType wt_activeState(WarTestContext* test, WarEntity* entity);
WarStateType wt_stateAt(WarTestContext* test, WarEntity* entity, u8 stackIndex);
bool wt_stackContains(WarTestContext* test, WarEntity* entity, WarStateType type);
WarTransitionRequest* wt_activeTransition(WarTestContext* test, WarEntity* entity);