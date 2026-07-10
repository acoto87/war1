#include "war_state_machine.h"

#include "war_map.h"
#include "war_actions.h"
#include "war_audio.h"
#include "war_units.h"
#include "war_cheats.h"
#include "war_pathfinder.h"

#include "TracyC.h"

WarStateBuild* wst_createBuildState(WarContext* context, WarEntity* entity, f32 buildTime)
{
    TracyCZoneN(ctx, "wst_createBuildState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_BUILD, entity->id);
    WarStateBuild* state = (WarStateBuild*)wst_deref(context, ref);
    state->workerId = 0;
    state->buildTime = 0;
    state->totalBuildTime = buildTime;
    state->cancelled = false;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveBuildState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveBuildState", true);

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
    wpath_setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);

    unit->building = false;

    TracyCZoneEnd(ctx);
}

void wst_updateBuildState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateBuildState", true);

    WarStateBuild* s = (WarStateBuild*)state;

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (!state->initialized)
    {
        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        vec2 unitSize = wu_getUnitSize(context, entity);
        vec2 position = wmap_mapToTileCoordinatesV(transform->position);
        wpath_setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);

        we_removeSpriteComponent(context, entity);

        const WarBuildingData* buildingData = wu_getBuildingData(unit->type);
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(buildingData->buildingResource));

        wact_setAction(context, entity, WAR_ACTION_TYPE_NONE, true, 1.0f);

        unit->building = true;
        unit->buildPercent = 0;

        state->initialized = true;
        TracyCZoneEnd(ctx);
        return;
    }

    if (s->cancelled)
    {
        if (sm->depth > 1)
        {
            wst_popState(context, entity);
        }
        else
        {
            WarStateCollapse* collapseState = wst_createCollapseState(context, entity);
            wst_replaceState(context, entity, (WarStateBase*)collapseState);
        }

        TracyCZoneEnd(ctx);
        return;
    }

    if (s->workerId <= 0)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    f32 buildSpeed = context->gameDeltaTime;

    if (map->hurryUp)
    {
        buildSpeed *= CHEAT_SPEED_UP_FACTOR;
    }

    s->buildTime += buildSpeed;

    if (s->buildTime >= s->totalBuildTime)
    {
        unit->buildPercent = 1;

        // find the worker that is building the building
        WarEntity* worker = we_findEntity(context, s->workerId);
        assert(worker);

        vec2 tile = wu_getUnitCenterTile(context, entity);
        vec2 spawnTile = wpath_findEmptyTile(&map->finder, (s32)tile.x, (s32)tile.y);
        wu_setUnitCenterTile(context, worker, spawnTile);

        we_removeSpriteComponent(context, entity);

        const WarUnitData* buildingData = wu_getUnitData(unit->type);
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(buildingData->resourceIndex));

        wst_popState(context, entity);

        if (unit->player == 0)
        {
            WarAudioId audioId = isHumanPlayer(player) ? WAR_HUMAN_WORK_COMPLETE : WAR_ORC_WORK_COMPLETE;
            wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=audioId, .loop=false));
        }

        TracyCZoneEnd(ctx);
        return;
    }

    unit->buildPercent = PERCENTF01(s->buildTime, s->totalBuildTime);

    // update the sprite of the building to show the construction steps
    //
    // NOTE: maybe this could be handled by the BUILD action if I add a `pauseAction`
    // and `resumeAction` functions to be able to pause it or resume it according
    // to the presence of the worker at the construction site
    //
    WarSpriteComponent* sprite = we_getSpriteComponent(context, entity);
    assert(sprite);

    s32 framesCount = sprite->sprite.framesCount;
    s32 frameIndex = sprite->frameIndex;
    f32 frameIndexStep = 1.0f / framesCount;
    if (unit->buildPercent >= (frameIndex + 1) * frameIndexStep)
    {
        if (frameIndex + 1 < framesCount)
        {
            frameIndex += 1;
        }
    }
    sprite->frameIndex = frameIndex;

    TracyCZoneEnd(ctx);
}


void wst_updateBuildStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateBuildStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateBuild*      states  = storage->build;
    bool*             occupied = storage->occupied[WAR_STATE_BUILD];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateBuild*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->depth == 0 || sm->stack[sm->depth - 1].type != WAR_STATE_BUILD || sm->stack[sm->depth - 1].idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateBuildState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
