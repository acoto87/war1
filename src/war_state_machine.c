WarStateDescriptor stateDescriptors[WAR_STATE_COUNT] = 
{
    { WAR_STATE_IDLE,     enterIdleState,       leaveIdleState,       updateIdleState,       freeIdleState       },
    { WAR_STATE_MOVE,     enterMoveState,       leaveMoveState,       updateMoveState,       freeMoveState       },
    { WAR_STATE_PATROL,   enterPatrolState,     leavePatrolState,     updatePatrolState,     freePatrolState     },
    { WAR_STATE_FOLLOW,   enterFollowState,     leaveFollowState,     updateFollowState,     freeFollowState     },
    { WAR_STATE_ATTACK,   enterAttackState,     leaveAttackState,     updateAttackState,     freeAttackState     },
    { WAR_STATE_GOLD,     enterGatherGoldState, leaveGatherGoldState, updateGatherGoldState, freeGatherGoldState },
    { WAR_STATE_MINING,   enterMiningState,     leaveMiningState,     updateMiningState,     freeMiningState     },
    { WAR_STATE_WOOD,     enterGatherWoodState, leaveGatherWoodState, updateGatherWoodState, freeGatherWoodState },
    { WAR_STATE_CHOPPING, enterChoppingState,   leaveChoppingState,   updateChoppingState,   freeChoppingState   },
    { WAR_STATE_DELIVER,  enterDeliverState,    leaveDeliverState,    updateDeliverState,    freeDeliverState    },
    { WAR_STATE_DEATH,    enterDeathState,      leaveDeathState,      updateDeathState,      freeDeathState      },
    { WAR_STATE_COLLAPSE, enterCollapseState,   leaveCollapseState,   updateCollapseState,   freeCollapseState   },
    { WAR_STATE_BUILDING, enterBuildingState,   leaveBuildingState,   updateBuildingState,   freeBuildingState   },
    { WAR_STATE_WAIT,     enterWaitState,       leaveWaitState,       updateWaitState,       freeWaitState       },
};

WarState* createState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarState* state = (WarState*)xcalloc(1, sizeof(WarState));
    state->type = type;
    state->entityId = entity->id;
    state->nextUpdateTime = 0;
    state->delay = 0;
    return state;
}

void changeNextState(WarContext* context, WarEntity* entity, WarState* state, bool leaveState, bool enterState)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    stateMachine->nextState = state;
    stateMachine->leaveState = leaveState;
    stateMachine->enterState = enterState;
}

bool changeStateNextState(WarContext* context, WarEntity* entity, WarState* state)
{
    if(state->nextState)
    {
        changeNextState(context, entity, state->nextState, true, false);
        state->nextState = NULL;
        return true;
    }

    return false;
}

WarState* getState(WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* state = stateMachine->currentState;
    while (state && state->type != type)
        state = state->nextState;
    return state;
}

WarState* getDirectState(WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* state = stateMachine->currentState;
    return state && state->type == type ? state : NULL;
}

WarState* getNextState(WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* state = stateMachine->nextState;
    return state && state->type == type ? state : NULL;
}

bool hasState(WarEntity* entity, WarStateType type)
{
    return getState(entity, type) != NULL;
}

bool hasDirectState(WarEntity* entity, WarStateType type)
{
    return getDirectState(entity, type) != NULL;
}

bool hasNextState(WarEntity* entity, WarStateType type)
{
    return getNextState(entity, type) != NULL;
}

void enterState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d for entity %d", state->type, entity->id);
        return;
    }

    stateDescriptors[state->type].enterStateFunc(context, entity, state);
}

void leaveState(WarContext* context, WarEntity* entity, WarState* state)
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
    freeState(state);
}

void updateStateMachine(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    if (stateMachine->enabled)
    {
        // the enterState could potentially change state if it determine that is not ready to start the current state
        while (stateMachine->nextState)
        {
            if (stateMachine->leaveState)
                leaveState(context, entity, stateMachine->currentState);
            
            stateMachine->currentState = stateMachine->nextState;
            stateMachine->nextState = NULL;

            if (stateMachine->enterState)
                enterState(context, entity, stateMachine->currentState);
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

void freeState(WarState* state)
{
    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d", state->type);
        return;
    }

    stateDescriptors[state->type].freeStateFunc(state);

    if (state->nextState)
        freeState(state->nextState);

    free(state);
}