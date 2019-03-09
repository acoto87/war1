WarState* createBuildingState(WarContext* context, WarEntity* entity, WarEntity* entityToBuild, f32 buildTime)
{
    WarState* state = createState(context, entity, WAR_STATE_BUILDING);
    state->building.entityToBuild = entityToBuild;
    state->building.buildTime = 0;
    state->building.totalBuildTime = buildTime;
    state->building.cancelled = false;
    return state;
}

void enterBuildingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarUnitComponent* unit = &entity->unit;

    unit->building = true;
    unit->buildPercent = 0;
}

void leaveBuildingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarUnitComponent* unit = &entity->unit;

    unit->building = false;
}

void updateBuildingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    if (state->building.cancelled)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, false);
            changeNextState(context, entity, idleState, true, true);
        }
        
        return;
    }

    state->building.buildTime += context->deltaTime;

    // if the building is finished...
    if (state->building.buildTime >= state->building.totalBuildTime)
    {
        unit->buildPercent = 100;

        if (state->building.entityToBuild)
        {
            // ...add the entity to the map
            WarEntityListAdd(&map->entities, (WarEntity*)state->building.entityToBuild);

            // ...find an empty position to put it
            vec2 position = getUnitCenterPosition(entity, true);
            vec2 spawnPosition = findEmptyPosition(map->finder, position);
            setUnitCenterPosition(entity, spawnPosition, true);

            // assign null to the field because freeBuildingState will try to free it
            state->building.entityToBuild = NULL;
        }

        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, false);
            changeNextState(context, entity, idleState, true, true);
        }

        return;
    }
        
    unit->buildPercent = percentabf01(state->building.buildTime, state->building.totalBuildTime);
}

void freeBuildingState(WarState* state)
{
    if (state->building.cancelled)
    {
        if (state->building.entityToBuild)
            freeEntity(state->building.entityToBuild);
    }
}