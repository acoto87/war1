WarState* createState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;

    WarState* state = (WarState*)xcalloc(1, sizeof(WarState));
    state->type = type;
    state->entityId = entity->id;
    return state;
}

void freeState(WarState* state)
{
    free(state);
}

WarState* createIdleState(WarContext* context, WarEntity* entity, bool lookAround)
{
    WarState* state = createState(context, entity, WAR_STATE_IDLE);
    state->updateFrequency = 1.0f;
    state->nextUpdateTime = 0;
    state->idle.lookAround = lookAround;
    return state;
}

WarState* createMoveState(WarContext* context, WarEntity* entity, vec2 target)
{
    WarState* state = createState(context, entity, WAR_STATE_MOVE);
    state->updateFrequency = (1.0f/60.0f);
    state->nextUpdateTime = 0;
    state->move.target = target;
    return state;
}

WarState* createPatrolState(WarContext* context, WarEntity* entity, vec2 from, vec2 to)
{
    WarState* state = createState(context, entity, WAR_STATE_PATROL);
    state->updateFrequency = (1.0f/60.0f);
    state->nextUpdateTime = 0;
    state->patrol.from = from;
    state->patrol.to = to;
    state->patrol.dir = 1;
    return state;
}

void changeNextState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    stateMachine->nextState = state;
}

void enterState(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    WarState* currentState = stateMachine->currentState;

    switch (currentState->type)
    {
        case WAR_STATE_IDLE:
        {
            setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, false);
            break;
        }
    
        case WAR_STATE_MOVE:
        {
            WarTransformComponent* transform = &entity->transform;
            vec2 unitCenterPoint = vec2Mulf(getUnitSpriteSize(entity), 0.5f);
            vec2 position = vec2Addv(transform->position, unitCenterPoint);
            vec2 target = currentState->move.target;

            WarUnitComponent* unit = &entity->unit;
            unit->direction = getDirectionFromDiff(target.x - position.x, target.y - position.y);

            setAction(context, entity, WAR_ACTION_TYPE_WALK, true, false);
            break;
        }

        case WAR_STATE_PATROL:
        {
            WarTransformComponent* transform = &entity->transform;
            vec2 unitCenterPoint = vec2Mulf(getUnitSpriteSize(entity), 0.5f);
            vec2 position = vec2Addv(transform->position, unitCenterPoint);
            vec2 target = currentState->patrol.to;

            WarUnitComponent* unit = &entity->unit;
            unit->direction = getDirectionFromDiff(target.x - position.x, target.y - position.y);

            setAction(context, entity, WAR_ACTION_TYPE_WALK, true, false);
            break;
        }

        default:
        {
            logError("Unkown state %d for entity %d", currentState->type, entity->id);
            break;
        }
    }
}

void updateIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (state->idle.lookAround)
    {
        if (chance(50))
        {
            WarUnitComponent* unit = &entity->unit;
            unit->direction += random(-1, 2);
            if (unit->direction < 0)
                unit->direction = WAR_DIRECTION_NORTH_WEST;
            else if(unit->direction >= WAR_DIRECTION_COUNT)
                unit->direction = WAR_DIRECTION_NORTH;
        }
    }

    // for(s32 i = 0; i < map->entities.count; i++)
    // {
    //     WarEntity* other = context->map->entities[i];
    //     if (other && other->type == WAR_ENTITY_TYPE_UNIT)
    //     {
    //         WarUnitComponent* unit = &other->unit;
    //         if (isDudeUnit(unit->type) && isEnemy(unit->player))
    //         {
                
    //         }
    //     }
    // }
}

void updateMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarTransformComponent* transform = &entity->transform;
    vec2 unitCenterPoint = vec2Mulf(getUnitSpriteSize(entity), 0.5f);
    vec2 position = vec2Addv(transform->position, unitCenterPoint);
    vec2 target = state->move.target;
    vec2 direction = vec2Normalize(vec2Subv(target, position));
    vec2 step = vec2Mulf(direction, 20 * context->deltaTime);
    position = vec2Addv(position, step);
    f32 distance = vec2Distance(position, target);
    if (distance < 0.1f)
    {
        position = target;

        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState);
    }

    transform->position = vec2Subv(position, unitCenterPoint);
}

void updatePatrolState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarTransformComponent* transform = &entity->transform;
    vec2 unitCenterPoint = vec2Mulf(getUnitSpriteSize(entity), 0.5f);
    vec2 position = vec2Addv(transform->position, unitCenterPoint);
    vec2 target = state->patrol.dir > 0 ? state->patrol.to : state->patrol.from;
    vec2 direction = vec2Normalize(vec2Subv(target, position));
    vec2 step = vec2Mulf(direction, 20 * context->deltaTime);
    position = vec2Addv(position, step);
    f32 distance = vec2Distance(position, target);
    if (distance < 0.1f)
    {
        position = target;

        state->patrol.dir *= -1;
        target = state->patrol.dir > 0 ? state->patrol.to : state->patrol.from;

        WarUnitComponent* unit = &entity->unit;
        unit->direction = getDirectionFromDiff(target.x - position.x, target.y - position.y);
    }

    transform->position = vec2Subv(position, unitCenterPoint);
}

void updateStateMachine(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;
    if (stateMachine->enabled)
    {
        if (stateMachine->nextState)
        {
            freeState(stateMachine->currentState);
            stateMachine->currentState = stateMachine->nextState;
            stateMachine->nextState = NULL;

            enterState(context, entity);
        }

        WarState* currentState = stateMachine->currentState;

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

                case WAR_STATE_PATROL:
                {
                    updatePatrolState(context, entity, currentState);
                    break;
                }
            
                default:
                {
                    logError("Unkown state %d for entity %d", currentState->type, entity->id);
                    break;
                }
            }

            currentState->nextUpdateTime = context->time + currentState->updateFrequency;
        }
    }
}
