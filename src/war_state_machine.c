#include "war_state_machine.h"

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
    if (wst_isMining(context, entity))
    {
        return true;
    }

    if(wst_isDelivering(context, entity))
    {
        WarState* deliver = wst_getDeliverState(context, entity);
        return deliver->deliver.insideBuilding;
    }

    if (wst_isRepairing2(context, entity))
    {
        WarState* repairing = wst_getRepairingState(context, entity);
        return repairing->repairing.insideBuilding;
    }

    return false;
}

WarState* wst_createState(WarContext* context, WarEntity* entity, WarStateType type)
{
    NOT_USED(context);

    WarState* state = (WarState*)wm_alloc(sizeof(WarState));
    state->type = type;
    state->entityId = entity->id;
    state->nextUpdateTime = 0;
    state->delay = 0;
    return state;
}

void wst_changeNextState(WarContext* context, WarEntity* entity, WarState* state, bool wst_leaveState, bool wst_enterState)
{
    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    stateMachine->nextState = state;
    stateMachine->leaveState = wst_leaveState;
    stateMachine->enterState = wst_enterState;
}

bool wst_changeStateNextState(WarContext* context, WarEntity* entity, WarState* state)
{
    if(state->nextState)
    {
        wst_changeNextState(context, entity, state->nextState, true, false);
        state->nextState = NULL;
        return true;
    }

    return false;
}

WarState* wst_getState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    WarState* state = stateMachine->currentState;
    while (state && state->type != type)
        state = state->nextState;
    return state;
}

WarState* wst_getDirectState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    WarState* state = stateMachine->currentState;
    return state && state->type == type ? state : NULL;
}

WarState* wst_getNextState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    WarState* state = stateMachine->nextState;
    return state && state->type == type ? state : NULL;
}

WarState* wst_getIdleState(WarContext* context, WarEntity* entity) { return wst_getDirectState(context, entity, WAR_STATE_IDLE); }
WarState* wst_getMoveState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_MOVE); }
WarState* wst_getPatrolState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_PATROL); }
WarState* wst_getFollowState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_FOLLOW); }
WarState* wst_getAttackState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_ATTACK); }
WarState* wst_getDeathState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_DEATH); }
WarState* wst_getCollapseState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_COLLAPSE); }
WarState* wst_getGatherGoldState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_GOLD); }
WarState* wst_getMiningState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_MINING); }
WarState* wst_getGatherWoodState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_WOOD); }
WarState* wst_getChoppingState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_CHOPPING); }
WarState* wst_getDeliverState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_DELIVER); }
WarState* wst_getTrainState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_TRAIN); }
WarState* wst_getUpgradeState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_UPGRADE); }
WarState* wst_getBuildState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_BUILD); }
WarState* wst_getRepairState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_REPAIR); }
WarState* wst_getRepairingState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_REPAIRING); }
WarState* wst_getCastState(WarContext* context, WarEntity* entity) { return wst_getState(context, entity, WAR_STATE_CAST); }

bool wst_hasState(WarContext* context, WarEntity* entity, WarStateType type)
{
    return wst_getState(context, entity, type) != NULL;
}

bool wst_hasDirectState(WarContext* context, WarEntity* entity, WarStateType type)
{
    return wst_getDirectState(context, entity, type) != NULL;
}

bool wst_hasNextState(WarContext* context, WarEntity* entity, WarStateType type)
{
    return wst_getNextState(context, entity, type) != NULL;
}

bool wst_isIdle(WarContext* context, WarEntity* entity) { return wst_hasDirectState(context, entity, WAR_STATE_IDLE); }
bool wst_isMoving(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_MOVE); }
bool wst_isPatrolling(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_PATROL); }
bool wst_isFollowing(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_FOLLOW); }
bool wst_isAttacking(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_ATTACK); }
bool wst_isDead(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_DEATH); }
bool wst_isCollapsing(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_COLLAPSE); }
bool wst_isGatheringGold(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_GOLD); }
bool wst_isMining(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_MINING); }
bool wst_isGatheringWood(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_WOOD); }
bool wst_isChopping(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_CHOPPING); }
bool wst_isDelivering(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_DELIVER); }
bool wst_isTraining(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_TRAIN); }
bool wst_isUpgrading(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_UPGRADE); }
bool wst_isBuilding(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_BUILD); }
bool wst_isRepairing(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_REPAIR); }
bool wst_isRepairing2(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_REPAIRING); }
bool wst_isCasting(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_CAST); }

bool wst_isGoingToIdle(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_IDLE); }
bool wst_isGoingToMove(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_MOVE); }
bool wst_isGoingToPatrol(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_PATROL); }
bool wst_isGoingToFollow(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_FOLLOW); }
bool wst_isGoingToAttack(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_ATTACK); }
bool wst_isGoingToDie(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_DEATH); }
bool wst_isGoingToCollapse(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_COLLAPSE); }
bool wst_isGoingToGatherGold(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_GOLD); }
bool wst_isGoingToMine(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_MINING); }
bool wst_isGoingToGatherWood(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_WOOD); }
bool wst_isGoingToChop(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_CHOPPING); }
bool wst_isGoingToDeliver(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_DELIVER); }
bool wst_isGoingToTrain(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_TRAIN); }
bool wst_isGoingToUpgrade(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_UPGRADE); }
bool wst_isGoingToBuild(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_BUILD); }
bool wst_isGoingToRepair(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_REPAIR); }
bool wst_isGoingToRepair2(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_REPAIRING); }
bool wst_isGoingToCast(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_CAST); }

void wst_enterState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d for entity %d", state->type, entity->id);
        return;
    }

    stateDescriptors[state->type].enterStateFunc(context, entity, state);
}

void wst_leaveState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (!state)
    {
        return;
    }

    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d for entity %d", state->type, entity->id);
        return;
    }

    stateDescriptors[state->type].leaveStateFunc(context, entity, state);
    wst_freeState(context, state);
}

void wst_updateStateMachine(WarContext* context, WarEntity* entity)
{
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

        if (currentState->delay > 0)
        {
            currentState->nextUpdateTime = context->time + currentState->delay;
            currentState->delay = 0;
        }

        if (context->time >= currentState->nextUpdateTime)
        {
            if (!inRange(currentState->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
            {
                logError("Unkown state %d for entity %d", currentState->type, entity->id);
                return;
            }

            stateDescriptors[currentState->type].updateStateFunc(context, entity, currentState);
        }
    }
}

void wst_freeState(WarContext* context, WarState* state)
{
    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d", state->type);
        return;
    }

    stateDescriptors[state->type].freeStateFunc(context, state);

    if (state->nextState)
        wst_freeState(context, state->nextState);

    wm_free(state);
}
