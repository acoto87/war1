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

void wst_enterUpgradeState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterUpgradeState", true);

    NOT_USED(state);

    WarMap* map = context->map;
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

    unit->building = true;
    unit->buildPercent = 0;

    TracyCZoneEnd(ctx);
}

void wst_leaveUpgradeState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveUpgradeState", true);

    NOT_USED(state);

    WarMap* map = context->map;
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

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

    if (s->cancelled)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, false);
            wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
        }

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

        if (!wst_changeStateNextState(context, entity, state))
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, false);
            wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    unit->buildPercent = PERCENTF01(s->buildTime, s->totalBuildTime);

    TracyCZoneEnd(ctx);
}

void wst_freeUpgradeState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeUpgradeState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
