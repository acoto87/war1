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
    if (entity->unit.type != WAR_UNIT_HUMAN_CORPSE)
    {
        vec2 mapPosition = entity->transform.position;
        vec2 tilePosition = vec2MapToTileCoordinates(mapPosition);

        // TODO: check here for the race of the unit to spawn the correct corpse type
        WarUnitType corpseType = WAR_UNIT_HUMAN_CORPSE; // entity->unit.race == WAR_RACE_ORCS ? WAR_UNIT_ORC_CORPSE : WAR_UNIT_HUMAN_CORPSE;
        WarEntity* corpse = createEntity(context, WAR_ENTITY_TYPE_UNIT, true);
        addUnitComponent(context, corpse, corpseType, (s32)tilePosition.x, (s32)tilePosition.y, WAR_RACE_NEUTRAL, WAR_RESOURCE_NONE, 0);
        addTransformComponent(context, corpse, mapPosition);

        WarUnitData unitData = getUnitData(corpseType);

        s32 spriteIndex = unitData.resourceIndex;
        if (spriteIndex == 0)
        {
            logError("Sprite for unit of type %d is not configure properly. Default to footman sprite.\n", corpseType);
            spriteIndex = 279;
        }
        addSpriteComponentFromResource(context, corpse, imageResourceRef(spriteIndex));

        addUnitActions(corpse);
        addAnimationsComponent(context, corpse);
        addStateMachineComponent(context, corpse);

        WarState* deathState = createDeathState(context, corpse);
        deathState->delay = __frameCountToSeconds(1201);
        changeNextState(context, corpse, deathState, true, true);
    }
    
    removeEntityById(context, entity->id);
}

void freeDeathState(WarState* state)
{
}