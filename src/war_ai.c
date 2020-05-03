WarAI* createAI(WarContext* context)
{
    WarAI* ai = (WarAI*)xmalloc(sizeof(WarAI));
    ai->staticCommandId = 0;
    ai->customData = NULL;

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

    return command;
}

WarAICommand* createUnitRequest(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count)
{
    WarAICommand* request = createAICommand(context, aiPlayer, WAR_AI_COMMAND_REQUEST);
    request->request.unitType = unitType;
    request->request.count = count;
    return request;
}

WarAICommand* createWaitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count)
{
    WarAICommand* wait = createAICommand(context, aiPlayer, WAR_AI_COMMAND_WAIT);
    wait->wait.unitType = unitType;
    wait->wait.count = count;
    return wait;
}

WarAICommand* createSleepForTime(WarContext* context, WarPlayerInfo* aiPlayer, f32 time)
{
    WarAICommand* sleep = createAICommand(context, aiPlayer, WAR_AI_COMMAND_SLEEP);
    sleep->sleep.time = time;
    return sleep;
}

typedef struct
{
    s32 index;
    WarAICommandList commands;
} WarAICustomData;

void initAI(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)xmalloc(sizeof(WarAICustomData));
    customData->index = 0;

    WarAICommandList* commands = &customData->commands;
    WarAICommandListInit(commands, WarAICommandListDefaultOptions);

    WarUnitType townHall = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, aiPlayer->race);
    WarAICommandListAdd(commands, createUnitRequest(context, aiPlayer, townHall, 1));
    WarAICommandListAdd(commands, createWaitForUnit(context, aiPlayer, townHall, 1));

    WarUnitType worker = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    WarAICommandListAdd(commands, createUnitRequest(context, aiPlayer, worker, 2));
    WarAICommandListAdd(commands, createWaitForUnit(context, aiPlayer, worker, 2));
    WarAICommandListAdd(commands, createUnitRequest(context, aiPlayer, worker, 2));
    WarAICommandListAdd(commands, createWaitForUnit(context, aiPlayer, worker, 4));

    ai->customData = customData;
}

WarAICommand* getNextAICommand(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    if (customData->index < customData->commands.count)
    {
        return customData->commands.items[customData->index++];
    }

    return createSleepForTime(context, aiPlayer, 10);
}

void initAIPlayer(WarContext* context, WarPlayerInfo* aiPlayer)
{
    aiPlayer->ai = createAI(context);
    initAI(context, aiPlayer);
}

void initAIPlayers(WarContext* context)
{
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    initAIPlayer(context, &map->players[1]);
}

bool tryCreateUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType)
{
    if (isDudeUnitType(unitType))
    {
        WarUnitStats stats = getUnitStats(unitType);
        if (!enoughPlayerResources(context, aiPlayer, stats.goldCost, stats.woodCost))
        {
            // there is not enough resources to create the unit
            return false;
        }

        // check if there is enough food
        if (!enoughFarmFood(context, aiPlayer))
        {
            // set a request for supply because there is not enough food
            return false;
        }

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
                        if (decreasePlayerResources(context, aiPlayer, stats.goldCost, stats.woodCost))
                        {
                            WarState* trainState = createTrainState(context, entity, unitType, stats.buildTime);
                            changeNextState(context, entity, trainState, true, true);
                        }

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

    if (tryCreateUnit(context, aiPlayer, command->request.unitType))
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

bool updateAICurrentCommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    bool updateNext = true;

    WarAICommandList* commands = &ai->currentCommands;
    for (s32 i = 0; i < commands->count; i++)
    {
        WarAICommand* command = commands->items[i];
        if (!executeAICommand(context, aiPlayer, command))
        {
            updateNext = false;
        }
    }

    return updateNext;
}

void updateAINextCommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

#define isWaitOrSleep(command) ((command)->type == WAR_AI_COMMAND_WAIT || (command)->type == WAR_AI_COMMAND_SLEEP)

    WarAICommandList* currentCommands = &ai->currentCommands;
    WarAICommandQueue* nextCommands = &ai->nextCommands;
    if (nextCommands->count > 0)
    {
        WarAICommand* command = NULL;
        do
        {
            command = WarAICommandQueuePop(nextCommands);
            WarAICommandListAdd(currentCommands, command);
        } while (nextCommands->count > 0 && !isWaitOrSleep(command));
    }

#undef isWaitOrSleep
}

void removeCompletedAICommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICommandList* commands = &ai->currentCommands;
    for (s32 i = commands->count - 1; i >= 0; i--)
    {
        WarAICommand* command = commands->items[i];
        if (command->status == WAR_AI_COMMAND_STATUS_COMPLETED)
        {
            WarAICommandListRemoveAt(commands, i);
        }
    }
}

void updateAIPlayer(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    if (updateAICurrentCommands(context, aiPlayer))
    {
        WarAICommand* command = getNextAICommand(context, aiPlayer);
        if (command)
        {
            WarAICommandQueuePush(&ai->nextCommands, command);
        }

        updateAINextCommands(context, aiPlayer);
    }

    removeCompletedAICommands(context, aiPlayer);
}


void updateAIPlayers(WarContext* context)
{
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    updateAIPlayer(context, &map->players[1]);
}
