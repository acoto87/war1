#include "war_state_machine.h"

#include "war_actions.h"
#include "war_audio.h"
#include "war_units.h"
#include "war_map.h"

WarState* wst_createChoppingState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_CHOPPING);
    state->chop.forestId = forestId;
    state->chop.position = position;
    return state;
}

void wst_enterChoppingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    vec2 unitSize = wun_getUnitSize(entity);
    vec2 position = wun_getUnitCenterPosition(entity, true);
    vec2 treePosition = state->chop.position;

    setStaticEntity(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
    wun_setUnitDirectionFromDiff(entity, treePosition.x - position.x, treePosition.y - position.y);
    wact_setAction(context, entity, WAR_ACTION_TYPE_HARVEST, true, 1.0f);
}

void wst_leaveChoppingState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updateChoppingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarUnitComponent* unit = &entity->unit;

    WarEntity* forest = went_findEntity(context, (WarEntityId)state->chop.forestId);

    // if the forest doesn't exists, go idle
    if (!forest)
    {
        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
        return;
    }

    vec2 treePosition = state->chop.position;
    WarTree* tree = went_getTreeAtPosition(forest, (s32)treePosition.x, (s32)treePosition.y);

    if (!tree || tree->amount == 0)
    {
        WarState* gatherWoodState = wst_createGatherWoodState(context, entity, forest->id, treePosition);
        wst_changeNextState(context, entity, gatherWoodState, true, true);
        return;
    }

    WarUnitAction* action = &unit->actions[unit->actionType];
    if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
    {
        unit->amount += went_chopTree(context, forest, tree, 2);
        if (unit->amount > 0)
        {
            unit->resourceKind = WAR_RESOURCE_WOOD;
        }

        if (action->lastSoundStep == WAR_ACTION_STEP_SOUND_CHOPPING)
        {
            vec2 targetPosition = wmap_vec2TileToMapCoordinates(treePosition, true);
            waud_createAudioRandomWithPosition(context, WAR_TREE_CHOPPING_1, WAR_TREE_CHOPPING_4, targetPosition, false);
        }

        if (unit->amount == UNIT_MAX_CARRY_WOOD)
        {
            // set the carrying gold sprites
            WarWorkerData workerData = wun_getWorkerData(unit->type);
            went_removeSpriteComponent(context, entity);
            went_addSpriteComponentFromResource(context, entity, imageResourceRef(workerData.carryingWoodResource));

            // find the closest town hall to wcmd_deliver the gold
            WarRace race = wun_getUnitRace(entity);
            WarUnitType townHallType = wun_getTownHallOfRace(race);
            WarEntity* townHall = went_findClosestUnitOfType(context, entity, townHallType);

            // if the town hall doesn't exists (it could be under wcmd_attack and get destroyed), go idle
            if (!townHall)
            {
                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);
                return;
            }

            WarState* deliverState = wst_createDeliverState(context, entity, townHall->id);
            deliverState->nextState = wst_createGatherWoodState(context, entity, forest->id, treePosition);
            wst_changeNextState(context, entity, deliverState, true, true);
        }

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }
}

void wst_freeChoppingState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}
