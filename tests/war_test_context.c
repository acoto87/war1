#include "war_test_context.h"

#include <string.h>

#include "shl/memzone.h"

#include "war_fwd.h"
#include "war_alloc.h"
#include "war_actions.h"
#include "war_entities.h"
#include "war_map.h"
#include "war_map_grid.h"
#include "war_pathfinder.h"
#include "war_state_machine.h"
#include "war_units.h"

#define WT_PERM_SIZE  (536870912ULL)  // 512 MB
#define WT_FRAME_SIZE  (67108864ULL)  // 64 MB
#define WT_AUDIO_SIZE  (33554432ULL)  // 32 MB

void wt_init(WarTestContext* test)
{
    memset(test, 0, sizeof(*test));

    if (!wm_allocInit(WT_PERM_SIZE, WT_FRAME_SIZE, WT_AUDIO_SIZE))
    {
        logError("wt_init: failed to initialize memory allocators");
        return;
    }

    test->fixedDeltaTime = WAR_TEST_DELTA_TIME;
    test->simulationTick = 0;

    WarContext* context = (WarContext*)wm_alloc(sizeof(WarContext));
    if (!context)
    {
        logError("wt_init: failed to allocate WarContext");
        wm_allocFree();
        return;
    }
    memset(context, 0, sizeof(WarContext));
    test->context = context;

    context->globalSpeed = 1.0f;
    context->globalScale = 1.0f;
    context->gameTime = 0.0;
    context->gameDeltaTime = 0.0f;
    context->audioEnabled = false;
    context->musicEnabled = false;
    context->soundEnabled = false;

    WarMap* map = (WarMap*)wm_alloc(sizeof(WarMap));
    if (!map)
    {
        logError("wt_init: failed to allocate WarMap");
        wm_free(context);
        wm_allocFree();
        return;
    }
    memset(map, 0, sizeof(WarMap));
    test->map = map;
    context->map = map;

    we_initEntityManager(context, &map->entityManager);

    WarEntityIdListInit(&map->selectedEntities, wm_globalAllocator());
    for (s32 i = 0; i < MAX_SELECTION_GROUPS; i++)
        WarEntityIdListInit(&map->selectionGroups[i], wm_globalAllocator());

    // Initialize pathfinder with all-passable data (0 = empty = passable)
    u16 passableData[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
    memset(passableData, 0, sizeof(passableData));
    map->finder = wpath_initPathFinder(passableData);

    // Initialize players
    for (s32 i = 0; i < MAX_PLAYERS_COUNT; i++)
    {
        map->players[i].index = (u8)i;
        map->players[i].race = (i == 0) ? WAR_RACE_HUMANS : WAR_RACE_ORCS;
        map->players[i].gold = 5000;
        map->players[i].wood = 5000;
    }

    map->playing = true;
    map->fowEnabled = false;

    wact_initUnitActionDefs();
}

void wt_shutdown(WarTestContext* test)
{
    if (test->map)
    {
        // Entity manager cleanup is handled by map free; zero out for safety
        memset(test->map, 0, sizeof(WarMap));
        wm_free(test->map);
        test->map = NULL;
    }

    if (test->context)
    {
        test->context->map = NULL;
        wm_free(test->context);
        test->context = NULL;
    }

    wm_allocFree();
}

void wt_updateGameTime(WarTestContext* test)
{
    WarContext* context = test->context;
    context->gameDeltaTime = test->fixedDeltaTime;
    context->gameTime += (f64)test->fixedDeltaTime;
}

void wt_updateStateMachines(WarTestContext* test)
{
    WarContext* context = test->context;

    wt_buildGrid(test);

    wst_updateIdleStates(context);
    wst_updateMoveStates(context);
    wst_updatePatrolStates(context);
    wst_updateFollowStates(context);
    wst_updateAttackStates(context);
    wst_updateGoldStates(context);
    wst_updateMiningStates(context);
    wst_updateWoodStates(context);
    wst_updateChoppingStates(context);
    wst_updateDeliverStates(context);
    wst_updateDeathStates(context);
    wst_updateCollapseStates(context);
    wst_updateTrainStates(context);
    wst_updateUpgradeStates(context);
    wst_updateBuildStates(context);
    wst_updateRepairStates(context);
    wst_updateRepairingStates(context);
    wst_updateCastStates(context);
    wst_updateWaitStates(context);
}

void wt_applyPendingTransitions(WarTestContext* test)
{
    WarContext* context = test->context;
    wst_processStateMachinePendingOps(context);
}

void wt_advanceTick(WarTestContext* test)
{
    if (frameZone) mz_reset(frameZone);
    test->simulationTick++;
}

void wt_setUnitCenterPosition(WarTestContext* test, WarEntity* unit, vec2 position)
{
    WarContext* context = test->context;
    wu_setUnitCenterPosition(context, unit, position);
}

void wt_setUnitVelocity(WarTestContext* test, WarEntity* unit, vec2 velocity)
{
    WarContext* context = test->context;
    WarStateMove* moveState = wst_getMoveState(context, unit);
    if (!moveState) return;
    moveState->rvoPreferredVelocity = velocity;
    moveState->rvoAdjustedVelocity = velocity;
    moveState->rvoVelocity = velocity;
}

void wt_buildGrid(WarTestContext* test)
{
    WarContext* context = test->context;
    wgrid_build(context);
}

void wt_step(WarTestContext* test)
{
    wt_updateGameTime(test);
    wt_applyPendingTransitions(test);
    wt_updateStateMachines(test);
    wt_buildGrid(test);
    wt_advanceTick(test);
}

void wt_stepTicks(WarTestContext* test, u32 ticks)
{
    for (u32 i = 0; i < ticks; i++)
        wt_step(test);
}

WarEntity* wt_spawnUnit(WarTestContext* test, WarUnitType type, s32 player, vec2 position)
{
    WarContext* context = test->context;
    WarMap* map = test->map;

    const WarUnitData* unitData = wu_getUnitData(type);
    assert(unitData);

    s32 tilex = (s32)(position.x / MEGA_TILE_WIDTH);
    s32 tiley = (s32)(position.y / MEGA_TILE_HEIGHT);

    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_UNIT, true);
    if (!entity) return NULL;

    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = position
    ));
    we_addUnitComponent(context, entity, WAR_UNIT_COMPONENT_INIT(
        .type      = type,
        .direction = WAR_DIRECTION_SOUTH,
        .tilex     = tilex,
        .tiley     = tiley,
        .sizex     = unitData->sizex,
        .sizey     = unitData->sizey,
        .player    = (u8)player,
    ));

    wact_addUnitActions(context, entity);
    we_addAnimationsComponent(context, entity);
    we_addStateMachineComponent(context, entity);

    if (wu_isDudeUnitType(type))
    {
        const WarUnitStats* stats = wu_getUnitStats(type);
        WarUnitComponent* uc = we_getUnitComponent(context, entity);
        assert(uc);
        uc->maxhp      = stats->hp;
        uc->hp         = stats->hp;
        uc->maxMana    = stats->mana;
        uc->mana       = 100;
        uc->armor      = stats->armor;
        uc->range      = stats->range;
        uc->minDamage  = stats->minDamage;
        uc->rndDamage  = stats->rndDamage;
        uc->decay      = stats->decay;
        uc->manaTime   = 1;
        uc->speed      = 0;
    }
    else if (wu_isBuildingUnitType(type))
    {
        const WarBuildingStats* stats = wu_getBuildingStats(type);
        WarUnitComponent* uc = we_getUnitComponent(context, entity);
        assert(uc);
        uc->maxhp = stats->hp;
        uc->hp    = stats->hp;
        uc->armor = stats->armor;
    }

    // Register in the units-by-type index
    WarEntityList* list = WarUnitMapGet(&map->entityManager.unitsByType, type);
    if (list)
        WarEntityListAdd(list, entity);

    // Set initial idle state
    we_setInitialIdleState(context, entity);

    return entity;
}

WarEntity* wt_spawnBuilding(WarTestContext* test, WarUnitType type, s32 player, vec2 position)
{
    return wt_spawnUnit(test, type, player, position);
}

u8 wt_stateDepth(WarTestContext* test, WarEntity* entity)
{
    WarStateMachineComponent* sm = we_getStateMachineComponent(test->context, entity);
    assert(sm);
    return sm->depth;
}

WarStateType wt_activeState(WarTestContext* test, WarEntity* entity)
{
    WarStateMachineComponent* sm = we_getStateMachineComponent(test->context, entity);
    assert(sm);
    if (sm->depth == 0)
        return INVALID_STATE_TYPE;
    return sm->stack[sm->depth - 1].type;
}

WarStateType wt_stateAt(WarTestContext* test, WarEntity* entity, u8 stackIndex)
{
    WarStateMachineComponent* sm = we_getStateMachineComponent(test->context, entity);
    assert(sm);
    if (stackIndex >= sm->depth)
        return INVALID_STATE_TYPE;
    return sm->stack[stackIndex].type;
}

bool wt_stackContains(WarTestContext* test, WarEntity* entity, WarStateType type)
{
    return wst_hasStateInStack(test->context, entity, type);
}

WarTransitionRequest* wt_activeTransition(WarTestContext* test, WarEntity* entity)
{
    WarStateMachineComponent* sm = we_getStateMachineComponent(test->context, entity);
    assert(sm);
    return &sm->pending;
}
