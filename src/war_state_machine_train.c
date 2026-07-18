#include <assert.h>

#include "war_state_machine.h"

#include "war_ai.h"
#include "war_audio.h"

#include "TracyC.h"

WarStateTrain* wst_createTrainState(WarContext* context, WarEntity* entity, WarUnitType unitToBuild, f32 buildTime, s32 goldCost, s32 woodCost, WarAICommand* aiCommand)
{
    TracyCZoneN(ctx, "wst_createTrainState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_TRAIN, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateTrain* state = (WarStateTrain*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->unitToBuild = unitToBuild;
    state->buildTime = 0;
    state->totalBuildTime = buildTime;
    state->goldCost = goldCost;
    state->woodCost = woodCost;
    state->aiCommand = aiCommand;
    state->transactionApplied = false;
    state->outputCommitted = false;
    state->cancelled = false;
    state->alreadyRefunded = false;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveTrainState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveTrainState", true);

    WarStateTrain* trainState = (WarStateTrain*)state;
    WarUnitComponent* unit = NULL;

    if (trainState->cancelled &&
        trainState->transactionApplied &&
        !trainState->outputCommitted &&
        !trainState->alreadyRefunded)
    {
        unit = we_getUnitComponent(context, entity);
        assert(unit);

        WarPlayerInfo* player = &context->map->players[unit->player];
        trainState->alreadyRefunded = true;
        we_increasePlayerResources(context, player, trainState->goldCost, trainState->woodCost);
    }

    if (!trainState->transactionApplied)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    WarMap* map = context->map;
    if (!unit)
    {
        unit = we_getUnitComponent(context, entity);
        assert(unit);
    }

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    wpath_setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    unit->building = false;

    TracyCZoneEnd(ctx);
}

static bool wst_applyTrainTransaction(WarContext* context, WarUnitComponent* unit, WarStateTrain* state)
{
    WarPlayerInfo* player = &context->map->players[unit->player];
    if (!we_decreasePlayerResources(context, player, state->goldCost, state->woodCost))
    {
        return false;
    }

    state->transactionApplied = true;
    if (state->aiCommand)
    {
        wai_applyUnitRequestProgress(state->aiCommand);
        state->aiCommand = NULL;
    }

    return true;
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
        state->initialized = true;
        if (!wst_applyTrainTransaction(context, unit, s))
        {
            s->cancelled = true;
            wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION);
            TracyCZoneEnd(ctx);
            return;
        }

        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        vec2 unitSize = wu_getUnitSize(context, entity);
        vec2 position = wmap_mapToTileCoordinatesV(transform->position);
        wpath_setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

        unit->building = true;
        unit->buildPercent = 0;

        TracyCZoneEnd(ctx);
        return;
    }

    if (s->cancelled)
    {
        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION);

        TracyCZoneEnd(ctx);
        return;
    }

    f32 trainSpeed = context->gameDeltaTime;

    if (map->hurryUp)
    {
        trainSpeed *= CHEAT_SPEED_UP_FACTOR;
    }

    s->buildTime += trainSpeed;

    if (s->buildTime >= s->totalBuildTime)
    {
        unit->buildPercent = 1;

        WarEntity* unitToBuild = we_createDude(context, CREATE_UNIT_ARGS_INIT(
            .type=s->unitToBuild,
            .x=0, .y=0,
            .player=unit->player,
            .isGoingToTrain=false
        ));

        vec2 tile = wu_getUnitCenterTile(context, entity);
        vec2 spawnTile = wpath_findEmptyTile(&map->finder, (s32)tile.x, (s32)tile.y);
        wu_setUnitCenterTile(context, unitToBuild, spawnTile);
        we_setInitialIdleState(context, unitToBuild);

        s->outputCommitted = true;

        wst_popState(context, entity, WAR_TRANSITION_CAUSE_COMPLETION);

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

        if (!wst_isCurrentState(context, entity, (WarStateBase*)state)) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateTrainState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
