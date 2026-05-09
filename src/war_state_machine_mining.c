#include "war_state_machine.h"

WarState* wst_createMiningState(WarContext* context, WarEntity* entity, WarEntityId goldmineId)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_MINING);
    state->mine.goldmineId = goldmineId;
    state->mine.miningTime = 0;
    return state;
}

void wst_enterMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    // remove the sprite to simulate the mining process
    we_removeSpriteComponent(context, entity);

    // set the mining time
    state->mine.miningTime = 2.0f;

    // remove the unit from selection to avoid the player giving it orders while inside the mine
    wmap_removeEntityFromSelection(context, entity->id);
}

void wst_leaveMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    WarEntity* goldmine = we_findEntity(context, (WarEntityId)state->mine.goldmineId);

    // NOTE: if the goldmine doesn't exists (it could ran out of gold, or other units destroyed it), or it's collapsing or going to collapse,
    // restore back the sprite component to the worker, so it should be visible again
    if (!goldmine || isCollapsing(goldmine) || isGoingToCollapse(goldmine))
    {
        WarUnitComponent* unit = we_getUnitComponent(context, entity);
        assert(unit);

        WarUnitData unitData = wu_getUnitData(unit->type);

        s32 spriteIndex = unitData.resourceIndex;
        if (spriteIndex == 0)
        {
            logError("Sprite for unit of type %d is not configure properly. Default to footman sprite.", unit->type);
            spriteIndex = 279;
        }
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(spriteIndex));
    }
}

void wst_updateMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    WarEntity* goldmine = we_findEntity(context, (WarEntityId)state->mine.goldmineId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), or it's collapsing or going to collapse, go idle
    // if the unit was already mining, and the gold mine ran out of gold, then another unit previouly got all the remaining gold
    // so, this unit get nothing
    if (!goldmine)
    {
        // find a valid spawn position for the unit
        vec2 position = wu_getUnitCenterPosition(context, entity, true);
        vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);
        wu_setUnitCenterPosition(context, entity, spawnPosition, true);

        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
        return;
    }

    if (isCollapsing(goldmine) || isGoingToCollapse(goldmine))
    {
        // find a valid spawn position for the unit
        vec2 position = wu_getUnitCenterPosition(context, goldmine, true);
        vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);
        wu_setUnitCenterPosition(context, entity, spawnPosition, true);

        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
        return;
    }

    state->mine.miningTime -= wmap_getMapScaledSpeed(context, context->deltaTime);

    if (state->mine.miningTime < 0)
    {
        unit->amount += mine(context, goldmine, UNIT_MAX_CARRY_GOLD);
        if (unit->amount > 0)
        {
            unit->resourceKind = WAR_RESOURCE_GOLD;
        }

        // find a valid spawn position for the unit
        vec2 position = wu_getUnitCenterPosition(context, goldmine, true);
        vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);
        wu_setUnitCenterPosition(context, entity, spawnPosition, true);

        // set the carrying gold sprites
        WarWorkerData workerData = wu_getWorkerData(unit->type);
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(workerData.carryingGoldResource));

        // find the closest town hall to deliver the gold
        WarRace race = wu_getUnitRace(context, entity);
        WarUnitType townHallType = wu_getTownHallOfRace(race);
        WarEntity* townHall = we_findClosestUnitOfType(context, entity, townHallType);

        // if the town hall doesn't exists (it could be under attack and get destroyed), go idle
        if (!townHall)
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
            return;
        }

        WarState* deliverState = wst_createDeliverState(context, entity, townHall->id);
        deliverState->nextState = wst_createGatherGoldState(context, entity, goldmine->id);
        wst_changeNextState(context, entity, deliverState, true, true);
    }
}

void wst_freeMiningState(WarContext* context, WarState* state)
{
    NOT_USED(context);
    NOT_USED(state);
}

