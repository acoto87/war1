WarState* createRepairingState(WarContext* context, WarEntity* entity, WarEntityId buildingId)
{
    WarState* state = createState(context, entity, WAR_STATE_REPAIRING);
    state->repairing.buildingId = buildingId;
    state->repairing.insideBuilding = false;
    return state;
}

void enterRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarEntity* building = findEntity(context, state->repairing.buildingId);

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
        WarState* buildState = getBuildState(building);
        assert(buildState);

        // if there is already someone building it, go idle
        if (buildState->build.workerId)
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);

            return;
        }

        // disable the sprite to simulate the building process
        entity->sprite.enabled = false;

        // set the unit as inside the building
        state->repairing.insideBuilding = true;

        // set up that this worker is the one building the building
        buildState->build.workerId = entity->id;
    }
    else
    {
        vec2 unitSize = getUnitSize(entity);
        vec2 position = getUnitCenterPosition(entity, true);
        vec2 targetPosition = getUnitCenterPosition(building, true);

        setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
        setUnitDirectionFromDiff(entity, targetPosition.x - position.x, targetPosition.y - position.y);
        setAction(context, entity, WAR_ACTION_TYPE_REPAIR, true, 1.0f);
    }
}

void leaveRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = getUnitCenterPosition(entity, true);
    setFreeTiles(map->finder, position.x, position.y, unitSize.x, unitSize.y);
}

void updateRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    WarUnitComponent* unit = &entity->unit;

    WarEntity* building = findEntity(context, state->repairing.buildingId);

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || isCollapsing(building) || isGoingToCollapse(building))
    {
        if (state->repairing.insideBuilding)
        {
            entity->sprite.enabled = true;

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
    // so don't make any repairing since new buildings always spawn with full hp
    if (!state->repairing.insideBuilding)
    {
        WarUnitAction* action = unit->actions.items[unit->actionIndex];
        if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
        {
            if (!decreasePlayerResource(context, player, WAR_RESOURCE_GOLD, 1) ||
                !decreasePlayerResource(context, player, WAR_RESOURCE_WOOD, 1))
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

                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);
            }

            action->lastActionStep = WAR_ACTION_STEP_NONE;
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

