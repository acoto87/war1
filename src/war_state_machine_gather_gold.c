WarState* createGatherGoldState(WarContext* context, WarEntity* entity, WarEntityId goldmineId)
{
    WarState* state = createState(context, entity, WAR_STATE_GOLD);
    state->gold.goldmineId = goldmineId;
    return state;
}

void enterGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void leaveGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = getUnitStats(unit->type);

    WarEntity* goldmine = findEntity(context, state->gold.goldmineId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), go idle
    // if the unit was already mining, and the gold mine ran out of gold, then another unit previouly got all the remaining gold
    // so, this unit get nothing
    if (!goldmine || isCollapsing(goldmine) || isGoingToCollapse(goldmine))
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    // if the goldmine is not in range, go to it
    if (!unitInRange(entity, goldmine, stats.range))
    {
        WarState* followState = createFollowState(context, entity, goldmine->id, VEC2_ZERO, stats.range);
        followState->nextState = state;
        changeNextState(context, entity, followState, false, true);
        return;
    }

    // the unit arrive to the goldmine, go mining
    WarState* miningState = createMiningState(context, entity, goldmine->id);
    changeNextState(context, entity, miningState, true, true);
}

void freeGatherGoldState(WarState* state)
{
}

