#include "war_state_machine.h"

#include "TracyC.h"

WarStateWood* wst_createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position)
{
    TracyCZoneN(ctx, "wst_createGatherWoodState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_WOOD, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateWood* state = (WarStateWood*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->forestId = forestId;
    state->position = position;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_updateGatherWoodState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateGatherWoodState", true);

    WarStateWood* s = (WarStateWood*)state;

    WarMap* map = context->map;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    WarEntity* forest = we_findEntity(context, (WarEntityId)s->forestId);
    if (!forest)
    {
        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
        TracyCZoneEnd(ctx);
        return;
    }

    vec2 treePosition = s->position;
    vec2 treeTile = wmap_mapToTileCoordinatesV(treePosition);
    WarTree* tree = we_getTreeAtTile(context, forest, (s32)treeTile.x, (s32)treeTile.y);

    if (!tree || tree->amount == 0 || !wpath_isTileAccesible(&map->finder, (s32)treeTile.x, (s32)treeTile.y))
    {
        tree = we_findAccesibleTree(context, forest, treeTile);
        if (!tree)
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
            TracyCZoneEnd(ctx);
            return;
        }

        treeTile = vec2i(tree->tilex, tree->tiley);
        treePosition = wmap_tileToMapCoordinatesV(treeTile, true);
        s->position = treePosition;
    }

    if (!wu_tileInRange(context, entity, treeTile, stats->range))
    {
        vec2 position = wu_getUnitCenterPosition(context, entity);
        WarStateMove* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, treePosition));
        wst_pushState(context, entity, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_COMPLETION);
        TracyCZoneEnd(ctx);
        return;
    }

    WarStateChopping* choppingState = wst_createChoppingState(context, entity, forest->id, treePosition);
    wst_replaceState(context, entity, (WarStateBase*)choppingState, WAR_TRANSITION_CAUSE_COMPLETION);

    TracyCZoneEnd(ctx);
}


void wst_updateWoodStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateWoodStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateWood*      states  = storage->wood;
    bool*             occupied = storage->occupied[WAR_STATE_WOOD];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateWood*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!wst_isCurrentState(context, entity, (WarStateBase*)state)) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateGatherWoodState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
