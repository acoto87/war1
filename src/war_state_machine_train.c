WarState* createTrainState(WarContext* context, WarEntity* entity, WarEntity* entityToBuild, f32 buildTime)
{
    WarState* state = createState(context, entity, WAR_STATE_TRAIN);
    state->train.entityToBuild = entityToBuild;
    state->train.buildTime = 0;
    state->train.totalBuildTime = buildTime;
    state->train.cancelled = false;
    return state;
}

void enterTrainState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);

    unit->building = true;
    unit->buildPercent = 0;
}

void leaveTrainState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    unit->building = false;
}

void updateTrainState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    if (state->train.cancelled)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, false);
            changeNextState(context, entity, idleState, true, true);
        }
        
        return;
    }

    state->train.buildTime += context->deltaTime;

    // if the building is finished...
    if (state->train.buildTime >= state->train.totalBuildTime)
    {
        unit->buildPercent = 1;

        // ...add the entity to the map
        WarEntityListAdd(&map->entities, state->train.entityToBuild);

        // ...find an empty position to put it
        vec2 position = getUnitCenterPosition(entity, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(state->train.entityToBuild, spawnPosition, true);

        // assign null to the field because freeBuildingState will try to free it
        state->train.entityToBuild = NULL;

        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, false);
            changeNextState(context, entity, idleState, true, true);
        }

        return;
    }
        
    unit->buildPercent = percentabf01(state->train.buildTime, state->train.totalBuildTime);
}

void freeTrainState(WarState* state)
{
    if (state->train.cancelled)
    {
        if (state->train.entityToBuild)
            freeEntity(state->train.entityToBuild);
    }
}