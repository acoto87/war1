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
    WarStateChopping* state = (WarStateChopping*)wst_deref(context, ref);
    state->forestId = forestId;
    state->position = position;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterChoppingState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterChoppingState", true);

    WarStateChopping* s = (WarStateChopping*)state;

    WarMap* map = context->map;

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wu_getUnitCenterPosition(context, entity, true);
    vec2 treePosition = s->position;

    setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
    wu_setUnitDirectionFromDiff(context, entity, treePosition.x - position.x, treePosition.y - position.y);
    wact_setAction(context, entity, WAR_ACTION_TYPE_HARVEST, true, 1.0f);

    TracyCZoneEnd(ctx);
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

    WarStateChopping* s = (WarStateChopping*)state;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    WarEntity* forest = we_findEntity(context, (WarEntityId)s->forestId);

    // if the forest doesn't exists, go idle
    if (!forest)
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
        TracyCZoneEnd(ctx);
        return;
    }

    vec2 treePosition = s->position;
    WarTree* tree = we_getTreeAtPosition(context, forest, (s32)treePosition.x, (s32)treePosition.y);

    if (!tree || tree->amount == 0)
    {
        WarStateWood* gatherWoodState = wst_createGatherWoodState(context, entity, forest->id, treePosition);
        wst_changeNextState(context, entity, (WarStateBase*)gatherWoodState, true, true);
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
            vec2 targetPosition = wmap_tileToMapCoordinatesV(treePosition, true);
            wa_createAudioRandomWithPosition(context, CREATE_AUDIO_ARGS_INIT(
                .randomFromId=WAR_TREE_CHOPPING_1,
                .randomToId=WAR_TREE_CHOPPING_4,
                .position=targetPosition,
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
                wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
                TracyCZoneEnd(ctx);
                return;
            }

            WarStateDeliver* deliverState = wst_createDeliverState(context, entity, townHall->id);
            wst_chainNext(context, (WarStateBase*)deliverState, (WarStateBase*)wst_createGatherWoodState(context, entity, forest->id, treePosition));
            wst_changeNextState(context, entity, (WarStateBase*)deliverState, true, true);
        }

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }

    TracyCZoneEnd(ctx);
}

void wst_freeChoppingState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeChoppingState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
