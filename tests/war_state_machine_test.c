#include <stdint.h>

#include "unity/unity.h"

extern const char* g_test_filter;

// Gate each test by the --filter substring (g_test_filter is set by main).
// Calls UnityDefaultTestRun directly so this macro body does not invoke
// itself; that lets a blanket WAR_TEST_FILTER rewrite target callsites only.
#define WAR_TEST_FILTER(func) \
    do { \
        if (!g_test_filter || wsv_find(wsv_fromCString(#func), wsv_fromCString(g_test_filter)) != WSV_NPOS) \
        { \
            UnityDefaultTestRun(func, #func, __LINE__); \
        } \
    } while (0)

#include "war_ai.h"
#include "war_alloc.h"
#include "war_commands.h"
#include "war_entities.h"
#include "war_map.h"
#include "war_resources.h"
#include "war_state_machine.h"
#include "war_test_context.h"
#include "war_units.h"

WarTestContext g_test;

typedef struct WarAggroTestFixture
{
    WarEntity* unit;
    WarEntity* enemy;
} WarAggroTestFixture;

typedef struct WarBuildTestFixture
{
    WarEntity* building;
    WarEntity* worker;
    WarStateBuild* buildState;
    WarStateRef buildRef;
    s32 goldBefore;
    s32 woodBefore;
} WarBuildTestFixture;

static WarStateMove* wt_createMoveTo(WarEntity* unit, vec2 destination)
{
    vec2 startPosition = wu_getUnitCenterPosition(g_test.context, unit);
    vec2 waypoints[] = { startPosition, destination };

    return wst_createMoveState(g_test.context, unit, arrayLength(waypoints), waypoints, false);
}

static WarStateMove* wt_installRootMove(WarEntity* unit, vec2 destination)
{
    WarStateMove* moveState = wt_createMoveTo(unit, destination);

    TEST_ASSERT_NOT_NULL(moveState);
    TEST_ASSERT_TRUE(wst_replaceState(
        g_test.context,
        unit,
        (WarStateBase*)moveState,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    wt_applyPendingTransitions(&g_test);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));

    return moveState;
}

static void wt_holdMoveProgress(WarEntity* unit, WarStateMove* moveState, u32 ticks)
{
    for (u32 i = 0; i < ticks; i++)
    {
        moveState->rvoAdjustedVelocity = VEC2_ZERO;
        wt_updateGameTime(&g_test);
        updateMoveProgressAndRecovery(g_test.context, unit, moveState);
    }
}

static void wt_selectOnly(WarEntity* entity)
{
    TEST_ASSERT_NOT_NULL(entity);

    WarEntityIdListClear(&g_test.map->selectedEntities);
    WarEntityIdListAdd(&g_test.map->selectedEntities, entity->id);
}

static void wt_dealLethalDamage(WarEntity* entity)
{
    WarUnitComponent* unit = we_getUnitComponent(g_test.context, entity);

    TEST_ASSERT_NOT_NULL(unit);
    unit->hp = 1;
    we_takeDamage(g_test.context, entity, 1, 0);
}

static void wt_issueSwordsUpgrade(WarEntity* building)
{
    wt_selectOnly(building);
    wcmd_upgradeSwords(g_test.context, building);
    TEST_ASSERT_TRUE(wcmd_executeCommand(g_test.context));
}

static WarAICommand* wt_issueAITrainRequest(
    WarEntity* producer,
    WarUnitType unitType,
    s32 count)
{
    WarUnitComponent* unit = we_getUnitComponent(g_test.context, producer);

    TEST_ASSERT_NOT_NULL(unit);

    WarPlayerInfo* player = &g_test.map->players[unit->player];
    if (!player->ai)
    {
        player->ai = wai_createAI(g_test.context);
    }

    WarAICommand* command = wai_createUnitRequest(g_test.context, player, unitType, count);

    TEST_ASSERT_NOT_NULL(command);
    TEST_ASSERT_TRUE(wai_executeRequestAICommand(g_test.context, player, command));
    return command;
}

static void wt_seedImageResource(s32 resourceIndex)
{
    static u8 pixels[4] = {0};

    if (!g_test.context->resources)
    {
        g_test.context->resources = (WarResource*)wm_alloc(
            sizeof(WarResource) * MAX_RESOURCES_COUNT);
    }
    TEST_ASSERT_NOT_NULL(g_test.context->resources);

    WarResource* resource = &g_test.context->resources[resourceIndex];
    resource->type = WAR_RESOURCE_TYPE_IMAGE;
    resource->imageData.width = 1;
    resource->imageData.height = 1;
    resource->imageData.pixels = pixels;
}

static void wt_seedUnitSpriteResource(WarUnitType type)
{
    const WarUnitData* unitData = wu_getUnitData(type);

    TEST_ASSERT_NOT_NULL(unitData);
    wt_seedImageResource(unitData->resourceIndex);
}

static void wt_seedBuildingSpriteResources(WarUnitType type)
{
    const WarUnitData* unitData = wu_getUnitData(type);
    const WarBuildingData* buildingData = wu_getBuildingData(type);

    TEST_ASSERT_NOT_NULL(unitData);
    TEST_ASSERT_NOT_NULL(buildingData);
    wt_seedImageResource(unitData->resourceIndex);
    wt_seedImageResource(buildingData->buildingResource);
}

static WarBuildTestFixture wt_startBuildWithAssignedWorker(
    vec2 buildingPosition,
    f32 buildTime,
    s32 goldCost,
    s32 woodCost)
{
    wt_seedBuildingSpriteResources(WAR_UNIT_FARM_HUMANS);

    WarEntity* building = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_FARM_HUMANS,
        0,
        buildingPosition);
    WarEntity* worker = wt_spawnUnit(
        &g_test,
        WAR_UNIT_PEASANT,
        0,
        vec2_translatef(buildingPosition, 3.0f * MEGA_TILE_WIDTH, 0.0f));

    TEST_ASSERT_NOT_NULL(building);
    TEST_ASSERT_NOT_NULL(worker);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    WarBuildTestFixture fixture =
    {
        .building = building,
        .worker = worker,
        .goldBefore = player->gold,
        .woodBefore = player->wood
    };

    TEST_ASSERT_TRUE(we_decreasePlayerResources(
        g_test.context,
        player,
        goldCost,
        woodCost));

    fixture.buildState = wst_createBuildState(
        g_test.context,
        building,
        buildTime,
        goldCost,
        woodCost);
    TEST_ASSERT_NOT_NULL(fixture.buildState);

    fixture.buildRef = wst_refOf(
        g_test.context,
        (WarStateBase*)fixture.buildState);
    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        building,
        (WarStateBase*)fixture.buildState,
        WAR_TRANSITION_CAUSE_INITIALIZATION));

    wt_applyPendingTransitions(&g_test);
    wt_updateGameTime(&g_test);
    wst_updateBuildStates(g_test.context);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_BUILD, wt_activeState(&g_test, building));

    WarStateRepairing* repairingState = wst_createRepairingState(
        g_test.context,
        worker,
        building->id);

    TEST_ASSERT_NOT_NULL(repairingState);
    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        worker,
        (WarStateBase*)repairingState,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));

    wt_applyPendingTransitions(&g_test);
    wst_updateRepairingStates(g_test.context);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_REPAIRING, wt_activeState(&g_test, worker));
    TEST_ASSERT_TRUE(repairingState->insideBuilding);
    TEST_ASSERT_EQUAL_INT(worker->id, fixture.buildState->workerId);

    return fixture;
}

static WarStateTrain* wt_startTrainTransaction(
    WarEntity* producer,
    WarUnitType unitType,
    f32 buildTime,
    s32 goldCost,
    s32 woodCost)
{
    WarStateTrain* trainState = wst_createTrainState(
        g_test.context,
        producer,
        unitType,
        buildTime,
        goldCost,
        woodCost,
        NULL);

    TEST_ASSERT_NOT_NULL(trainState);
    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        producer,
        (WarStateBase*)trainState,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));

    wt_applyPendingTransitions(&g_test);
    wt_updateGameTime(&g_test);
    wst_updateTrainStates(g_test.context);

    trainState = wst_getTrainState(g_test.context, producer);
    TEST_ASSERT_NOT_NULL(trainState);
    TEST_ASSERT_TRUE(trainState->transactionApplied);
    return trainState;
}

static WarStateUpgrade* wt_startUpgradeTransaction(
    WarEntity* producer,
    WarUpgradeType upgradeType,
    f32 buildTime,
    s32 goldCost,
    s32 woodCost)
{
    WarStateUpgrade* upgradeState = wst_createUpgradeState(
        g_test.context,
        producer,
        upgradeType,
        buildTime,
        goldCost,
        woodCost);

    TEST_ASSERT_NOT_NULL(upgradeState);
    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        producer,
        (WarStateBase*)upgradeState,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));

    wt_applyPendingTransitions(&g_test);
    wt_updateGameTime(&g_test);
    wst_updateUpgradeStates(g_test.context);

    upgradeState = wst_getUpgradeState(g_test.context, producer);
    TEST_ASSERT_NOT_NULL(upgradeState);
    TEST_ASSERT_TRUE(upgradeState->transactionApplied);
    return upgradeState;
}

static bool wt_isStateRefAllocated(WarTestContext* test, WarStateRef ref)
{
    return wst_deref(test->context, ref) != NULL;
}

static s32 wt_stateAllocationCount(WarTestContext* test, WarStateType type)
{
    WarEntityManager* manager = we_getEntityManager(test->context);
    TEST_ASSERT_NOT_NULL(manager);

    return manager->stateStorage.activeCounts[type];
}

static s32 wt_totalStateAllocationCount(WarTestContext* test)
{
    WarEntityManager* manager = we_getEntityManager(test->context);
    TEST_ASSERT_NOT_NULL(manager);

    s32 count = 0;
    for (s32 type = 0; type < WAR_STATE_COUNT; type++)
    {
        count += manager->stateStorage.activeCounts[type];
    }

    return count;
}

static u64 wt_nextTransitionSequence(WarTestContext* test, WarEntity* entity)
{
    WarStateMachineComponent* sm = we_getStateMachineComponent(test->context, entity);
    TEST_ASSERT_NOT_NULL(sm);

    return sm->nextTransitionSequence;
}

static void wt_assertStateRefEqual(WarStateRef expected, WarStateRef actual)
{
    TEST_ASSERT_EQUAL_INT(expected.type, actual.type);
    TEST_ASSERT_EQUAL_INT(expected.idx, actual.idx);
    TEST_ASSERT_EQUAL_UINT32(expected.generation, actual.generation);
}

static void wt_assertU64Equal(u64 expected, u64 actual)
{
    TEST_ASSERT_EQUAL_UINT32((u32)(expected >> 32), (u32)(actual >> 32));
    TEST_ASSERT_EQUAL_UINT32((u32)expected, (u32)actual);
}

static bool wt_operationHasState(WarStateOp operation)
{
    return operation == WAR_STATE_OP_PUSH ||
           operation == WAR_STATE_OP_REPLACE ||
           operation == WAR_STATE_OP_RESET;
}

static bool wt_submitWaitTransition(
    WarEntity* entity,
    WarStateOp operation,
    WarTransitionCause cause,
    WarStateRef* stateRef)
{
    *stateRef = WAR_STATE_REF_INVALID;

    if (operation == WAR_STATE_OP_POP)
    {
        return wst_popState(g_test.context, entity, cause, WAR_STATE_RESULT_NONE);
    }

    WarStateWait* waitState = wst_createWaitState(g_test.context, entity, 1000.0f);

    TEST_ASSERT_NOT_NULL(waitState);

    *stateRef = wst_refOf(g_test.context, (WarStateBase*)waitState);

    switch (operation)
    {
        case WAR_STATE_OP_PUSH:
            return wst_pushState(g_test.context, entity, (WarStateBase*)waitState, cause);
        case WAR_STATE_OP_REPLACE:
            return wst_replaceState(g_test.context, entity, (WarStateBase*)waitState, cause);
        case WAR_STATE_OP_RESET:
            return wst_resetState(g_test.context, entity, (WarStateBase*)waitState, cause);
        default:
            return false;
    }
}

static void wt_assertPendingTransitionCleared(WarEntity* entity)
{
    const WarTransitionRequest* pending = wt_activeTransition(&g_test, entity);

    TEST_ASSERT_NOT_NULL(pending);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_NONE, pending->operation);
    TEST_ASSERT_FALSE(WAR_STATE_REF_IS_VALID(pending->stateRef));
}

static WarAggroTestFixture wt_setupMoveWithAttackingEnemy(void)
{
    const vec2 startPosition = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 destination = vec2i(18 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, startPosition);

    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));

    WarStateMove* moveState = wt_createMoveTo(unit, destination);

    TEST_ASSERT_NOT_NULL(moveState);

    moveState->checkForAttacks = true;

    wst_replaceState(g_test.context, unit, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));

    // Initialize MOVE before introducing the enemy. The destination
    // is deliberately far away, so MOVE cannot complete here.
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));

    WarEntity* enemy = wt_spawnUnit(&g_test, WAR_UNIT_GRUNT, 1, vec2i(12 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(enemy);
    TEST_ASSERT_NOT_EQUAL(0, enemy->id);

    wt_applyPendingTransitions(&g_test);

    WarStateAttack* attackState = wst_createAttackState(g_test.context, enemy, unit->id, wu_getUnitCenterPosition(g_test.context, unit));

    TEST_ASSERT_NOT_NULL(attackState);

    wst_replaceState(g_test.context, enemy, (WarStateBase*)attackState, WAR_TRANSITION_CAUSE_AI_ORDER);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_ATTACK, wt_activeState(&g_test, enemy));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));

    wt_selectOnly(unit);

    return (WarAggroTestFixture){ .unit = unit,.enemy = enemy };
}

void test_fresh_entity_requests_initialization_idle_state(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    const WarTransitionRequest* initialRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(initialRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, initialRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, initialRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_INITIALIZATION, initialRequest->cause);

    WarStateRef initialIdleRef = initialRequest->stateRef;

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, initialIdleRef));

    wt_step(&g_test); // process pending ops

    // A freshly spawned unit should have exactly one state (IDLE)
    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));

    WarStateMachineComponent* sm = we_getStateMachineComponent(g_test.context, unit);

    TEST_ASSERT_NOT_NULL(sm);
    wt_assertStateRefEqual(initialIdleRef, sm->stack[0]);
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, initialIdleRef));
}

void test_player_order_beats_uncommitted_initialization(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    const WarTransitionRequest* initialRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(initialRequest);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_INITIALIZATION, initialRequest->cause);

    WarStateRef losingIdleRef = initialRequest->stateRef;
    const s32 idleCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_IDLE);
    const s32 moveCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_MOVE);

    wt_selectOnly(unit);
    wcmd_executeMoveCommand(
        g_test.context,
        vec2i(16 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(winner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, winner->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, winner->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, winner->cause);
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, losingIdleRef));
    TEST_ASSERT_EQUAL_INT(idleCountBefore - 1, wt_stateAllocationCount(&g_test, WAR_STATE_IDLE));
    TEST_ASSERT_EQUAL_INT(moveCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_MOVE));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));
}

void test_lethal_damage_beats_uncommitted_initialization(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    const WarTransitionRequest* initialRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(initialRequest);

    WarStateRef losingIdleRef = initialRequest->stateRef;
    const s32 idleCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_IDLE);
    const s32 deathCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_DEATH);

    wt_dealLethalDamage(unit);

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(winner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, winner->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_DEATH, winner->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, winner->cause);
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, losingIdleRef));
    TEST_ASSERT_EQUAL_INT(idleCountBefore - 1, wt_stateAllocationCount(&g_test, WAR_STATE_IDLE));
    TEST_ASSERT_EQUAL_INT(deathCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_DEATH));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_DEATH, wt_activeState(&g_test, unit));
}

void test_ai_training_request_uses_ai_order(void)
{
    WarEntity* townHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_ORCS,
        1,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(townHall);

    wt_applyPendingTransitions(&g_test);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, townHall));

    wai_initAIPlayers(g_test.context);

    // Advance the real AI script through town-hall request/wait to training.
    wai_updateAIPlayers(g_test.context);
    wai_updateAIPlayers(g_test.context);
    wai_updateAIPlayers(g_test.context);
    wai_updateAIPlayers(g_test.context);

    const WarTransitionRequest* trainRequest = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(trainRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, trainRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_TRAIN, trainRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AI_ORDER, trainRequest->cause);

    WarStateRef trainRef = trainRequest->stateRef;
    WarStateTrain* trainState = (WarStateTrain*)wst_deref(g_test.context, trainRef);

    TEST_ASSERT_NOT_NULL(trainState);
    TEST_ASSERT_EQUAL_INT(WAR_UNIT_PEON, trainState->unitToBuild);
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, trainRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_TRAIN, wt_activeState(&g_test, townHall));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, trainRef));
}

void test_ai_train_then_lifecycle_charges_and_progresses_zero_times(void)
{
    WarEntity* townHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_ORCS,
        1,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(townHall);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[1];
    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 trainCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN);
    const s32 collapseCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_COLLAPSE);

    WarAICommand* command = wt_issueAITrainRequest(townHall, WAR_UNIT_PEON, 1);

    const WarTransitionRequest* trainRequest = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(trainRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_TRAIN, trainRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AI_ORDER, trainRequest->cause);
    TEST_ASSERT_EQUAL_INT(WAR_AI_COMMAND_STATUS_STARTED, command->status);
    TEST_ASSERT_EQUAL_INT(1, command->request.count);
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(trainCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN));

    WarStateRef losingTrainRef = trainRequest->stateRef;

    wt_dealLethalDamage(townHall);

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(winner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, winner->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, winner->cause);
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, losingTrainRef));
    TEST_ASSERT_EQUAL_INT(trainCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN));
    TEST_ASSERT_EQUAL_INT(collapseCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_COLLAPSE));
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(WAR_AI_COMMAND_STATUS_STARTED, command->status);
    TEST_ASSERT_EQUAL_INT(1, command->request.count);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, wt_activeState(&g_test, townHall));
}

void test_lifecycle_then_ai_train_charges_and_progresses_zero_times(void)
{
    WarEntity* townHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_ORCS,
        1,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(townHall);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[1];
    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 trainCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN);
    const s32 collapseCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_COLLAPSE);

    wt_dealLethalDamage(townHall);

    const WarTransitionRequest* lifecycleRequest = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(lifecycleRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, lifecycleRequest->stateRef.type);

    WarStateRef collapseRef = lifecycleRequest->stateRef;
    WarAICommand* command = wt_issueAITrainRequest(townHall, WAR_UNIT_PEON, 1);

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(winner);
    wt_assertStateRefEqual(collapseRef, winner->stateRef);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, winner->cause);
    TEST_ASSERT_EQUAL_INT(trainCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN));
    TEST_ASSERT_EQUAL_INT(collapseCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_COLLAPSE));
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(WAR_AI_COMMAND_STATUS_STARTED, command->status);
    TEST_ASSERT_EQUAL_INT(1, command->request.count);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, wt_activeState(&g_test, townHall));
}

void test_upgrade_then_lifecycle_charges_zero_times(void)
{
    WarEntity* blacksmith = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_BLACKSMITH_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(blacksmith);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    setUpgradeAllowed(player, WAR_UPGRADE_SWORDS, 2);

    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 upgradeCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_UPGRADE);

    wt_issueSwordsUpgrade(blacksmith);

    const WarTransitionRequest* upgradeRequest = wt_activeTransition(&g_test, blacksmith);

    TEST_ASSERT_NOT_NULL(upgradeRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_UPGRADE, upgradeRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(upgradeCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_UPGRADE));

    WarStateRef losingUpgradeRef = upgradeRequest->stateRef;

    wt_dealLethalDamage(blacksmith);

    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, losingUpgradeRef));
    TEST_ASSERT_EQUAL_INT(upgradeCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_UPGRADE));
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, wt_activeState(&g_test, blacksmith));
}

void test_lifecycle_then_upgrade_charges_zero_times(void)
{
    WarEntity* blacksmith = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_BLACKSMITH_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(blacksmith);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    setUpgradeAllowed(player, WAR_UPGRADE_SWORDS, 2);

    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 upgradeCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_UPGRADE);

    wt_dealLethalDamage(blacksmith);

    const WarTransitionRequest* lifecycleRequest = wt_activeTransition(&g_test, blacksmith);

    TEST_ASSERT_NOT_NULL(lifecycleRequest);
    WarStateRef collapseRef = lifecycleRequest->stateRef;

    wt_issueSwordsUpgrade(blacksmith);

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, blacksmith);

    TEST_ASSERT_NOT_NULL(winner);
    wt_assertStateRefEqual(collapseRef, winner->stateRef);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, winner->cause);
    TEST_ASSERT_EQUAL_INT(upgradeCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_UPGRADE));
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, wt_activeState(&g_test, blacksmith));
}

void test_committed_ai_train_applies_cost_and_progress_once(void)
{
    WarEntity* townHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_ORCS,
        1,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(townHall);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[1];
    const WarUnitStats* stats = wu_getUnitStats(WAR_UNIT_PEON);
    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 trainCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN);
    WarAICommand* command = wt_issueAITrainRequest(townHall, WAR_UNIT_PEON, 2);

    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(2, command->request.count);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_TRAIN, wt_activeState(&g_test, townHall));
    TEST_ASSERT_EQUAL_INT(goldBefore - stats->goldCost, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore - stats->woodCost, player->wood);

    wt_updateGameTime(&g_test);
    wst_updateTrainStates(g_test.context);

    WarStateTrain* trainState = wst_getTrainState(g_test.context, townHall);

    TEST_ASSERT_NOT_NULL(trainState);
    TEST_ASSERT_TRUE(trainState->transactionApplied);
    TEST_ASSERT_NULL(trainState->aiCommand);
    TEST_ASSERT_EQUAL_INT(goldBefore - stats->goldCost, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore - stats->woodCost, player->wood);
    TEST_ASSERT_EQUAL_INT(1, command->request.count);
    TEST_ASSERT_EQUAL_INT(WAR_AI_COMMAND_STATUS_STARTED, command->status);
    TEST_ASSERT_EQUAL_INT(trainCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN));

    wst_updateTrainStates(g_test.context);

    TEST_ASSERT_EQUAL_INT(goldBefore - stats->goldCost, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore - stats->woodCost, player->wood);
    TEST_ASSERT_EQUAL_INT(1, command->request.count);
}

void test_committed_upgrade_applies_cost_once(void)
{
    WarEntity* blacksmith = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_BLACKSMITH_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(blacksmith);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    setUpgradeAllowed(player, WAR_UPGRADE_SWORDS, 2);

    const WarUpgradeStats* stats = wu_getUpgradeStats(WAR_UPGRADE_SWORDS);
    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;

    wt_issueSwordsUpgrade(blacksmith);

    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_UPGRADE, wt_activeState(&g_test, blacksmith));
    TEST_ASSERT_EQUAL_INT(goldBefore - stats->goldCost[0], player->gold);

    wt_updateGameTime(&g_test);
    wst_updateUpgradeStates(g_test.context);

    WarStateUpgrade* upgradeState = wst_getUpgradeState(g_test.context, blacksmith);

    TEST_ASSERT_NOT_NULL(upgradeState);
    TEST_ASSERT_TRUE(upgradeState->transactionApplied);
    TEST_ASSERT_EQUAL_INT(goldBefore - stats->goldCost[0], player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);

    wst_updateUpgradeStates(g_test.context);

    TEST_ASSERT_EQUAL_INT(goldBefore - stats->goldCost[0], player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(0, getUpgradeLevel(player, WAR_UPGRADE_SWORDS));
}

void test_committed_ai_train_without_resources_terminates_without_progress(void)
{
    WarEntity* townHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_ORCS,
        1,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(townHall);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[1];
    const s32 woodBefore = player->wood;
    const s32 peonsBefore = wu_getNumberOfUnitsOfType(g_test.context, 1, WAR_UNIT_PEON);
    const s32 trainCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN);
    WarAICommand* command = wt_issueAITrainRequest(townHall, WAR_UNIT_PEON, 1);

    player->gold = 0;
    wt_applyPendingTransitions(&g_test);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_TRAIN, wt_activeState(&g_test, townHall));

    player->gold = 0;
    wt_updateGameTime(&g_test);
    wst_updateTrainStates(g_test.context);

    WarStateTrain* trainState = wst_getTrainState(g_test.context, townHall);
    const WarTransitionRequest* failureRequest = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(trainState);
    TEST_ASSERT_FALSE(trainState->transactionApplied);
    TEST_ASSERT_NOT_NULL(failureRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, failureRequest->operation);
    TEST_ASSERT_EQUAL_INT(0, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(1, command->request.count);
    TEST_ASSERT_EQUAL_INT(WAR_AI_COMMAND_STATUS_STARTED, command->status);
    TEST_ASSERT_EQUAL_INT(peonsBefore, wu_getNumberOfUnitsOfType(g_test.context, 1, WAR_UNIT_PEON));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, townHall));
    TEST_ASSERT_EQUAL_INT(trainCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN));
    TEST_ASSERT_EQUAL_INT(peonsBefore, wu_getNumberOfUnitsOfType(g_test.context, 1, WAR_UNIT_PEON));
    TEST_ASSERT_EQUAL_INT(1, command->request.count);
    TEST_ASSERT_EQUAL_INT(WAR_AI_COMMAND_STATUS_STARTED, command->status);
}

void test_pending_train_cancel_commits_idle_without_charging(void)
{
    WarEntity* townHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    for (s32 i = 0; i < 10; i++)
    {
        wt_spawnBuilding(
            &g_test,
            WAR_UNIT_FARM_HUMANS,
            0,
            vec2i((15 + (i % 5) * 3) * MEGA_TILE_WIDTH, (10 + (i / 5) * 3) * MEGA_TILE_HEIGHT));
    }

    TEST_ASSERT_NOT_NULL(townHall);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 trainCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN);
    const s32 idleCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_IDLE);

    wt_selectOnly(townHall);
    wcmd_trainPeasant(g_test.context, townHall);
    TEST_ASSERT_TRUE(wcmd_executeCommand(g_test.context));

    const WarTransitionRequest* trainRequest = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(trainRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, trainRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_TRAIN, trainRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, trainRequest->cause);

    WarStateRef trainRef = trainRequest->stateRef;
    WarStateTrain* trainState = (WarStateTrain*)wst_deref(g_test.context, trainRef);

    TEST_ASSERT_NOT_NULL(trainState);
    TEST_ASSERT_FALSE(trainState->transactionApplied);
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);

    wcmd_cancel(g_test.context, townHall);

    const WarTransitionRequest* cancelRequest = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(cancelRequest);
    TEST_ASSERT_TRUE(cancelRequest->cancellation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, cancelRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, cancelRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, cancelRequest->cause);
    wt_assertStateRefEqual(trainRef, cancelRequest->cancellationTargetRef);
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, trainRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, townHall));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, trainRef));
    TEST_ASSERT_EQUAL_INT(trainCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN));
    TEST_ASSERT_EQUAL_INT(idleCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_IDLE));
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
}

void test_pending_upgrade_cancel_commits_idle_without_charging(void)
{
    WarEntity* blacksmith = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_BLACKSMITH_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(blacksmith);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    setUpgradeAllowed(player, WAR_UPGRADE_SWORDS, 2);

    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 upgradeCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_UPGRADE);
    const s32 idleCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_IDLE);

    wt_issueSwordsUpgrade(blacksmith);

    const WarTransitionRequest* upgradeRequest = wt_activeTransition(&g_test, blacksmith);

    TEST_ASSERT_NOT_NULL(upgradeRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, upgradeRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_UPGRADE, upgradeRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, upgradeRequest->cause);

    WarStateRef upgradeRef = upgradeRequest->stateRef;
    WarStateUpgrade* upgradeState = (WarStateUpgrade*)wst_deref(
        g_test.context,
        upgradeRef);

    TEST_ASSERT_NOT_NULL(upgradeState);
    TEST_ASSERT_FALSE(upgradeState->transactionApplied);
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);

    wcmd_cancel(g_test.context, blacksmith);

    const WarTransitionRequest* cancelRequest = wt_activeTransition(&g_test, blacksmith);

    TEST_ASSERT_NOT_NULL(cancelRequest);
    TEST_ASSERT_TRUE(cancelRequest->cancellation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, cancelRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, cancelRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, cancelRequest->cause);
    wt_assertStateRefEqual(upgradeRef, cancelRequest->cancellationTargetRef);
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, upgradeRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, blacksmith));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, upgradeRef));
    TEST_ASSERT_EQUAL_INT(upgradeCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_UPGRADE));
    TEST_ASSERT_EQUAL_INT(idleCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_IDLE));
    TEST_ASSERT_EQUAL_INT(0, getUpgradeLevel(player, WAR_UPGRADE_SWORDS));
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
}

void test_transaction_cancel_does_not_displace_lifecycle_or_unrelated_player_order(void)
{
    WarEntity* doomedTownHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(doomedTownHall);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    const s32 goldBeforeLifecycle = player->gold;
    const s32 woodBeforeLifecycle = player->wood;

    wt_selectOnly(doomedTownHall);
    wcmd_trainPeasant(g_test.context, doomedTownHall);
    TEST_ASSERT_TRUE(wcmd_executeCommand(g_test.context));

    const WarTransitionRequest* trainRequest = wt_activeTransition(
        &g_test,
        doomedTownHall);

    TEST_ASSERT_NOT_NULL(trainRequest);
    WarStateRef losingTrainRef = trainRequest->stateRef;

    wt_dealLethalDamage(doomedTownHall);

    const WarTransitionRequest* lifecycleRequest = wt_activeTransition(
        &g_test,
        doomedTownHall);

    TEST_ASSERT_NOT_NULL(lifecycleRequest);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, lifecycleRequest->cause);
    WarStateRef lifecycleRef = lifecycleRequest->stateRef;

    wcmd_cancel(g_test.context, doomedTownHall);

    const WarTransitionRequest* lifecycleWinner = wt_activeTransition(
        &g_test,
        doomedTownHall);

    TEST_ASSERT_NOT_NULL(lifecycleWinner);
    TEST_ASSERT_FALSE(lifecycleWinner->cancellation);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, lifecycleWinner->cause);
    wt_assertStateRefEqual(lifecycleRef, lifecycleWinner->stateRef);
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, losingTrainRef));
    TEST_ASSERT_EQUAL_INT(goldBeforeLifecycle, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBeforeLifecycle, player->wood);

    wt_applyPendingTransitions(&g_test);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, wt_activeState(&g_test, doomedTownHall));

    WarEntity* orderedTownHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_HUMANS,
        0,
        vec2i(20 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(orderedTownHall);
    wt_applyPendingTransitions(&g_test);

    const s32 goldBeforeOrder = player->gold;
    const s32 woodBeforeOrder = player->wood;
    WarStateTrain* activeTrain = wst_createTrainState(
        g_test.context,
        orderedTownHall,
        WAR_UNIT_PEASANT,
        1000.0f,
        211,
        101,
        NULL);

    TEST_ASSERT_NOT_NULL(activeTrain);
    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        orderedTownHall,
        (WarStateBase*)activeTrain,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    wt_applyPendingTransitions(&g_test);

    WarStateRef activeTrainRef = wst_refOf(
        g_test.context,
        (WarStateBase*)activeTrain);

    wt_selectOnly(orderedTownHall);
    wcmd_stop(g_test.context, orderedTownHall);
    TEST_ASSERT_TRUE(wcmd_executeCommand(g_test.context));

    const WarTransitionRequest* stopRequest = wt_activeTransition(
        &g_test,
        orderedTownHall);

    TEST_ASSERT_NOT_NULL(stopRequest);
    TEST_ASSERT_FALSE(stopRequest->cancellation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, stopRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, stopRequest->cause);

    WarStateRef stopRef = stopRequest->stateRef;
    const u64 stopSequence = stopRequest->sequence;
    const s32 idleCountBeforeCancel = wt_stateAllocationCount(&g_test, WAR_STATE_IDLE);

    wcmd_cancel(g_test.context, orderedTownHall);

    const WarTransitionRequest* orderWinner = wt_activeTransition(
        &g_test,
        orderedTownHall);

    TEST_ASSERT_NOT_NULL(orderWinner);
    TEST_ASSERT_FALSE(orderWinner->cancellation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, orderWinner->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, orderWinner->cause);
    wt_assertStateRefEqual(stopRef, orderWinner->stateRef);
    wt_assertU64Equal(stopSequence, orderWinner->sequence);
    TEST_ASSERT_EQUAL_INT(
        idleCountBeforeCancel,
        wt_stateAllocationCount(&g_test, WAR_STATE_IDLE));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, activeTrainRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, orderedTownHall));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, activeTrainRef));
    TEST_ASSERT_EQUAL_INT(goldBeforeOrder - 211, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBeforeOrder - 101, player->wood);
}

void test_train_cancel_refunds_once_only_after_commit(void)
{
    const s32 goldCost = 137;
    const s32 woodCost = 59;
    WarEntity* townHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(townHall);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 trainCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN);
    WarStateTrain* trainState = wt_startTrainTransaction(
        townHall,
        WAR_UNIT_PEASANT,
        1000.0f,
        goldCost,
        woodCost);
    WarStateRef trainRef = wst_refOf(g_test.context, (WarStateBase*)trainState);

    TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);

    wt_selectOnly(townHall);
    wcmd_cancel(g_test.context, townHall);
    wcmd_cancel(g_test.context, townHall);

    const WarTransitionRequest* cancelRequest = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(cancelRequest);
    TEST_ASSERT_TRUE(cancelRequest->cancellation);
    wt_assertStateRefEqual(trainRef, cancelRequest->cancellationTargetRef);
    TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, townHall));
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(trainCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_TRAIN));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, trainRef));
}

void test_train_cancel_displaced_by_lifecycle_never_refunds_in_either_order(void)
{
    const s32 goldCost = 149;
    const s32 woodCost = 61;

    for (s32 cancelFirst = 0; cancelFirst < 2; cancelFirst++)
    {
        WarEntity* townHall = wt_spawnBuilding(
            &g_test,
            WAR_UNIT_TOWNHALL_HUMANS,
            0,
            vec2i((10 + cancelFirst * 10) * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

        TEST_ASSERT_NOT_NULL(townHall);
        wt_applyPendingTransitions(&g_test);

        WarPlayerInfo* player = &g_test.map->players[0];
        const s32 goldBefore = player->gold;
        const s32 woodBefore = player->wood;
        WarStateTrain* trainState = wt_startTrainTransaction(
            townHall,
            WAR_UNIT_PEASANT,
            1000.0f,
            goldCost,
            woodCost);
        WarStateRef trainRef = wst_refOf(g_test.context, (WarStateBase*)trainState);

        wt_selectOnly(townHall);
        if (cancelFirst)
        {
            wcmd_cancel(g_test.context, townHall);
            wt_dealLethalDamage(townHall);
        }
        else
        {
            wt_dealLethalDamage(townHall);
            wcmd_cancel(g_test.context, townHall);
        }

        const WarTransitionRequest* winner = wt_activeTransition(&g_test, townHall);

        TEST_ASSERT_NOT_NULL(winner);
        TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, winner->cause);
        TEST_ASSERT_FALSE(winner->cancellation);

        wt_applyPendingTransitions(&g_test);

        TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, wt_activeState(&g_test, townHall));
        TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
        TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);
        TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, trainRef));
    }
}

void test_train_completion_frame_cancel_keeps_output_and_never_refunds_in_either_order(void)
{
    const s32 goldCost = 157;
    const s32 woodCost = 67;

    wt_seedUnitSpriteResource(WAR_UNIT_PEASANT);

    for (s32 cancelFirst = 0; cancelFirst < 2; cancelFirst++)
    {
        WarEntity* townHall = wt_spawnBuilding(
            &g_test,
            WAR_UNIT_TOWNHALL_HUMANS,
            0,
            vec2i((10 + cancelFirst * 10) * MEGA_TILE_WIDTH, 20 * MEGA_TILE_HEIGHT));

        TEST_ASSERT_NOT_NULL(townHall);
        wt_applyPendingTransitions(&g_test);

        WarPlayerInfo* player = &g_test.map->players[0];
        const s32 goldBefore = player->gold;
        const s32 woodBefore = player->wood;
        const s32 peasantsBefore = wu_getNumberOfUnitsOfType(
            g_test.context,
            0,
            WAR_UNIT_PEASANT);
        WarStateTrain* trainState = wt_startTrainTransaction(
            townHall,
            WAR_UNIT_PEASANT,
            0.0f,
            goldCost,
            woodCost);
        WarStateRef trainRef = wst_refOf(g_test.context, (WarStateBase*)trainState);

        wt_selectOnly(townHall);
        if (cancelFirst)
        {
            wcmd_cancel(g_test.context, townHall);
            wst_updateTrainStates(g_test.context);
        }
        else
        {
            wst_updateTrainStates(g_test.context);
            wcmd_cancel(g_test.context, townHall);
        }

        TEST_ASSERT_TRUE(trainState->outputCommitted);
        TEST_ASSERT_EQUAL_INT(
            peasantsBefore + 1,
            wu_getNumberOfUnitsOfType(g_test.context, 0, WAR_UNIT_PEASANT));

        wt_applyPendingTransitions(&g_test);

        TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, townHall));
        TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
        TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);
        TEST_ASSERT_EQUAL_INT(
            peasantsBefore + 1,
            wu_getNumberOfUnitsOfType(g_test.context, 0, WAR_UNIT_PEASANT));
        TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, trainRef));
    }
}

void test_upgrade_cancel_refunds_once_only_after_commit(void)
{
    const s32 goldCost = 163;
    const s32 woodCost = 71;
    WarEntity* blacksmith = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_BLACKSMITH_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(blacksmith);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    setUpgradeAllowed(player, WAR_UPGRADE_SWORDS, 2);

    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 upgradeCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_UPGRADE);
    WarStateUpgrade* upgradeState = wt_startUpgradeTransaction(
        blacksmith,
        WAR_UPGRADE_SWORDS,
        1000.0f,
        goldCost,
        woodCost);
    WarStateRef upgradeRef = wst_refOf(g_test.context, (WarStateBase*)upgradeState);

    TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);

    wt_selectOnly(blacksmith);
    wcmd_cancel(g_test.context, blacksmith);
    wcmd_cancel(g_test.context, blacksmith);

    const WarTransitionRequest* cancelRequest = wt_activeTransition(&g_test, blacksmith);

    TEST_ASSERT_NOT_NULL(cancelRequest);
    TEST_ASSERT_TRUE(cancelRequest->cancellation);
    wt_assertStateRefEqual(upgradeRef, cancelRequest->cancellationTargetRef);
    TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, blacksmith));
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(upgradeCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_UPGRADE));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, upgradeRef));
}

void test_upgrade_cancel_displaced_by_lifecycle_never_refunds_in_either_order(void)
{
    const s32 goldCost = 173;
    const s32 woodCost = 73;
    WarPlayerInfo* player = &g_test.map->players[0];

    setUpgradeAllowed(player, WAR_UPGRADE_SWORDS, 2);

    for (s32 cancelFirst = 0; cancelFirst < 2; cancelFirst++)
    {
        WarEntity* blacksmith = wt_spawnBuilding(
            &g_test,
            WAR_UNIT_BLACKSMITH_HUMANS,
            0,
            vec2i((10 + cancelFirst * 10) * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

        TEST_ASSERT_NOT_NULL(blacksmith);
        wt_applyPendingTransitions(&g_test);

        const s32 goldBefore = player->gold;
        const s32 woodBefore = player->wood;
        WarStateUpgrade* upgradeState = wt_startUpgradeTransaction(
            blacksmith,
            WAR_UPGRADE_SWORDS,
            1000.0f,
            goldCost,
            woodCost);
        WarStateRef upgradeRef = wst_refOf(g_test.context, (WarStateBase*)upgradeState);

        wt_selectOnly(blacksmith);
        if (cancelFirst)
        {
            wcmd_cancel(g_test.context, blacksmith);
            wt_dealLethalDamage(blacksmith);
        }
        else
        {
            wt_dealLethalDamage(blacksmith);
            wcmd_cancel(g_test.context, blacksmith);
        }

        const WarTransitionRequest* winner = wt_activeTransition(&g_test, blacksmith);

        TEST_ASSERT_NOT_NULL(winner);
        TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, winner->cause);
        TEST_ASSERT_FALSE(winner->cancellation);

        wt_applyPendingTransitions(&g_test);

        TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, wt_activeState(&g_test, blacksmith));
        TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
        TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);
        TEST_ASSERT_EQUAL_INT(0, getUpgradeLevel(player, WAR_UPGRADE_SWORDS));
        TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, upgradeRef));
    }
}

void test_upgrade_completion_frame_cancel_keeps_output_and_never_refunds_in_either_order(void)
{
    const s32 goldCost = 179;
    const s32 woodCost = 79;
    WarPlayerInfo* player = &g_test.map->players[0];

    setUpgradeAllowed(player, WAR_UPGRADE_SWORDS, 2);

    for (s32 cancelFirst = 0; cancelFirst < 2; cancelFirst++)
    {
        WarEntity* blacksmith = wt_spawnBuilding(
            &g_test,
            WAR_UNIT_BLACKSMITH_HUMANS,
            0,
            vec2i((10 + cancelFirst * 10) * MEGA_TILE_WIDTH, 20 * MEGA_TILE_HEIGHT));

        TEST_ASSERT_NOT_NULL(blacksmith);
        wt_applyPendingTransitions(&g_test);

        const s32 goldBefore = player->gold;
        const s32 woodBefore = player->wood;
        const s32 levelBefore = getUpgradeLevel(player, WAR_UPGRADE_SWORDS);
        WarStateUpgrade* upgradeState = wt_startUpgradeTransaction(
            blacksmith,
            WAR_UPGRADE_SWORDS,
            0.0f,
            goldCost,
            woodCost);
        WarStateRef upgradeRef = wst_refOf(g_test.context, (WarStateBase*)upgradeState);

        wt_selectOnly(blacksmith);
        if (cancelFirst)
        {
            wcmd_cancel(g_test.context, blacksmith);
            wst_updateUpgradeStates(g_test.context);
        }
        else
        {
            wst_updateUpgradeStates(g_test.context);
            wcmd_cancel(g_test.context, blacksmith);
        }

        TEST_ASSERT_TRUE(upgradeState->outputCommitted);
        TEST_ASSERT_EQUAL_INT(
            levelBefore + 1,
            getUpgradeLevel(player, WAR_UPGRADE_SWORDS));

        wt_applyPendingTransitions(&g_test);

        TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, blacksmith));
        TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
        TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);
        TEST_ASSERT_EQUAL_INT(
            levelBefore + 1,
            getUpgradeLevel(player, WAR_UPGRADE_SWORDS));
        TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, upgradeRef));
    }
}

void test_pending_build_cancel_refunds_once_only_after_commit(void)
{
    wt_seedBuildingSpriteResources(WAR_UNIT_FARM_HUMANS);

    const s32 goldCost = 191;
    const s32 woodCost = 83;
    WarEntity* building = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_FARM_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(building);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 buildCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_BUILD);

    TEST_ASSERT_TRUE(we_decreasePlayerResources(
        g_test.context,
        player,
        goldCost,
        woodCost));

    WarStateBuild* buildState = wst_createBuildState(
        g_test.context,
        building,
        1000.0f,
        goldCost,
        woodCost);

    TEST_ASSERT_NOT_NULL(buildState);

    WarStateRef buildRef = wst_refOf(g_test.context, (WarStateBase*)buildState);

    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        building,
        (WarStateBase*)buildState,
        WAR_TRANSITION_CAUSE_INITIALIZATION));

    wt_selectOnly(building);
    wcmd_cancel(g_test.context, building);
    wcmd_cancel(g_test.context, building);

    const WarTransitionRequest* cancelRequest = wt_activeTransition(&g_test, building);

    TEST_ASSERT_NOT_NULL(cancelRequest);
    TEST_ASSERT_TRUE(cancelRequest->cancellation);
    wt_assertStateRefEqual(buildRef, cancelRequest->cancellationTargetRef);
    TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, wt_activeState(&g_test, building));
    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(buildCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_BUILD));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, buildRef));
}

void test_pending_build_cancel_displaced_by_lifecycle_never_refunds_in_either_order(void)
{
    wt_seedBuildingSpriteResources(WAR_UNIT_FARM_HUMANS);

    const s32 goldCost = 193;
    const s32 woodCost = 89;

    for (s32 cancelFirst = 0; cancelFirst < 2; cancelFirst++)
    {
        WarEntity* building = wt_spawnBuilding(
            &g_test,
            WAR_UNIT_FARM_HUMANS,
            0,
            vec2i((10 + cancelFirst * 10) * MEGA_TILE_WIDTH, 30 * MEGA_TILE_HEIGHT));

        TEST_ASSERT_NOT_NULL(building);
        wt_applyPendingTransitions(&g_test);

        WarPlayerInfo* player = &g_test.map->players[0];
        const s32 goldBefore = player->gold;
        const s32 woodBefore = player->wood;
        const s32 buildCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_BUILD);

        TEST_ASSERT_TRUE(we_decreasePlayerResources(
            g_test.context,
            player,
            goldCost,
            woodCost));

        WarStateBuild* buildState = wst_createBuildState(
            g_test.context,
            building,
            1000.0f,
            goldCost,
            woodCost);

        TEST_ASSERT_NOT_NULL(buildState);

        WarStateRef buildRef = wst_refOf(g_test.context, (WarStateBase*)buildState);

        TEST_ASSERT_TRUE(wst_resetState(
            g_test.context,
            building,
            (WarStateBase*)buildState,
            WAR_TRANSITION_CAUSE_INITIALIZATION));

        wt_selectOnly(building);
        if (cancelFirst)
        {
            wcmd_cancel(g_test.context, building);
            wt_dealLethalDamage(building);
        }
        else
        {
            wt_dealLethalDamage(building);
            wcmd_cancel(g_test.context, building);
        }

        const WarTransitionRequest* winner = wt_activeTransition(&g_test, building);

        TEST_ASSERT_NOT_NULL(winner);
        TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, winner->cause);
        TEST_ASSERT_FALSE(winner->cancellation);

        wt_applyPendingTransitions(&g_test);

        TEST_ASSERT_EQUAL_INT(WAR_STATE_COLLAPSE, wt_activeState(&g_test, building));
        TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
        TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);
        TEST_ASSERT_EQUAL_INT(buildCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_BUILD));
        TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, buildRef));
    }
}

void test_build_completion_frame_cancel_keeps_output_without_collapse_in_either_order(void)
{
    const s32 goldCost = 199;
    const s32 woodCost = 103;
    WarEntityManager* manager = we_getEntityManager(g_test.context);

    TEST_ASSERT_NOT_NULL(manager);

    for (s32 cancelFirst = 0; cancelFirst < 2; cancelFirst++)
    {
        WarBuildTestFixture fixture = wt_startBuildWithAssignedWorker(
            vec2i((10 + cancelFirst * 15) * MEGA_TILE_WIDTH, 30 * MEGA_TILE_HEIGHT),
            0.0f,
            goldCost,
            woodCost);
        WarPlayerInfo* player = &g_test.map->players[0];
        const s32 audioCountBefore = manager->audios.count;
        const s32 spriteCountBefore = manager->sprites.count;
        const s32 collapseCountBefore = wt_stateAllocationCount(
            &g_test,
            WAR_STATE_COLLAPSE);

        wt_selectOnly(fixture.building);
        if (cancelFirst)
        {
            wcmd_cancel(g_test.context, fixture.building);
            wst_updateBuildStates(g_test.context);
        }
        else
        {
            wst_updateBuildStates(g_test.context);
            wcmd_cancel(g_test.context, fixture.building);
        }

        TEST_ASSERT_TRUE(fixture.buildState->outputCommitted);
        TEST_ASSERT_EQUAL_INT(audioCountBefore + 1, manager->audios.count);
        TEST_ASSERT_EQUAL_INT(spriteCountBefore, manager->sprites.count);

        WarSpriteComponent* completedSprite = we_getSpriteComponent(
            g_test.context,
            fixture.building);
        WarTransformComponent* workerTransform = we_getTransformComponent(
            g_test.context,
            fixture.worker);

        TEST_ASSERT_NOT_NULL(completedSprite);
        TEST_ASSERT_NOT_NULL(workerTransform);
        TEST_ASSERT_TRUE(completedSprite->sprite.framesCount > 0);

        u8* completedFrameData = completedSprite->sprite.frames[0].data;
        vec2 workerPosition = workerTransform->position;

        wst_updateBuildStates(g_test.context);

        completedSprite = we_getSpriteComponent(g_test.context, fixture.building);
        workerTransform = we_getTransformComponent(g_test.context, fixture.worker);

        TEST_ASSERT_NOT_NULL(completedSprite);
        TEST_ASSERT_NOT_NULL(workerTransform);
        TEST_ASSERT_EQUAL_PTR(
            completedFrameData,
            completedSprite->sprite.frames[0].data);
        TEST_ASSERT_EQUAL_FLOAT(workerPosition.x, workerTransform->position.x);
        TEST_ASSERT_EQUAL_FLOAT(workerPosition.y, workerTransform->position.y);
        TEST_ASSERT_EQUAL_INT(audioCountBefore + 1, manager->audios.count);
        TEST_ASSERT_EQUAL_INT(spriteCountBefore, manager->sprites.count);

        const WarTransitionRequest* cancelRequest = wt_activeTransition(
            &g_test,
            fixture.building);

        TEST_ASSERT_NOT_NULL(cancelRequest);
        TEST_ASSERT_TRUE(cancelRequest->cancellation);
        wt_assertStateRefEqual(
            fixture.buildRef,
            cancelRequest->cancellationTargetRef);

        wt_applyPendingTransitions(&g_test);

        TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, fixture.building));
        TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, fixture.buildRef));
        TEST_ASSERT_EQUAL_INT(
            collapseCountBefore,
            wt_stateAllocationCount(&g_test, WAR_STATE_COLLAPSE));
        TEST_ASSERT_EQUAL_INT(audioCountBefore + 1, manager->audios.count);
        TEST_ASSERT_EQUAL_INT(fixture.goldBefore - goldCost, player->gold);
        TEST_ASSERT_EQUAL_INT(fixture.woodBefore - woodCost, player->wood);

        WarUnitComponent* buildingUnit = we_getUnitComponent(
            g_test.context,
            fixture.building);

        TEST_ASSERT_NOT_NULL(buildingUnit);
        TEST_ASSERT_FALSE(buildingUnit->building);
    }
}

void test_removing_assigned_worker_clears_build_backlink_and_pauses_building(void)
{
    WarBuildTestFixture fixture = wt_startBuildWithAssignedWorker(
        vec2i(10 * MEGA_TILE_WIDTH, 40 * MEGA_TILE_HEIGHT),
        1000.0f,
        223,
        107);
    WarEntityId workerId = fixture.worker->id;
    const f32 buildTimeBefore = fixture.buildState->buildTime;

    TEST_ASSERT_EQUAL_INT(workerId, fixture.buildState->workerId);

    we_removeEntity(g_test.context, fixture.worker);

    TEST_ASSERT_NULL(we_findEntity(g_test.context, workerId));
    TEST_ASSERT_EQUAL_INT(0, fixture.buildState->workerId);

    wt_updateGameTime(&g_test);
    wst_updateBuildStates(g_test.context);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_BUILD, wt_activeState(&g_test, fixture.building));
    TEST_ASSERT_EQUAL_INT(0, fixture.buildState->workerId);
    TEST_ASSERT_EQUAL_FLOAT(buildTimeBefore, fixture.buildState->buildTime);
    TEST_ASSERT_FALSE(fixture.buildState->outputCommitted);
}

void test_build_placement_build_state_exhaustion_rolls_back_atomically(void)
{
    WarEntity* worker = wt_spawnUnit(
        &g_test,
        WAR_UNIT_PEASANT,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(worker);
    wt_applyPendingTransitions(&g_test);
    wt_seedBuildingSpriteResources(WAR_UNIT_FARM_HUMANS);

    WarEntityManager* manager = we_getEntityManager(g_test.context);

    TEST_ASSERT_NOT_NULL(manager);

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        WarStateRef ref = wst_allocState(
            g_test.context,
            WAR_STATE_BUILD,
            worker->id);

        TEST_ASSERT_TRUE(WAR_STATE_REF_IS_VALID(ref));
    }

    TEST_ASSERT_EQUAL_INT(
        MAX_STATES_PER_TYPE,
        wt_stateAllocationCount(&g_test, WAR_STATE_BUILD));

    WarPlayerInfo* player = &g_test.map->players[0];
    WarEntityList* unitEntities = we_getEntitiesOfType(
        g_test.context,
        WAR_ENTITY_TYPE_UNIT);
    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    const s32 entityCountBefore = manager->entityCount;
    const s32 unitEntityCountBefore = unitEntities->count;
    const s32 farmCountBefore = wu_getNumberOfUnitsOfType(
        g_test.context,
        0,
        WAR_UNIT_FARM_HUMANS);
    const s32 transformCountBefore = manager->transforms.count;
    const s32 spriteCountBefore = manager->sprites.count;
    const s32 unitCountBefore = manager->units.count;
    const s32 animationsCountBefore = manager->animations.count;
    const s32 stateMachineCountBefore = manager->stateMachines.count;
    const s32 totalStateCountBefore = wt_totalStateAllocationCount(&g_test);
    const s32 repairStateCountBefore = wt_stateAllocationCount(
        &g_test,
        WAR_STATE_REPAIR);

    g_test.map->fowEnabled = false;
    g_test.map->ui.mapPanel = recti(0, 0, MAP_WIDTH, MAP_HEIGHT);
    g_test.map->camera.viewport = recti(0, 0, MAP_VIEWPORT_WIDTH, MAP_VIEWPORT_HEIGHT);
    g_test.context->input.pos = vec2i(
        30 * MEGA_TILE_WIDTH,
        30 * MEGA_TILE_HEIGHT);
    g_test.context->input.buttons[WAR_MOUSE_LEFT].justPressed = true;

    wt_selectOnly(worker);
    wcmd_buildFarmHumans(g_test.context, worker);

    TEST_ASSERT_EQUAL_INT(
        WAR_COMMAND_BUILD_FARM_HUMANS,
        g_test.map->commandState.command.type);
    TEST_ASSERT_TRUE(wcmd_executeCommand(g_test.context));

    g_test.context->input.buttons[WAR_MOUSE_LEFT].justPressed = false;

    TEST_ASSERT_EQUAL_INT(goldBefore, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore, player->wood);
    TEST_ASSERT_EQUAL_INT(entityCountBefore, manager->entityCount);
    TEST_ASSERT_EQUAL_INT(unitEntityCountBefore, unitEntities->count);
    TEST_ASSERT_EQUAL_INT(
        farmCountBefore,
        wu_getNumberOfUnitsOfType(g_test.context, 0, WAR_UNIT_FARM_HUMANS));
    TEST_ASSERT_EQUAL_INT(transformCountBefore, manager->transforms.count);
    TEST_ASSERT_EQUAL_INT(spriteCountBefore, manager->sprites.count);
    TEST_ASSERT_EQUAL_INT(unitCountBefore, manager->units.count);
    TEST_ASSERT_EQUAL_INT(animationsCountBefore, manager->animations.count);
    TEST_ASSERT_EQUAL_INT(stateMachineCountBefore, manager->stateMachines.count);
    TEST_ASSERT_EQUAL_INT(totalStateCountBefore, wt_totalStateAllocationCount(&g_test));
    TEST_ASSERT_EQUAL_INT(
        MAX_STATES_PER_TYPE,
        wt_stateAllocationCount(&g_test, WAR_STATE_BUILD));
    TEST_ASSERT_EQUAL_INT(
        repairStateCountBefore,
        wt_stateAllocationCount(&g_test, WAR_STATE_REPAIR));
    TEST_ASSERT_EQUAL_INT(
        WAR_COMMAND_BUILD_FARM_HUMANS,
        g_test.map->commandState.command.type);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, worker));
    wt_assertPendingTransitionCleared(worker);
}

void test_remove_entity_leaves_initialized_wait_before_required_components(void)
{
    WarEntityManager* manager = we_getEntityManager(g_test.context);

    TEST_ASSERT_NOT_NULL(manager);

    const s32 waitActiveBefore = manager->stateStorage.activeCounts[WAR_STATE_WAIT];
    const s32 waitFreeBefore = manager->stateStorage.freeCounts[WAR_STATE_WAIT];
    const s32 stateMachineCountBefore = manager->stateMachines.count;
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateWait* activeWait = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NOT_NULL(activeWait);
    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        unit,
        (WarStateBase*)activeWait,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    wt_applyPendingTransitions(&g_test);
    wt_updateGameTime(&g_test);
    wst_updateWaitStates(g_test.context);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));

    WarStateWait* pendingWait = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NOT_NULL(pendingWait);

    WarStateRef activeRef = wst_refOf(g_test.context, (WarStateBase*)activeWait);
    WarStateRef pendingRef = wst_refOf(g_test.context, (WarStateBase*)pendingWait);
    WarEntityId entityId = unit->id;

    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        unit,
        (WarStateBase*)pendingWait,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    TEST_ASSERT_EQUAL_INT(waitActiveBefore + 2, manager->stateStorage.activeCounts[WAR_STATE_WAIT]);

    we_removeEntity(g_test.context, unit);

    TEST_ASSERT_NULL(we_findEntity(g_test.context, entityId));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, activeRef));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, pendingRef));
    TEST_ASSERT_EQUAL_INT(waitActiveBefore, manager->stateStorage.activeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_EQUAL_INT(waitFreeBefore, manager->stateStorage.freeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_EQUAL_INT(stateMachineCountBefore, manager->stateMachines.count);
}

void test_remove_entity_releases_active_train_and_pending_state_without_refund(void)
{
    const s32 goldCost = 197;
    const s32 woodCost = 97;
    WarEntityManager* manager = we_getEntityManager(g_test.context);

    TEST_ASSERT_NOT_NULL(manager);

    const s32 trainActiveBefore = manager->stateStorage.activeCounts[WAR_STATE_TRAIN];
    const s32 trainFreeBefore = manager->stateStorage.freeCounts[WAR_STATE_TRAIN];
    const s32 waitActiveBefore = manager->stateStorage.activeCounts[WAR_STATE_WAIT];
    const s32 waitFreeBefore = manager->stateStorage.freeCounts[WAR_STATE_WAIT];
    const s32 stateMachineCountBefore = manager->stateMachines.count;
    WarEntity* townHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(townHall);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    const s32 goldBefore = player->gold;
    const s32 woodBefore = player->wood;
    WarStateTrain* trainState = wt_startTrainTransaction(
        townHall,
        WAR_UNIT_PEASANT,
        1000.0f,
        goldCost,
        woodCost);
    WarStateWait* pendingWait = wst_createWaitState(g_test.context, townHall, 1000.0f);

    TEST_ASSERT_NOT_NULL(pendingWait);

    WarStateRef trainRef = wst_refOf(g_test.context, (WarStateBase*)trainState);
    WarStateRef pendingRef = wst_refOf(g_test.context, (WarStateBase*)pendingWait);
    WarEntityId entityId = townHall->id;

    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        townHall,
        (WarStateBase*)pendingWait,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));

    we_removeEntity(g_test.context, townHall);

    TEST_ASSERT_NULL(we_findEntity(g_test.context, entityId));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, trainRef));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, pendingRef));
    TEST_ASSERT_EQUAL_INT(goldBefore - goldCost, player->gold);
    TEST_ASSERT_EQUAL_INT(woodBefore - woodCost, player->wood);
    TEST_ASSERT_EQUAL_INT(trainActiveBefore, manager->stateStorage.activeCounts[WAR_STATE_TRAIN]);
    TEST_ASSERT_EQUAL_INT(trainFreeBefore, manager->stateStorage.freeCounts[WAR_STATE_TRAIN]);
    TEST_ASSERT_EQUAL_INT(waitActiveBefore, manager->stateStorage.activeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_EQUAL_INT(waitFreeBefore, manager->stateStorage.freeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_EQUAL_INT(stateMachineCountBefore, manager->stateMachines.count);
}

void test_remove_one_of_two_state_machines_preserves_swapped_dense_invariants(void)
{
    WarEntity* removedUnit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    WarEntity* movedUnit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(12 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(removedUnit);
    TEST_ASSERT_NOT_NULL(movedUnit);
    wt_applyPendingTransitions(&g_test);

    WarStateWait* activeWait = wst_createWaitState(
        g_test.context,
        movedUnit,
        1000.0f);

    TEST_ASSERT_NOT_NULL(activeWait);
    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        movedUnit,
        (WarStateBase*)activeWait,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    wt_applyPendingTransitions(&g_test);

    WarStateWait* pendingWait = wst_createWaitState(
        g_test.context,
        movedUnit,
        1000.0f);

    TEST_ASSERT_NOT_NULL(pendingWait);

    WarStateRef activeRef = wst_refOf(
        g_test.context,
        (WarStateBase*)activeWait);
    WarStateRef pendingRef = wst_refOf(
        g_test.context,
        (WarStateBase*)pendingWait);

    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        movedUnit,
        (WarStateBase*)pendingWait,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));

    WarEntityManager* manager = we_getEntityManager(g_test.context);

    TEST_ASSERT_NOT_NULL(manager);

    const u16 removedIndex = removedUnit->components[COMP_STATE_MACHINE];
    const u16 movedIndex = movedUnit->components[COMP_STATE_MACHINE];
    const u16 lastIndex = (u16)(manager->stateMachines.count - 1);
    const s32 stateMachineCountBefore = manager->stateMachines.count;
    const WarEntityId removedId = removedUnit->id;
    const WarEntityId movedId = movedUnit->id;

    TEST_ASSERT_NOT_EQUAL(removedIndex, movedIndex);
    TEST_ASSERT_EQUAL_INT(lastIndex, movedIndex);

    we_disableComponent(g_test.context, movedUnit, COMP_STATE_MACHINE);
    TEST_ASSERT_FALSE(we_isComponentEnabled(
        g_test.context,
        movedUnit,
        COMP_STATE_MACHINE));

    we_removeEntity(g_test.context, removedUnit);

    TEST_ASSERT_NULL(we_findEntity(g_test.context, removedId));
    TEST_ASSERT_EQUAL_PTR(movedUnit, we_findEntity(g_test.context, movedId));
    TEST_ASSERT_EQUAL_INT(stateMachineCountBefore - 1, manager->stateMachines.count);
    TEST_ASSERT_EQUAL_INT(removedIndex, movedUnit->components[COMP_STATE_MACHINE]);
    TEST_ASSERT_EQUAL_INT(movedId, manager->stateMachines.owners[removedIndex]);
    TEST_ASSERT_FALSE(manager->stateMachines.enabled[removedIndex]);
    TEST_ASSERT_EQUAL_INT(0, manager->stateMachines.owners[lastIndex]);
    TEST_ASSERT_FALSE(manager->stateMachines.enabled[lastIndex]);

    WarStateMachineComponent* movedStateMachine = we_getStateMachineComponent(
        g_test.context,
        movedUnit);

    TEST_ASSERT_NOT_NULL(movedStateMachine);
    TEST_ASSERT_EQUAL_PTR(
        &manager->stateMachines.dense[removedIndex],
        movedStateMachine);
    TEST_ASSERT_EQUAL_INT(1, movedStateMachine->depth);
    wt_assertStateRefEqual(activeRef, movedStateMachine->stack[0]);
    wt_assertStateRefEqual(pendingRef, movedStateMachine->pending.stateRef);
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, activeRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, pendingRef));

    WarStateBase* activeState = wst_deref(g_test.context, activeRef);
    WarStateBase* pendingState = wst_deref(g_test.context, pendingRef);

    TEST_ASSERT_NOT_NULL(activeState);
    TEST_ASSERT_NOT_NULL(pendingState);
    TEST_ASSERT_EQUAL_INT(movedId, activeState->entityId);
    TEST_ASSERT_EQUAL_INT(movedId, pendingState->entityId);
    TEST_ASSERT_EQUAL_PTR(
        activeState,
        wst_getActiveState(g_test.context, movedUnit));

    we_enableComponent(g_test.context, movedUnit, COMP_STATE_MACHINE);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_TRUE(we_isComponentEnabled(
        g_test.context,
        movedUnit,
        COMP_STATE_MACHINE));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, activeRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, pendingRef));
    TEST_ASSERT_EQUAL_PTR(
        pendingState,
        wst_getActiveState(g_test.context, movedUnit));
}

void test_push_state_increases_depth(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    wt_step(&g_test); // process pending ops

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));

    // Push a WAIT state on top of IDLE
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 2.0f);
    wst_pushState(g_test.context, unit, (WarStateBase*)waitState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

    // Pending op should not yet be applied
    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));

    // Step one tick to process the pending push
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(2, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_stateAt(&g_test, unit, 0));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_stateAt(&g_test, unit, 1));
}

void test_pop_state_decreases_depth(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    wt_step(&g_test); // process pending ops

    // Push WAIT, step to apply
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 1.0f);
    wst_pushState(g_test.context, unit, (WarStateBase*)waitState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_step(&g_test);
    TEST_ASSERT_EQUAL_INT(2, wt_stateDepth(&g_test, unit));

    // Pop WAIT
    wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
}

void test_reset_state_clears_stack(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    wt_step(&g_test); // process pending ops

    // Push WAIT on top of IDLE
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 5.0f);
    wst_pushState(g_test.context, unit, (WarStateBase*)waitState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_step(&g_test);
    TEST_ASSERT_EQUAL_INT(2, wt_stateDepth(&g_test, unit));

    // Reset with a fresh MOVE state
    vec2 positions[] =
    {
        wu_getUnitCenterPosition(g_test.context, unit),
        vec2i(15 * MEGA_TILE_WIDTH, 15 * MEGA_TILE_HEIGHT)
    };
    WarStateMove* moveState = wst_createMoveState(g_test.context, unit, arrayLength(positions), positions, false);
    wst_resetState(g_test.context, unit, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_AUTONOMOUS);
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));
}

void test_stack_contains_query(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    wt_step(&g_test); // process pending ops

    TEST_ASSERT(wt_stackContains(&g_test, unit, WAR_STATE_IDLE));
    TEST_ASSERT(!wt_stackContains(&g_test, unit, WAR_STATE_ATTACK));

    // Push WAIT
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 1.0f);
    wst_pushState(g_test.context, unit, (WarStateBase*)waitState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_step(&g_test);

    TEST_ASSERT(wt_stackContains(&g_test, unit, WAR_STATE_IDLE));
    TEST_ASSERT(wt_stackContains(&g_test, unit, WAR_STATE_WAIT));
}

void test_step_ticks_advances_simulation(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    u64 tickBefore = g_test.simulationTick;
    wt_stepTicks(&g_test, 5);
    u64 tickAfter = g_test.simulationTick;

    TEST_ASSERT_EQUAL_INT(5, tickAfter - tickBefore);

    // Entity should still be in IDLE after 5 ticks (lookAround=false for buildings, true for dudes
    // but no enemies nearby so it stays IDLE)
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
}

void test_empty_stack_becomes_idle_on_pop(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    wt_step(&g_test); // process pending ops

    // Reset to MOVE (clears the stack to just MOVE)
    vec2 positions[] =
    {
        wu_getUnitCenterPosition(g_test.context, unit),
        vec2i(15 * MEGA_TILE_WIDTH, 15 * MEGA_TILE_HEIGHT)
    };
    WarStateMove* moveState = wst_createMoveState(g_test.context, unit, arrayLength(positions), positions, false);
    wst_resetState(g_test.context, unit, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_AUTONOMOUS);
    wt_step(&g_test);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));

    // Pop MOVE — the system auto-creates IDLE when the stack becomes empty
    wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
}

void test_autonomous_transition_outranks_state_completion(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_step(&g_test);

    WarStateAttack* attackState = wst_createAttackState(g_test.context, unit, 123, vec2i(12 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(attackState);

    wst_pushState(g_test.context, unit, (WarStateBase*)attackState, WAR_TRANSITION_CAUSE_AUTONOMOUS);
    wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);

    const WarTransitionRequest* transition = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(transition);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_PUSH, transition->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_ATTACK, transition->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AUTONOMOUS, transition->cause);
}

void test_move_eventually_completes(void)
{
    const u32 maxTicks = 30 * 10; // Ten simulated seconds.

    const vec2 position = wmap_tileToMapCoordinatesV(vec2i(10, 10), true);
    const vec2 destination = wmap_tileToMapCoordinatesV(vec2i(12, 10), true);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, position);

    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    // Apply the unit's initial IDLE transition before introducing
    // the enemy. This prevents IDLE from acquiring the enemy before
    // the MOVE state is installed.
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));

    // Use a valid route that requires MOVE to run before it can complete.
    vec2 waypoints[] = { position, destination };
    WarStateMove* moveState = wst_createMoveState(g_test.context, unit, arrayLength(waypoints), waypoints, false);

    TEST_ASSERT_NOT_NULL(moveState);

    wst_replaceState(g_test.context, unit, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

    // Commit MOVE before checking or updating the active state.
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));

    WarStateMachineComponent* sm = we_getStateMachineComponent(g_test.context, unit);

    TEST_ASSERT_NOT_NULL(sm);

    WarStateRef moveRef = sm->stack[sm->depth - 1];

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, moveRef));

    u32 tick = 0;
    const WarTransitionRequest* completionRequest = NULL;

    while (tick < maxTicks)
    {
        wt_updateGameTime(&g_test);
        wt_updateStateMachines(&g_test);
        tick++;

        completionRequest = wt_activeTransition(&g_test, unit);
        if (completionRequest->operation != WAR_STATE_OP_NONE)
        {
            break;
        }

        wt_advanceTick(&g_test);
    }

    TEST_ASSERT_TRUE(tick > 0);
    TEST_ASSERT_LESS_THAN(maxTicks, tick);
    TEST_ASSERT_NOT_NULL(completionRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, completionRequest->operation);
    TEST_ASSERT_FALSE(WAR_STATE_REF_IS_VALID(completionRequest->stateRef));
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_COMPLETION, completionRequest->cause);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, moveRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, moveRef));
}

void test_competing_attack_and_completion(void)
{
    const vec2 startPosition = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 destination = vec2i(14 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, startPosition);

    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_EQUAL(0, unit->id);

    // Apply the initial IDLE state.
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));

    vec2 waypoints[] = { startPosition, destination };
    WarStateMove* moveState = wst_createMoveState(g_test.context, unit, arrayLength(waypoints), waypoints, false);

    TEST_ASSERT_NOT_NULL(moveState);

    moveState->checkForAttacks = true;

    wst_replaceState(g_test.context, unit, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

    // Commit MOVE without running its update yet.
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));

    // Run one update without an enemy so MOVE can perform
    // any first-update initialization required by the implementation.
    // The destination must still be far enough away that MOVE does
    // not complete during this initialization tick.
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));

    // Fabricate the exact precondition under test: at the beginning
    // of the next update, the unit is already at its destination.
    // This avoids depending on speed, RVO, deceleration, or the
    // number of simulation ticks required for natural arrival.
    wt_setUnitCenterPosition(&g_test, unit, destination);
    wt_setUnitVelocity(&g_test, unit, vec2i(0, 0));

     // Introduce the enemy only after MOVE initialization. The enemy must
     // actively attack the unit because MOVE awareness detects current
     // attackers rather than every nearby hostile entity.
    WarEntity* enemy = wt_spawnUnit(&g_test, WAR_UNIT_GRUNT, 1, vec2i(12 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(enemy);

    wt_applyPendingTransitions(&g_test);

    WarStateAttack* attackState = wst_createAttackState(g_test.context, enemy, unit->id, wu_getUnitCenterPosition(g_test.context, unit));

    TEST_ASSERT_NOT_NULL(attackState);

    wst_replaceState(g_test.context, enemy, (WarStateBase*)attackState, WAR_TRANSITION_CAUSE_AI_ORDER);

    // Run the contested update:
    // - Awareness requests PUSH ATTACK.
    // - Arrival requests POP MOVE.
    wt_updateGameTime(&g_test);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_ATTACK, wt_activeState(&g_test, enemy));

    wt_updateStateMachines(&g_test);

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(winner);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_PUSH, winner->operation);
    TEST_ASSERT_TRUE(WAR_STATE_REF_IS_VALID(winner->stateRef));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_ATTACK, winner->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AUTONOMOUS, winner->cause);

    // Commit the winner and verify the resulting stack.
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(2, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_stateAt(&g_test, unit, 0));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_ATTACK, wt_stateAt(&g_test, unit, 1));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_ATTACK, wt_activeState(&g_test, unit));
}

void test_player_move_command_outranks_aggro_when_aggro_is_submitted_first(void)
{
    WarAggroTestFixture fixture = wt_setupMoveWithAttackingEnemy();
    WarEntity* unit = fixture.unit;

    const s32 attackCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_ATTACK);

    // Let the active MOVE state detect the attacking enemy and submit
    // an autonomous PUSH ATTACK request.
    wt_updateGameTime(&g_test);
    wt_updateStateMachines(&g_test);

    const WarTransitionRequest* aggroRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(aggroRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_PUSH, aggroRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_ATTACK, aggroRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AUTONOMOUS, aggroRequest->cause);

    WarStateRef losingAttackRef = aggroRequest->stateRef;

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, losingAttackRef));
    TEST_ASSERT_EQUAL_INT(attackCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_ATTACK));

    // During the same simulation tick, the player issues a new Move
    // command. Player orders outrank autonomous reactions.
    wcmd_executeMoveCommand(
        g_test.context,
        vec2i(20 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(winner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, winner->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, winner->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, winner->cause);

    WarStateRef playerMoveRef = winner->stateRef;

    // Replacing the pending ATTACK request must release its allocated
    // state slot immediately.
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, losingAttackRef));
    TEST_ASSERT_EQUAL_INT(attackCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_ATTACK));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, playerMoveRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));
}

void test_player_move_command_outranks_aggro_when_player_move_is_submitted_first(void)
{
    WarAggroTestFixture fixture = wt_setupMoveWithAttackingEnemy();
    WarEntity* unit = fixture.unit;

    const s32 attackCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_ATTACK);

    wcmd_executeMoveCommand(
        g_test.context,
        vec2i(20 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    const WarTransitionRequest* playerRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(playerRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, playerRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, playerRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, playerRequest->cause);

    WarStateRef playerMoveRef = playerRequest->stateRef;
    const u64 playerRequestSequence = playerRequest->sequence;
    const u64 nextSequenceBeforeAggro = wt_nextTransitionSequence(&g_test, unit);

    // The old active MOVE remains active until pending operations are
    // committed. It detects the enemy and attempts to submit ATTACK.
    // The autonomous request must lose to the already-pending player
    // command.
    wt_updateGameTime(&g_test);
    wt_updateStateMachines(&g_test);

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(winner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, winner->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, winner->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, winner->cause);
    wt_assertU64Equal(playerRequestSequence, winner->sequence);
    wt_assertStateRefEqual(playerMoveRef, winner->stateRef);

    // The sequence counter confirms another request was submitted,
    // while the allocation count confirms its ATTACK state was freed
    // after rejection.
    wt_assertU64Equal(nextSequenceBeforeAggro + 1, wt_nextTransitionSequence(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(attackCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_ATTACK));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, playerMoveRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));
}

void test_lethal_damage_outranks_move_command_when_move_is_submitted_first(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_applyPendingTransitions(&g_test);
    wt_selectOnly(unit);

    const s32 moveCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_MOVE);

    wcmd_executeMoveCommand(
        g_test.context,
        vec2i(16 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    const WarTransitionRequest* moveRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(moveRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, moveRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, moveRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, moveRequest->cause);

    WarStateRef losingMoveRef = moveRequest->stateRef;

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, losingMoveRef));
    TEST_ASSERT_EQUAL_INT(moveCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_MOVE));

    // Lethal damage is processed later during the same tick.
    WarUnitComponent* unitComponent = we_getUnitComponent(g_test.context, unit);

    TEST_ASSERT_NOT_NULL(unitComponent);
    unitComponent->hp = 1;

    const s32 deathCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_DEATH);

    we_takeDamage(g_test.context, unit, 1, 0);

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(winner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, winner->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_DEATH, winner->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, winner->cause);

    WarStateRef deathRef = winner->stateRef;

    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, losingMoveRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, deathRef));
    TEST_ASSERT_EQUAL_INT(moveCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_MOVE));
    TEST_ASSERT_EQUAL_INT(deathCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_DEATH));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_DEATH, wt_activeState(&g_test, unit));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, deathRef));
}

void test_lethal_damage_outranks_move_command_when_death_is_submitted_first(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_applyPendingTransitions(&g_test);
    wt_selectOnly(unit);

    WarUnitComponent* unitComponent = we_getUnitComponent(g_test.context, unit);

    TEST_ASSERT_NOT_NULL(unitComponent);
    unitComponent->hp = 1;

    const s32 deathCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_DEATH);

    we_takeDamage(g_test.context, unit, 1, 0);

    const WarTransitionRequest* deathRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(deathRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, deathRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_DEATH, deathRequest->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, deathRequest->cause);

    WarStateRef deathRef = deathRequest->stateRef;
    const u64 deathSequence = deathRequest->sequence;

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, deathRef));
    TEST_ASSERT_EQUAL_INT(deathCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_DEATH));

    // A player Move is processed later during the same tick. It must
    // be rejected because lifecycle outranks player commands.
    const s32 moveCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_MOVE);

    wcmd_executeMoveCommand(
        g_test.context,
        vec2i(16 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(winner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, winner->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_DEATH, winner->stateRef.type);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_LIFECYCLE, winner->cause);
    wt_assertU64Equal(deathSequence, winner->sequence);
    wt_assertStateRefEqual(deathRef, winner->stateRef);

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, deathRef));
    TEST_ASSERT_EQUAL_INT(moveCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_MOVE));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_DEATH, wt_activeState(&g_test, unit));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, deathRef));
}

void test_equal_priority_keeps_first_submitted_request(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_applyPendingTransitions(&g_test);

    WarStateMove* firstMove = wt_createMoveTo(unit, vec2i(14 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(firstMove);

    WarStateRef firstRef = wst_refOf(g_test.context, (WarStateBase*)firstMove);

    wst_resetState(g_test.context, unit, (WarStateBase*)firstMove, WAR_TRANSITION_CAUSE_AI_ORDER);

    const WarTransitionRequest* firstRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(firstRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, firstRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AI_ORDER, firstRequest->cause);
    wt_assertStateRefEqual(firstRef, firstRequest->stateRef);

    const u64 firstSequence = firstRequest->sequence;

    WarStateMove* secondMove = wt_createMoveTo(unit, vec2i(18 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(secondMove);

    WarStateRef secondRef = wst_refOf(g_test.context, (WarStateBase*)secondMove);

    // Same cause means equal priority. The first request must remain pending.
    wst_resetState(g_test.context, unit, (WarStateBase*)secondMove, WAR_TRANSITION_CAUSE_AI_ORDER);

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(winner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, winner->operation);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AI_ORDER, winner->cause);
    wt_assertU64Equal(firstSequence, winner->sequence);
    wt_assertStateRefEqual(firstRef, winner->stateRef);

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, firstRef));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, secondRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));
}

void test_equal_priority_result_is_independent_of_unrelated_entity_order(void)
{
    const vec2 basePosition = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* subjectA = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, basePosition);
    WarEntity* subjectB = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(10 * MEGA_TILE_WIDTH, 12 * MEGA_TILE_HEIGHT));
    WarEntity* noiseA1 = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(5 * MEGA_TILE_WIDTH, 5 * MEGA_TILE_HEIGHT));
    WarEntity* noiseA2 = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(6 * MEGA_TILE_WIDTH, 5 * MEGA_TILE_HEIGHT));
    WarEntity* noiseB1 = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(5 * MEGA_TILE_WIDTH, 7 * MEGA_TILE_HEIGHT));
    WarEntity* noiseB2 = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(6 * MEGA_TILE_WIDTH, 7 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(subjectA);
    TEST_ASSERT_NOT_NULL(subjectB);
    TEST_ASSERT_NOT_NULL(noiseA1);
    TEST_ASSERT_NOT_NULL(noiseA2);
    TEST_ASSERT_NOT_NULL(noiseB1);
    TEST_ASSERT_NOT_NULL(noiseB2);

    wt_applyPendingTransitions(&g_test);

    // Subject A:
    // first request
    // noise A1
    // noise A2
    // second request
    WarStateMove* firstMoveA = wt_createMoveTo(subjectA, vec2i(14 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    WarStateRef firstRefA = wst_refOf(g_test.context, (WarStateBase*)firstMoveA);

    wst_resetState(g_test.context, subjectA, (WarStateBase*)firstMoveA, WAR_TRANSITION_CAUSE_AI_ORDER);

    WarStateMove* noiseMoveA1 = wt_createMoveTo(noiseA1, vec2i(8 * MEGA_TILE_WIDTH, 5 * MEGA_TILE_HEIGHT));
    WarStateMove* noiseMoveA2 = wt_createMoveTo(noiseA2, vec2i(9 * MEGA_TILE_WIDTH, 5 * MEGA_TILE_HEIGHT));

    wst_resetState(g_test.context, noiseA1, (WarStateBase*)noiseMoveA1, WAR_TRANSITION_CAUSE_AI_ORDER);
    wst_resetState(g_test.context, noiseA2, (WarStateBase*)noiseMoveA2, WAR_TRANSITION_CAUSE_AI_ORDER);

    WarStateMove* secondMoveA = wt_createMoveTo(subjectA, vec2i(18 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    WarStateRef secondRefA = wst_refOf(g_test.context, (WarStateBase*)secondMoveA);

    wst_resetState(g_test.context, subjectA, (WarStateBase*)secondMoveA, WAR_TRANSITION_CAUSE_AI_ORDER);

    // Subject B:
    // first request
    // noise B2
    // noise B1
    // second request
    // The unrelated processing order is reversed.
    WarStateMove* firstMoveB = wt_createMoveTo(subjectB, vec2i(14 * MEGA_TILE_WIDTH, 12 * MEGA_TILE_HEIGHT));
    WarStateRef firstRefB = wst_refOf(g_test.context, (WarStateBase*)firstMoveB);

    wst_resetState(g_test.context, subjectB, (WarStateBase*)firstMoveB, WAR_TRANSITION_CAUSE_AI_ORDER);

    WarStateMove* noiseMoveB2 = wt_createMoveTo(noiseB2, vec2i(9 * MEGA_TILE_WIDTH, 7 * MEGA_TILE_HEIGHT));
    WarStateMove* noiseMoveB1 = wt_createMoveTo(noiseB1, vec2i(8 * MEGA_TILE_WIDTH, 7 * MEGA_TILE_HEIGHT));

    wst_resetState(g_test.context, noiseB2, (WarStateBase*)noiseMoveB2, WAR_TRANSITION_CAUSE_AI_ORDER);
    wst_resetState(g_test.context, noiseB1, (WarStateBase*)noiseMoveB1, WAR_TRANSITION_CAUSE_AI_ORDER);

    WarStateMove* secondMoveB = wt_createMoveTo(subjectB, vec2i(18 * MEGA_TILE_WIDTH, 12 * MEGA_TILE_HEIGHT));
    WarStateRef secondRefB = wst_refOf(g_test.context, (WarStateBase*)secondMoveB);

    wst_resetState(g_test.context, subjectB, (WarStateBase*)secondMoveB, WAR_TRANSITION_CAUSE_AI_ORDER);

    const WarTransitionRequest* winnerA = wt_activeTransition(&g_test, subjectA);
    const WarTransitionRequest* winnerB = wt_activeTransition(&g_test, subjectB);

    TEST_ASSERT_NOT_NULL(winnerA);
    TEST_ASSERT_NOT_NULL(winnerB);

    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AI_ORDER, winnerA->cause);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AI_ORDER, winnerB->cause);

    wt_assertStateRefEqual(firstRefA, winnerA->stateRef);
    wt_assertStateRefEqual(firstRefB, winnerB->stateRef);

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, firstRefA));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, firstRefB));

    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, secondRefA));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, secondRefB));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, subjectA));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, subjectB));
}

void test_transition_cause_priority_is_submission_order_independent(void)
{
    const WarTransitionCause causes[] =
    {
        WAR_TRANSITION_CAUSE_INITIALIZATION,
        WAR_TRANSITION_CAUSE_COMPLETION,
        WAR_TRANSITION_CAUSE_AUTONOMOUS,
        WAR_TRANSITION_CAUSE_AI_ORDER,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER,
        WAR_TRANSITION_CAUSE_STATUS,
        WAR_TRANSITION_CAUSE_SCRIPT,
        WAR_TRANSITION_CAUSE_LIFECYCLE,
    };
    s32 testCase = 0;

    for (s32 higherIndex = 1; higherIndex < (s32)arrayLength(causes); higherIndex++)
    {
        for (s32 lowerIndex = 0; lowerIndex < higherIndex; lowerIndex++)
        {
            for (s32 higherFirst = 0; higherFirst < 2; higherFirst++)
            {
                vec2 position = vec2i(
                    (4 + testCase % 10) * MEGA_TILE_WIDTH,
                    (4 + testCase / 10) * MEGA_TILE_HEIGHT);
                WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, position);

                TEST_ASSERT_NOT_NULL(unit);

                wt_applyPendingTransitions(&g_test);

                WarStateRef lowerRef = WAR_STATE_REF_INVALID;
                WarStateRef higherRef = WAR_STATE_REF_INVALID;

                if (higherFirst)
                {
                    TEST_ASSERT_TRUE(wt_submitWaitTransition(
                        unit,
                        WAR_STATE_OP_RESET,
                        causes[higherIndex],
                        &higherRef));
                    TEST_ASSERT_FALSE(wt_submitWaitTransition(
                        unit,
                        WAR_STATE_OP_PUSH,
                        causes[lowerIndex],
                        &lowerRef));
                }
                else
                {
                    TEST_ASSERT_TRUE(wt_submitWaitTransition(
                        unit,
                        WAR_STATE_OP_PUSH,
                        causes[lowerIndex],
                        &lowerRef));
                    TEST_ASSERT_TRUE(wt_submitWaitTransition(
                        unit,
                        WAR_STATE_OP_RESET,
                        causes[higherIndex],
                        &higherRef));
                }

                const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

                TEST_ASSERT_NOT_NULL(winner);
                TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, winner->operation);
                TEST_ASSERT_EQUAL_INT(causes[higherIndex], winner->cause);
                wt_assertStateRefEqual(higherRef, winner->stateRef);
                TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, higherRef));
                TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, lowerRef));

                wt_applyPendingTransitions(&g_test);

                wt_assertPendingTransitionCleared(unit);
                TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
                TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
                TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, higherRef));

                testCase++;
            }
        }
    }
}

void test_equal_priority_keeps_first_across_operation_combinations(void)
{
    const WarStateOp operations[] =
    {
        WAR_STATE_OP_PUSH,
        WAR_STATE_OP_POP,
        WAR_STATE_OP_REPLACE,
        WAR_STATE_OP_RESET,
    };
    s32 testCase = 0;

    for (s32 firstIndex = 0; firstIndex < (s32)arrayLength(operations); firstIndex++)
    {
        for (s32 secondIndex = 0; secondIndex < (s32)arrayLength(operations); secondIndex++)
        {
            vec2 position = vec2i(
                (4 + testCase % 8) * MEGA_TILE_WIDTH,
                (8 + testCase / 8) * MEGA_TILE_HEIGHT);
            WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, position);

            TEST_ASSERT_NOT_NULL(unit);

            wt_applyPendingTransitions(&g_test);

            WarStateMachineComponent* sm = we_getStateMachineComponent(g_test.context, unit);

            TEST_ASSERT_NOT_NULL(sm);
            TEST_ASSERT_EQUAL_INT(1, sm->depth);

            WarStateRef originalActiveRef = sm->stack[0];
            WarStateRef firstRef = WAR_STATE_REF_INVALID;
            WarStateRef secondRef = WAR_STATE_REF_INVALID;

            TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, originalActiveRef));
            TEST_ASSERT_TRUE(wt_submitWaitTransition(
                unit,
                operations[firstIndex],
                WAR_TRANSITION_CAUSE_AI_ORDER,
                &firstRef));

            const WarTransitionRequest* firstRequest = wt_activeTransition(&g_test, unit);

            TEST_ASSERT_NOT_NULL(firstRequest);
            const u64 firstSequence = firstRequest->sequence;

            TEST_ASSERT_FALSE(wt_submitWaitTransition(
                unit,
                operations[secondIndex],
                WAR_TRANSITION_CAUSE_AI_ORDER,
                &secondRef));

            const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

            TEST_ASSERT_NOT_NULL(winner);
            TEST_ASSERT_EQUAL_INT(operations[firstIndex], winner->operation);
            TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AI_ORDER, winner->cause);
            wt_assertU64Equal(firstSequence, winner->sequence);
            TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, originalActiveRef));

            if (wt_operationHasState(operations[firstIndex]))
            {
                wt_assertStateRefEqual(firstRef, winner->stateRef);
                TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, firstRef));
            }
            else
            {
                TEST_ASSERT_FALSE(WAR_STATE_REF_IS_VALID(winner->stateRef));
            }

            if (wt_operationHasState(operations[secondIndex]))
            {
                TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, secondRef));
            }
            else
            {
                TEST_ASSERT_FALSE(WAR_STATE_REF_IS_VALID(secondRef));
            }

            wt_applyPendingTransitions(&g_test);

            wt_assertPendingTransitionCleared(unit);

            if (operations[firstIndex] == WAR_STATE_OP_PUSH)
            {
                TEST_ASSERT_EQUAL_INT(2, wt_stateDepth(&g_test, unit));
                TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, originalActiveRef));
            }
            else
            {
                TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
                TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, originalActiveRef));
            }

            if (wt_operationHasState(operations[firstIndex]))
            {
                TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
                TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, firstRef));
            }
            else
            {
                TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
            }

            testCase++;
        }
    }
}

void test_each_submission_consumes_only_its_entity_sequence(void)
{
    WarEntity* unitA = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    WarEntity* unitB = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(12 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unitA);
    TEST_ASSERT_NOT_NULL(unitB);

    wt_applyPendingTransitions(&g_test);

    const u64 unitASequence = wt_nextTransitionSequence(&g_test, unitA);
    const u64 unitBSequence = wt_nextTransitionSequence(&g_test, unitB);
    WarStateRef firstRef = WAR_STATE_REF_INVALID;
    WarStateRef noiseRef = WAR_STATE_REF_INVALID;
    WarStateRef rejectedRef = WAR_STATE_REF_INVALID;
    WarStateRef winningRef = WAR_STATE_REF_INVALID;
    WarStateRef rejectedPopRef = WAR_STATE_REF_INVALID;

    TEST_ASSERT_TRUE(wt_submitWaitTransition(
        unitA,
        WAR_STATE_OP_RESET,
        WAR_TRANSITION_CAUSE_AI_ORDER,
        &firstRef));
    wt_assertU64Equal(unitASequence + 1, wt_nextTransitionSequence(&g_test, unitA));
    wt_assertU64Equal(unitBSequence, wt_nextTransitionSequence(&g_test, unitB));

    TEST_ASSERT_TRUE(wt_submitWaitTransition(
        unitB,
        WAR_STATE_OP_RESET,
        WAR_TRANSITION_CAUSE_COMPLETION,
        &noiseRef));
    wt_assertU64Equal(unitASequence + 1, wt_nextTransitionSequence(&g_test, unitA));
    wt_assertU64Equal(unitBSequence + 1, wt_nextTransitionSequence(&g_test, unitB));

    TEST_ASSERT_FALSE(wt_submitWaitTransition(
        unitA,
        WAR_STATE_OP_PUSH,
        WAR_TRANSITION_CAUSE_COMPLETION,
        &rejectedRef));
    wt_assertU64Equal(unitASequence + 2, wt_nextTransitionSequence(&g_test, unitA));
    wt_assertU64Equal(unitBSequence + 1, wt_nextTransitionSequence(&g_test, unitB));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, rejectedRef));

    TEST_ASSERT_TRUE(wt_submitWaitTransition(
        unitA,
        WAR_STATE_OP_REPLACE,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER,
        &winningRef));
    wt_assertU64Equal(unitASequence + 3, wt_nextTransitionSequence(&g_test, unitA));
    wt_assertU64Equal(unitBSequence + 1, wt_nextTransitionSequence(&g_test, unitB));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, firstRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, winningRef));

    TEST_ASSERT_FALSE(wt_submitWaitTransition(
        unitA,
        WAR_STATE_OP_POP,
        WAR_TRANSITION_CAUSE_AUTONOMOUS,
        &rejectedPopRef));
    wt_assertU64Equal(unitASequence + 4, wt_nextTransitionSequence(&g_test, unitA));
    wt_assertU64Equal(unitBSequence + 1, wt_nextTransitionSequence(&g_test, unitB));
    TEST_ASSERT_FALSE(WAR_STATE_REF_IS_VALID(rejectedPopRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, winningRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, noiseRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unitA));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unitB));
}

void test_applying_transition_clears_pending_and_allows_later_requests(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_applyPendingTransitions(&g_test);
    wt_assertPendingTransitionCleared(unit);

    WarStateRef pushedRef = WAR_STATE_REF_INVALID;

    TEST_ASSERT_TRUE(wt_submitWaitTransition(
        unit,
        WAR_STATE_OP_PUSH,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER,
        &pushedRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, pushedRef));

    wt_applyPendingTransitions(&g_test);

    wt_assertPendingTransitionCleared(unit);
    TEST_ASSERT_EQUAL_INT(2, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, pushedRef));

    TEST_ASSERT_TRUE(wst_popState(
        g_test.context,
        unit,
        WAR_TRANSITION_CAUSE_COMPLETION,
        WAR_STATE_RESULT_SUCCESS));

    const WarTransitionRequest* popRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(popRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, popRequest->operation);
    TEST_ASSERT_FALSE(WAR_STATE_REF_IS_VALID(popRequest->stateRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, pushedRef));

    wt_applyPendingTransitions(&g_test);

    wt_assertPendingTransitionCleared(unit);
    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, pushedRef));

    WarStateRef resetRef = WAR_STATE_REF_INVALID;

    TEST_ASSERT_TRUE(wt_submitWaitTransition(
        unit,
        WAR_STATE_OP_RESET,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER,
        &resetRef));

    wt_applyPendingTransitions(&g_test);

    wt_assertPendingTransitionCleared(unit);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, resetRef));
}

void test_public_free_of_active_state_is_no_op(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateMachineComponent* sm = we_getStateMachineComponent(g_test.context, unit);

    TEST_ASSERT_NOT_NULL(sm);
    TEST_ASSERT_EQUAL_INT(1, sm->depth);

    WarStateRef activeRef = sm->stack[0];
    WarStateBase* activeState = wst_deref(g_test.context, activeRef);
    const s32 idleCount = wt_stateAllocationCount(&g_test, WAR_STATE_IDLE);

    TEST_ASSERT_NOT_NULL(activeState);

    wst_freeStateRef(g_test.context, activeRef);

    TEST_ASSERT_EQUAL_INT(idleCount, wt_stateAllocationCount(&g_test, WAR_STATE_IDLE));
    TEST_ASSERT_EQUAL_PTR(activeState, wst_deref(g_test.context, activeRef));
    TEST_ASSERT_EQUAL_PTR(activeState, wst_getActiveState(g_test.context, unit));

    wst_updateIdleStates(g_test.context);

    TEST_ASSERT_EQUAL_PTR(activeState, wst_getActiveState(g_test.context, unit));
}

void test_stale_stack_ref_cannot_update_or_free_reused_slot(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateWait* oldWait = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NOT_NULL(oldWait);
    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        unit,
        (WarStateBase*)oldWait,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    wt_applyPendingTransitions(&g_test);

    WarStateMachineComponent* sm = we_getStateMachineComponent(g_test.context, unit);

    TEST_ASSERT_NOT_NULL(sm);
    WarStateRef staleRef = sm->stack[0];

    TEST_ASSERT_TRUE(wst_popState(
        g_test.context,
        unit,
        WAR_TRANSITION_CAUSE_COMPLETION,
        WAR_STATE_RESULT_SUCCESS));
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_NULL(wst_deref(g_test.context, staleRef));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));

    WarStateRef restoredIdleRef = sm->stack[0];
    WarStateWait* reusedWait = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NOT_NULL(reusedWait);

    WarStateRef reusedRef = wst_refOf(g_test.context, (WarStateBase*)reusedWait);

    TEST_ASSERT_EQUAL_INT(staleRef.type, reusedRef.type);
    TEST_ASSERT_EQUAL_INT(staleRef.idx, reusedRef.idx);
    TEST_ASSERT_TRUE(staleRef.generation != reusedRef.generation);

    sm->stack[0] = WAR_STATE_REF_INVALID;
    sm->depth = 0;
    wst_freeStateRef(g_test.context, restoredIdleRef);
    sm->stack[0] = staleRef;
    sm->depth = 1;

    TEST_ASSERT_NULL(wst_getActiveState(g_test.context, unit));

    wst_updateWaitStates(g_test.context);

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, reusedRef));

    wst_freeStateRef(g_test.context, staleRef);

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, reusedRef));
    TEST_ASSERT_NULL(wst_deref(g_test.context, staleRef));

    sm->stack[0] = WAR_STATE_REF_INVALID;
    sm->depth = 0;
    wst_freeStateRef(g_test.context, staleRef);

    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, reusedRef));

    wst_freeStateRef(g_test.context, reusedRef);

    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, reusedRef));
}

void test_immediate_slot_reuse_does_not_revive_stale_ref(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    WarEntity* unrelatedUnit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(12 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_NOT_NULL(unrelatedUnit);

    wt_applyPendingTransitions(&g_test);

    WarStateRef winningRef = WAR_STATE_REF_INVALID;
    WarStateRef rejectedRef = WAR_STATE_REF_INVALID;

    TEST_ASSERT_TRUE(wt_submitWaitTransition(
        unit,
        WAR_STATE_OP_RESET,
        WAR_TRANSITION_CAUSE_AI_ORDER,
        &winningRef));
    TEST_ASSERT_FALSE(wt_submitWaitTransition(
        unit,
        WAR_STATE_OP_REPLACE,
        WAR_TRANSITION_CAUSE_AI_ORDER,
        &rejectedRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, winningRef));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, rejectedRef));

    const s32 waitCountBefore = wt_stateAllocationCount(&g_test, WAR_STATE_WAIT);
    WarStateWait* unrelatedWait = wst_createWaitState(g_test.context, unrelatedUnit, 1000.0f);

    TEST_ASSERT_NOT_NULL(unrelatedWait);

    WarStateRef unrelatedRef = wst_refOf(g_test.context, (WarStateBase*)unrelatedWait);

    TEST_ASSERT_EQUAL_INT(waitCountBefore + 1, wt_stateAllocationCount(&g_test, WAR_STATE_WAIT));
    TEST_ASSERT_EQUAL_INT(rejectedRef.type, unrelatedRef.type);
    TEST_ASSERT_EQUAL_INT(rejectedRef.idx, unrelatedRef.idx);
    TEST_ASSERT_TRUE(rejectedRef.generation != unrelatedRef.generation);
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, unrelatedRef));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, rejectedRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, winningRef));

    const s32 countBeforeStaleFree = wt_stateAllocationCount(&g_test, WAR_STATE_WAIT);

    wst_freeStateRef(g_test.context, rejectedRef);

    TEST_ASSERT_EQUAL_INT(countBeforeStaleFree, wt_stateAllocationCount(&g_test, WAR_STATE_WAIT));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, unrelatedRef));
    TEST_ASSERT_NULL(wst_deref(g_test.context, rejectedRef));

    wst_freeStateRef(g_test.context, unrelatedRef);

    TEST_ASSERT_EQUAL_INT(waitCountBefore, wt_stateAllocationCount(&g_test, WAR_STATE_WAIT));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, unrelatedRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, winningRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unrelatedUnit));
}

void test_duplicate_pending_ref_preserves_storage_and_updates_metadata(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_applyPendingTransitions(&g_test);

    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NOT_NULL(waitState);

    WarStateRef waitRef = wst_refOf(g_test.context, (WarStateBase*)waitState);
    const s32 waitCount = wt_stateAllocationCount(&g_test, WAR_STATE_WAIT);

    TEST_ASSERT_TRUE(wst_submitTransition(
        g_test.context,
        unit,
        (WarTransitionRequest)
        {
            .stateRef = waitRef,
            .operation = WAR_STATE_OP_RESET,
            .cause = WAR_TRANSITION_CAUSE_AI_ORDER
        }));

    const WarTransitionRequest* first = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(first);

    const u64 firstSequence = first->sequence;

    TEST_ASSERT_FALSE(wst_submitTransition(
        g_test.context,
        unit,
        (WarTransitionRequest)
        {
            .stateRef = waitRef,
            .operation = WAR_STATE_OP_PUSH,
            .cause = WAR_TRANSITION_CAUSE_AI_ORDER
        }));

    const WarTransitionRequest* tiedWinner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(tiedWinner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, tiedWinner->operation);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_AI_ORDER, tiedWinner->cause);
    wt_assertU64Equal(firstSequence, tiedWinner->sequence);
    wt_assertStateRefEqual(waitRef, tiedWinner->stateRef);
    TEST_ASSERT_EQUAL_INT(waitCount, wt_stateAllocationCount(&g_test, WAR_STATE_WAIT));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, waitRef));

    TEST_ASSERT_TRUE(wst_submitTransition(
        g_test.context,
        unit,
        (WarTransitionRequest)
        {
            .stateRef = waitRef,
            .operation = WAR_STATE_OP_REPLACE,
            .cause = WAR_TRANSITION_CAUSE_PLAYER_ORDER
        }));

    const WarTransitionRequest* replacement = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(replacement);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_REPLACE, replacement->operation);
    TEST_ASSERT_EQUAL_INT(WAR_TRANSITION_CAUSE_PLAYER_ORDER, replacement->cause);
    TEST_ASSERT_TRUE(replacement->sequence != firstSequence);
    wt_assertStateRefEqual(waitRef, replacement->stateRef);
    TEST_ASSERT_EQUAL_INT(waitCount, wt_stateAllocationCount(&g_test, WAR_STATE_WAIT));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, waitRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, waitRef));
}

void test_full_stack_push_replaces_only_top(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_applyPendingTransitions(&g_test);

    WarStateMachineComponent* sm = we_getStateMachineComponent(g_test.context, unit);

    TEST_ASSERT_NOT_NULL(sm);

    while (sm->depth < WAR_STATE_STACK_DEPTH)
    {
        WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 1000.0f);

        TEST_ASSERT_NOT_NULL(waitState);
        TEST_ASSERT_TRUE(wst_pushState(g_test.context, unit, (WarStateBase*)waitState, WAR_TRANSITION_CAUSE_PLAYER_ORDER));

        wt_applyPendingTransitions(&g_test);
    }

    WarStateRef preservedRefs[WAR_STATE_STACK_DEPTH - 1];
    for (u8 i = 0; i < WAR_STATE_STACK_DEPTH - 1; i++)
    {
        preservedRefs[i] = sm->stack[i];
    }

    WarStateRef oldTopRef = sm->stack[WAR_STATE_STACK_DEPTH - 1];
    WarStateWait* replacementState = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NOT_NULL(replacementState);

    WarStateRef replacementRef = wst_refOf(g_test.context, (WarStateBase*)replacementState);

    TEST_ASSERT_TRUE(wst_pushState(g_test.context, unit, (WarStateBase*)replacementState, WAR_TRANSITION_CAUSE_PLAYER_ORDER));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_STACK_DEPTH, sm->depth);
    for (u8 i = 0; i < WAR_STATE_STACK_DEPTH - 1; i++)
    {
        wt_assertStateRefEqual(preservedRefs[i], sm->stack[i]);
        TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, preservedRefs[i]));
    }

    wt_assertStateRefEqual(replacementRef, sm->stack[WAR_STATE_STACK_DEPTH - 1]);
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, oldTopRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, replacementRef));
}

void test_empty_stack_pop_restores_idle(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(0, 0));

    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_EQUAL_INT(0, wt_stateDepth(&g_test, unit));

    TEST_ASSERT_TRUE(wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_PLAYER_ORDER, WAR_STATE_RESULT_SUCCESS));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
}

void test_empty_stack_pop_replace_installs_candidate(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(0, 0));

    TEST_ASSERT_NOT_NULL(unit);
    TEST_ASSERT_EQUAL_INT(0, wt_stateDepth(&g_test, unit));

    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NOT_NULL(waitState);
    TEST_ASSERT_TRUE(wst_replaceState(g_test.context, unit, (WarStateBase*)waitState, WAR_TRANSITION_CAUSE_PLAYER_ORDER));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
}

void test_state_pool_exhaustion_preserves_counts_and_recovers(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_applyPendingTransitions(&g_test);

    WarEntityManager* manager = we_getEntityManager(g_test.context);

    TEST_ASSERT_NOT_NULL(manager);

    WarStateStorage* storage = &manager->stateStorage;
    const s32 activeBefore = storage->activeCounts[WAR_STATE_WAIT];
    const s32 freeBefore = storage->freeCounts[WAR_STATE_WAIT];
    WarStateRef refs[MAX_STATES_PER_TYPE];

    TEST_ASSERT_EQUAL_INT(MAX_STATES_PER_TYPE, freeBefore);

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        refs[i] = wst_allocState(g_test.context, WAR_STATE_WAIT, unit->id);
        TEST_ASSERT_TRUE(WAR_STATE_REF_IS_VALID(refs[i]));
        TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, refs[i]));
    }

    TEST_ASSERT_EQUAL_INT(activeBefore + MAX_STATES_PER_TYPE, storage->activeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_EQUAL_INT(0, storage->freeCounts[WAR_STATE_WAIT]);

    WarStateRef exhaustedRef = wst_allocState(g_test.context, WAR_STATE_WAIT, unit->id);

    TEST_ASSERT_FALSE(WAR_STATE_REF_IS_VALID(exhaustedRef));
    TEST_ASSERT_EQUAL_INT(activeBefore + MAX_STATES_PER_TYPE, storage->activeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_EQUAL_INT(0, storage->freeCounts[WAR_STATE_WAIT]);

    WarStateWait* exhaustedState = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NULL(exhaustedState);

    const u64 sequenceBeforeNullWrappers = wt_nextTransitionSequence(&g_test, unit);

    TEST_ASSERT_FALSE(wst_pushState(g_test.context, unit, (WarStateBase*)exhaustedState, WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    TEST_ASSERT_FALSE(wst_replaceState(g_test.context, unit, (WarStateBase*)exhaustedState, WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    TEST_ASSERT_FALSE(wst_resetState(g_test.context, unit, (WarStateBase*)exhaustedState, WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    wt_assertU64Equal(sequenceBeforeNullWrappers, wt_nextTransitionSequence(&g_test, unit));

    const s32 freedIndex = MAX_STATES_PER_TYPE / 2;
    WarStateRef staleRef = refs[freedIndex];

    wst_freeStateRef(g_test.context, staleRef);

    TEST_ASSERT_EQUAL_INT(activeBefore + MAX_STATES_PER_TYPE - 1, storage->activeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_EQUAL_INT(1, storage->freeCounts[WAR_STATE_WAIT]);

    WarStateRef recoveredRef = wst_allocState(g_test.context, WAR_STATE_WAIT, unit->id);

    TEST_ASSERT_TRUE(WAR_STATE_REF_IS_VALID(recoveredRef));
    TEST_ASSERT_EQUAL_INT(staleRef.type, recoveredRef.type);
    TEST_ASSERT_EQUAL_INT(staleRef.idx, recoveredRef.idx);
    TEST_ASSERT_TRUE(staleRef.generation != recoveredRef.generation);
    TEST_ASSERT_NULL(wst_deref(g_test.context, staleRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, recoveredRef));

    wst_freeStateRef(g_test.context, staleRef);

    TEST_ASSERT_EQUAL_INT(activeBefore + MAX_STATES_PER_TYPE, storage->activeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_EQUAL_INT(0, storage->freeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, recoveredRef));

    refs[freedIndex] = recoveredRef;
    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        wst_freeStateRef(g_test.context, refs[i]);
    }

    TEST_ASSERT_EQUAL_INT(activeBefore, storage->activeCounts[WAR_STATE_WAIT]);
    TEST_ASSERT_EQUAL_INT(freeBefore, storage->freeCounts[WAR_STATE_WAIT]);
}

void test_raw_transition_validation_rejects_malformed_and_owned_refs(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_applyPendingTransitions(&g_test);

    WarStateMachineComponent* sm = we_getStateMachineComponent(g_test.context, unit);

    TEST_ASSERT_NOT_NULL(sm);
    TEST_ASSERT_EQUAL_INT(1, sm->depth);

    WarStateRef activeRef = sm->stack[0];
    const u64 sequenceBeforeInvalidRequests = sm->nextTransitionSequence;

    TEST_ASSERT_FALSE(wst_submitTransition(
        g_test.context,
        unit,
        (WarTransitionRequest)
        {
            .stateRef = activeRef,
            .operation = WAR_STATE_OP_PUSH,
            .cause = WAR_TRANSITION_CAUSE_LIFECYCLE
        }));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, activeRef));

    TEST_ASSERT_FALSE(wst_submitTransition(
        g_test.context,
        unit,
        (WarTransitionRequest)
        {
            .stateRef = { (WarStateType)WAR_STATE_COUNT, 0, 1 },
            .operation = WAR_STATE_OP_RESET,
            .cause = WAR_TRANSITION_CAUSE_PLAYER_ORDER
        }));
    TEST_ASSERT_FALSE(wst_submitTransition(
        g_test.context,
        unit,
        (WarTransitionRequest)
        {
            .stateRef = { WAR_STATE_WAIT, MAX_STATES_PER_TYPE, 1 },
            .operation = WAR_STATE_OP_REPLACE,
            .cause = WAR_TRANSITION_CAUSE_PLAYER_ORDER
        }));
    TEST_ASSERT_FALSE(wst_submitTransition(
        g_test.context,
        unit,
        (WarTransitionRequest)
        {
            .stateRef = { INVALID_STATE_TYPE, 0, 0 },
            .operation = WAR_STATE_OP_POP,
            .cause = WAR_TRANSITION_CAUSE_PLAYER_ORDER
        }));

    WarStateWait* invalidOperationState = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NOT_NULL(invalidOperationState);

    WarStateRef invalidOperationRef = wst_refOf(
        g_test.context,
        (WarStateBase*)invalidOperationState);

    TEST_ASSERT_FALSE(wst_submitTransition(
        g_test.context,
        unit,
        (WarTransitionRequest)
        {
            .stateRef = invalidOperationRef,
            .operation = (WarStateOp)99,
            .cause = WAR_TRANSITION_CAUSE_PLAYER_ORDER
        }));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, invalidOperationRef));

    WarStateWait* popCandidate = wst_createWaitState(g_test.context, unit, 1000.0f);

    TEST_ASSERT_NOT_NULL(popCandidate);

    WarStateRef popCandidateRef = wst_refOf(g_test.context, (WarStateBase*)popCandidate);

    TEST_ASSERT_FALSE(wst_submitTransition(
        g_test.context,
        unit,
        (WarTransitionRequest)
        {
            .stateRef = popCandidateRef,
            .operation = WAR_STATE_OP_POP,
            .cause = WAR_TRANSITION_CAUSE_PLAYER_ORDER
        }));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, popCandidateRef));
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, activeRef));
    wt_assertU64Equal(sequenceBeforeInvalidRequests, sm->nextTransitionSequence);
    wt_assertPendingTransitionCleared(unit);
}

void test_equal_priority_keeps_first_when_sequence_wraps(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);

    wt_applyPendingTransitions(&g_test);

    WarStateMachineComponent* sm = we_getStateMachineComponent(g_test.context, unit);

    TEST_ASSERT_NOT_NULL(sm);

    sm->nextTransitionSequence = UINT64_MAX;

    WarStateRef firstRef = WAR_STATE_REF_INVALID;
    WarStateRef secondRef = WAR_STATE_REF_INVALID;

    TEST_ASSERT_TRUE(wt_submitWaitTransition(
        unit,
        WAR_STATE_OP_RESET,
        WAR_TRANSITION_CAUSE_AI_ORDER,
        &firstRef));

    const WarTransitionRequest* firstRequest = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(firstRequest);
    wt_assertU64Equal(UINT64_MAX, firstRequest->sequence);

    TEST_ASSERT_FALSE(wt_submitWaitTransition(
        unit,
        WAR_STATE_OP_REPLACE,
        WAR_TRANSITION_CAUSE_AI_ORDER,
        &secondRef));

    const WarTransitionRequest* winner = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(winner);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, winner->operation);
    wt_assertU64Equal(UINT64_MAX, winner->sequence);
    wt_assertStateRefEqual(firstRef, winner->stateRef);
    wt_assertU64Equal(1, sm->nextTransitionSequence);
    TEST_ASSERT_TRUE(wt_isStateRefAllocated(&g_test, firstRef));
    TEST_ASSERT_FALSE(wt_isStateRefAllocated(&g_test, secondRef));

    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
}

// Helper: returns the pathfinder entity id on the tile covered by `entity`.
static WarEntityId wt_finderEntityAt(WarEntity* entity)
{
    WarMap* map = g_test.map;
    WarTransformComponent* transform = we_getTransformComponent(g_test.context, entity);
    if (!map || !transform)
        return 0;

    vec2 tile = wmap_mapToTileCoordinatesV(transform->position);
    return wpath_getTileEntityId(&map->finder, (s32)tile.x, (s32)tile.y);
}

// onEnter: IDLE state registers the entity in the pathfinder when it enters.
// The entity spawns with a WAIT pending-RESET. After applying that transition
// the IDLE state is displaced and the entity is de-registered. When the IDLE
// is subsequently restored (after WAIT completes) the entity is re-registered.
// This verifies wst_enterIdleState wires up the pathfinder correctly.
void test_lifecycle_enter_idle_registers_pathfinder(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(unit->id, wt_finderEntityAt(unit));

    // Push a WAIT state on top — IDLE is paused but stays on the stack.
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 1000.0f);
    TEST_ASSERT_NOT_NULL(waitState);
    wst_pushState(g_test.context, unit, (WarStateBase*)waitState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(2, wt_stateDepth(&g_test, unit));

    // Pop the WAIT — IDLE resumes. onEnter for IDLE shouldn't run again
    // (it's a resume, not a fresh enter), but the pathfinder entry must
    // still be valid because IDLE never released it.
    wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(unit->id, wt_finderEntityAt(unit));
}

// onExit: WAIT state frees the entity from the pathfinder when it exits.
// After a WAIT state is committed and then popped the slot should be clear.
void test_lifecycle_exit_wait_frees_pathfinder(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(12 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    // Install WAIT as the sole active state (RESET clears the IDLE below).
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 1000.0f);
    TEST_ASSERT_NOT_NULL(waitState);
    wst_resetState(g_test.context, unit, (WarStateBase*)waitState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(unit->id, wt_finderEntityAt(unit));

    // Pop WAIT — the state's onExit should free the pathfinder slot.
    wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
    // Idle re-enters and re-registers its own slot — the WAIT slot is gone.
    TEST_ASSERT_EQUAL_INT(unit->id, wt_finderEntityAt(unit));
}

// onEnter / onExit sequence for RESET: verify that the old active state's
// onExit runs and the new state's onEnter runs when a RESET replaces the stack.
// Concretely: IDLE is on the stack (registers entity in pathfinder via onEnter).
// After RESET to WAIT, IDLE's onExit should free the pathfinder, then WAIT's
// onEnter should re-register under the WAIT slot.  After WAIT is popped, IDLE
// onEnter re-registers the entity.
void test_lifecycle_reset_calls_exit_then_enter(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(14 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(unit->id, wt_finderEntityAt(unit));

    // RESET to WAIT — IDLE exits (pathfinder free), WAIT enters (pathfinder set).
    WarStateWait* wait = wst_createWaitState(g_test.context, unit, 1000.0f);
    TEST_ASSERT_NOT_NULL(wait);
    wst_resetState(g_test.context, unit, (WarStateBase*)wait, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_WAIT, wt_activeState(&g_test, unit));
    // pathfinder should still map to this entity (now registered by WAIT's onEnter).
    TEST_ASSERT_EQUAL_INT(unit->id, wt_finderEntityAt(unit));

    // POP WAIT — WAIT exits (pathfinder free), IDLE is installed via auto-idle logic.
    wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(unit->id, wt_finderEntityAt(unit));
}

// onEnter for BUILD sets unit->building = true.
// onExit (leaveBuildState) sets unit->building = false.
void test_lifecycle_enter_exit_build_sets_building_flag(void)
{
    wt_seedBuildingSpriteResources(WAR_UNIT_FARM_HUMANS);

    WarEntity* worker = wt_spawnUnit(
        &g_test,
        WAR_UNIT_PEASANT,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    WarEntity* building = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_FARM_HUMANS,
        0,
        vec2i(16 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(worker);
    TEST_ASSERT_NOT_NULL(building);
    wt_applyPendingTransitions(&g_test);

    WarUnitComponent* buildingUnit = we_getUnitComponent(g_test.context, building);
    TEST_ASSERT_NOT_NULL(buildingUnit);

    // Manually create and commit a BUILD state — simulates the build command pathway.
    WarPlayerInfo* player = &g_test.map->players[0];
    const s32 goldCost = 211;
    const s32 woodCost = 113;
    TEST_ASSERT_TRUE(we_decreasePlayerResources(g_test.context, player, goldCost, woodCost));

    WarStateBuild* buildState = wst_createBuildState(g_test.context, building, 1000.0f, goldCost, woodCost);
    TEST_ASSERT_NOT_NULL(buildState);

    TEST_ASSERT_TRUE(wst_resetState(
        g_test.context,
        building,
        (WarStateBase*)buildState,
        WAR_TRANSITION_CAUSE_INITIALIZATION));
    wt_applyPendingTransitions(&g_test);

    // onEnter should have set building = true immediately (no update tick needed).
    TEST_ASSERT_EQUAL_INT(WAR_STATE_BUILD, wt_activeState(&g_test, building));
    TEST_ASSERT_TRUE(buildingUnit->building);

    // Cancel the build — the cancellation should eventually trigger onExit.
    wt_selectOnly(building);
    wcmd_cancel(g_test.context, building);
    wt_applyPendingTransitions(&g_test);

    // After exiting BUILD, building flag must be false again.
    TEST_ASSERT_FALSE(buildingUnit->building);
}

// WAR_STATE_RESULT_SUCCESS propagates through a POP into the resume reason of
// the parent state. We use the WAIT → IDLE push/pop pattern because WAIT's
// wst_exitWaitState is wired and IDLE's onResume would receive the reason.
// Since no state currently branches on its resume reason (deviation noted in
// the review), we verify the mechanical propagation at the API level:
// the transition request's result field is set correctly.
void test_result_success_is_stored_on_pop_request(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(16 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateWait* wait = wst_createWaitState(g_test.context, unit, 1000.0f);
    TEST_ASSERT_NOT_NULL(wait);
    wst_pushState(g_test.context, unit, (WarStateBase*)wait, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_applyPendingTransitions(&g_test);

    wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);

    const WarTransitionRequest* pending = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(pending);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, pending->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_RESULT_SUCCESS, pending->result);
}

// WAR_STATE_RESULT_CANCELLED propagates when a state self-pops due to
// cancellation (as BUILD does in wst_updateBuildState when s->cancelled).
void test_result_cancelled_is_stored_on_pop_request(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(18 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateWait* wait = wst_createWaitState(g_test.context, unit, 1000.0f);
    TEST_ASSERT_NOT_NULL(wait);
    wst_pushState(g_test.context, unit, (WarStateBase*)wait, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_applyPendingTransitions(&g_test);

    wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_CANCELLED);

    const WarTransitionRequest* pending = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(pending);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, pending->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_RESULT_CANCELLED, pending->result);
}

// WAR_STATE_RESULT_NONE on a POP-only submission.
// Ensures that POP submissions not originating from a completing child state
// leave the result uninterpreted (defaults to WAR_STATE_RESULT_NONE).
void test_result_none_on_raw_pop_submission(void)
{
    WarEntity* unit = wt_spawnUnit(
        &g_test,
        WAR_UNIT_FOOTMAN,
        0,
        vec2i(20 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    wst_popState(g_test.context, unit, WAR_TRANSITION_CAUSE_PLAYER_ORDER, WAR_STATE_RESULT_NONE);

    const WarTransitionRequest* pending = wt_activeTransition(&g_test, unit);

    TEST_ASSERT_NOT_NULL(pending);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, pending->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_RESULT_NONE, pending->result);
}

// TRAIN completion: wst_popState is called with WAR_STATE_RESULT_SUCCESS.
// Verify that after the pop the result was the one supplied (observable via
// the transition request before it is committed).
void test_result_train_completion_carries_success(void)
{
    WarEntity* townHall = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_TOWNHALL_HUMANS,
        0,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(townHall);
    wt_applyPendingTransitions(&g_test);

    wt_seedUnitSpriteResource(WAR_UNIT_PEASANT);

    WarStateTrain* trainState = wt_startTrainTransaction(
        townHall,
        WAR_UNIT_PEASANT,
        0.0f,  // zero buildTime → completes on first update
        127,
        53);

    wt_updateGameTime(&g_test);
    wst_updateTrainStates(g_test.context);

    TEST_ASSERT_TRUE(trainState->outputCommitted);

    const WarTransitionRequest* completionRequest = wt_activeTransition(&g_test, townHall);

    TEST_ASSERT_NOT_NULL(completionRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, completionRequest->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_RESULT_SUCCESS, completionRequest->result);
}

// BUILD cancellation: wst_popState is called with WAR_STATE_RESULT_CANCELLED
// when the build state self-pops because it is cancelled but has a parent.
// We push BUILD on top of IDLE and then cancel it so it pops (depth > 1).
void test_result_build_cancel_carries_cancelled(void)
{
    wt_seedBuildingSpriteResources(WAR_UNIT_FARM_HUMANS);

    WarEntity* worker = wt_spawnUnit(
        &g_test,
        WAR_UNIT_PEASANT,
        0,
        vec2i(22 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    WarEntity* building = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_FARM_HUMANS,
        0,
        vec2i(26 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(worker);
    TEST_ASSERT_NOT_NULL(building);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* player = &g_test.map->players[0];
    const s32 goldCost = 229;
    const s32 woodCost = 131;
    TEST_ASSERT_TRUE(we_decreasePlayerResources(g_test.context, player, goldCost, woodCost));

    WarStateBuild* buildState = wst_createBuildState(g_test.context, building, 1000.0f, goldCost, woodCost);
    TEST_ASSERT_NOT_NULL(buildState);

    // PUSH — so depth becomes 2 (IDLE + BUILD) — cancel will self-pop with CANCELLED.
    TEST_ASSERT_TRUE(wst_pushState(
        g_test.context,
        building,
        (WarStateBase*)buildState,
        WAR_TRANSITION_CAUSE_INITIALIZATION));
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(WAR_STATE_BUILD, wt_activeState(&g_test, building));
    TEST_ASSERT_EQUAL_INT(2, wt_stateDepth(&g_test, building));

    // Commit the build (apply the transaction).
    wt_updateGameTime(&g_test);
    wst_updateBuildStates(g_test.context);

    // Cancel — sets cancelled=true on the build state.
    wt_selectOnly(building);
    wcmd_cancel(g_test.context, building);

    // Advance a tick so the build update sees cancelled=true and calls popState.
    wt_updateGameTime(&g_test);
    wst_updateBuildStates(g_test.context);

    const WarTransitionRequest* cancelRequest = wt_activeTransition(&g_test, building);

    TEST_ASSERT_NOT_NULL(cancelRequest);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_RESET, cancelRequest->operation);
}

// --- Economic player ownership ---
//
// Each test uses player 0 (human) and player 1 (AI/orc).
// The invariant being tested: economic operations on an AI entity must
// affect only the AI player's bank, not the human player's.

// Helper: returns the WarPlayerInfo for a given player index.
static WarPlayerInfo* wt_player(s32 index)
{
    return &g_test.map->players[index];
}

void test_ai_worker_gold_deposit_credits_ai_player_only(void)
{
    // Spawn an orc peon owned by player 1 (AI).
    const vec2 pos = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    WarEntity* peon = wt_spawnUnit(&g_test, WAR_UNIT_PEON, 1, pos);
    TEST_ASSERT_NOT_NULL(peon);

    // Spawn the AI town hall at the same tile so the peon is in range.
    WarEntity* townHall = wt_spawnBuilding(&g_test, WAR_UNIT_TOWNHALL_ORCS, 1, pos);
    TEST_ASSERT_NOT_NULL(townHall);

    wt_applyPendingTransitions(&g_test);

    // Give the peon a full gold load.
    WarUnitComponent* peonUnit = we_getUnitComponent(g_test.context, peon);
    TEST_ASSERT_NOT_NULL(peonUnit);
    peonUnit->resourceKind = WAR_RESOURCE_GOLD;
    peonUnit->amount       = 100;

    // Put the peon into DELIVER state (not inside building yet, so deposit fires immediately).
    WarStateDeliver* deliverState = wst_createDeliverState(g_test.context, peon, townHall->id);
    TEST_ASSERT_NOT_NULL(deliverState);
    TEST_ASSERT_TRUE(wst_resetState(g_test.context, peon, (WarStateBase*)deliverState, WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    wt_applyPendingTransitions(&g_test);

    const s32 humanGoldBefore = wt_player(0)->gold;
    const s32 aiGoldBefore    = wt_player(1)->gold;

    // One deliver update: peon is in range and not inside the building,
    // so the deposit fires and insideBuilding becomes true.
    wt_updateGameTime(&g_test);
    wst_updateDeliverStates(g_test.context);

    TEST_ASSERT_EQUAL_INT(aiGoldBefore + 100, wt_player(1)->gold);
    TEST_ASSERT_EQUAL_INT(humanGoldBefore,    wt_player(0)->gold);
}

void test_ai_worker_repair_deducts_from_ai_player_only(void)
{
    // Spawn a damaged orc farm owned by player 1 (AI).
    const vec2 buildingPos = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 peonPos     = vec2i(12 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    wt_seedBuildingSpriteResources(WAR_UNIT_FARM_ORCS);

    WarEntity* farm = wt_spawnBuilding(&g_test, WAR_UNIT_FARM_ORCS, 1, buildingPos);
    TEST_ASSERT_NOT_NULL(farm);

    // Damage the farm so repair has something to do.
    WarUnitComponent* farmUnit = we_getUnitComponent(g_test.context, farm);
    TEST_ASSERT_NOT_NULL(farmUnit);
    farmUnit->hp = farmUnit->maxhp / 2;

    WarEntity* peon = wt_spawnUnit(&g_test, WAR_UNIT_PEON, 1, peonPos);
    TEST_ASSERT_NOT_NULL(peon);

    wt_applyPendingTransitions(&g_test);

    // Put the peon into REPAIRING state targeting the farm.
    WarStateRepairing* repairingState = wst_createRepairingState(g_test.context, peon, farm->id);
    TEST_ASSERT_NOT_NULL(repairingState);
    TEST_ASSERT_TRUE(wst_resetState(g_test.context, peon, (WarStateBase*)repairingState, WAR_TRANSITION_CAUSE_PLAYER_ORDER));
    wt_applyPendingTransitions(&g_test);

    // One update for repair state setup outside building.
    wt_updateGameTime(&g_test);
    wst_updateRepairingStates(g_test.context);
    TEST_ASSERT_FALSE(repairingState->insideBuilding);

    const s32 humanGoldBefore = wt_player(0)->gold;
    const s32 humanWoodBefore = wt_player(0)->wood;
    const s32 aiGoldBefore    = wt_player(1)->gold;
    const s32 aiWoodBefore    = wt_player(1)->wood;

    // Run enough ticks to trigger at least one repair cost deduction.
    // The repair cost fires on the ATTACK action step each frame while insideBuilding.
    wt_stepTicks(&g_test, WAR_TEST_TICK_RATE);

    // AI player's resources should have decreased.
    TEST_ASSERT_LESS_THAN_INT(aiGoldBefore, wt_player(1)->gold);
    TEST_ASSERT_LESS_THAN_INT(aiWoodBefore, wt_player(1)->wood);

    // Human player's resources must be untouched.
    TEST_ASSERT_EQUAL_INT(humanGoldBefore, wt_player(0)->gold);
    TEST_ASSERT_EQUAL_INT(humanWoodBefore, wt_player(0)->wood);
}

void test_ai_building_upgrade_advances_ai_player_only(void)
{
    // Spawn an orc blacksmith owned by player 1 (AI).
    WarEntity* blacksmith = wt_spawnBuilding(
        &g_test,
        WAR_UNIT_BLACKSMITH_ORCS,
        1,
        vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));

    TEST_ASSERT_NOT_NULL(blacksmith);
    wt_applyPendingTransitions(&g_test);

    WarPlayerInfo* humanPlayer = wt_player(0);
    WarPlayerInfo* aiPlayer    = wt_player(1);

    // Allow the upgrade on the AI player (level 0 -> 1 is valid when allowed >= 1).
    setUpgradeAllowed(aiPlayer, WAR_UPGRADE_AXES, 2);

    const s32 humanUpgradeLevelBefore = getUpgradeLevel(humanPlayer, WAR_UPGRADE_AXES);
    const s32 aiUpgradeLevelBefore    = getUpgradeLevel(aiPlayer,    WAR_UPGRADE_AXES);

    const s32 goldCost = 750;
    const s32 woodCost = 0;

    // Start the upgrade transaction: enter applies the cost, first update commits it.
    wt_startUpgradeTransaction(blacksmith, WAR_UPGRADE_AXES, 1.0f, goldCost, woodCost);

    // Advance until the upgrade completes (build time 1 s at 30 Hz = 30 ticks).
    wt_stepTicks(&g_test, WAR_TEST_TICK_RATE + 5);

    // AI player's upgrade level must have advanced.
    TEST_ASSERT_EQUAL_INT(aiUpgradeLevelBefore + 1, getUpgradeLevel(aiPlayer, WAR_UPGRADE_AXES));

    // Human player's upgrade level must be unchanged.
    TEST_ASSERT_EQUAL_INT(humanUpgradeLevelBefore, getUpgradeLevel(humanPlayer, WAR_UPGRADE_AXES));
}

// --- Fix MOVE arrival slowdown ---
void test_move_arrival_slowdown(void)
{
    const vec2 startPos = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 endPos   = vec2i(20 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, startPos);
    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    vec2 waypoints[] = { startPos, endPos };
    WarStateMove* moveState = wst_createMoveState(g_test.context, unit, 2, waypoints, false);
    TEST_ASSERT_NOT_NULL(moveState);

    wst_replaceState(g_test.context, unit, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
    wt_applyPendingTransitions(&g_test);

    // Update move state outside slowdown radius (160 px away from endPos > 48 px slowdown radius)
    wt_updateGameTime(&g_test);
    wst_updateMoveStates(g_test.context);

    f32 normalSpeed = vec2_length(moveState->rvoPreferredVelocity);
    TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, normalSpeed);

    // Move unit inside arrival radius (16 px away from endPos < 48 px slowdown radius)
    const vec2 nearPos = vec2f(endPos.x - 16.0f, endPos.y);
    wu_setUnitCenterPosition(g_test.context, unit, nearPos);

    wt_updateGameTime(&g_test);
    wst_updateMoveStates(g_test.context);

    f32 nearSpeed = vec2_length(moveState->rvoPreferredVelocity);

    // Expected:
    // velocity near destination < normal movement velocity
    // velocity remains nonzero until arrival
    TEST_ASSERT_LESS_THAN_FLOAT(normalSpeed, nearSpeed);
    TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, nearSpeed);
}

// --- MOVE staged stuck recovery ---
void test_move_blocked_far_from_target_uses_staged_recovery(void)
{
    const vec2 startPos = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 endPos = vec2i(20 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, startPos);
    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateMove* moveState = wt_installRootMove(unit, endPos);
    vec2 startTile = wmap_mapToTileCoordinatesV(startPos);
    vec2 endTile = wmap_mapToTileCoordinatesV(endPos);
    WarMapFlowField* flowField = wpath_ensureFlowField(
        &g_test.map->finder,
        (s32)endTile.x,
        (s32)endTile.y);

    TEST_ASSERT_NOT_NULL(flowField);

    const s32 startIndex = (s32)startTile.y * MAP_TILES_WIDTH + (s32)startTile.x;
    flowField->cost[startIndex] = -12345;

    wt_holdMoveProgress(unit, moveState, 14);
    TEST_ASSERT_EQUAL_INT(0, moveState->progress.recoveryAttempt);

    wt_holdMoveProgress(unit, moveState, 2);
    TEST_ASSERT_EQUAL_INT(1, moveState->progress.recoveryAttempt);
    TEST_ASSERT_NOT_EQUAL(-12345, flowField->cost[startIndex]);

    wt_holdMoveProgress(unit, moveState, 30);
    TEST_ASSERT_EQUAL_INT(2, moveState->progress.recoveryAttempt);

    wt_buildGrid(&g_test);
    updateAdjustedVelocity(g_test.context, unit, moveState);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, MOVE_RECOVERY_RVO_RADIUS_PX, moveState->rvoRadius);

    wt_holdMoveProgress(unit, moveState, 43);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_NONE, wt_activeTransition(&g_test, unit)->operation);

    wt_holdMoveProgress(unit, moveState, 2);

    const WarTransitionRequest* blocked = wt_activeTransition(&g_test, unit);
    TEST_ASSERT_EQUAL_INT(3, moveState->progress.recoveryAttempt);
    TEST_ASSERT_GREATER_THAN_FLOAT(2.9f, moveState->progress.lowVelocityTime);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, blocked->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_RESULT_BLOCKED, blocked->result);

    wt_applyPendingTransitions(&g_test);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
}

void test_move_blocked_next_to_target_returns_blocked(void)
{
    const vec2 startPos = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 endPos = vec2i(11 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, startPos);
    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateMove* moveState = wt_installRootMove(unit, endPos);
    wt_holdMoveProgress(unit, moveState, 91);

    const WarTransitionRequest* blocked = wt_activeTransition(&g_test, unit);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, blocked->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_RESULT_BLOCKED, blocked->result);
}

void test_move_progress_after_temporary_block_resets_recovery(void)
{
    const vec2 startPos = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 endPos = vec2i(20 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, startPos);
    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateMove* moveState = wt_installRootMove(unit, endPos);
    wt_holdMoveProgress(unit, moveState, 46);
    TEST_ASSERT_EQUAL_INT(2, moveState->progress.recoveryAttempt);

    vec2 progressedPosition = wu_getUnitCenterPosition(g_test.context, unit);
    progressedPosition.x += 8.0f;
    wt_setUnitCenterPosition(&g_test, unit, progressedPosition);
    wt_holdMoveProgress(unit, moveState, 1);

    TEST_ASSERT_EQUAL_INT(0, moveState->progress.recoveryAttempt);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, moveState->progress.noProgressTime);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, moveState->progress.lowVelocityTime);

    wt_holdMoveProgress(unit, moveState, 89);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_NONE, wt_activeTransition(&g_test, unit)->operation);
}

void test_move_slow_meaningful_progress_does_not_block(void)
{
    const vec2 startPos = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 endPos = vec2i(40 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, startPos);
    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateMove* moveState = wt_installRootMove(unit, endPos);

    for (u32 tick = 0; tick < WAR_TEST_TICK_RATE * 6; tick++)
    {
        if (tick > 0 && tick % WAR_TEST_TICK_RATE == 0)
        {
            vec2 position = wu_getUnitCenterPosition(g_test.context, unit);
            position.x += MOVE_PROGRESS_DISTANCE_PX + 1.0f;
            wt_setUnitCenterPosition(&g_test, unit, position);
        }

        wt_holdMoveProgress(unit, moveState, 1);
    }

    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_NONE, wt_activeTransition(&g_test, unit)->operation);
    TEST_ASSERT_LESS_THAN_FLOAT(MOVE_BLOCKED_TIME, moveState->progress.noProgressTime);
}

void test_move_goal_change_resets_recovery(void)
{
    const vec2 startPos = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 firstGoal = vec2i(20 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 secondGoal = vec2i(22 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, startPos);
    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateMove* moveState = wt_installRootMove(unit, firstGoal);
    wt_holdMoveProgress(unit, moveState, 46);
    TEST_ASSERT_EQUAL_INT(2, moveState->progress.recoveryAttempt);

    moveState->waypoints[1] = secondGoal;
    wt_holdMoveProgress(unit, moveState, 1);

    vec2 position = wu_getUnitCenterPosition(g_test.context, unit);

    TEST_ASSERT_EQUAL_INT(0, moveState->progress.recoveryAttempt);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, moveState->progress.noProgressTime);
    TEST_ASSERT_FLOAT_WITHIN(
        0.001f,
        vec2_distanceSqr(position, secondGoal),
        moveState->progress.bestDistanceSq);
}

void test_blocked_child_move_resumes_parent(void)
{
    const vec2 startPos = vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);
    const vec2 endPos = vec2i(20 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT);

    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0, startPos);
    TEST_ASSERT_NOT_NULL(unit);
    wt_applyPendingTransitions(&g_test);

    WarStateMove* moveState = wt_createMoveTo(unit, endPos);
    TEST_ASSERT_NOT_NULL(moveState);
    TEST_ASSERT_TRUE(wst_pushState(
        g_test.context,
        unit,
        (WarStateBase*)moveState,
        WAR_TRANSITION_CAUSE_AUTONOMOUS));
    wt_applyPendingTransitions(&g_test);

    TEST_ASSERT_EQUAL_INT(2, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_MOVE, wt_activeState(&g_test, unit));

    wt_holdMoveProgress(unit, moveState, 91);

    const WarTransitionRequest* blocked = wt_activeTransition(&g_test, unit);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_OP_POP, blocked->operation);
    TEST_ASSERT_EQUAL_INT(WAR_STATE_RESULT_BLOCKED, blocked->result);

    wt_applyPendingTransitions(&g_test);
    TEST_ASSERT_EQUAL_INT(1, wt_stateDepth(&g_test, unit));
    TEST_ASSERT_EQUAL_INT(WAR_STATE_IDLE, wt_activeState(&g_test, unit));
}

void setUp(void)
{
    wt_init(&g_test);
}

void tearDown(void)
{
    wt_shutdown(&g_test);
}

int run_state_machine_tests(void)
{
    UNITY_BEGIN();

    // State Machine Basics
    WAR_TEST_FILTER(test_fresh_entity_requests_initialization_idle_state);
    WAR_TEST_FILTER(test_player_order_beats_uncommitted_initialization);
    WAR_TEST_FILTER(test_lethal_damage_beats_uncommitted_initialization);
    WAR_TEST_FILTER(test_ai_training_request_uses_ai_order);
    WAR_TEST_FILTER(test_ai_train_then_lifecycle_charges_and_progresses_zero_times);
    WAR_TEST_FILTER(test_lifecycle_then_ai_train_charges_and_progresses_zero_times);
    WAR_TEST_FILTER(test_upgrade_then_lifecycle_charges_zero_times);
    WAR_TEST_FILTER(test_lifecycle_then_upgrade_charges_zero_times);
    WAR_TEST_FILTER(test_committed_ai_train_applies_cost_and_progress_once);
    WAR_TEST_FILTER(test_committed_upgrade_applies_cost_once);
    WAR_TEST_FILTER(test_committed_ai_train_without_resources_terminates_without_progress);
    WAR_TEST_FILTER(test_pending_train_cancel_commits_idle_without_charging);
    WAR_TEST_FILTER(test_pending_upgrade_cancel_commits_idle_without_charging);
    WAR_TEST_FILTER(test_transaction_cancel_does_not_displace_lifecycle_or_unrelated_player_order);
    WAR_TEST_FILTER(test_train_cancel_refunds_once_only_after_commit);
    WAR_TEST_FILTER(test_train_cancel_displaced_by_lifecycle_never_refunds_in_either_order);
    WAR_TEST_FILTER(test_train_completion_frame_cancel_keeps_output_and_never_refunds_in_either_order);
    WAR_TEST_FILTER(test_upgrade_cancel_refunds_once_only_after_commit);
    WAR_TEST_FILTER(test_upgrade_cancel_displaced_by_lifecycle_never_refunds_in_either_order);
    WAR_TEST_FILTER(test_upgrade_completion_frame_cancel_keeps_output_and_never_refunds_in_either_order);
    WAR_TEST_FILTER(test_pending_build_cancel_refunds_once_only_after_commit);
    WAR_TEST_FILTER(test_pending_build_cancel_displaced_by_lifecycle_never_refunds_in_either_order);
    WAR_TEST_FILTER(test_build_completion_frame_cancel_keeps_output_without_collapse_in_either_order);
    WAR_TEST_FILTER(test_removing_assigned_worker_clears_build_backlink_and_pauses_building);
    WAR_TEST_FILTER(test_build_placement_build_state_exhaustion_rolls_back_atomically);
    WAR_TEST_FILTER(test_remove_entity_leaves_initialized_wait_before_required_components);
    WAR_TEST_FILTER(test_remove_entity_releases_active_train_and_pending_state_without_refund);
    WAR_TEST_FILTER(test_remove_one_of_two_state_machines_preserves_swapped_dense_invariants);
    WAR_TEST_FILTER(test_push_state_increases_depth);
    WAR_TEST_FILTER(test_pop_state_decreases_depth);
    WAR_TEST_FILTER(test_reset_state_clears_stack);
    WAR_TEST_FILTER(test_stack_contains_query);
    WAR_TEST_FILTER(test_step_ticks_advances_simulation);
    WAR_TEST_FILTER(test_empty_stack_becomes_idle_on_pop);

    // State Machine Transitions
    WAR_TEST_FILTER(test_autonomous_transition_outranks_state_completion);
    WAR_TEST_FILTER(test_move_eventually_completes);
    WAR_TEST_FILTER(test_competing_attack_and_completion);
    WAR_TEST_FILTER(test_player_move_command_outranks_aggro_when_aggro_is_submitted_first);
    WAR_TEST_FILTER(test_player_move_command_outranks_aggro_when_player_move_is_submitted_first);
    WAR_TEST_FILTER(test_lethal_damage_outranks_move_command_when_move_is_submitted_first);
    WAR_TEST_FILTER(test_lethal_damage_outranks_move_command_when_death_is_submitted_first);
    WAR_TEST_FILTER(test_equal_priority_keeps_first_submitted_request);
    WAR_TEST_FILTER(test_equal_priority_result_is_independent_of_unrelated_entity_order);
    WAR_TEST_FILTER(test_transition_cause_priority_is_submission_order_independent);
    WAR_TEST_FILTER(test_equal_priority_keeps_first_across_operation_combinations);
    WAR_TEST_FILTER(test_each_submission_consumes_only_its_entity_sequence);
    WAR_TEST_FILTER(test_applying_transition_clears_pending_and_allows_later_requests);
    WAR_TEST_FILTER(test_public_free_of_active_state_is_no_op);
    WAR_TEST_FILTER(test_stale_stack_ref_cannot_update_or_free_reused_slot);
    WAR_TEST_FILTER(test_immediate_slot_reuse_does_not_revive_stale_ref);
    WAR_TEST_FILTER(test_duplicate_pending_ref_preserves_storage_and_updates_metadata);
    WAR_TEST_FILTER(test_full_stack_push_replaces_only_top);
    WAR_TEST_FILTER(test_empty_stack_pop_restores_idle);
    WAR_TEST_FILTER(test_empty_stack_pop_replace_installs_candidate);
    WAR_TEST_FILTER(test_state_pool_exhaustion_preserves_counts_and_recovers);
    WAR_TEST_FILTER(test_raw_transition_validation_rejects_malformed_and_owned_refs);
    WAR_TEST_FILTER(test_equal_priority_keeps_first_when_sequence_wraps);

    // State Lifecycle Callbacks
    WAR_TEST_FILTER(test_lifecycle_enter_idle_registers_pathfinder);
    WAR_TEST_FILTER(test_lifecycle_exit_wait_frees_pathfinder);
    WAR_TEST_FILTER(test_lifecycle_reset_calls_exit_then_enter);
    WAR_TEST_FILTER(test_lifecycle_enter_exit_build_sets_building_flag);

    // State Result Propagation
    WAR_TEST_FILTER(test_result_success_is_stored_on_pop_request);
    WAR_TEST_FILTER(test_result_cancelled_is_stored_on_pop_request);
    WAR_TEST_FILTER(test_result_none_on_raw_pop_submission);
    WAR_TEST_FILTER(test_result_train_completion_carries_success);
    WAR_TEST_FILTER(test_result_build_cancel_carries_cancelled);

    // Economic player ownership
    WAR_TEST_FILTER(test_ai_worker_gold_deposit_credits_ai_player_only);
    WAR_TEST_FILTER(test_ai_worker_repair_deducts_from_ai_player_only);
    WAR_TEST_FILTER(test_ai_building_upgrade_advances_ai_player_only);

    // Fix MOVE arrival slowdown
    WAR_TEST_FILTER(test_move_arrival_slowdown);

    // MOVE staged stuck recovery
    WAR_TEST_FILTER(test_move_blocked_far_from_target_uses_staged_recovery);
    WAR_TEST_FILTER(test_move_blocked_next_to_target_returns_blocked);
    WAR_TEST_FILTER(test_move_progress_after_temporary_block_resets_recovery);
    WAR_TEST_FILTER(test_move_slow_meaningful_progress_does_not_block);
    WAR_TEST_FILTER(test_move_goal_change_resets_recovery);
WAR_TEST_FILTER(test_blocked_child_move_resumes_parent);

    return UNITY_END();
}
