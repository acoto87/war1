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

WarAICommandResult* executeRequestAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarAICommandResult* result = NULL;

    WarUnitType unitType = command->request.unitType;

    if (isDudeUnitType(unitType))
    {
        WarUnitStats stats = getUnitStats(unitType);
        if (!enoughPlayerResources(context, aiPlayer, stats.goldCost, stats.woodCost))
        {
            // there is not enough resources to create the unit
            return result;
        }

        // check if there is enough food
        if (!enoughFarmFood(context, aiPlayer))
        {
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
        // find a free worker
        // find a place to build
        // send worker to build the building

        // if there is no free worker, try to get one assigned to a resource
        return result;
    }
    else
    {
        logWarning("Unkown unit type %d to be built by player %d\n", unitType, aiPlayer->index);
    }

    return result;
}

static s32 idleWorkersCompare(const WarEntity* e1, const WarEntity* e2)
{
    assert(isUnit(e1));
    assert(isUnit(e2));

    s32 idle1 = isIdle((WarEntity*)e1);
    s32 idle2 = isIdle((WarEntity*)e2);

    return idle2 - idle1;
}

bool findMineForWorker(WarContext* context, WarEntity* worker)
{
    WarRace workerRace = getUnitRace(worker);
    WarEntityType townHallType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, workerRace);
    WarEntity* townHall = findClosestUnitOfType(context, worker, townHallType);
    assert(townHall);

    WarEntity* goldmine = findClosestUnitOfType(context, townHall, WAR_UNIT_GOLDMINE);
    return goldmine && sendWorkerToMine(context, worker, goldmine, townHall);
}

bool findForestForWorker(WarContext* context, WarEntity* worker)
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
    WarEntityId workerIds[4];

    for (s32 i = 0; i < workers->count; i++)
    {
        if (workerCount == count)
            break;

        WarEntity* worker = workers->items[i];
        if (worker)
        {
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

    return NULL;
}

WarAICommandResult* executeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    switch (command->type)
    {
        case WAR_AI_COMMAND_REQUEST:
        {
            return executeRequestAICommand(context, aiPlayer, command);
        }

        case WAR_AI_COMMAND_RESOURCE:
        {
            return executeResourceAICommand(context, aiPlayer, command);
        }

        case WAR_AI_COMMAND_SLEEP:
        {
            return executeSleepAICommand(context, aiPlayer, command);
        }

        default:
        {
            logWarning("AI commands of type %d are NOT handled yet\n", command->type);
            return NULL;
        }
    }
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
    if (command)
    {
        WarAICommandResult* result = executeAICommand(context, aiPlayer, command);
        ai->executedCommandFunc(context, aiPlayer, command, result);
    }
}

void updateAIPlayers(WarContext* context)
{
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    updateAIPlayer(context, &map->players[1]);
}
