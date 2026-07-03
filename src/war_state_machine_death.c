#include "war_state_machine.h"

#include "war_actions.h"

#include "TracyC.h"

WarState* wst_createDeathState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_createDeathState", true);

    WarState* state = wst_createState(context, entity, WAR_STATE_DEATH);

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterDeathState", true);

    WarMap* map = context->map;

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 unitSize = wu_getUnitSize(context, entity);
    vec2 position = wmap_mapToTileCoordinatesV(transform->position);
    setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    wact_setAction(context, entity, WAR_ACTION_TYPE_DEATH, true, 1.0f);
    wmap_removeEntityFromSelection(context, entity->id);

    s32 deathDuration = wact_getActionDuration(context, entity, WAR_ACTION_TYPE_DEATH);
    state->delay = wmap_getMapScaledTime(context, __frameCountToSeconds(deathDuration));

    TracyCZoneEnd(ctx);
}

void wst_leaveDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveDeathState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateDeathState", true);

    NOT_USED(state);

    // when this state updates there will have pass the time of the death animation,
    // using the delay field of the states
    if (!wu_isCorpseUnit(context, entity) && !wu_isCatapultUnit(context, entity) &&
        !wu_isSummonUnit(context, entity) && !wu_isSkeletonUnit(context, entity))
    {
        vec2 position = wu_getUnitCenterPosition(context, entity, true);

        WarUnitType corpseType = wu_getUnitRace(context, entity) == WAR_RACE_ORCS
            ? WAR_UNIT_ORC_CORPSE : WAR_UNIT_HUMAN_CORPSE;

        WarEntity* corpse = we_createUnit(context, CREATE_UNIT_ARGS_INIT(
            .type=corpseType,
            .x=(s32)position.x,
            .y=(s32)position.y,
            .player=4,
            .resourceKind=WAR_RESOURCE_NONE,
            .amount=0,
            .addToMap=true
        ));

        wu_setUnitDirection(context, corpse, wu_getUnitDirection(context, entity));

        WarState* deathState = wst_createDeathState(context, corpse);
        wst_changeNextState(context, corpse, deathState, true, true);
    }

    we_removeEntityById(context, entity->id);

    TracyCZoneEnd(ctx);
}

void wst_freeDeathState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeDeathState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
