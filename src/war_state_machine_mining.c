#include "war_state_machine.h"

#include "TracyC.h"

WarStateMining* wst_createMiningState(WarContext* context, WarEntity* entity, WarEntityId goldmineId)
{
    TracyCZoneN(ctx, "wst_createMiningState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_MINING, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateMining* state = (WarStateMining*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->goldmineId = goldmineId;
    state->miningTime = 0;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterMiningState", true);

    WarStateMining* s = (WarStateMining*)state;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    s->miningTime = 2.0f;

    we_removeSpriteComponent(context, entity);
    wact_resetAction(context, entity, unit->actionType);
    wmap_removeEntityFromSelection(context, entity->id);

    TracyCZoneEnd(ctx);
}

void wst_exitMiningState(WarContext* context, WarEntity* entity, WarState* state, WarStateExitReason reason)
{
    TracyCZoneN(ctx, "wst_exitMiningState", true);

    WarStateMining* s = (WarStateMining*)state;

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(reason);

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

    WarEntity* goldmine = we_findEntity(context, (WarEntityId)s->goldmineId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), or it's collapsing or going to collapse, go idle
    // if the unit was already mining, and the gold mine ran out of gold, then another unit previouly got all the remaining gold
    // so, this unit get nothing
    if (!goldmine)
    {
        vec2 tile = wu_getUnitCenterTile(context, entity);
        vec2 spawnTile = wpath_findEmptyTile(&map->finder, (s32)tile.x, (s32)tile.y);
        wu_setUnitCenterTile(context, entity, spawnTile);

        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
        TracyCZoneEnd(ctx);
        return;
    }

    if (wst_isCollapsing(context, goldmine) || wst_isGoingToCollapse(context, goldmine))
    {
        // find a valid spawn position for the unit
        vec2 tile = wu_getUnitCenterTile(context, goldmine);
        vec2 spawnTile = wpath_findEmptyTile(&map->finder, (s32)tile.x, (s32)tile.y);
        wu_setUnitCenterTile(context, entity, spawnTile);

        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
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
        vec2 tile = wu_getUnitCenterTile(context, goldmine);
        vec2 spawnTile = wpath_findEmptyTile(&map->finder, (s32)tile.x, (s32)tile.y);
        wu_setUnitCenterTile(context, entity, spawnTile);

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
            wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
            TracyCZoneEnd(ctx);
            return;
        }

        WarStateDeliver* deliverState = wst_createDeliverState(context, entity, townHall->id);
        deliverState->cycle = true;
        deliverState->sourceKind = WAR_RESOURCE_GOLD;
        deliverState->sourceId = goldmine->id;
        wst_replaceState(context, entity, (WarStateBase*)deliverState, WAR_TRANSITION_CAUSE_COMPLETION);
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

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateMiningState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
