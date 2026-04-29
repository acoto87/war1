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

bool wst_isInsideBuilding(WarEntity* entity)
{
    if (isMining(entity))
    {
        return true;
    }

    if(isDelivering(entity))
    {
        WarState* deliver = getDeliverState(entity);
        return deliver->deliver.insideBuilding;
    }

    if (isRepairing2(entity))
    {
        WarState* repairing = getRepairingState(entity);
        return repairing->repairing.insideBuilding;
    }

    return false;
}

WarState* wst_createState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarState* state = (WarState*)wm_alloc(sizeof(WarState));
    state->type = type;
    state->entityId = entity->id;
    state->nextUpdateTime = 0;
    state->delay = 0;
    return state;
}

void wst_changeNextState(WarContext* context, WarEntity* entity, WarState* state, bool wst_leaveState, bool wst_enterState)
{
    NOT_USED(context);

    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    stateMachine->nextState = state;
    stateMachine->wst_leaveState = wst_leaveState;
    stateMachine->wst_enterState = wst_enterState;
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

WarState* wst_getState(WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* state = stateMachine->currentState;
    while (state && state->type != type)
        state = state->nextState;
    return state;
}

WarState* wst_getDirectState(WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* state = stateMachine->currentState;
    return state && state->type == type ? state : NULL;
}

WarState* wst_getNextState(WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* state = stateMachine->nextState;
    return state && state->type == type ? state : NULL;
}

bool wst_hasState(WarEntity* entity, WarStateType type)
{
    return wst_getState(entity, type) != NULL;
}

bool wst_hasDirectState(WarEntity* entity, WarStateType type)
{
    return wst_getDirectState(entity, type) != NULL;
}

bool wst_hasNextState(WarEntity* entity, WarStateType type)
{
    return wst_getNextState(entity, type) != NULL;
}

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
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    if (stateMachine->enabled)
    {
        // the wst_enterState could potentially change state if it determine that is not ready to start the current state
        while (stateMachine->nextState)
        {
            if (stateMachine->wst_leaveState)
                wst_leaveState(context, entity, stateMachine->currentState);

            stateMachine->currentState = stateMachine->nextState;
            stateMachine->nextState = NULL;

            if (stateMachine->wst_enterState)
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
