#include "war_state_machine.h"

#include "TracyC.h"

WarState* wst_createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    TracyCZoneN(ctx, "wst_createGatherWoodState", true);

    WarState* state = wst_createState(context, entity, WAR_STATE_WOOD);
    state->wood.forestId = forestId;
    state->wood.position = position;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterGatherWoodState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_leaveGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveGatherWoodState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateGatherWoodState", true);

    WarMap* map = context->map;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    const WarUnitStats* stats = wu_getUnitStats(unit->type);
    vec2 position = wu_getUnitCenterPosition(context, entity, true);

    WarEntity* forest = we_findEntity(context, (WarEntityId)state->wood.forestId);

    // if the forest doesn't exists, go idle
    if (!forest)
    {
        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
        TracyCZoneEnd(ctx);
        return;
    }

    vec2 treePosition = state->wood.position;
    WarTree* tree = we_getTreeAtPosition(context, forest, (s32)treePosition.x, (s32)treePosition.y);

    if (!tree || tree->amount == 0 || !wpath_isPositionAccesible(&map->finder, treePosition))
    {
        tree = we_findAccesibleTree(context, forest, treePosition);

        // if there is no more nearby tree, go idle
        if (!tree)
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
            TracyCZoneEnd(ctx);
            return;
        }

        treePosition = vec2i(tree->tilex, tree->tiley);
        state->wood.position = treePosition;
    }

    // if the tree is not in range, go to it
    if (!wu_tileInRange(context, entity, treePosition, stats->range))
    {
        WarState* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, treePosition));
        moveState->nextState = state;
        wst_changeNextState(context, entity, moveState, false, true);
        TracyCZoneEnd(ctx);
        return;
    }

    // the unit arrive to the tree, go chopping
    WarState* choppingState = wst_createChoppingState(context, entity, forest->id, treePosition);
    wst_changeNextState(context, entity, choppingState, true, true);

    TracyCZoneEnd(ctx);
}

void wst_freeGatherWoodState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeGatherWoodState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
