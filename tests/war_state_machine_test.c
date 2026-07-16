#include "unity/unity.h"

#include "war_test_context.h"
#include "war_entities.h"
#include "war_map.h"
#include "war_state_machine.h"
#include "war_units.h"

WarTestContext g_test;

void setUp(void)
{
    wt_init(&g_test);
}

void tearDown(void)
{
    wt_shutdown(&g_test);
}

void test_create_entity_and_idle_state(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT(unit != NULL);
    TEST_ASSERT(unit->id != 0);

    wt_step(&g_test); // process pending ops

    // A freshly spawned unit should have exactly one state (IDLE)
    TEST_ASSERT_EQUAL_INT(wt_stateDepth(&g_test, unit), 1);
    TEST_ASSERT_EQUAL_INT(wt_activeState(&g_test, unit), WAR_STATE_IDLE);
}

void test_push_state_increases_depth(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT(unit != NULL);
    TEST_ASSERT(unit->id != 0);

    wt_step(&g_test); // process pending ops

    TEST_ASSERT_EQUAL_INT(wt_stateDepth(&g_test, unit), 1);

    // Push a WAIT state on top of IDLE
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 2.0f);
    wst_pushState(g_test.context, unit, (WarStateBase*)waitState);

    // Pending op should not yet be applied
    TEST_ASSERT_EQUAL_INT(wt_stateDepth(&g_test, unit), 1);

    // Step one tick to process the pending push
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(wt_stateDepth(&g_test, unit), 2);
    TEST_ASSERT_EQUAL_INT(wt_activeState(&g_test, unit), WAR_STATE_WAIT);
    TEST_ASSERT_EQUAL_INT(wt_stateAt(&g_test, unit, 0), WAR_STATE_IDLE);
    TEST_ASSERT_EQUAL_INT(wt_stateAt(&g_test, unit, 1), WAR_STATE_WAIT);
}

void test_pop_state_decreases_depth(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT(unit != NULL);
    TEST_ASSERT(unit->id != 0);

    wt_step(&g_test); // process pending ops

    // Push WAIT, step to apply
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 1.0f);
    wst_pushState(g_test.context, unit, (WarStateBase*)waitState);
    wt_step(&g_test);
    TEST_ASSERT_EQUAL_INT(wt_stateDepth(&g_test, unit), 2);

    // Pop WAIT
    wst_popState(g_test.context, unit);
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(wt_stateDepth(&g_test, unit), 1);
    TEST_ASSERT_EQUAL_INT(wt_activeState(&g_test, unit), WAR_STATE_IDLE);
}

void test_reset_state_clears_stack(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT(unit != NULL);
    TEST_ASSERT(unit->id != 0);

    wt_step(&g_test); // process pending ops

    // Push WAIT on top of IDLE
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 5.0f);
    wst_pushState(g_test.context, unit, (WarStateBase*)waitState);
    wt_step(&g_test);
    TEST_ASSERT_EQUAL_INT(wt_stateDepth(&g_test, unit), 2);

    // Reset with a fresh MOVE state
    vec2 positions[] = { vec2i(15 * MEGA_TILE_WIDTH, 15 * MEGA_TILE_HEIGHT) };
    WarStateMove* moveState = wst_createMoveState(g_test.context, unit, 1, positions);
    wst_resetState(g_test.context, unit, (WarStateBase*)moveState);
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(wt_stateDepth(&g_test, unit), 1);
    TEST_ASSERT_EQUAL_INT(wt_activeState(&g_test, unit), WAR_STATE_MOVE);
}

void test_stack_contains_query(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT(unit != NULL);
    TEST_ASSERT(unit->id != 0);

    wt_step(&g_test); // process pending ops

    TEST_ASSERT(wt_stackContains(&g_test, unit, WAR_STATE_IDLE));
    TEST_ASSERT(!wt_stackContains(&g_test, unit, WAR_STATE_ATTACK));

    // Push WAIT
    WarStateWait* waitState = wst_createWaitState(g_test.context, unit, 1.0f);
    wst_pushState(g_test.context, unit, (WarStateBase*)waitState);
    wt_step(&g_test);

    TEST_ASSERT(wt_stackContains(&g_test, unit, WAR_STATE_IDLE));
    TEST_ASSERT(wt_stackContains(&g_test, unit, WAR_STATE_WAIT));
}

void test_step_ticks_advances_simulation(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT(unit != NULL);
    TEST_ASSERT(unit->id != 0);

    u64 tickBefore = g_test.simulationTick;
    wt_stepTicks(&g_test, 5);
    u64 tickAfter = g_test.simulationTick;

    TEST_ASSERT_EQUAL_INT(tickAfter - tickBefore, 5);

    // Entity should still be in IDLE after 5 ticks (lookAround=false for buildings, true for dudes
    // but no enemies nearby so it stays IDLE)
    TEST_ASSERT_EQUAL_INT(wt_activeState(&g_test, unit), WAR_STATE_IDLE);
}

void test_empty_stack_becomes_idle_on_pop(void)
{
    WarEntity* unit = wt_spawnUnit(&g_test, WAR_UNIT_FOOTMAN, 0,
                                   vec2i(10 * MEGA_TILE_WIDTH, 10 * MEGA_TILE_HEIGHT));
    TEST_ASSERT(unit != NULL);
    TEST_ASSERT(unit->id != 0);

    wt_step(&g_test); // process pending ops

    // Reset to MOVE (clears the stack to just MOVE)
    vec2 positions[] = { vec2i(15 * MEGA_TILE_WIDTH, 15 * MEGA_TILE_HEIGHT) };
    WarStateMove* moveState = wst_createMoveState(g_test.context, unit, 1, positions);
    wst_resetState(g_test.context, unit, (WarStateBase*)moveState);
    wt_step(&g_test);
    TEST_ASSERT_EQUAL_INT(wt_activeState(&g_test, unit), WAR_STATE_MOVE);

    // Pop MOVE — the system auto-creates IDLE when the stack becomes empty
    wst_popState(g_test.context, unit);
    wt_step(&g_test);

    TEST_ASSERT_EQUAL_INT(wt_stateDepth(&g_test, unit), 1);
    TEST_ASSERT_EQUAL_INT(wt_activeState(&g_test, unit), WAR_STATE_IDLE);
}

void run_state_machine_tests(void)
{
    UNITY_BEGIN();

    // State Machine Basics
    RUN_TEST(test_create_entity_and_idle_state);
    RUN_TEST(test_push_state_increases_depth);
    RUN_TEST(test_pop_state_decreases_depth);
    RUN_TEST(test_reset_state_clears_stack);
    RUN_TEST(test_stack_contains_query);
    RUN_TEST(test_step_ticks_advances_simulation);
    RUN_TEST(test_empty_stack_becomes_idle_on_pop);

    UNITY_END();
}
