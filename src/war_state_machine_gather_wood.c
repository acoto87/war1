WarState* createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    WarState* state = createState(context, entity, WAR_STATE_WOOD);
    state->wood.forestId = forestId;
    state->wood.position = position;
    return state;
}

void enterGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void leaveGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = getUnitStats(unit->type);
    vec2 position = getUnitCenterPosition(entity, true);

    WarEntity* forest = findEntity(context, state->wood.forestId);

    // if the forest doesn't exists, go idle
    if (!forest)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    vec2 treePosition = state->wood.position;
    WarTree* tree = getTreeAtPosition(forest, treePosition.x, treePosition.y);

    if (!tree || tree->amount == 0 || !isPositionAccesible(map->finder, treePosition))
    {
        tree = findAccesibleTree(context, forest, treePosition);

        // if there is no more nearby tree, go idle
        if (!tree)
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
            return;
        }
        
        treePosition = vec2i(tree->tilex, tree->tiley);
        state->wood.position = treePosition;
    }

    // if the tree is not in range, go to it
    if (!entityTilePositionInRange(entity, treePosition, stats.range))
    {
        WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, treePosition));
        moveState->nextState = state;
        changeNextState(context, entity, moveState, false, true);
        return;
    }

    // the unit arrive to the tree, go chopping
    WarState* choppingState = createChoppingState(context, entity, forest->id, treePosition);
    changeNextState(context, entity, choppingState, true, true);
}

void freeGatherWoodState(WarState* state)
{
}
