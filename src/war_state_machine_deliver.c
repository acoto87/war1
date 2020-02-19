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

    WarEntity* townHall = findEntity(context, state->deliver.townHallId);

    // if the town hall doesn't exists (or other units attacking it), go idle
    if (!townHall)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    if (!unitInRange(entity, townHall, stats.range))
    {
        vec2 targetTile = unitPointOnTarget(entity, townHall);

        WarState* followState = createFollowState(context, entity, townHall->id, targetTile, stats.range);
        followState->nextState = state;
        changeNextState(context, entity, followState, false, true);
        return;
    }

    if (state->deliver.insideBuilding)
    {
        // find a valid spawn position for the unit
        vec2 position = getUnitCenterPosition(townHall, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(entity, spawnPosition, true);

        WarUnitData unitData = getUnitData(unit->type);
        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, imageResourceRef(unitData.resourceIndex));

        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }

        return;
    }

    if (unit->resourceKind == WAR_RESOURCE_GOLD)
    {
        increasePlayerResources(context, &map->players[0], unit->amount, 0);
    }
    else if (unit->resourceKind == WAR_RESOURCE_WOOD)
    {
        increasePlayerResources(context, &map->players[0], 0, unit->amount);
    }

    unit->resourceKind = WAR_RESOURCE_NONE;
    unit->amount = 0;

    // the unit arrive to the townhall, so now the unit go inside the building for some time to simulate the depositing
    // then need go back to the goldmine/trees.
    state->deliver.insideBuilding = true;

    // for this the sprite is turn off for the depositing time
    entity->sprite.enabled = false;

    // remove the unit from selection to avoid the player giving it orders
    // while inside the townhall
    removeEntityFromSelection(context, entity->id);

    // simulate the time inside the townhall
    setDelay(state, getMapScaledTime(context, 1.0f));
}

void freeDeliverState(WarState* state)
{
}

