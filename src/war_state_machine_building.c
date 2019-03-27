WarState* createBuildingUnitState(WarContext* context, WarEntity* entity, WarEntity* entityToBuild, f32 buildTime)
{
    WarState* state = createState(context, entity, WAR_STATE_BUILDING);
    state->building.entityToBuild = entityToBuild;
    state->building.buildTime = 0;
    state->building.totalBuildTime = buildTime;
    state->building.cancelled = false;
    return state;
}

WarState* createBuildingUpgradeState(WarContext* context, WarEntity* entity, WarUpgradeType upgradeToBuild, f32 buildTime)
{
    WarState* state = createState(context, entity, WAR_STATE_BUILDING);
    state->building.upgradeToBuild = upgradeToBuild;
    state->building.buildTime = 0;
    state->building.totalBuildTime = buildTime;
    state->building.cancelled = false;
    return state;
}

void enterBuildingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);

    unit->building = true;
    unit->buildPercent = 0;
}

void leaveBuildingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    unit->building = false;
}

void updateBuildingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
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
        unit->buildPercent = 1;

        if (state->building.entityToBuild)
        {
            // ...add the entity to the map
            WarEntityListAdd(&map->entities, state->building.entityToBuild);

            // ...find an empty position to put it
            vec2 position = getUnitCenterPosition(entity, true);
            vec2 spawnPosition = findEmptyPosition(map->finder, position);
            setUnitCenterPosition(state->building.entityToBuild, spawnPosition, true);

            // assign null to the field because freeBuildingState will try to free it
            state->building.entityToBuild = NULL;
        }
        else
        {
            increaseUpgradeLevel(player, state->building.upgradeToBuild);
            assert(checkUpgradeLevel(player, state->building.upgradeToBuild));
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