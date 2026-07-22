#include "war_state_machine.h"

#include "war_units.h"

#include "TracyC.h"

WarStateFollow* wst_createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetPosition, s32 targetDistance)
{
    TracyCZoneN(ctx, "wst_createFollowState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_FOLLOW, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateFollow* state = (WarStateFollow*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->targetEntityId = targetEntityId;
    state->targetPosition = targetPosition;
    state->targetDistance = targetDistance;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_updateFollowState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateFollowState", true);

    WarStateFollow* s = (WarStateFollow*)state;

    vec2 start = wu_getUnitCenterPosition(context, entity);
    vec2 end = s->targetPosition;

    if (s->targetEntityId)
    {
        WarEntity* targetEntity = we_findEntity(context, s->targetEntityId);
        if (!targetEntity)
        {
            // if the target entity doesn't exist anymore, pop to resume any
            // previous behavior, or fall back to idle if the stack empties.
            wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_TARGET_INVALID);

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
            end = wu_getUnitCenterPosition(context, targetEntity);
        }
    }

    f32 distanceSq = vec2_distanceSqr(start, end);

    // if the unit is already in distance, pop to resume any previous behavior,
    // or wait briefly and resume following if this is the bottom state.
    if (distanceSq <= s->targetDistance * s->targetDistance)
    {
        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION, WAR_STATE_RESULT_SUCCESS);

        TracyCZoneEnd(ctx);
        return;
    }

    WarStateMove* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, start, end), false);
    wst_pushState(context, entity, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_COMPLETION);

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

        if (!wst_isCurrentState(context, entity, (WarStateBase*)state)) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateFollowState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
