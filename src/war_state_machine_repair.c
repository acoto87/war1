#include "war_state_machine.h"

#include "TracyC.h"

WarStateRepair* wst_createRepairState(WarContext* context, WarEntity* entity, WarEntityId buildingId)
{
    TracyCZoneN(ctx, "wst_createRepairState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_REPAIR, entity->id);
    WarStateRepair* state = (WarStateRepair*)wst_deref(context, ref);
    state->buildingId = buildingId;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterRepairState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_leaveRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveRepairState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
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
        wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
        TracyCZoneEnd(ctx);
        return;
    }

    // if the building is not in range, go to it
    if (!wu_unitInRange(context, entity, building, stats->range))
    {
        vec2 targetTile = wu_unitPointOnTarget(context, entity, building);

        WarStateFollow* followState = wst_createFollowState(context, entity, building->id, targetTile, stats->range);
        wst_chainNext(context, (WarStateBase*)followState, (WarStateBase*)state);
        wst_changeNextState(context, entity, (WarStateBase*)followState, false, true);
        TracyCZoneEnd(ctx);
        return;
    }

    // the unit arrive to the building, go repairing
    WarStateRepairing* repairingState = wst_createRepairingState(context, entity, building->id);
    wst_changeNextState(context, entity, (WarStateBase*)repairingState, true, true);

    TracyCZoneEnd(ctx);
}

void wst_freeRepairState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeRepairState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
