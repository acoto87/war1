#include "TracyC.h"

#include "war_state_machine.h"
#include "war_actions.h"

WarStateWait* wst_createWaitState(WarContext* context, WarEntity* entity, f32 waitTime)
{
    TracyCZoneN(ctx, "wst_createWaitState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_WAIT, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateWait* state = (WarStateWait*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->waitEndGameTime = context->gameTime + waitTime;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterWaitState", true);

    NOT_USED(state);

    WarMap* map = context->map;
    assert(map);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    wpath_setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
    wact_setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, 1.0f);

    TracyCZoneEnd(ctx);
}

void wst_exitWaitState(WarContext* context, WarEntity* entity, WarState* state, WarStateExitReason reason)
{
    TracyCZoneN(ctx, "wst_exitWaitState", true);

    NOT_USED(state);
    NOT_USED(reason);

    WarMap* map = context->map;
    assert(map);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    wpath_setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    TracyCZoneEnd(ctx);
}

void wst_updateWaitState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateWaitState", true);

    WarStateWait* s = (WarStateWait*)state;

    if (context->gameTime >= s->waitEndGameTime)
    {
        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);
    }

    TracyCZoneEnd(ctx);
}

void wst_updateWaitStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateWaitStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateWait*      states = storage->wait;
    bool*            occupied = storage->occupied[WAR_STATE_WAIT];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateWait*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!wst_isCurrentState(context, entity, (WarStateBase*)state)) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateWaitState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
