WarState* createBuildState(WarContext* context, WarEntity* entity, f32 buildTime)
{
    WarState* state = createState(context, entity, WAR_STATE_BUILD);
    state->build.workerId = 0;
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

    // set the action to NONE because the sprite changes will be handled by this state
    setAction(context, entity, WAR_ACTION_TYPE_NONE, true, 1.0f);

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
    WarPlayerInfo* player = &map->players[0];
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

    // if there is no worker building the building, don't advance build time
    if (state->build.workerId <= 0)
    {
        return;
    }

    f32 buildSpeed = getMapScaledSpeed(context, context->deltaTime);

    // if hurry up cheat is enabled, speed up the build time by 5000%
    if (map->hurryUp)
    {
        buildSpeed *= CHEAT_SPEED_UP_FACTOR;
    }

    state->build.buildTime += buildSpeed;

    // if the building is finished...
    if (state->upgrade.buildTime >= state->upgrade.totalBuildTime)
    {
        unit->buildPercent = 1;

        // find the worker that is building the building
        WarEntity* worker = findEntity(context, state->build.workerId);
        assert(worker);

        // ...find an empty position to put it
        vec2 position = getUnitCenterPosition(entity, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(worker, spawnPosition, true);

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

        WarAudioId audioId = isHumanPlayer(player) ? WAR_HUMAN_WORK_COMPLETE : WAR_ORC_WORK_COMPLETE;
        createAudio(context, audioId, false);

        return;
    }

    unit->buildPercent = percentabf01(state->upgrade.buildTime, state->upgrade.totalBuildTime);

    // update the sprite of the building to show the construction steps
    //
    // NOTE: maybe this could be handled by the BUILD action if I add a `pauseAction`
    // and `resumeAction` functions to be able to pause it or resume it according
    // to the presence of the worker at the construction site
    //
    s32 framesCount = entity->sprite.sprite.framesCount;
    s32 frameIndex = entity->sprite.frameIndex;
    f32 frameIndexStep = 1.0f / framesCount;
    if (unit->buildPercent >= (frameIndex + 1) * frameIndexStep)
    {
        if (frameIndex + 1 < framesCount)
        {
            frameIndex += 1;
        }
    }
    entity->sprite.frameIndex = frameIndex;
}

void freeBuildState(WarState* state)
{
}