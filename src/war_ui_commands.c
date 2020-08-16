void setFlashStatusFromCommandStatus(WarContext* context, WarCommandStatus status)
{
    switch (status)
    {
        case WAR_COMMAND_STATUS_NOT_ENOUGH_FOOD:
        {
            setFlashStatus(context, 1.5f, "NOT ENOUGH FOOD... BUILD MORE FARMS");
            break;
        }
        case WAR_COMMAND_STATUS_NOT_ENOUGH_GOLD:
        {
            setFlashStatus(context, 1.5f, "NOT ENOUGH GOLD... MINE MORE GOLD");
            break;
        }
        case WAR_COMMAND_STATUS_NOT_ENOUGH_WOOD:
        {
            setFlashStatus(context, 1.5f, "NOT ENOUGH LUMBER... CHOP MORE TREES");
            break;
        }
        case WAR_COMMAND_STATUS_INVALID_POSITION:
        {
            setFlashStatus(context, 1.5f, "CAN'T BUILD THERE");
            break;
        }
        default:
            break;
    }
}

void createAudioFromBuildCommandStatus(WarContext* context, WarCommandStatus status)
{
    switch (status)
    {
        case WAR_COMMAND_STATUS_INVALID_POSITION:
        {
            createAudio(context, WAR_UI_CANCEL, false);
            break;
        }
        case WAR_COMMAND_STATUS_DONE:
        {
            createAudio(context, WAR_BUILD_ROAD, false);
            break;
        }
        default:
            break;
    }
}

void executeMoveUICommand(WarContext* context, vec2 targetTile)
{
    assert(!vec2IsZero(targetTile));

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitGroup unitGroup = createUnitGroupFromSelection(context);
    assert(unitGroup.count > 0);

    WarCommand* command = createMoveCommand(context, player, unitGroup, targetTile);
    WarCommandStatus status = executeCommand(context, player, command);

    if (status < WAR_COMMAND_STATUS_FAILED)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeFollowUICommand(WarContext* context, WarEntity* targetEntity)
{
    assert(targetEntity);

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitGroup unitGroup = createUnitGroupFromSelection(context);
    assert(unitGroup.count > 0);

    WarCommand* command = createFollowCommand(context, player, unitGroup, targetEntity->id);
    WarCommandStatus status = executeCommand(context, player, command);

    if (status < WAR_COMMAND_STATUS_FAILED)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeAttackUICommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    assert(targetEntity || !vec2IsZero(targetTile));

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitGroup unitGroup = createUnitGroupFromSelection(context);
    assert(unitGroup.count > 0);

    WarCommand* command = targetEntity
        ? createAttackEnemyCommand(context, player, unitGroup, targetEntity->id)
        : createAttackPositionCommand(context, player, unitGroup, targetTile);

    WarCommandStatus status = executeCommand(context, player, command);

    if (status < WAR_COMMAND_STATUS_FAILED)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeStopUICommand(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitGroup unitGroup = createUnitGroupFromSelection(context);
    assert(unitGroup.count > 0);

    WarCommand* command = createStopCommand(context, player, unitGroup);
    executeCommand(context, player, command);
}

void executeGatherUICommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    assert(isGoldmineUnit(targetEntity) || isForest(targetEntity));

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitGroup unitGroup = createUnitGroupFromSelection(context);
    assert(unitGroup.count > 0);

    WarCommand* command = isGoldmineUnit(targetEntity)
        ? createGatherGoldCommand(context, player, unitGroup, targetEntity->id)
        : createGatherWoodCommand(context, player, unitGroup, targetEntity->id, targetTile);

    WarCommandStatus status = executeCommand(context, player, command);

    if (status < WAR_COMMAND_STATUS_FAILED)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeDeliverUICommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitGroup unitGroup = createUnitGroupFromSelection(context);
    assert(unitGroup.count > 0);

    WarCommand* command = createDeliverCommand(context, player, unitGroup, targetEntity ? targetEntity->id : 0);
    WarCommandStatus status = executeCommand(context, player, command);

    if (status < WAR_COMMAND_STATUS_FAILED)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeRepairUICommand(WarContext* context, WarEntity* targetEntity)
{
    assert(targetEntity);
    assert(isBuildingUnit(targetEntity));

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitGroup unitGroup = createUnitGroupFromSelection(context);
    assert(unitGroup.count > 0);

    WarCommand* command = createRepairCommand(context, player, unitGroup, targetEntity->id);
    WarCommandStatus status = executeCommand(context, player, command);

    if (status < WAR_COMMAND_STATUS_FAILED)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeSpellUICommand(WarContext* context, WarSpellType spellType, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitGroup unitGroup = createUnitGroupFromSelection(context);
    assert(unitGroup.count > 0);

    WarEntityId targetEntityId = targetEntity ? targetEntity->id : NO_ENTITY;

    WarCommand* command = createCastCommand(context, player, unitGroup, spellType, targetEntityId, targetTile);
    executeCommand(context, player, command);
}

bool executeUICommand(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarPlayerInfo* player = &map->players[0];
    WarUICommand* uiCommand = &map->uiCommand;

    if (uiCommand->type == WAR_UI_COMMAND_NONE)
    {
        return false;
    }

    switch (uiCommand->type)
    {
        case WAR_UI_COMMAND_MOVE:
        {
            if (!wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                return false;
            }

            bool clickOnMap = rectContainsf(map->mapPanel, input->pos.x, input->pos.y);
            bool clickOnMinimap = rectContainsf(map->minimapPanel, input->pos.x, input->pos.y);

            if (clickOnMap || clickOnMinimap)
            {
                vec2 targetTile = clickOnMap
                    ? vec2MapToTileCoordinates(vec2ScreenToMapCoordinates(context, input->pos))
                    : vec2ScreenToMinimapCoordinates(context, input->pos);

                executeMoveUICommand(context, targetTile);

                uiCommand->type = WAR_UI_COMMAND_NONE;
                return true;
            }
        }

        case WAR_UI_COMMAND_STOP:
        {
            executeStopUICommand(context);

            uiCommand->type = WAR_UI_COMMAND_NONE;
            return true;
        }

        case WAR_UI_COMMAND_GATHER:
        {
            if (!wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                return false;
            }

            if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
            {
                vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                WarEntity* targetEntity = findEntityAt(context, targetTile);
                if (targetEntity)
                {
                    s32 count = map->selectedEntities.count;
                    WarEntityId unitIds[MAX_UNIT_SELECTION_COUNT] = {0};
                    memcpy(unitIds, map->selectedEntities.items, count * sizeof(WarEntityId));

                    if (isUnitOfType(targetEntity, WAR_UNIT_GOLDMINE))
                    {
                        if (!isUnitUnknown(map, targetEntity))
                        {
                            executeGatherUICommand(context, targetEntity, targetTile);
                        }
                        else
                        {
                            executeMoveUICommand(context, targetTile);
                        }
                    }
                    else if (isForest(targetEntity))
                    {
                        if (!isTileUnkown(map, (s32)targetTile.x, (s32)targetTile.y))
                        {
                            executeGatherUICommand(context, targetEntity, targetTile);
                        }
                        else
                        {
                            WarTree* tree = findAccesibleTree(context, targetEntity, targetTile);
                            if (tree)
                            {
                                targetTile = vec2i(tree->tilex, tree->tiley);
                                executeGatherUICommand(context, targetEntity, targetTile);
                            }
                            else
                            {
                                executeMoveUICommand(context, targetTile);
                            }
                        }
                    }
                }

                uiCommand->type = WAR_UI_COMMAND_NONE;
                return true;
            }
        }

        case WAR_UI_COMMAND_DELIVER:
        {
            executeDeliverUICommand(context, NULL);

            uiCommand->type = WAR_UI_COMMAND_NONE;
            return true;
        }

        case WAR_UI_COMMAND_REPAIR:
        {
            if (!wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                return false;
            }

            if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
            {
                vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                if (isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y) ||
                    isTileFog(map, (s32)targetTile.x, (s32)targetTile.y))
                {
                    WarEntity* targetEntity = findEntityAt(context, targetTile);
                    if (targetEntity && isBuildingUnit(targetEntity))
                    {
                        executeRepairUICommand(context, targetEntity);
                    }
                }

                uiCommand->type = WAR_UI_COMMAND_NONE;
                return true;
            }
        }

        case WAR_UI_COMMAND_ATTACK:
        {
            if (!wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                return false;
            }

            if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
            {
                vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                WarEntity* targetEntity = findEntityAt(context, targetTile);
                if (targetEntity)
                {
                    if (isUnit(targetEntity))
                    {
                        // if the target entity is not visible or partially visible, just attack to the point
                        if (isUnitUnknown(map, targetEntity))
                            targetEntity = NULL;
                    }
                    else if (isWall(targetEntity))
                    {
                        // if the target wall piece is not visible, just attack to the point
                        if (!isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y))
                            targetEntity = NULL;
                    }
                }

                executeAttackUICommand(context, targetEntity, targetTile);

                uiCommand->type = WAR_UI_COMMAND_NONE;
                return true;
            }
            else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
            {
                vec2 targetTile = vec2ScreenToMinimapCoordinates(context, input->pos);

                executeAttackUICommand(context, NULL, targetTile);

                uiCommand->type = WAR_UI_COMMAND_NONE;
                return true;
            }
        }

        case WAR_UI_COMMAND_TRAIN_FOOTMAN:
        case WAR_UI_COMMAND_TRAIN_GRUNT:
        case WAR_UI_COMMAND_TRAIN_PEASANT:
        case WAR_UI_COMMAND_TRAIN_PEON:
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
            WarUnitType unitType = uiCommand->train.unitType;

            assert(map->selectedEntities.count == 1);
            WarEntityId producerId = map->selectedEntities.items[0];
            assert(producerId);

            WarCommand* command = createTrainCommand(context, player, unitType, producerId);
            WarCommandStatus status = executeCommand(context, player, command);

            setFlashStatusFromCommandStatus(context, status);

            uiCommand->type = WAR_UI_COMMAND_NONE;
            return true;
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
            WarUpgradeType upgradeToBuild = uiCommand->upgrade.upgradeToBuild;

            assert(map->selectedEntities.count == 1);
            WarEntityId producerId = map->selectedEntities.items[0];
            assert(producerId);

            assert(hasRemainingUpgrade(player, upgradeToBuild));

            WarCommand* command = createUpgradeCommand(context, player, upgradeToBuild, producerId);
            WarCommandStatus status = executeCommand(context, player, command);

            setFlashStatusFromCommandStatus(context, status);

            uiCommand->type = WAR_UI_COMMAND_NONE;
            return true;
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
            // NOTE: It could happen that the worker which is selected to build
            // get inside a building (because it's mining or delivering something)
            // at that moment the worker is no longer selected
            if (map->selectedEntities.count == 0)
            {
                uiCommand->type = WAR_UI_COMMAND_NONE;
                return true;
            }

            if (!wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                return false;
            }

            if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
            {
                WarEntityId workerId = map->selectedEntities.items[0];

                vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                WarUnitType unitType = uiCommand->build.unitType;

                WarCommand* command = createBuildCommand(context, player, unitType, workerId, targetTile);
                WarCommandStatus status = executeCommand(context, player, command);

                if (status == WAR_COMMAND_STATUS_RUNNING)
                {
                    uiCommand->type = WAR_UI_COMMAND_NONE;
                }
                else
                {
                    setFlashStatusFromCommandStatus(context, status);
                    createAudioFromBuildCommandStatus(context, status);
                }

                return true;
            }
        }

        case WAR_UI_COMMAND_BUILD_WALL:
        {
            if (!wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                return false;
            }

            if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
            {
                WarEntityId townHallId = map->selectedEntities.items[0];
                WarEntity* townHall = findEntity(context, townHallId);

                WarUnitType townHallType = getTownHallOfRace(player->race);
                assert(isUnitOfType(townHall, townHallType));

                vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                WarCommand* command = createBuildWallCommand(context, player, targetTile);
                WarCommandStatus status = executeCommand(context, player, command);

                setFlashStatusFromCommandStatus(context, status);
                createAudioFromBuildCommandStatus(context, status);

                return true;
            }
        }

        case WAR_UI_COMMAND_BUILD_ROAD:
        {
            if (!wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                return false;
            }

            if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
            {
                WarEntityId townHallId = map->selectedEntities.items[0];
                WarEntity* townHall = findEntity(context, townHallId);

                WarUnitType townHallType = getTownHallOfRace(player->race);
                assert(isUnitOfType(townHall, townHallType));

                vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                WarCommand* command = createBuildRoadCommand(context, player, targetTile);
                WarCommandStatus status = executeCommand(context, player, command);

                setFlashStatusFromCommandStatus(context, status);
                createAudioFromBuildCommandStatus(context, status);

                return true;
            }
        }

        case WAR_UI_COMMAND_SUMMON_SPIDER:
        case WAR_UI_COMMAND_SUMMON_SCORPION:
        case WAR_UI_COMMAND_SUMMON_DAEMON:
        case WAR_UI_COMMAND_SUMMON_WATER_ELEMENTAL:
        {
            WarUICommandMapping uiCommandMapping = getCommandMapping(uiCommand->type);
            WarSpellType spellType = (WarSpellType)uiCommandMapping.mappedType;
            executeSpellUICommand(context, spellType, NULL, VEC2_ZERO);

            uiCommand->type = WAR_UI_COMMAND_NONE;
            return true;
        }

        case WAR_UI_COMMAND_SPELL_RAIN_OF_FIRE:
        case WAR_UI_COMMAND_SPELL_POISON_CLOUD:
        case WAR_UI_COMMAND_SPELL_HEALING:
        case WAR_UI_COMMAND_SPELL_INVISIBILITY:
        case WAR_UI_COMMAND_SPELL_UNHOLY_ARMOR:
        case WAR_UI_COMMAND_SPELL_RAISE_DEAD:
        case WAR_UI_COMMAND_SPELL_FAR_SIGHT:
        case WAR_UI_COMMAND_SPELL_DARK_VISION:
        {
            if (!wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                return false;
            }

            if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
            {
                vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                WarEntity* targetEntity = findEntityAt(context, targetTile);

                WarUICommandMapping uiCommandMapping = getCommandMapping(uiCommand->type);
                WarSpellType spellType = (WarSpellType)uiCommandMapping.mappedType;
                executeSpellUICommand(context, spellType, targetEntity, targetTile);

                uiCommand->type = WAR_UI_COMMAND_NONE;
                return true;
            }
            else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
            {
                vec2 targetTile = vec2ScreenToMinimapCoordinates(context, input->pos);

                WarUICommandMapping uiCommandMapping = getCommandMapping(uiCommand->type);
                WarSpellType spellType = (WarSpellType)uiCommandMapping.mappedType;
                executeSpellUICommand(context, spellType, NULL, targetTile);

                uiCommand->type = WAR_UI_COMMAND_NONE;
                return true;
            }

            break;
        }

        case WAR_UI_COMMAND_BUILD_BASIC:
        case WAR_UI_COMMAND_BUILD_ADVANCED:
        {
            // do nothing here
            break;
        }

        default:
        {
            logError("Not implemented command: %d\n", uiCommand->type);
            return false;
        }
    }

    return false;
}

// train units
void trainUnit(WarContext* context, WarUICommandType commandType, WarUnitType unitType)
{
    WarMap* map = context->map;

    map->uiCommand.type = commandType;
    map->uiCommand.train.unitType = unitType;
}

void trainFootman(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_FOOTMAN, WAR_UNIT_FOOTMAN);
}

void trainGrunt(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_GRUNT, WAR_UNIT_GRUNT);
}

void trainPeasant(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_PEASANT, WAR_UNIT_PEASANT);
}

void trainPeon(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_PEON, WAR_UNIT_PEON);
}

void trainHumanCatapult(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_CATAPULT_HUMANS, WAR_UNIT_CATAPULT_HUMANS);
}

void trainOrcCatapult(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_CATAPULT_ORCS, WAR_UNIT_CATAPULT_ORCS);
}

void trainKnight(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_KNIGHT, WAR_UNIT_KNIGHT);
}

void trainRaider(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_RAIDER, WAR_UNIT_KNIGHT);
}

void trainArcher(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_ARCHER, WAR_UNIT_ARCHER);
}

void trainSpearman(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_SPEARMAN, WAR_UNIT_SPEARMAN);
}

void trainConjurer(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_CONJURER, WAR_UNIT_CONJURER);
}

void trainWarlock(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_WARLOCK, WAR_UNIT_WARLOCK);
}

void trainCleric(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_CLERIC, WAR_UNIT_CLERIC);
}

void trainNecrolyte(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_UI_COMMAND_TRAIN_NECROLYTE, WAR_UNIT_NECROLYTE);
}

// upgrades
void upgradeUpgrade(WarContext* context, WarUICommandType commandType, WarUpgradeType upgradeToBuild)
{
    WarMap* map = context->map;

    map->uiCommand.type = commandType;
    map->uiCommand.upgrade.upgradeToBuild = upgradeToBuild;
}

void upgradeSwords(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_SWORDS, WAR_UPGRADE_SWORDS);
}

void upgradeAxes(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_AXES, WAR_UPGRADE_AXES);
}

void upgradeHumanShields(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_SHIELD_HUMANS, WAR_UPGRADE_SHIELD);
}

void upgradeOrcsShields(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_SHIELD_ORCS, WAR_UPGRADE_SHIELD);
}

void upgradeArrows(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_ARROWS, WAR_UPGRADE_ARROWS);
}

void upgradeSpears(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_SPEARS, WAR_UPGRADE_SPEARS);
}

void upgradeHorses(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_HORSES, WAR_UPGRADE_HORSES);
}

void upgradeWolves(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_WOLVES, WAR_UPGRADE_WOLVES);
}

void upgradeScorpions(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_SCORPION, WAR_UPGRADE_SCORPIONS);
}

void upgradeSpiders(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_SPIDER, WAR_UPGRADE_SPIDERS);
}

void upgradeRainOfFire(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_RAIN_OF_FIRE, WAR_UPGRADE_RAIN_OF_FIRE);
}

void upgradePoisonCloud(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_POISON_CLOUD, WAR_UPGRADE_POISON_CLOUD);
}

void upgradeWaterElemental(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_WATER_ELEMENTAL, WAR_UPGRADE_WATER_ELEMENTAL);
}

void upgradeDaemon(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_DAEMON, WAR_UPGRADE_DAEMON);
}

void upgradeHealing(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_HEALING, WAR_UPGRADE_HEALING);
}

void upgradeRaiseDead(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_RAISE_DEAD, WAR_UPGRADE_RAISE_DEAD);
}

void upgradeFarSight(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_FAR_SIGHT, WAR_UPGRADE_FAR_SIGHT);
}

void upgradeDarkVision(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_DARK_VISION, WAR_UPGRADE_DARK_VISION);
}

void upgradeInvisibility(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_INVISIBILITY, WAR_UPGRADE_INVISIBILITY);
}

void upgradeUnholyArmor(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_UI_COMMAND_UPGRADE_UNHOLY_ARMOR, WAR_UPGRADE_UNHOLY_ARMOR);
}

// cancel
void cancel(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    map->uiCommand.type = WAR_UI_COMMAND_NONE;

    for (s32 i = 0; i < map->selectedEntities.count; i++)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[i];
        WarEntity* selectedEntity = findEntity(context, selectedEntityId);
        if (selectedEntity)
        {
            bool playCollapsedSound = isBuildingUnit(selectedEntity) && (isBuilding(selectedEntity) || isGoingToBuild(selectedEntity));

            WarCommand* command = createCancelCommand(context, player, selectedEntityId);
            WarCommandStatus status = executeCommand(context, player, command);

            if (status < WAR_COMMAND_STATUS_FAILED && playCollapsedSound)
            {
                createAudioRandom(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, false);
            }
        }
    }
}

// basic
void move(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_MOVE;
}

void stop(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_STOP;
}

void harvest(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_GATHER;
}

void deliver(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_DELIVER;
}

void repair(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_REPAIR;
}

void attack(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_ATTACK;
}

void buildBasic(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_BUILD_BASIC;
}

void buildAdvanced(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_BUILD_ADVANCED;
}

void buildBuilding(WarContext* context, WarUICommandType commandType, WarUnitType unitType)
{
    WarMap* map = context->map;

    map->uiCommand.type = commandType;
    map->uiCommand.build.unitType = unitType;
}

void buildFarmHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_FARM_HUMANS, WAR_UNIT_FARM_HUMANS);
}

void buildFarmOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_FARM_ORCS, WAR_UNIT_FARM_ORCS);
}

void buildBarracksHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_BARRACKS_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void buildBarracksOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_BARRACKS_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void buildChurch(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_CHURCH, WAR_UNIT_CHURCH);
}

void buildTemple(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_TEMPLE, WAR_UNIT_TEMPLE);
}

void buildTowerHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_TOWER_HUMANS, WAR_UNIT_TOWER_HUMANS);
}

void buildTowerOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_TOWER_ORCS, WAR_UNIT_TOWER_ORCS);
}

void buildTownHallHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_TOWNHALL_HUMANS, WAR_UNIT_TOWNHALL_HUMANS);
}

void buildTownHallOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_TOWNHALL_ORCS, WAR_UNIT_TOWNHALL_ORCS);
}

void buildLumbermillHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_LUMBERMILL_HUMANS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void buildLumbermillOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_LUMBERMILL_ORCS, WAR_UNIT_LUMBERMILL_ORCS);
}

void buildStable(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_STABLE, WAR_UNIT_STABLE);
}

void buildKennel(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_KENNEL, WAR_UNIT_KENNEL);
}

void buildBlacksmithHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_BLACKSMITH_HUMANS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void buildBlacksmithOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_UI_COMMAND_BUILD_BLACKSMITH_ORCS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void buildWall(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_BUILD_WALL;
}

void buildRoad(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_BUILD_ROAD;
}

// spells
void castRainOfFire(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SPELL_RAIN_OF_FIRE;
}

void castPoisonCloud(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SPELL_POISON_CLOUD;
}

void castHeal(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SPELL_HEALING;
}

void castFarSight(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SPELL_FAR_SIGHT;
}

void castDarkVision(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SPELL_DARK_VISION;
}

void castInvisibility(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SPELL_INVISIBILITY;
}

void castUnHolyArmor(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SPELL_UNHOLY_ARMOR;
}

void castRaiseDead(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SPELL_RAISE_DEAD;
}

// summons
void summonSpider(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SUMMON_SPIDER;
}

void summonScorpion(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SUMMON_SCORPION;
}

void summonDaemon(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SUMMON_DAEMON;
}

void summonWaterElemental(WarContext* context, WarEntity* entity)
{
    context->map->uiCommand.type = WAR_UI_COMMAND_SUMMON_WATER_ELEMENTAL;
}
