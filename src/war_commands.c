void trainPeasant(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    
    assert(map->selectedEntities.count == 1);

    WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
    
    assert(selectedEntity && isUnit(selectedEntity));
    assert(selectedEntity->unit.type == WAR_UNIT_TOWNHALL_HUMANS);

    WarUnitStats stats = getUnitStats(WAR_UNIT_PEASANT);
    WarEntity* peasant = createUnit(context, WAR_UNIT_PEASANT, 0, 0, 0, WAR_RESOURCE_NONE, 0, true);
    WarState* buildingState = createBuildingState(context, selectedEntity, peasant, getScaledTime(context, stats.buildTime));
    changeNextState(context, selectedEntity, buildingState, true, true);
}

void trainFootman(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    
    assert(map->selectedEntities.count == 1);

    WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
    
    assert(selectedEntity && isUnit(selectedEntity));
    assert(selectedEntity->unit.type == WAR_UNIT_BARRACKS_HUMANS);

    WarUnitStats stats = getUnitStats(WAR_UNIT_FOOTMAN);
    WarEntity* peasant = createUnit(context, WAR_UNIT_FOOTMAN, 0, 0, 0, WAR_RESOURCE_NONE, 0, true);
    WarState* buildingState = createBuildingState(context, selectedEntity, peasant, getScaledTime(context, stats.buildTime));
    changeNextState(context, selectedEntity, buildingState, true, true);
}

void trainHumanCatapult(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    
    assert(map->selectedEntities.count == 1);

    WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
    
    assert(selectedEntity && isUnit(selectedEntity));
    assert(selectedEntity->unit.type == WAR_UNIT_BARRACKS_HUMANS);

    WarUnitStats stats = getUnitStats(WAR_UNIT_CATAPULT_HUMANS);
    WarEntity* peasant = createUnit(context, WAR_UNIT_CATAPULT_HUMANS, 0, 0, 0, WAR_RESOURCE_NONE, 0, true);
    WarState* buildingState = createBuildingState(context, selectedEntity, peasant, getScaledTime(context, stats.buildTime));
    changeNextState(context, selectedEntity, buildingState, true, true);
}

void trainKnight(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    
    assert(map->selectedEntities.count == 1);

    WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
    
    assert(selectedEntity && isUnit(selectedEntity));
    assert(selectedEntity->unit.type == WAR_UNIT_BARRACKS_HUMANS);

    WarUnitStats stats = getUnitStats(WAR_UNIT_KNIGHT);
    WarEntity* peasant = createUnit(context, WAR_UNIT_KNIGHT, 0, 0, 0, WAR_RESOURCE_NONE, 0, true);
    WarState* buildingState = createBuildingState(context, selectedEntity, peasant, getScaledTime(context, stats.buildTime));
    changeNextState(context, selectedEntity, buildingState, true, true);
}

void trainArcher(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    
    assert(map->selectedEntities.count == 1);

    WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
    
    assert(selectedEntity && isUnit(selectedEntity));
    assert(selectedEntity->unit.type == WAR_UNIT_BARRACKS_HUMANS);

    WarUnitStats stats = getUnitStats(WAR_UNIT_ARCHER);
    WarEntity* peasant = createUnit(context, WAR_UNIT_ARCHER, 0, 0, 0, WAR_RESOURCE_NONE, 0, true);
    WarState* buildingState = createBuildingState(context, selectedEntity, peasant, getScaledTime(context, stats.buildTime));
    changeNextState(context, selectedEntity, buildingState, true, true);
}
