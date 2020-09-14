const WPActionMetadata actions[] =
{
    {
        WP_ACTION_TYPE_COLLECT_GOLD, 300,
        .require = {
            [WP_RESOURCE_TYPE_TOWNHALL] = 1
        },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = { 0 },
        .produce = {
            [WP_RESOURCE_TYPE_GOLD] = 100
        }
    },
    {
        WP_ACTION_TYPE_COLLECT_WOOD, 1200,
        .require = {
            [WP_RESOURCE_TYPE_TOWNHALL] = 1
        },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = { 0 },
        .produce = {
            [WP_RESOURCE_TYPE_WOOD] = 100
        }
    },
    {
        WP_ACTION_TYPE_BUILD_SUPPLY, 1000,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 500,
            [WP_RESOURCE_TYPE_WOOD] = 300
        },
        .produce = {
            [WP_RESOURCE_TYPE_SUPPLY] = 4
        }
    },
    {
        WP_ACTION_TYPE_BUILD_TOWNHALL, 1000,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 400,
            [WP_RESOURCE_TYPE_WOOD] = 400
        },
        .produce = {
            [WP_RESOURCE_TYPE_TOWNHALL] = 1
        }
    },
    {
        WP_ACTION_TYPE_BUILD_BARRACKS, 1500,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 600,
            [WP_RESOURCE_TYPE_WOOD] = 500
        },
        .produce = {
            [WP_RESOURCE_TYPE_BARRACKS] = 1
        }
    },
    {
        WP_ACTION_TYPE_BUILD_CHURCH_TEMPLE, 2000,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 800,
            [WP_RESOURCE_TYPE_WOOD] = 500
        },
        .produce = {
            [WP_RESOURCE_TYPE_CHURCH_TEMPLE] = 1
        }
    },
    {
        WP_ACTION_TYPE_BUILD_TOWER, 2000,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 1400,
            [WP_RESOURCE_TYPE_WOOD] = 300
        },
        .produce = {
            [WP_RESOURCE_TYPE_TOWER] = 1
        }
    },
    {
        WP_ACTION_TYPE_BUILD_STABLE_KENNEL, 1500,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 1000,
            [WP_RESOURCE_TYPE_WOOD] = 400
        },
        .produce = {
            [WP_RESOURCE_TYPE_STABLE_KENNEL] = 1
        }
    },
    {
        WP_ACTION_TYPE_BUILD_LUMBERMILL, 1500,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 600,
            [WP_RESOURCE_TYPE_WOOD] = 500
        },
        .produce = {
            [WP_RESOURCE_TYPE_LUMBERMILL] = 1
        }
    },
    {
        WP_ACTION_TYPE_BUILD_BLACKSMITH, 1500,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 900,
            [WP_RESOURCE_TYPE_WOOD] = 400
        },
        .produce = {
            [WP_RESOURCE_TYPE_BLACKSMITH] = 1
        }
    },
    {
        WP_ACTION_TYPE_TRAIN_PEASANT_PEON, 750,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_TOWNHALL] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 400,
            [WP_RESOURCE_TYPE_SUPPLY] = 1
        },
        .produce = {
            [WP_RESOURCE_TYPE_PEASANT_PEON] = 1
        }
    },
    {
        WP_ACTION_TYPE_TRAIN_FOOTMAN_GRUNT, 600,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_BARRACKS] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 400,
            [WP_RESOURCE_TYPE_SUPPLY] = 1
        },
        .produce = {
            [WP_RESOURCE_TYPE_FOOTMAN_GRUNT] = 1
        }
    },
    {
        WP_ACTION_TYPE_TRAIN_ARCHER_SPEARMAN, 700,
        .require = {
            [WP_RESOURCE_TYPE_LUMBERMILL] = 1
        },
        .borrow = {
            [WP_RESOURCE_TYPE_BARRACKS] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 450,
            [WP_RESOURCE_TYPE_WOOD] = 50,
            [WP_RESOURCE_TYPE_SUPPLY] = 1
        },
        .produce = {
            [WP_RESOURCE_TYPE_ARCHER_SPEARMAN] = 1
        }
    },
    {
        WP_ACTION_TYPE_TRAIN_KNIGHT_RAIDER, 800,
        .require = {
            [WP_RESOURCE_TYPE_STABLE_KENNEL] = 1
        },
        .borrow = {
            [WP_RESOURCE_TYPE_BARRACKS] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 850,
            [WP_RESOURCE_TYPE_SUPPLY] = 1
        },
        .produce = {
            [WP_RESOURCE_TYPE_KNIGHT_RAIDER] = 1
        }
    },
    {
        WP_ACTION_TYPE_TRAIN_CATAPULT, 1000,
        .require = {
            [WP_RESOURCE_TYPE_BLACKSMITH] = 1
        },
        .borrow = {
            [WP_RESOURCE_TYPE_BARRACKS] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 900,
            [WP_RESOURCE_TYPE_WOOD] = 200,
            [WP_RESOURCE_TYPE_SUPPLY] = 1
        },
        .produce = {
            [WP_RESOURCE_TYPE_CATAPULT] = 1
        }
    },
    {
        WP_ACTION_TYPE_TRAIN_CONJURER_WARLOCK, 900,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_CHURCH_TEMPLE] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 900,
            [WP_RESOURCE_TYPE_SUPPLY] = 1
        },
        .produce = {
            [WP_RESOURCE_TYPE_CONJURER_WARLOCK] = 1
        }
    },
    {
        WP_ACTION_TYPE_TRAIN_CLERIC_NECROLYTE, 800,
        .require = { 0 },
        .borrow = {
            [WP_RESOURCE_TYPE_TOWER] = 1
        },
        .consume = {
            [WP_RESOURCE_TYPE_GOLD] = 700,
            [WP_RESOURCE_TYPE_SUPPLY] = 1
        },
        .produce = {
            [WP_RESOURCE_TYPE_CLERIC_NECROLYTE] = 1
        }
    }
};

WPActionMetadata wpGetActionMetadataThatProduces(WPResourceType type)
{
    for (int i = 0; i < arrayLength(actions); i++)
    {
        if (actions[i].produce[(u32)type] > 0)
            return actions[i];
    }

    // it should never reach here
    assert(false);
    return WP_INVALID_ACTION_METADATA;
}

WPGameState wpCreateGameState(u32 time, const u32 resources[])
{
    WPGameState result = (WPGameState){0};
    result.time = time;
    memset(result.resources, 0, WP_RESOURCE_TYPE_COUNT * sizeof(WPResourceType));
    memcpy(result.resources, resources, WP_RESOURCE_TYPE_COUNT * sizeof(WPResourceType));
    return result;
}

WPGameState wpCreateGameStateFromArgs(u32 time, s32 resourcesCount, ...)
{
    u32 resources[WP_RESOURCE_TYPE_COUNT] = {0};

    va_list resourceValues;
    va_start(resourceValues, resourcesCount);

    for(s32 i = 0; i < resourcesCount; i++)
    {
        WPResourceType resource = va_arg(resourceValues, WPResourceType);
        u32 value = va_arg(resourceValues, u32);

        resources[(s32)resource] += value;
    }

    va_end(resourceValues);

    return wpCreateGameState(time, resources);
}

WPGoal wpCreateGoal(const u32 resources[])
{
    WPGoal result = (WPGoal){0};
    memset(result.resources, 0, WP_RESOURCE_TYPE_COUNT * sizeof(WPResourceType));
    memcpy(result.resources, resources, WP_RESOURCE_TYPE_COUNT * sizeof(WPResourceType));
    return result;
}

WPGoal wpCreateGoalFromArgs(s32 resourcesCount, ...)
{
    u32 resources[WP_RESOURCE_TYPE_COUNT] = {0};

    va_list resourceValues;
    va_start(resourceValues, resourcesCount);

    for(s32 i = 0; i < resourcesCount; i++)
    {
        WPResourceType resource = va_arg(resourceValues, WPResourceType);
        u32 value = va_arg(resourceValues, u32);

        resources[(s32)resource] += value;
    }

    va_end(resourceValues);

    return wpCreateGoal(resources);
}

WPGoal wpCreateGoalFromAction(const WPActionMetadata* action, u32 repeat)
{
    u32 resources[WP_RESOURCE_TYPE_COUNT] = {0};
    for (s32 i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
    {
        if (action->require[i] > 0)
            resources[i] += action->require[i];
        if (action->borrow[i] > 0)
            resources[i] += action->borrow[i];
        if (action->consume[i] > 0)
            resources[i] += action->consume[i] * repeat;
    }
    return wpCreateGoal(resources);
}

WPAction createAction(WPActionType type, u32 start)
{
    assert(type >= WP_ACTION_TYPE_COLLECT_GOLD && type < WP_ACTION_TYPE_COUNT);

    static s32 staticActionId = 0;

    WPActionMetadata metadata = actions[type];

    WPAction result = (WPAction){0};
    result.id = ++staticActionId;
    result.type = type;
    result.start = start;
    result.end = start + metadata.duration;
    result.duration = metadata.duration;
    return result;
}

void wpUpdateAction(WPAction* action, u32 start)
{
    assert(action->type >= WP_ACTION_TYPE_COLLECT_GOLD && action->type < WP_ACTION_TYPE_COUNT);

    WPActionMetadata metadata = actions[action->type];

    action->start = start;
    action->end = start + metadata.duration;
    action->duration = metadata.duration;
}

u32 wpGetPlanDuration(const WPPlan* plan)
{
    if (plan->count == 0)
        return 0;

    WPAction* first = &plan->items[0];
    WPAction* last = &plan->items[plan->count - 1];
    return last->end - first->start;
}

bool wpCanExecuteAction(const WPGameState* gameState, const WPAction* action)
{
    WPActionMetadata metadata = actions[action->type];

    for (int i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
    {
        if (gameState->resources[i] < metadata.require[i] ||
            gameState->resources[i] < metadata.borrow[i] ||
            gameState->resources[i] < metadata.consume[i])
        {
            return false;
        }
    }

    return true;
}

void wpStartAction(WPGameState* gameState, const WPAction* action)
{
    WPActionMetadata metadata = actions[action->type];

    for (s32 i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
    {
        assert(gameState->resources[i] >= metadata.require[i]);
    }

    for (s32 i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
    {
        assert(gameState->resources[i] >= metadata.borrow[i]);
        gameState->resources[i] -= metadata.borrow[i];
    }

    for (s32 i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
    {
        assert(gameState->resources[i] >= metadata.consume[i]);
        gameState->resources[i] -= metadata.consume[i];
    }
}

void wpEndAction(WPGameState* gameState, const WPAction* action)
{
    WPActionMetadata metadata = actions[action->type];

    for (s32 i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
    {
        gameState->resources[i] += metadata.borrow[i];
        gameState->resources[i] += metadata.produce[i];
    }
}

bool wpSatisfyGoal(const WPGameState* gameState, const WPGoal* goal)
{
    for (s32 i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
    {
        if (gameState->resources[i] < goal->resources[i])
            return false;
    }

    return true;
}

void wpExecutePlan(WPGameState* gameState, const WPPlan* plan, s32 index, u32 endTime)
{
    if (plan->count == 0)
        return;

    u32List times;
    u32ListInit(&times, u32ListDefaultOptions);

    for (int i = 0; i < plan->count; i++)
    {
        u32ListAdd(&times, plan->items[i].start);
        u32ListAdd(&times, plan->items[i].end);
    }

    u32ListSort(&times, compareU32);

    u32 timeIndex = 0;

    endTime = min(endTime, plan->items[plan->count - 1].end);
    while (gameState->time <= endTime)
    {
        u32 nextTime = times.items[timeIndex];
        if (nextTime > endTime)
        {
            gameState->time = endTime;
            break;
        }

        while (timeIndex + 1 < plan->count * 2 && times.items[timeIndex + 1] == nextTime)
            timeIndex++;

        for (int i = index; i < plan->count; i++)
        {
            WPAction* action = &plan->items[i];

            if (action->end == nextTime)
            {
                wpEndAction(gameState, action);
            }

            if (action->start == nextTime)
            {
                wpStartAction(gameState, action);
            }
        }

        gameState->time = nextTime + 1;
        timeIndex++;
    }

    u32ListFree(&times);
}

void wpSchedulePlan(WPPlan* plan, WPGameState gameState)
{
    if (plan->count == 0)
        return;

    u32Heap times;
    u32HeapInit(&times, u32HeapDefaultOptions);
    u32HeapPush(&times, gameState.time);
    while (times.count > 0)
    {
        gameState.time = u32HeapPop(&times);

        for (s32 i = 0; i < plan->count; i++)
        {
            WPAction* action = &plan->items[i];

            if (action->start > gameState.time)
            {
                if (!wpCanExecuteAction(&gameState, action))
                    break;

                wpUpdateAction(action, gameState.time);
            }

            if (action->end == gameState.time)
            {
                wpEndAction(&gameState, action);
            }

            if (action->start == gameState.time)
            {
                wpStartAction(&gameState, action);

                if (!u32HeapContains(&times, action->end))
                {
                    u32HeapPush(&times, action->end);
                }
            }
        }
    }

    u32HeapFree(&times);
}

void wpGroupPlan(const WPPlan* plan, WPGroupedPlan* groupedPlan)
{
    if (plan->count == 0)
        return;

    WPAction action = plan->items[0];
    WPGroupedAction groupedAction = (WPGroupedAction){action.type, 1};

    for (s32 i = 1; i < plan->count; i++)
    {
        action = plan->items[i];

        if (action.type == groupedAction.type)
        {
            groupedAction.count++;
        }
        else
        {
            WPGroupedPlanAdd(groupedPlan, groupedAction);
            groupedAction = (WPGroupedAction){action.type, 1};
        }
    }
}

void wpMEA(WPGameState* gameState, const WPGoal* goal, WPPlan* plan)
{
    if (wpSatisfyGoal(gameState, goal))
        return;

    WPResourceType resource;
    u32 want;
    for (s32 i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
    {
        if (gameState->resources[i] < goal->resources[i])
        {
            resource = (WPResourceType)i;
            want = goal->resources[i];
            break;
        }
    }

    assert(resource >= WP_RESOURCE_TYPE_GOLD && resource < WP_RESOURCE_TYPE_COUNT);
    assert(want > 0);

    WPActionMetadata action = wpGetActionMetadataThatProduces(resource);
    u32 have = gameState->resources[resource];
    u32 produces = action.produce[resource];
    u32 repeat = (u32)ceil((want - have) / (f64)produces);

    WPGoal preGoal = wpCreateGoalFromAction(&action, repeat);
    wpMEA(gameState, &preGoal, plan);

    for (s32 i = 0; i < repeat; i++)
    {
        WPPlanAdd(plan, createAction(action.type, gameState->time + i * action.duration));
    }

    wpExecutePlan(gameState, plan, plan->count - repeat, UINT32_MAX);
    wpMEA(gameState, goal, plan);
}

void wpBestPlan(
    WPGameState* gameState, const WPGoal* goal,
    s32 intermediateGoalsCount, const WPGoal intermediateGoals[],
    WPPlan* plan)
{
    WPGameState initialGameState = *gameState;
    wpMEA(&initialGameState, goal, plan);
    wpSchedulePlan(plan, *gameState);

    u32 planDuration = wpGetPlanDuration(plan);
    printf("Direct plan with duration %d\n", planDuration);

    for (s32 i = 0; i < intermediateGoalsCount; i++)
    {
        WPGoal interGoal = intermediateGoals[i];

        WPGameState interGameState = *gameState;

        WPPlan interPlan;
        WPPlanInit(&interPlan, WPPlanDefaultOptions);
        wpMEA(&interGameState, &interGoal, &interPlan);
        wpMEA(&interGameState, goal, &interPlan);
        wpSchedulePlan(&interPlan, *gameState);

        u32 interPlanDuration = wpGetPlanDuration(&interPlan);
        printf("If using intermediate goal %d with duration %d\n", i, interPlanDuration);
        if (interPlanDuration < planDuration)
        {
            planDuration = interPlanDuration;

            WPPlanClear(plan);
            WPPlanAddRange(plan, interPlan.count, interPlan.items);
        }

        WPPlanFree(&interPlan);
    }
}

char* resourceTypeToString(WPResourceType resourceType)
{
    switch (resourceType)
    {
        case WP_RESOURCE_TYPE_GOLD: return "Gold";
        case WP_RESOURCE_TYPE_WOOD: return "Wood";
        case WP_RESOURCE_TYPE_SUPPLY: return "Supply";
        case WP_RESOURCE_TYPE_TOWNHALL: return "TownHall";
        case WP_RESOURCE_TYPE_BARRACKS: return "Barracks";
        case WP_RESOURCE_TYPE_CHURCH_TEMPLE: return "Church/Temple";
        case WP_RESOURCE_TYPE_TOWER: return "Tower";
        case WP_RESOURCE_TYPE_STABLE_KENNEL: return "Stable/Kennel";
        case WP_RESOURCE_TYPE_LUMBERMILL: return "Lumbermill";
        case WP_RESOURCE_TYPE_BLACKSMITH: return "Blacksmith";
        case WP_RESOURCE_TYPE_PEASANT_PEON: return "Peasant/Peon";
        case WP_RESOURCE_TYPE_FOOTMAN_GRUNT: return "Footman/Grunt";
        case WP_RESOURCE_TYPE_ARCHER_SPEARMAN: return "Archer/Spearman";
        case WP_RESOURCE_TYPE_KNIGHT_RAIDER: return "Knight/Rider";
        case WP_RESOURCE_TYPE_CATAPULT: return "Catapult";
        case WP_RESOURCE_TYPE_CONJURER_WARLOCK: return "Conjurer/Warlock";
        case WP_RESOURCE_TYPE_CLERIC_NECROLYTE: return "Cleric/Necrolyte";
        default: return "<error>";
    }
}

char* actionTypeToString(WPActionType actionType)
{
    switch (actionType)
    {
        case WP_ACTION_TYPE_COLLECT_GOLD: return "Collect Gold";
        case WP_ACTION_TYPE_COLLECT_WOOD: return "Collect Wood";
        case WP_ACTION_TYPE_BUILD_SUPPLY: return "Build Supply";
        case WP_ACTION_TYPE_BUILD_TOWNHALL: return "Build TownHall";
        case WP_ACTION_TYPE_BUILD_BARRACKS: return "Build Barracks";
        case WP_ACTION_TYPE_BUILD_CHURCH_TEMPLE: return "Build Church/Temple";
        case WP_ACTION_TYPE_BUILD_TOWER: return "Build Tower";
        case WP_ACTION_TYPE_BUILD_STABLE_KENNEL: return "Build Stable/Kennel";
        case WP_ACTION_TYPE_BUILD_LUMBERMILL: return "Build Lumbermill";
        case WP_ACTION_TYPE_BUILD_BLACKSMITH: return "Build Blacksmith";
        case WP_ACTION_TYPE_TRAIN_PEASANT_PEON: return "Train Peasant/Peon";
        case WP_ACTION_TYPE_TRAIN_FOOTMAN_GRUNT: return "Train Footman/Grunt";
        case WP_ACTION_TYPE_TRAIN_ARCHER_SPEARMAN: return "Train Archer/Spearman";
        case WP_ACTION_TYPE_TRAIN_KNIGHT_RAIDER: return "Train Knight/Rider";
        case WP_ACTION_TYPE_TRAIN_CATAPULT: return "Train Captapult";
        case WP_ACTION_TYPE_TRAIN_CONJURER_WARLOCK: return "Train Conjurer/Warlock";
        case WP_ACTION_TYPE_TRAIN_CLERIC_NECROLYTE: return "Train Clerci/Necrolyte";
        default: return "<error>";
    }
}

void printGameState(const WPGameState* gameState)
{
    printf("Time: %d\n", gameState->time);
    for (int i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
        printf("[%s, %d]\n", resourceTypeToString((WPResourceType)i), gameState->resources[i]);
}

void printGoal(const WPGoal* goal)
{
    for (int i = 0; i < WP_RESOURCE_TYPE_COUNT; i++)
        printf("[%s, %d]\n", resourceTypeToString((WPResourceType)i), goal->resources[i]);
}

void printAction(const WPAction* action)
{
    printf("[%s] (%d, %d)\n", actionTypeToString(action->type), action->start, action->end);
}

void printPlan(const WPPlan* plan)
{
    printf("Count: %d\n", plan->count);
    for (int i = 0; i < plan->count; i++)
        printAction(&plan->items[i]);
}

void testMEA()
{
    WPGameState gameState = wpCreateGameStateFromArgs(
        0, 4,
        WP_RESOURCE_TYPE_GOLD, 600,
        WP_RESOURCE_TYPE_WOOD, 400,
        WP_RESOURCE_TYPE_TOWNHALL, 1,
        WP_RESOURCE_TYPE_PEASANT_PEON, 1
    );

    printf("Game state\n");
    printf("======================\n");
    printGameState(&gameState);
    printf("======================\n");

    WPGoal goal = wpCreateGoalFromArgs(
        6,
        WP_RESOURCE_TYPE_FOOTMAN_GRUNT, 10,
        WP_RESOURCE_TYPE_ARCHER_SPEARMAN, 10,
        WP_RESOURCE_TYPE_KNIGHT_RAIDER, 6,
        WP_RESOURCE_TYPE_CATAPULT, 4,
        WP_RESOURCE_TYPE_CONJURER_WARLOCK, 2,
        WP_RESOURCE_TYPE_CLERIC_NECROLYTE, 2
    );
    // WPGoal goal = wpCreateGoalFromArgs(
    //     2,
    //     WP_RESOURCE_TYPE_PEASANT_PEON, 2,
    //     WP_RESOURCE_TYPE_FOOTMAN_GRUNT, 1
    // );

    printf("Goal\n");
    printf("======================\n");
    printGoal(&goal);
    printf("======================\n");

    clock_t start, end;
    double cpuTimeUsed = 0.0;

    start = clock();
    WPGameState gameStateTest1 = gameState;
    WPPlan plan;
    WPPlanInit(&plan, WPPlanDefaultOptions);
    wpMEA(&gameStateTest1, &goal, &plan);
    end = clock();
    cpuTimeUsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("cpu time used for MEA: %f\n", cpuTimeUsed);
    printf("======================\n");

    printf("Testing before scheduling\n");
    printf("======================\n");
    printf("Plan with %d actions\n", plan.count);
    WPGameState gameStateTest2 = gameState;
    wpExecutePlan(&gameStateTest2, &plan, 0, UINT32_MAX);
    if (wpSatisfyGoal(&gameStateTest2, &goal))
        printf("Plan is correct with duration: %d!\n", wpGetPlanDuration(&plan));
    else
        printf("Plan is NOT correct\n");
    printf("======================\n");

    // printf("Plan before schedule\n");
    // printf("======================\n");
    // printPlan(&plan);
    // printf("======================\n");

    start = clock();
    wpSchedulePlan(&plan, gameState);
    end = clock();
    cpuTimeUsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("cpu time used for Schedule: %f\n", cpuTimeUsed);
    printf("======================\n");

    printf("Testing after scheduling\n");
    printf("======================\n");
    printf("Plan with %d actions\n", plan.count);
    WPGameState gameStateTest3 = gameState;
    wpExecutePlan(&gameStateTest3, &plan, 0, UINT32_MAX);
    if (wpSatisfyGoal(&gameStateTest3, &goal))
        printf("Plan is correct with duration: %d!\n", wpGetPlanDuration(&plan));
    else
        printf("Plan is NOT correct\n");
    printf("======================\n");

    // printf("Plan after schedule\n");
    // printf("======================\n");
    // printPlan(&plan);
    // printf("======================\n");
}

void testBestPlan()
{
    WPGameState gameState = wpCreateGameStateFromArgs(
        0, 5,
        WP_RESOURCE_TYPE_GOLD, 600,
        WP_RESOURCE_TYPE_WOOD, 400,
        WP_RESOURCE_TYPE_SUPPLY, 1,
        WP_RESOURCE_TYPE_TOWNHALL, 1,
        WP_RESOURCE_TYPE_PEASANT_PEON, 1
    );

    printf("Game state\n");
    printf("======================\n");
    printGameState(&gameState);
    printf("======================\n");

    WPGoal goal = wpCreateGoalFromArgs(
        6,
        WP_RESOURCE_TYPE_FOOTMAN_GRUNT, 10,
        WP_RESOURCE_TYPE_ARCHER_SPEARMAN, 10,
        WP_RESOURCE_TYPE_KNIGHT_RAIDER, 6,
        WP_RESOURCE_TYPE_CATAPULT, 4,
        WP_RESOURCE_TYPE_CONJURER_WARLOCK, 2,
        WP_RESOURCE_TYPE_CLERIC_NECROLYTE, 2
    );
    // WPGoal goal = wpCreateGoalFromArgs(
    //     2,
    //     WP_RESOURCE_TYPE_PEASANT_PEON, 2,
    //     WP_RESOURCE_TYPE_FOOTMAN_GRUNT, 1
    // );

    WPGoal intermediateGoals[] =
    {
        wpCreateGoalFromArgs(
            1,
            WP_RESOURCE_TYPE_PEASANT_PEON, 10
        ),
        wpCreateGoalFromArgs(
            1,
            WP_RESOURCE_TYPE_BARRACKS, 3
        ),
        wpCreateGoalFromArgs(
            2,
            WP_RESOURCE_TYPE_PEASANT_PEON, 10,
            WP_RESOURCE_TYPE_BARRACKS, 3
        ),
        wpCreateGoalFromArgs(
            2,
            WP_RESOURCE_TYPE_PEASANT_PEON, 10,
            WP_RESOURCE_TYPE_TOWER, 2
        ),
        wpCreateGoalFromArgs(
            3,
            WP_RESOURCE_TYPE_PEASANT_PEON, 10,
            WP_RESOURCE_TYPE_BARRACKS, 3,
            WP_RESOURCE_TYPE_TOWER, 2
        ),
        wpCreateGoalFromArgs(
            4,
            WP_RESOURCE_TYPE_PEASANT_PEON, 10,
            WP_RESOURCE_TYPE_BARRACKS, 3,
            WP_RESOURCE_TYPE_TOWER, 2,
            WP_RESOURCE_TYPE_CHURCH_TEMPLE, 2
        )
    };

    printf("Goal\n");
    printf("======================\n");
    printGoal(&goal);
    printf("======================\n");

    clock_t start, end;
    double cpuTimeUsed = 0.0;

    start = clock();
    WPGameState gameStateTest1 = gameState;
    WPPlan plan;
    WPPlanInit(&plan, WPPlanDefaultOptions);
    wpBestPlan(&gameStateTest1, &goal, arrayLength(intermediateGoals), intermediateGoals, &plan);
    end = clock();
    cpuTimeUsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("cpu time used for BestPlan: %f\n", cpuTimeUsed);

    printf("Testing\n");
    printf("======================\n");
    printf("Plan with %d actions\n", plan.count);
    WPGameState gameStateTest2 = gameState;
    wpExecutePlan(&gameStateTest2, &plan, 0, UINT32_MAX);
    if (wpSatisfyGoal(&gameStateTest2, &goal))
        printf("Plan is correct with duration: %d!\n", wpGetPlanDuration(&plan));
    else
        printf("Plan is NOT correct\n");
    printf("======================\n");

    // printf("Plan after schedule\n");
    // printf("======================\n");
    // printPlan(&plan);
    // printf("======================\n");
}