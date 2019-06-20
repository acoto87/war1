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
}

void leaveDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateDeathState(WarContext* context, WarEntity* entity, WarState* state)
{
    // when this state updates there will have pass the time of the death animation, 
    // using the delay field of the states
    if (!isCorpseUnit(entity))
    {
        vec2 position = entity->transform.position;
        vec2 tile = vec2MapToTileCoordinates(position);

        WarUnitType corpseType = getUnitRace(entity) == WAR_RACE_ORCS 
            ? WAR_UNIT_ORC_CORPSE : WAR_UNIT_HUMAN_CORPSE;

        WarEntity* corpse = createUnit(context, corpseType, tile.x, tile.y, 4, 
                                       WAR_RESOURCE_NONE, 0, true);

        WarState* deathState = createDeathState(context, corpse);
        deathState->delay = __frameCountToSeconds(1201);
        changeNextState(context, corpse, deathState, true, true);
    }
    
    removeEntityById(context, entity->id);
}

void freeDeathState(WarState* state)
{
}