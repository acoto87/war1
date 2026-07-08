#include "war_state_machine.h"

#include "war_actions.h"

#include "TracyC.h"

WarStateDeath* wst_createDeathState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_createDeathState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_DEATH, entity->id);
    WarStateDeath* state = (WarStateDeath*)wst_deref(context, ref);

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveDeathState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateDeathState", true);

    WarMap* map = context->map;

    if (!state->initialized)
    {
        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        vec2 unitSize = wu_getUnitSize(context, entity);
        vec2 position = wmap_mapToTileCoordinatesV(transform->position);
        setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
        wact_setAction(context, entity, WAR_ACTION_TYPE_DEATH, true, 1.0f);
        wmap_removeEntityFromSelection(context, entity->id);

        s32 deathDuration = wact_getActionDuration(context, entity, WAR_ACTION_TYPE_DEATH);
        state->nextUpdateGameTime = context->gameTime + wmap_getMapScaledTime(context, __frameCountToSeconds(deathDuration));
        state->initialized = true;
        TracyCZoneEnd(ctx);
        return;
    }

    // when this state updates there will have pass the time of the death animation,
    // using the delay field of the states
    if (!wu_isCorpseUnit(context, entity) && !wu_isCatapultUnit(context, entity) &&
        !wu_isSummonUnit(context, entity) && !wu_isSkeletonUnit(context, entity))
    {
        vec2 position = wu_getUnitCenterPosition(context, entity, true);

        WarUnitType corpseType = wu_getUnitRace(context, entity) == WAR_RACE_ORCS
            ? WAR_UNIT_ORC_CORPSE : WAR_UNIT_HUMAN_CORPSE;

        WarEntity* corpse = we_createUnit(context, CREATE_UNIT_ARGS_INIT(
            .type=corpseType,
            .x=(s32)position.x,
            .y=(s32)position.y,
            .player=4,
            .resourceKind=WAR_RESOURCE_NONE,
            .amount=0,
            .addToMap=true
        ));

        wu_setUnitDirection(context, corpse, wu_getUnitDirection(context, entity));

        WarStateDeath* deathState = wst_createDeathState(context, corpse);
        wst_changeNextState(context, corpse, (WarStateBase*)deathState, true);
    }

    we_removeEntityById(context, entity->id);

    TracyCZoneEnd(ctx);
}


void wst_updateDeathStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateDeathStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateDeath*      states  = storage->death;
    bool*             occupied = storage->occupied[WAR_STATE_DEATH];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateDeath*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->currentRef.type != WAR_STATE_DEATH || sm->currentRef.idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateDeathState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
