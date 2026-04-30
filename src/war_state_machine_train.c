#include "war_state_machine.h"

#include "war_audio.h"

WarState* wst_createTrainState(WarContext* context, WarEntity* entity, WarUnitType unitToBuild, f32 buildTime)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_TRAIN);
    state->train.unitToBuild = unitToBuild;
    state->train.buildTime = 0;
    state->train.totalBuildTime = buildTime;
    state->train.cancelled = false;
    return state;
}

void wst_enterTrainState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = wu_getUnitSize(entity);
    vec2 position = wmap_mapToTileCoordinatesV(entity->transform.position);
    setStaticEntity(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

    unit->building = true;
    unit->buildPercent = 0;
}

void wst_leaveTrainState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = wu_getUnitSize(entity);
    vec2 position = wmap_mapToTileCoordinatesV(entity->transform.position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    unit->building = false;
}

void wst_updateTrainState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarUnitComponent* unit = &entity->unit;

    if (state->train.cancelled)
    {
        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, false);
            wst_changeNextState(context, entity, idleState, true, true);
        }

        return;
    }

    f32 trainSpeed = wmap_getMapScaledSpeed(context, context->deltaTime);

    // if hurry up cheat is enabled, speed up the train time by 5000%
    if (map->hurryUp)
    {
        trainSpeed *= CHEAT_SPEED_UP_FACTOR;
    }

    state->train.buildTime += trainSpeed;

    // if the building is finished...
    if (state->train.buildTime >= state->train.totalBuildTime)
    {
        unit->buildPercent = 1;

        // ...create the unit
        WarEntity* unitToBuild = we_createDude(context, state->train.unitToBuild, 0, 0, unit->player, false);

        // ...find an empty position to put it
        vec2 position = wu_getUnitCenterPosition(entity, true);
        vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);
        wu_setUnitCenterPosition(unitToBuild, spawnPosition, true);

        if (!wst_changeStateNextState(context, entity, state))
        {
            WarState* idleState = wst_createIdleState(context, entity, false);
            wst_changeNextState(context, entity, idleState, true, true);
        }

        WarAudioId audioId = isHumanUnit(unitToBuild) ? WAR_HUMAN_READY : WAR_ORC_READY;
        wa_createAudio(context, audioId, false);

        return;
    }

    unit->buildPercent = percentabf01(state->train.buildTime, state->train.totalBuildTime);
}

void wst_freeTrainState(WarContext* context, WarState* state)
{
    NOT_USED(context);
    NOT_USED(state);
}
