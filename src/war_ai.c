const WarAIData aiData[] =
{
    { "level", levelInitAI, levelGetAICommand }
};

WarAIData getAIData(const char* name)
{
    s32 index = 0;
    s32 length = arrayLength(aiData);
    while (index < length && !strEquals(aiData[index].name, name))
        index++;

    assert(index < length);
    return aiData[index];
}

WarAI* createAI(WarContext* context, const char* name)
{
    WarAI* ai = (WarAI*)xmalloc(sizeof(WarAI));
    memset(ai->name, 0, sizeof(ai->name));
    strcpy(ai->name, name);

    WarAIData aiData = getAIData(name);
    ai->initFunc = aiData.initFunc;
    ai->getCommandFunc = aiData.getCommandFunc;
    ai->customData = NULL;

    return ai;
}

void initAIPlayer(WarContext* context, WarPlayerInfo* aiPlayer)
{
    aiPlayer->ai = createAI(context, "level");

    if (aiPlayer->ai->initFunc)
    {
        aiPlayer->ai->initFunc(context, aiPlayer);
    }
}

void initAIPlayers(WarContext* context)
{
    WarMap* map = context->map;

    for (s32 i = 1; i < map->playersCount; i++)
    {
        initAIPlayer(context, &map->players[i]);
    }
}

// static s32 idleWorkersCompare(const WarEntity* e1, const WarEntity* e2)
// {
//     assert(isUnit(e1));
//     assert(isUnit(e2));

//     s32 idle1 = isIdle((WarEntity*)e1);
//     s32 idle2 = isIdle((WarEntity*)e2);

//     // NOTE: should this consider distance, because if a idle worker is to far
//     // maybe the AI could prefer to change one busy worker over a far idle one
//     return idle2 - idle1;
// }

// static bool findPlaceToBuild(WarContext* context, WarUnitType unitType, vec2 startPosition, vec2* finalPosition)
// {
//     const s32 dirC = 8;
//     const s32 dirX[] = { -1,  0,  1, 1, 1, 0, -1, -1 };
//     const s32 dirY[] = { -1, -1, -1, 0, 1, 1,  1,  0 };

//     bool result = false;

//     vec2List positions;
//     vec2ListInit(&positions, vec2ListDefaultOptions);

//     vec2ListAdd(&positions, startPosition);

//     for (s32 i = 0; i < positions.count; i++)
//     {
//         vec2 position = positions.items[i];

//         if (canBuildingBeBuilt(context, unitType, position.x, position.y))
//         {
//             *finalPosition = position;
//             result = true;
//             break;
//         }

//         for (s32 d = 0; d < dirC; d++)
//         {
//             vec2 newPosition = vec2Addv(position, vec2i(dirX[d], dirY[d]));

//             // NOTE: this can be optimized if we don't test for containment in the list
//             // all the time, maybe having a 64x64 array with bools could be faster since
//             // I only need to access the corresponding tile? or should I use a hash set?

//             if (inRange(newPosition.x, 0, MAP_TILES_WIDTH) &&
//                 inRange(newPosition.y, 0, MAP_TILES_HEIGHT) &&
//                 !vec2ListContains(&positions, newPosition))
//             {
//                 vec2ListAdd(&positions, newPosition);
//             }
//         }
//     }

//     vec2ListFree(&positions);

//     return result;
// }

// static bool findMineForWorker(WarContext* context, WarEntity* worker)
// {
//     WarRace workerRace = getUnitRace(worker);
//     WarEntityType townHallType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, workerRace);
//     WarEntity* townHall = findClosestUnitOfType(context, worker, townHallType);
//     assert(townHall);

//     WarEntity* goldmine = findClosestUnitOfType(context, townHall, WAR_UNIT_GOLDMINE);
//     return goldmine && sendWorkerToMine(context, worker, goldmine, townHall);
// }

// static bool findForestForWorker(WarContext* context, WarEntity* worker)
// {
//     WarRace workerRace = getUnitRace(worker);
//     WarEntityType townHallType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, workerRace);
//     WarEntity* townHall = findClosestUnitOfType(context, worker, townHallType);
//     assert(townHall);

//     vec2 townHallPosition = getUnitCenterPosition(townHall, true);

//     WarEntity* forest = findClosestForest(context, townHall);
//     if (forest)
//     {
//         WarTree* tree = findAccesibleTree(context, forest, townHallPosition);
//         if (tree)
//         {
//             vec2 treeTile = vec2i(tree->tilex, tree->tiley);
//             return sendWorkerToChop(context, worker, forest, treeTile, townHall);
//         }
//     }

//     return false;
// }


// WarCommandStatus executeTrainAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
// {
//     WarUnitType unitType = command->train.unitType;
//     WarEntityId producerId = command->train.buildingId;

//     if (!isValidUnitType(unitType))
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_UNIT_TYPE ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (!producerId)
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_PRODUCER ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     WarUnitType producerType = getUnitTypeProducer(unitType);
//     WarEntity* producer = findEntity(context, producerId);
//     if (!producer || isCollapsing(producer) || producer->unit.type != producerType)
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_PRODUCER ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (isBuilding(producer) || isGoingToBuild(producer))
//     {
//         // WAR_AI_COMMAND_STATUS_NOT_READY ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (isTraining(producer) || isGoingToTrain(producer) ||
//         isUpgrading(producer) || isGoingToUpgrade(producer))
//     {
//         // WAR_AI_COMMAND_STATUS_TRAINING_UPGRADING ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     WarBuildingStats stats = getBuildingStats(unitType);
//     if (!enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost) ||
//         !enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_WOOD, stats.woodCost))
//     {
//         // WAR_AI_COMMAND_RESULT_NOT_ENOUGH_RESOURCE ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost);
//     decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_WOOD, stats.woodCost);

//     WarState* trainState = createTrainState(context, building, unitType, stats.buildTime);
//     changeNextState(context, building, trainState, true, true);

//     return WAR_AI_COMMAND_STATUS_RUNNING;
// }

// WarCommandStatus executeBuildAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
// {
//     WarMap* map = context->map;
//     assert(map);

//     WarUnitType unitType = command->build.unitType;
//     WarEntityId workerId = command->build.workerId;
//     vec2 targetTile = command->build.position;

//     if (!isValidUnitType(unitType))
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_UNIT_TYPE ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (!workerId)
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_WORKER ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     WarEntity* worker = findEntity(context, workerId);
//     if (!worker || isDead(worker) || isGoingToDie(worker))
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_WORKER ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (isInsideBuilding(worker))
//     {
//         // WAR_AI_COMMAND_STATUS_WORKER_INSIDE ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (isRepairing(worker))
//     {
//         // WAR_AI_COMMAND_STATUS_WORKER_BUILDING ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (!checkTileToBuild(context, unitType, targetTile.x, targetTile.y))
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_POSITION ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     WarUnitStats stats = getUnitStats(unitType);
//     if (!enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost) ||
//         !enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_WOOD, stats.woodCost))
//     {
//         // WAR_AI_COMMAND_RESULT_NOT_ENOUGH_RESOURCE ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost);
//     decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_WOOD, stats.woodCost);

//     WarEntity* building = createBuilding(context, unitType, targetTile.x, targetTile.y, aiPlayer->index, true);
//     WarState* repairState = createRepairState(context, worker, building->id);
//     changeNextState(context, worker, repairState, true, true);

//     return WAR_AI_COMMAND_STATUS_RUNNING;
// }

// WarCommandStatus executeUpgradeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
// {
//     WarUpgradeType upgradeType = command->upgrade.upgradeType;
//     WarEntityId producerId = command->upgrade.buildingId;

//     if (!isValidUnitType(unitType))
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_UNIT_TYPE ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (!producerId)
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_PRODUCER ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     WarUnitType producerType = getUpgradeTypeProducer(upgradeType, aiPlayer->race);
//     WarEntity* producer = findEntity(context, producerId);
//     if (!producer || isCollapsing(producer) || producer->unit.type != producerType)
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_PRODUCER ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (isBuilding(producer) || isGoingToBuild(producer))
//     {
//         // WAR_AI_COMMAND_STATUS_NOT_READY ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (isTraining(producer) || isGoingToTrain(producer) ||
//         isUpgrading(producer) || isGoingToUpgrade(producer))
//     {
//         // WAR_AI_COMMAND_STATUS_TRAINING_UPGRADING ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (!hasRemainingUpgrade(aiPlayer, upgradeType))
//     {
//         // WAR_AI_COMMAND_RESULT_NOT_MORE_UPGRADE ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     s32 upgradeLevel = getUpgradeLevel(aiPlayer, upgradeType);
//     WarUpgradeStats stats = getUpgradeStats(upgradeType);
//     if (!enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost[upgradeLevel]))
//     {
//         // WAR_AI_COMMAND_RESULT_NOT_ENOUGH_RESOURCE ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost[upgradeLevel]);

//     WarState* upgradeState = createUpgradeState(context, producer, upgradeType, stats.buildTime);
//     changeNextState(context, producer, upgradeState, true, true);

//     return WAR_AI_COMMAND_STATUS_RUNNING;
// }

// WarCommandStatus executeMoveAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
// {
//     s32 count = command->attack.count;
//     WarEntityId* unitIds = command->attack.unitIds;
//     vec2 targetTile = command->attack.position;

//     WarSquadId squadId = command->attack.squadId;
//     if (inRange(squadId, 0, MAX_SQUAD_COUNT))
//     {
//         WarSquad* squad = &aiPlayer->squads[squadId];
//         count = squad->count;
//         unitIds = squad->units;
//     }

//     if (!inRange(count, 1, MAX_UNIT_SELECTION_COUNT))
//     {
//         // WAR_AI_COMMAND_STATUS_NO_UNITS ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (vec2IsZero(targetTile))
//     {
//         // WAR_AI_COMMAND_STATUS_NO_MOVE_TARGET ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     // WAR_AI_COMMAND_STATUS_NO_SQUAD_UNIT ?
//     WarCommandStatus status = WAR_AI_COMMAND_STATUS_FAILED;

//     for (s32 i = 0; i < count; i++)
//     {
//         WarEntityId unitId = unitIds[i];
//         WarEntity* unit = findEntity(context, unitId);
//         if (unit && !isDead(unit) && !isGoingToDie(unit))
//         {
//             WarState* moveState = createMoveState(context, unit, 1, arrayArg(vec2, targetTile));
//             changeNextState(context, unit, moveState, true, true);

//             // mark it as done if at least one unit of the squad went to the attack
//             status = WAR_AI_COMMAND_STATUS_DONE;
//         }
//     }

//     return status;
// }

// WarCommandStatus executeAttackAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
// {
//     s32 count = command->attack.count;
//     WarEntityId* unitIds = command->attack.unitIds;
//     WarEntityId enemyId = command->attack.enemyId;
//     vec2 targetTile = command->attack.position;

//     WarSquadId squadId = command->attack.squadId;
//     if (inRange(squadId, 0, MAX_SQUAD_COUNT))
//     {
//         WarSquad* squad = &aiPlayer->squads[squadId];
//         count = squad->count;
//         unitIds = squad->units;
//     }

//     if (!inRange(count, 1, MAX_UNIT_SELECTION_COUNT))
//     {
//         // WAR_AI_COMMAND_STATUS_NO_UNITS ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (!enemyId && vec2IsZero(targetTile))
//     {
//         // WAR_AI_COMMAND_STATUS_NO_ATTACK_TARGET ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (enemyId)
//     {
//         WarEntity* enemy = findEntity(context, enemyId);
//         if (!enemy || isDead(enemy) || isGoingToDie(enemy) || isCollapsing(enemy))
//         {
//             // WAR_AI_COMMAND_STATUS_NO_ATTACK_TARGET ?
//             return WAR_AI_COMMAND_STATUS_FAILED;
//         }
//     }

//     // WAR_AI_COMMAND_STATUS_NO_SQUAD_UNIT ?
//     WarCommandStatus status = WAR_AI_COMMAND_STATUS_FAILED;

//     for (s32 i = 0; i < count; i++)
//     {
//         WarEntityId unitId = unitIds[i];
//         WarEntity* unit = findEntity(context, unitId);
//         if (unit && !isDead(unit) && !isGoingToDie(unit))
//         {
//             WarState* attackState = createAttackState(context, unit, enemyId, targetTile);
//             changeNextState(context, unit, attackState, true, true);

//             // mark it as done if at least one unit of the squad went to the attack
//             status = WAR_AI_COMMAND_STATUS_DONE;
//         }
//     }

//     return status;
// }

// WarCommandStatus executeGatherAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
// {
//     s32 count = command->gather.count;
//     WarEntityId* unitIds = command->gather.unitIds;
//     WarResourceKind resource = command->gather.resource;
//     WarEntityId goldmineId = command->gather.goldmineId;
//     vec2 treePosition = command->gather.treePosition;

//     if (resource != WAR_RESOURCE_GOLD && resource != WAR_RESOURCE_WOOD)
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_RESOURCE ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (!inRange(count, 0, MAX_UNIT_SELECTION_COUNT))
//     {
//         // WAR_AI_COMMAND_STATUS_NO_UNITS ?
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (resource == WAR_RESOURCE_GOLD)
//     {
//         if (!goldmineId)
//         {
//             // WAR_AI_COMMAND_STATUS_INVALID_GOLDMINE ?
//             return WAR_AI_COMMAND_STATUS_FAILED;
//         }

//         WarEntity* goldmine = findEntity(context, goldmineId);
//         if (!goldmine || isCollapsing(goldmine))
//         {
//             // WAR_AI_COMMAND_STATUS_INVALID_GOLDMINE ?
//             return WAR_AI_COMMAND_STATUS_FAILED;
//         }

//         WarCommandStatus status = WAR_AI_COMMAND_STATUS_FAILED;

//         for (s32 i = 0; i < count; i++)
//         {
//             WarEntityId unitId = unitIds[i];
//             WarEntity* worker = findEntity(context, unitId);
//             if (worker && isRepairing(worker) && !isInsideBuilding(worker))
//             {
//                 if (sendWorkerToMine(context, worker, goldmine, NULL))
//                 {
//                     status = WAR_AI_COMMAND_STATUS_DONE;
//                 }
//             }
//         }

//         return status;
//     }
//     else
//     {
//         WarForest* forest = findForest(context, treePosition);
//         if (!forest)
//         {
//             // WAR_AI_COMMAND_STATUS_INVALID_FOREST ?
//             return WAR_AI_COMMAND_STATUS_FAILED;
//         }

//         WarTree* tree = findTree(context, forest, treePosition):
//         if (!tree)
//         {
//             // WAR_AI_COMMAND_STATUS_INVALID_TREE ?
//             return WAR_AI_COMMAND_STATUS_FAILED;
//         }

//         vec2 treeTile = vec2i(tree->tilex, tree->tiley);

//         for (s32 i = 0; i < count; i++)
//         {
//             WarEntityId unitId = unitIds[i];
//             WarEntity* worker = findEntity(context, unitId);
//             if (worker && isRepairing(worker) && !isInsideBuilding(worker))
//             {
//                 if (sendWorkerToChop(context, worker, forest, treeTile, NULL))
//                 {
//                     status = WAR_AI_COMMAND_STATUS_DONE;
//                 }
//             }
//         }

//         return status;
//     }
// }

// WarCommandStatus executeSquadAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
// {
//     WarSquadId squadId = command->squad.squadId;
//     s32 count = command->squad.count;
//     WarEntityId* unitIds = command->squad.unitIds;

//     if (!inRange(squadId, 0, MAX_SQUAD_COUNT))
//     {
//         // WAR_AI_COMMAND_STATUS_INVALID_SQUAD
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     if (count > MAX_UNIT_SELECTION_COUNT)
//     {
//         // WAR_AI_COMMAND_STATUS_TOO_MANY
//         return WAR_AI_COMMAND_STATUS_FAILED;
//     }

//     WarSquad squad = (WarSquad){squadId, count};
//     memset(squad.units, 0, sizeof(squad.units));
//     memcpy(squad.units, unitIds, count * sizeof(WarEntityId));
//     aiPlayer->squads[squadId] = squad;

//     return WAR_AI_COMMAND_STATUS_DONE;
// }

// WarCommandStatus executeWaitAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
// {
//     return WAR_AI_COMMAND_STATUS_RUNNING;
// }

// WarCommandStatus executeSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
// {
//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     ai->sleeping = true;
//     ai->sleepTime = command->sleep.time;

//     return WAR_AI_COMMAND_STATUS_DONE;
// }

WarCommandStatus executeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarCommand* command)
{
    // static WarAIExecuteFunc executeFuncs[WAR_AI_COMMAND_COUNT] =
    // {
    //     NULL,                       // WAR_AI_COMMAND_NONE,
    //     executeTrainAICommand,      // WAR_AI_COMMAND_TRAIN,
    //     executeBuildAICommand,      // WAR_AI_COMMAND_BUILD,
    //     executeUpgradeAICommand,    // WAR_AI_COMMAND_UPGRADE,
    //     executeMoveAICommand,       // WAR_AI_COMMAND_MOVE,
    //     executeAttackAICommand,     // WAR_AI_COMMAND_ATTACK,
    //     executeGatherAICommand,     // WAR_AI_COMMAND_GATHER,
    //     executeDeliverAICommand,    // WAR_AI_COMMAND_DELIVER,
    //     executeRepairAICommand,     // WAR_AI_COMMAND_REPAIR,
    //     executeCastAICommand,       // WAR_AI_COMMAND_CAST,
    //     executeSquadAICommand,      // WAR_AI_COMMAND_SQUAD,
    //     executeWaitAICommand,       // WAR_AI_COMMAND_WAIT,
    //     executeSleepAICommand,      // WAR_AI_COMMAND_SLEEP,
    // };

    // WarAIExecuteFunc executeFunc = executeFuncs[(s32)command->type];
    // if (!executeFunc)
    // {
    //     logError("AI commands of type %d can't be executed\n", command->type);
    //     return createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
    // }

    // return executeFunc(context, aiPlayer, command);

    return WAR_COMMAND_STATUS_FAILED;
}

// static bool checkRunningCommands(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarCommandList* commands = &ai->commands;

//     bool wait = false;

//     for (s32 i = 0; i < commands->count; i++)
//     {
//         WarCommand* command = commands->items[i];
//         assert(command);

//         WarCommandResult* commandResult = command->result;
//         if (commandResult && commandResult->status == WAR_AI_COMMAND_STATUS_RUNNING)
//         {
//             WarCommandStatus status = WAR_AI_COMMAND_STATUS_RUNNING;

//             if (command->type == WAR_AI_COMMAND_REQUEST_UNIT)
//             {
//                 WarUnitType unitType = commandResult->requestUnit.unitType;
//                 WarEntityId buildingId = commandResult->requestUnit.buildingId;
//                 WarEntityId workerId = commandResult->requestUnit.workerId;

//                 if (isDudeUnitType(unitType))
//                 {
//                     WarEntity* building = findEntity(context, buildingId);
//                     if (building)
//                     {
//                         if (!isTraining(building) && !isGoingToTrain(building))
//                         {
//                             status = WAR_AI_COMMAND_STATUS_DONE;
//                         }
//                     }
//                     else
//                     {
//                         // mark the command failed if the building no longer exists
//                         status = WAR_AI_COMMAND_STATUS_FAILED;
//                     }
//                 }
//                 else if (isBuildingUnitType(unitType))
//                 {
//                     WarEntity* building = findEntity(context, buildingId);
//                     WarEntity* worker = findEntity(context, workerId);
//                     if (building && worker)
//                     {
//                         if (!isBuilding(building) && !isGoingToBuild(building) &&
//                             !isRepairing2(worker) && !isGoingToRepair2(worker))
//                         {
//                             status = WAR_AI_COMMAND_STATUS_DONE;
//                         }
//                     }
//                     else
//                     {
//                         // if the building no longer exists, then mark the command failed
//                         status = WAR_AI_COMMAND_STATUS_FAILED;
//                     }
//                 }
//             }
//             else if (command->type == WAR_AI_COMMAND_RESOURCE)
//             {
//                 // do nothing here? maybe I need to check if there are workers
//                 // that couldn't be sent to the resource
//             }
//             else if (command->type == WAR_AI_COMMAND_WAIT)
//             {
//                 if (command->wait.commandId)
//                 {
//                     WarCommand* commandToWaitFor = NULL;
//                     for (s32 k = i - 1; k >= 0; k--)
//                     {
//                         WarCommand* prevCommand = commands->items[k];
//                         if (prevCommand && prevCommand->id == command->wait.commandId)
//                         {
//                             commandToWaitFor = prevCommand;
//                             break;
//                         }
//                     }

//                     if (commandToWaitFor && commandToWaitFor->result)
//                     {
//                         if (commandToWaitFor->result->status != WAR_AI_COMMAND_STATUS_RUNNING)
//                         {
//                             status = WAR_AI_COMMAND_STATUS_DONE;
//                         }
//                     }
//                     else
//                     {
//                         status = WAR_AI_COMMAND_STATUS_FAILED;
//                     }
//                 }
//                 else if (command->wait.resource != WAR_RESOURCE_NONE)
//                 {
//                     s32 resourceAmount = getPlayerResourceAmount(aiPlayer, command->wait.resource);
//                     if (resourceAmount >= command->wait.amount)
//                     {
//                         status = WAR_AI_COMMAND_STATUS_DONE;
//                     }

//                     // what if the AI is waiting for a particular resource and there is no
//                     // worker gathering it, maybe fail this command? put some workers to gather the resource?
//                 }
//                 else
//                 {
//                     status = WAR_AI_COMMAND_STATUS_DONE;
//                 }

//                 if (status == WAR_AI_COMMAND_STATUS_RUNNING)
//                 {
//                     wait = true;
//                 }
//             }

//             commandResult->status = status;
//         }
//     }

//     return !wait;
// }

void updateAIPlayer(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    if (ai->sleeping)
    {
        // should I use here the time scaling factors of the game?
        ai->sleepTime -= context->deltaTime;

        if (ai->sleepTime > 0)
        {
            return;
        }

        ai->sleeping = false;
        ai->sleepTime = 0.0f;
    }

    // if at least one wait command is still running,
    // don't call the ai get command func
    // if (!checkRunningCommands(context, aiPlayer))
    // {
    //     return;
    // }

    WarCommand* command = ai->getCommandFunc(context, aiPlayer);
    if (command /* && command->type != WAR_AI_COMMAND_NONE */)
    {
        command->status = executeAICommand(context, aiPlayer, command);

        WarCommandListAdd(&aiPlayer->commands, command);
    }
}

void updateAIPlayers(WarContext* context)
{
    WarMap* map = context->map;

    for (s32 i = 1; i < map->playersCount; i++)
    {
        updateAIPlayer(context, &map->players[i]);
    }
}
