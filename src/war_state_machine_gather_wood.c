#include "war_state_machine.h"

WarState* wst_createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_WOOD);
    state->wood.forestId = forestId;
    state->wood.position = position;
    return state;
}

void wst_enterGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_leaveGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updateGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;
    WarUnitStats stats = wun_getUnitStats(unit->type);
    vec2 position = wun_getUnitCenterPosition(entity, true);

    WarEntity* forest = went_findEntity(context, (WarEntityId)state->wood.forestId);

    // if the forest doesn't exists, go idle
    if (!forest)
    {
        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
        return;
    }

    vec2 treePosition = state->wood.position;
    WarTree* tree = went_getTreeAtPosition(forest, (s32)treePosition.x, (s32)treePosition.y);

    if (!tree || tree->amount == 0 || !wpath_isPositionAccesible(map->finder, treePosition))
    {
        tree = went_findAccesibleTree(context, forest, treePosition);

        // if there is no more nearby tree, go idle
        if (!tree)
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
            return;
        }

        treePosition = vec2i(tree->tilex, tree->tiley);
        state->wood.position = treePosition;
    }

    // if the tree is not in range, go to it
    if (!wun_tileInRange(entity, treePosition, stats.range))
    {
        WarState* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, treePosition));
        moveState->nextState = state;
        wst_changeNextState(context, entity, moveState, false, true);
        return;
    }

    // the unit arrive to the tree, go chopping
    WarState* choppingState = wst_createChoppingState(context, entity, forest->id, treePosition);
    wst_changeNextState(context, entity, choppingState, true, true);
}

void wst_freeGatherWoodState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}
