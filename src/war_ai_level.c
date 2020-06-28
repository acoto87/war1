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

    ai->customData = customData;

    requestTownHall(context, aiPlayer, true, true);
    requestPeasantOrPeon(context, aiPlayer);
    waitForUnit(context, aiPlayer, -1);
    sendToGatherGold(context, aiPlayer, 1, true);
    requestPeasantOrPeon(context, aiPlayer);
    waitForUnit(context, aiPlayer, -1);
    sendToGatherWood(context, aiPlayer, 1, true);
    waitForGold(context, aiPlayer, 500);
    waitForWood(context, aiPlayer, 300);
    requestFarm(context, aiPlayer, false, false);
    waitForUnit(context, aiPlayer, -1);
    sendToGatherGold(context, aiPlayer, 1, true);
    waitForGold(context, aiPlayer, 400);
    requestPeasantOrPeon(context, aiPlayer);
    waitForUnit(context, aiPlayer, -1);
    sendToGatherWood(context, aiPlayer, 1, true);
    waitForGold(context, aiPlayer, 600);
    waitForWood(context, aiPlayer, 500);
    requestBarracks(context, aiPlayer, false, false);
    waitForUnit(context, aiPlayer, -1);
    sendToGatherGold(context, aiPlayer, 1, true);
    waitForGold(context, aiPlayer, 400);
    requestFootmanOrGrunt(context, aiPlayer);
    waitForUnit(context, aiPlayer, -1);
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
    else
    {
        logWarning("AI command failed: %d - %d\n", command->id, command->type);

        free(command);
    }
}

WarAICommand* requestUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool checkExisting, bool waitForIdleWorker)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    WarAICommand* command = createRequestUnitAICommand(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
    WarAICommandQueuePush(&customData->commands, command);

    return command;
}

WarAICommand* requestFootmanOrGrunt(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_FOOTMAN, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, false, false);
}

WarAICommand* requestPeasantOrPeon(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, false, false);
}
WarAICommand* requestCatapult(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CATAPULT_HUMANS, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, false, false);
}
WarAICommand* requestKnightOrRider(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_KNIGHT, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, false, false);
}
WarAICommand* requestArcherOrSpearman(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_ARCHER, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, false, false);
}
WarAICommand* requestConjurerOrWarlock(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CONJURER, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, false, false);
}
WarAICommand* requestClericOrNecrolyte(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CLERIC, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, false, false);
}

WarAICommand* requestFarm(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_FARM_HUMANS, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
}

WarAICommand* requestBarracks(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_BARRACKS_HUMANS, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
}

WarAICommand* requestChurchOrTemple(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CHURCH, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
}

WarAICommand* requestTower(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_TOWER_HUMANS, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
}

WarAICommand* requestTownHall(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
}

WarAICommand* requestLumbermill(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_LUMBERMILL_HUMANS, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
}

WarAICommand* requestStableOrKennel(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_STABLE, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
}

WarAICommand* requestBlacksmith(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
{
    WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_BLACKSMITH_HUMANS, aiPlayer->race);
    return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
}

WarAICommand* requestUpgrade(WarContext* context, WarPlayerInfo* aiPlayer, WarUpgradeType upgradeType)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    WarAICommand* command = createRequestUpgradeAICommand(context, aiPlayer, upgradeType);
    WarAICommandQueuePush(&customData->commands, command);

    return command;
}

WarAICommand* requestUpgradeArrowsOrSpears(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_ARROWS, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* requestUpgradeSwordsOrAxes(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_SWORDS, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* requestUpgradeHorsesOrWolves(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_HORSES, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* requestUpgradeScorpionsOrSpiders(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_SCORPIONS, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* requestUpgradeRainOfFireOrPoisonCloud(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_RAIN_OF_FIRE, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* requestUpgradeWaterElementalOrDaemon(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_WATER_ELEMENTAL, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* requestUpgradeHealingOrRaiseDead(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_HEALING, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* requestUpgradeFarSightOrDarkVision(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_FAR_SIGHT, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* requestUpgradeInvisibilityOrUnholyArmor(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_INVISIBILITY, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* requestUpgradeShields(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_SHIELD, aiPlayer->race);
    return requestUpgrade(context, aiPlayer, upgradeType);
}

WarAICommand* sendToGatherResource(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resource, s32 count, bool waitForIdleWorker)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    WarAICommand* command = createResourceAICommand(context, aiPlayer, resource, count, waitForIdleWorker);
    WarAICommandQueuePush(&customData->commands, command);

    return command;
}

WarAICommand* waitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandId commandId)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    if (commandId < 0)
    {
        s32 indexFromEnd = abs(commandId);
        if (indexFromEnd <= customData->commands.count)
        {
            WarAICommand* commandToWaitFor = customData->commands.items[customData->commands.count - indexFromEnd];
            commandId = commandToWaitFor->id;
        }
    }

    WarAICommand* command = createWaitAICommand(context, aiPlayer, commandId, WAR_RESOURCE_NONE, 0);
    WarAICommandQueuePush(&customData->commands, command);

    return command;
}

WarAICommand* waitForResource(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resource, s32 amount)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    WarAICommand* command = createWaitAICommand(context, aiPlayer, 0, resource, amount);
    WarAICommandQueuePush(&customData->commands, command);

    return command;
}
