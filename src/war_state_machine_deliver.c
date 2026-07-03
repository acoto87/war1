#include "war_state_machine.h"

#include "TracyC.h"

WarState* wst_createDeliverState(WarContext* context, WarEntity* entity, WarEntityId townHallId)
{
    TracyCZoneN(ctx, "wst_createDeliverState", true);

    WarState* state = wst_createState(context, entity, WAR_STATE_DELIVER);
    state->deliver.townHallId = townHallId;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterDeliverState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterDeliverState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_leaveDeliverState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveDeliverState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateDeliverState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateDeliverState", true);

    WarMap* map = context->map;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    WarEntity* townHall = we_findEntity(context, (WarEntityId)state->deliver.townHallId);

    // if the town hall doesn't exists (or other units attacking it), go idle
    if (!townHall)
    {
        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
        TracyCZoneEnd(ctx);
        return;
    }

    if (!wu_unitInRange(context, entity, townHall, stats->range))
    {
        vec2 targetTile = wu_unitPointOnTarget(context, entity, townHall);

        WarState* followState = wst_createFollowState(context, entity, townHall->id, targetTile, stats->range);
        followState->nextState = state;
        wst_changeNextState(context, entity, followState, false, true);
        TracyCZoneEnd(ctx);
        return;
    }

    if (state->deliver.insideBuilding)
    {
        // find a valid spawn position for the unit
        vec2 position = wu_getUnitCenterPosition(context, townHall, true);
        vec2 spawnPosition = wpath_findEmptyPosition(&map->finder, position);
        wu_setUnitCenterPosition(context, entity, spawnPosition, true);

        const WarUnitData* unitData = wu_getUnitData(unit->type);
        we_removeSpriteComponent(context, entity);
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(unitData->resourceIndex));

        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    if (unit->resourceKind == WAR_RESOURCE_GOLD)
    {
        we_increasePlayerResources(context, &map->players[0], unit->amount, 0);
    }
    else if (unit->resourceKind == WAR_RESOURCE_WOOD)
    {
        we_increasePlayerResources(context, &map->players[0], 0, unit->amount);
    }

    unit->resourceKind = WAR_RESOURCE_NONE;
    unit->amount = 0;

    // the unit arrive to the townhall, so now the unit go inside the building for some time to simulate the depositing
    // then need go back to the goldmine/trees.
    state->deliver.insideBuilding = true;

    we_disableComponent(context, entity, COMP_SPRITE);

    // remove the unit from selection to avoid the player giving it orders
    // while inside the townhall
    wmap_removeEntityFromSelection(context, entity->id);

    // simulate the time inside the townhall
    state->delay = wmap_getMapScaledTime(context, 1.0f);

    TracyCZoneEnd(ctx);
}

void wst_freeDeliverState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeDeliverState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
