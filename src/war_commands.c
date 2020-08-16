WarCommand* createCommand(WarContext* context, WarPlayerInfo* player, WarCommandType type)
{
    static WarCommandId staticCommandId = 0;

    WarCommand* command = (WarCommand*)xcalloc(1, sizeof(WarCommand));
    command->id = ++staticCommandId;
    command->type = type;
    command->status = WAR_COMMAND_STATUS_NONE;

    WarCommandListAdd(&player->commands, command);

    return command;
}

WarCommand* createTrainCommand(WarContext* context, WarPlayerInfo* player, WarUnitType unitType, WarEntityId buildingId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_TRAIN);
    command->train.unitType = unitType;
    command->train.buildingId = buildingId;
    return command;
}

WarCommand* createBuildCommand(WarContext* context, WarPlayerInfo* player, WarUnitType unitType, WarEntityId workerId, vec2 position)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_BUILD);
    command->build.unitType = unitType;
    command->build.workerId = workerId;
    command->build.position = position;
    return command;
}

WarCommand* createBuildWallCommand(WarContext* context, WarPlayerInfo* player, vec2 position)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_BUILD_WALL);
    command->wall.position = position;
    return command;
}
WarCommand* createBuildRoadCommand(WarContext* context, WarPlayerInfo* player, vec2 position)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_BUILD_ROAD);
    command->road.position = position;
    return command;
}

WarCommand* createUpgradeCommand(WarContext* context, WarPlayerInfo* player, WarUpgradeType upgradeType, WarEntityId buildingId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_UPGRADE);
    command->upgrade.upgradeType = upgradeType;
    command->upgrade.buildingId = buildingId;
    return command;
}

WarCommand* createMoveCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, vec2 position)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_MOVE);
    command->move.squadId = WAR_INVALID_SQUAD;
    command->move.unitGroup = unitGroup;
    command->move.position = position;
    return command;
}

WarCommand* createSquadMoveCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, vec2 position)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_MOVE);
    command->move.squadId = squadId;
    command->move.position = position;
    return command;
}

WarCommand* createFollowCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, WarEntityId targetEntityId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_FOLLOW);
    command->follow.squadId = WAR_INVALID_SQUAD;
    command->follow.unitGroup = unitGroup;
    command->follow.targetEntityId = targetEntityId;
    return command;
}

WarCommand* createSquadFollowCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, WarEntityId targetEntityId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_FOLLOW);
    command->follow.squadId = squadId;
    command->follow.targetEntityId = targetEntityId;
    return command;
}

WarCommand* createAttackEnemyCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, WarEntityId targetEntityId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_ATTACK);
    command->attack.squadId = WAR_INVALID_SQUAD;
    command->attack.unitGroup = unitGroup;
    command->attack.targetEntityId = targetEntityId;
    return command;
}

WarCommand* createAttackPositionCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, vec2 position)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_ATTACK);
    command->attack.squadId = WAR_INVALID_SQUAD;
    command->attack.unitGroup = unitGroup;
    command->attack.position = position;
    return command;
}

WarCommand* createSquadAttackEnemyCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, WarEntityId targetEntityId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_ATTACK);
    command->attack.squadId = squadId;
    command->attack.targetEntityId = targetEntityId;
    return command;
}

WarCommand* createSquadAttackPositionCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, vec2 position)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_ATTACK);
    command->attack.squadId = squadId;
    command->attack.position = position;
    return command;
}

WarCommand* createStopCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_STOP);
    command->stop.squadId = WAR_INVALID_SQUAD;
    command->stop.unitGroup = unitGroup;
    return command;
}

WarCommand* createSquadStopCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_STOP);
    command->stop.squadId = squadId;
    return command;
}

WarCommand* createGatherGoldCommand(
    WarContext* context, WarPlayerInfo* player,
    WarUnitGroup unitGroup, WarEntityId targetEntityId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_GATHER);
    command->gather.unitGroup = unitGroup;
    command->gather.resource = WAR_RESOURCE_GOLD;
    command->gather.targetEntityId = targetEntityId;
    return command;
}

WarCommand* createGatherWoodCommand(
    WarContext* context, WarPlayerInfo* player,
    WarUnitGroup unitGroup, WarEntityId targetEntityId,
    vec2 targetTile)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_GATHER);
    command->gather.unitGroup = unitGroup;
    command->gather.resource = WAR_RESOURCE_WOOD;
    command->gather.targetEntityId = targetEntityId;
    command->gather.targetTile = targetTile;
    return command;
}

WarCommand* createDeliverCommand(
    WarContext* context, WarPlayerInfo* player,
    WarUnitGroup unitGroup, WarEntityId targetEntityId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_DELIVER);
    command->deliver.unitGroup = unitGroup;
    command->deliver.targetEntityId = targetEntityId;
    return command;
}

WarCommand* createRepairCommand(
    WarContext* context, WarPlayerInfo* player,
    WarUnitGroup unitGroup, WarEntityId targetEntityId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_REPAIR);
    command->repair.unitGroup = unitGroup;
    command->repair.targetEntityId = targetEntityId;
    return command;
}

WarCommand* createCastCommand(
    WarContext* context, WarPlayerInfo* player,
    WarUnitGroup unitGroup, WarSpellType spellType,
    WarEntityId targetEntityId, vec2 position)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_CAST);
    command->cast.squadId = WAR_INVALID_SQUAD;
    command->cast.unitGroup = unitGroup;
    command->cast.spellType = spellType;
    command->cast.targetEntityId = targetEntityId;
    command->cast.position = position;
    return command;
}

WarCommand* createSquadCastCommand(
    WarContext* context, WarPlayerInfo* player,
    WarSquadId squadId, WarSpellType spellType,
    WarEntityId targetEntityId, vec2 position)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_CAST);
    command->cast.squadId = squadId;
    command->cast.spellType = spellType;
    command->cast.targetEntityId = targetEntityId;
    command->cast.position = position;
    return command;
}

WarCommand* createSquadCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, WarUnitGroup unitGroup)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_SQUAD);
    command->squad.squadId = squadId;
    command->squad.unitGroup = unitGroup;
    return command;
}

WarCommand* createWaitCommandCommand(WarContext* context, WarPlayerInfo* player, WarCommandId commandId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_WAIT);
    command->wait.commandId = commandId;
    return command;
}

WarCommand* createWaitResourceCommand(WarContext* context, WarPlayerInfo* player, WarResourceKind resource, s32 amount)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_WAIT);
    command->wait.resource = resource;
    command->wait.amount = amount;
    return command;
}

WarCommand* createSleepCommand(WarContext* context, WarPlayerInfo* player, f32 time)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_SLEEP);
    command->sleep.time = time;
    return command;
}

WarCommand* createCancelCommand(WarContext* context, WarPlayerInfo* player, WarEntityId targetEntityId)
{
    WarCommand* command = createCommand(context, player, WAR_COMMAND_CANCEL);
    command->cancel.targetEntityId = targetEntityId;
    return command;
}

WarCommandStatus executeTrainCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarUnitType unitType = command->train.unitType;
    WarEntityId producerId = command->train.buildingId;

    if (!isValidUnitType(unitType) || !isTrainableUnitType(unitType))
    {
        return WAR_COMMAND_STATUS_INVALID_UNIT_TYPE;
    }

    WarRace unitTypeRace = getUnitTypeRace(unitType);
    if (player->race != unitTypeRace)
    {
        return WAR_COMMAND_STATUS_INVALID_UNIT_RACE;
    }

    if (!producerId)
    {
        return WAR_COMMAND_STATUS_INVALID_PRODUCER;
    }

    WarUnitType producerType = getUnitTypeProducer(unitType);
    WarEntity* producer = findEntity(context, producerId);
    if (!producer || !isBuildingUnit(producer) || isCollapsedUnit(producer) ||
        producer->unit.type != producerType)
    {
        return WAR_COMMAND_STATUS_INVALID_PRODUCER;
    }

    if (isBuilding(producer) || isGoingToBuild(producer))
    {
        return WAR_COMMAND_STATUS_PRODUCER_NOT_READY;
    }

    if (isTraining(producer) || isGoingToTrain(producer) ||
        isUpgrading(producer) || isGoingToUpgrade(producer))
    {
        return WAR_COMMAND_STATUS_PRODUCER_BUSY;
    }

    if (!enoughFarmFood(context, player))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_FOOD;
    }

    WarUnitStats stats = getUnitStats(unitType);
    if (!enoughPlayerResource(context, player, WAR_RESOURCE_GOLD, stats.goldCost))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_GOLD;
    }

    if (!enoughPlayerResource(context, player, WAR_RESOURCE_WOOD, stats.woodCost))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_WOOD;
    }

    decreasePlayerResource(context, player, WAR_RESOURCE_GOLD, stats.goldCost);
    decreasePlayerResource(context, player, WAR_RESOURCE_WOOD, stats.woodCost);

    sendToTrainState(context, producer, unitType, stats.buildTime);

    return WAR_COMMAND_STATUS_RUNNING;
}

WarCommandStatus executeBuildCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarUnitType unitType = command->build.unitType;
    WarEntityId workerId = command->build.workerId;
    vec2 targetTile = command->build.position;

    if (!isValidUnitType(unitType) || !isBuildableUnitType(unitType))
    {
        return WAR_COMMAND_STATUS_INVALID_UNIT_TYPE;
    }

    WarRace unitTypeRace = getUnitTypeRace(unitType);
    if (player->race != unitTypeRace)
    {
        return WAR_COMMAND_STATUS_INVALID_UNIT_RACE;
    }

    if (!workerId)
    {
        return WAR_COMMAND_STATUS_INVALID_WORKER;
    }

    WarEntity* worker = findEntity(context, workerId);
    if (!worker || !isWorkerUnit(worker) || isDeadUnit(worker))
    {
        return WAR_COMMAND_STATUS_INVALID_WORKER;
    }

    if (isWorkerBusy(worker))
    {
        return WAR_COMMAND_STATUS_WORKER_BUSY;
    }

    if (!canBuildingBeBuilt(context, unitType, targetTile.x, targetTile.y))
    {
        return WAR_COMMAND_STATUS_INVALID_POSITION;
    }

    WarBuildingStats stats = getBuildingStats(unitType);
    if (!enoughPlayerResource(context, player, WAR_RESOURCE_GOLD, stats.goldCost))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_GOLD;
    }

    if (!enoughPlayerResource(context, player, WAR_RESOURCE_WOOD, stats.woodCost))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_WOOD;
    }

    decreasePlayerResource(context, player, WAR_RESOURCE_GOLD, stats.goldCost);
    decreasePlayerResource(context, player, WAR_RESOURCE_WOOD, stats.woodCost);

    WarEntity* building = createBuilding(context, unitType, targetTile.x, targetTile.y, player->index, true);
    sendToRepairState(context, worker, building->id);

    return WAR_COMMAND_STATUS_RUNNING;
}

WarCommandStatus executeBuildWallCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarMap* map = context->map;

    vec2 targetTile = command->wall.position;

    if (!canRoadOrWallBeBuilt(context, targetTile.x, targetTile.y))
    {
        return WAR_COMMAND_STATUS_INVALID_POSITION;
    }

    if (!enoughPlayerResource(context, player, WAR_RESOURCE_GOLD, WAR_WALL_GOLD_COST))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_GOLD;
    }

    if (!enoughPlayerResource(context, player, WAR_RESOURCE_WOOD, WAR_WALL_WOOD_COST))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_WOOD;
    }

    decreasePlayerResource(context, player, WAR_RESOURCE_GOLD, WAR_WALL_GOLD_COST);
    decreasePlayerResource(context, player, WAR_RESOURCE_WOOD, WAR_WALL_WOOD_COST);

    WarEntity* wall = map->wall;
    WarWallPiece* piece = addWallPiece(wall, targetTile.x, targetTile.y, 0);
    piece->hp = WAR_WALL_MAX_HP;
    piece->maxhp = WAR_WALL_MAX_HP;

    determineWallTypes(context, wall);

    return WAR_COMMAND_STATUS_RUNNING;
}

WarCommandStatus executeBuildRoadCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarMap* map = context->map;

    vec2 targetTile = command->wall.position;

    if (!canRoadOrWallBeBuilt(context, targetTile.x, targetTile.y))
    {
        return WAR_COMMAND_STATUS_INVALID_POSITION;
    }

    if (!enoughPlayerResource(context, player, WAR_RESOURCE_GOLD, WAR_ROAD_GOLD_COST))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_GOLD;
    }

    if (!enoughPlayerResource(context, player, WAR_RESOURCE_WOOD, WAR_ROAD_WOOD_COST))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_WOOD;
    }

    decreasePlayerResource(context, player, WAR_RESOURCE_GOLD, WAR_ROAD_GOLD_COST);
    decreasePlayerResource(context, player, WAR_RESOURCE_WOOD, WAR_ROAD_WOOD_COST);

    WarEntity* road = map->road;
    addRoadPiece(road, targetTile.x, targetTile.y, 0);

    determineRoadTypes(context, road);

    return WAR_COMMAND_STATUS_DONE;
}

WarCommandStatus executeUpgradeCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarUpgradeType upgradeType = command->upgrade.upgradeType;
    WarEntityId producerId = command->upgrade.buildingId;

    if (!isValidUpgradeType(upgradeType))
    {
        return WAR_COMMAND_STATUS_INVALID_UPGRADE_TYPE;
    }

    WarRace upgradeTypeRace = getUpgradeTypeRace(upgradeType);
    if (player->race != upgradeTypeRace && upgradeType != WAR_UPGRADE_SHIELD)
    {
        return WAR_COMMAND_STATUS_INVALID_UPGRADE_RACE;
    }

    if (!producerId)
    {
        return WAR_COMMAND_STATUS_INVALID_PRODUCER;
    }

    WarUnitType producerType = getUpgradeTypeProducer(upgradeType, player->race);
    WarEntity* producer = findEntity(context, producerId);
    if (!producer || !isBuildingUnit(producer) || isCollapsedUnit(producer) ||
        producer->unit.type != producerType)
    {
        return WAR_COMMAND_STATUS_INVALID_PRODUCER;
    }

    if (isBuilding(producer) || isGoingToBuild(producer))
    {
        return WAR_COMMAND_STATUS_PRODUCER_NOT_READY;
    }

    if (isTraining(producer) || isGoingToTrain(producer) ||
        isUpgrading(producer) || isGoingToUpgrade(producer))
    {
        return WAR_COMMAND_STATUS_PRODUCER_BUSY;
    }

    if (!hasRemainingUpgrade(player, upgradeType))
    {
        return WAR_COMMAND_STATUS_NOT_MORE_UPGRADE;
    }

    s32 upgradeLevel = getUpgradeLevel(player, upgradeType);
    WarUpgradeStats stats = getUpgradeStats(upgradeType);
    if (!enoughPlayerResource(context, player, WAR_RESOURCE_GOLD, stats.goldCost[upgradeLevel]))
    {
        return WAR_COMMAND_STATUS_NOT_ENOUGH_GOLD;
    }

    decreasePlayerResource(context, player, WAR_RESOURCE_GOLD, stats.goldCost[upgradeLevel]);

    sendToUpgradeState(context, producer, upgradeType, stats.buildTime);

    return WAR_COMMAND_STATUS_RUNNING;
}

WarCommandStatus executeMoveCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarInput* input = &context->input;

    WarSquadId squadId = command->move.squadId;
    WarUnitGroup unitGroup = command->move.unitGroup;
    vec2 targetTile = command->move.position;

    if (inRange(squadId, 0, MAX_SQUAD_COUNT))
    {
        unitGroup = createUnitGroupFromSquad(&player->squads[squadId]);
    }

    if (unitGroup.count <= 0)
    {
        return WAR_COMMAND_STATUS_NO_UNITS;
    }

    if (unitGroup.count > MAX_UNIT_SELECTION_COUNT)
    {
        return WAR_COMMAND_STATUS_TOO_MANY_UNITS;
    }

    if (vec2IsZero(targetTile))
    {
        return WAR_COMMAND_STATUS_INVALID_MOVE_TARGET;
    }

    WarCommandStatus status = WAR_COMMAND_STATUS_FAILED;

    for (s32 i = 0; i < unitGroup.count; i++)
    {
        WarEntityId unitId = unitGroup.unitIds[i];
        WarEntity* unit = findEntity(context, unitId);

        if (!unit || !isDudeUnit(unit) || isDeadUnit(unit))
            continue;

        if (!isFriendlyUnit(context, unit))
            continue;

        vec2 position = getUnitCenterPosition(unit, true);

        if (isKeyPressed(input, WAR_KEY_SHIFT))
        {
            if (isPatrolling(unit))
            {
                if(isMoving(unit))
                {
                    WarState* moveState = getMoveState(unit);
                    vec2ListAdd(&moveState->move.positions, targetTile);
                }

                WarState* patrolState = getPatrolState(unit);
                vec2ListAdd(&patrolState->patrol.positions, targetTile);
            }
            else if(isMoving(unit) && !isAttacking(unit))
            {
                WarState* moveState = getMoveState(unit);
                vec2ListAdd(&moveState->move.positions, targetTile);
            }
            else
            {
                sendToMoveState(context, unit, 2, arrayArg(vec2, position, targetTile));
            }
        }
        else if (isKeyPressed(input, WAR_KEY_CTRL))
        {
            if (isPatrolling(unit))
            {
                if(isMoving(unit))
                {
                    WarState* moveState = getMoveState(unit);
                    vec2ListAdd(&moveState->move.positions, targetTile);
                }

                WarState* patrolState = getPatrolState(unit);
                vec2ListAdd(&patrolState->patrol.positions, targetTile);
            }
            else if(isMoving(unit) && !isAttacking(unit))
            {
                WarState* moveState = getMoveState(unit);
                vec2ListAdd(&moveState->move.positions, targetTile);
            }
            else
            {
                sendToPatrolState(context, unit, 2, arrayArg(vec2, position, targetTile));
            }
        }
        else
        {
            sendToMoveState(context, unit, 2, arrayArg(vec2, position, targetTile));
        }

        status = WAR_COMMAND_STATUS_DONE;
    }

    return status;
}

WarCommandStatus executeFollowCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarSquadId squadId = command->follow.squadId;
    WarUnitGroup unitGroup = command->follow.unitGroup;
    WarEntityId targetEntityId = command->follow.targetEntityId;

    if (inRange(squadId, 0, MAX_SQUAD_COUNT))
    {
        unitGroup = createUnitGroupFromSquad(&player->squads[squadId]);
    }

    if (unitGroup.count <= 0)
    {
        return WAR_COMMAND_STATUS_NO_UNITS;
    }

    if (unitGroup.count > MAX_UNIT_SELECTION_COUNT)
    {
        return WAR_COMMAND_STATUS_TOO_MANY_UNITS;
    }

    if (!targetEntityId)
    {
        return WAR_COMMAND_STATUS_INVALID_FOLLOW_TARGET;
    }

    WarEntity* targetEntity = findEntity(context, targetEntityId);
    if (!targetEntity)
    {
        return WAR_COMMAND_STATUS_INVALID_FOLLOW_TARGET;
    }

    WarCommandStatus status = WAR_COMMAND_STATUS_FAILED;

    for (s32 i = 0; i < unitGroup.count; i++)
    {
        WarEntityId unitId = unitGroup.unitIds[i];
        WarEntity* unit = findEntity(context, unitId);

        if (!unit || !isDudeUnit(unit) || isDeadUnit(unit))
            continue;

        if (!isFriendlyUnit(context, unit))
            continue;

        sendToFollowState(context, unit, targetEntity->id, VEC2_ZERO, 1);

        status = WAR_COMMAND_STATUS_DONE;
    }

    return status;
}

WarCommandStatus executeAttackCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarSquadId squadId = command->attack.squadId;
    WarUnitGroup unitGroup = command->attack.unitGroup;
    WarEntityId targetEntityId = command->attack.targetEntityId;
    vec2 targetTile = command->attack.position;

    WarEntity* targetEntity = NULL;

    if (inRange(squadId, 0, MAX_SQUAD_COUNT))
    {
        unitGroup = createUnitGroupFromSquad(&player->squads[squadId]);
    }

    if (unitGroup.count <= 0)
    {
        return WAR_COMMAND_STATUS_NO_UNITS;
    }

    if (unitGroup.count > MAX_UNIT_SELECTION_COUNT)
    {
        return WAR_COMMAND_STATUS_TOO_MANY_UNITS;
    }

    if (!targetEntityId && vec2IsZero(targetTile))
    {
        return WAR_COMMAND_STATUS_INVALID_ATTACK_TARGET;
    }

    if (targetEntityId)
    {
        targetEntity = findEntity(context, targetEntityId);
        if (!targetEntity || !isUnit(targetEntity) || isDeadUnit(targetEntity) || isCollapsedUnit(targetEntity))
        {
            return WAR_COMMAND_STATUS_INVALID_ATTACK_TARGET;
        }
    }

    WarCommandStatus status = WAR_COMMAND_STATUS_FAILED;

    for (s32 i = 0; i < unitGroup.count; i++)
    {
        WarEntityId unitId = unitGroup.unitIds[i];
        WarEntity* unit = findEntity(context, unitId);
        if (unit && isDudeUnit(unit) && !isDeadUnit(unit))
        {
            if (isFriendlyUnit(context, unit))
            {
                if (targetEntity)
                {
                    // the unit can't attack itself
                    if (unit->id != targetEntity->id)
                    {
                        if (canAttack(context, unit, targetEntity))
                        {
                            sendToAttackState(context, unit, targetEntity->id, targetTile);

                            status = WAR_COMMAND_STATUS_DONE;
                        }
                    }
                }
                else
                {
                    sendToAttackState(context, unit, 0, targetTile);

                    status = WAR_COMMAND_STATUS_DONE;
                }
            }

        }
    }

    return status;
}

WarCommandStatus executeStopCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarSquadId squadId = command->stop.squadId;
    WarUnitGroup unitGroup = command->stop.unitGroup;

    if (inRange(squadId, 0, MAX_SQUAD_COUNT))
    {
        unitGroup = createUnitGroupFromSquad(&player->squads[squadId]);
    }

    if (unitGroup.count <= 0)
    {
        return WAR_COMMAND_STATUS_NO_UNITS;
    }

    if (unitGroup.count > MAX_UNIT_SELECTION_COUNT)
    {
        return WAR_COMMAND_STATUS_TOO_MANY_UNITS;
    }

    WarCommandStatus status = WAR_COMMAND_STATUS_FAILED;

    for (s32 i = 0; i < unitGroup.count; i++)
    {
        WarEntityId unitId = unitGroup.unitIds[i];
        WarEntity* unit = findEntity(context, unitId);

        if (unit && isDudeUnit(unit) && isFriendlyUnit(context, unit) && !isDeadUnit(unit))
        {
            sendToIdleState(context, unit, true);

            status = WAR_COMMAND_STATUS_DONE;
        }
    }

    return status;
}

WarCommandStatus executeGatherCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarUnitGroup unitGroup = command->gather.unitGroup;
    WarResourceKind resource = command->gather.resource;
    WarEntityId targetEntityId = command->gather.targetEntityId;
    vec2 targetTile = command->gather.targetTile;

    if (unitGroup.count <= 0)
    {
        return WAR_COMMAND_STATUS_NO_UNITS;
    }

    if (unitGroup.count > MAX_UNIT_SELECTION_COUNT)
    {
        return WAR_COMMAND_STATUS_TOO_MANY_UNITS;
    }

    switch (resource)
    {
        case WAR_RESOURCE_GOLD:
        {
            if (!targetEntityId)
            {
                return WAR_COMMAND_STATUS_INVALID_GOLDMINE;
            }

            WarEntity* goldmine = findEntity(context, targetEntityId);
            if (!goldmine || !isGoldmineUnit(goldmine) || isCollapsedUnit(goldmine))
            {
                return WAR_COMMAND_STATUS_INVALID_GOLDMINE;
            }

            WarCommandStatus status = WAR_COMMAND_STATUS_FAILED;

            for (s32 i = 0; i < unitGroup.count; i++)
            {
                WarEntityId unitId = unitGroup.unitIds[i];
                WarEntity* unit = findEntity(context, unitId);
                if (unit && isWorkerUnit(unit) && isFriendlyUnit(context, unit) && !isWorkerBusy(unit))
                {
                    if (sendWorkerToMine(context, unit, goldmine, NULL))
                    {
                        status = WAR_COMMAND_STATUS_DONE;
                    }
                }
            }

            return status;
        }
        case WAR_RESOURCE_WOOD:
        {
            WarEntity* forest = findEntity(context, targetEntityId);
            if (!forest)
            {
                return WAR_COMMAND_STATUS_INVALID_FOREST;
            }

            WarTree* tree = getTreeAtPosition(forest, (s32)targetTile.x, (s32)targetTile.y);
            if (!tree)
            {
                return WAR_COMMAND_STATUS_INVALID_TREE;
            }

            WarCommandStatus status = WAR_COMMAND_STATUS_FAILED;

            for (s32 i = 0; i < unitGroup.count; i++)
            {
                WarEntityId unitId = unitGroup.unitIds[i];
                WarEntity* unit = findEntity(context, unitId);
                if (unit && isWorkerUnit(unit) && isFriendlyUnit(context, unit) && !isWorkerBusy(unit))
                {
                    if (sendWorkerToChop(context, unit, forest, targetTile, NULL))
                    {
                        status = WAR_COMMAND_STATUS_DONE;
                    }
                }
            }

            return status;
        }
        default:
        {
            logWarning("Trying to execute gather command with resource: %d\n", resource);
            return WAR_COMMAND_STATUS_INVALID_RESOURCE;
        }
    }
}

WarCommandStatus executeDeliverCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarUnitGroup unitGroup = command->deliver.unitGroup;
    WarEntityId targetEntityId = command->deliver.targetEntityId;

    if (unitGroup.count <= 0)
    {
        return WAR_COMMAND_STATUS_NO_UNITS;
    }

    if (unitGroup.count > MAX_UNIT_SELECTION_COUNT)
    {
        return WAR_COMMAND_STATUS_TOO_MANY_UNITS;
    }

    WarEntity* targetEntity = findEntity(context, targetEntityId);

    WarCommandStatus status = WAR_COMMAND_STATUS_FAILED;

    for (s32 i = 0; i < unitGroup.count; i++)
    {
        WarEntityId unitId = unitGroup.unitIds[i];
        WarEntity* unit = findEntity(context, unitId);
        if (unit && isWorkerUnit(unit) && isFriendlyUnit(context, unit) && isCarryingResources(unit))
        {
            WarEntity* townHall = targetEntity;
            if (!townHall)
            {
                WarRace race = getUnitRace(unit);
                WarUnitType townHallType = getTownHallOfRace(race);
                townHall = findClosestUnitOfType(context, unit, townHallType);
                if (!townHall)
                    continue;
            }

            sendToDeliverState(context, unit, townHall->id, NULL);

            status = WAR_COMMAND_STATUS_DONE;
        }
    }

    return status;
}

WarCommandStatus executeRepairCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarUnitGroup unitGroup = command->repair.unitGroup;
    WarEntityId targetEntityId = command->repair.targetEntityId;

    if (unitGroup.count <= 0)
    {
        return WAR_COMMAND_STATUS_NO_UNITS;
    }

    if (unitGroup.count > MAX_UNIT_SELECTION_COUNT)
    {
        return WAR_COMMAND_STATUS_TOO_MANY_UNITS;
    }

    WarEntity* targetEntity = findEntity(context, targetEntityId);
    if (!isBuildingUnit(targetEntity))
    {
        return WAR_COMMAND_STATUS_INVALID_TARGET;
    }

    WarCommandStatus status = WAR_COMMAND_STATUS_FAILED;

    for (s32 i = 0; i < unitGroup.count; i++)
    {
        WarEntityId unitId = unitGroup.unitIds[i];
        WarEntity* unit = findEntity(context, unitId);
        if (unit && isWorkerUnit(unit) && isFriendlyUnit(context, unit))
        {
            sendToRepairState(context, unit, targetEntity->id);

            status = WAR_COMMAND_STATUS_DONE;
        }
    }

    return status;
}

WarCommandStatus executeCastCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarSquadId squadId = command->cast.squadId;
    WarUnitGroup unitGroup = command->cast.unitGroup;
    WarSpellType spellType = command->cast.spellType;
    WarEntityId targetEntityId = command->cast.targetEntityId;
    vec2 position = command->cast.position;

    if (inRange(squadId, 0, MAX_SQUAD_COUNT))
    {
        unitGroup = createUnitGroupFromSquad(&player->squads[squadId]);
    }

    if (unitGroup.count <= 0)
    {
        return WAR_COMMAND_STATUS_NO_UNITS;
    }

    if (unitGroup.count > MAX_UNIT_SELECTION_COUNT)
    {
        return WAR_COMMAND_STATUS_TOO_MANY_UNITS;
    }

    if (targetEntityId)
    {
        WarEntity* targetEntity = findEntity(context, targetEntityId);
        if (!targetEntity || isDeadUnit(targetEntity) || isCollapsedUnit(targetEntity))
        {
            return WAR_COMMAND_STATUS_INVALID_ATTACK_TARGET;
        }
    }

    WarUnitType casterType = getSpellCasterType(spellType);

    WarCommandStatus status = WAR_COMMAND_STATUS_FAILED;

    for (s32 i = 0; i < unitGroup.count; i++)
    {
        WarEntityId unitId = unitGroup.unitIds[i];
        WarEntity* unit = findEntity(context, unitId);
        if (unit && isMagicUnit(unit) && isFriendlyUnit(context, unit) &&
            isUnitOfType(unit, casterType) && !isDeadUnit(unit))
        {
            sendToCastState(context, unit, spellType, targetEntityId, position);

            status = WAR_COMMAND_STATUS_DONE;
        }
    }

    return status;
}

WarCommandStatus executeSquadCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarSquadId squadId = command->squad.squadId;
    WarUnitGroup unitGroup = command->repair.unitGroup;

    if (!inRange(squadId, 0, MAX_SQUAD_COUNT))
    {
        return WAR_COMMAND_STATUS_INVALID_SQUAD;
    }

    if (unitGroup.count > MAX_UNIT_SELECTION_COUNT)
    {
        return WAR_COMMAND_STATUS_TOO_MANY_UNITS;
    }

    player->squads[squadId] = createSquadFromUnitGroup(squadId, unitGroup);

    return WAR_COMMAND_STATUS_DONE;
}

WarCommandStatus executeWaitCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    return WAR_COMMAND_STATUS_RUNNING;
}

WarCommandStatus executeSleepCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarAI* ai = player->ai;
    assert(ai);

    ai->sleeping = true;
    ai->sleepTime = command->sleep.time;

    return WAR_COMMAND_STATUS_DONE;
}

WarCommandStatus executeCancelCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    WarEntityId targetEntityId = command->cancel.targetEntityId;
    if (!targetEntityId)
    {
        return WAR_COMMAND_STATUS_INVALID_CANCEL_TARGET;
    }

    WarEntity* targetEntity = findEntity(context, targetEntityId);
    if (!targetEntity || !isBuildingUnit(targetEntity))
    {
        return WAR_COMMAND_STATUS_INVALID_CANCEL_TARGET;
    }

    if (isBuilding(targetEntity) || isGoingToBuild(targetEntity))
    {
        WarBuildingStats stats = getBuildingStats(targetEntity->unit.type);

        increasePlayerResource(context, player, WAR_RESOURCE_GOLD, stats.goldCost);
        increasePlayerResource(context, player, WAR_RESOURCE_WOOD, stats.woodCost);

        sendToCollapseState(context, targetEntity);
    }

    if (isTraining(targetEntity) || isGoingToTrain(targetEntity))
    {
        WarState* trainState = getTrainState(targetEntity);
        WarUnitType unitToBuild = trainState->train.unitToBuild;

        WarUnitStats stats = getUnitStats(unitToBuild);

        increasePlayerResource(context, player, WAR_RESOURCE_GOLD, stats.goldCost);
        increasePlayerResource(context, player, WAR_RESOURCE_WOOD, stats.woodCost);

        sendToIdleState(context, targetEntity, false);
    }

    if (isUpgrading(targetEntity) || isGoingToUpgrade(targetEntity))
    {
        WarState* upgradeState = getUpgradeState(targetEntity);
        WarUpgradeType upgradeToBuild = upgradeState->upgrade.upgradeToBuild;
        assert(hasRemainingUpgrade(player, upgradeToBuild));

        s32 upgradeLevel = getUpgradeLevel(player, upgradeToBuild);
        WarUpgradeStats stats = getUpgradeStats(upgradeToBuild);

        increasePlayerResource(context, player, WAR_RESOURCE_GOLD, stats.goldCost[upgradeLevel]);

        sendToIdleState(context, targetEntity, false);
    }

    return WAR_COMMAND_STATUS_DONE;
}

WarCommandStatus executeCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command)
{
    static WarExecuteFunc executeFuncs[WAR_COMMAND_COUNT] =
    {
        NULL,                       // WAR_COMMAND_NONE,
        executeTrainCommand,        // WAR_COMMAND_TRAIN,
        executeBuildCommand,        // WAR_COMMAND_BUILD,
        executeBuildWallCommand,    // WAR_COMMAND_BUILD_WALL,
        executeBuildRoadCommand,    // WAR_COMMAND_BUILD_ROAD,
        executeUpgradeCommand,      // WAR_COMMAND_UPGRADE,
        executeMoveCommand,         // WAR_COMMAND_MOVE,
        executeFollowCommand,       // WAR_COMMAND_FOLLOW,
        executeAttackCommand,       // WAR_COMMAND_ATTACK,
        executeStopCommand,         // WAR_COMMAND_STOP
        executeGatherCommand,       // WAR_COMMAND_GATHER,
        executeDeliverCommand,      // WAR_COMMAND_DELIVER,
        executeRepairCommand,       // WAR_COMMAND_REPAIR,
        executeCastCommand,         // WAR_COMMAND_CAST,
        executeSquadCommand,        // WAR_COMMAND_SQUAD,
        executeWaitCommand,         // WAR_COMMAND_WAIT,
        executeSleepCommand,        // WAR_COMMAND_SLEEP,
        executeCancelCommand,       // WAR_COMMAND_CANCEL
    };

    WarExecuteFunc executeFunc = executeFuncs[(s32)command->type];
    if (!executeFunc)
    {
        logError("Commands of type %d can't be executed\n", command->type);
        return WAR_COMMAND_STATUS_FAILED;
    }

    command->status = executeFunc(context, player, command);
    return command->status;
}
