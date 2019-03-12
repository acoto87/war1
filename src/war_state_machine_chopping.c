WarState* createChoppingState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    WarState* state = createState(context, entity, WAR_STATE_CHOPPING);
    state->chop.forestId = forestId;
    state->chop.position = position;
    state->chop.chopTime = 0;
    return state;
}

void enterChoppingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = getUnitCenterPosition(entity, true);
    vec2 treePosition = state->chop.position;

    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
    setUnitDirectionFromDiff(entity, treePosition.x - position.x, treePosition.y - position.y);
    setAction(context, entity, WAR_ACTION_TYPE_HARVEST, false, 1.0f);

    state->chop.chopTime = context->time + getScaledTime(context, 0.5f);
}

void leaveChoppingState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateChoppingState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarUnitComponent* unit = &entity->unit;

    WarEntity* forest = findEntity(context, state->chop.forestId);

    // if the forest doesn't exists, go idle
    if (!forest)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    vec2 treePosition = state->chop.position;
    WarTree* tree = getTreeAtPosition(forest, treePosition.x, treePosition.y);

    if (!tree || tree->amount == 0)
    {
        WarState* gatherWoodState = createGatherWoodState(context, entity, forest->id, treePosition);
        changeNextState(context, entity, gatherWoodState, true, true);
        return;
    }

    if (context->time >= state->chop.chopTime)
    {
        unit->amount += chopTree(context, forest, tree, 2);
        if (unit->amount > 0)
            unit->resourceKind = WAR_RESOURCE_WOOD;

        if (unit->amount == UNIT_MAX_CARRY_WOOD)
        {
            // set the carrying gold sprites
            WarWorkerData workerData = getWorkerData(unit->type);
            removeSpriteComponent(context, entity);
            addSpriteComponentFromResource(context, entity, imageResourceRef(workerData.carryingWoodResource));

            // find the closest town hall to deliver the gold
            WarRace race = getUnitRace(entity);
            WarUnitType townHallType = getTownHallOfRace(race);
            WarEntity* townHall = findClosestUnitOfType(context, entity, townHallType);

            // if the town hall doesn't exists (it could be under attack and get destroyed), go idle
            if (!townHall)
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);
                return;
            }

            WarState* deliverState = createDeliverState(context, entity, townHall->id);
            deliverState->nextState = createGatherWoodState(context, entity, forest->id, treePosition);
            changeNextState(context, entity, deliverState, true, true);
        }

        state->chop.chopTime = context->time + getScaledTime(context, 0.5f);
    }
}

void freeChoppingState(WarState* state)
{
}
