#include "war_state_machine.h"

WarState* createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    WarState* state = createState(context, entity, WAR_STATE_WOOD);
    state->wood.forestId = forestId;
    state->wood.position = position;
    return state;
}

void enterGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void leaveGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void updateGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = getUnitStats(unit->type);
    vec2 position = getUnitCenterPosition(entity, true);

    WarEntity* forest = went_findEntity(context, (WarEntityId)state->wood.forestId);

    // if the forest doesn't exists, go idle
    if (!forest)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    vec2 treePosition = state->wood.position;
    WarTree* tree = went_getTreeAtPosition(forest, (s32)treePosition.x, (s32)treePosition.y);

    if (!tree || tree->amount == 0 || !isPositionAccesible(map->finder, treePosition))
    {
        tree = went_findAccesibleTree(context, forest, treePosition);

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
    if (!tileInRange(entity, treePosition, stats.range))
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

void freeGatherWoodState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}
