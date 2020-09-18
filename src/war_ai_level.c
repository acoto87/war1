WarAISystem* createAISystem(WarAISystemType type)
{
    WarAISystem* system = (WarAISystem*)xcalloc(1, sizeof(WarAISystem));
    system->type = type;
    system->enabled = false;
    return system;
}

WarAISystem* createAITrainSystem()
{
    WarAISystem* system = createAISystem(WAR_AI_SYSTEM_TRAIN);
    return system;
}

WarAISystem* createAIBuildSystem()
{
    WarAISystem* system = createAISystem(WAR_AI_SYSTEM_BUILD);
    return system;
}

WarAISystem* createAIPlanningSystem()
{
    WarAISystem* system = createAISystem(WAR_AI_SYSTEM_PLANNING);
    system->planning.index = 0;
    WPPlanInit(&system->planning.simplifiedPlan, WPPlanDefaultOptions);
    return system;
}

WarAISystem* createAIResourceSystem()
{
    WarAISystem* system = createAISystem(WAR_AI_SYSTEM_RESOURCE);
    return system;
}

void levelInitAI(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarCommandQueueInit(&ai->commands, WarCommandQueueDefaultOptions);

    WarAISystemsMapInit(&ai->systems, WarAISystemsMapDefaultOptions);
    WarAISystemsMapSet(&ai->systems, WAR_AI_SYSTEM_TRAIN, createAITrainSystem());
    WarAISystemsMapSet(&ai->systems, WAR_AI_SYSTEM_BUILD, createAITrainSystem());
    WarAISystemsMapSet(&ai->systems, WAR_AI_SYSTEM_PLANNING, createAIPlanningSystem());
    WarAISystemsMapSet(&ai->systems, WAR_AI_SYSTEM_RESOURCE, createAIResourceSystem());

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

s32 idleWorkersCompare(const WarEntity* e1, const WarEntity* e2)
{
    assert(isUnit(e1));
    assert(isUnit(e2));

    s32 idle1 = isIdle((WarEntity*)e1);
    s32 idle2 = isIdle((WarEntity*)e2);

    // NOTE: should this consider distance, because if a idle worker is to far
    // maybe the AI could prefer to change one busy worker over a far idle one
    return idle2 - idle1;
}

bool findPlaceToBuild(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, vec2 startPosition, vec2* finalPosition)
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

        if (canBuildingBeBuilt(context, aiPlayer, unitType, position.x, position.y))
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

void updatePlanningSystem(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAISystem* planningSystem = WarAISystemsMapGet(&ai->systems, WAR_AI_SYSTEM_PLANNING);
    WarAISystem* trainSystem = WarAISystemsMapGet(&ai->systems, WAR_AI_SYSTEM_TRAIN);
    WarAISystem* buildSystem = WarAISystemsMapGet(&ai->systems, WAR_AI_SYSTEM_BUILD);

    u32 resources[WP_RESOURCE_TYPE_COUNT] = {0};
    resources[WP_RESOURCE_TYPE_GOLD] = aiPlayer->gold;
    resources[WP_RESOURCE_TYPE_WOOD] = aiPlayer->wood;

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            WarUnitComponent* unit = &entity->unit;
            if (unit->player != aiPlayer->index)
                continue;

            if (isDeadUnit(entity) || isCollapsedUnit(entity))
                continue;

            if (isWorkerUnit(entity) && isWorkerBusy(entity))
                continue;

            if (isBuildingUnit(entity) && (isBeingBuiltUnit(entity) || isTrainingUnit(entity) || isUpgradingUnit(entity)))
                continue;

            switch (unit->type)
            {
                case WAR_UNIT_FOOTMAN:
                case WAR_UNIT_GRUNT:
                {
                    resources[WP_RESOURCE_TYPE_FOOTMAN_GRUNT]++;
                    break;
                }
                case WAR_UNIT_PEASANT:
                case WAR_UNIT_PEON:
                {
                    resources[WP_RESOURCE_TYPE_PEASANT_PEON]++;
                    break;
                }
                case WAR_UNIT_CATAPULT_HUMANS:
                case WAR_UNIT_CATAPULT_ORCS:
                {
                    resources[WP_RESOURCE_TYPE_CATAPULT]++;
                    break;
                }
                case WAR_UNIT_KNIGHT:
                case WAR_UNIT_RAIDER:
                {
                    resources[WP_RESOURCE_TYPE_KNIGHT_RAIDER]++;
                    break;
                }
                case WAR_UNIT_ARCHER:
                case WAR_UNIT_SPEARMAN:
                {
                    resources[WP_RESOURCE_TYPE_ARCHER_SPEARMAN]++;
                    break;
                }
                case WAR_UNIT_CONJURER:
                case WAR_UNIT_WARLOCK:
                {
                    resources[WP_RESOURCE_TYPE_CONJURER_WARLOCK]++;
                    break;
                }
                case WAR_UNIT_CLERIC:
                case WAR_UNIT_NECROLYTE:
                {
                    resources[WP_RESOURCE_TYPE_CLERIC_NECROLYTE]++;
                    break;
                }

                // buildings
                case WAR_UNIT_FARM_HUMANS:
                case WAR_UNIT_FARM_ORCS:
                {
                    resources[WP_RESOURCE_TYPE_SUPPLY] += 4;
                    break;
                }
                case WAR_UNIT_BARRACKS_HUMANS:
                case WAR_UNIT_BARRACKS_ORCS:
                {
                    resources[WP_RESOURCE_TYPE_BARRACKS]++;
                    break;
                }
                case WAR_UNIT_CHURCH:
                case WAR_UNIT_TEMPLE:
                {
                    resources[WP_RESOURCE_TYPE_CHURCH_TEMPLE]++;
                    break;
                }
                case WAR_UNIT_TOWER_HUMANS:
                case WAR_UNIT_TOWER_ORCS:
                {
                    resources[WP_RESOURCE_TYPE_TOWER]++;
                    break;
                }
                case WAR_UNIT_TOWNHALL_HUMANS:
                case WAR_UNIT_TOWNHALL_ORCS:
                {
                    resources[WP_RESOURCE_TYPE_TOWNHALL]++;
                    resources[WP_RESOURCE_TYPE_SUPPLY]++;
                    break;
                }
                case WAR_UNIT_LUMBERMILL_HUMANS:
                case WAR_UNIT_LUMBERMILL_ORCS:
                {
                    resources[WP_RESOURCE_TYPE_LUMBERMILL]++;
                    break;
                }
                case WAR_UNIT_STABLE:
                case WAR_UNIT_KENNEL:
                {
                    resources[WP_RESOURCE_TYPE_STABLE_KENNEL]++;
                    break;
                }
                case WAR_UNIT_BLACKSMITH_HUMANS:
                case WAR_UNIT_BLACKSMITH_ORCS:
                {
                    resources[WP_RESOURCE_TYPE_BLACKSMITH]++;
                    break;
                }
                default:
                    break;
            }
        }
    }

    // Remove the supplies that are currently being consumed
    for (s32 i = WP_RESOURCE_TYPE_PEASANT_PEON; i < WP_RESOURCE_TYPE_COUNT; i++)
        resources[WP_RESOURCE_TYPE_SUPPLY] -= resources[i];

    WPGameState gameState = wpCreateGameState(0, resources);

    s32 index = planningSystem->planning.index;
    WPPlan* simplifiedPlan = &planningSystem->planning.simplifiedPlan;
    if (simplifiedPlan->count == 0)
    {
        WPGoal goal = wpCreateGoalFromArgs(
            4,
            WP_RESOURCE_TYPE_FOOTMAN_GRUNT, 5,
            WP_RESOURCE_TYPE_ARCHER_SPEARMAN, 3,
            WP_RESOURCE_TYPE_KNIGHT_RAIDER, 3,
            WP_RESOURCE_TYPE_CATAPULT, 2

            // WP_RESOURCE_TYPE_CONJURER_WARLOCK,
            // WP_RESOURCE_TYPE_CLERIC_NECROLYTE,
        );

        WPGoal intermediateGoals[] =
        {
            wpCreateGoalFromArgs(
                1,
                WP_RESOURCE_TYPE_PEASANT_PEON, 5
            )
        };

        index = 0;

        WPPlan plan;
        WPPlanInit(&plan, WPPlanDefaultOptions);

        wpBestPlan(&gameState, &goal, arrayLength(intermediateGoals), intermediateGoals, &plan);
        wpSimplifyPlan(&plan, simplifiedPlan);
        if (simplifiedPlan->count == 0)
        {
            logWarning("The planning system couldn't find any suitable actions for the current state.\n");
            return;
        }

        logInfo("The plan was calculated as:\n");
        printPlan(simplifiedPlan);

        WPPlanFree(&plan);
    }

    if (index >= simplifiedPlan->count)
        return;

    WPAction* action = &simplifiedPlan->items[index];
    if (wpCanExecuteAction(&gameState, action))
    {
        logInfo("Executing action of type: %s, %d\n", actionTypeToString(action->type), index);

        switch (action->type)
        {
            case WP_ACTION_TYPE_BUILD_SUPPLY:
            {
                index++;
                buildSystem->enabled = true;
                buildSystem->build.unitToBuild = getUnitTypeForRace(WAR_UNIT_FARM_HUMANS, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_BUILD_TOWNHALL:
            {
                index++;
                buildSystem->enabled = true;
                buildSystem->build.unitToBuild = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_BUILD_BARRACKS:
            {
                index++;
                buildSystem->enabled = true;
                buildSystem->build.unitToBuild = getUnitTypeForRace(WAR_UNIT_BARRACKS_HUMANS, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_BUILD_CHURCH_TEMPLE:
            {
                index++;
                buildSystem->enabled = true;
                buildSystem->build.unitToBuild = getUnitTypeForRace(WAR_UNIT_CHURCH, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_BUILD_TOWER:
            {
                index++;
                buildSystem->enabled = true;
                buildSystem->build.unitToBuild = getUnitTypeForRace(WAR_UNIT_TOWER_HUMANS, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_BUILD_STABLE_KENNEL:
            {
                index++;
                buildSystem->enabled = true;
                buildSystem->build.unitToBuild = getUnitTypeForRace(WAR_UNIT_STABLE, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_BUILD_LUMBERMILL:
            {
                index++;
                buildSystem->enabled = true;
                buildSystem->build.unitToBuild = getUnitTypeForRace(WAR_UNIT_LUMBERMILL_HUMANS, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_BUILD_BLACKSMITH:
            {
                index++;
                buildSystem->enabled = true;
                buildSystem->build.unitToBuild = getUnitTypeForRace(WAR_UNIT_BLACKSMITH_HUMANS, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_TRAIN_PEASANT_PEON:
            {
                index++;
                trainSystem->enabled = true;
                trainSystem->train.unitToTrain = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_TRAIN_FOOTMAN_GRUNT:
            {
                index++;
                trainSystem->enabled = true;
                trainSystem->train.unitToTrain = getUnitTypeForRace(WAR_UNIT_FOOTMAN, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_TRAIN_ARCHER_SPEARMAN:
            {
                index++;
                trainSystem->enabled = true;
                trainSystem->train.unitToTrain = getUnitTypeForRace(WAR_UNIT_ARCHER, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_TRAIN_KNIGHT_RAIDER:
            {
                index++;
                trainSystem->enabled = true;
                trainSystem->train.unitToTrain = getUnitTypeForRace(WAR_UNIT_KNIGHT, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_TRAIN_CATAPULT:
            {
                index++;
                trainSystem->enabled = true;
                trainSystem->train.unitToTrain = getUnitTypeForRace(WAR_UNIT_CATAPULT_HUMANS, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_TRAIN_CONJURER_WARLOCK:
            {
                index++;
                trainSystem->enabled = true;
                trainSystem->train.unitToTrain = getUnitTypeForRace(WAR_UNIT_CONJURER, aiPlayer->race);
                break;
            }
            case WP_ACTION_TYPE_TRAIN_CLERIC_NECROLYTE:
            {
                index++;
                trainSystem->enabled = true;
                trainSystem->train.unitToTrain = getUnitTypeForRace(WAR_UNIT_CLERIC, aiPlayer->race);
                break;
            }
            default:
                break;
        }

        planningSystem->planning.index = index;
    }
}

void updateResourceSystem(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAISystem* system = WarAISystemsMapGet(&ai->systems, WAR_AI_SYSTEM_RESOURCE);
    assert(system);

    // if (!system->enabled)
    //     return;

    WarUnitType workerType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    WarEntityList* workers = getUnitsOfTypeOfPlayer(context, workerType, aiPlayer->index);

    WarEntityListSort(workers, idleWorkersCompare);

    s32 gatheringGold = 0;
    s32 gatheringWood = 0;
    s32 idle = 0;

    WarEntityIdList idleIds;
    WarEntityIdListInit(&idleIds, WarEntityIdListDefaultOptions);

    for (s32 i = 0; i < workers->count; i++)
    {
        WarEntity* worker = workers->items[i];
        if (!worker || isDeadUnit(worker))
            continue;

        if (isGatheringGold(worker) || isMining(worker) || isDeliveringGold(worker))
            gatheringGold++;
        else if (isGatheringWood(worker) || isChopping(worker) || isDeliveringWood(worker))
            gatheringWood++;
        else if (isIdle(worker))
        {
            idle++;
            WarEntityIdListAdd(&idleIds, worker->id);
        }
    }

    if (idle > 0)
    {
        // 1 worker(s) -> 1 worker(s) to gold
        // 2 worker(s) -> 2 worker(s) to gold
        // 3 worker(s) -> 3 worker(s) to gold
        // 4 worker(s) -> 3 worker(s) to gold, 1 worker(s) to wood
        // 5 worker(s) -> 3 worker(s) to gold, 2 worker(s) to wood
        // 6 worker(s) -> 4 worker(s) to gold, 2 worker(s) to wood
        // 7 worker(s) -> 4 worker(s) to gold, 3 worker(s) to wood
        // 8 worker(s) -> 5 worker(s) to gold, 3 worker(s) to wood
        //
        static s32 toGatherGoldTable[9] = { 0, 1, 2, 3, 3, 3, 4, 4, 5 };
        static s32 toGatherWoodTable[9] = { 0, 0, 0, 0, 1, 2, 2, 3, 3 };

        s32 toGatherGold = 0;
        s32 toGatherWood = 0;

        if (workers->count > 8)
        {
            toGatherWood += idle;
        }
        else
        {
            toGatherGold = min(idle, toGatherGoldTable[workers->count] - gatheringGold);
            idle -= toGatherGold;

            toGatherWood = min(idle, toGatherWoodTable[workers->count] - gatheringWood);
            idle -= toGatherWood;
        }

        assert(idle == 0);

        WarEntityType townHallType = getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, aiPlayer->race);
        WarEntityList* townHalls = getUnitsOfTypeOfPlayer(context, townHallType, aiPlayer->index);
        assert(townHalls->count > 0);

        WarEntity* townHall = townHalls->items[0];
        assert(townHall);

        if (toGatherGold > 0)
        {
            WarEntity* goldmine = findClosestUnitOfType(context, townHall, WAR_UNIT_GOLDMINE);
            assert(goldmine);

            while (toGatherGold > 0)
            {
                s32 unitCount = min(toGatherGold, MAX_UNIT_SELECTION_COUNT);
                WarUnitGroup unitGroup = createUnitGroup(unitCount, idleIds.items);

                WarCommand* gatherCommand = createGatherGoldCommand(context, aiPlayer, unitGroup, goldmine->id);
                WarCommandQueuePush(&ai->commands, gatherCommand);

                WarEntityIdListRemoveAtRange(&idleIds, 0, unitCount);
                toGatherGold -= unitCount;
            }
        }

        if (toGatherWood > 0)
        {
            vec2 townHallPosition = getUnitCenterPosition(townHall, true);

            WarEntity* forest = findClosestForest(context, townHall);
            assert(forest);

            WarTree* tree = findAccesibleTree(context, aiPlayer, forest, townHallPosition);
            assert(tree);

            vec2 treeTile = vec2i(tree->tilex, tree->tiley);

            while (toGatherWood > 0)
            {
                s32 unitCount = min(toGatherWood, MAX_UNIT_SELECTION_COUNT);
                WarUnitGroup unitGroup = createUnitGroup(unitCount, idleIds.items);

                WarCommand* gatherCommand = createGatherWoodCommand(context, aiPlayer, unitGroup, forest->id, treeTile);
                WarCommandQueuePush(&ai->commands, gatherCommand);

                WarEntityIdListRemoveAtRange(&idleIds, 0, unitCount);
                toGatherWood -= unitCount;
            }
        }
    }

    WarEntityIdListFree(&idleIds);
    WarEntityListFree(workers);
}

void updateBuildSystem(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAISystem* system = WarAISystemsMapGet(&ai->systems, WAR_AI_SYSTEM_BUILD);
    assert(system);

    if (!system->enabled)
        return;

    WarUnitType unitType = system->train.unitToTrain;

    WarUnitType workerType = getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    WarEntityList* workers = getUnitsOfTypeOfPlayer(context, workerType, aiPlayer->index);

    WarEntityListSort(workers, idleWorkersCompare);

    for (s32 i = 0; i < workers->count; i++)
    {
        WarEntity* worker = workers->items[i];
        if (worker)
        {
            vec2 position = getUnitCenterPosition(worker, true);
            if (findPlaceToBuild(context, aiPlayer, unitType, position, &position))
            {
                WarCommand* command = createBuildCommand(context, aiPlayer, unitType, worker->id, position);
                WarCommandQueuePush(&ai->commands, command);

                system->enabled = false;
                break;
            }
        }
    }

    WarEntityListFree(workers);
}

void updateTrainSystem(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAISystem* system = WarAISystemsMapGet(&ai->systems, WAR_AI_SYSTEM_TRAIN);
    assert(system);


    if (!system->enabled)
        return;

    WarUnitType unitType = system->train.unitToTrain;

    WarUnitType producerType = getUnitTypeProducer(unitType);
    WarEntityList* producers = getUnitsOfTypeOfPlayer(context, producerType, aiPlayer->index);
    for (s32 k = 0; k < producers->count; k++)
    {
        WarEntity* producer = producers->items[k];
        if (producer)
        {
            WarCommand* command = createTrainCommand(context, aiPlayer, unitType, producer->id);
            WarCommandQueuePush(&ai->commands, command);

            system->enabled = false;
            break;
        }
    }
    WarEntityListFree(producers);
}

WarCommand* levelGetAICommand(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    if (ai->commands.count == 0)
    {
        updatePlanningSystem(context, aiPlayer);
        updateBuildSystem(context, aiPlayer);
        updateTrainSystem(context, aiPlayer);
        // updateUpgradesSystem(context, aiPlayer);
        updateResourceSystem(context, aiPlayer);

        // updateSquadsSystem(context, aiPlayer);
    }

    return ai->commands.count > 0
        ? WarCommandQueuePop(&ai->commands)
        : NULL;
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
