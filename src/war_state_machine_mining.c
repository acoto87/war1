#include "war_state_machine.h"

WarState* createMiningState(WarContext* context, WarEntity* entity, WarEntityId goldmineId)
{
    WarState* state = createState(context, entity, WAR_STATE_MINING);
    state->went_mine.goldmineId = goldmineId;
    state->went_mine.miningTime = 0;
    return state;
}

void enterMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    // disable the sprite to simulate the mining process
    entity->sprite.enabled = false;

    // set the mining time
    state->went_mine.miningTime = 2.0f;

    // remove the unit from selection to avoid the player giving it orders while inside the went_mine
    wmap_removeEntityFromSelection(context, entity->id);
}

void leaveMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void updateMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;

    WarEntity* goldmine = went_findEntity(context, (WarEntityId)state->gold.goldmineId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), go idle
    // if the unit was already mining, and the gold went_mine ran out of gold, then another unit previouly got all the remaining gold
    // so, this unit get nothing
    if (!goldmine || isCollapsing(goldmine) || isGoingToCollapse(goldmine))
    {
        // find a valid spawn position for the unit
        vec2 position = wun_getUnitCenterPosition(goldmine, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        wun_setUnitCenterPosition(entity, spawnPosition, true);

        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    state->went_mine.miningTime -= wmap_getMapScaledSpeed(context, context->deltaTime);

    if (state->went_mine.miningTime < 0)
    {
        unit->amount += went_mine(context, goldmine, UNIT_MAX_CARRY_GOLD);
        if (unit->amount > 0)
        {
            unit->resourceKind = WAR_RESOURCE_GOLD;
        }

        // find a valid spawn position for the unit
        vec2 position = wun_getUnitCenterPosition(goldmine, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        wun_setUnitCenterPosition(entity, spawnPosition, true);

        // set the carrying gold sprites
        WarWorkerData workerData = wun_getWorkerData(unit->type);
        went_removeSpriteComponent(context, entity);
        went_addSpriteComponentFromResource(context, entity, imageResourceRef(workerData.carryingGoldResource));

        // find the closest town hall to wcmd_deliver the gold
        WarRace race = wun_getUnitRace(entity);
        WarUnitType townHallType = wun_getTownHallOfRace(race);
        WarEntity* townHall = went_findClosestUnitOfType(context, entity, townHallType);

        // if the town hall doesn't exists (it could be under wcmd_attack and get destroyed), go idle
        if (!townHall)
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
            return;
        }

        WarState* deliverState = createDeliverState(context, entity, townHall->id);
        deliverState->nextState = createGatherGoldState(context, entity, goldmine->id);
        changeNextState(context, entity, deliverState, true, true);
    }
}

void freeMiningState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}

