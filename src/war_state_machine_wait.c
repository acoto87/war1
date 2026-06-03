#include "war_state_machine.h"

#include "war_actions.h"

WarState* wst_createWaitState(WarContext* context, WarEntity* entity, f32 waitTime)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_WAIT);
    state->wait.waitEndGameTime = (f32)(context->gameTime + waitTime);
    return state;
}

void wst_enterWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    WarMap* map = context->map;

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    setStaticEntity(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
    wact_setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, 1.0f);
}

void wst_leaveWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    WarMap* map = context->map;

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
}

void wst_updateWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (context->gameTime >= state->wait.waitEndGameTime)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }
    }
}

void wst_freeWaitState(WarContext* context, WarState* state)
{
    NOT_USED(context);
    NOT_USED(state);
}
