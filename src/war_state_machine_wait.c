WarState* createWaitState(WarContext* context, WarEntity* entity, f32 waitTime)
{
    WarState* state = createState(context, entity, WAR_STATE_WAIT);
    state->wait.waitTime = waitTime;
    return state;
}

void enterWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
    setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, 1.0f);
}

void leaveWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
}

void updateWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    state->wait.waitTime -= getMapScaledSpeed(context, context->deltaTime);

    if (state->wait.waitTime < 0)
    {
        if (!changeStateNextState(context, entity, state))
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }
    }
}

void freeWaitState(WarState* state)
{
}