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

bool checkRunningCommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarCommandList* commands = &aiPlayer->commands;

    bool wait = false;

    for (s32 i = 0; i < commands->count; i++)
    {
        WarCommand* command = commands->items[i];
        assert(command);

        if (command->status == WAR_COMMAND_STATUS_RUNNING)
        {
            WarCommandStatus status = WAR_COMMAND_STATUS_RUNNING;

            if (command->type == WAR_COMMAND_TRAIN)
            {
                WarEntityId buildingId = command->train.buildingId;
                WarEntity* building = findEntity(context, buildingId);
                if (building)
                {
                    if (!isTraining(building) && !isGoingToTrain(building))
                    {
                        status = WAR_COMMAND_STATUS_DONE;
                    }
                }
                else
                {
                    // mark the command failed if the building no longer exists
                    status = WAR_COMMAND_STATUS_FAILED;
                }
            }
            else if (command->type == WAR_COMMAND_BUILD)
            {
                WarEntityId buildingId = command->build.buildingId;
                WarEntity* building = findEntity(context, buildingId);

                WarEntityId workerId = command->build.workerId;
                WarEntity* worker = findEntity(context, workerId);
                if (building && worker)
                {
                    if (!isBuilding(building) && !isGoingToBuild(building) &&
                        !isRepairing2(worker) && !isGoingToRepair2(worker))
                    {
                        status = WAR_COMMAND_STATUS_DONE;
                    }
                }
                else
                {
                    // if the building no longer exists, then mark the command failed
                    status = WAR_COMMAND_STATUS_FAILED;
                }
            }
            else if (command->type == WAR_COMMAND_GATHER)
            {
                // do nothing here? maybe I need to check if there are workers
                // that couldn't be sent to the resource
            }
            else if (command->type == WAR_COMMAND_WAIT)
            {
                if (command->wait.commandId)
                {
                    WarCommand* commandToWaitFor = NULL;
                    for (s32 k = i - 1; k >= 0; k--)
                    {
                        WarCommand* prevCommand = commands->items[k];
                        if (prevCommand && prevCommand->id == command->wait.commandId)
                        {
                            commandToWaitFor = prevCommand;
                            break;
                        }
                    }

                    status = commandToWaitFor && commandToWaitFor->status != WAR_COMMAND_STATUS_RUNNING
                        ? WAR_COMMAND_STATUS_DONE : WAR_COMMAND_STATUS_FAILED;
                }
                else if (command->wait.resource != WAR_RESOURCE_NONE)
                {
                    s32 resourceAmount = getPlayerResourceAmount(aiPlayer, command->wait.resource);
                    if (resourceAmount >= command->wait.amount)
                    {
                        status = WAR_COMMAND_STATUS_DONE;
                    }

                    // what if the AI is waiting for a particular resource and there is no
                    // worker gathering it, maybe fail this command? put some workers to gather the resource?
                }
                else
                {
                    status = WAR_COMMAND_STATUS_DONE;
                }

                if (status == WAR_COMMAND_STATUS_RUNNING)
                {
                    wait = true;
                }
            }

            command->status = status;
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

    // if at least one wait command is still running, don't call the ai get command func
    if (!checkRunningCommands(context, aiPlayer))
    {
        return;
    }

    WarCommand* command = ai->getCommandFunc(context, aiPlayer);
    if (command && command->type != WAR_COMMAND_NONE)
    {
        logInfo("Executing command of type: %s\n", commandTypeToString(command->type));
        command->status = executeCommand(context, aiPlayer, command);
        logInfo("Result is : %s\n", commandStatusToString(command->status));

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
