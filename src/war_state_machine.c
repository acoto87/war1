WarState* createState(WarStateType type)
{
    WarState* state = (WarState*)xcalloc(1, sizeof(WarState));
    state->type = type;
    return state;
}

WarState* createIdleState(bool lookAround)
{
    WarState* state = createState(WAR_STATE_IDLE);
    state->idle.lookAround = lookAround;
    return state;
}

void changeState(WarStateMachineComponent* stateMachine, WarState* state)
{
    stateMachine->currentState = state;
}

// void enterState(WarContext* context, WarEntity* entity)
// {
    
//     switch (currentState->type)
//     {
//         case WAR_STATE_IDLE:
//         {
//             setCurrentAnimation( entity, "Idle");        
            
//             break;
//         }
    
//         default:
//         {
//             logError("Unkown state %d for entity %d", currentState->type, entity->id);
//             break;
//         }
//     }

    
// }

void updateIdleState(WarContext* context, WarEntity* entity, WarStateMachineComponent* stateMachine)
{
    WarState* state = stateMachine->currentState;
    if (state->idle.lookAround)
    {
        if (chance(10))
        {

        }
    }

    for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    {
        WarEntity* other = context->map->entities[i];
        if (other && other->type == WAR_ENTITY_TYPE_UNIT)
        {
            WarUnitComponent* unit = &other->unit;
            if (isDudeUnit(unit->type) && isEnemy(unit->player))
            {
                
            }
        }
    }
    
}

void updateStateMachine(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    if (stateMachine->enabled)
    {
        if (context->time >= stateMachine->nextUpdateTime)
        {
            WarState* currentState = stateMachine->currentState;
            switch (currentState->type)
            {
                case WAR_STATE_IDLE:
                {
                    updateIdleState(context, entity, stateMachine);
                    break;
                }
            
                default:
                {
                    logError("Unkown state %d for entity %d", currentState->type, entity->id);
                    break;
                }
            }

            stateMachine->nextUpdateTime = context->time + 1.0f;
        }
    }
}
