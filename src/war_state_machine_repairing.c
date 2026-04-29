#include "war_state_machine.h"

#include "war_actions.h"
#include "war_units.h"
#include "war_pathfinder.h"

WarState* wst_createRepairingState(WarContext* context, WarEntity* entity, WarEntityId buildingId)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_REPAIRING);
    state->repairing.buildingId = buildingId;
    state->repairing.insideBuilding = false;
    return state;
}

void wst_enterRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarEntity* building = we_findEntity(context, state->repairing.buildingId);

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || isCollapsing(building) || isGoingToCollapse(building))
    {
        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);

        return;
    }

    // if the building needs to be built, enter the building and build it
    if (isBuilding(building) || isGoingToBuild(building))
    {
        WarState* buildState = getBuildState(building);
        assert(buildState);

        // if there is already someone building it, go idle
        if (buildState->build.workerId)
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);

            return;
        }

        // disable the sprite to simulate the building process
        entity->sprite.enabled = false;

        // set the unit as inside the building
        state->repairing.insideBuilding = true;

        // set up that this worker is the one building the building
        buildState->build.workerId = entity->id;
    }
    else
    {
        vec2 unitSize = wu_getUnitSize(entity);
        vec2 position = wu_getUnitCenterPosition(entity, true);
        vec2 targetPosition = wu_getUnitCenterPosition(building, true);

        setStaticEntity(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
        wu_setUnitDirectionFromDiff(entity, targetPosition.x - position.x, targetPosition.y - position.y);
        wact_setAction(context, entity, WAR_ACTION_TYPE_REPAIR, true, 1.0f);
    }
}

void wst_leaveRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    WarMap* map = context->map;

    vec2 unitSize = wu_getUnitSize(entity);
    vec2 position = wu_getUnitCenterPosition(entity, true);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
}

void wst_updateRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    WarUnitComponent* unit = &entity->unit;

    WarEntity* building = we_findEntity(context, state->repairing.buildingId);

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || isCollapsing(building) || isGoingToCollapse(building))
    {
        if (state->repairing.insideBuilding)
        {
            entity->sprite.enabled = true;

            // find a valid spawn position for the unit
            vec2 position = wu_getUnitCenterPosition(entity, true);
            vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);
            wu_setUnitCenterPosition(entity, spawnPosition, true);
        }

        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);

        return;
    }

    // if the worker is inside the building then he is building it
    // so don't make any repairing since new buildings always spawn with full hp
    if (!state->repairing.insideBuilding)
    {
        WarUnitAction* action = &unit->actions[unit->actionType];
        if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
        {
            if (!we_decreasePlayerResources(context, player, 1, 1))
            {
                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);
                return;
            }

            // to calculate the amount of wood and gold needed to repair a
            // building I'm taking the 12% of the damage of the building,
            // so for the a FARM if it has a damage of 200, the amount of
            // wood and gold would be 200 * 0.12 = 24.
            //
            // when repairing each second the amount of wood and gold decrease
            // in 1, so for each we need to increase the hp in the proportional
            // amount, in this case is 1 * 100 / 12 = 8.33 (rounding to 9 here)
            building->unit.hp += 9;

            if (building->unit.hp >= building->unit.maxhp)
            {
                building->unit.hp = building->unit.maxhp;

                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);
            }

            action->lastActionStep = WAR_ACTION_STEP_NONE;
        }
    }
    else if (!isBuilding(building) && !isGoingToBuild(building))
    {
        entity->sprite.enabled = true;

        // find a valid spawn position for the unit
        vec2 position = wu_getUnitCenterPosition(entity, true);
        vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);
        wu_setUnitCenterPosition(entity, spawnPosition, true);

        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
    }
}

void wst_freeRepairingState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}

