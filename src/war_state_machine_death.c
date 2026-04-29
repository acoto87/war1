#include "war_state_machine.h"

#include "war_actions.h"

WarState* wst_createDeathState(WarContext* context, WarEntity* entity)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_DEATH);
    return state;
}

void wst_enterDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = wun_getUnitSize(entity);
    vec2 position = wmap_vec2MapToTileCoordinates(entity->transform.position);
    setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    wact_setAction(context, entity, WAR_ACTION_TYPE_DEATH, true, 1.0f);
    wmap_removeEntityFromSelection(context, entity->id);

    s32 deathDuration = wact_getActionDuration(entity, WAR_ACTION_TYPE_DEATH);
    setDelay(state, wmap_getMapScaledTime(context, __frameCountToSeconds(deathDuration)));
}

void wst_leaveDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updateDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    // when this state updates there will have pass the time of the death animation,
    // using the delay field of the states
    if (!wun_isCorpseUnit(entity) && !wun_isCatapultUnit(entity) &&
        !wun_isSummonUnit(entity) && !wun_isSkeletonUnit(entity))
    {
        vec2 position = wun_getUnitCenterPosition(entity, true);

        WarUnitType corpseType = wun_getUnitRace(entity) == WAR_RACE_ORCS
            ? WAR_UNIT_ORC_CORPSE : WAR_UNIT_HUMAN_CORPSE;

        WarEntity* corpse = went_createUnit(context, corpseType, (s32)position.x, (s32)position.y, 4,
                                       WAR_RESOURCE_NONE, 0, true);

        wun_setUnitDirection(corpse, wun_getUnitDirection(entity));

        WarState* deathState = wst_createDeathState(context, corpse);
        wst_changeNextState(context, corpse, deathState, true, true);
    }

    went_removeEntityById(context, entity->id);
}

void wst_freeDeathState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}
