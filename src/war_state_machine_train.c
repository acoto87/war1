#include <assert.h>

#include "war_state_machine.h"

#include "war_audio.h"

#include "TracyC.h"

WarStateTrain* wst_createTrainState(WarContext* context, WarEntity* entity, WarUnitType unitToBuild, f32 buildTime)
{
    TracyCZoneN(ctx, "wst_createTrainState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_TRAIN, entity->id);
    WarStateTrain* state = (WarStateTrain*)wst_deref(context, ref);
    state->unitToBuild = unitToBuild;
    state->buildTime = 0;
    state->totalBuildTime = buildTime;
    state->cancelled = false;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveTrainState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveTrainState", true);

    if (!state->initialized)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    NOT_USED(state);

    WarMap* map = context->map;
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    unit->building = false;

    TracyCZoneEnd(ctx);
}

void wst_updateTrainState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateTrainState", true);

    WarStateTrain* s = (WarStateTrain*)state;

    WarMap* map = context->map;
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (!state->initialized)
    {
        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        vec2 unitSize = wu_getUnitSize(context, entity);
        vec2 position = wmap_mapToTileCoordinatesV(transform->position);
        setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

        unit->building = true;
        unit->buildPercent = 0;

        state->initialized = true;
        TracyCZoneEnd(ctx);
        return;
    }

    if (s->cancelled)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, false);
            wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    f32 trainSpeed = context->gameDeltaTime;

    // if hurry up cheat is enabled, speed up the train time by 5000%
    if (map->hurryUp)
    {
        trainSpeed *= CHEAT_SPEED_UP_FACTOR;
    }

    s->buildTime += trainSpeed;

    // if the building is finished...
    if (s->buildTime >= s->totalBuildTime)
    {
        unit->buildPercent = 1;

        // ...create the unit
        WarEntity* unitToBuild = we_createDude(context, CREATE_UNIT_ARGS_INIT(
            .type=s->unitToBuild,
            .x=0, .y=0,
            .player=unit->player,
            .isGoingToTrain=false
        ));

        // ...find an empty position to put it
        vec2 position = wu_getUnitCenterPosition(context, entity, true);
        vec2 spawnPosition = wpath_findEmptyPosition(&map->finder, position);
        wu_setUnitCenterPosition(context, unitToBuild, spawnPosition, true);
        we_setInitialIdleState(context, unitToBuild);

        if (!wst_changeStateNextState(context, entity, state))
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, false);
            wst_changeNextState(context, entity, (WarStateBase*)idleState, true);
        }

        if (unit->player == 0)
        {
            WarAudioId audioId = wu_isHumanUnit(context, unitToBuild) ? WAR_HUMAN_READY : WAR_ORC_READY;
            wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=audioId, .loop=false));
        }

        TracyCZoneEnd(ctx);
        return;
    }

    unit->buildPercent = PERCENTF01(s->buildTime, s->totalBuildTime);

    TracyCZoneEnd(ctx);
}


void wst_updateTrainStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateTrainStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateTrain*      states  = storage->train;
    bool*             occupied = storage->occupied[WAR_STATE_TRAIN];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateTrain*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->currentRef.type != WAR_STATE_TRAIN || sm->currentRef.idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateTrainState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
