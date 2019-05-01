WarState* createRepairingState(WarContext* context, WarEntity* entity, WarEntityId buildingId)
{
    WarState* state = createState(context, entity, WAR_STATE_REPAIRING);
    state->repairing.buildingId = buildingId;
    return state;
}

void enterRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarEntity* building = findEntity(context, state->repairing.buildingId);
    assert(isBuildingUnit(building));

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || isCollapsing(building) || isGoingToCollapse(building))
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);

        return;
    }

    // if the building needs to be built, enter the building and build it
    if (isBuilding(building) || isGoingToBuild(building))
    {
        // disable the sprite to simulate the building process
        entity->sprite.enabled = false;

        // set the unit as inside the building
        state->repairing.insideBuilding = true;

        WarState* buildState = getBuildState(building);
        assert(buildState);

        buildState->build.workerId = entity->id;
    }
    else
    {
        state->repairing.repairTime = context->time;
        setAction(context, entity, WAR_ACTION_TYPE_REPAIR, true, 1.0f);
    }
}

void leaveRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarEntity* building = findEntity(context, state->repairing.buildingId);
    assert(isBuildingUnit(building));

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || isCollapsing(building) || isGoingToCollapse(building))
    {
        if (state->repairing.insideBuilding)
        {
            // find a valid spawn position for the unit
            vec2 position = getUnitCenterPosition(entity, true);
            vec2 spawnPosition = findEmptyPosition(map->finder, position);
            setUnitCenterPosition(entity, spawnPosition, true);
        }

        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);

        return;
    }

    // if the worker is inside the building then he is building it
    // so don't make anything since new buildings always spawn with full hp
    if (!state->repairing.insideBuilding)
    {
        if (context->time - state->repairing.repairTime >= 1.0f)
        {
            if (!withdrawFromPlayer(context, player, 1, 1))
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);
                return;
            }

            // to calculate the amount of wood and gold needed to repair a 
            // building I'm taking the 12% of the damage of the building,
            // so for the a FARM if it has a damage of 200, the amount of 
            // wood and gold would be 200 * 0.12 = 24.
            //
            // when repairing each second the amount of wood and gold decrease
            // in 1, so for each we need to increase the hp in the proportional
            // amount, in this case is 1 * 100 / 12 = 8.33 (rounding to 9 here)
            building->unit.hp += 9;

            if (building->unit.hp >= building->unit.maxhp)
            {
                building->unit.hp = building->unit.maxhp;

                if (state->repairing.insideBuilding)
                {
                    // find a valid spawn position for the unit
                    vec2 position = getUnitCenterPosition(entity, true);
                    vec2 spawnPosition = findEmptyPosition(map->finder, position);
                    setUnitCenterPosition(entity, spawnPosition, true);
                }

                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);
            }

            state->repairing.repairTime = context->time;
        }
    }
    else if (!isBuilding(building) && !isGoingToBuild(building))
    {
        entity->sprite.enabled = true;

        // find a valid spawn position for the unit
        vec2 position = getUnitCenterPosition(entity, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(entity, spawnPosition, true);

        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
    }
}

void freeRepairingState(WarState* state)
{
}

