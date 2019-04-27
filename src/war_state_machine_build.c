WarState* createBuildState(WarContext* context, WarEntity* entity, f32 buildTime)
{
    WarState* state = createState(context, entity, WAR_STATE_BUILD);
    state->build.worker = NULL;
    state->build.buildTime = 0;
    state->build.totalBuildTime = buildTime;
    state->build.cancelled = false;
    return state;
}

void enterBuildState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);

    // remove the current sprite...
    removeSpriteComponent(context, entity);

    // ...and add the sprite for the construction of the building
    WarBuildingData buildingData = getBuildingData(entity->unit.type);
    addSpriteComponentFromResource(context, entity, imageResourceRef(buildingData.buildingResource));

    // finally set the action of build on the building
    f32 scale = state->build.totalBuildTime / __frameCountToSeconds(3 * 50);
    setAction(context, entity, WAR_ACTION_TYPE_BUILD, true, scale);

    unit->building = true;
    unit->buildPercent = 0;
}

void leaveBuildState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    unit->building = false;
}

void updateBuildState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    if (state->build.cancelled)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* collapseState = createCollapseState(context, entity);
            changeNextState(context, entity, collapseState, true, true);
        }
        
        return;
    }

    // if there is no worker building the building,
    // don't advance build time
    // if (!state->build.worker)
    // {
    //     return;
    // }

    state->build.buildTime += context->deltaTime;

    // if the building is finished...
    if (state->upgrade.buildTime >= state->upgrade.totalBuildTime)
    {
        unit->buildPercent = 1;

        // ...find an empty position to put it
        vec2 position = getUnitCenterPosition(entity, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(state->build.worker, spawnPosition, true);

        // assign null to the field because freeBuildingState will try to free it
        state->build.worker = NULL;

        // remove the building sprite...
        removeSpriteComponent(context, entity);

        // ...and add the normal sprite of the building
        WarUnitData buildingData = getUnitData(entity->unit.type);
        addSpriteComponentFromResource(context, entity, imageResourceRef(buildingData.resourceIndex));

        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, false);
            changeNextState(context, entity, idleState, true, true);
        }

        return;
    }
    
    unit->buildPercent = percentabf01(state->upgrade.buildTime, state->upgrade.totalBuildTime);
}

void freeBuildState(WarState* state)
{
}