#include "war_state_machine.h"

#include "TracyC.h"

WarStateWood* wst_createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    TracyCZoneN(ctx, "wst_createGatherWoodState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_WOOD, entity->id);
    WarStateWood* state = (WarStateWood*)wst_deref(context, ref);
    state->forestId = forestId;
    state->position = position;

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

    WarStateWood* s = (WarStateWood*)state;

    WarMap* map = context->map;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    const WarUnitStats* stats = wu_getUnitStats(unit->type);
    vec2 position = wu_getUnitCenterPosition(context, entity, true);

    WarEntity* forest = we_findEntity(context, (WarEntityId)s->forestId);

    // if the forest doesn't exists, go idle
    if (!forest)
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
        TracyCZoneEnd(ctx);
        return;
    }

    vec2 treePosition = s->position;
    WarTree* tree = we_getTreeAtPosition(context, forest, (s32)treePosition.x, (s32)treePosition.y);

    if (!tree || tree->amount == 0 || !wpath_isPositionAccesible(&map->finder, treePosition))
    {
        tree = we_findAccesibleTree(context, forest, treePosition);

        // if there is no more nearby tree, go idle
        if (!tree)
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
            TracyCZoneEnd(ctx);
            return;
        }

        treePosition = vec2i(tree->tilex, tree->tiley);
        s->position = treePosition;
    }

    // if the tree is not in range, go to it
    if (!wu_tileInRange(context, entity, treePosition, stats->range))
    {
        WarStateMove* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, treePosition));
        wst_chainNext(context, (WarStateBase*)moveState, (WarStateBase*)state);
        wst_changeNextState(context, entity, (WarStateBase*)moveState, false, true);
        TracyCZoneEnd(ctx);
        return;
    }

    // the unit arrive to the tree, go chopping
    WarStateChopping* choppingState = wst_createChoppingState(context, entity, forest->id, treePosition);
    wst_changeNextState(context, entity, (WarStateBase*)choppingState, true, true);

    TracyCZoneEnd(ctx);
}

void wst_freeGatherWoodState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeGatherWoodState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
