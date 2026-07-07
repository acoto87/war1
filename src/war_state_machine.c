#include "war_state_machine.h"

#include "TracyC.h"

WarStateDescriptor stateDescriptors[WAR_STATE_COUNT] =
{
    { WAR_STATE_IDLE,       wst_enterIdleState,       wst_leaveIdleState,       wst_updateIdleState,       wst_freeIdleState       },
    { WAR_STATE_MOVE,       wst_enterMoveState,       wst_leaveMoveState,       wst_updateMoveState,       wst_freeMoveState       },
    { WAR_STATE_PATROL,     wst_enterPatrolState,     wst_leavePatrolState,     wst_updatePatrolState,     wst_freePatrolState     },
    { WAR_STATE_FOLLOW,     wst_enterFollowState,     wst_leaveFollowState,     wst_updateFollowState,     wst_freeFollowState     },
    { WAR_STATE_ATTACK,     wst_enterAttackState,     wst_leaveAttackState,     wst_updateAttackState,     wst_freeAttackState     },
    { WAR_STATE_GOLD,       wst_enterGatherGoldState, wst_leaveGatherGoldState, wst_updateGatherGoldState, wst_freeGatherGoldState },
    { WAR_STATE_MINING,     wst_enterMiningState,     wst_leaveMiningState,     wst_updateMiningState,     wst_freeMiningState     },
    { WAR_STATE_WOOD,       wst_enterGatherWoodState, wst_leaveGatherWoodState, wst_updateGatherWoodState, wst_freeGatherWoodState },
    { WAR_STATE_CHOPPING,   wst_enterChoppingState,   wst_leaveChoppingState,   wst_updateChoppingState,   wst_freeChoppingState   },
    { WAR_STATE_DELIVER,    wst_enterDeliverState,    wst_leaveDeliverState,    wst_updateDeliverState,    wst_freeDeliverState    },
    { WAR_STATE_DEATH,      wst_enterDeathState,      wst_leaveDeathState,      wst_updateDeathState,      wst_freeDeathState      },
    { WAR_STATE_COLLAPSE,   wst_enterCollapseState,   wst_leaveCollapseState,   wst_updateCollapseState,   wst_freeCollapseState   },
    { WAR_STATE_TRAIN,      wst_enterTrainState,      wst_leaveTrainState,      wst_updateTrainState,      wst_freeTrainState      },
    { WAR_STATE_UPGRADE,    wst_enterUpgradeState,    wst_leaveUpgradeState,    wst_updateUpgradeState,    wst_freeUpgradeState    },
    { WAR_STATE_BUILD,      wst_enterBuildState,      wst_leaveBuildState,      wst_updateBuildState,      wst_freeBuildState      },
    { WAR_STATE_REPAIR,     wst_enterRepairState,     wst_leaveRepairState,     wst_updateRepairState,     wst_freeRepairState     },
    { WAR_STATE_REPAIRING,  wst_enterRepairingState,  wst_leaveRepairingState,  wst_updateRepairingState,  wst_freeRepairingState  },
    { WAR_STATE_CAST,       wst_enterCastState,       wst_leaveCastState,       wst_updateCastState,       wst_freeCastState       },
    { WAR_STATE_WAIT,       wst_enterWaitState,       wst_leaveWaitState,       wst_updateWaitState,       wst_freeWaitState       },
};

bool wst_isInsideBuilding(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isInsideBuilding", true);

    if (wst_isMining(context, entity))
    {
        TracyCZoneEnd(ctx);
        return true;
    }

    if(wst_isDelivering(context, entity))
    {
        WarState* deliver = wst_getDeliverState(context, entity);
        TracyCZoneEnd(ctx);
        return deliver->deliver.insideBuilding;
    }

    if (wst_isRepairing2(context, entity))
    {
        WarState* repairing = wst_getRepairingState(context, entity);
        TracyCZoneEnd(ctx);
        return repairing->repairing.insideBuilding;
    }

    TracyCZoneEnd(ctx);
    return false;
}

WarState* wst_createState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_createState", true);

    NOT_USED(context);

    WarState* state = (WarState*)wm_alloc(sizeof(WarState));
    state->type = type;
    state->entityId = entity->id;
    state->nextUpdateGameTime = 0;
    state->delay = 0;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_changeNextState(WarContext* context, WarEntity* entity, WarState* state, bool wst_leaveState, bool wst_enterState)
{
    TracyCZoneN(ctx, "wst_changeNextState", true);

    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    stateMachine->nextState = state;
    stateMachine->leaveState = wst_leaveState;
    stateMachine->enterState = wst_enterState;

    TracyCZoneEnd(ctx);
}

bool wst_changeStateNextState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_changeStateNextState", true);

    if(state->nextState)
    {
        wst_changeNextState(context, entity, state->nextState, true, false);
        state->nextState = NULL;

        TracyCZoneEnd(ctx);
        return true;
    }

    TracyCZoneEnd(ctx);
    return false;
}

WarState* wst_getState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_getState", true);

    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    WarState* state = stateMachine->currentState;
    while (state && state->type != type)
        state = state->nextState;

    TracyCZoneEnd(ctx);
    return state;
}

WarState* wst_getDirectState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_getDirectState", true);

    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    WarState* state = stateMachine->currentState;
    WarState* result = state && state->type == type ? state : NULL;

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getNextState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_getNextState", true);

    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    WarState* state = stateMachine->nextState;
    WarState* result = state && state->type == type ? state : NULL;

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getIdleState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getIdleState", true);

    WarState* result = wst_getDirectState(context, entity, WAR_STATE_IDLE);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getMoveState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getMoveState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_MOVE);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getPatrolState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getPatrolState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_PATROL);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getFollowState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getFollowState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_FOLLOW);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getAttackState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getAttackState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_ATTACK);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getDeathState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getDeathState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_DEATH);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getCollapseState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getCollapseState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_COLLAPSE);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getGatherGoldState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getGatherGoldState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_GOLD);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getMiningState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getMiningState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_MINING);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getGatherWoodState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getGatherWoodState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_WOOD);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getChoppingState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getChoppingState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_CHOPPING);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getDeliverState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getDeliverState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_DELIVER);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getTrainState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getTrainState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_TRAIN);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getUpgradeState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getUpgradeState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_UPGRADE);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getBuildState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getBuildState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_BUILD);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getRepairState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getRepairState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_REPAIR);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getRepairingState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getRepairingState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_REPAIRING);

    TracyCZoneEnd(ctx);
    return result;
}

WarState* wst_getCastState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getCastState", true);

    WarState* result = wst_getState(context, entity, WAR_STATE_CAST);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_hasState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_hasState", true);

    bool result = wst_getState(context, entity, type) != NULL;

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_hasDirectState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_hasDirectState", true);

    bool result = wst_getDirectState(context, entity, type) != NULL;

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_hasNextState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_hasNextState", true);

    bool result = wst_getNextState(context, entity, type) != NULL;

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isIdle(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isIdle", true);

    bool result = wst_hasDirectState(context, entity, WAR_STATE_IDLE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isMoving(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isMoving", true);

    bool result = wst_hasState(context, entity, WAR_STATE_MOVE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isPatrolling(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isPatrolling", true);

    bool result = wst_hasState(context, entity, WAR_STATE_PATROL);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isFollowing(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isFollowing", true);

    bool result = wst_hasState(context, entity, WAR_STATE_FOLLOW);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isAttacking(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isAttacking", true);

    bool result = wst_hasState(context, entity, WAR_STATE_ATTACK);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isDead(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isDead", true);

    bool result = wst_hasState(context, entity, WAR_STATE_DEATH);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isCollapsing(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isCollapsing", true);

    bool result = wst_hasState(context, entity, WAR_STATE_COLLAPSE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGatheringGold(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGatheringGold", true);

    bool result = wst_hasState(context, entity, WAR_STATE_GOLD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isMining(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isMining", true);

    bool result = wst_hasState(context, entity, WAR_STATE_MINING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGatheringWood(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGatheringWood", true);

    bool result = wst_hasState(context, entity, WAR_STATE_WOOD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isChopping(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isChopping", true);

    bool result = wst_hasState(context, entity, WAR_STATE_CHOPPING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isDelivering(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isDelivering", true);

    bool result = wst_hasState(context, entity, WAR_STATE_DELIVER);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isTraining(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isTraining", true);

    bool result = wst_hasState(context, entity, WAR_STATE_TRAIN);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isUpgrading(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isUpgrading", true);

    bool result = wst_hasState(context, entity, WAR_STATE_UPGRADE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isBuilding(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isBuilding", true);

    bool result = wst_hasState(context, entity, WAR_STATE_BUILD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isRepairing(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isRepairing", true);

    bool result = wst_hasState(context, entity, WAR_STATE_REPAIR);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isRepairing2(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isRepairing2", true);

    bool result = wst_hasState(context, entity, WAR_STATE_REPAIRING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isCasting(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isCasting", true);

    bool result = wst_hasState(context, entity, WAR_STATE_CAST);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToIdle(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToIdle", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_IDLE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToMove(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToMove", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_MOVE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToPatrol(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToPatrol", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_PATROL);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToFollow(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToFollow", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_FOLLOW);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToAttack(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToAttack", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_ATTACK);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToDie(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToDie", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_DEATH);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToCollapse(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToCollapse", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_COLLAPSE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToGatherGold(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToGatherGold", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_GOLD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToMine(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToMine", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_MINING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToGatherWood(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToGatherWood", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_WOOD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToChop(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToChop", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_CHOPPING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToDeliver(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToDeliver", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_DELIVER);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToTrain(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToTrain", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_TRAIN);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToUpgrade(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToUpgrade", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_UPGRADE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToBuild(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToBuild", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_BUILD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToRepair(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToRepair", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_REPAIR);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToRepair2(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToRepair2", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_REPAIRING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToCast(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToCast", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_CAST);

    TracyCZoneEnd(ctx);
    return result;
}

void wst_enterState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterState", true);

    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d for entity %d", state->type, entity->id);
        TracyCZoneEnd(ctx);
        return;
    }

    stateDescriptors[state->type].enterStateFunc(context, entity, state);

    TracyCZoneEnd(ctx);
}

void wst_leaveState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveState", true);

    if (!state)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d for entity %d", state->type, entity->id);
        TracyCZoneEnd(ctx);
        return;
    }

    stateDescriptors[state->type].leaveStateFunc(context, entity, state);
    wst_freeState(context, state);

    TracyCZoneEnd(ctx);
}

void wst_updateStateMachine(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_updateStateMachine", true);

    if (we_isComponentEnabled(context, entity, COMP_STATE_MACHINE))
    {
        WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
        assert(stateMachine);

        // the wst_enterState could potentially change state if it determine that is not ready to start the current state
        while (stateMachine->nextState)
        {
            if (stateMachine->leaveState)
                wst_leaveState(context, entity, stateMachine->currentState);

            stateMachine->currentState = stateMachine->nextState;
            stateMachine->nextState = NULL;

            if (stateMachine->enterState)
                wst_enterState(context, entity, stateMachine->currentState);
        }

        WarState* currentState = stateMachine->currentState;

        if (currentState->type == WAR_STATE_MOVE)
        {
            // NOTE: Move state are updated in `updateMoveStates` function
            // This will be fixed removed later after restructuring the state machines memory layout
            return;
        }

        if (currentState->delay > 0)
        {
            currentState->nextUpdateGameTime = context->gameTime + currentState->delay;
            currentState->delay = 0;
        }

        if (context->gameTime >= currentState->nextUpdateGameTime)
        {
            if (!inRange(currentState->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
            {
                logError("Unkown state %d for entity %d", currentState->type, entity->id);
                TracyCZoneEnd(ctx);
                return;
            }

            stateDescriptors[currentState->type].updateStateFunc(context, entity, currentState);
        }
    }

    TracyCZoneEnd(ctx);
}

void wst_freeState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeState", true);

    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d", state->type);
        TracyCZoneEnd(ctx);
        return;
    }

    stateDescriptors[state->type].freeStateFunc(context, state);

    if (state->nextState)
        wst_freeState(context, state->nextState);

    wm_free(state);

    TracyCZoneEnd(ctx);
}
