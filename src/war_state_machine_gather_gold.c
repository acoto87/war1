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

    // this state enable and disable the sprite component to simulate the mining time in the goldmine
    // so enable it by default for the walk part, and let the process of updating the state disable it if needed
    entity->sprite.enabled = true;

    // by default, the worker unit isn't inside a building
    state->gold.insideBuilding = false;

    // if the state doesn't know where to go next, go idle
    if (state->gold.direction == 0)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    WarEntity* targetEntity = findEntity(context, state->gold.targetEntityId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), go idle
    if (!targetEntity)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    // if the worker is going towards the goldmine
    if (state->gold.direction > 0)
    {
        // if the goldmine is not in range, go to it
        if (!unitInRange(entity, targetEntity, stats.range))
        {
            WarState* followState = createFollowState(context, entity, state->gold.targetEntityId, stats.range);
            followState->nextState = state;
            changeNextState(context, entity, followState, false, true);
            return;
        }

        // the unit arrive to the goldmine, so now the unit go to wait state for some time to simulate the mining
        // then need go back to the townhall. for this the sprite of the unit need to be changed to the one with carrying gold.
        state->gold.direction = -1;
        state->gold.insideBuilding = true;

        // change the sprite of the worker
        WarWorkerData workerData = getWorkerData(unit->type);
        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, workerData.carryingGoldResource);
        entity->sprite.enabled = false;

        setDelay(state, 5.0f);
    }
    else
    {
        vec2 position = getUnitCenterPosition(entity, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(entity, spawnPosition, true);

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

        if (!unitInRange(entity, townHall, stats.range))
        {
            WarState* followState = createFollowState(context, entity, townHall->id, stats.range);
            followState->nextState = state;
            changeNextState(context, entity, followState, false, true);
            return;
        }

        // TODO: deliver gold here to the player!!
        logDebug("gold delivered!");

        // the unit arrive to the townhall, so now the unit go to wait state for some time to simulate the depositing
        // then need go back to the goldmine. for this the sprite of the unit need to be changed to the normal one.
        state->gold.direction = 1;
        state->gold.insideBuilding = true;

        WarUnitsData unitData = getUnitsData(unit->type);
        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, unitData.resourceIndex);
        entity->sprite.enabled = false;

        setDelay(state, 1.0f);
    }
}

void freeGatherGoldState(WarState* state)
{
}

