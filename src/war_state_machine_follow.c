#include "war_state_machine.h"

#include "war_units.h"

#include "TracyC.h"

WarStateFollow* wst_createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile, s32 distance)
{
    TracyCZoneN(ctx, "wst_createFollowState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_FOLLOW, entity->id);
    WarStateFollow* state = (WarStateFollow*)wst_deref(context, ref);
    state->targetEntityId = targetEntityId;
    state->targetTile = targetTile;
    state->distance = distance;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveFollowState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateFollowState", true);

    WarStateFollow* s = (WarStateFollow*)state;

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    WarMap* map = context->map;

    vec2 start = wu_getUnitCenterPosition(context, entity, true);
    vec2 end = s->targetTile;

    if (s->targetEntityId)
    {
        WarEntity* targetEntity = we_findEntity(context, (WarEntityId)s->targetEntityId);
        if (!targetEntity)
        {
            // if the target entity doesn't exist anymore, pop to resume any
            // previous behavior, or fall back to idle if the stack empties.
            wst_popState(context, entity);

            TracyCZoneEnd(ctx);
            return;
        }

        if (wu_isUnit(targetEntity))
        {
            // if the target entity is an unit the instead of using the tile where
            // the player click, use a point on the target unit that is closer to
            // the following unit
            end = wu_unitPointOnTarget(context, entity, targetEntity);
        }
        else
        {
            end = wu_getUnitCenterPosition(context, targetEntity, true);
        }
    }

    f32 distance = vec2_distanceInTiles(start, end);

    // if the unit is already in distance, pop to resume any previous behavior,
    // or wait briefly and resume following if this is the bottom state.
    if (distance <= s->distance)
    {
        if (sm->depth > 1)
        {
            wst_popState(context, entity);
        }
        else
        {
            WarStateWait* waitState = wst_createWaitState(context, entity, wmap_getMapScaledTime(context, MOVE_WAIT_TIME));
            wst_pushState(context, entity, (WarStateBase*)waitState);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    WarMapPath path = wpath_findPath(&map->finder, (s32)start.x, (s32)start.y, (s32)end.x, (s32)end.y);

    // if there is no path to the target, pop to resume any previous behavior,
    // or fall back to idle if the stack empties.
    if (path.nodes.count <= 1)
    {
        wst_popState(context, entity);

        Vec2ListFree(&path.nodes);
        TracyCZoneEnd(ctx);
        return;
    }

    WarStateMove* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, path.nodes.items[0], path.nodes.items[1]));
    wst_pushState(context, entity, (WarStateBase*)moveState);

    Vec2ListFree(&path.nodes);

    TracyCZoneEnd(ctx);
}


void wst_updateFollowStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateFollowStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateFollow*      states  = storage->follow;
    bool*             occupied = storage->occupied[WAR_STATE_FOLLOW];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateFollow*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->depth == 0 || sm->stack[sm->depth - 1].type != WAR_STATE_FOLLOW || sm->stack[sm->depth - 1].idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateFollowState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
