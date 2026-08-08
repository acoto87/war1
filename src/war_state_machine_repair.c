#include "war_state_machine.h"

#include "TracyC.h"

WarStateRepair* wst_createRepairState(WarContext* context, WarEntity* entity, WarEntityId buildingId)
{
    TracyCZoneN(ctx, "wst_createRepairState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_REPAIR, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateRepair* state = (WarStateRepair*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->buildingId = buildingId;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_updateRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateRepairState", true);

    WarStateRepair* s = (WarStateRepair*)state;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    WarEntity* building = we_findEntity(context, s->buildingId);

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || wst_isCollapsing(context, building) || wst_isGoingToCollapse(context, building))
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
        TracyCZoneEnd(ctx);
        return;
    }

    // if the building is not in range, go to it
    if (!wu_unitInRange(context, entity, building, stats->range))
    {
        vec2 targetPosition = wu_unitPointOnTarget(context, entity, building);
        WarStateFollow* followState = wst_createFollowState(context, entity, building->id, targetPosition, stats->range * MEGA_TILE_WIDTH);
        wst_pushState(context, entity, (WarStateBase*)followState, WAR_TRANSITION_CAUSE_COMPLETION);
        TracyCZoneEnd(ctx);
        return;
    }

    // the unit arrive to the building, go repairing
    WarStateRepairing* repairingState = wst_createRepairingState(context, entity, building->id);
    wst_replaceState(context, entity, (WarStateBase*)repairingState, WAR_TRANSITION_CAUSE_COMPLETION);

    TracyCZoneEnd(ctx);
}

void wst_updateRepairStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateRepairStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateRepair*      states  = storage->repair;
    bool*             occupied = storage->occupied[WAR_STATE_REPAIR];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateRepair*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateRepairState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
