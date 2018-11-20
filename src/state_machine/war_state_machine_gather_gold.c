#include "../war_state_machine.h"

WarState* createGatherGoldState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId)
{
    WarState* state = createState(context, entity, WAR_STATE_GOLD);
    state->gold.targetEntityId = targetEntityId;
    state->gold.direction = 1;
    return state;
}

void enterGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void leaveGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = getUnitStats(unit->type);

    WarEntity* targetEntity = findEntity(context, state->gold.targetEntityId);

    // if the goldmine doesn't exists (it could ran out of gold), go idle
    if (!targetEntity)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    // if the state doesn't know where to go next, go idle
    if (state->gold.direction == 0)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    // if the worker is going towards the goldmine
    if (state->gold.direction > 0)
    {
        if (!unitInRange(entity, targetEntity, stats.range))
        {
            WarState* followState = createFollowState(context, entity, state->gold.targetEntityId, stats.range);
            followState->nextState = state;
            changeNextState(context, entity, followState, false, true);
            return;
        }

        // put the worker with the carrying resources sprite
        WarWorkerData workerData = getWorkerData(unit->type);
        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, workerData.carryingGoldResource);
        entity->sprite.enabled = false;

        state->gold.direction = -1;

        WarState* waitState = createWaitState(context, entity, 5);
        waitState->nextState = state;
        changeNextState(context, entity, waitState, false, true);
    }
    else
    {
        // TODO: search here an empty position around the goldmine to spawn the entity
        // this will be similar to when and unit is done training
        
        entity->sprite.enabled = true;

        // find the closest town hall to deliver the gold
        WarRace race = getUnitRace(entity);
        WarUnitType townHallType = getTownHallOfRace(race);
        WarEntity* townHall = findClosestUnitOfType(context, entity, townHallType);

        // if the town hall doesn't exists (it could be under attack), go idle
        if (!townHall)
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
            return;
        }

        if (!unitInRange(entity, townHall, stats.range))
        {
            WarState* followState = createFollowState(context, entity, townHall->id, stats.range);
            followState->nextState = state;
            changeNextState(context, entity, followState, false, true);
            return;
        }

        // DELIVER GOLD HERE!!

        WarUnitsData unitData = getUnitsData(unit->type);
        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, unitData.resourceIndex);

        state->gold.direction = 1;
    }
}

void freeGatherGoldState(WarState* state)
{
}

