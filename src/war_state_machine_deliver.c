#include "war_state_machine.h"

#include "TracyC.h"

WarStateDeliver* wst_createDeliverState(WarContext* context, WarEntity* entity, WarEntityId townHallId)
{
    TracyCZoneN(ctx, "wst_createDeliverState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_DELIVER, entity->id);
    WarStateDeliver* state = (WarStateDeliver*)wst_deref(context, ref);
    state->townHallId = townHallId;

    TracyCZoneEnd(ctx);
    return state;
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

    WarStateDeliver* s = (WarStateDeliver*)state;

    WarMap* map = context->map;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    WarEntity* townHall = we_findEntity(context, (WarEntityId)s->townHallId);

    // if the town hall doesn't exists (or other units attacking it), go idle
    if (!townHall)
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
        TracyCZoneEnd(ctx);
        return;
    }

    if (!wu_unitInRange(context, entity, townHall, stats->range))
    {
        vec2 targetTile = wu_unitPointOnTarget(context, entity, townHall);

        WarStateFollow* followState = wst_createFollowState(context, entity, townHall->id, targetTile, stats->range);
        wst_chainNext(context, (WarStateBase*)followState, (WarStateBase*)state);
        wst_changeNextState(context, entity, (WarStateBase*)followState, false);
        TracyCZoneEnd(ctx);
        return;
    }

    if (s->insideBuilding)
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
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
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
    s->insideBuilding = true;

    we_disableComponent(context, entity, COMP_SPRITE);

    // remove the unit from selection to avoid the player giving it orders
    // while inside the townhall
    wmap_removeEntityFromSelection(context, entity->id);

    // simulate the time inside the townhall
    state->delay = wmap_getMapScaledTime(context, 1.0f);

    TracyCZoneEnd(ctx);
}


void wst_updateDeliverStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateDeliverStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateDeliver*      states  = storage->deliver;
    bool*             occupied = storage->occupied[WAR_STATE_DELIVER];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateDeliver*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->currentRef.type != WAR_STATE_DELIVER || sm->currentRef.idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateDeliverState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
