#include "war_state_machine.h"

#include "war_actions.h"
#include "war_units.h"
#include "war_pathfinder.h"

#include "TracyC.h"

WarStateRepairing* wst_createRepairingState(WarContext* context, WarEntity* entity, WarEntityId buildingId)
{
    TracyCZoneN(ctx, "wst_createRepairingState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_REPAIRING, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateRepairing* state = (WarStateRepairing*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->buildingId = buildingId;
    state->insideBuilding = false;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterRepairingState", true);

    WarMap* map = context->map;
    assert(map);

    WarStateRepairing* s = (WarStateRepairing*)state;

    WarEntity* building = we_findEntity(context, s->buildingId);

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || wst_isCollapsing(context, building) || wst_isGoingToCollapse(context, building))
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);

        TracyCZoneEnd(ctx);
        return;
    }

    // if the building needs to be built, enter the building and build it
    if (wst_isBuilding(context, building) || wst_isGoingToBuild(context, building))
    {
        WarStateBuild* buildState = wst_getBuildState(context, building);
        assert(buildState);

        // if there is already someone building it, go idle
        if (buildState->workerId)
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);

            TracyCZoneEnd(ctx);
            return;
        }

        we_disableComponent(context, entity, COMP_SPRITE);

        // set the unit as inside the building
        s->insideBuilding = true;

        // set up that this worker is the one building the building
        buildState->workerId = entity->id;
    }
    else
    {
        vec2 tile = wu_getUnitCenterTile(context, entity);
        vec2 targetTile = wu_getUnitCenterTile(context, building);
        vec2 unitSize = wu_getUnitSize(context, entity);

        wpath_setStaticEntity(&map->finder, (s32)tile.x, (s32)tile.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
        wu_setUnitDirectionFromDiff(context, entity, targetTile.x - tile.x, targetTile.y - tile.y);
        wact_setAction(context, entity, WAR_ACTION_TYPE_REPAIR, true, 1.0f);
    }

    TracyCZoneEnd(ctx);
}

void wst_exitRepairingState(WarContext* context, WarEntity* entity, WarState* state, WarStateExitReason reason)
{
    TracyCZoneN(ctx, "wst_exitRepairingState", true);

    NOT_USED(reason);

    WarMap* map = context->map;
    assert(map);

    WarStateRepairing* s = (WarStateRepairing*)state;

    WarEntity* building = we_findEntity(context, s->buildingId);
    if (building)
    {
        WarStateBuild* buildState = wst_getBuildState(context, building);
        if (buildState && buildState->workerId == entity->id)
        {
            buildState->workerId = 0;
        }
    }

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 tile = wu_getUnitCenterTile(context, entity);
    wpath_setFreeTiles(&map->finder, (s32)tile.x, (s32)tile.y, (s32)unitSize.x, (s32)unitSize.y);

    TracyCZoneEnd(ctx);
}

void wst_updateRepairingState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateRepairingState", true);

    WarMap* map = context->map;
    assert(map);

    WarStateRepairing* s = (WarStateRepairing*)state;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    WarEntity* building = we_findEntity(context, s->buildingId);

    // if the building doesn't exists or is collapsing (it could be attacked by other units), go idle
    if (!building || wst_isCollapsing(context, building) || wst_isGoingToCollapse(context, building))
    {
        if (s->insideBuilding)
        {
            we_enableComponent(context, entity, COMP_SPRITE);

            vec2 tile = wu_getUnitCenterTile(context, entity);
            vec2 spawnTile = wpath_findEmptyTile(&map->finder, (s32)tile.x, (s32)tile.y);
            wu_setUnitCenterTile(context, entity, spawnTile);
        }

        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);

        TracyCZoneEnd(ctx);
        return;
    }

    // if the worker is inside the building then he is building it
    // so don't make any repairing since new buildings always spawn with full hp
    if (!s->insideBuilding)
    {
        WarUnitAction* action = &unit->actions[unit->actionType];
        if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
        {
            WarPlayerInfo* player = wu_getOwningPlayer(context, entity);
            if (!we_decreasePlayerResources(context, player, 1, 1))
            {
                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
                TracyCZoneEnd(ctx);
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
            WarUnitComponent* buildingUnit = we_getUnitComponent(context, building);
            assert(buildingUnit);

            buildingUnit->hp += 9;

            if (buildingUnit->hp >= buildingUnit->maxhp)
            {
                buildingUnit->hp = buildingUnit->maxhp;

                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
            }

            action->lastActionStep = WAR_ACTION_STEP_NONE;
        }
    }
    else if (!wst_isBuilding(context, building) && !wst_isGoingToBuild(context, building))
    {
        we_enableComponent(context, entity, COMP_SPRITE);

        vec2 tile = wu_getUnitCenterTile(context, entity);
        vec2 spawnTile = wpath_findEmptyTile(&map->finder, (s32)tile.x, (s32)tile.y);
        wu_setUnitCenterTile(context, entity, spawnTile);

        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
    }

    TracyCZoneEnd(ctx);
}

void wst_updateRepairingStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateRepairingStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateRepairing*      states  = storage->repairing;
    bool*             occupied = storage->occupied[WAR_STATE_REPAIRING];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateRepairing*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (wst_getActiveState(context, entity) != (WarStateBase*)state) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateRepairingState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
