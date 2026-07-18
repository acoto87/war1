#include "war_state_machine.h"

#include "war_actions.h"
#include "war_audio.h"
#include "war_units.h"
#include "war_map.h"

#include "TracyC.h"

WarStateChopping* wst_createChoppingState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    TracyCZoneN(ctx, "wst_createChoppingState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_CHOPPING, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateChopping* state = (WarStateChopping*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->forestId = forestId;
    state->position = position;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveChoppingState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveChoppingState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateChoppingState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateChoppingState", true);

    WarMap* map = context->map;
    assert(map);

    WarStateChopping* s = (WarStateChopping*)state;

    if (!state->initialized)
    {
        vec2 unitTile = wu_getUnitCenterTile(context, entity);
        vec2 unitSize = wu_getUnitSize(context, entity);
        vec2 treePosition = s->position;
        vec2 treeTile = wmap_mapToTileCoordinatesV(treePosition);

        wpath_setStaticEntity(&map->finder, (s32)unitTile.x, (s32)unitTile.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
        wu_setUnitDirectionFromDiff(context, entity, treeTile.x - unitTile.x, treeTile.y - unitTile.y);
        wact_setAction(context, entity, WAR_ACTION_TYPE_HARVEST, true, 1.0f);

        state->initialized = true;
        TracyCZoneEnd(ctx);
        return;
    }

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    WarEntity* forest = we_findEntity(context, (WarEntityId)s->forestId);

    // if the forest doesn't exists, go idle
    if (!forest)
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
        TracyCZoneEnd(ctx);
        return;
    }

    vec2 treePosition = s->position;
    vec2 treeTile = wmap_mapToTileCoordinatesV(treePosition);
    WarTree* tree = we_getTreeAtTile(context, forest, (s32)treeTile.x, (s32)treeTile.y);

    if (!tree || tree->amount == 0)
    {
        WarStateWood* gatherWoodState = wst_createGatherWoodState(context, entity, forest->id, treePosition);
        wst_replaceState(context, entity, (WarStateBase*)gatherWoodState, WAR_TRANSITION_CAUSE_COMPLETION);
        TracyCZoneEnd(ctx);
        return;
    }

    WarUnitAction* action = &unit->actions[unit->actionType];
    if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
    {
        unit->amount += we_chopTree(context, forest, tree, 2);
        if (unit->amount > 0)
        {
            unit->resourceKind = WAR_RESOURCE_WOOD;
        }

        if (action->lastSoundStep == WAR_ACTION_STEP_SOUND_CHOPPING)
        {
            wa_createAudioRandomWithPosition(context, CREATE_AUDIO_ARGS_INIT(
                .randomFromId=WAR_TREE_CHOPPING_1,
                .randomToId=WAR_TREE_CHOPPING_4,
                .position=treePosition,
                .hasPosition=true,
                .loop=false
            ));
        }

        if (unit->amount == UNIT_MAX_CARRY_WOOD)
        {
            // set the carrying gold sprites
            const WarWorkerData* workerData = wu_getWorkerData(unit->type);
            we_removeSpriteComponent(context, entity);
            we_addSpriteComponentFromResource(context, entity, imageResourceRef(workerData->carryingWoodResource));

            // find the closest town hall to deliver the gold
            WarRace race = wu_getUnitRace(context, entity);
            WarUnitType townHallType = wu_getTownHallOfRace(race);
            WarEntity* townHall = we_findClosestUnitOfType(context, entity, townHallType);

            // if the town hall doesn't exists (it could be under attack and get destroyed), go idle
            if (!townHall)
            {
                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
                TracyCZoneEnd(ctx);
                return;
            }

            WarStateDeliver* deliverState = wst_createDeliverState(context, entity, townHall->id);
            deliverState->cycle = true;
            deliverState->sourceKind = WAR_RESOURCE_WOOD;
            deliverState->sourceId = forest->id;
            deliverState->sourcePosition = treePosition;
            wst_replaceState(context, entity, (WarStateBase*)deliverState, WAR_TRANSITION_CAUSE_COMPLETION);
        }

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }

    TracyCZoneEnd(ctx);
}


void wst_updateChoppingStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateChoppingStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateChopping*      states  = storage->chopping;
    bool*             occupied = storage->occupied[WAR_STATE_CHOPPING];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateChopping*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!wst_isCurrentState(context, entity, (WarStateBase*)state)) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateChoppingState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
