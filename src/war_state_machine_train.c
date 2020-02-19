WarState* createTrainState(WarContext* context, WarEntity* entity, WarUnitType unitToBuild, f32 buildTime)
{
    WarState* state = createState(context, entity, WAR_STATE_TRAIN);
    state->train.unitToBuild = unitToBuild;
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

    f32 trainSpeed = getMapScaledSpeed(context, context->deltaTime);

    // if hurry up cheat is enabled, speed up the train time by 5000%
    if (map->hurryUp)
    {
        trainSpeed *= CHEAT_SPEED_UP_FACTOR;
    }

    state->train.buildTime += trainSpeed;

    // if the building is finished...
    if (state->train.buildTime >= state->train.totalBuildTime)
    {
        unit->buildPercent = 1;

        // ...create the unit
        WarEntity* unitToBuild = createDude(context, state->train.unitToBuild, 0, 0, 0, false);

        // ...find an empty position to put it
        vec2 position = getUnitCenterPosition(entity, true);
        vec2 spawnPosition = findEmptyPosition(map->finder, position);
        setUnitCenterPosition(unitToBuild, spawnPosition, true);

        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, false);
            changeNextState(context, entity, idleState, true, true);
        }

        WarAudioId audioId = isHumanUnit(unitToBuild) ? WAR_HUMAN_READY : WAR_ORC_READY;
        createAudio(context, audioId, false);

        return;
    }

    unit->buildPercent = percentabf01(state->train.buildTime, state->train.totalBuildTime);
}

void freeTrainState(WarState* state)
{
}