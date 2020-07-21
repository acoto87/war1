WarAI* createAI(WarContext* context, const char* name)
{
    WarAI* ai = (WarAI*)xmalloc(sizeof(WarAI));
    memset(ai->name, 0, sizeof(ai->name));
    strcpy(ai->name, name);

    WarAICommandListInit(&ai->commands, WarAICommandListDefaultOptions);

    WarAIData aiData = getAIData(name);
    ai->initFunc = aiData.initFunc;
    ai->getCommandFunc = aiData.getCommandFunc;
    ai->customData = NULL;

    return ai;
}

WarAICommand* createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type)
{
    static WarAICommandId staticCommandId = 0;

    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICommand* command = (WarAICommand*)xcalloc(1, sizeof(WarAICommand));
    command->id = ++staticCommandId;
    command->type = type;
    command->result = NULL;

    return command;
}

WarAICommand* createRequestUnitAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool checkExisting, bool waitForIdleWorker)
{
    WarAICommand* request = createAICommand(context, aiPlayer, WAR_AI_COMMAND_REQUEST_UNIT);
    request->requestUnit.unitType = unitType;
    request->requestUnit.checkExisting = checkExisting;
    request->requestUnit.waitForIdleWorker = waitForIdleWorker;
    return request;
}

WarAICommand* createRequestUpgradeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarUpgradeType upgradeType)
{
    WarAICommand* request = createAICommand(context, aiPlayer, WAR_AI_COMMAND_REQUEST_UPGRADE);
    request->requestUpgrade.upgradeType = upgradeType;
    return request;
}

WarAICommand* createResourceAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resourceKind, s32 count, bool waitForIdleWorker)
{
    WarAICommand* resource = createAICommand(context, aiPlayer, WAR_AI_COMMAND_RESOURCE);
    resource->resource.resourceKind = resourceKind;
    resource->resource.count = count;
    resource->resource.waitForIdleWorker = waitForIdleWorker;
    return resource;
}

WarAICommand* createSquadAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarSquadId id, s32 count, WarSquadUnitRequest requests[])
{
    assert(inRange(id, 0, MAX_SQUAD_COUNT));

    WarAICommand* squad = createAICommand(context, aiPlayer, WAR_AI_COMMAND_SQUAD);
    squad->squad.id = id;
    squad->squad.count = count;
    memset(squad->squad.requests, 0, sizeof(squad->squad.requests));
    memcpy(squad->squad.requests, requests, count * sizeof(WarSquadUnitRequest));
    return squad;
}

WarAICommand* createAttackAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarSquadId id)
{
    assert(inRange(id, 0, MAX_SQUAD_COUNT));

    WarAICommand* attack = createAICommand(context, aiPlayer, WAR_AI_COMMAND_ATTACK);
    attack->attack.id = id;
    return attack;
}

WarAICommand* createWaitAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandId commandId, WarResourceKind resource, s32 amount)
{
    WarAICommand* wait = createAICommand(context, aiPlayer, WAR_AI_COMMAND_WAIT);
    wait->wait.commandId = commandId;
    wait->wait.resource = resource;
    wait->wait.amount = amount;
    return wait;
}

WarAICommand* createSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, f32 time)
{
    WarAICommand* sleep = createAICommand(context, aiPlayer, WAR_AI_COMMAND_SLEEP);
    sleep->sleep.time = time;
    return sleep;
}

WarAICommandResult* createAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status)
{
    WarAICommandResult* result = (WarAICommandResult*)xcalloc(1, sizeof(WarAICommandResult));
    result->status = status;
    return result;
}

WarAICommandResult* createRequestUnitAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status, WarEntityId buildingId, WarEntityId workerId)
{
    WarAICommandResult* result = createAICommandResult(context, command, status);
    result->requestUnit.unitType = command->requestUnit.unitType;
    result->requestUnit.buildingId = buildingId;
    result->requestUnit.workerId = workerId;
    return result;
}

WarAICommandResult* createRequestUpgradeAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status, WarEntityId buildingId)
{
    WarAICommandResult* result = createAICommandResult(context, command, status);
    result->requestUpgrade.upgradeType = command->requestUpgrade.upgradeType;
    result->requestUpgrade.buildingId = buildingId;
    return result;
}

WarAICommandResult* createResourceAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status, s32 count, WarEntityId workerIds[])
{
    WarAICommandResult* result = createAICommandResult(context, command, status);
    result->resource.resourceKind = command->resource.resourceKind;
    result->resource.count = count;
    memset(result->resource.workerIds, 0, sizeof(result->resource.workerIds));
    memcpy(result->resource.workerIds, workerIds, count * sizeof(WarEntityId));
    return result;
}

WarAICommandResult* createSquadAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status, s32 count, WarEntityId unitIds[])
{
    WarAICommandResult* result = createAICommandResult(context, command, status);
    result->squad.count = count;
    memset(result->squad.unitIds, 0, sizeof(result->squad.unitIds));
    memcpy(result->squad.unitIds, unitIds, count * sizeof(WarEntityId));
    return result;
}

WarAICommandResult* createSleepAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status)
{
    return createAICommandResult(context, command, status);
}

void initAIPlayer(WarContext* context, WarPlayerInfo* aiPlayer)
{
    aiPlayer->ai = createAI(context, "level");

    if (aiPlayer->ai->initFunc)
        aiPlayer->ai->initFunc(context, aiPlayer);
}

void initAIPlayers(WarContext* context)
{
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    initAIPlayer(context, &map->players[1]);
}

static s32 idleWorkersCompare(const WarEntity* e1, const WarEntity* e2)
{
    assert(isUnit(e1));
    assert(isUnit(e2));

    s32 idle1 = isIdle((WarEntity*)e1);
    s32 idle2 = isIdle((WarEntity*)e2);

    // NOTE: should this consider distance, because if a idle worker is to far
    // maybe the AI could prefer to change one busy worker over a far idle one
    return idle2 - idle1;
}

static bool findPlaceToBuild(WarContext* context, WarUnitType unitType, vec2 startPosition, vec2* finalPosition)
{
    const s32 dirC = 8;
    const s32 dirX[] = { -1,  0,  1, 1, 1, 0, -1, -1 };
    const s32 dirY[] = { -1, -1, -1, 0, 1, 1,  1,  0 };

    bool result = false;

    vec2List positions;
    vec2ListInit(&positions, vec2ListDefaultOptions);

    vec2ListAdd(&positions, startPosition);

    for (s32 i = 0; i < positions.count; i++)
    {
        vec2 position = positions.items[i];

        if (canBuildingBeBuilt(context, unitType, position.x, position.y))
        {
            *finalPosition = position;
            result = true;
            break;
        }

        for (s32 d = 0; d < dirC; d++)
        {
            vec2 newPosition = vec2Addv(position, vec2i(dirX[d], dirY[d]));

            // NOTE: this can be optimized if we don't test for containment in the list
            // all the time, maybe having a 64x64 array with bools could be faster since
            // I only need to access the corresponding tile? or should I use a hash set?

            if (inRange(newPosition.x, 0, MAP_TILES_WIDTH) &&
                inRange(newPosition.y, 0, MAP_TILES_HEIGHT) &&
                !vec2ListContains(&positions, newPosition))
            {
                vec2ListAdd(&positions, newPosition);
            }
        }
    }

    vec2ListFree(&positions);

    return result;
}

static bool findMineForWorker(WarContext* context, WarEntity* worker)
{
    WarRace workerRace = getUnitRace(worker);
    WarEntityType townHallType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, workerRace);
    WarEntity* townHall = findClosestUnitOfType(context, worker, townHallType);
    assert(townHall);

    WarEntity* goldmine = findClosestUnitOfType(context, townHall, WAR_UNIT_GOLDMINE);
    return goldmine && sendWorkerToMine(context, worker, goldmine, townHall);
}

static bool findForestForWorker(WarContext* context, WarEntity* worker)
{
    WarRace workerRace = getUnitRace(worker);
    WarEntityType townHallType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, workerRace);
    WarEntity* townHall = findClosestUnitOfType(context, worker, townHallType);
    assert(townHall);

    vec2 townHallPosition = getUnitCenterPosition(townHall, true);

    WarEntity* forest = findClosestForest(context, townHall);
    if (forest)
    {
        WarTree* tree = findAccesibleTree(context, forest, townHallPosition);
        if (tree)
        {
            vec2 treeTile = vec2i(tree->tilex, tree->tiley);
            return sendWorkerToChop(context, worker, forest, treeTile, townHall);
        }
    }

    return false;
}

WarAICommandResult* executeRequestUnitCommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarAICommandResult* result = NULL;

    WarUnitType unitType = command->requestUnit.unitType;
    bool checkExisting = command->requestUnit.checkExisting;
    bool waitForIdleWorker = command->requestUnit.waitForIdleWorker;

    if (checkExisting)
    {
        if (playerHasUnit(context, aiPlayer->index, unitType))
        {
            result = createRequestUnitAICommandResult(context, command, WAR_AI_COMMAND_STATUS_DONE, 0, 0);
            return result;
        }
    }

    if (isDudeUnitType(unitType))
    {
        WarUnitStats stats = getUnitStats(unitType);
        if (!enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost) ||
            !enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_WOOD, stats.woodCost))
        {
            // WAR_AI_COMMAND_RESULT_NOT_ENOUGH_RESOURCE ?
            result = createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
            return result;
        }

        if (!enoughFarmFood(context, aiPlayer))
        {
            // WAR_AI_COMMAND_RESULT_NOT_ENOUGH_SUPPLY ?
            result = createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
            return result;
        }

        WarUnitType producerType = getUnitTypeProducer(unitType);
        assert(isValidUnitType(producerType));

        // find a producer (typically a building) that can build the stuff
        WarEntityList* producerUnits = getUnitsOfTypeOfPlayer(context, producerType, aiPlayer->index);
        for (s32 i = 0; i < producerUnits->count; i++)
        {
            WarEntity* producer = producerUnits->items[i];
            if (!isBuilding(producer) && !isGoingToBuild(producer) &&
                !isTraining(producer) && !isGoingToTrain(producer) &&
                !isUpgrading(producer) && !isGoingToUpgrade(producer))
            {
                // playerResourcesDecreased should be true at this point because we checked earlier
                bool playerResourcesDecreased = decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost) &&
                                                decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_WOOD, stats.woodCost);

                assert(playerResourcesDecreased);

                WarState* trainState = createTrainState(context, producer, unitType, stats.buildTime);
                changeNextState(context, producer, trainState, true, true);

                result = createRequestUnitAICommandResult(context, command, WAR_AI_COMMAND_STATUS_RUNNING, producer->id, 0);
                break;
            }
        }

        if (!result)
        {
            // WAR_AI_COMMAND_RESULT_NO_PRODUCER_AVAILABLE ?
            result = createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
        }

        WarEntityListFree(producerUnits);
    }
    else if (isBuildingUnitType(unitType))
    {
        WarBuildingStats stats = getBuildingStats(unitType);
        if (!enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost) ||
            !enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_WOOD, stats.woodCost))
        {
            // WAR_AI_COMMAND_RESULT_NOT_ENOUGH_RESOURCE ?
            result = createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
            return result;
        }

        WarUnitType workerType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
        WarEntityList* workers = getUnitsOfTypeOfPlayer(context, workerType, aiPlayer->index);

        WarEntityListSort(workers, idleWorkersCompare);

        // find a worker that can build the stuff
        for (s32 i = 0; i < workers->count; i++)
        {
            WarEntity* worker = workers->items[i];
            if (isIdle(worker) || (!waitForIdleWorker && !isInsideBuilding(worker)))
            {
                // find a place to build
                vec2 targetTile = getUnitCenterPosition(worker, true);
                if (findPlaceToBuild(context, unitType, targetTile, &targetTile))
                {
                    // playerResourcesDecreased should be true at this point because we checked earlier
                    bool playerResourcesDecreased = decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost) &&
                                                    decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_WOOD, stats.woodCost);

                    assert(playerResourcesDecreased);

                    WarEntity* building = createBuilding(context, unitType, targetTile.x, targetTile.y, aiPlayer->index, true);
                    WarState* repairState = createRepairState(context, worker, building->id);
                    changeNextState(context, worker, repairState, true, true);

                    result = createRequestUnitAICommandResult(context, command, WAR_AI_COMMAND_STATUS_RUNNING, building->id, worker->id);
                }
                else
                {
                    // WAR_AI_COMMAND_RESULT_NOT_ENOUGH_SPACE ?
                    result = createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
                }

                break;
            }
        }

        if (!result)
        {
            // WAR_AI_COMMAND_RESULT_NO_WORKER_AVAILABLE ?
            result = createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
        }

        WarEntityListFree(workers);
    }
    else
    {
        logError("Unkown unit type %d to be built by player %d\n", unitType, aiPlayer->index);
        assert(false);
        return NULL;
    }

    return result;
}

WarAICommandResult* executeRequestUpgradeCommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarAICommandResult* result = NULL;

    WarUpgradeType upgradeType = command->requestUpgrade.upgradeType;

    if (!hasRemainingUpgrade(aiPlayer, upgradeType))
    {
        // WAR_AI_COMMAND_RESULT_NOT_MORE_UPGRADE ?
        result = createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
        return result;
    }

    s32 upgradeLevel = getUpgradeLevel(aiPlayer, upgradeType);
    WarUpgradeStats stats = getUpgradeStats(upgradeType);
    if (!enoughPlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost[upgradeLevel]))
    {
        // WAR_AI_COMMAND_RESULT_NOT_ENOUGH_RESOURCE ?
        result = createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
        return result;
    }

    WarUnitType producerType = getUpgradeTypeProducer(upgradeType, aiPlayer->race);
    assert(isValidUnitType(producerType));

    // find a producer (typically a building) that can build the stuff
    WarEntityList* producerUnits = getUnitsOfTypeOfPlayer(context, producerType, aiPlayer->index);
    for (s32 i = 0; i < producerUnits->count; i++)
    {
        WarEntity* producer = producerUnits->items[i];
        if (!isBuilding(producer) && !isGoingToBuild(producer) &&
            !isTraining(producer) && !isGoingToTrain(producer) &&
            !isUpgrading(producer) && !isGoingToUpgrade(producer))
        {
            bool playerResourcesDecreased = decreasePlayerResource(context, aiPlayer, WAR_RESOURCE_GOLD, stats.goldCost[upgradeLevel]);
            assert(playerResourcesDecreased);

            WarState* upgradeState = createUpgradeState(context, producer, upgradeType, stats.buildTime);
            changeNextState(context, producer, upgradeState, true, true);

            result = createRequestUnitAICommandResult(context, command, WAR_AI_COMMAND_STATUS_RUNNING, producer->id, 0);
            break;
        }
    }

    if (!result)
    {
        // WAR_AI_COMMAND_RESULT_NO_PRODUCER_AVAILABLE ?
        result = createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
    }

    WarEntityListFree(producerUnits);

    return result;
}

WarAICommandResult* executeResourceAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarResourceKind resourceKind = command->resource.resourceKind;
    if (resourceKind != WAR_RESOURCE_GOLD && resourceKind != WAR_RESOURCE_WOOD)
    {
        logError("Can't execute resource AI command for resource kind: %d\n", command->resource.resourceKind);
        assert(false);
        return NULL;
    }

    s32 count = command->resource.count;
    if (count == 0)
    {
        logWarning("Sending 0 workers to harvest resource %d?\n", resourceKind);

        // WAR_AI_COMMAND_STATUS_NOT_ENOUGH_WORKERS ?
        return createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
    }

    if (count > 4)
    {
        logError("Can't send more than 4 workers to harvest resources at the same time: %d\n", count);

        // // WAR_AI_COMMAND_STATUS_TOO_MANY_WORKERS ?
        return createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
    }

    bool waitForIdleWorker = command->resource.waitForIdleWorker;

    WarUnitType workerType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    WarEntityList* workers = getUnitsOfTypeOfPlayer(context, workerType, aiPlayer->index);

    WarEntityListSort(workers, idleWorkersCompare);

    s32 workerCount = 0;
    WarEntityId workerIds[4] = {0};

    for (s32 i = 0; i < workers->count && workerCount < count; i++)
    {
        WarEntity* worker = workers->items[i];

        bool isIdle = isIdle(worker);
        if (isIdle || (!waitForIdleWorker && !isInsideBuilding(worker)))
        {
            if (resourceKind == WAR_RESOURCE_GOLD)
            {
                bool isMining = !isIdle && (isMining(worker) || isGoingToMine(worker));
                if (!isMining && findMineForWorker(context, worker))
                    workerIds[workerCount++] = worker->id;
            }
            else if (resourceKind == WAR_RESOURCE_WOOD)
            {
                bool isChopping = !isIdle && (isChopping(worker) || isGoingToChop(worker));
                if (!isChopping && findForestForWorker(context, worker))
                    workerIds[workerCount++] = worker->id;
            }
        }
        else
        {
            // since the data is sorted, first the idle workers and then the others,
            // from this index, there is only non-idle workers, and we need to wait for idle ones,
            // or every worker are inside buildings. so we can stop searching for idle workers at this point
            break;
        }
    }

    WarEntityListFree(workers);

    WarAICommandStatus status = WAR_AI_COMMAND_STATUS_RUNNING;
    if (workerCount == 0)
        status = WAR_AI_COMMAND_STATUS_FAILED;
    else if (workerCount == count)
        status = WAR_AI_COMMAND_STATUS_DONE;

    return createResourceAICommandResult(context, command, status, workerCount, workerIds);
}

WarAICommandResult* executeSquadAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarSquadId squadId = command->squad.id;
    s32 requestsCount = command->squad.count;
    WarSquadUnitRequest* requests = command->squad.requests;

    s32 squadCountRequested = 0;
    for (s32 i = 0; i < requestsCount; i++)
        squadCountRequested += requests[i].count;

    // ai can't request a squad that have more units than it's allowed to be selected at a time
    if (squadCountRequested > MAX_UNIT_SELECTION_COUNT)
    {
        return createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
    }

    s32 squadCount = 0;
    WarEntityId squadUnits[MAX_UNIT_SELECTION_COUNT];

    for (s32 i = 0; i < requestsCount; i++)
    {
        WarSquadUnitRequest request = requests[i];

        WarEntityList* units = getUnitsOfTypeOfPlayer(context, request.unitType, aiPlayer->index);

        for (s32 k = 0; k < units->count; k++)
        {
            if (squadCount >= request.count)
                break;

            WarEntity* unit = units->items[k];
            assert(unit);

            if (!isInSquad(context, aiPlayer, unit->id))
            {
                squadUnits[squadCount++] = unit->id;
            }
        }

        WarEntityListFree(units);
    }

    WarSquad squad = (WarSquad){squadId, squadCount};
    memset(squad.units, 0, sizeof(squad.units));
    memcpy(squad.units, squadUnits, squadCount * sizeof(WarEntityId));
    aiPlayer->squads[squadId] = squad;

    return createSquadAICommandResult(context, command, WAR_AI_COMMAND_STATUS_DONE, squadCount, squadUnits);
}

WarAICommandResult* executeAttackAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarMap* map = context->map;
    assert(map);

    WarSquadId squadId = command->attack.id;


    WarSquad* squad = &aiPlayer->squads[squadId];
    if (squad->count == 0)
    {
        // WAR_AI_COMMAND_STATUS_NO_SQUAD ?
        return createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
    }

    WarPlayerInfo* enemyPlayer = &map->players[0];

    WarUnitType enemyTownHallType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, enemyPlayer->race);
    WarEntityList* enemyTownHalls = getUnitsOfTypeOfPlayer(context, enemyTownHallType, enemyPlayer->index);

    if (enemyTownHalls->count == 0)
    {
        // WAR_AI_COMMAND_STATUS_NO_ATTACK_TARGET ?
        return createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
    }

    WarEntity* enemyTownHall = enemyTownHalls->count > 1
        ? enemyTownHalls->items[randomi(0, enemyTownHalls->count)]
        : enemyTownHalls->items[0];

    // find attack point and send to attack
    vec2 townHallTile = getEntityPosition(enemyTownHall, true);

    // WAR_AI_COMMAND_STATUS_NO_SQUAD
    WarAICommandStatus status = WAR_AI_COMMAND_STATUS_FAILED;

    for (s32 i = 0; i < squad->count; i++)
    {
        WarEntityId unitId = squad->units[i];
        WarEntity* unit = findEntity(context, unitId);
        if (unit && !isDead(unit) && !isGoingToDie(unit))
        {

            WarState* attackState = createAttackState(context, unit, 0, townHallTile);
            changeNextState(context, unit, attackState, true, true);

            // mark it as done if at least one unit of the squad went to the attack
            status = WAR_AI_COMMAND_STATUS_DONE;
        }
    }

    return createAICommandResult(context, command, status);
}

WarAICommandResult* executeWaitAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    return createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_RUNNING);
}

WarAICommandResult* executeSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    ai->sleeping = true;
    ai->sleepTime = command->sleep.time;

    return createSleepAICommandResult(context, command, WAR_AI_COMMAND_STATUS_DONE);
}

WarAICommandResult* executeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    static WarAIExecuteFunc executeFuncs[WAR_AI_COMMAND_COUNT] =
    {
        NULL,                           // WAR_AI_COMMAND_NONE
        executeRequestUnitCommand,      // WAR_AI_COMMAND_REQUEST_UNIT
        executeRequestUpgradeCommand,   // WAR_AI_COMMAND_REQUEST_UPGRADE
        executeResourceAICommand,       // WAR_AI_COMMAND_RESOURCE
        executeSquadAICommand,          // WAR_AI_COMMAND_SQUAD
        executeAttackAICommand,         // WAR_AI_COMMAND_ATTACK
        executeWaitAICommand,           // WAR_AI_COMMAND_WAIT
        executeSleepAICommand,          // WAR_AI_COMMAND_SLEEP
    };

    WarAIExecuteFunc executeFunc = executeFuncs[(s32)command->type];
    if (!executeFunc)
    {
        logError("AI commands of type %d can't be executed\n", command->type);
        return createAICommandResult(context, command, WAR_AI_COMMAND_STATUS_FAILED);
    }

    return executeFunc(context, aiPlayer, command);
}

static bool checkRunningCommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICommandList* commands = &ai->commands;

    bool wait = false;

    for (s32 i = 0; i < commands->count; i++)
    {
        WarAICommand* command = commands->items[i];
        assert(command);

        WarAICommandResult* commandResult = command->result;
        if (commandResult && commandResult->status == WAR_AI_COMMAND_STATUS_RUNNING)
        {
            WarAICommandStatus status = WAR_AI_COMMAND_STATUS_RUNNING;

            if (command->type == WAR_AI_COMMAND_REQUEST_UNIT)
            {
                WarUnitType unitType = commandResult->requestUnit.unitType;
                WarEntityId buildingId = commandResult->requestUnit.buildingId;
                WarEntityId workerId = commandResult->requestUnit.workerId;

                if (isDudeUnitType(unitType))
                {
                    WarEntity* building = findEntity(context, buildingId);
                    if (building)
                    {
                        if (!isTraining(building) && !isGoingToTrain(building))
                        {
                            status = WAR_AI_COMMAND_STATUS_DONE;
                        }
                    }
                    else
                    {
                        // mark the command failed if the building no longer exists
                        status = WAR_AI_COMMAND_STATUS_FAILED;
                    }
                }
                else if (isBuildingUnitType(unitType))
                {
                    WarEntity* building = findEntity(context, buildingId);
                    WarEntity* worker = findEntity(context, workerId);
                    if (building && worker)
                    {
                        if (!isBuilding(building) && !isGoingToBuild(building) &&
                            !isRepairing2(worker) && !isGoingToRepair2(worker))
                        {
                            status = WAR_AI_COMMAND_STATUS_DONE;
                        }
                    }
                    else
                    {
                        // if the building no longer exists, then mark the command failed
                        status = WAR_AI_COMMAND_STATUS_FAILED;
                    }
                }
            }
            else if (command->type == WAR_AI_COMMAND_RESOURCE)
            {
                // do nothing here? maybe I need to check if there are workers
                // that couldn't be sent to the resource
            }
            else if (command->type == WAR_AI_COMMAND_WAIT)
            {
                if (command->wait.commandId)
                {
                    WarAICommand* commandToWaitFor = NULL;
                    for (s32 k = i - 1; k >= 0; k--)
                    {
                        WarAICommand* prevCommand = commands->items[k];
                        if (prevCommand && prevCommand->id == command->wait.commandId)
                        {
                            commandToWaitFor = prevCommand;
                            break;
                        }
                    }

                    if (commandToWaitFor && commandToWaitFor->result)
                    {
                        if (commandToWaitFor->result->status != WAR_AI_COMMAND_STATUS_RUNNING)
                        {
                            status = WAR_AI_COMMAND_STATUS_DONE;
                        }
                    }
                    else
                    {
                        status = WAR_AI_COMMAND_STATUS_FAILED;
                    }
                }
                else if (command->wait.resource != WAR_RESOURCE_NONE)
                {
                    s32 resourceAmount = getPlayerResourceAmount(aiPlayer, command->wait.resource);
                    if (resourceAmount >= command->wait.amount)
                    {
                        status = WAR_AI_COMMAND_STATUS_DONE;
                    }

                    // what if the AI is waiting for a particular resource and there is no
                    // worker gathering it, maybe fail this command? put some workers to gather the resource?
                }
                else
                {
                    status = WAR_AI_COMMAND_STATUS_DONE;
                }

                if (status == WAR_AI_COMMAND_STATUS_RUNNING)
                {
                    wait = true;
                }
            }

            commandResult->status = status;
        }
    }

    return !wait;
}

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
    if (!checkRunningCommands(context, aiPlayer))
    {
        return;
    }

    WarAICommand* command = ai->getCommandFunc(context, aiPlayer);
    if (command && command->type != WAR_AI_COMMAND_NONE)
    {
        command->result = executeAICommand(context, aiPlayer, command);

        WarAICommandListAdd(&ai->commands, command);
    }
}

void updateAIPlayers(WarContext* context)
{
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    updateAIPlayer(context, &map->players[1]);
}
