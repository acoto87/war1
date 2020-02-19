WarState* createDeathState(WarContext* context, WarEntity* entity)
{
    WarState* state = createState(context, entity, WAR_STATE_DEATH);
    return state;
}

void enterDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setFreeTiles(map->finder, position.x, position.y, unitSize.x, unitSize.y);
    setAction(context, entity, WAR_ACTION_TYPE_DEATH, true, 1.0f);
    removeEntityFromSelection(context, entity->id);

    s32 deathDuration = getActionDuration(entity, WAR_ACTION_TYPE_DEATH);
    setDelay(state, getMapScaledTime(context, __frameCountToSeconds(deathDuration)));
}

void leaveDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    // when this state updates there will have pass the time of the death animation,
    // using the delay field of the states
    if (!isCorpseUnit(entity) && !isCatapultUnit(entity) &&
        !isSummonUnit(entity) && !isSkeletonUnit(entity))
    {
        vec2 position = getUnitCenterPosition(entity, true);

        WarUnitType corpseType = getUnitRace(entity) == WAR_RACE_ORCS
            ? WAR_UNIT_ORC_CORPSE : WAR_UNIT_HUMAN_CORPSE;

        WarEntity* corpse = createUnit(context, corpseType, position.x, position.y, 4,
                                       WAR_RESOURCE_NONE, 0, true);

        setUnitDirection(corpse, getUnitDirection(entity));

        WarState* deathState = createDeathState(context, corpse);
        changeNextState(context, corpse, deathState, true, true);
    }

    removeEntityById(context, entity->id);
}

void freeDeathState(WarState* state)
{
}