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
            default:
            {
                logInfo("Trying to get type %d for race %d, returning %d\n", type, race, type);
                return type;
            }
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
            default:
            {
                logInfo("Trying to get type %d for race %d, returning %d\n", type, race, type);
                return type;
            }
        }
    }

    logInfo("Trying to get type %d for race %d, returning %d\n", type, race, type);
    return type;
}

void getUnitCommands(WarContext* context, WarEntity* entity, WarUnitCommandType commands[])
{
    assert(entity);
    assert(isUnit(entity));

    WarMap* map = context->map;
    WarUnitCommand* command = &map->command;
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
            if (command->type != WAR_COMMAND_NONE)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_MOVE;
                commands[1] = WAR_COMMAND_STOP;
                commands[2] = WAR_COMMAND_ATTACK;
            }

            break;
        }

        case WAR_UNIT_PEASANT:
        {
            if (command->type != WAR_COMMAND_NONE)
            {
                if (command->type == WAR_COMMAND_BUILD_BASIC)
                {
                    commands[0] = WAR_COMMAND_BUILD_FARM_HUMANS;

                    if (isFeatureAllowed(player, WAR_FEATURE_UNIT_LUMBER_MILL_HUMANS))
                    {
                        commands[1] = WAR_COMMAND_BUILD_LUMBERMILL_HUMANS;
                    }

                    commands[2] = WAR_COMMAND_BUILD_BARRACKS_HUMANS;
                }
                else if (command->type == WAR_COMMAND_BUILD_ADVANCED)
                {
                    commands[0] = WAR_COMMAND_BUILD_BLACKSMITH_HUMANS;
                    commands[1] = WAR_COMMAND_BUILD_CHURCH;
                    commands[2] = WAR_COMMAND_BUILD_STABLE;

                    if (playerHasBuilding(context, player->index, WAR_UNIT_BLACKSMITH_HUMANS))
                    {
                        commands[3] = WAR_COMMAND_BUILD_TOWER_HUMANS;
                    }
                }

                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_MOVE;
                commands[1] = WAR_COMMAND_STOP;
                commands[2] = WAR_COMMAND_REPAIR;
                commands[3] = !isCarryingResources(entity)
                    ? WAR_COMMAND_HARVEST : WAR_COMMAND_DELIVER;

                commands[4] = WAR_COMMAND_BUILD_BASIC;

                if (playerHasBuilding(context, player->index, WAR_UNIT_LUMBERMILL_HUMANS))
                {
                    commands[5] = WAR_COMMAND_BUILD_ADVANCED;
                }
            }

            break;
        }
        case WAR_UNIT_PEON:
        {
            if (command->type != WAR_COMMAND_NONE)
            {
                if (command->type == WAR_COMMAND_BUILD_BASIC)
                {
                    commands[0] = WAR_COMMAND_BUILD_FARM_ORCS;

                    if (isFeatureAllowed(player, WAR_FEATURE_UNIT_LUMBER_MILL_ORCS))
                    {
                        commands[1] = WAR_COMMAND_BUILD_LUMBERMILL_ORCS;
                    }

                    commands[2] = WAR_COMMAND_BUILD_BARRACKS_ORCS;
                }
                else if (command->type == WAR_COMMAND_BUILD_ADVANCED)
                {
                    commands[0] = WAR_COMMAND_BUILD_BARRACKS_ORCS;
                    commands[1] = WAR_COMMAND_BUILD_TEMPLE;
                    commands[2] = WAR_COMMAND_BUILD_KENNEL;

                    if (playerHasBuilding(context, player->index, WAR_UNIT_BLACKSMITH_ORCS))
                    {
                        commands[3] = WAR_COMMAND_BUILD_TOWER_ORCS;
                    }
                }

                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_MOVE;
                commands[1] = WAR_COMMAND_STOP;
                commands[2] = WAR_COMMAND_REPAIR;
                commands[3] = !isCarryingResources(entity)
                    ? WAR_COMMAND_HARVEST : WAR_COMMAND_DELIVER;

                commands[4] = WAR_COMMAND_BUILD_BASIC;

                if (playerHasBuilding(context, player->index, WAR_UNIT_LUMBERMILL_ORCS))
                {
                    commands[5] = WAR_COMMAND_BUILD_ADVANCED;
                }
            }

            break;
        }
        case WAR_UNIT_CONJURER:
        {
            if (command->type != WAR_COMMAND_NONE)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_MOVE;
                commands[1] = WAR_COMMAND_STOP;
                commands[2] = WAR_COMMAND_ATTACK;

                // only if these spells are researshed
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAIN_OF_FIRE) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_RAIN_OF_FIRE))
                {
                    commands[3] = WAR_COMMAND_SPELL_RAIN_OF_FIRE;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SCORPION) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_SCORPIONS))
                {
                    commands[4] = WAR_COMMAND_SUMMON_SCORPION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_WATER_ELEMENTAL) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_WATER_ELEMENTAL))
                {
                    commands[5] = WAR_COMMAND_SUMMON_WATER_ELEMENTAL;
                }
            }

            break;
        }
        case WAR_UNIT_WARLOCK:
        {
            if (command->type != WAR_COMMAND_NONE)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_MOVE;
                commands[1] = WAR_COMMAND_STOP;
                commands[2] = WAR_COMMAND_ATTACK;

                // only if these spells are researshed
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_POISON_CLOUD) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_POISON_CLOUD))
                {
                    commands[3] = WAR_COMMAND_SPELL_POISON_CLOUD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SPIDER) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_SPIDERS))
                {
                    commands[4] = WAR_COMMAND_SUMMON_SPIDER;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DAEMON) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_DAEMON))
                {
                    commands[5] = WAR_COMMAND_SUMMON_DAEMON;
                }
            }

            break;
        }
        case WAR_UNIT_CLERIC:
        {
            if (command->type != WAR_COMMAND_NONE)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_MOVE;
                commands[1] = WAR_COMMAND_STOP;
                commands[2] = WAR_COMMAND_ATTACK;

                // only if these spells are researshed
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_HEALING) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_HEALING))
                {
                    commands[3] = WAR_COMMAND_SPELL_HEALING;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_FAR_SIGHT) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_FAR_SIGHT))
                {
                    commands[4] = WAR_COMMAND_SPELL_FAR_SIGHT;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_INVISIBILITY) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_INVISIBILITY))
                {
                    commands[5] = WAR_COMMAND_SPELL_INVISIBILITY;
                }
            }

            break;
        }
        case WAR_UNIT_NECROLYTE:
        {
            if (command->type != WAR_COMMAND_NONE)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_MOVE;
                commands[1] = WAR_COMMAND_STOP;
                commands[2] = WAR_COMMAND_ATTACK;

                // only if these spells are researshed
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAISE_DEAD) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_RAISE_DEAD))
                {
                    commands[3] = WAR_COMMAND_SPELL_RAISE_DEAD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DARK_VISION) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_DARK_VISION))
                {
                    commands[4] = WAR_COMMAND_SPELL_DARK_VISION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_UNHOLY_ARMOR) &&
                    hasAnyUpgrade(player, WAR_UPGRADE_UNHOLY_ARMOR))
                {
                    commands[5] = WAR_COMMAND_SPELL_UNHOLY_ARMOR;
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
                commands[5] = WAR_COMMAND_CANCEL;
            }

            break;
        }

        case WAR_UNIT_BARRACKS_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_FOOTMAN;

                // only if there is a lumber mill
                if (playerHasBuilding(context, player->index, WAR_UNIT_LUMBERMILL_HUMANS))
                {
                    commands[1] = WAR_COMMAND_TRAIN_ARCHER;
                }

                // only if there is a blacksmith
                if (playerHasBuilding(context, player->index, WAR_UNIT_BLACKSMITH_HUMANS))
                {
                    commands[2] = WAR_COMMAND_TRAIN_CATAPULT_HUMANS;

                    // only if there is a stable
                    if (playerHasBuilding(context, player->index, WAR_UNIT_STABLE))
                    {
                        commands[3] = WAR_COMMAND_TRAIN_KNIGHT;
                    }
                }
            }

            break;
        }
        case WAR_UNIT_BARRACKS_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_GRUNT;

                // only if there is a lumber mill
                if (playerHasBuilding(context, player->index, WAR_UNIT_LUMBERMILL_ORCS))
                {
                    commands[1] = WAR_COMMAND_TRAIN_SPEARMAN;
                }

                // only if there is a blacksmith
                if (playerHasBuilding(context, player->index, WAR_UNIT_BLACKSMITH_ORCS))
                {
                    commands[2] = WAR_COMMAND_TRAIN_CATAPULT_ORCS;

                    // only if there is a kennel
                    if (playerHasBuilding(context, player->index, WAR_UNIT_KENNEL))
                    {
                        commands[3] = WAR_COMMAND_TRAIN_RAIDER;
                    }
                }
            }

            break;
        }
        case WAR_UNIT_CHURCH:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_CLERIC;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_HEALING) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_HEALING))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_HEALING;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_FAR_SIGHT) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_FAR_SIGHT))
                {
                    commands[2] = WAR_COMMAND_UPGRADE_FAR_SIGHT;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_INVISIBILITY) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_INVISIBILITY))
                {
                    commands[3] = WAR_COMMAND_UPGRADE_INVISIBILITY;
                }
            }

            break;
        }
        case WAR_UNIT_TEMPLE:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_NECROLYTE;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAISE_DEAD) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_RAISE_DEAD))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_RAISE_DEAD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DARK_VISION) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_DARK_VISION))
                {
                    commands[2] = WAR_COMMAND_UPGRADE_DARK_VISION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_UNHOLY_ARMOR) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_UNHOLY_ARMOR))
                {
                    commands[3] = WAR_COMMAND_UPGRADE_UNHOLY_ARMOR;
                }
            }

            break;
        }
        case WAR_UNIT_TOWER_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_CONJURER;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SCORPION) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_SCORPIONS))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_SCORPION;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_RAIN_OF_FIRE) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_RAIN_OF_FIRE))
                {
                    commands[2] = WAR_COMMAND_UPGRADE_RAIN_OF_FIRE;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_WATER_ELEMENTAL) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_WATER_ELEMENTAL))
                {
                    commands[3] = WAR_COMMAND_UPGRADE_WATER_ELEMENTAL;
                }
            }

            break;
        }
        case WAR_UNIT_TOWER_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = WAR_COMMAND_TRAIN_WARLOCK;

                // only if this upgrade is not been researched yet
                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_SPIDER) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_SPIDERS))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_SPIDER;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_POISON_CLOUD) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_POISON_CLOUD))
                {
                    commands[2] = WAR_COMMAND_UPGRADE_POISON_CLOUD;
                }

                if (isFeatureAllowed(player, WAR_FEATURE_SPELL_DAEMON) &&
                    hasRemainingUpgrade(player, WAR_UPGRADE_DAEMON))
                {
                    commands[3] = WAR_COMMAND_UPGRADE_DAEMON;
                }
            }

            break;
        }
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_TOWNHALL_ORCS:
        {
            if (command->type == WAR_COMMAND_BUILD_WALL ||
                command->type == WAR_COMMAND_BUILD_ROAD ||
                unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                commands[0] = unit->type == WAR_UNIT_TOWNHALL_HUMANS
                    ? WAR_COMMAND_TRAIN_PEASANT : WAR_COMMAND_TRAIN_PEON;

                commands[1] = WAR_COMMAND_BUILD_ROAD;

                // only if this is allowed
                if (isFeatureAllowed(player, WAR_FEATURE_UNIT_WALL))
                {
                    commands[2] = WAR_COMMAND_BUILD_WALL;
                }
            }

            break;
        }
        case WAR_UNIT_LUMBERMILL_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_ARROWS))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_ARROWS;
                }
            }

            break;
        }
        case WAR_UNIT_LUMBERMILL_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_SPEARS))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_SPEARS;
                }
            }

            break;
        }
        case WAR_UNIT_STABLE:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_HORSES))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_HORSES;
                }
            }

            break;
        }
        case WAR_UNIT_KENNEL:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_WOLVES))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_WOLVES;
                }
            }

            break;
        }
        case WAR_UNIT_BLACKSMITH_HUMANS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_SWORDS))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_SWORDS;
                }

                if (hasRemainingUpgrade(player, WAR_UPGRADE_SHIELD))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_SHIELD_HUMANS;
                }
            }

            break;
        }
        case WAR_UNIT_BLACKSMITH_ORCS:
        {
            if (unit->building)
            {
                commands[5] = WAR_COMMAND_CANCEL;
            }
            else
            {
                // only if this upgrade is less than level 2
                if (hasRemainingUpgrade(player, WAR_UPGRADE_AXES))
                {
                    commands[0] = WAR_COMMAND_UPGRADE_AXES;
                }

                if (hasRemainingUpgrade(player, WAR_UPGRADE_SHIELD))
                {
                    commands[1] = WAR_COMMAND_UPGRADE_SHIELD_ORCS;
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

WarUnitCommandData getUnitCommandData(WarContext* context, WarEntity* entity, WarUnitCommandType commandType)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitCommandBaseData commandBaseData = getCommandBaseData(commandType);

    WarUnitCommandData data = (WarUnitCommandData){0};
    data.type = commandType;
    data.hotKey = commandBaseData.hotKey;
    data.highlightIndex = commandBaseData.highlightIndex;
    data.highlightCount = commandBaseData.highlightCount;
    strcpy(data.tooltip, commandBaseData.tooltip);
    data.clickHandler = commandBaseData.clickHandler;

    switch (commandType)
    {
        // unit commands
        case WAR_COMMAND_MOVE:
        {
            data.frameIndex = isHumanPlayer(player)
                ? WAR_PORTRAIT_MOVE_HUMANS : WAR_PORTRAIT_MOVE_ORCS;
            break;
        }
        case WAR_COMMAND_STOP:
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
        case WAR_COMMAND_HARVEST:
        {
            data.frameIndex = WAR_PORTRAIT_HARVEST;
            break;
        }
        case WAR_COMMAND_DELIVER:
        {
            data.frameIndex = WAR_PORTRAIT_DELIVER;
            break;
        }
        case WAR_COMMAND_REPAIR:
        {
            data.frameIndex = WAR_PORTRAIT_REPAIR;
            break;
        }
        case WAR_COMMAND_BUILD_BASIC:
        {
            data.frameIndex = WAR_PORTRAIT_BUILD_BASIC;
            break;
        }
        case WAR_COMMAND_BUILD_ADVANCED:
        {
            data.frameIndex = WAR_PORTRAIT_BUILD_ADVANCED;
            break;
        }
        case WAR_COMMAND_ATTACK:
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

        case WAR_COMMAND_TRAIN_PEASANT:
        case WAR_COMMAND_TRAIN_PEON:
        case WAR_COMMAND_TRAIN_FOOTMAN:
        case WAR_COMMAND_TRAIN_GRUNT:
        case WAR_COMMAND_TRAIN_CATAPULT_HUMANS:
        case WAR_COMMAND_TRAIN_CATAPULT_ORCS:
        case WAR_COMMAND_TRAIN_KNIGHT:
        case WAR_COMMAND_TRAIN_RAIDER:
        case WAR_COMMAND_TRAIN_ARCHER:
        case WAR_COMMAND_TRAIN_SPEARMAN:
        case WAR_COMMAND_TRAIN_CONJURER:
        case WAR_COMMAND_TRAIN_WARLOCK:
        case WAR_COMMAND_TRAIN_CLERIC:
        case WAR_COMMAND_TRAIN_NECROLYTE:
        {
            WarUnitCommandMapping commandMapping = getCommandMapping(commandType);
            WarUnitData unitData = getUnitData(commandMapping.mappedType);
            WarUnitStats stats = getUnitStats(commandMapping.mappedType);

            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = unitData.portraitFrameIndex;
            break;
        }

        case WAR_COMMAND_BUILD_FARM_HUMANS:
        case WAR_COMMAND_BUILD_FARM_ORCS:
        case WAR_COMMAND_BUILD_BARRACKS_HUMANS:
        case WAR_COMMAND_BUILD_BARRACKS_ORCS:
        case WAR_COMMAND_BUILD_CHURCH:
        case WAR_COMMAND_BUILD_TEMPLE:
        case WAR_COMMAND_BUILD_TOWER_HUMANS:
        case WAR_COMMAND_BUILD_TOWER_ORCS:
        case WAR_COMMAND_BUILD_TOWNHALL_HUMANS:
        case WAR_COMMAND_BUILD_TOWNHALL_ORCS:
        case WAR_COMMAND_BUILD_LUMBERMILL_HUMANS:
        case WAR_COMMAND_BUILD_LUMBERMILL_ORCS:
        case WAR_COMMAND_BUILD_STABLE:
        case WAR_COMMAND_BUILD_KENNEL:
        case WAR_COMMAND_BUILD_BLACKSMITH_HUMANS:
        case WAR_COMMAND_BUILD_BLACKSMITH_ORCS:
        {
            WarUnitCommandMapping commandMapping = getCommandMapping(commandType);
            WarUnitData unitData = getUnitData(commandMapping.mappedType);
            WarBuildingStats stats = getBuildingStats(commandMapping.mappedType);

            data.gold = stats.goldCost;
            data.wood = stats.woodCost;
            data.frameIndex = unitData.portraitFrameIndex;
            break;
        }

        case WAR_COMMAND_BUILD_ROAD:
        {
            data.gold = 50;
            data.frameIndex = WAR_PORTRAIT_ROAD;
            break;
        }

        case WAR_COMMAND_BUILD_WALL:
        {
            data.gold = WAR_WALL_GOLD_COST;
            data.frameIndex = WAR_PORTRAIT_WALL;
            break;
        }

        case WAR_COMMAND_UPGRADE_SWORDS:
        case WAR_COMMAND_UPGRADE_AXES:
        case WAR_COMMAND_UPGRADE_SHIELD_HUMANS:
        case WAR_COMMAND_UPGRADE_SHIELD_ORCS:
        case WAR_COMMAND_UPGRADE_ARROWS:
        case WAR_COMMAND_UPGRADE_SPEARS:
        case WAR_COMMAND_UPGRADE_HORSES:
        case WAR_COMMAND_UPGRADE_WOLVES:
        case WAR_COMMAND_UPGRADE_SCORPION:
        case WAR_COMMAND_UPGRADE_SPIDER:
        case WAR_COMMAND_UPGRADE_RAIN_OF_FIRE:
        case WAR_COMMAND_UPGRADE_POISON_CLOUD:
        case WAR_COMMAND_UPGRADE_WATER_ELEMENTAL:
        case WAR_COMMAND_UPGRADE_DAEMON:
        case WAR_COMMAND_UPGRADE_HEALING:
        case WAR_COMMAND_UPGRADE_RAISE_DEAD:
        case WAR_COMMAND_UPGRADE_FAR_SIGHT:
        case WAR_COMMAND_UPGRADE_DARK_VISION:
        case WAR_COMMAND_UPGRADE_INVISIBILITY:
        case WAR_COMMAND_UPGRADE_UNHOLY_ARMOR:
        {
            WarUnitCommandMapping commandMapping = getCommandMapping(commandType);

            assert(hasRemainingUpgrade(player, commandMapping.mappedType));

            WarUpgradeData upgradeData = getUpgradeData(commandMapping.mappedType);
            WarUpgradeStats stats = getUpgradeStats(commandMapping.mappedType);
            s32 upgradeLevel = getUpgradeLevel(player, commandMapping.mappedType);

            data.gold = stats.goldCost[upgradeLevel];
            data.frameIndex = upgradeData.frameIndices[upgradeLevel];
            break;
        }

        case WAR_COMMAND_SUMMON_SPIDER:
        case WAR_COMMAND_SUMMON_SCORPION:
        case WAR_COMMAND_SUMMON_DAEMON:
        case WAR_COMMAND_SUMMON_WATER_ELEMENTAL:
        case WAR_COMMAND_SPELL_HEALING:
        case WAR_COMMAND_SPELL_FAR_SIGHT:
        case WAR_COMMAND_SPELL_INVISIBILITY:
        case WAR_COMMAND_SPELL_RAIN_OF_FIRE:
        case WAR_COMMAND_SPELL_RAISE_DEAD:
        case WAR_COMMAND_SPELL_DARK_VISION:
        case WAR_COMMAND_SPELL_UNHOLY_ARMOR:
        case WAR_COMMAND_SPELL_POISON_CLOUD:
        {
            WarUnitCommandMapping commandMapping = getCommandMapping(commandType);
            WarSpellData spellData = getSpellData(commandMapping.mappedType);

            data.frameIndex = spellData.portraitFrameIndex;
            break;
        }

        // cancel
        case WAR_COMMAND_CANCEL:
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