#include "war_state_machine.h"

#include "TracyC.h"

WarStateMining* wst_createMiningState(WarContext* context, WarEntity* entity, WarEntityId goldmineId)
{
    TracyCZoneN(ctx, "wst_createMiningState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_MINING, entity->id);
    WarStateMining* state = (WarStateMining*)wst_deref(context, ref);
    state->goldmineId = goldmineId;
    state->miningTime = 0;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveMiningState", true);

    if (!state->initialized)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    WarStateMining* s = (WarStateMining*)state;

    NOT_USED(context);
    NOT_USED(entity);

    WarEntity* goldmine = we_findEntity(context, (WarEntityId)s->goldmineId);

    // NOTE: if the goldmine doesn't exists (it could ran out of gold, or other units destroyed it), or it's collapsing or going to collapse,
    // restore back the sprite component to the worker, so it should be visible again
    if (!goldmine || wst_isCollapsing(context, goldmine) || wst_isGoingToCollapse(context, goldmine))
    {
        WarUnitComponent* unit = we_getUnitComponent(context, entity);
        assert(unit);

        const WarUnitData* unitData = wu_getUnitData(unit->type);
        s32 spriteIndex = unitData->resourceIndex;
        if (spriteIndex == 0)
        {
            logError("Sprite for unit of type %d is not configure properly. Default to footman sprite.", unit->type);
            spriteIndex = 279;
        }
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(spriteIndex));
    }

    TracyCZoneEnd(ctx);
}

void wst_updateMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateMiningState", true);

    WarStateMining* s = (WarStateMining*)state;

    WarMap* map = context->map;
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (!state->initialized)
    {
        // remove the sprite to simulate the mining process
        we_removeSpriteComponent(context, entity);

        // reset the current action to stop any movement or attack action
        wact_resetAction(context, entity, unit->actionType);

        // set the mining time
        s->miningTime = 2.0f;

        // remove the unit from selection to avoid the player giving it orders while inside the mine
        wmap_removeEntityFromSelection(context, entity->id);

        state->initialized = true;
        TracyCZoneEnd(ctx);
        return;
    }

    WarEntity* goldmine = we_findEntity(context, (WarEntityId)s->goldmineId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), or it's collapsing or going to collapse, go idle
    // if the unit was already mining, and the gold mine ran out of gold, then another unit previouly got all the remaining gold
    // so, this unit get nothing
    if (!goldmine)
    {
        // find a valid spawn position for the unit
        vec2 position = wu_getUnitCenterPosition(context, entity, true);
        vec2 spawnPosition = wpath_findEmptyPosition(&map->finder, position);
        wu_setUnitCenterPosition(context, entity, spawnPosition, true);

        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState);
        TracyCZoneEnd(ctx);
        return;
    }

    if (wst_isCollapsing(context, goldmine) || wst_isGoingToCollapse(context, goldmine))
    {
        // find a valid spawn position for the unit
        vec2 position = wu_getUnitCenterPosition(context, goldmine, true);
        vec2 spawnPosition = wpath_findEmptyPosition(&map->finder, position);
        wu_setUnitCenterPosition(context, entity, spawnPosition, true);

        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState);
        TracyCZoneEnd(ctx);
        return;
    }

    s->miningTime -= context->gameDeltaTime;

    if (s->miningTime < 0)
    {
        unit->amount += mine(context, goldmine, UNIT_MAX_CARRY_GOLD);
        if (unit->amount > 0)
        {
            unit->resourceKind = WAR_RESOURCE_GOLD;
        }

        // find a valid spawn position for the unit
        vec2 position = wu_getUnitCenterPosition(context, goldmine, true);
        vec2 spawnPosition = wpath_findEmptyPosition(&map->finder, position);
        wu_setUnitCenterPosition(context, entity, spawnPosition, true);

        // set the carrying gold sprites
        const WarWorkerData* workerData = wu_getWorkerData(unit->type);
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(workerData->carryingGoldResource));

        // find the closest town hall to deliver the gold
        WarRace race = wu_getUnitRace(context, entity);
        WarUnitType townHallType = wu_getTownHallOfRace(race);
        WarEntity* townHall = we_findClosestUnitOfType(context, entity, townHallType);

        // if the town hall doesn't exists (it could be under attack and get destroyed), go idle
        if (!townHall)
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_replaceState(context, entity, (WarStateBase*)idleState);
            TracyCZoneEnd(ctx);
            return;
        }

        WarStateDeliver* deliverState = wst_createDeliverState(context, entity, townHall->id);
        deliverState->cycle = true;
        deliverState->sourceKind = WAR_RESOURCE_GOLD;
        deliverState->sourceId = goldmine->id;
        wst_replaceState(context, entity, (WarStateBase*)deliverState);
    }

    TracyCZoneEnd(ctx);
}


void wst_updateMiningStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateMiningStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateMining*      states  = storage->mining;
    bool*             occupied = storage->occupied[WAR_STATE_MINING];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateMining*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->depth == 0 || sm->stack[sm->depth - 1].type != WAR_STATE_MINING || sm->stack[sm->depth - 1].idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateMiningState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
