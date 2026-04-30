#include "war_state_machine.h"

WarState* wst_createRepairState(WarContext* context, WarEntity* entity, WarEntityId buildingId)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_REPAIR);
    state->repair.buildingId = buildingId;
    return state;
}

void wst_enterRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_leaveRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updateRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = wu_getUnitStats(unit->type);

    WarEntity* building = we_findEntity(context, state->repair.buildingId);

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || isCollapsing(building) || isGoingToCollapse(building))
    {
        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
        return;
    }

    // if the building is not in range, go to it
    if (!wu_unitInRange(entity, building, stats.range))
    {
        vec2 targetTile = wu_unitPointOnTarget(entity, building);

        WarState* followState = wst_createFollowState(context, entity, building->id, targetTile, stats.range);
        followState->nextState = state;
        wst_changeNextState(context, entity, followState, false, true);
        return;
    }

    // the unit arrive to the building, go repairing
    WarState* repairingState = wst_createRepairingState(context, entity, building->id);
    wst_changeNextState(context, entity, repairingState, true, true);
}

void wst_freeRepairState(WarContext* context, WarState* state)
{
    NOT_USED(context);
    NOT_USED(state);
}

