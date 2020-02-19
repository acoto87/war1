WarState* createMiningState(WarContext* context, WarEntity* entity, WarEntityId goldmineId)
{
    WarState* state = createState(context, entity, WAR_STATE_MINING);
    state->mine.goldmineId = goldmineId;
    state->mine.miningTime = 0;
    return state;
}

void enterMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    // disable the sprite to simulate the mining process
    entity->sprite.enabled = false;

    // set the mining time
    state->mine.miningTime = 2.0f;

    // remove the unit from selection to avoid the player giving it orders while inside the mine
    removeEntityFromSelection(context, entity->id);
}

void leaveMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateMiningState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;

    WarEntity* goldmine = findEntity(context, state->gold.goldmineId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), go idle
    // if the unit was already mining, and the gold mine ran out of gold, then another unit previouly got all the remaining gold
    // so, this unit get nothing
    if (!goldmine || isCollapsing(goldmine) || isGoingToCollapse(goldmine))
    {
        // find a valid spawn position for the unit
        vec2 position = getUnitCenterPosition(goldmine, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(entity, spawnPosition, true);

        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    state->mine.miningTime -= getMapScaledSpeed(context, context->deltaTime);

    if (state->mine.miningTime < 0)
    {
        unit->amount += mine(context, goldmine, UNIT_MAX_CARRY_GOLD);
        if (unit->amount > 0)
        {
            unit->resourceKind = WAR_RESOURCE_GOLD;
        }

        // find a valid spawn position for the unit
        vec2 position = getUnitCenterPosition(goldmine, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(entity, spawnPosition, true);

        // set the carrying gold sprites
        WarWorkerData workerData = getWorkerData(unit->type);
        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, imageResourceRef(workerData.carryingGoldResource));

        // find the closest town hall to deliver the gold
        WarRace race = getUnitRace(entity);
        WarUnitType townHallType = getTownHallOfRace(race);
        WarEntity* townHall = findClosestUnitOfType(context, entity, townHallType);

        // if the town hall doesn't exists (it could be under attack and get destroyed), go idle
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

void freeMiningState(WarState* state)
{
}

