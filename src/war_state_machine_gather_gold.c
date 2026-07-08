#include "war_state_machine.h"

#include "TracyC.h"

WarStateGold* wst_createGatherGoldState(WarContext* context, WarEntity* entity, WarEntityId goldmineId)
{
    TracyCZoneN(ctx, "wst_createGatherGoldState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_GOLD, entity->id);
    WarStateGold* state = (WarStateGold*)wst_deref(context, ref);
    state->goldmineId = goldmineId;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveGatherGoldState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateGatherGoldState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateGatherGoldState", true);

    WarStateGold* s = (WarStateGold*)state;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    WarEntity* goldmine = we_findEntity(context, (WarEntityId)s->goldmineId);

    // if the goldmine doesn't exists (it could ran out of gold, or other units attacking it), go idle
    // if the unit was already mining, and the gold mine ran out of gold, then another unit previouly got all the remaining gold
    // so, this unit get nothing
    if (!goldmine || wst_isCollapsing(context, goldmine) || wst_isGoingToCollapse(context, goldmine))
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
        TracyCZoneEnd(ctx);
        return;
    }

    // if the goldmine is not in range, go to it
    if (!wu_unitInRange(context, entity, goldmine, stats->range))
    {
        WarStateFollow* followState = wst_createFollowState(context, entity, goldmine->id, VEC2_ZERO, stats->range);
        wst_chainNext(context, (WarStateBase*)followState, (WarStateBase*)state);
        wst_changeNextState(context, entity, (WarStateBase*)followState, false);
        TracyCZoneEnd(ctx);
        return;
    }

    // the unit arrive to the goldmine, go mining
    WarStateMining* miningState = wst_createMiningState(context, entity, goldmine->id);
    wst_changeNextState(context, entity, (WarStateBase*)miningState, true);

    TracyCZoneEnd(ctx);
}


void wst_updateGoldStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateGoldStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateGold*      states  = storage->gold;
    bool*             occupied = storage->occupied[WAR_STATE_GOLD];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateGold*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->currentRef.type != WAR_STATE_GOLD || sm->currentRef.idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateGatherGoldState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
