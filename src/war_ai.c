WarAI* createAI(WarContext* context)
{
    WarAI* ai = (WarAI*)xmalloc(sizeof(WarAI));
    ai->staticCommandId = 0;

    WarAICommandListInit(&ai->currentCommands, WarAICommandListDefaultOptions);
    WarAICommandQueueInit(&ai->nextCommands, WarAICommandQueueDefaultOptions);

    return ai;
}

WarAICommand* createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICommand* command = (WarAICommand*)xcalloc(1, sizeof(WarAICommand));
    command->id = ++ai->staticCommandId;
    command->type = type;
    command->status = WAR_AI_COMMAND_STATUS_CREATED;

    WarAIRequestQueuePush(&ai->nextCommands, command);

    return command;
}

void addUnitRequest(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count, bool checkExisting)
{
    WarAICommand* request = createAICommand(context, aiPlayer, WAR_AI_COMMAND_REQUEST);
    request->request.unitType = unitType;
    request->request.count = count;
    request->request.checkExisting = checkExisting;
}

void waitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count)
{
    WarAICommand* wait = createAICommand(context, aiPlayer, WAR_AI_COMMAND_WAIT);
    wait->wait.unitType = unitType;
    wait->wait.count = count;
}

void sleepForTime(WarContext* context, WarPlayerInfo* aiPlayer,s32 time)
{
    WarAICommand* sleep = createAICommand(context, aiPlayer, WAR_AI_COMMAND_SLEEP);
    sleep->sleep.time = time;
}

#define isValidUnitType(type) inRange(type, WAR_UNIT_FOOTMAN, WAR_UNIT_COUNT)

bool tryBuildUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType)
{
    if (isDudeUnitType(unitType))
    {
        WarUnitType producerType = getProducerUnitOfType(unitType);
        if (isValidUnitType(producerType))
        {
            WarEntityList* units = getUnitsOfType(context, producerType);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                if (entity && entity->unit.player == aiPlayer->index)
                {
                    if (!isTraining(entity) && !isUpgrading(entity))
                    {
                        // this building can train the unit

                        return true;
                    }
                }
            }
        }

        // if there is no building to train the unit or all of them are busy
        // let the request live to try again later
        return false;
    }

    if (isBuildingUnitType(unitType))
    {
        // find a free worker
        // find a place to build
        // send worker to build the building

        // if there is no free worker, try to get one assigned to a resource

        // at the worst case, the building can't be built right now, so
        // let the request live to try again later
        return true;
    }

    logWarning("Unkown unit type %d to be built by player %d\n", unitType, aiPlayer->index);
    return false;
}

bool executeRequestAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    command->status = WAR_AI_COMMAND_STATUS_STARTED;

    if (command->request.checkExisting)
    {
        s32 numberOfUnits = getNumberOfUnitsOfType(context, aiPlayer->index, command->request.unitType);
        command->request.count -= numberOfUnits;
        if (command->request.count <= 0)
        {
            command->status = WAR_AI_COMMAND_STATUS_COMPLETED;
            return true;
        }
    }

    if (tryBuildUnit(context, aiPlayer, command->request.unitType))
    {
        command->request.count--;
        if (command->request.count == 0)
        {
            command->status = WAR_AI_COMMAND_STATUS_COMPLETED;
        }
    }

    return true;
}

bool executeWaitAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    command->status = WAR_AI_COMMAND_STATUS_STARTED;

    s32 numberOfUnits = getNumberOfUnitsOfType(context, aiPlayer->index, command->wait.unitType);
    if (numberOfUnits >= command->wait.count)
    {
        command->status = WAR_AI_COMMAND_STATUS_COMPLETED;
        return true;
    }

    return false;
}

bool executeSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    command->status = WAR_AI_COMMAND_STATUS_STARTED;
    command->sleep.time -= context->deltaTime;

    if (command->sleep.time <= 0)
    {
        command->status = WAR_AI_COMMAND_STATUS_COMPLETED;
        return true;
    }

    return false;
}

bool executeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    if (command->status == WAR_AI_COMMAND_STATUS_COMPLETED)
    {
        return true;
    }

    switch (command->type)
    {
        case WAR_AI_COMMAND_REQUEST:
        {
            return executeRequestAICommand(context, aiPlayer, command);
        }

        case WAR_AI_COMMAND_WAIT:
        {
            return executeWaitAICommand(context, aiPlayer, command);
        }

        case WAR_AI_COMMAND_SLEEP:
        {
            return executeSleepAICommand(context, aiPlayer, command);
        }

        default:
        {
            logWarning("AI commands of type %d are NOT handled yet\n", command->type);
            return true;
        }
    }
}

void initAI(WarContext* context)
{
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    WarPlayerInfo* aiPlayer = &map->players[1];
    aiPlayer->ai = createAI(context);
}

void updateAICommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUnitType townHall = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, aiPlayer->race);
    addUnitRequest(context, aiPlayer, townHall, 1, true);
    waitForUnit(context, aiPlayer, townHall, 1);

    WarUnitType worker = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    addUnitRequest(context, aiPlayer, worker, 1, true);
    waitForUnit(context, aiPlayer, worker, 1);
}

bool processAICurrentCommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    bool processNext = true;

    WarAICommandList* commands = &ai->currentCommands;
    for (s32 i = 0; i < commands->count; i++)
    {
        WarAICommand* command = commands->items[i];
        if (!executeAICommand(context, aiPlayer, command))
        {
            processNext = false;
        }
    }

    return processNext;
}

void processAINextCommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    bool processNext = true;

    WarAICommandList* currentCommands = &ai->currentCommands;
    WarAICommandQueue* nextCommands = &ai->nextCommands;
    if (nextCommands->count > 0)
    {
        WarAICommand* command = WarAICommandQueuePop(nextCommands);
        while (nextCommands->count > 0 &&
               command->type != WAR_AI_COMMAND_WAIT &&
               command->type != WAR_AI_COMMAND_SLEEP)
        {
            WarAICommandListAdd(currentCommands, command);
            command = WarAICommandQueuePop(nextCommands);
        }
    }
}

void updateAI(WarContext* context)
{
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    WarPlayerInfo* aiPlayer = &map->players[1];

    updateAICommands(context, aiPlayer);
    if (processAICurrentCommands(context, aiPlayer))
    {
        processAINextCommands(context, aiPlayer);
    }

    // TODO: delete completed requests from current commands
}
