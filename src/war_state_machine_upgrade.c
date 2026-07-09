#include "war_state_machine.h"

#include "TracyC.h"

WarStateUpgrade* wst_createUpgradeState(WarContext* context, WarEntity* entity, WarUpgradeType upgradeToBuild, f32 buildTime)
{
    TracyCZoneN(ctx, "wst_createUpgradeState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_UPGRADE, entity->id);
    WarStateUpgrade* state = (WarStateUpgrade*)wst_deref(context, ref);
    state->upgradeToBuild = upgradeToBuild;
    state->buildTime = 0;
    state->totalBuildTime = buildTime;
    state->cancelled = false;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveUpgradeState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveUpgradeState", true);

    if (!state->initialized)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    NOT_USED(state);

    WarMap* map = context->map;
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

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
    WarPlayerInfo* player = &map->players[0];
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (!state->initialized)
    {
        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        vec2 unitSize = wu_getUnitSize(context, entity);
        vec2 position = wmap_mapToTileCoordinatesV(transform->position);
        wpath_setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

        unit->building = true;
        unit->buildPercent = 0;

        state->initialized = true;
        TracyCZoneEnd(ctx);
        return;
    }

    if (s->cancelled)
    {
        wst_popState(context, entity);

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

        wst_popState(context, entity);

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

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->depth == 0 || sm->stack[sm->depth - 1].type != WAR_STATE_UPGRADE || sm->stack[sm->depth - 1].idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateUpgradeState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
