#include "war_state_machine.h"

WarState* createRepairState(WarContext* context, WarEntity* entity, WarEntityId buildingId)
{
    WarState* state = createState(context, entity, WAR_STATE_REPAIR);
    state->wcmd_repair.buildingId = buildingId;
    return state;
}

void enterRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void leaveRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void updateRepairState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = wun_getUnitStats(unit->type);

    WarEntity* building = went_findEntity(context, state->wcmd_repair.buildingId);

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || isCollapsing(building) || isGoingToCollapse(building))
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    // if the building is not in range, go to it
    if (!wun_unitInRange(entity, building, stats.range))
    {
        vec2 targetTile = wun_unitPointOnTarget(entity, building);

        WarState* followState = createFollowState(context, entity, building->id, targetTile, stats.range);
        followState->nextState = state;
        changeNextState(context, entity, followState, false, true);
        return;
    }

    // the unit arrive to the building, go repairing
    WarState* repairingState = createRepairingState(context, entity, building->id);
    changeNextState(context, entity, repairingState, true, true);
}

void freeRepairState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}

