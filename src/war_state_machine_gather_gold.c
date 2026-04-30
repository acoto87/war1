#include "war_state_machine.h"

WarState* wst_createGatherGoldState(WarContext* context, WarEntity* entity, WarEntityId goldmineId)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_GOLD);
    state->gold.goldmineId = goldmineId;
    return state;
}

void wst_enterGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_leaveGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updateGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = wu_getUnitStats(unit->type);

    WarEntity* goldmine = we_findEntity(context, (WarEntityId)state->gold.goldmineId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), go idle
    // if the unit was already mining, and the gold mine ran out of gold, then another unit previouly got all the remaining gold
    // so, this unit get nothing
    if (!goldmine || isCollapsing(goldmine) || isGoingToCollapse(goldmine))
    {
        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
        return;
    }

    // if the goldmine is not in range, go to it
    if (!wu_unitInRange(entity, goldmine, stats.range))
    {
        WarState* followState = wst_createFollowState(context, entity, goldmine->id, VEC2_ZERO, stats.range);
        followState->nextState = state;
        wst_changeNextState(context, entity, followState, false, true);
        return;
    }

    // the unit arrive to the goldmine, go mining
    WarState* miningState = wst_createMiningState(context, entity, goldmine->id);
    wst_changeNextState(context, entity, miningState, true, true);
}

void wst_freeGatherGoldState(WarContext* context, WarState* state)
{
    NOT_USED(context);
    NOT_USED(state);
}

