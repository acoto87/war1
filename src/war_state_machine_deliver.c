WarState* createDeliverState(WarContext* context, WarEntity* entity, WarEntityId townHallId)
{
    WarState* state = createState(context, entity, WAR_STATE_DELIVER);
    state->deliver.townHallId = townHallId;
    return state;
}

void enterDeliverState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void leaveDeliverState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateDeliverState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = getUnitStats(unit->type);

    if (state->deliver.insideBuilding)
    {
        // find a valid spawn position for the unit
        vec2 position = getUnitCenterPosition(entity, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(entity, spawnPosition, true);

        WarUnitsData unitData = getUnitsData(unit->type);
        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, unitData.resourceIndex, 0, NULL);

        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }
        
        return;
    }

    WarEntity* townHall = findEntity(context, state->deliver.townHallId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), go idle
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

    // TODO: deliver goods here to the player!!
    if (unit->resourceKind == WAR_RESOURCE_GOLD)
    {
        map->players[0].gold += unit->amount;
        updateGoldText(context);
    }
    else if (unit->resourceKind == WAR_RESOURCE_WOOD)
    {
        map->players[0].wood += unit->amount;
        updateWoodText(context);
    }

    unit->resourceKind = WAR_RESOURCE_NONE;
    unit->amount = 0;

    // the unit arrive to the townhall, so now the unit go inside the building for some time to simulate the depositing
    // then need go back to the goldmine/trees. 
    state->deliver.insideBuilding = true;

    // for this the sprite is turn off for the depositing time
    entity->sprite.enabled = false;

    setDelay(state, getScaledTime(context, 1.0f));
}

void freeDeliverState(WarState* state)
{
}

