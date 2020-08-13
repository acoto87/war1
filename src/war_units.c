bool isFriendlyUnit(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    return isUnit(entity) && entity->unit.player == player->index;
}

bool isEnemyUnit(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    if (!isUnit(entity))
        return false;

    if (entity->unit.player == player->index)
        return false;

    WarPlayerInfo* otherPlayer = &map->players[entity->unit.player];
    return !isNeutralPlayer(otherPlayer);
}

bool areEnemies(WarContext* context, WarEntity* entity1, WarEntity* entity2)
{
    WarMap* map = context->map;

    if (!isUnit(entity1) || !isUnit(entity2))
        return false;

	if (entity1->id == entity2->id)
		return false;

    if (entity1->unit.player == entity2->unit.player)
        return false;

    WarPlayerInfo* otherPlayer = &map->players[entity2->unit.player];
    return !isNeutralPlayer(otherPlayer);
}

bool canAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity)
{
    if (isWarriorUnit(entity) && !isDead(entity) && !isGoingToDie(entity))
    {
        if (isUnit(targetEntity))
        {
            if (!isDead(targetEntity) &&
                !isGoingToDie(targetEntity) &&
                !isCorpseUnit(targetEntity) &&
                !isCollapsing(entity) &&
                !isGoingToCollapse(entity))
            {
                return true;
            }
        }
        else if (isWall(targetEntity))
            return true;
    }

    return false;
}

bool isInsideBuilding(WarEntity* entity)
{
    if (isMining(entity))
    {
        return true;
    }

    if(isDelivering(entity))
    {
        WarState* deliver = getDeliverState(entity);
        return deliver->deliver.insideBuilding;
    }

    if (isRepairing2(entity))
    {
        WarState* repairing = getRepairingState(entity);
        return repairing->repairing.insideBuilding;
    }

    return false;
}

bool isWorkerBusy(WarEntity* entity)
{
    assert(isWorkerUnit(entity));

    return isRepairing(entity) || isInsideBuilding(entity);
}

bool displayUnitOnMinimap(WarEntity* entity)
{
    assert(isUnit(entity));

    if (isCorpseUnit(entity))
        return false;

    if (isDead(entity) || isGoingToDie(entity))
        return false;

    if (isCollapsing(entity) || isGoingToCollapse(entity))
        return false;

    return true;
}

u8Color getUnitColorOnMinimap(WarEntity* entity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    u8 r = 211, g = 211, b = 211;

    if (unit->type == WAR_UNIT_TOWNHALL_HUMANS ||
        unit->type == WAR_UNIT_TOWNHALL_ORCS)
    {
        r = 255; g = 255; b = 0;
    }
    else if(unit->player == 0)
    {
        r = 0; g = 199; b = 0;
    }
    else if(unit->player < 4)
    {
        r = 199; g = 0; b = 0;
    }

    return u8RgbColor(r, g, b);
}

s32 getTotalNumberOfUnits(WarContext* context, u8 player)
{
    s32 count = 0;

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            if (entity->unit.player == player)
            {
                count++;
            }
        }
    }

    return count;
}

s32 getTotalNumberOfDudes(WarContext* context, u8 player)
{
    s32 count = 0;

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity && isDudeUnit(entity) && !isSummonUnit(entity) && !isSkeletonUnit(entity))
        {
            if (entity->unit.player == player)
            {
                count++;
            }
        }
    }

    return count;
}

s32 getTotalNumberOfBuildings(WarContext* context, u8 player, bool alreadyBuilt)
{
    s32 count = 0;

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            if (entity->unit.player == player && isBuildingUnit(entity))
            {
                if (alreadyBuilt && (isBuilding(entity) || isGoingToBuild(entity)))
                    continue;

                count++;
            }
        }
    }

    return count;
}

s32 getNumberOfBuildingsOfType(WarContext* context, u8 player, WarUnitType unitType, bool alreadyBuilt)
{
    assert(isBuildingUnitType(unitType));

    s32 count = 0;

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            if (entity->unit.player == player &&
                entity->unit.type == unitType)
            {
                if (alreadyBuilt && (isBuilding(entity) || isGoingToBuild(entity)))
                    continue;

                count++;
            }
        }
    }

    return count;
}

s32 getNumberOfUnitsOfType(WarContext* context, u8 player, WarUnitType unitType)
{
    s32 count = 0;

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            if (entity->unit.player == player &&
                entity->unit.type == unitType)
            {
                count++;
            }
        }
    }

    return count;
}

bool isInSquad(WarContext* context, WarPlayerInfo* player, WarEntityId entityId)
{
    for (s32 i = 0; i < MAX_SQUAD_COUNT; i++)
    {
        WarSquad* squad = &player->squads[i];
        for (s32 j = 0; j < squad->count; j++)
        {
            if (squad->units[j] == entityId)
                return true;
        }
    }

    return false;
}

bool isDudeUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_PEASANT:
        case WAR_UNIT_PEON:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_MEDIVH:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_WOUNDED:
        case WAR_UNIT_GRIZELDA:
        case WAR_UNIT_GARONA:
        case WAR_UNIT_OGRE:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_SLIME:
        case WAR_UNIT_FIRE_ELEMENTAL:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_BRIGAND:
        case WAR_UNIT_THE_DEAD:
        case WAR_UNIT_SKELETON:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_WATER_ELEMENTAL:
            return true;

        default:
            return false;
    }
}

bool isBuildingUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FARM_HUMANS:
        case WAR_UNIT_FARM_ORCS:
        case WAR_UNIT_BARRACKS_HUMANS:
        case WAR_UNIT_BARRACKS_ORCS:
        case WAR_UNIT_CHURCH:
        case WAR_UNIT_TEMPLE:
        case WAR_UNIT_TOWER_HUMANS:
        case WAR_UNIT_TOWER_ORCS:
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_TOWNHALL_ORCS:
        case WAR_UNIT_LUMBERMILL_HUMANS:
        case WAR_UNIT_LUMBERMILL_ORCS:
        case WAR_UNIT_STABLE:
        case WAR_UNIT_KENNEL:
        case WAR_UNIT_BLACKSMITH_HUMANS:
        case WAR_UNIT_BLACKSMITH_ORCS:
        case WAR_UNIT_STORMWIND:
        case WAR_UNIT_BLACKROCK:
        case WAR_UNIT_GOLDMINE:
            return true;

        default:
            return false;
    }
}

bool isWorkerUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_PEASANT:
        case WAR_UNIT_PEON:
            return true;

        default:
            return false;
    }
}

bool isWarriorUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_MEDIVH:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_WOUNDED:
        case WAR_UNIT_OGRE:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_SLIME:
        case WAR_UNIT_FIRE_ELEMENTAL:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_BRIGAND:
        case WAR_UNIT_THE_DEAD:
        case WAR_UNIT_SKELETON:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_WATER_ELEMENTAL:
            return true;

        default:
            return false;
    }
}

bool isRangeUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_MEDIVH:
        case WAR_UNIT_WATER_ELEMENTAL:
            return true;

        default:
            return false;
    }
}

bool isMeleeUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_OGRE:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_SLIME:
        case WAR_UNIT_FIRE_ELEMENTAL:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_BRIGAND:
        case WAR_UNIT_SKELETON:
        case WAR_UNIT_DAEMON:
            return true;

        default:
            return false;
    }
}

bool isFistUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_OGRE:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_SLIME:
        case WAR_UNIT_FIRE_ELEMENTAL:
        case WAR_UNIT_SCORPION:
            return true;

        default:
            return false;
    }
}

bool isSwordUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_BRIGAND:
        case WAR_UNIT_SKELETON:
        case WAR_UNIT_DAEMON:
            return true;

        default:
            return false;
    }
}

bool isMagicUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_WATER_ELEMENTAL:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_THE_DEAD:
            return true;

        default:
            return false;
    }
}

bool isCorpseUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_HUMAN_CORPSE:
        case WAR_UNIT_ORC_CORPSE:
            return true;

        default:
            return false;
    }
}

bool isCatapultUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
            return true;

        default:
            return false;
    }
}

bool isConjurerOrWarlockUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
            return true;

        default:
            return false;
    }
}

bool isClericOrNecrolyteUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
            return true;

        default:
            return false;
    }
}

bool isSummonUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_WATER_ELEMENTAL:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_THE_DEAD:
            return true;

        default:
            return false;
    }
}

bool isSkeletonUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_SKELETON:
            return true;

        default:
            return false;
    }
}

bool isGoldmineUnitType(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_GOLDMINE:
            return true;

        default:
            return false;
    }
}

bool isDudeUnit(WarEntity* entity)
{
    return isUnit(entity) && isDudeUnitType(entity->unit.type);
}

bool isBuildingUnit(WarEntity* entity)
{
    return isUnit(entity) && isBuildingUnitType(entity->unit.type);
}

bool isWorkerUnit(WarEntity* entity)
{
    return isUnit(entity) && isWorkerUnitType(entity->unit.type);
}

bool isWarriorUnit(WarEntity* entity)
{
    return isUnit(entity) && isWarriorUnitType(entity->unit.type);
}

bool isRangeUnit(WarEntity* entity)
{
    return isUnit(entity) && isRangeUnitType(entity->unit.type);
}

bool isMeleeUnit(WarEntity* entity)
{
    return isUnit(entity) && isMeleeUnitType(entity->unit.type);
}

bool isFistUnit(WarEntity* entity)
{
    return isUnit(entity) && isFistUnitType(entity->unit.type);
}

bool isSwordUnit(WarEntity* entity)
{
    return isUnit(entity) && isSwordUnitType(entity->unit.type);
}

bool isMagicUnit(WarEntity* entity)
{
    return isUnit(entity) && isMagicUnitType(entity->unit.type);
}

bool isCorpseUnit(WarEntity* entity)
{
    return isUnit(entity) && isCorpseUnitType(entity->unit.type);
}

bool isCatapultUnit(WarEntity* entity)
{
    return isUnit(entity) && isCatapultUnitType(entity->unit.type);
}

bool isConjurerOrWarlockUnit(WarEntity* entity)
{
    return isUnit(entity) && isConjurerOrWarlockUnitType(entity->unit.type);
}

bool isClericOrNecrolyteUnit(WarEntity* entity)
{
    return isUnit(entity) && isClericOrNecrolyteUnitType(entity->unit.type);
}

bool isSummonUnit(WarEntity* entity)
{
    return isUnit(entity) && isSummonUnitType(entity->unit.type);
}

bool isSkeletonUnit(WarEntity* entity)
{
    return isUnit(entity) && isSkeletonUnitType(entity->unit.type);
}

bool isGoldmineUnit(WarEntity* entity)
{
    return isUnit(entity) && isGoldmineUnitType(entity->unit.type);
}

WarRace getUnitTypeRace(WarUnitType type)
{
    switch (type)
    {
        // units
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_PEASANT:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_LOTHAR:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_WATER_ELEMENTAL:
        case WAR_UNIT_HUMAN_CORPSE:
        // buildings
        case WAR_UNIT_FARM_HUMANS:
        case WAR_UNIT_BARRACKS_HUMANS:
        case WAR_UNIT_CHURCH:
        case WAR_UNIT_TOWER_HUMANS:
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_LUMBERMILL_HUMANS:
        case WAR_UNIT_STABLE:
        case WAR_UNIT_BLACKSMITH_HUMANS:
        case WAR_UNIT_STORMWIND:
            return WAR_RACE_HUMANS;

        // units
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_PEON:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_GRIZELDA:
        case WAR_UNIT_GARONA:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_ORC_CORPSE:
        // buildings
        case WAR_UNIT_FARM_ORCS:
        case WAR_UNIT_BARRACKS_ORCS:
        case WAR_UNIT_TEMPLE:
        case WAR_UNIT_TOWER_ORCS:
        case WAR_UNIT_TOWNHALL_ORCS:
        case WAR_UNIT_LUMBERMILL_ORCS:
        case WAR_UNIT_KENNEL:
        case WAR_UNIT_BLACKSMITH_ORCS:
        case WAR_UNIT_BLACKROCK:
            return WAR_RACE_ORCS;

        default:
            return WAR_RACE_NEUTRAL;
    }
}

WarRace getUnitRace(WarEntity* entity)
{
    if (!isUnit(entity))
        return WAR_RACE_NEUTRAL;

    return getUnitTypeRace(entity->unit.type);
}

WarRace getUpgradeTypeRace(WarUpgradeType type)
{
    switch (type)
    {
        case WAR_UPGRADE_ARROWS:
        case WAR_UPGRADE_SWORDS:
        case WAR_UPGRADE_HORSES:
        case WAR_UPGRADE_SCORPIONS:
        case WAR_UPGRADE_RAIN_OF_FIRE:
        case WAR_UPGRADE_WATER_ELEMENTAL:
        case WAR_UPGRADE_HEALING:
        case WAR_UPGRADE_FAR_SIGHT:
        case WAR_UPGRADE_INVISIBILITY:
            return WAR_RACE_HUMANS;

        case WAR_UPGRADE_SPEARS:
        case WAR_UPGRADE_AXES:
        case WAR_UPGRADE_WOLVES:
        case WAR_UPGRADE_SPIDERS:
        case WAR_UPGRADE_POISON_CLOUD:
        case WAR_UPGRADE_DAEMON:
        case WAR_UPGRADE_RAISE_DEAD:
        case WAR_UPGRADE_DARK_VISION:
        case WAR_UPGRADE_UNHOLY_ARMOR:
            return WAR_RACE_ORCS;

        case WAR_UPGRADE_SHIELD:
            return WAR_RACE_NEUTRAL;

        default:
            return WAR_RACE_NEUTRAL;
    }
}

WarUnitType getUnitTypeForRace(WarUnitType type, WarRace race)
{
    if (race == WAR_RACE_HUMANS)
    {
        switch (type)
        {
            case WAR_UNIT_GRUNT: return WAR_UNIT_FOOTMAN;
            case WAR_UNIT_PEON: return WAR_UNIT_PEASANT;
            case WAR_UNIT_CATAPULT_ORCS: return WAR_UNIT_CATAPULT_HUMANS;
            case WAR_UNIT_RAIDER: return WAR_UNIT_KNIGHT;
            case WAR_UNIT_SPEARMAN: return WAR_UNIT_ARCHER;
            case WAR_UNIT_WARLOCK: return WAR_UNIT_CONJURER;
            case WAR_UNIT_NECROLYTE: return WAR_UNIT_CLERIC;
            case WAR_UNIT_FARM_ORCS: return WAR_UNIT_FARM_HUMANS;
            case WAR_UNIT_BARRACKS_ORCS: return WAR_UNIT_BARRACKS_HUMANS;
            case WAR_UNIT_TEMPLE: return WAR_UNIT_CHURCH;
            case WAR_UNIT_TOWER_ORCS: return WAR_UNIT_TOWER_HUMANS;
            case WAR_UNIT_TOWNHALL_ORCS: return WAR_UNIT_TOWNHALL_HUMANS;
            case WAR_UNIT_LUMBERMILL_ORCS: return WAR_UNIT_LUMBERMILL_HUMANS;
            case WAR_UNIT_KENNEL: return WAR_UNIT_STABLE;
            case WAR_UNIT_BLACKSMITH_ORCS: return WAR_UNIT_BLACKSMITH_HUMANS;
            case WAR_UNIT_ORC_CORPSE: return WAR_UNIT_HUMAN_CORPSE;
            default: return type;
        }
    }

    if (race == WAR_RACE_ORCS)
    {
        switch (type)
        {
            case WAR_UNIT_FOOTMAN: return WAR_UNIT_GRUNT;
            case WAR_UNIT_PEASANT: return WAR_UNIT_PEON;
            case WAR_UNIT_CATAPULT_HUMANS: return WAR_UNIT_CATAPULT_ORCS;
            case WAR_UNIT_KNIGHT: return WAR_UNIT_RAIDER;
            case WAR_UNIT_ARCHER: return WAR_UNIT_SPEARMAN;
            case WAR_UNIT_CONJURER: return WAR_UNIT_WARLOCK;
            case WAR_UNIT_CLERIC: return WAR_UNIT_NECROLYTE;
            case WAR_UNIT_FARM_HUMANS: return WAR_UNIT_FARM_ORCS;
            case WAR_UNIT_BARRACKS_HUMANS: return WAR_UNIT_BARRACKS_ORCS;
            case WAR_UNIT_CHURCH: return WAR_UNIT_TEMPLE;
            case WAR_UNIT_TOWER_HUMANS: return WAR_UNIT_TOWER_ORCS;
            case WAR_UNIT_TOWNHALL_HUMANS: return WAR_UNIT_TOWNHALL_ORCS;
            case WAR_UNIT_LUMBERMILL_HUMANS: return WAR_UNIT_LUMBERMILL_ORCS;
            case WAR_UNIT_STABLE: return WAR_UNIT_KENNEL;
            case WAR_UNIT_BLACKSMITH_HUMANS: return WAR_UNIT_BLACKSMITH_ORCS;
            case WAR_UNIT_HUMAN_CORPSE: return WAR_UNIT_ORC_CORPSE;
            default: return type;
        }
    }

    logInfo("Trying to get unit type %d for race %d, returning %d\n", type, race, type);
    return type;
}

WarUpgradeType getUpgradeTypeForRace(WarUpgradeType type, WarRace race)
{
    if (race == WAR_RACE_HUMANS)
    {
        switch (type)
        {
            case WAR_UPGRADE_SPEARS: return WAR_UPGRADE_ARROWS;
            case WAR_UPGRADE_AXES: return WAR_UPGRADE_SWORDS;
            case WAR_UPGRADE_WOLVES: return WAR_UPGRADE_HORSES;
            case WAR_UPGRADE_SPIDERS: return WAR_UPGRADE_SCORPIONS;
            case WAR_UPGRADE_POISON_CLOUD: return WAR_UPGRADE_RAIN_OF_FIRE;
            case WAR_UPGRADE_DAEMON: return WAR_UPGRADE_WATER_ELEMENTAL;
            case WAR_UPGRADE_RAISE_DEAD: return WAR_UPGRADE_HEALING;
            case WAR_UPGRADE_DARK_VISION: return WAR_UPGRADE_FAR_SIGHT;
            case WAR_UPGRADE_UNHOLY_ARMOR: return WAR_UPGRADE_INVISIBILITY;
            default: return type;
        }
    }

    if (race == WAR_RACE_ORCS)
    {
        switch (type)
        {
            case WAR_UPGRADE_ARROWS: return WAR_UPGRADE_SPEARS;
            case WAR_UPGRADE_SWORDS: return WAR_UPGRADE_AXES;
            case WAR_UPGRADE_HORSES: return WAR_UPGRADE_WOLVES;
            case WAR_UPGRADE_SCORPIONS: return WAR_UPGRADE_SPIDERS;
            case WAR_UPGRADE_RAIN_OF_FIRE: return WAR_UPGRADE_POISON_CLOUD;
            case WAR_UPGRADE_WATER_ELEMENTAL: return WAR_UPGRADE_DAEMON;
            case WAR_UPGRADE_HEALING: return WAR_UPGRADE_RAISE_DEAD;
            case WAR_UPGRADE_FAR_SIGHT: return WAR_UPGRADE_DARK_VISION;
            case WAR_UPGRADE_INVISIBILITY: return WAR_UPGRADE_UNHOLY_ARMOR;
            default: return type;
        }
    }

    logInfo("Trying to get upgrade type %d for race %d, returning %d\n", type, race, type);
    return type;
}

WarProjectileType getProjectileType(WarUnitType type)
{
    assert(isRangeUnitType(type));

    switch (type)
    {
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        {
            return WAR_PROJECTILE_ARROW;
        }

        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        {
            return WAR_PROJECTILE_CATAPULT;
        }

        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        {
            return WAR_PROJECTILE_FIREBALL;
        }

        case WAR_UNIT_WATER_ELEMENTAL:
        {
            return WAR_PROJECTILE_WATER_ELEMENTAL;
        }

        default:
        {
            // unreachable
            logWarning("Invalid unit firing a projectile: %d\n", type);
            return WAR_PROJECTILE_ARROW;
        }
    }
}

WarUnitType getTownHallOfRace(WarRace race)
{
    switch (race)
    {
        case WAR_RACE_HUMANS: return WAR_UNIT_TOWNHALL_HUMANS;
        case WAR_RACE_ORCS: return WAR_UNIT_TOWNHALL_ORCS;
        default: return WAR_UNIT_TOWNHALL_HUMANS;
    }
}

WarUnitType getUnitTypeProducer(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_PEASANT:
            return WAR_UNIT_TOWNHALL_HUMANS;
        case WAR_UNIT_PEON:
            return WAR_UNIT_TOWNHALL_ORCS;

        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_ARCHER:
            return WAR_UNIT_BARRACKS_HUMANS;

        case WAR_UNIT_GRUNT:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_SPEARMAN:
            return WAR_UNIT_BARRACKS_ORCS;

        case WAR_UNIT_CONJURER:
            return WAR_UNIT_TOWER_HUMANS;
        case WAR_UNIT_WARLOCK:
            return WAR_UNIT_TOWER_ORCS;
        case WAR_UNIT_CLERIC:
            return WAR_UNIT_CHURCH;
        case WAR_UNIT_NECROLYTE:
            return WAR_UNIT_TEMPLE;

        default:
        {
            logWarning("There is not producer unit for unit type %d\n", type);
            return -1;
        }
    }
}

WarUnitType getUpgradeTypeProducer(WarUpgradeType type, WarRace race)
{
    switch (type)
    {
        case WAR_UPGRADE_SWORDS:
            return WAR_UNIT_BLACKSMITH_HUMANS;

        case WAR_UPGRADE_AXES:
            return WAR_UNIT_BLACKSMITH_ORCS;

        case WAR_UPGRADE_ARROWS:
            return WAR_UNIT_LUMBERMILL_HUMANS;

        case WAR_UPGRADE_SPEARS:
            return WAR_UNIT_LUMBERMILL_ORCS;

        case WAR_UPGRADE_HORSES:
            return WAR_UNIT_STABLE;

        case WAR_UPGRADE_WOLVES:
            return WAR_UNIT_KENNEL;

        case WAR_UPGRADE_HEALING:
        case WAR_UPGRADE_FAR_SIGHT:
        case WAR_UPGRADE_INVISIBILITY:
            return WAR_UNIT_CHURCH;

        case WAR_UPGRADE_RAISE_DEAD:
        case WAR_UPGRADE_DARK_VISION:
        case WAR_UPGRADE_UNHOLY_ARMOR:
            return WAR_UNIT_TEMPLE;

        case WAR_UPGRADE_SCORPIONS:
        case WAR_UPGRADE_RAIN_OF_FIRE:
        case WAR_UPGRADE_WATER_ELEMENTAL:
            return WAR_UNIT_TOWER_HUMANS;

        case WAR_UPGRADE_SPIDERS:
        case WAR_UPGRADE_POISON_CLOUD:
        case WAR_UPGRADE_DAEMON:
            return WAR_UNIT_TOWER_ORCS;

        case WAR_UPGRADE_SHIELD:
            return race == WAR_RACE_HUMANS
                ? WAR_UNIT_BLACKSMITH_HUMANS
                : WAR_UNIT_BLACKSMITH_ORCS;

        default:
        {
            logWarning("There is not producer unit for upgrade type %d\n", type);
            return -1;
        }
    }
}

WarUnitType getSpellCasterType(WarSpellType type)
{
    switch (type)
    {
        // spells
        case WAR_SPELL_HEALING:
        case WAR_SPELL_FAR_SIGHT:
        case WAR_SPELL_INVISIBILITY:
            return WAR_UNIT_CLERIC;

        case WAR_SPELL_RAIN_OF_FIRE:
        case WAR_SUMMON_SCORPION:
        case WAR_SUMMON_WATER_ELEMENTAL:
            return WAR_UNIT_CONJURER;

        case WAR_SPELL_RAISE_DEAD:
        case WAR_SPELL_DARK_VISION:
        case WAR_SPELL_UNHOLY_ARMOR:
            return WAR_UNIT_NECROLYTE;

        case WAR_SPELL_POISON_CLOUD:
        case WAR_SUMMON_SPIDER:
        case WAR_SUMMON_DAEMON:
            return WAR_UNIT_WARLOCK;

        default:
        {
            logWarning("Trying to get spell caster of spell type: %d\n", type);
            assert(false);
            return WAR_UNIT_FOOTMAN;
        }
    }
}

vec2 getUnitSize(WarEntity* entity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    return vec2i(unit->sizex, unit->sizey);
}

vec2 getUnitFrameSize(WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    return vec2i(sprite->sprite.frameWidth, sprite->sprite.frameHeight);
}

rect getUnitFrameRect(WarEntity* entity)
{
    return rectv(VEC2_ZERO, getUnitFrameSize(entity));
}

vec2 getUnitSpriteSize(WarEntity* entity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    return vec2i(unit->sizex * MEGA_TILE_WIDTH, unit->sizey * MEGA_TILE_HEIGHT);
}

rect getUnitSpriteRect(WarEntity* entity)
{
    assert(isUnit(entity));

    vec2 frameSize = getUnitFrameSize(entity);
    vec2 unitSize = getUnitSpriteSize(entity);
    vec2 pos = vec2Half(vec2Subv(frameSize, unitSize));
    return rectv(pos, unitSize);
}

vec2 getUnitSpriteCenter(WarEntity* entity)
{
    assert(isUnit(entity));

    vec2 frameSize = getUnitFrameSize(entity);
    vec2 unitSize = getUnitSpriteSize(entity);
    vec2 pos = vec2Half(vec2Subv(frameSize, unitSize));
    return vec2Addv(pos, vec2Half(unitSize));
}

rect getUnitRect(WarEntity* entity)
{
    assert(isUnit(entity));

    return rectv(entity->transform.position, getUnitSpriteSize(entity));
}

vec2 getEntityPosition(WarEntity* entity, bool inTiles)
{
    vec2 position = entity->transform.position;
    return inTiles ? vec2MapToTileCoordinates(position) : position;
}

vec2 getUnitCenterPosition(WarEntity* entity, bool inTiles)
{
    assert(isUnit(entity));

    WarTransformComponent* transform = &entity->transform;
    vec2 spriteSize = getUnitSpriteSize(entity);
    vec2 unitCenter = vec2Half(spriteSize);
    vec2 position = vec2Addv(transform->position, unitCenter);
    return inTiles ? vec2MapToTileCoordinates(position) : position;
}

void setEntityPosition(WarEntity* entity, vec2 position, bool inTiles)
{
    if (inTiles)
    {
        position = vec2TileToMapCoordinates(position, true);
    }

    entity->transform.position = position;
}

void setUnitCenterPosition(WarEntity* entity, vec2 position, bool inTiles)
{
    assert(isUnit(entity));

    if (inTiles)
    {
        position = vec2TileToMapCoordinates(position, true);
    }

    WarTransformComponent* transform = &entity->transform;
    vec2 spriteSize = getUnitSpriteSize(entity);
    vec2 unitCenter = vec2Half(spriteSize);
    transform->position = vec2Subv(position, unitCenter);
}

WarUnitDirection getUnitDirection(WarEntity* entity)
{
    assert(isUnit(entity));

    return entity->unit.direction;
}

WarUnitDirection getDirectionFromDiff(f32 x, f32 y)
{
    if (x < 0 && y < 0)
        return WAR_DIRECTION_NORTH_WEST;
    if (x == 0 && y < 0)
        return WAR_DIRECTION_NORTH;
    if (x > 0 && y < 0)
        return WAR_DIRECTION_NORTH_EAST;

    if (x < 0 && y == 0)
        return WAR_DIRECTION_WEST;
    if (x > 0 && y == 0)
        return WAR_DIRECTION_EAST;

    if (x < 0 && y > 0)
        return WAR_DIRECTION_SOUTH_WEST;
    if (x == 0 && y > 0)
        return WAR_DIRECTION_SOUTH;
    if (x > 0 && y > 0)
        return WAR_DIRECTION_SOUTH_EAST;

    return WAR_DIRECTION_NORTH;
}

void setUnitDirection(WarEntity* entity, WarUnitDirection direction)
{
    assert(isUnit(entity));

    entity->unit.direction = direction;
}

void setUnitDirectionFromDiff(WarEntity* entity, f32 dx, f32 dy)
{
    assert(isUnit(entity));

    WarUnitDirection direction = getDirectionFromDiff(dx, dy);
    setUnitDirection(entity, direction);
}

f32 getUnitActionScale(WarEntity* entity)
{
    assert(isUnit(entity));

    // this is the scale of the animation, for walking
    // the lower the less time is the transition between the frames
    // thus the animation goes faster.
    //
    // speed 0 -> 1.0f
    // speed 1 -> 0.9f
    // speed 2 -> 0.8f
    return 1 - entity->unit.speed * 0.1f;
}

vec2 unitPointOnTarget(WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));
    assert(isUnit(targetEntity));

    vec2 position = getUnitCenterPosition(entity, true);

    vec2 targetPosition = vec2MapToTileCoordinates(targetEntity->transform.position);
    vec2 unitSize = getUnitSize(targetEntity);
    rect unitRect = rectv(targetPosition, unitSize);

    return getClosestPointOnRect(position, unitRect);
}

s32 unitDistanceInTilesToPosition(WarEntity* entity, vec2 targetPosition)
{
    assert(isUnit(entity));

    vec2 position = getUnitCenterPosition(entity, true);
    f32 distance = vec2DistanceInTiles(position, targetPosition);
    return (s32)distance;
}

bool unitTileInRange(WarEntity* entity, vec2 targetTile, s32 range)
{
    assert(isUnit(entity));
    assert(range >= 0);

    s32 distance = unitDistanceInTilesToPosition(entity, targetTile);
    return distance <= range;
}

s32 unitDistanceInTilesToUnit(WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));
    assert(isUnit(targetEntity));

    vec2 pointOnTarget = unitPointOnTarget(entity, targetEntity);
    return unitDistanceInTilesToPosition(entity, pointOnTarget);
}

bool unitInRange(WarEntity* entity, WarEntity* targetEntity, s32 range)
{
    assert(isUnit(entity));
    assert(isUnit(targetEntity));
    assert(range >= 0);

    s32 distance = unitDistanceInTilesToUnit(entity, targetEntity);
    return distance <= range;
}

bool isCarryingResources(WarEntity* entity)
{
    assert(entity);
    assert(isUnit(entity));

    switch (entity->unit.resourceKind)
    {
        case WAR_RESOURCE_GOLD: return entity->unit.amount == UNIT_MAX_CARRY_WOOD;
        case WAR_RESOURCE_WOOD: return entity->unit.amount == UNIT_MAX_CARRY_GOLD;
        default: return false;
    }
}

s32 getUnitSightRange(WarEntity* entity)
{
    assert(isUnit(entity));

    s32 sight = 0;

    if (isBuildingUnit(entity))
    {
        WarBuildingStats stats = getBuildingStats(entity->unit.type);
        sight = stats.sight;
    }
    else
    {
        WarUnitStats stats = getUnitStats(entity->unit.type);
        sight = stats.sight;
    }

    return sight;
}

WarUnitGroup createUnitGroup(s32 count, WarEntityId unitIds[])
{
    WarUnitGroup unitGroup = (WarUnitGroup){0};
    unitGroup.count = count;
    memcpy(unitGroup.unitIds, unitIds, count * sizeof(WarEntityId));
    return unitGroup;
}

WarUnitGroup createUnitGroupFromSquad(WarSquad* squad)
{
    assert(squad);
    return createUnitGroup(squad->count, squad->units);
}

WarUnitGroup createUnitGroupFromSelection(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    return createUnitGroup(map->selectedEntities.count, map->selectedEntities.items);
}

WarSquad createSquadFromUnitGroup(WarSquadId squadId, WarUnitGroup unitGroup)
{
    WarSquad squad = (WarSquad){0};
    squad.id = squadId;
    squad.count = unitGroup.count;
    memcpy(squad.units, unitGroup.unitIds, unitGroup.count * sizeof(WarEntityId));
    return squad;
}

void getUnitCommands(WarContext* context, WarEntity* entity, WarUICommandType commands[])
{
    assert(entity);
    assert(isUnit(entity));

    WarMap* map = context->map;
    WarUICommand* command = &map->uiCommand;
    WarPlayerInfo* player = &map->players[0];

    WarUnitComponent* unit = &entity->unit;

    if (player->race != getUnitRace(entity))
    {
        return;
    }

    switch (unit->type)
    {
        // units
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_WATER_ELEMENTAL:
        case WAR_UNIT_DAEMON:
        case WAR_UNIT_SCORPION:
        case WAR_UNIT_SPIDER:
        case WAR_UNIT_SKELETON:
        {
            if (command->type != WAR_UI_COMMAND_NONE)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_MOVE;
                commands[1] = WAR_UI_COMMAND_STOP;
                commands[2] = WAR_UI_COMMAND_ATTACK;
            }

            break;
        }

        case WAR_UNIT_PEASANT:
        {
            if (command->type != WAR_UI_COMMAND_NONE)
            {
                if (command->type == WAR_UI_COMMAND_BUILD_BASIC)
                {
                    commands[0] = WAR_UI_COMMAND_BUILD_FARM_HUMANS;

                    if (isFeatureAllowed(player, WAR_FEATURE_UNIT_LUMBER_MILL_HUMANS))
                    {
                        commands[1] = WAR_UI_COMMAND_BUILD_LUMBERMILL_HUMANS;
                    }

                    commands[2] = WAR_UI_COMMAND_BUILD_BARRACKS_HUMANS;
                }
                else if (command->type == WAR_UI_COMMAND_BUILD_ADVANCED)
                {
                    commands[0] = WAR_UI_COMMAND_BUILD_BLACKSMITH_HUMANS;
                    commands[1] = WAR_UI_COMMAND_BUILD_CHURCH;
                    commands[2] = WAR_UI_COMMAND_BUILD_STABLE;

                    if (playerHasBuilding(context, player->index, WAR_UNIT_BLACKSMITH_HUMANS))
                    {
                        commands[3] = WAR_UI_COMMAND_BUILD_TOWER_HUMANS;
                    }
                }

                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_MOVE;
                commands[1] = WAR_UI_COMMAND_STOP;
                commands[2] = WAR_UI_COMMAND_REPAIR;
                commands[3] = !isCarryingResources(entity)
                    ? WAR_UI_COMMAND_GATHER : WAR_UI_COMMAND_DELIVER;

                commands[4] = WAR_UI_COMMAND_BUILD_BASIC;

                if (playerHasBuilding(context, player->index, WAR_UNIT_LUMBERMILL_HUMANS))
                {
                    commands[5] = WAR_UI_COMMAND_BUILD_ADVANCED;
                }
            }

            break;
        }
        case WAR_UNIT_PEON:
        {
            if (command->type != WAR_UI_COMMAND_NONE)
            {
                if (command->type == WAR_UI_COMMAND_BUILD_BASIC)
                {
                    commands[0] = WAR_UI_COMMAND_BUILD_FARM_ORCS;

                    if (isFeatureAllowed(player, WAR_FEATURE_UNIT_LUMBER_MILL_ORCS))
                    {
                        commands[1] = WAR_UI_COMMAND_BUILD_LUMBERMILL_ORCS;
                    }

                    commands[2] = WAR_UI_COMMAND_BUILD_BARRACKS_ORCS;
                }
                else if (command->type == WAR_UI_COMMAND_BUILD_ADVANCED)
                {
                    commands[0] = WAR_UI_COMMAND_BUILD_BARRACKS_ORCS;
                    commands[1] = WAR_UI_COMMAND_BUILD_TEMPLE;
                    commands[2] = WAR_UI_COMMAND_BUILD_KENNEL;

                    if (playerHasBuilding(context, player->index, WAR_UNIT_BLACKSMITH_ORCS))
                    {
                        commands[3] = WAR_UI_COMMAND_BUILD_TOWER_ORCS;
                    }
                }

                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_MOVE;
                commands[1] = WAR_UI_COMMAND_STOP;
                commands[2] = WAR_UI_COMMAND_REPAIR;
                commands[3] = !isCarryingResources(entity)
                    ? WAR_UI_COMMAND_GATHER : WAR_UI_COMMAND_DELIVER;

                commands[4] = WAR_UI_COMMAND_BUILD_BASIC;

                if (playerHasBuilding(context, player->index, WAR_UNIT_LUMBERMILL_ORCS))
                {
                    commands[5] = WAR_UI_COMMAND_BUILD_ADVANCED;
                }
            }

            break;
        }
        case WAR_UNIT_CONJURER:
        {
            if (command->type != WAR_UI_COMMAND_NONE)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_MOVE;
                commands[1] = WAR_UI_COMMAND_STOP;
                commands[2] = WAR_UI_COMMAND_ATTACK;

                // only if these spells are researshed
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAIN_OF_FIRE) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_RAIN_OF_FIRE))
                {
                    commands[3] = WAR_UI_COMMAND_SPELL_RAIN_OF_FIRE;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SCORPION) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_SCORPIONS))
                {
                    commands[4] = WAR_UI_COMMAND_SUMMON_SCORPION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_WATER_ELEMENTAL) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_WATER_ELEMENTAL))
                {
                    commands[5] = WAR_UI_COMMAND_SUMMON_WATER_ELEMENTAL;
                }
            }

            break;
        }
        case WAR_UNIT_WARLOCK:
        {
            if (command->type != WAR_UI_COMMAND_NONE)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_MOVE;
                commands[1] = WAR_UI_COMMAND_STOP;
                commands[2] = WAR_UI_COMMAND_ATTACK;

                // only if these spells are researshed
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_POISON_CLOUD) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_POISON_CLOUD))
                {
                    commands[3] = WAR_UI_COMMAND_SPELL_POISON_CLOUD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SPIDER) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_SPIDERS))
                {
                    commands[4] = WAR_UI_COMMAND_SUMMON_SPIDER;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DAEMON) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_DAEMON))
                {
                    commands[5] = WAR_UI_COMMAND_SUMMON_DAEMON;
                }
            }

            break;
        }
        case WAR_UNIT_CLERIC:
        {
            if (command->type != WAR_UI_COMMAND_NONE)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_MOVE;
                commands[1] = WAR_UI_COMMAND_STOP;
                commands[2] = WAR_UI_COMMAND_ATTACK;

                // only if these spells are researshed
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_HEALING) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_HEALING))
                {
                    commands[3] = WAR_UI_COMMAND_SPELL_HEALING;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_FAR_SIGHT) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_FAR_SIGHT))
                {
                    commands[4] = WAR_UI_COMMAND_SPELL_FAR_SIGHT;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_INVISIBILITY) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_INVISIBILITY))
                {
                    commands[5] = WAR_UI_COMMAND_SPELL_INVISIBILITY;
                }
            }

            break;
        }
        case WAR_UNIT_NECROLYTE:
        {
            if (command->type != WAR_UI_COMMAND_NONE)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_MOVE;
                commands[1] = WAR_UI_COMMAND_STOP;
                commands[2] = WAR_UI_COMMAND_ATTACK;

                // only if these spells are researshed
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAISE_DEAD) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_RAISE_DEAD))
                {
                    commands[3] = WAR_UI_COMMAND_SPELL_RAISE_DEAD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DARK_VISION) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_DARK_VISION))
                {
                    commands[4] = WAR_UI_COMMAND_SPELL_DARK_VISION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_UNHOLY_ARMOR) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_UNHOLY_ARMOR))
                {
                    commands[5] = WAR_UI_COMMAND_SPELL_UNHOLY_ARMOR;
                }
            }

            break;
        }

        // buildings
        case WAR_UNIT_FARM_HUMANS:
        case WAR_UNIT_FARM_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }

            break;
        }

        case WAR_UNIT_BARRACKS_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_TRAIN_FOOTMAN;

                // only if there is a lumber mill
                if (playerHasBuilding(context, player->index, WAR_UNIT_LUMBERMILL_HUMANS))
                {
                    commands[1] = WAR_UI_COMMAND_TRAIN_ARCHER;
                }

                // only if there is a blacksmith
                if (playerHasBuilding(context, player->index, WAR_UNIT_BLACKSMITH_HUMANS))
                {
                    commands[2] = WAR_UI_COMMAND_TRAIN_CATAPULT_HUMANS;

                    // only if there is a stable
                    if (playerHasBuilding(context, player->index, WAR_UNIT_STABLE))
                    {
                        commands[3] = WAR_UI_COMMAND_TRAIN_KNIGHT;
                    }
                }
            }

            break;
        }
        case WAR_UNIT_BARRACKS_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_TRAIN_GRUNT;

                // only if there is a lumber mill
                if (playerHasBuilding(context, player->index, WAR_UNIT_LUMBERMILL_ORCS))
                {
                    commands[1] = WAR_UI_COMMAND_TRAIN_SPEARMAN;
                }

                // only if there is a blacksmith
                if (playerHasBuilding(context, player->index, WAR_UNIT_BLACKSMITH_ORCS))
                {
                    commands[2] = WAR_UI_COMMAND_TRAIN_CATAPULT_ORCS;

                    // only if there is a kennel
                    if (playerHasBuilding(context, player->index, WAR_UNIT_KENNEL))
                    {
                        commands[3] = WAR_UI_COMMAND_TRAIN_RAIDER;
                    }
                }
            }

            break;
        }
        case WAR_UNIT_CHURCH:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_TRAIN_CLERIC;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_HEALING) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_HEALING))
                {
                    commands[1] = WAR_UI_COMMAND_UPGRADE_HEALING;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_FAR_SIGHT) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_FAR_SIGHT))
                {
                    commands[2] = WAR_UI_COMMAND_UPGRADE_FAR_SIGHT;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_INVISIBILITY) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_INVISIBILITY))
                {
                    commands[3] = WAR_UI_COMMAND_UPGRADE_INVISIBILITY;
                }
            }

            break;
        }
        case WAR_UNIT_TEMPLE:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_TRAIN_NECROLYTE;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAISE_DEAD) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_RAISE_DEAD))
                {
                    commands[1] = WAR_UI_COMMAND_UPGRADE_RAISE_DEAD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DARK_VISION) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_DARK_VISION))
                {
                    commands[2] = WAR_UI_COMMAND_UPGRADE_DARK_VISION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_UNHOLY_ARMOR) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_UNHOLY_ARMOR))
                {
                    commands[3] = WAR_UI_COMMAND_UPGRADE_UNHOLY_ARMOR;
                }
            }

            break;
        }
        case WAR_UNIT_TOWER_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_TRAIN_CONJURER;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SCORPION) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_SCORPIONS))
                {
                    commands[1] = WAR_UI_COMMAND_UPGRADE_SCORPION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAIN_OF_FIRE) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_RAIN_OF_FIRE))
                {
                    commands[2] = WAR_UI_COMMAND_UPGRADE_RAIN_OF_FIRE;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_WATER_ELEMENTAL) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_WATER_ELEMENTAL))
                {
                    commands[3] = WAR_UI_COMMAND_UPGRADE_WATER_ELEMENTAL;
                }
            }

            break;
        }
        case WAR_UNIT_TOWER_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_UI_COMMAND_TRAIN_WARLOCK;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SPIDER) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_SPIDERS))
                {
                    commands[1] = WAR_UI_COMMAND_UPGRADE_SPIDER;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_POISON_CLOUD) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_POISON_CLOUD))
                {
                    commands[2] = WAR_UI_COMMAND_UPGRADE_POISON_CLOUD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DAEMON) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_DAEMON))
                {
                    commands[3] = WAR_UI_COMMAND_UPGRADE_DAEMON;
                }
            }

            break;
        }
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_TOWNHALL_ORCS:
        {
            if (command->type == WAR_UI_COMMAND_BUILD_WALL ||
                command->type == WAR_UI_COMMAND_BUILD_ROAD ||
                unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = unit->type == WAR_UNIT_TOWNHALL_HUMANS
                    ? WAR_UI_COMMAND_TRAIN_PEASANT : WAR_UI_COMMAND_TRAIN_PEON;

                commands[1] = WAR_UI_COMMAND_BUILD_ROAD;

                // only if this is allowed
                if (isFeatureAllowed(player, WAR_FEATURE_UNIT_WALL))
                {
                    commands[2] = WAR_UI_COMMAND_BUILD_WALL;
                }
            }

            break;
        }
        case WAR_UNIT_LUMBERMILL_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_ARROWS))
                {
                    commands[0] = WAR_UI_COMMAND_UPGRADE_ARROWS;
                }
            }

            break;
        }
        case WAR_UNIT_LUMBERMILL_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_SPEARS))
                {
                    commands[0] = WAR_UI_COMMAND_UPGRADE_SPEARS;
                }
            }

            break;
        }
        case WAR_UNIT_STABLE:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_HORSES))
                {
                    commands[0] = WAR_UI_COMMAND_UPGRADE_HORSES;
                }
            }

            break;
        }
        case WAR_UNIT_KENNEL:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_WOLVES))
                {
                    commands[0] = WAR_UI_COMMAND_UPGRADE_WOLVES;
                }
            }

            break;
        }
        case WAR_UNIT_BLACKSMITH_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_SWORDS))
                {
                    commands[0] = WAR_UI_COMMAND_UPGRADE_SWORDS;
                }

                if (hasRemainingUpgrade(player, WAR_UPGRADE_SHIELD))
                {
                    commands[1] = WAR_UI_COMMAND_UPGRADE_SHIELD_HUMANS;
                }
            }

            break;
        }
        case WAR_UNIT_BLACKSMITH_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_UI_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_AXES))
                {
                    commands[0] = WAR_UI_COMMAND_UPGRADE_AXES;
                }

                if (hasRemainingUpgrade(player, WAR_UPGRADE_SHIELD))
                {
                    commands[1] = WAR_UI_COMMAND_UPGRADE_SHIELD_ORCS;
                }
            }

            break;
        }

        default:
        {
            logWarning("Commands for unit type %d not handled yet.\n", unit->type);
            break;
        }
    }
}

WarUICommandData getUnitCommandData(WarContext* context, WarEntity* entity, WarUICommandType commandType)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUICommandBaseData commandBaseData = getCommandBaseData(commandType);

    WarUICommandData data = (WarUICommandData){0};
    data.type = commandType;
    data.hotKey = commandBaseData.hotKey;
    data.highlightIndex = commandBaseData.highlightIndex;
    data.highlightCount = commandBaseData.highlightCount;
    strcpy(data.tooltip, commandBaseData.tooltip);
    data.clickHandler = commandBaseData.clickHandler;

    switch (commandType)
    {
        // unit commands
        case WAR_UI_COMMAND_MOVE:
        {
            data.frameIndex = isHumanPlayer(player)
                ? WAR_PORTRAIT_MOVE_HUMANS : WAR_PORTRAIT_MOVE_ORCS;
            break;
        }
        case WAR_UI_COMMAND_STOP:
        {
            // check here upgrades for the shields
            if (getUpgradeLevel(player, WAR_UPGRADE_SHIELD) == 2)
            {
                data.frameIndex = isHumanPlayer(player)
                    ? WAR_PORTRAIT_SHIELD_3_HUMANS : WAR_PORTRAIT_SHIELD_3_ORCS;
            }
            else if (getUpgradeLevel(player, WAR_UPGRADE_SHIELD) == 1)
            {
                data.frameIndex = isHumanPlayer(player)
                    ? WAR_PORTRAIT_SHIELD_2_HUMANS : WAR_PORTRAIT_SHIELD_2_ORCS;
            }
            else
            {
                data.frameIndex = isHumanPlayer(player)
                    ? WAR_PORTRAIT_SHIELD_1_HUMANS : WAR_PORTRAIT_SHIELD_1_ORCS;
            }

            break;
        }
        case WAR_UI_COMMAND_GATHER:
        {
            data.frameIndex = WAR_PORTRAIT_HARVEST;
            break;
        }
        case WAR_UI_COMMAND_DELIVER:
        {
            data.frameIndex = WAR_PORTRAIT_DELIVER;
            break;
        }
        case WAR_UI_COMMAND_REPAIR:
        {
            data.frameIndex = WAR_PORTRAIT_REPAIR;
            break;
        }
        case WAR_UI_COMMAND_BUILD_BASIC:
        {
            data.frameIndex = WAR_PORTRAIT_BUILD_BASIC;
            break;
        }
        case WAR_UI_COMMAND_BUILD_ADVANCED:
        {
            data.frameIndex = WAR_PORTRAIT_BUILD_ADVANCED;
            break;
        }
        case WAR_UI_COMMAND_ATTACK:
        {
            if (entity && isUnit(entity))
            {
                WarUnitComponent* unit = &entity->unit;
                switch (unit->type)
                {
                    // units
                    case WAR_UNIT_FOOTMAN:
                    case WAR_UNIT_GRUNT:
                    case WAR_UNIT_KNIGHT:
                    case WAR_UNIT_RAIDER:
                    case WAR_UNIT_CATAPULT_HUMANS:
                    case WAR_UNIT_CATAPULT_ORCS:
                    case WAR_UNIT_SCORPION:
                    case WAR_UNIT_SPIDER:
                    case WAR_UNIT_DAEMON:
                    {
                        // check here upgrades for the sword
                        WarUpgradeType swordAxeUpgrade = isHumanPlayer(player)
                            ? WAR_UPGRADE_SWORDS : WAR_UPGRADE_AXES;

                        if (getUpgradeLevel(player, swordAxeUpgrade) == 2)
                        {
                            data.frameIndex = isHumanPlayer(player)
                                ? WAR_PORTRAIT_SWORD_3 : WAR_PORTRAIT_AXE_3;
                        }
                        else if (getUpgradeLevel(player, swordAxeUpgrade) == 1)
                        {
                            data.frameIndex = isHumanPlayer(player)
                                ? WAR_PORTRAIT_SWORD_2 : WAR_PORTRAIT_AXE_2;
                        }
                        else
                        {
                            data.frameIndex = isHumanPlayer(player)
                                ? WAR_PORTRAIT_SWORD_1 : WAR_PORTRAIT_AXE_1;
                        }

                        break;
                    }

                    case WAR_UNIT_ARCHER:
                    case WAR_UNIT_SPEARMAN:
                    {
                        // check here upgrades for the arrows
                        WarUpgradeType arrowSpearUpgrade = isHumanPlayer(player)
                            ? WAR_UPGRADE_ARROWS : WAR_UPGRADE_SPEARS;

                        if (getUpgradeLevel(player, arrowSpearUpgrade) == 2)
                        {
                            data.frameIndex = isHumanPlayer(player)
                                ? WAR_PORTRAIT_ARROW_3 : WAR_PORTRAIT_SPEAR_3;
                        }
                        else if (getUpgradeLevel(player, arrowSpearUpgrade) == 1)
                        {
                            data.frameIndex = isHumanPlayer(player)
                                ? WAR_PORTRAIT_ARROW_2 : WAR_PORTRAIT_SPEAR_2;
                        }
                        else
                        {
                            data.frameIndex = isHumanPlayer(player)
                                ? WAR_PORTRAIT_ARROW_1 : WAR_PORTRAIT_SPEAR_1;
                        }

                        break;
                    }

                    case WAR_UNIT_CONJURER:
                    case WAR_UNIT_WATER_ELEMENTAL:
                    {
                        data.frameIndex = WAR_PORTRAIT_ELEMENTAL_BLAST;
                        break;
                    }

                    case WAR_UNIT_CLERIC:
                    {
                        data.frameIndex = WAR_PORTRAIT_HOLY_LANCE;
                        break;
                    }

                    case WAR_UNIT_NECROLYTE:
                    {
                        data.frameIndex = WAR_PORTRAIT_SHADOW_SPEAR;
                        break;
                    }

                    case WAR_UNIT_WARLOCK:
                    {
                        data.frameIndex = WAR_PORTRAIT_FIREBALL;
                        break;
                    }

                    default:
                    {
                        logWarning("Trying to get an attack command for unit of type: %d\n", unit->type);
                        break;
                    }
                }
            }
            else
            {
                // check here upgrades for the sword
                if (isHumanPlayer(player))
                {
                    if (getUpgradeLevel(player, WAR_UPGRADE_SWORDS) == 2)
                        data.frameIndex = WAR_PORTRAIT_SWORD_3;
                    else if (getUpgradeLevel(player, WAR_UPGRADE_SWORDS) == 1)
                        data.frameIndex = WAR_PORTRAIT_SWORD_2;
                    else
                        data.frameIndex = WAR_PORTRAIT_SWORD_1;
                }
                else if (isOrcPlayer(player))
                {
                    if (getUpgradeLevel(player, WAR_UPGRADE_AXES) == 2)
                        data.frameIndex = WAR_PORTRAIT_AXE_3;
                    else if (getUpgradeLevel(player, WAR_UPGRADE_AXES) == 1)
                        data.frameIndex = WAR_PORTRAIT_AXE_2;
                    else
                        data.frameIndex = WAR_PORTRAIT_AXE_1;
                }
                else
                {
                    logWarning("Trying to get a frame index for player race: %d\n", player->race);
                    assert(false);
                }
            }

            break;
        }

        case WAR_UI_COMMAND_TRAIN_PEASANT:
        case WAR_UI_COMMAND_TRAIN_PEON:
        case WAR_UI_COMMAND_TRAIN_FOOTMAN:
        case WAR_UI_COMMAND_TRAIN_GRUNT:
        case WAR_UI_COMMAND_TRAIN_CATAPULT_HUMANS:
        case WAR_UI_COMMAND_TRAIN_CATAPULT_ORCS:
        case WAR_UI_COMMAND_TRAIN_KNIGHT:
        case WAR_UI_COMMAND_TRAIN_RAIDER:
        case WAR_UI_COMMAND_TRAIN_ARCHER:
        case WAR_UI_COMMAND_TRAIN_SPEARMAN:
        case WAR_UI_COMMAND_TRAIN_CONJURER:
        case WAR_UI_COMMAND_TRAIN_WARLOCK:
        case WAR_UI_COMMAND_TRAIN_CLERIC:
        case WAR_UI_COMMAND_TRAIN_NECROLYTE:
        {
            WarUICommandMapping commandMapping = getCommandMapping(commandType);
            WarUnitData unitData = getUnitData(commandMapping.mappedType);
            WarUnitStats stats = getUnitStats(commandMapping.mappedType);

            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = unitData.portraitFrameIndex;
            break;
        }

        case WAR_UI_COMMAND_BUILD_FARM_HUMANS:
        case WAR_UI_COMMAND_BUILD_FARM_ORCS:
        case WAR_UI_COMMAND_BUILD_BARRACKS_HUMANS:
        case WAR_UI_COMMAND_BUILD_BARRACKS_ORCS:
        case WAR_UI_COMMAND_BUILD_CHURCH:
        case WAR_UI_COMMAND_BUILD_TEMPLE:
        case WAR_UI_COMMAND_BUILD_TOWER_HUMANS:
        case WAR_UI_COMMAND_BUILD_TOWER_ORCS:
        case WAR_UI_COMMAND_BUILD_TOWNHALL_HUMANS:
        case WAR_UI_COMMAND_BUILD_TOWNHALL_ORCS:
        case WAR_UI_COMMAND_BUILD_LUMBERMILL_HUMANS:
        case WAR_UI_COMMAND_BUILD_LUMBERMILL_ORCS:
        case WAR_UI_COMMAND_BUILD_STABLE:
        case WAR_UI_COMMAND_BUILD_KENNEL:
        case WAR_UI_COMMAND_BUILD_BLACKSMITH_HUMANS:
        case WAR_UI_COMMAND_BUILD_BLACKSMITH_ORCS:
        {
            WarUICommandMapping commandMapping = getCommandMapping(commandType);
            WarUnitData unitData = getUnitData(commandMapping.mappedType);
            WarBuildingStats stats = getBuildingStats(commandMapping.mappedType);

            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = unitData.portraitFrameIndex;
            break;
        }

        case WAR_UI_COMMAND_BUILD_ROAD:
        {
            data.gold = 50;
            data.frameIndex = WAR_PORTRAIT_ROAD;
            break;
        }

        case WAR_UI_COMMAND_BUILD_WALL:
        {
            data.gold = WAR_WALL_GOLD_COST;
            data.frameIndex = WAR_PORTRAIT_WALL;
            break;
        }

        case WAR_UI_COMMAND_UPGRADE_SWORDS:
        case WAR_UI_COMMAND_UPGRADE_AXES:
        case WAR_UI_COMMAND_UPGRADE_SHIELD_HUMANS:
        case WAR_UI_COMMAND_UPGRADE_SHIELD_ORCS:
        case WAR_UI_COMMAND_UPGRADE_ARROWS:
        case WAR_UI_COMMAND_UPGRADE_SPEARS:
        case WAR_UI_COMMAND_UPGRADE_HORSES:
        case WAR_UI_COMMAND_UPGRADE_WOLVES:
        case WAR_UI_COMMAND_UPGRADE_SCORPION:
        case WAR_UI_COMMAND_UPGRADE_SPIDER:
        case WAR_UI_COMMAND_UPGRADE_RAIN_OF_FIRE:
        case WAR_UI_COMMAND_UPGRADE_POISON_CLOUD:
        case WAR_UI_COMMAND_UPGRADE_WATER_ELEMENTAL:
        case WAR_UI_COMMAND_UPGRADE_DAEMON:
        case WAR_UI_COMMAND_UPGRADE_HEALING:
        case WAR_UI_COMMAND_UPGRADE_RAISE_DEAD:
        case WAR_UI_COMMAND_UPGRADE_FAR_SIGHT:
        case WAR_UI_COMMAND_UPGRADE_DARK_VISION:
        case WAR_UI_COMMAND_UPGRADE_INVISIBILITY:
        case WAR_UI_COMMAND_UPGRADE_UNHOLY_ARMOR:
        {
            WarUICommandMapping commandMapping = getCommandMapping(commandType);

            assert(hasRemainingUpgrade(player, commandMapping.mappedType));

            WarUpgradeData upgradeData = getUpgradeData(commandMapping.mappedType);
            WarUpgradeStats stats = getUpgradeStats(commandMapping.mappedType);
            s32 upgradeLevel = getUpgradeLevel(player, commandMapping.mappedType);

            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = upgradeData.frameIndices[upgradeLevel];
            break;
        }

        case WAR_UI_COMMAND_SUMMON_SPIDER:
        case WAR_UI_COMMAND_SUMMON_SCORPION:
        case WAR_UI_COMMAND_SUMMON_DAEMON:
        case WAR_UI_COMMAND_SUMMON_WATER_ELEMENTAL:
        case WAR_UI_COMMAND_SPELL_HEALING:
        case WAR_UI_COMMAND_SPELL_FAR_SIGHT:
        case WAR_UI_COMMAND_SPELL_INVISIBILITY:
        case WAR_UI_COMMAND_SPELL_RAIN_OF_FIRE:
        case WAR_UI_COMMAND_SPELL_RAISE_DEAD:
        case WAR_UI_COMMAND_SPELL_DARK_VISION:
        case WAR_UI_COMMAND_SPELL_UNHOLY_ARMOR:
        case WAR_UI_COMMAND_SPELL_POISON_CLOUD:
        {
            WarUICommandMapping commandMapping = getCommandMapping(commandType);
            WarSpellData spellData = getSpellData(commandMapping.mappedType);

            data.frameIndex = spellData.portraitFrameIndex;
            break;
        }

        // cancel
        case WAR_UI_COMMAND_CANCEL:
        {
            data.frameIndex = WAR_PORTRAIT_CANCEL;
            break;
        }

        default:
        {
            logWarning("Unkown command type: %d\n", commandType);
            break;
        }
    }

    return data;
}