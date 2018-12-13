WarState* createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    WarState* state = createState(context, entity, WAR_STATE_WOOD);
    state->wood.forestId = forestId;
    state->wood.position = position;
    state->wood.direction = 1;
    state->wood.insideBuilding = false;
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
    vec2 unitSize = getUnitSize(entity);
    WarUnitStats stats = getUnitStats(unit->type);
    vec2 position = getUnitCenterPosition(entity, true);

    // this state enable and disable the sprite component to simulate the depositing of the wood in the townhall
    // so enable it by default for the walk part, and let the process of updating the state disable it if needed
    entity->sprite.enabled = true;

    // by default, the worker unit isn't inside a building
    state->wood.insideBuilding = false;

    // if the state doesn't know where to go next, go idle
    if (state->wood.direction == 0)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

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

    // if the worker is going towards the tree
    if (state->wood.direction > 0)
    {
        // if the tree is not accesible, or if there is no more wood on the tree (it could ran out of wood by other workers chopping it), 
        // find another nearby and accesible tree
        if (!tree || tree->amount == 0 || isPositionAccesible(map->finder, treePosition))
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
        if (!positionInRange(entity, treePosition, stats.range))
        {
            WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, treePosition));
            moveState->nextState = state;
            changeNextState(context, entity, moveState, false, true);
            return;
        }

        setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
        setUnitDirectionFromDiff(entity, treePosition.x - position.x, treePosition.y - position.y);
        setAction(context, entity, WAR_ACTION_TYPE_HARVEST, false, 1.0f);

        // the unit arrive to the tree, so now the unit start chopping the tree,
        // then need go back to the townhall. for this the sprite of the unit need to be changed to the one with carrying wood.
        state->wood.direction = -1;

        // TODO: I think is better to move this to a state CHOPPING, because it would be easier to detect when the tree
        // is out of wood if multiple units are chopping it. This way, the unit will keep chopping even if the tree already
        // has no wood, until the 5 seconds passed. 
        // This also apply to the mining in the goldmine.
        setDelay(state, 5.0f);
    }
    else
    {
        if (tree)
        {
            chopTree(context, forest, tree, 100);
            determineTreeTiles(context, forest);
        }
        
        WarWorkerData workerData = getWorkerData(unit->type);
        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, workerData.carryingWoodResource);

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

        if (!unitInRange(entity, townHall, stats.range))
        {
            WarState* followState = createFollowState(context, entity, townHall->id, stats.range);
            followState->nextState = state;
            changeNextState(context, entity, followState, false, true);
            return;
        }

        // TODO: deliver wood here to the player!!
        logDebug("wood delivered!");
        
        // the unit arrive to the townhall, so now the unit go to wait state for some time to simulate the depositing
        // then need go back to the tree. for this the sprite of the unit need to be changed to the normal one.
        state->wood.direction = 1;
        state->wood.insideBuilding = true;

        WarUnitsData unitData = getUnitsData(unit->type);
        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, unitData.resourceIndex);
        entity->sprite.enabled = false;

        setDelay(state, 1.0f);
    }
}

void freeGatherWoodState(WarState* state)
{
}
