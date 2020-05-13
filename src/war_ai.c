WarAI* createAI(WarContext* context, const char* name)
{
    WarAI* ai = (WarAI*)xmalloc(sizeof(WarAI));
    memset(ai->name, 0, sizeof(ai->name));
    strcpy(ai->name, name);

    ai->staticCommandId = 0;

    WarAIData aiData = getAIData(name);
    ai->initFunc = aiData.initFunc;
    ai->getCommandFunc = aiData.getCommandFunc;
    ai->executedCommandFunc = aiData.executedCommandFunc;

    ai->customData = NULL;

    return ai;
}

WarAICommand* createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICommand* command = (WarAICommand*)xcalloc(1, sizeof(WarAICommand));
    command->id = ++ai->staticCommandId;
    command->type = type;

    return command;
}

WarAICommand* createRequestAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool waitForIdleWorker)
{
    WarAICommand* request = createAICommand(context, aiPlayer, WAR_AI_COMMAND_REQUEST);
    request->request.unitType = unitType;
    request->request.waitForIdleWorker = waitForIdleWorker;
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

WarAICommand* createSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, f32 time)
{
    WarAICommand* sleep = createAICommand(context, aiPlayer, WAR_AI_COMMAND_SLEEP);
    sleep->sleep.time = time;
    return sleep;
}

WarAICommandResult* createAICommandResult(WarContext* context, WarAICommand* command, bool completed)
{
    WarAICommandResult* result = (WarAICommandResult*)xcalloc(1, sizeof(WarAICommandResult));
    result->commandId = command->id;
    result->commandType = command->type;
    result->completed = completed;
    return result;
}

WarAICommandResult* createRequestAICommandResult(WarContext* context, WarAICommand* command, bool completed, WarEntityId buildingId, WarEntityId workerId)
{
    WarAICommandResult* result = createAICommandResult(context, command, completed);
    result->request.unitType = command->request.unitType;
    result->request.buildingId = buildingId;
    result->request.workerId = workerId;
    return result;
}

WarAICommandResult* createResourceAICommandResult(WarContext* context, WarAICommand* command, bool completed, s32 count, WarEntityId workerIds[])
{
    WarAICommandResult* result = createAICommandResult(context, command, completed);
    result->resource.resourceKind = command->resource.resourceKind;
    result->resource.count = count;
    memset(result->resource.workerIds, 0, sizeof(result->resource.workerIds));
    memcpy(result->resource.workerIds, workerIds, count * sizeof(WarEntityId));
    return result;
}

WarAICommandResult* createSleepAICommandResult(WarContext* context, WarAICommand* command, bool completed)
{
    return createAICommandResult(context, command, completed);
}

void initAIPlayer(WarContext* context, WarPlayerInfo* aiPlayer)
{
    aiPlayer->ai = createAI(context, "land-attack");
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

WarAICommandResult* executeRequestAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarAICommandResult* result = NULL;

    WarUnitType unitType = command->request.unitType;
    bool waitForIdleWorker = command->request.waitForIdleWorker;

    if (isDudeUnitType(unitType))
    {
        WarUnitStats stats = getUnitStats(unitType);
        if (!enoughPlayerResources(context, aiPlayer, stats.goldCost, stats.woodCost))
        {
            // NOTE: I think here I should return a result with some kind of
            // status WAR_AI_COMMAND_RESULT_NOT_ENOUGH_RESOURCE or something?

            // there is not enough resources to create the unit
            return result;
        }

        // check if there is enough food
        if (!enoughFarmFood(context, aiPlayer))
        {
            // NOTE: I think here I should return a result with some kind of
            // status WAR_AI_COMMAND_RESULT_NOT_ENOUGH_SUPPLY or something?

            // set a request for supply because there is not enough food
            return result;
        }

        WarUnitType producerType = getProducerUnitOfType(unitType);
        if (isValidUnitType(producerType))
        {
            WarEntityList* producerUnits = getUnitsOfTypeOfPlayer(context, producerType, aiPlayer->index);
            for (s32 i = 0; i < producerUnits->count; i++)
            {
                WarEntity* producer = producerUnits->items[i];
                if (!isTraining(producer) && !isUpgrading(producer))
                {
                    if (decreasePlayerResources(context, aiPlayer, stats.goldCost, stats.woodCost))
                    {
                        WarState* trainState = createTrainState(context, producer, unitType, stats.buildTime);
                        changeNextState(context, producer, trainState, true, true);

                        result = createRequestAICommandResult(context, command, true, producer->id, 0);
                        break;
                    }
                }
            }
            WarEntityListFree(producerUnits);
        }
    }
    else if (isBuildingUnitType(unitType))
    {
        WarBuildingStats stats = getBuildingStats(unitType);
        if (!enoughPlayerResources(context, aiPlayer, stats.goldCost, stats.woodCost))
        {
            // NOTE: I think here I should return a result with some kind of
            // status WAR_AI_COMMAND_RESULT_NOT_ENOUGH_RESOURCE or something?

            // there is not enough resources to create the unit
            return result;
        }

        WarUnitType workerType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
        WarEntityList* workers = getUnitsOfTypeOfPlayer(context, workerType, aiPlayer->index);

        WarEntityListSort(workers, idleWorkersCompare);

        // find a worker that can build the stuff
        if (workers->count > 0)
        {
            WarEntity* worker = workers->items[0];

            // since this worker collection is sorted by idle then,
            // if the first worker doesn't satisfy this condition,
            // no other will
            if (isIdle(worker) || !waitForIdleWorker)
            {
                // find a place to build
                vec2 targetTile = getUnitCenterPosition(worker, true);
                if (findPlaceToBuild(context, unitType, targetTile, &targetTile))
                {
                    if (decreasePlayerResources(context, aiPlayer, stats.goldCost, stats.woodCost))
                    {
                        WarEntity* building = createBuilding(context, unitType, targetTile.x, targetTile.y, aiPlayer->index, true);
                        WarState* repairState = createRepairState(context, worker, building->id);
                        changeNextState(context, worker, repairState, true, true);

                        result = createRequestAICommandResult(context, command, true, building->id, worker->id);
                    }
                }
            }
        }

        WarEntityListFree(workers);
    }
    else
    {
        logWarning("Unkown unit type %d to be built by player %d\n", unitType, aiPlayer->index);
    }

    return result;
}

WarAICommandResult* executeResourceAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarResourceKind resourceKind = command->resource.resourceKind;
    if (resourceKind != WAR_RESOURCE_GOLD && resourceKind != WAR_RESOURCE_WOOD)
    {
        logWarning("Can't execute resource AI command for resource kind: %d\n", command->resource.resourceKind);
        return NULL;
    }

    s32 count = command->resource.count;
    if (count == 0)
    {
        logWarning("Sending 0 workers to harvest resource %d?\n", resourceKind);
        return NULL;
    }

    if (count > 4)
    {
        logError("Can't send more than 4 workers to harvest resources at the same time: %d\n", count);
        return NULL;
    }

    bool waitForIdleWorker = command->resource.waitForIdleWorker;

    WarUnitType workerType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    WarEntityList* workers = getUnitsOfTypeOfPlayer(context, workerType, aiPlayer->index);

    WarEntityListSort(workers, idleWorkersCompare);

    s32 workerCount = 0;
    WarEntityId workerIds[4] = {0};

    for (s32 i = 0; i < workers->count; i++)
    {
        if (workerCount == count)
            break;

        WarEntity* worker = workers->items[i];
        if (isIdle(worker))
        {
            if (resourceKind == WAR_RESOURCE_GOLD)
            {
                if (findMineForWorker(context, worker))
                    workerIds[workerCount++] = worker->id;
            }
            else if (resourceKind == WAR_RESOURCE_WOOD)
            {
                if (findForestForWorker(context, worker))
                    workerIds[workerCount++] = worker->id;
            }
        }
        else if (!waitForIdleWorker)
        {
            if (resourceKind == WAR_RESOURCE_GOLD && !isMining(worker) && !isGoingToMine(worker))
            {
                if (findMineForWorker(context, worker))
                    workerIds[workerCount++] = worker->id;
            }
            else if (resourceKind == WAR_RESOURCE_WOOD && !isChopping(worker) && !isGoingToChop(worker))
            {
                if (findForestForWorker(context, worker))
                    workerIds[workerCount++] = worker->id;
            }
        }
        else
        {
            // since the data is sorted, first the idle workers and then the others,
            // from here there is only non-idle workers, and we need to wait for idle ones
            // we can stop searching for idle workers at this point
            break;
        }
    }

    WarEntityListFree(workers);

    return createResourceAICommandResult(context, command, workerCount == count, workerCount, workerIds);
}

WarAICommandResult* executeSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    ai->sleeping = true;
    ai->sleepTime = command->sleep.time;

    return createSleepAICommandResult(context, command, true);
}

WarAICommandResult* executeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    static WarAIExecuteFunc executeFuncs[WAR_AI_COMMAND_COUNT] =
    {
        NULL,                       // WAR_AI_COMMAND_NONE
        executeRequestAICommand,    // WAR_AI_COMMAND_REQUEST
        executeResourceAICommand,   // WAR_AI_COMMAND_RESOURCE
        executeSleepAICommand,      // WAR_AI_COMMAND_SLEEP
    };

    WarAIExecuteFunc executeFunc = executeFuncs[(s32)command->type];
    if (!executeFunc)
    {
        logError("AI commands of type %d can't be executed\n", command->type);
        return NULL;
    }

    return executeFunc(context, aiPlayer, command);
}

void updateAIPlayer(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    if (ai->sleeping)
    {
        ai->sleepTime -= context->deltaTime;

        if (ai->sleepTime > 0)
        {
            return;
        }

        ai->sleeping = false;
        ai->sleepTime = 0.0f;
    }

    WarAICommand* command = ai->getCommandFunc(context, aiPlayer);
    if (command && command->type != WAR_AI_COMMAND_NONE)
    {
        WarAICommandResult* result = executeAICommand(context, aiPlayer, command);
        ai->executedCommandFunc(context, aiPlayer, command, result);

        // we shouldn't use here `command` or `result` since the AI could dispose them
    }
}

void updateAIPlayers(WarContext* context)
{
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    updateAIPlayer(context, &map->players[1]);
}
