void landAttackInitAI(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)xmalloc(sizeof(WarAICustomData));
    customData->index = 0;

    WarAICommandList* commands = &customData->commands;
    WarAICommandListInit(commands, WarAICommandListDefaultOptions);

    WarAICommandFlagsMap* commandFlags = &customData->commandFlags;
    WarAICommandFlagsMapInit(commandFlags, WarAICommandFlagsMapDefaultOptions);

    WarAICommandResultsMap* commandResults = &customData->commandResults;
    WarAICommandResultsMapInit(commandResults, WarAICommandResultsMapDefaultOptions);

    WarUnitType townHallType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, aiPlayer->race);
    WarAICommand* townHallRequest = createRequestAICommand(context, aiPlayer, townHallType, true);
    WarAICommandListAdd(commands, townHallRequest);
    // WarAICommandFlagsSet(commandFlags, townHallRequest->id, WAR_AI_COMMAND_FLAG_WAIT);

    WarUnitType workerType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    WarAICommand* workerRequest = createRequestAICommand(context, aiPlayer, workerType, false);
    WarAICommandListAdd(commands, workerRequest);
    WarAICommandFlagsMapSet(commandFlags, workerRequest->id, WAR_AI_COMMAND_FLAG_WAIT);

    WarUnitType farmType = getUnitTypeForRace(WAR_UNIT_FARM_HUMANS, aiPlayer->race);
    WarAICommand* farmRequest = createRequestAICommand(context, aiPlayer, farmType, false);
    WarAICommandListAdd(commands, farmRequest);
    WarAICommandFlagsMapSet(commandFlags, farmRequest->id, WAR_AI_COMMAND_FLAG_WAIT);

    WarAICommandListAdd(commands, createResourceAICommand(context, aiPlayer, WAR_RESOURCE_GOLD, 1, true));

    // workerType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    // workerRequest = createRequestAICommand(context, aiPlayer, workerType, false);
    // WarAICommandListAdd(commands, workerRequest);
    // WarAICommandFlagsMapSet(commandFlags, workerRequest->id, WAR_AI_COMMAND_FLAG_WAIT);

    // WarAICommandListAdd(commands, createResourceAICommand(context, aiPlayer, WAR_RESOURCE_WOOD, 1, true));

    // TODO: create a WAIT_FOR_GOLD and WAIT_FOR_WOOD command
    // should it be a WAIT_FOR_RESOURCE command that takes a resources kind and amount?

    ai->customData = customData;
}

WarAICommand* landAttackGetAICommand(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    WarAICommandList* commands = &customData->commands;
    WarAICommandFlagsMap* commandFlags = &customData->commandFlags;
    WarAICommandResultsMap* commandResults = &customData->commandResults;

    if (customData->index < commands->count)
    {
        if (customData->index > 0)
        {
            WarAICommand* lastCommand = commands->items[customData->index - 1];
            if (WarAICommandFlagsMapContains(commandFlags, lastCommand->id))
            {
                WarAICommandFlags flags = WarAICommandFlagsMapGet(commandFlags, lastCommand->id);
                if ((flags & WAR_AI_COMMAND_FLAG_WAIT) == WAR_AI_COMMAND_FLAG_WAIT)
                {
                    if (lastCommand->type == WAR_AI_COMMAND_REQUEST)
                    {
                        if (WarAICommandResultsMapContains(commandResults, lastCommand->id))
                        {
                            WarAICommandResult* lastCommandResult = WarAICommandResultsMapGet(commandResults, lastCommand->id);

                            WarUnitType unitType = lastCommand->request.unitType;
                            WarEntityId buildingId = lastCommandResult->request.buildingId;
                            WarEntityId workerId = lastCommandResult->request.workerId;

                            if (isDudeUnitType(unitType))
                            {
                                WarEntity* building = findEntity(context, buildingId);
                                if (building)
                                {
                                    if (isTraining(building) || isGoingToTrain(building))
                                    {
                                        // building is still training
                                        return NULL;
                                    }
                                }
                            }
                            else if (isBuildingUnitType(unitType))
                            {
                                WarEntity* building = findEntity(context, buildingId);
                                if (building)
                                {
                                    if (isBuilding(building) || isGoingToBuild(building))
                                    {
                                        logInfo("building is still being built\n");
                                        // building is still being built
                                        return NULL;
                                    }
                                }

                                logInfo("finding worker %d\n", workerId);
                                WarEntity* worker = findEntity(context, workerId);

                                if (worker)
                                {
                                    logInfo("worker found\n");

                                    if (isRepairing2(worker) || isGoingToRepair2(worker))
                                    {
                                        logInfo("worker is still repairing\n");
                                        // worker is still repairing
                                        return NULL;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        logInfo("returning command %d of type %d\n", commands->items[customData->index]->id, commands->items[customData->index]->type);
        return commands->items[customData->index++];
    }

    return createSleepAICommand(context, aiPlayer, 10);
}

void landAttackExecutedAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command, WarAICommandResult* result)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    WarAICommandResultsMap* commandResults = &customData->commandResults;

    if (result)
    {
        WarAICommandResultsMapSet(commandResults, command->id, result);
    }
}
