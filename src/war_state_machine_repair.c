WarState* createRepairState(WarContext* context, WarEntity* entity, WarEntityId buildingId)
{
    WarState* state = createState(context, entity, WAR_STATE_REPAIR);
    state->repair.buildingId = buildingId;
    return state;
}

void enterRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void leaveRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = getUnitStats(unit->type);

    WarEntity* building = findEntity(context, state->repair.buildingId);
    assert(isBuildingUnit(building));

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || isCollapsing(building) || isGoingToCollapse(building))
    {
        // find a valid spawn position for the unit
        vec2 position = getUnitCenterPosition(entity, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(entity, spawnPosition, true);

        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    // if the building is not in range, go to it
    if (!unitInRange(entity, building, stats.range))
    {
        WarState* followState = createFollowState(context, entity, building->id, stats.range);
        followState->nextState = state;
        changeNextState(context, entity, followState, false, true);
        return;
    }

    // the unit arrive to the building, go repairing
    WarState* repairingState = createRepairingState(context, entity, building->id);
    changeNextState(context, entity, repairingState, true, true);
}

void freeRepairState(WarState* state)
{
}

