#include "war_state_machine.h"
#include "state_machine/war_state_machine_idle.c"
#include "state_machine/war_state_machine_move.c"
#include "state_machine/war_state_machine_follow.c"
#include "state_machine/war_state_machine_patrol.c"
#include "state_machine/war_state_machine_attack.c"
#include "state_machine/war_state_machine_death.c"
#include "state_machine/war_state_machine_damaged.c"
#include "state_machine/war_state_machine_collapse.c"
#include "state_machine/war_state_machine_wait.c"

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
    switch (state->type)
    {
        case WAR_STATE_IDLE:
        {
            enterIdleState(context, entity, state);
            break;
        }

        case WAR_STATE_MOVE:
        {
            enterMoveState(context, entity, state);
            break;
        }

        case WAR_STATE_FOLLOW:
        {
            enterFollowState(context, entity, state);
            break;
        }

        case WAR_STATE_PATROL:
        {
            enterPatrolState(context, entity, state);
            break;
        }

        case WAR_STATE_ATTACK:
        {
            enterAttackState(context, entity, state);
            break;
        }

        case WAR_STATE_DEATH:
        {
            enterDeathState(context, entity, state);
            break;
        }

        case WAR_STATE_DAMAGED:
        {
            enterDamagedState(context, entity, state);
            break;
        }

        case WAR_STATE_COLLAPSE:
        {
            enterCollapseState(context, entity, state);
            break;
        }

        case WAR_STATE_WAIT:
        {
            enterWaitState(context, entity, state);
            break;
        }

        default:
        {
            logError("Unkown state %d for entity %d", state->type, entity->id);
            break;
        }
    }
}

void leaveState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (!state)
        return;

    switch (state->type)
    {
        case WAR_STATE_IDLE:
        {
            leaveIdleState(context, entity, state);
            break;
        }

        case WAR_STATE_MOVE:
        {
            leaveMoveState(context, entity, state);
            break;
        }

        case WAR_STATE_FOLLOW:
        {
            leaveFollowState(context, entity, state);
            break;
        }

        case WAR_STATE_PATROL:
        {
            leavePatrolState(context, entity, state);
            break;
        }

        case WAR_STATE_ATTACK:
        {
            leaveAttackState(context, entity, state);
            break;
        }

        case WAR_STATE_DEATH:
        {
            leaveDeathState(context, entity, state);
            break;
        }

        case WAR_STATE_DAMAGED:
        {
            leaveDamagedState(context, entity, state);
            break;
        }

        case WAR_STATE_COLLAPSE:
        {
            leaveCollapseState(context, entity, state);
            break;
        }

        case WAR_STATE_WAIT:
        {
            leaveWaitState(context, entity, state);
            break;
        }

        default:
        {
            logError("Unkown state %d for entity %d", state->type, entity->id);
            break;
        }
    }

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
            switch (currentState->type)
            {
                case WAR_STATE_IDLE:
                {
                    updateIdleState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_MOVE:
                {
                    updateMoveState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_FOLLOW:
                {
                    updateFollowState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_PATROL:
                {
                    updatePatrolState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_ATTACK:
                {
                    updateAttackState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_DEATH:
                {
                    updateDeathState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_DAMAGED:
                {
                    updateDamagedState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_COLLAPSE:
                {
                    updateCollapseState(context, entity, currentState);
                    break;
                }

                case WAR_STATE_WAIT:
                {
                    updateWaitState(context, entity, currentState);
                    break;
                }

                default:
                {
                    logError("Unkown state %d for entity %d", currentState->type, entity->id);
                    break;
                }
            }
        }
    }
}

void freeState(WarState* state)
{
    switch (state->type)
    {
        case WAR_STATE_IDLE:
        {
            freeIdleState(state);
            break;
        }

        case WAR_STATE_MOVE:
        {
            freeMoveState(state);
            break;
        }

        case WAR_STATE_FOLLOW:
        {
            freeFollowState(state);
            break;
        }

        case WAR_STATE_PATROL:
        {
            freePatrolState(state);
            break;
        }

        case WAR_STATE_ATTACK:
        {
            freeAttackState(state);
            break;
        }

        case WAR_STATE_DEATH:
        {
            freeDeathState(state);
            break;
        }

        case WAR_STATE_DAMAGED:
        {
            freeDamagedState(state);
            break;
        }

        case WAR_STATE_COLLAPSE:
        {
            freeCollapseState(state);
            break;
        }

        case WAR_STATE_WAIT:
        {
            freeWaitState(state);
            break;
        }

        default:
        {
            logError("Unkown state %d for entity %d", state->type);
            break;
        }
    }

    if (state->nextState)
        freeState(state->nextState);

    free(state);
}