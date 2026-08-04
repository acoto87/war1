#include "war_state_machine.h"

#include "TracyC.h"

static bool wst_applyUpgradeTransaction(WarContext* context, WarUnitComponent* unit, WarStateUpgrade* state)
{
    WarPlayerInfo* player = &context->map->players[unit->player];
    if (!we_decreasePlayerResources(context, player, state->goldCost, state->woodCost))
    {
        return false;
    }

    state->transactionApplied = true;
    return true;
}

WarStateUpgrade* wst_createUpgradeState(WarContext* context, WarEntity* entity, WarUpgradeType upgradeToBuild, f32 buildTime, s32 goldCost, s32 woodCost)
{
    TracyCZoneN(ctx, "wst_createUpgradeState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_UPGRADE, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateUpgrade* state = (WarStateUpgrade*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->upgradeToBuild = upgradeToBuild;
    state->buildTime = 0;
    state->totalBuildTime = buildTime;
    state->goldCost = goldCost;
    state->woodCost = woodCost;
    state->transactionApplied = false;
    state->outputCommitted = false;
    state->cancelled = false;
    state->alreadyRefunded = false;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterUpgradeState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterUpgradeState", true);

    WarMap* map = context->map;
    assert(map);

    WarStateUpgrade* s = (WarStateUpgrade*)state;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (!wst_applyUpgradeTransaction(context, unit, s))
    {
        s->cancelled = true;
        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_CANCELLED);
        TracyCZoneEnd(ctx);
        return;
    }

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    wpath_setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

    unit->building = true;
    unit->buildPercent = 0;

    TracyCZoneEnd(ctx);
}

void wst_exitUpgradeState(WarContext* context, WarEntity* entity, WarState* state, WarStateExitReason reason)
{
    TracyCZoneN(ctx, "wst_exitUpgradeState", true);

    NOT_USED(reason);

    WarMap* map = context->map;
    assert(map);

    WarStateUpgrade* s = (WarStateUpgrade*)state;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (s->cancelled &&
        s->transactionApplied &&
        !s->outputCommitted &&
        !s->alreadyRefunded)
    {
        WarPlayerInfo* player = &context->map->players[unit->player];
        s->alreadyRefunded = true;
        we_increasePlayerResources(context, player, s->goldCost, s->woodCost);
    }

    if (!s->transactionApplied)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    wpath_setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    unit->building = false;

    TracyCZoneEnd(ctx);
}

void wst_updateUpgradeState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateUpgradeState", true);

    WarStateUpgrade* s = (WarStateUpgrade*)state;

    WarMap* map = context->map;
    assert(map);

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    WarPlayerInfo* player = &map->players[unit->player];

    if (s->cancelled)
    {
        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_CANCELLED);

        TracyCZoneEnd(ctx);
        return;
    }

    f32 buildSpeed = context->gameDeltaTime;

    // if hurry up cheat is enabled, speed up the build time by 5000%
    if (map->hurryUp)
    {
        buildSpeed *= CHEAT_SPEED_UP_FACTOR;
    }

    s->buildTime += buildSpeed;

    // if the building is finished...
    if (s->buildTime >= s->totalBuildTime)
    {
        unit->buildPercent = 1;

        // increase the level of the upgrade
        we_increaseUpgradeLevel(context, player, s->upgradeToBuild);
        assert(checkUpgradeLevel(player, s->upgradeToBuild));
        s->outputCommitted = true;

        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);

        TracyCZoneEnd(ctx);
        return;
    }

    unit->buildPercent = PERCENTF01(s->buildTime, s->totalBuildTime);

    TracyCZoneEnd(ctx);
}

void wst_updateUpgradeStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateUpgradeStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateUpgrade*      states  = storage->upgrade;
    bool*             occupied = storage->occupied[WAR_STATE_UPGRADE];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateUpgrade*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateUpgradeState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
