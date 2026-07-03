#include "war_state_machine.h"

#include "war_actions.h"

#include "TracyC.h"

WarState* wst_createWaitState(WarContext* context, WarEntity* entity, f32 waitTime)
{
    TracyCZoneN(ctx, "wst_createWaitState", true);

    WarState* state = wst_createState(context, entity, WAR_STATE_WAIT);
    state->wait.waitEndGameTime = context->gameTime + waitTime;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterWaitState", true);

    NOT_USED(state);

    WarMap* map = context->map;

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
    wact_setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, 1.0f);

    TracyCZoneEnd(ctx);
}

void wst_leaveWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveWaitState", true);

    NOT_USED(state);

    WarMap* map = context->map;

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    TracyCZoneEnd(ctx);
}

void wst_updateWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateWaitState", true);

    if (context->gameTime >= state->wait.waitEndGameTime)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }
    }

    TracyCZoneEnd(ctx);
}

void wst_freeWaitState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeWaitState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
