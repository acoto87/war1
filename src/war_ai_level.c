void levelInitAI(WarContext* context, WarPlayerInfo* aiPlayer)
{
    // WarAI* ai = aiPlayer->ai;
    // assert(ai);

    // // How would look like a system that does something like:
    // //
    // // planningSystem.update()
    // // resourceSystem.update()
    // // researchSystem.update()
    // // trainSystem.update()
    // // buildSystem.update()
    // // squadSystem.update()
    // // scriptSystem.update()

    // WarAICustomData* customData = (WarAICustomData*)xmalloc(sizeof(WarAICustomData));
    // WarCommandQueueInit(&customData->commands, WarCommandQueueNonFreeOptions);
    // ai->customData = customData;

    // requestTownHall(context, aiPlayer, true, true);

    // requestPeasantOrPeon(context, aiPlayer);
    // waitForUnit(context, aiPlayer, -1);
    // sendToGatherGold(context, aiPlayer, 1, true);
    // requestPeasantOrPeon(context, aiPlayer);
    // waitForUnit(context, aiPlayer, -1);
    // sendToGatherWood(context, aiPlayer, 1, true);
    // waitForGold(context, aiPlayer, 500);
    // waitForWood(context, aiPlayer, 300);
    // requestFarm(context, aiPlayer, false, false);
    // waitForUnit(context, aiPlayer, -1);
    // sendToGatherGold(context, aiPlayer, 1, true);
    // waitForGold(context, aiPlayer, 400);
    // requestPeasantOrPeon(context, aiPlayer);
    // waitForUnit(context, aiPlayer, -1);
    // sendToGatherWood(context, aiPlayer, 1, true);
    // waitForGold(context, aiPlayer, 600);
    // waitForWood(context, aiPlayer, 500);
    // requestBarracks(context, aiPlayer, false, false);
    // waitForUnit(context, aiPlayer, -1);
    // sendToGatherGold(context, aiPlayer, 1, true);

    // waitForGold(context, aiPlayer, 400);
    // requestFootmanOrGrunt(context, aiPlayer);
    // waitForUnit(context, aiPlayer, -1);

    // waitForGold(context, aiPlayer, 400);
    // requestFootmanOrGrunt(context, aiPlayer);
    // waitForUnit(context, aiPlayer, -1);

    // waitForGold(context, aiPlayer, 400);
    // requestFootmanOrGrunt(context, aiPlayer);
    // waitForUnit(context, aiPlayer, -1);

    // requestSquad(context, aiPlayer, 1, 1, arrayArg(WarSquadUnitRequest, squadFootmanOrGruntRequest(aiPlayer, 3)));
    // attackWithSquad(context, aiPlayer, 1);
}

WarCommand* levelGetAICommand(WarContext* context, WarPlayerInfo* aiPlayer)
{
    // WarAI* ai = aiPlayer->ai;
    // assert(ai);

    // WarAICustomData* customData = (WarAICustomData*)ai->customData;
    // assert(customData);

    // WarCommandQueue* commands = &customData->commands;

    // // here a new command is ready to be processed
    // if (commands->count > 0)
    // {
    //     WarCommand* command = WarCommandQueuePop(commands);

    //     if (command)
    //     {
    //         logInfo("returning command %d of type %d\n", command->id, command->type);
    //     }
    //     else
    //     {
    //         logInfo("returning NULL command\n");
    //     }

    //     return command;
    // }

    // return createSleepAICommand(context, aiPlayer, 10);

    return NULL;
}

// WarCommand* requestUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool checkExisting, bool waitForIdleWorker)
// {
//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarAICustomData* customData = (WarAICustomData*)ai->customData;
//     assert(customData);

//     WarCommand* command = createRequestUnitAICommand(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
//     WarCommandQueuePush(&customData->commands, command);

//     return command;
// }

// WarCommand* requestFootmanOrGrunt(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_FOOTMAN, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, false, false);
// }

// WarCommand* requestPeasantOrPeon(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, false, false);
// }
// WarCommand* requestCatapult(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CATAPULT_HUMANS, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, false, false);
// }
// WarCommand* requestKnightOrRider(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_KNIGHT, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, false, false);
// }
// WarCommand* requestArcherOrSpearman(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_ARCHER, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, false, false);
// }
// WarCommand* requestConjurerOrWarlock(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CONJURER, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, false, false);
// }
// WarCommand* requestClericOrNecrolyte(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CLERIC, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, false, false);
// }

// WarCommand* requestFarm(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_FARM_HUMANS, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
// }

// WarCommand* requestBarracks(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_BARRACKS_HUMANS, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
// }

// WarCommand* requestChurchOrTemple(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CHURCH, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
// }

// WarCommand* requestTower(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_TOWER_HUMANS, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
// }

// WarCommand* requestTownHall(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
// }

// WarCommand* requestLumbermill(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_LUMBERMILL_HUMANS, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
// }

// WarCommand* requestStableOrKennel(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_STABLE, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
// }

// WarCommand* requestBlacksmith(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_BLACKSMITH_HUMANS, aiPlayer->race);
//     return requestUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
// }

// WarCommand* requestUpgrade(WarContext* context, WarPlayerInfo* aiPlayer, WarUpgradeType upgradeType)
// {
//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarAICustomData* customData = (WarAICustomData*)ai->customData;
//     assert(customData);

//     WarCommand* command = createRequestUpgradeAICommand(context, aiPlayer, upgradeType);
//     WarCommandQueuePush(&customData->commands, command);

//     return command;
// }

// WarCommand* requestUpgradeArrowsOrSpears(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_ARROWS, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* requestUpgradeSwordsOrAxes(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_SWORDS, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* requestUpgradeHorsesOrWolves(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_HORSES, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* requestUpgradeScorpionsOrSpiders(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_SCORPIONS, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* requestUpgradeRainOfFireOrPoisonCloud(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_RAIN_OF_FIRE, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* requestUpgradeWaterElementalOrDaemon(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_WATER_ELEMENTAL, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* requestUpgradeHealingOrRaiseDead(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_HEALING, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* requestUpgradeFarSightOrDarkVision(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_FAR_SIGHT, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* requestUpgradeInvisibilityOrUnholyArmor(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_INVISIBILITY, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* requestUpgradeShields(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarUpgradeType upgradeType = getUpgradeTypeForRace(WAR_UPGRADE_SHIELD, aiPlayer->race);
//     return requestUpgrade(context, aiPlayer, upgradeType);
// }

// WarCommand* sendToGatherResource(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resource, s32 count, bool waitForIdleWorker)
// {
//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarAICustomData* customData = (WarAICustomData*)ai->customData;
//     assert(customData);

//     WarCommand* command = createResourceAICommand(context, aiPlayer, resource, count, waitForIdleWorker);
//     WarCommandQueuePush(&customData->commands, command);

//     return command;
// }

// WarCommand* waitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarCommandId commandId)
// {
//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarAICustomData* customData = (WarAICustomData*)ai->customData;
//     assert(customData);

//     if (commandId < 0)
//     {
//         s32 indexFromEnd = abs(commandId);
//         if (indexFromEnd <= customData->commands.count)
//         {
//             WarCommand* commandToWaitFor = customData->commands.items[customData->commands.count - indexFromEnd];
//             commandId = commandToWaitFor->id;
//         }
//     }

//     WarCommand* command = createWaitAICommand(context, aiPlayer, commandId, WAR_RESOURCE_NONE, 0);
//     WarCommandQueuePush(&customData->commands, command);

//     return command;
// }

// WarCommand* waitForResource(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resource, s32 amount)
// {
//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarAICustomData* customData = (WarAICustomData*)ai->customData;
//     assert(customData);

//     WarCommand* command = createWaitAICommand(context, aiPlayer, 0, resource, amount);
//     WarCommandQueuePush(&customData->commands, command);

//     return command;
// }

// WarCommand* requestSquad(WarContext* context, WarPlayerInfo* aiPlayer, WarSquadId id, s32 count, WarSquadUnitRequest requests[])
// {
//     assert(inRange(id, 0, MAX_SQUAD_COUNT));

//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarAICustomData* customData = (WarAICustomData*)ai->customData;
//     assert(customData);

//     WarCommand* command = createSquadAICommand(context, aiPlayer, id, count, requests);
//     WarCommandQueuePush(&customData->commands, command);

//     return command;
// }

// WarSquadUnitRequest createSquadUnitRequest(WarUnitType unitType, s32 count)
// {
//     return (WarSquadUnitRequest){unitType, count};
// }

// WarSquadUnitRequest squadFootmanOrGruntRequest(WarPlayerInfo* aiPlayer, s32 count)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_FOOTMAN, aiPlayer->race);
//     return createSquadUnitRequest(unitType, count);
// }

// WarSquadUnitRequest squadCatapultRequest(WarPlayerInfo* aiPlayer, s32 count)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CATAPULT_HUMANS, aiPlayer->race);
//     return createSquadUnitRequest(unitType, count);
// }

// WarSquadUnitRequest squadKnightOrRiderRequest(WarPlayerInfo* aiPlayer, s32 count)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_KNIGHT, aiPlayer->race);
//     return createSquadUnitRequest(unitType, count);
// }

// WarSquadUnitRequest squadArcherOrSpearmanRequest(WarPlayerInfo* aiPlayer, s32 count)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_ARCHER, aiPlayer->race);
//     return createSquadUnitRequest(unitType, count);
// }

// WarSquadUnitRequest squadConjurerOrWarlockRequest(WarPlayerInfo* aiPlayer, s32 count)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CONJURER, aiPlayer->race);
//     return createSquadUnitRequest(unitType, count);
// }

// WarSquadUnitRequest squadClericOrNecrolyteRequest(WarPlayerInfo* aiPlayer, s32 count)
// {
//     WarUnitType unitType = getUnitTypeForRace(WAR_UNIT_CLERIC, aiPlayer->race);
//     return createSquadUnitRequest(unitType, count);
// }

// WarCommand* attackWithSquad(WarContext* context, WarPlayerInfo* aiPlayer, WarSquadId id)
// {
//     assert(inRange(id, 0, MAX_SQUAD_COUNT));

//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarAICustomData* customData = (WarAICustomData*)ai->customData;
//     assert(customData);

//     WarCommand* command = createAttackAICommand(context, aiPlayer, id);
//     WarCommandQueuePush(&customData->commands, command);

//     return command;
// }
