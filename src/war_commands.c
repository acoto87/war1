// train units
void trainUnit(WarContext* context, WarUnitType unitToTrain, WarUnitType buildingUnit)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    
    assert(map->selectedEntities.count == 1);

    WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
    
    assert(selectedEntity && isBuildingUnit(selectedEntity));
    assert(selectedEntity->unit.type == buildingUnit);

    WarUnitStats stats = getUnitStats(unitToTrain);
    if (withdrawFromPlayer(context, player, stats.goldCost, stats.woodCost) &&
        checkFarmFood(context, player))
    {
        WarEntity* peasant = createUnit(context, unitToTrain, 0, 0, 0, WAR_RESOURCE_NONE, 0, true);
        WarState* buildingState = createBuildingUnitState(context, selectedEntity, peasant, getScaledTime(context, stats.buildTime));
        changeNextState(context, selectedEntity, buildingState, true, true);
    }
}

void trainFootman(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_FOOTMAN, WAR_UNIT_BARRACKS_HUMANS);
}

void trainGrunt(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_GRUNT, WAR_UNIT_BARRACKS_ORCS);
}

void trainPeasant(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_PEASANT, WAR_UNIT_TOWNHALL_HUMANS);
}

void trainPeon(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_PEON, WAR_UNIT_TOWNHALL_ORCS);
}

void trainHumanCatapult(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_CATAPULT_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void trainOrcCatapult(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_CATAPULT_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void trainKnight(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_KNIGHT, WAR_UNIT_BARRACKS_HUMANS);
}

void trainRaider(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_KNIGHT, WAR_UNIT_BARRACKS_ORCS);
}

void trainArcher(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_ARCHER, WAR_UNIT_BARRACKS_HUMANS);
}

void trainSpearman(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_SPEARMAN, WAR_UNIT_BARRACKS_ORCS);
}

void trainConjurer(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_CONJURER, WAR_UNIT_TOWER_HUMANS);
}

void trainWarlock(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_WARLOCK, WAR_UNIT_TOWER_ORCS);
}

void trainCleric(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_CLERIC, WAR_UNIT_CHURCH);
}

void trainNecrolyte(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UNIT_NECROLYTE, WAR_UNIT_TEMPLE);
}

// upgrades
void upgradeUpgrade(WarContext* context, WarUpgradeType upgradeToBuild, WarUnitType buildingUnit)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    
    assert(map->selectedEntities.count == 1);

    WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
    
    assert(selectedEntity && isBuildingUnit(selectedEntity));
    assert(selectedEntity->unit.type == buildingUnit);

    assert(hasRemainingUpgrade(player, upgradeToBuild));

    WarUpgradeStats stats = getUpgradeStats(WAR_UPGRADE_SWORDS);
    s32 level = getUpgradeLevel(player, upgradeToBuild);
    if (withdrawFromPlayer(context, player, stats.goldCost[level], 0))
    {
        WarState* buildingState = createBuildingUpgradeState(context, selectedEntity, upgradeToBuild, getScaledTime(context, stats.buildTime));
        changeNextState(context, selectedEntity, buildingState, true, true);
    }
}

void upgradeSwords(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_SWORDS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void upgradeAxes(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_AXES, WAR_UNIT_BLACKSMITH_ORCS);
}

void upgradeHumanShields(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_HUMANS);
}

void upgradeOrcsShields(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_ORCS);
}

void upgradeArrows(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_ARROWS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void upgradeSpears(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_SPEARS, WAR_UNIT_LUMBERMILL_ORCS);
}

void upgradeHorses(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_HORSES, WAR_UNIT_STABLE);
}

void upgradeWolves(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_WOLVES, WAR_UNIT_KENNEL);
}

void upgradeScorpions(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_SCORPIONS, WAR_UNIT_TOWER_HUMANS);
}

void upgradeSpiders(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_SPIDERS, WAR_UNIT_TOWER_ORCS);
}

void upgradeRainOfFire(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_RAIN_OF_FIRE, WAR_UNIT_TOWER_HUMANS);
}

void upgradePoisonCloud(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_POISON_CLOUD, WAR_UNIT_TOWER_ORCS);
}

void upgradeWaterElemental(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_WATER_ELEMENTAL, WAR_UNIT_TOWER_HUMANS);
}

void upgradeDaemon(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_DAEMON, WAR_UNIT_TOWER_ORCS);
}

void upgradeHealing(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_HEALING, WAR_UNIT_CHURCH);
}

void upgradeRaiseDead(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_RAISE_DEAD, WAR_UNIT_TEMPLE);
}

void upgradeFarSight(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_FAR_SIGHT, WAR_UNIT_CHURCH);
}

void upgradeDarkVision(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_DARK_VISION, WAR_UNIT_TEMPLE);
}

void upgradeInvisibility(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_INVISIBILITY, WAR_UNIT_CHURCH);
}

void upgradeUnholyArmor(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UPGRADE_UNHOLY_ARMOR, WAR_UNIT_TEMPLE);
}

// cancel
void cancelTrainOrUpgrade(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    
    assert(map->selectedEntities.count == 1);

    WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
    
    assert(selectedEntity && isBuildingUnit(selectedEntity));

    WarState* idleState = createIdleState(context, entity, false);
    changeNextState(context, selectedEntity, idleState, true, true);
}