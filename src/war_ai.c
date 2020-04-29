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

    WarAIRequestQueuePush(&ai->nextCommands, command);

    return command;
}

void addUnitRequest(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count)
{
    WarAICommand* request = createAICommand(context, aiPlayer, WAR_AI_COMMAND_REQUEST);
    request->request.status = WAR_REQUEST_STATUS_NOT_STARTED;
    request->request.unitType = unitType;
    request->request.count = count;
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

bool executeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    bool processNext = true;

    switch (command->type)
    {
        case WAR_AI_COMMAND_REQUEST:
        {

            break;
        }

        case WAR_AI_COMMAND_WAIT:
        {
            s32 numberOfUnits = getNumberOfUnitsOfType(context, aiPlayer->index, command->wait.unitType);
            if (numberOfUnits < command->wait.count)
            {
                processNext = false;
            }

            break;
        }

        case WAR_AI_COMMAND_SLEEP:
        {
            if (command->sleep.time > 0)
            {
                command->sleep.time -= context->deltaTime;

                processNext = false;
            }

            break;
        }

        default:
        {
            logWarning("AI commands of type %d are NOT handled yet\n", command->type);
            break;
        }
    }

    return processNext;
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
    addUnitRequest(context, aiPlayer, townHall, 1);
    waitForUnit(context, aiPlayer, townHall, 1);

    WarUnitType worker = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    addUnitRequest(context, aiPlayer, worker, 1);
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
}
