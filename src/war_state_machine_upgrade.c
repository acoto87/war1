WarState* createUpgradeState(WarContext* context, WarEntity* entity, WarUpgradeType upgradeToBuild, f32 buildTime)
{
    WarState* state = createState(context, entity, WAR_STATE_UPGRADE);
    state->upgrade.upgradeToBuild = upgradeToBuild;
    state->upgrade.buildTime = 0;
    state->upgrade.totalBuildTime = buildTime;
    state->upgrade.cancelled = false;
    return state;
}

void enterUpgradeState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);

    unit->building = true;
    unit->buildPercent = 0;
}

void leaveUpgradeState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    unit->building = false;
}

void updateUpgradeState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    WarUnitComponent* unit = &entity->unit;

    if (state->upgrade.cancelled)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, false);
            changeNextState(context, entity, idleState, true, true);
        }

        return;
    }

    f32 buildSpeed = getMapScaledSpeed(context, context->deltaTime);

    // if hurry up cheat is enabled, speed up the build time by 5000%
    if (map->hurryUp)
    {
        buildSpeed *= CHEAT_SPEED_UP_FACTOR;
    }

    state->upgrade.buildTime += buildSpeed;

    // if the building is finished...
    if (state->upgrade.buildTime >= state->upgrade.totalBuildTime)
    {
        unit->buildPercent = 1;

        // increase the level of the upgrade
        increaseUpgradeLevel(context, player, state->upgrade.upgradeToBuild);
        assert(checkUpgradeLevel(player, state->upgrade.upgradeToBuild));

        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, false);
            changeNextState(context, entity, idleState, true, true);
        }

        return;
    }

    unit->buildPercent = percentabf01(state->upgrade.buildTime, state->upgrade.totalBuildTime);
}

void freeUpgradeState(WarState* state)
{
}