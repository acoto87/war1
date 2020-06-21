void levelInitAI(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)xmalloc(sizeof(WarAICustomData));
    customData->index = 0;

    WarAICommandQueue* commands = &customData->commands;
    WarAICommandQueueInit(commands, WarAICommandQueueNonFreeOptions);

    WarAICommandList* runningCommands = &customData->runningCommands;
    WarAICommandListInit(runningCommands, WarAICommandListDefaultOptions);

    // create townhall and wait => this can generate wait commands when the create is executed?
    // create worker and wait
    // create farm and wait
    // send worker to mine
    // wait for gold

    // vs

    // create townhall
    // wait for townhall => more explicitly waiting for things, but how to link the wait with the command that will be waited
    // create worker
    // wait for worker
    // create farm
    // wait for farm
    // send worker to mine
    // wait for gold

    // second variant will allow things like:
    //
    // create townhall
    // wait for townhall
    // create worker
    // wait for worker
    // create farm
    // wait for farm
    // send worker to mine
    // send worker to wood
    // create farm
    // wait for everything?

    WarUnitType townHallType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, aiPlayer->race);
    WarAICommand* checkTownHall = createRequestAICommand(context, aiPlayer, townHallType, true, true);
    WarAICommandQueuePush(commands, checkTownHall);

    WarUnitType workerType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    WarAICommand* firstWorker = createRequestAICommand(context, aiPlayer, workerType, false, false);
    WarAICommandQueuePush(commands, firstWorker);

    WarAICommand* waitForFirstWorker = createWaitForCommand(context, aiPlayer, firstWorker->id);
    WarAICommandQueuePush(commands, waitForFirstWorker);

    WarUnitType farmType = getUnitTypeForRace(WAR_UNIT_FARM_HUMANS, aiPlayer->race);
    WarAICommand* firstFarm = createRequestAICommand(context, aiPlayer, farmType, false, true);
    WarAICommandQueuePush(commands, firstFarm);

    WarAICommand* waitForFirstFarm = createWaitForCommand(context, aiPlayer, firstFarm->id);
    WarAICommandQueuePush(commands, waitForFirstFarm);

    WarAICommand* sendToGold = createResourceAICommand(context, aiPlayer, WAR_RESOURCE_GOLD, 1, true);
    WarAICommandQueuePush(commands, sendToGold);

    WarAICommand* waitForGold = createWaitForResource(context, aiPlayer, WAR_RESOURCE_GOLD, 400);
    WarAICommandQueuePush(commands, waitForGold);

    WarAICommand* secondWorker = createRequestAICommand(context, aiPlayer, workerType, false, false);
    WarAICommandQueuePush(commands, secondWorker);

    WarAICommand* waitForSecondWorker = createWaitForCommand(context, aiPlayer, secondWorker->id);
    WarAICommandQueuePush(commands, waitForSecondWorker);

    WarAICommand* sendToWood = createResourceAICommand(context, aiPlayer, WAR_RESOURCE_WOOD, 1, true);
    WarAICommandQueuePush(commands, sendToWood);

    waitForGold = createWaitForResource(context, aiPlayer, WAR_RESOURCE_GOLD, 600);
    WarAICommandQueuePush(commands, waitForGold);

    WarAICommand* waitForWood = createWaitForResource(context, aiPlayer, WAR_RESOURCE_WOOD, 300);
    WarAICommandQueuePush(commands, waitForWood);

    WarAICommand* secondFarm = createRequestAICommand(context, aiPlayer, farmType, false, false);
    WarAICommandQueuePush(commands, secondFarm);

    WarAICommand* waitForSecondFarm = createWaitForCommand(context, aiPlayer, secondFarm->id);
    WarAICommandQueuePush(commands, waitForSecondFarm);

    ai->customData = customData;
}

WarAICommand* levelGetAICommand(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    WarAICommandQueue* commands = &customData->commands;
    WarAICommandList* runningCommands = &customData->runningCommands;

    bool wait = false;

    for (s32 i = 0; i < runningCommands->count; i++)
    {
        WarAICommandStatus status = WAR_AI_COMMAND_STATUS_RUNNING;

        WarAICommand* command = runningCommands->items[i];
        if (command && command->result)
        {
            WarAICommandResult* commandResult = command->result;

            if (command->type == WAR_AI_COMMAND_REQUEST)
            {
                WarUnitType unitType = commandResult->request.unitType;
                WarEntityId buildingId = commandResult->request.buildingId;
                WarEntityId workerId = commandResult->request.workerId;

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
                        WarAICommand* prevCommand = runningCommands->items[k];
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

    // remove all finished commands
    for (s32 i = runningCommands->count - 1; i >= 0; i--)
    {
        WarAICommand* command = runningCommands->items[i];
        if (command && command->result && command->result->status != WAR_AI_COMMAND_STATUS_RUNNING)
        {
            WarAICommandListRemoveAt(runningCommands, i);
        }
    }

    // if at least one wait command is still running,
    // don't return a new command from the list until that command finishes
    if (wait)
    {
        // NOTE: should I return a sleep command here?
        return NULL;
    }

    // here a new command is ready to be processed
    if (commands->count > 0)
    {
        WarAICommand* command = WarAICommandQueuePop(commands);

        if (command)
        {
            logInfo("returning command %d of type %d\n", command->id, command->type);
        }
        else
        {
            logInfo("returning NULL command\n");
        }

        if (command->id == 13)
        {
            int x = 10;
        }

        return command;
    }

    return createSleepAICommand(context, aiPlayer, 10);
}

void levelExecutedAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    if (command->result && command->result->status == WAR_AI_COMMAND_STATUS_RUNNING)
    {
        // NOTE: how to handle failed commands here?
        // reinserting it in the queue?
        // forget about it?
        // check some retry variable inside the command?

        WarAICommandList* runningCommands = &customData->runningCommands;
        WarAICommandListAdd(runningCommands, command);
    }
}
