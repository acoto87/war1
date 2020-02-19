void executeMoveCommand(WarContext* context, vec2 targetPoint)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarPlayerInfo* player = &map->players[0];

    bool goingToMove = false;

    s32 selEntitiesCount = map->selectedEntities.count;

    // move the selected units to the target point,
    // but keeping the bounding box that the
    // selected units make, this is an intent to keep the
    // formation of the selected units
    //
    rect* rs = (rect*)xcalloc(selEntitiesCount, sizeof(rect));

    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        rs[i] = getUnitRect(entity);
    }

    rect bbox = rs[0];

    for(s32 i = 1; i < selEntitiesCount; i++)
    {
        if (rs[i].x < bbox.x)
            bbox.x = rs[i].x;
        if (rs[i].y < bbox.y)
            bbox.y = rs[i].y;
        if (rs[i].x + rs[i].width > bbox.x + bbox.width)
            bbox.width = (rs[i].x + rs[i].width) - bbox.x;
        if (rs[i].y + rs[i].height > bbox.y + bbox.height)
            bbox.height = (rs[i].y + rs[i].height) - bbox.y;
    }

    rect targetbbox = rectf(
        targetPoint.x - halff(bbox.width),
        targetPoint.y - halff(bbox.height),
        bbox.width,
        bbox.height);

    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        vec2 position = vec2f(
            rs[i].x + halff(rs[i].width),
            rs[i].y + halff(rs[i].height));

        position = vec2MapToTileCoordinates(position);

        rect targetRect = rectf(
            targetbbox.x + (rs[i].x - bbox.x),
            targetbbox.y + (rs[i].y - bbox.y),
            rs[i].width,
            rs[i].height);

        vec2 target = vec2f(
            targetRect.x + halff(targetRect.width),
            targetRect.y + halff(targetRect.height));

        target = vec2MapToTileCoordinates(target);

        if (isDudeUnit(entity) && isFriendlyUnit(context, entity))
        {
            if (isKeyPressed(input, WAR_KEY_SHIFT))
            {
                if (isPatrolling(entity))
                {
                    if(isMoving(entity))
                    {
                        WarState* moveState = getMoveState(entity);
                        vec2ListAdd(&moveState->move.positions, target);
                    }

                    WarState* patrolState = getPatrolState(entity);
                    vec2ListAdd(&patrolState->patrol.positions, target);
                }
                else if(isMoving(entity) && !isAttacking(entity))
                {
                    WarState* moveState = getMoveState(entity);
                    vec2ListAdd(&moveState->move.positions, target);
                }
                else
                {
                    WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, target));
                    changeNextState(context, entity, moveState, true, true);
                }
            }
            else
            {
                WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, target));
                changeNextState(context, entity, moveState, true, true);

                // WarState* patrolState = createPatrolState(context, entity, 2, arrayArg(vec2, position, target));
                // changeNextState(context, entity, patrolState, true, true);
            }

            goingToMove = true;
        }
    }

    if (goingToMove)
    {
        playAcknowledgementSound(context, player);
    }

    free(rs);
}

void executeFollowCommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool goingToFollow = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for (s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isFriendlyUnit(context, entity))
        {
            WarState* followState = createFollowState(context, entity, targetEntity->id, VEC2_ZERO, 1);
            changeNextState(context, entity, followState, true, true);

            goingToFollow = true;
        }
    }

    if (goingToFollow)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeStopCommand(WarContext* context)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for (s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isFriendlyUnit(context, entity))
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }
    }
}

void executeHarvestCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    assert(isUnitOfType(targetEntity, WAR_UNIT_GOLDMINE) ||
           isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST));

    bool goingToHarvest = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isFriendlyUnit(context, entity))
        {
            if (isWorkerUnit(entity))
            {
                if (isCarryingResources(entity))
                {
                    // find the closest town hall to deliver the gold
                    WarRace race = getUnitRace(entity);
                    WarUnitType townHallType = getTownHallOfRace(race);
                    WarEntity* townHall = findClosestUnitOfType(context, entity, townHallType);
                    if (townHall)
                    {
                        WarState* deliverState = createDeliverState(context, entity, townHall->id);
                        deliverState->nextState = isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST)
                            ? createGatherWoodState(context, entity, targetEntity->id, targetTile)
                            : createGatherGoldState(context, entity, targetEntity->id);

                        changeNextState(context, entity, deliverState, true, true);
                    }
                }
                else
                {
                    WarState* gatherGoldOrWoodState = isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST)
                        ? createGatherWoodState(context, entity, targetEntity->id, targetTile)
                        : createGatherGoldState(context, entity, targetEntity->id);

                    changeNextState(context, entity, gatherGoldOrWoodState, true, true);
                }

                goingToHarvest = true;
            }
            else if (isDudeUnit(entity))
            {
                vec2 position = getUnitCenterPosition(entity, true);
                WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position,  targetTile));
                changeNextState(context, entity, moveState, true, true);

                goingToHarvest = true;
            }
        }
    }

    if (goingToHarvest)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeDeliverCommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool goingToDeliver = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isFriendlyUnit(context, entity))
        {
            WarEntity* townHall = targetEntity;
            if (!townHall)
            {
                WarRace race = getUnitRace(entity);
                WarUnitType townHallType = getTownHallOfRace(race);
                townHall = findClosestUnitOfType(context, entity, townHallType);
                assert(townHall);
            }

            if (isWorkerUnit(entity) && isCarryingResources(entity))
            {
                WarState* deliverState = createDeliverState(context, entity, townHall->id);
                changeNextState(context, entity, deliverState, true, true);

                goingToDeliver = true;
            }
            else if (isDudeUnit(entity))
            {
                WarState* followState = createFollowState(context, entity, townHall->id, VEC2_ZERO, 1);
                changeNextState(context, entity, followState, true, true);

                goingToDeliver = true;
            }
        }
    }

    if (goingToDeliver)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeRepairCommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool goingToRepair = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isFriendlyUnit(context, entity))
        {
            // the unit can't repair itself
            if (entity->id == targetEntity->id)
            {
                continue;
            }

            if (isWorkerUnit(entity))
            {
                WarState* repairState = createRepairState(context, entity, targetEntity->id);
                changeNextState(context, entity, repairState, true, true);

                goingToRepair = true;
            }
        }
    }

    if (goingToRepair)
    {
        playAcknowledgementSound(context, player);
    }
}

void executeSummonCommand(WarContext* context, WarUnitCommandType summonType)
{
    WarMap* map = context->map;

    bool casted = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isConjurerOrWarlockUnit(entity))
        {
            WarUnitComponent* unit = &entity->unit;

            // when the unit summon another unit, it is not invisible anymore
            unit->invisible = false;
            unit->invisibilityTime = 0;

            WarUnitCommandMapping commandMapping = getCommandMapping(summonType);
            WarSpellMapping spellMapping = getSpellMapping(commandMapping.mappedType);
            WarSpellStats stats = getSpellStats(commandMapping.mappedType);

            while (decreaseUnitMana(context, entity, stats.manaCost))
            {
                vec2 position = getUnitCenterPosition(entity, true);
                vec2 spawnPosition = findEmptyPosition(map->finder, position);

                WarEntity* summonedUnit = createUnit(context, spellMapping.mappedType,
                                                     spawnPosition.x, spawnPosition.y,
                                                     unit->player, WAR_RESOURCE_NONE, 0, true);

                vec2 unitSize = getUnitSize(summonedUnit);
                setStaticEntity(map->finder, spawnPosition.x, spawnPosition.y, unitSize.x, unitSize.y, summonedUnit->id);

                WarEntity* animEntity = createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                createSpellAnimation(context, animEntity, vec2TileToMapCoordinates(spawnPosition, true));

                casted = true;
            }
        }
    }

    if (casted)
    {
        createAudio(context, WAR_NORMAL_SPELL, false);
    }
}

void executeRainOfFireCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isConjurerOrWarlockUnit(entity))
        {
            WarState* castState = createCastState(context, entity, WAR_SPELL_RAIN_OF_FIRE, 0, targetTile);
            changeNextState(context, entity, castState, true, true);
        }
    }
}

void executePoisonCloudCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isConjurerOrWarlockUnit(entity))
        {
            WarState* castState = createCastState(context, entity, WAR_SPELL_POISON_CLOUD, 0, targetTile);
            changeNextState(context, entity, castState, true, true);
        }
    }
}

void executeHealingCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;

    if (targetEntity && isDudeUnit(targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = findEntity(context, entityId);
            assert(entity);

            if (isClericOrNecrolyteUnit(entity))
            {
                // the unit can't heal itself
                if (entity->id != targetEntity->id)
                {
                    WarState* castState = createCastState(context, entity, WAR_SPELL_HEALING, targetEntity->id, targetTile);
                    changeNextState(context, entity, castState, true, true);
                }
            }
        }
    }
}

void executeInvisiblityCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;

    if (targetEntity && isDudeUnit(targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = findEntity(context, entityId);
            assert(entity);

            if (isClericOrNecrolyteUnit(entity))
            {
                WarState* castState = createCastState(context, entity, WAR_SPELL_INVISIBILITY, targetEntity->id, targetTile);
                changeNextState(context, entity, castState, true, true);
            }
        }
    }
}

void executeUnholyArmorCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;

    if (targetEntity && isDudeUnit(targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = findEntity(context, entityId);
            assert(entity);

            if (isClericOrNecrolyteUnit(entity))
            {
                WarState* castState = createCastState(context, entity, WAR_SPELL_UNHOLY_ARMOR, targetEntity->id, targetTile);
                changeNextState(context, entity, castState, true, true);
            }
        }
    }
}

void executeRaiseDeadCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isClericOrNecrolyteUnit(entity))
        {
            WarState* castState = createCastState(context, entity, WAR_SPELL_RAISE_DEAD, 0, targetTile);
            changeNextState(context, entity, castState, true, true);
        }
    }
}

void executeSightCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isClericOrNecrolyteUnit(entity))
        {
            WarSpellType spellType = isHumanUnit(entity) ? WAR_SPELL_FAR_SIGHT : WAR_SPELL_DARK_VISION;
            WarState* castState = createCastState(context, entity, spellType, 0, targetTile);
            changeNextState(context, entity, castState, true, true);
        }
    }
}

void executeAttackCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool playSound = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = findEntity(context, entityId);
        assert(entity);

        if (isFriendlyUnit(context, entity))
        {
            if (targetEntity)
            {
                // the unit can't attack itself
                if (entity->id != targetEntity->id)
                {
                    if (canAttack(context, entity, targetEntity))
                    {
                        WarState* attackState = createAttackState(context, entity, targetEntity->id, targetTile);
                        changeNextState(context, entity, attackState, true, true);

                        playSound = true;
                    }
                    else if (isWorkerUnit(entity))
                    {
                        WarState* followState = createFollowState(context, entity, targetEntity->id, VEC2_ZERO, 1);
                        changeNextState(context, entity, followState, true, true);
                    }
                }
            }
            else
            {
                WarState* attackState = createAttackState(context, entity, 0, targetTile);
                changeNextState(context, entity, attackState, true, true);

                playSound = true;
            }
        }
    }

    if (playSound)
    {
        playAcknowledgementSound(context, player);
    }
}

bool executeCommand(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarPlayerInfo* player = &map->players[0];
    WarUnitCommand* command = &map->command;

    if (command->type == WAR_COMMAND_NONE)
    {
        return false;
    }

    switch (command->type)
    {
        case WAR_COMMAND_MOVE:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);

                    executeMoveCommand(context, targetPoint);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = vec2ScreenToMinimapCoordinates(context, input->pos);
                    vec2 targetPoint = vec2TileToMapCoordinates(targetTile, true);
                    executeMoveCommand(context, targetPoint);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_STOP:
        {
            executeStopCommand(context);

            command->type = WAR_COMMAND_NONE;
            return true;
        }

        case WAR_COMMAND_HARVEST:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, targetTile.x, targetTile.y);
                    WarEntity* targetEntity = findEntity(context, targetEntityId);
                    if (targetEntity)
                    {
                        if (isUnitOfType(targetEntity, WAR_UNIT_GOLDMINE))
                        {
                            if (!isUnitUnknown(map, targetEntity))
                                executeHarvestCommand(context, targetEntity, targetTile);
                            else
                                executeMoveCommand(context, targetPoint);
                        }
                        else if (isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST))
                        {
                            if (!isTileUnkown(map, (s32)targetTile.x, (s32)targetTile.y))
                            {
                                executeHarvestCommand(context, targetEntity, targetTile);
                            }
                            else
                            {
                                WarTree* tree = findAccesibleTree(context, targetEntity, targetTile);
                                if (tree)
                                {
                                    targetTile = vec2i(tree->tilex, tree->tiley);
                                    executeHarvestCommand(context, targetEntity, targetTile);
                                }
                                else
                                {
                                    executeMoveCommand(context, targetPoint);
                                }
                            }
                        }
                    }

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_DELIVER:
        {
            executeDeliverCommand(context, NULL);

            command->type = WAR_COMMAND_NONE;
            return true;
        }

        case WAR_COMMAND_REPAIR:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);
                    if (isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y) ||
                        isTileFog(map, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        WarEntityId targetEntityId = getTileEntityId(map->finder, targetTile.x, targetTile.y);
                        WarEntity* targetEntity = findEntity(context, targetEntityId);
                        if (targetEntity && isBuildingUnit(targetEntity))
                        {
                            executeRepairCommand(context, targetEntity);
                        }
                    }

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_ATTACK:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, targetTile.x, targetTile.y);
                    WarEntity* targetEntity = findEntity(context, targetEntityId);
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

                    executeAttackCommand(context, targetEntity, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = vec2ScreenToMinimapCoordinates(context, input->pos);
                    executeAttackCommand(context, NULL, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_TRAIN_FOOTMAN:
        case WAR_COMMAND_TRAIN_GRUNT:
        case WAR_COMMAND_TRAIN_PEASANT:
        case WAR_COMMAND_TRAIN_PEON:
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
            WarUnitType unitToTrain = command->train.unitToTrain;
            WarUnitType buildingUnit = command->train.buildingUnit;

            assert(map->selectedEntities.count == 1);

            WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
            assert(selectedEntity && isBuildingUnit(selectedEntity));
            assert(selectedEntity->unit.type == buildingUnit);

            WarUnitStats stats = getUnitStats(unitToTrain);
            if (checkFarmFood(context, player) &&
                decreasePlayerResources(context, player, stats.goldCost, stats.woodCost))
            {
                WarState* trainState = createTrainState(context, selectedEntity, unitToTrain, stats.buildTime);
                changeNextState(context, selectedEntity, trainState, true, true);
            }

            command->type = WAR_COMMAND_NONE;
            return true;
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
            WarUpgradeType upgradeToBuild = command->upgrade.upgradeToBuild;
            WarUnitType buildingUnit = command->upgrade.buildingUnit;

            assert(map->selectedEntities.count == 1);

            WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[0]);
            assert(selectedEntity && isBuildingUnit(selectedEntity));
            assert(selectedEntity->unit.type == buildingUnit);

            assert(hasRemainingUpgrade(player, upgradeToBuild));

            WarUpgradeStats stats = getUpgradeStats(upgradeToBuild);
            s32 level = getUpgradeLevel(player, upgradeToBuild);
            if (decreasePlayerResources(context, player, stats.goldCost[level], 0))
            {
                WarState* upgradeState = createUpgradeState(context, selectedEntity, upgradeToBuild, stats.buildTime);
                changeNextState(context, selectedEntity, upgradeState, true, true);
            }

            command->type = WAR_COMMAND_NONE;
            return true;
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
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    assert(map->selectedEntities.count > 0);

                    WarEntityId workerId = map->selectedEntities.items[0];
                    WarEntity* worker = findEntity(context, workerId);
                    assert(worker);

                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    WarUnitType buildingToBuild = command->build.buildingToBuild;

                    WarBuildingStats stats = getBuildingStats(buildingToBuild);
                    if (checkTileToBuild(context, buildingToBuild, targetTile.x, targetTile.y))
                    {
                        if (decreasePlayerResources(context, player, stats.goldCost, stats.woodCost))
                        {
                            WarEntity* building = createBuilding(context, buildingToBuild, targetTile.x, targetTile.y, 0, true);
                            WarState* repairState = createRepairState(context, worker, building->id);
                            changeNextState(context, worker, repairState, true, true);

                            command->type = WAR_COMMAND_NONE;
                        }
                    }
                    else
                    {
                        createAudio(context, WAR_UI_CANCEL, false);
                    }

                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_BUILD_WALL:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    assert(map->selectedEntities.count > 0);

                    WarEntityId townHallId = map->selectedEntities.items[0];
                    WarEntity* townHall = findEntity(context, townHallId);

                    WarUnitType townHallType = getTownHallOfRace(player->race);
                    assert(isUnitOfType(townHall, townHallType));

                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    if (checkTileToBuildRoadOrWall(context, targetTile.x, targetTile.y))
                    {
                        if (decreasePlayerResources(context, player, WAR_WALL_GOLD_COST, WAR_WALL_WOOD_COST))
                        {
                            WarEntity* wall = map->wall;
                            WarWallPiece* piece = addWallPiece(wall, targetTile.x, targetTile.y, 0);
                            piece->hp = WAR_WALL_MAX_HP;
                            piece->maxhp = WAR_WALL_MAX_HP;

                            determineWallTypes(context, wall);

                            // don't reset the current command if the player is building
                            // roads or walls, to allow rapid construction of those structures
                            //
                            // command->type = WAR_COMMAND_NONE;

                            createAudio(context, WAR_BUILD_ROAD, false);
                        }
                    }
                    else
                    {
                        createAudio(context, WAR_UI_CANCEL, false);
                    }

                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_BUILD_ROAD:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    assert(map->selectedEntities.count > 0);

                    WarEntityId townHallId = map->selectedEntities.items[0];
                    WarEntity* townHall = findEntity(context, townHallId);

                    WarUnitType townHallType = getTownHallOfRace(player->race);
                    assert(isUnitOfType(townHall, townHallType));

                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    if (checkTileToBuildRoadOrWall(context, targetTile.x, targetTile.y))
                    {
                        if (decreasePlayerResources(context, player, WAR_ROAD_GOLD_COST, WAR_ROAD_WOOD_COST))
                        {
                            WarEntity* road = map->road;
                            addRoadPiece(road, targetTile.x, targetTile.y, 0);

                            determineRoadTypes(context, road);

                            // don't reset the current command if the player is building
                            // roads or walls, to allow rapid construction of those structures
                            //
                            // command->type = WAR_COMMAND_NONE;

                            createAudio(context, WAR_BUILD_ROAD, false);
                        }
                    }
                    else
                    {
                        createAudio(context, WAR_UI_CANCEL, false);
                    }

                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SUMMON_SPIDER:
        case WAR_COMMAND_SUMMON_SCORPION:
        case WAR_COMMAND_SUMMON_DAEMON:
        case WAR_COMMAND_SUMMON_WATER_ELEMENTAL:
        {
            executeSummonCommand(context, command->type);

            command->type = WAR_COMMAND_NONE;
            return true;
        }

        case WAR_COMMAND_SPELL_RAIN_OF_FIRE:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    executeRainOfFireCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = vec2ScreenToMinimapCoordinates(context, input->pos);
                    executeRainOfFireCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_POISON_CLOUD:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    executePoisonCloudCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = vec2ScreenToMinimapCoordinates(context, input->pos);
                    executePoisonCloudCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_HEALING:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, targetTile.x, targetTile.y);
                    WarEntity* targetEntity = findEntity(context, targetEntityId);

                    executeHealingCommand(context, targetEntity, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_INVISIBILITY:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, targetTile.x, targetTile.y);
                    WarEntity* targetEntity = findEntity(context, targetEntityId);

                    executeInvisiblityCommand(context, targetEntity, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_UNHOLY_ARMOR:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, targetTile.x, targetTile.y);
                    WarEntity* targetEntity = findEntity(context, targetEntityId);

                    executeUnholyArmorCommand(context, targetEntity, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_RAISE_DEAD:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    executeRaiseDeadCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_FAR_SIGHT:
        case WAR_COMMAND_SPELL_DARK_VISION:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    executeSightCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = vec2ScreenToMinimapCoordinates(context, input->pos);
                    executeSightCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_BUILD_BASIC:
        case WAR_COMMAND_BUILD_ADVANCED:
        {
            // do nothing here
            break;
        }

        default:
        {
            logError("Not implemented command: %d\n", command->type);
            return false;
        }
    }

    return false;
}

// train units
void trainUnit(WarContext* context, WarUnitCommandType commandType, WarUnitType unitToTrain, WarUnitType buildingUnit)
{
    WarMap* map = context->map;

    map->command.type = commandType;
    map->command.train.unitToTrain = unitToTrain;
    map->command.train.buildingUnit = buildingUnit;
}

void trainFootman(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_FOOTMAN, WAR_UNIT_FOOTMAN, WAR_UNIT_BARRACKS_HUMANS);
}

void trainGrunt(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_GRUNT, WAR_UNIT_GRUNT, WAR_UNIT_BARRACKS_ORCS);
}

void trainPeasant(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_PEASANT, WAR_UNIT_PEASANT, WAR_UNIT_TOWNHALL_HUMANS);
}

void trainPeon(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_PEON, WAR_UNIT_PEON, WAR_UNIT_TOWNHALL_ORCS);
}

void trainHumanCatapult(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_CATAPULT_HUMANS, WAR_UNIT_CATAPULT_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void trainOrcCatapult(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_CATAPULT_ORCS, WAR_UNIT_CATAPULT_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void trainKnight(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_KNIGHT, WAR_UNIT_KNIGHT, WAR_UNIT_BARRACKS_HUMANS);
}

void trainRaider(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_RAIDER, WAR_UNIT_KNIGHT, WAR_UNIT_BARRACKS_ORCS);
}

void trainArcher(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_ARCHER, WAR_UNIT_ARCHER, WAR_UNIT_BARRACKS_HUMANS);
}

void trainSpearman(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_SPEARMAN, WAR_UNIT_SPEARMAN, WAR_UNIT_BARRACKS_ORCS);
}

void trainConjurer(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_CONJURER, WAR_UNIT_CONJURER, WAR_UNIT_TOWER_HUMANS);
}

void trainWarlock(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_WARLOCK, WAR_UNIT_WARLOCK, WAR_UNIT_TOWER_ORCS);
}

void trainCleric(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_CLERIC, WAR_UNIT_CLERIC, WAR_UNIT_CHURCH);
}

void trainNecrolyte(WarContext* context, WarEntity* entity)
{
    trainUnit(context, WAR_COMMAND_TRAIN_NECROLYTE, WAR_UNIT_NECROLYTE, WAR_UNIT_TEMPLE);
}

// upgrades
void upgradeUpgrade(WarContext* context, WarUnitCommandType commandType, WarUpgradeType upgradeToBuild, WarUnitType buildingUnit)
{
    WarMap* map = context->map;

    map->command.type = commandType;
    map->command.upgrade.upgradeToBuild = upgradeToBuild;
    map->command.upgrade.buildingUnit = buildingUnit;
}

void upgradeSwords(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SWORDS, WAR_UPGRADE_SWORDS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void upgradeAxes(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_AXES, WAR_UPGRADE_AXES, WAR_UNIT_BLACKSMITH_ORCS);
}

void upgradeHumanShields(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SHIELD_HUMANS, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_HUMANS);
}

void upgradeOrcsShields(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SHIELD_ORCS, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_ORCS);
}

void upgradeArrows(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_ARROWS, WAR_UPGRADE_ARROWS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void upgradeSpears(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SPEARS, WAR_UPGRADE_SPEARS, WAR_UNIT_LUMBERMILL_ORCS);
}

void upgradeHorses(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_HORSES, WAR_UPGRADE_HORSES, WAR_UNIT_STABLE);
}

void upgradeWolves(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_WOLVES, WAR_UPGRADE_WOLVES, WAR_UNIT_KENNEL);
}

void upgradeScorpions(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SCORPION, WAR_UPGRADE_SCORPIONS, WAR_UNIT_TOWER_HUMANS);
}

void upgradeSpiders(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SPIDER, WAR_UPGRADE_SPIDERS, WAR_UNIT_TOWER_ORCS);
}

void upgradeRainOfFire(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_RAIN_OF_FIRE, WAR_UPGRADE_RAIN_OF_FIRE, WAR_UNIT_TOWER_HUMANS);
}

void upgradePoisonCloud(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_POISON_CLOUD, WAR_UPGRADE_POISON_CLOUD, WAR_UNIT_TOWER_ORCS);
}

void upgradeWaterElemental(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_WATER_ELEMENTAL, WAR_UPGRADE_WATER_ELEMENTAL, WAR_UNIT_TOWER_HUMANS);
}

void upgradeDaemon(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_DAEMON, WAR_UPGRADE_DAEMON, WAR_UNIT_TOWER_ORCS);
}

void upgradeHealing(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_HEALING, WAR_UPGRADE_HEALING, WAR_UNIT_CHURCH);
}

void upgradeRaiseDead(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_RAISE_DEAD, WAR_UPGRADE_RAISE_DEAD, WAR_UNIT_TEMPLE);
}

void upgradeFarSight(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_FAR_SIGHT, WAR_UPGRADE_FAR_SIGHT, WAR_UNIT_CHURCH);
}

void upgradeDarkVision(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_DARK_VISION, WAR_UPGRADE_DARK_VISION, WAR_UNIT_TEMPLE);
}

void upgradeInvisibility(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_INVISIBILITY, WAR_UPGRADE_INVISIBILITY, WAR_UNIT_CHURCH);
}

void upgradeUnholyArmor(WarContext* context, WarEntity* entity)
{
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_UNHOLY_ARMOR, WAR_UPGRADE_UNHOLY_ARMOR, WAR_UNIT_TEMPLE);
}

// cancel
void cancel(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    map->command.type = WAR_COMMAND_NONE;

    for (s32 i = 0; i < map->selectedEntities.count; i++)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[i];
        WarEntity* selectedEntity = findEntity(context, selectedEntityId);

        if (isBuildingUnit(selectedEntity))
        {
            if (isBuilding(selectedEntity) || isGoingToBuild(selectedEntity))
            {
                WarBuildingStats stats = getBuildingStats(selectedEntity->unit.type);
                increasePlayerResources(context, player, stats.goldCost, stats.woodCost);

                WarState* collapseState = createCollapseState(context, selectedEntity);
                changeNextState(context, selectedEntity, collapseState, true, true);

                createAudioRandom(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, false);
            }
            else if (selectedEntity->unit.building)
            {
                if (isTraining(selectedEntity) || isGoingToTrain(selectedEntity))
                {
                    WarState* trainState = getTrainState(selectedEntity);
                    WarUnitType unitToBuild = trainState->train.unitToBuild;

                    WarUnitStats stats = getUnitStats(unitToBuild);
                    increasePlayerResources(context, player, stats.goldCost, stats.woodCost);
                }
                else if (isUpgrading(selectedEntity) || isGoingToUpgrade(selectedEntity))
                {
                    WarState* upgradeState = getUpgradeState(selectedEntity);
                    WarUpgradeType upgradeToBuild = upgradeState->upgrade.upgradeToBuild;
                    assert(hasRemainingUpgrade(player, upgradeToBuild));

                    s32 upgradeLevel = getUpgradeLevel(player, upgradeToBuild);
                    WarUpgradeStats stats = getUpgradeStats(upgradeToBuild);
                    increasePlayerResources(context, player, stats.goldCost[upgradeLevel], 0);
                }

                WarState* idleState = createIdleState(context, entity, false);
                changeNextState(context, selectedEntity, idleState, true, true);
            }
        }
    }
}

// basic
void move(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_MOVE;
}

void stop(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_STOP;
}

void harvest(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_HARVEST;
}

void deliver(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_DELIVER;
}

void repair(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_REPAIR;
}

void attack(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_ATTACK;
}

void buildBasic(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_BASIC;
}

void buildAdvanced(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_ADVANCED;
}

void buildBuilding(WarContext* context, WarUnitCommandType commandType, WarUnitType buildingToBuild)
{
    WarMap* map = context->map;

    map->command.type = commandType;
    map->command.build.buildingToBuild = buildingToBuild;
}

void buildFarmHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_FARM_HUMANS, WAR_UNIT_FARM_HUMANS);
}

void buildFarmOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_FARM_ORCS, WAR_UNIT_FARM_ORCS);
}

void buildBarracksHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_BARRACKS_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void buildBarracksOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_BARRACKS_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void buildChurch(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_CHURCH, WAR_UNIT_CHURCH);
}

void buildTemple(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_TEMPLE, WAR_UNIT_TEMPLE);
}

void buildTowerHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_TOWER_HUMANS, WAR_UNIT_TOWER_HUMANS);
}

void buildTowerOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_TOWER_ORCS, WAR_UNIT_TOWER_ORCS);
}

void buildTownHallHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_TOWNHALL_HUMANS, WAR_UNIT_TOWNHALL_HUMANS);
}

void buildTownHallOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_TOWNHALL_ORCS, WAR_UNIT_TOWNHALL_ORCS);
}

void buildLumbermillHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_LUMBERMILL_HUMANS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void buildLumbermillOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_LUMBERMILL_ORCS, WAR_UNIT_LUMBERMILL_ORCS);
}

void buildStable(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_STABLE, WAR_UNIT_STABLE);
}

void buildKennel(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_KENNEL, WAR_UNIT_KENNEL);
}

void buildBlacksmithHumans(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_BLACKSMITH_HUMANS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void buildBlacksmithOrcs(WarContext* context, WarEntity* entity)
{
    buildBuilding(context, WAR_COMMAND_BUILD_BLACKSMITH_ORCS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void buildWall(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_WALL;
}

void buildRoad(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_ROAD;
}

// spells
void castRainOfFire(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_RAIN_OF_FIRE;
}

void castPoisonCloud(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_POISON_CLOUD;
}

void castHeal(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_HEALING;
}

void castFarSight(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_FAR_SIGHT;
}

void castDarkVision(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_DARK_VISION;
}

void castInvisibility(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_INVISIBILITY;
}

void castUnHolyArmor(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_UNHOLY_ARMOR;
}

void castRaiseDead(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_RAISE_DEAD;
}

// summons
void summonSpider(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_SPIDER;
}

void summonScorpion(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_SCORPION;
}

void summonDaemon(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_DAEMON;
}

void summonWaterElemental(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_WATER_ELEMENTAL;
}
