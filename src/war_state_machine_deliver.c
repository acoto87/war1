#include "war_state_machine.h"

#include "TracyC.h"

WarStateDeliver* wst_createDeliverState(WarContext* context, WarEntity* entity, WarEntityId townHallId)
{
    TracyCZoneN(ctx, "wst_createDeliverState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_DELIVER, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateDeliver* state = (WarStateDeliver*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->townHallId = townHallId;
    state->insideBuilding = false;
    state->cycle = false;
    state->sourceKind = WAR_RESOURCE_NONE;
    state->sourceId = 0;
    state->sourcePosition = VEC2_ZERO;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_updateDeliverState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateDeliverState", true);

    WarStateDeliver* s = (WarStateDeliver*)state;

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    WarMap* map = context->map;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    WarEntity* townHall = we_findEntity(context, (WarEntityId)s->townHallId);

    // if the town hall doesn't exists (or other units attacking it), go idle
    if (!townHall)
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
        TracyCZoneEnd(ctx);
        return;
    }

    if (!wu_unitInRange(context, entity, townHall, stats->range))
    {
        vec2 targetPosition = wu_unitPointOnTarget(context, entity, townHall);
        WarStateFollow* followState = wst_createFollowState(context, entity, townHall->id, targetPosition, stats->range * MEGA_TILE_WIDTH);
        wst_pushState(context, entity, (WarStateBase*)followState, WAR_TRANSITION_CAUSE_COMPLETION);
        TracyCZoneEnd(ctx);
        return;
    }

    if (s->insideBuilding)
    {
        // find a valid spawn position for the unit
        vec2 tile = wu_getUnitCenterTile(context, townHall);
        vec2 spawnTile = wpath_findEmptyTile(&map->finder, (s32)tile.x, (s32)tile.y);
        wu_setUnitCenterTile(context, entity, spawnTile);

        const WarUnitData* unitData = wu_getUnitData(unit->type);
        we_removeSpriteComponent(context, entity);
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(unitData->resourceIndex));

        if (s->cycle)
        {
            if (s->sourceKind == WAR_RESOURCE_GOLD)
            {
                WarStateGold* gatherGoldState = wst_createGatherGoldState(context, entity, s->sourceId);
                wst_replaceState(context, entity, (WarStateBase*)gatherGoldState, WAR_TRANSITION_CAUSE_COMPLETION);
            }
            else
            {
                WarStateWood* gatherWoodState = wst_createGatherWoodState(context, entity, s->sourceId, s->sourcePosition);
                wst_replaceState(context, entity, (WarStateBase*)gatherWoodState, WAR_TRANSITION_CAUSE_COMPLETION);
            }
        }
        else if (sm->depth > 1)
        {
            wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);
        }
        else
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
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

        if (!wst_isCurrentState(context, entity, (WarStateBase*)state)) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateDeliverState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
