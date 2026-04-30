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
    // disable the sprite to simulate the mining process
    entity->sprite.enabled = false;

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
}

void wst_updateMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;

    WarEntity* goldmine = we_findEntity(context, (WarEntityId)state->gold.goldmineId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), go idle
    // if the unit was already mining, and the gold mine ran out of gold, then another unit previouly got all the remaining gold
    // so, this unit get nothing
    if (!goldmine || isCollapsing(goldmine) || isGoingToCollapse(goldmine))
    {
        // find a valid spawn position for the unit
        vec2 position = wu_getUnitCenterPosition(goldmine, true);
        vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);
        wu_setUnitCenterPosition(entity, spawnPosition, true);

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
        vec2 position = wu_getUnitCenterPosition(goldmine, true);
        vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);
        wu_setUnitCenterPosition(entity, spawnPosition, true);

        // set the carrying gold sprites
        WarWorkerData workerData = wu_getWorkerData(unit->type);
        we_removeSpriteComponent(context, entity);
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(workerData.carryingGoldResource));

        // find the closest town hall to deliver the gold
        WarRace race = wu_getUnitRace(entity);
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

