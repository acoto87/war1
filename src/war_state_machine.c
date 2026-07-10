#include <assert.h>
#include <string.h>

#include "war_state_machine.h"

#include "TracyC.h"

WarStateDescriptor stateDescriptors[WAR_STATE_COUNT] =
{
    { WAR_STATE_IDLE,       wst_leaveIdleState       },
    { WAR_STATE_MOVE,       wst_leaveMoveState       },
    { WAR_STATE_PATROL,     wst_leavePatrolState     },
    { WAR_STATE_FOLLOW,     wst_leaveFollowState     },
    { WAR_STATE_ATTACK,     wst_leaveAttackState     },
    { WAR_STATE_GOLD,       wst_leaveGatherGoldState },
    { WAR_STATE_MINING,     wst_leaveMiningState     },
    { WAR_STATE_WOOD,       wst_leaveGatherWoodState },
    { WAR_STATE_CHOPPING,   wst_leaveChoppingState   },
    { WAR_STATE_DELIVER,    wst_leaveDeliverState    },
    { WAR_STATE_DEATH,      wst_leaveDeathState      },
    { WAR_STATE_COLLAPSE,   wst_leaveCollapseState   },
    { WAR_STATE_TRAIN,      wst_leaveTrainState      },
    { WAR_STATE_UPGRADE,    wst_leaveUpgradeState    },
    { WAR_STATE_BUILD,      wst_leaveBuildState      },
    { WAR_STATE_REPAIR,     wst_leaveRepairState     },
    { WAR_STATE_REPAIRING,  wst_leaveRepairingState  },
    { WAR_STATE_CAST,       wst_leaveCastState       },
    { WAR_STATE_WAIT,       wst_leaveWaitState       },
};

bool wst_isInsideBuilding(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isInsideBuilding", true);

    if (wst_isMining(context, entity))
    {
        TracyCZoneEnd(ctx);
        return true;
    }

    if(wst_isDelivering(context, entity))
    {
        WarStateDeliver* deliver = wst_getDeliverState(context, entity);
        TracyCZoneEnd(ctx);
        return deliver->insideBuilding;
    }

    if (wst_isRepairing2(context, entity))
    {
        WarStateRepairing* repairing = wst_getRepairingState(context, entity);
        TracyCZoneEnd(ctx);
        return repairing->insideBuilding;
    }

    TracyCZoneEnd(ctx);
    return false;
}

static void* wst_getTypeArray(WarStateStorage* s, WarStateType type)
{
    switch (type)
    {
        case WAR_STATE_IDLE:      return s->idle;
        case WAR_STATE_MOVE:      return s->move;
        case WAR_STATE_PATROL:    return s->patrol;
        case WAR_STATE_FOLLOW:    return s->follow;
        case WAR_STATE_ATTACK:    return s->attack;
        case WAR_STATE_GOLD:      return s->gold;
        case WAR_STATE_MINING:    return s->mining;
        case WAR_STATE_WOOD:      return s->wood;
        case WAR_STATE_CHOPPING:  return s->chopping;
        case WAR_STATE_DELIVER:   return s->deliver;
        case WAR_STATE_DEATH:     return s->death;
        case WAR_STATE_COLLAPSE:  return s->collapse;
        case WAR_STATE_TRAIN:     return s->train;
        case WAR_STATE_UPGRADE:   return s->upgrade;
        case WAR_STATE_BUILD:     return s->build;
        case WAR_STATE_REPAIR:    return s->repair;
        case WAR_STATE_REPAIRING: return s->repairing;
        case WAR_STATE_CAST:      return s->cast;
        case WAR_STATE_WAIT:      return s->wait;
        default:
            assert(false);
            return NULL;
    }
}

static const size_t stateTypeSizes[WAR_STATE_COUNT] =
{
    sizeof(WarStateIdle),
    sizeof(WarStateMove),
    sizeof(WarStatePatrol),
    sizeof(WarStateFollow),
    sizeof(WarStateAttack),
    sizeof(WarStateGold),
    sizeof(WarStateMining),
    sizeof(WarStateWood),
    sizeof(WarStateChopping),
    sizeof(WarStateDeliver),
    sizeof(WarStateDeath),
    sizeof(WarStateCollapse),
    sizeof(WarStateTrain),
    sizeof(WarStateUpgrade),
    sizeof(WarStateBuild),
    sizeof(WarStateRepair),
    sizeof(WarStateRepairing),
    sizeof(WarStateCast),
    sizeof(WarStateWait),
};

WarStateBase* wst_deref(WarContext* context, WarStateRef ref)
{
    if (!WAR_STATE_REF_IS_VALID(ref)) return NULL;
    WarStateStorage* s = &we_getEntityManager(context)->stateStorage;
    char* base = (char*)wst_getTypeArray(s, ref.type);
    return (WarStateBase*)(base + (size_t)ref.idx * stateTypeSizes[ref.type]);
}

WarStateRef wst_refOf(WarContext* context, const WarStateBase* state)
{
    WarStateStorage* s = &we_getEntityManager(context)->stateStorage;
    char* arrayBase = (char*)wst_getTypeArray(s, state->type);
    s32 idx = (s32)(((const char*)state - arrayBase) / (s32)stateTypeSizes[state->type]);
    return (WarStateRef){ state->type, idx };
}

void wst_initStorage(WarStateStorage* s)
{
    memset(s->occupied, 0, sizeof(s->occupied));
    memset(s->activeCounts, 0, sizeof(s->activeCounts));
    for (s32 t = 0; t < WAR_STATE_COUNT; t++)
    {
        s->freeCounts[t] = MAX_STATES_PER_TYPE;
        for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
            s->freeLists[t][i] = MAX_STATES_PER_TYPE - 1 - i; // LIFO
    }
}

WarStateRef wst_allocState(WarContext* context, WarStateType type, WarEntityId entityId)
{
    TracyCZoneN(ctx, "wst_allocState", true);

    WarStateStorage* s = &we_getEntityManager(context)->stateStorage;
    assert(s->freeCounts[type] > 0 && "State storage exhausted for type");

    s32 idx = s->freeLists[type][--s->freeCounts[type]];
    s->occupied[type][idx] = true;
    s->activeCounts[type]++;

    WarStateBase* state = (WarStateBase*)((char*)wst_getTypeArray(s, type) + (size_t)idx * stateTypeSizes[type]);
    memset(state, 0, stateTypeSizes[type]);
    state->type     = type;
    state->entityId = entityId;

    TracyCZoneEnd(ctx);
    return (WarStateRef){ type, idx };
}

void wst_freeStateRef(WarContext* context, WarStateRef ref)
{
    TracyCZoneN(ctx, "wst_freeStateRef", true);

    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return;
    }

    if (!inRange(ref.type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unknown state type %d in wst_freeStateRef", ref.type);
        TracyCZoneEnd(ctx);
        return;
    }

    WarStateStorage* s = &we_getEntityManager(context)->stateStorage;
    assert(wst_deref(context, ref) != NULL);

    // Idempotent: the slot may already have been freed.
    if (!s->occupied[ref.type][ref.idx])
    {
        TracyCZoneEnd(ctx);
        return;
    }

    s->occupied[ref.type][ref.idx] = false;
    s->freeLists[ref.type][s->freeCounts[ref.type]++] = ref.idx;
    s->activeCounts[ref.type]--;

    TracyCZoneEnd(ctx);
}

static void wst_clearPendingState(WarContext* context, WarStateMachineComponent* sm)
{
    if (WAR_STATE_REF_IS_VALID(sm->pendingRef))
    {
        // The pending state was never active, so just free its slot.
        wst_freeStateRef(context, sm->pendingRef);
        sm->pendingRef = WAR_STATE_REF_INVALID;
    }
    sm->pendingOp = WAR_FSM_OP_NONE;
    sm->leaveCurrent = false;
}

void wst_pushState(WarContext* context, WarEntity* entity, WarStateBase* state)
{
    TracyCZoneN(ctx, "wst_pushState", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    wst_clearPendingState(context, sm);

    if (sm->depth >= WAR_STATE_STACK_DEPTH)
    {
        logWarning("State stack overflow for entity %d; replacing top state", entity->id);
        wst_replaceState(context, entity, state);
        TracyCZoneEnd(ctx);
        return;
    }

    sm->pendingRef = wst_refOf(context, state);
    sm->pendingOp = WAR_FSM_OP_PUSH;
    sm->leaveCurrent = false;

    TracyCZoneEnd(ctx);
}

void wst_popState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_popState", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);
    assert(sm->depth > 0);

    wst_clearPendingState(context, sm);

    sm->pendingOp = WAR_FSM_OP_POP;
    sm->leaveCurrent = true;

    TracyCZoneEnd(ctx);
}

void wst_replaceState(WarContext* context, WarEntity* entity, WarStateBase* state)
{
    TracyCZoneN(ctx, "wst_replaceState", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);
    assert(sm->depth > 0);

    wst_clearPendingState(context, sm);

    sm->pendingRef = wst_refOf(context, state);
    sm->pendingOp = WAR_FSM_OP_REPLACE;
    sm->leaveCurrent = true;

    TracyCZoneEnd(ctx);
}

void wst_resetState(WarContext* context, WarEntity* entity, WarStateBase* state)
{
    TracyCZoneN(ctx, "wst_resetState", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    wst_clearPendingState(context, sm);

    sm->pendingRef = wst_refOf(context, state);
    sm->pendingOp = WAR_FSM_OP_RESET;
    sm->leaveCurrent = true;

    TracyCZoneEnd(ctx);
}

WarStateBase* wst_currentState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_currentState", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    WarStateBase* result = NULL;
    if (sm->depth > 0)
        result = wst_deref(context, sm->stack[sm->depth - 1]);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_hasStateInStack(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_hasStateInStack", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    for (u8 i = 0; i < sm->depth; i++)
    {
        if (sm->stack[i].type == type)
        {
            TracyCZoneEnd(ctx);
            return true;
        }
    }

    TracyCZoneEnd(ctx);
    return false;
}

WarStateBase* wst_peekAt(WarContext* context, WarEntity* entity, u8 index)
{
    TracyCZoneN(ctx, "wst_peekAt", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    WarStateBase* result = NULL;
    if (index < sm->depth)
        result = wst_deref(context, sm->stack[index]);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateBase* wst_getState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_getState", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    for (s32 i = (s32)sm->depth - 1; i >= 0; i--)
    {
        WarStateRef ref = sm->stack[i];
        if (ref.type == type)
        {
            WarStateBase* state = wst_deref(context, ref);
            TracyCZoneEnd(ctx);
            return state;
        }
    }

    TracyCZoneEnd(ctx);
    return NULL;
}

WarStateBase* wst_getDirectState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_getDirectState", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    WarStateBase* result = NULL;
    if (sm->depth > 0)
    {
        WarStateRef ref = sm->stack[sm->depth - 1];
        if (ref.type == type)
            result = wst_deref(context, ref);
    }

    TracyCZoneEnd(ctx);
    return result;
}

WarStateIdle* wst_getIdleState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getIdleState", true);

    WarStateIdle* result = (WarStateIdle*)wst_getDirectState(context, entity, WAR_STATE_IDLE);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateMove* wst_getMoveState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getMoveState", true);

    WarStateMove* result = (WarStateMove*)wst_getState(context, entity, WAR_STATE_MOVE);

    TracyCZoneEnd(ctx);
    return result;
}

WarStatePatrol* wst_getPatrolState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getPatrolState", true);

    WarStatePatrol* result = (WarStatePatrol*)wst_getState(context, entity, WAR_STATE_PATROL);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateFollow* wst_getFollowState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getFollowState", true);

    WarStateFollow* result = (WarStateFollow*)wst_getState(context, entity, WAR_STATE_FOLLOW);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateAttack* wst_getAttackState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getAttackState", true);

    WarStateAttack* result = (WarStateAttack*)wst_getState(context, entity, WAR_STATE_ATTACK);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateDeath* wst_getDeathState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getDeathState", true);

    WarStateDeath* result = (WarStateDeath*)wst_getState(context, entity, WAR_STATE_DEATH);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateCollapse* wst_getCollapseState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getCollapseState", true);

    WarStateCollapse* result = (WarStateCollapse*)wst_getState(context, entity, WAR_STATE_COLLAPSE);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateGold* wst_getGatherGoldState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getGatherGoldState", true);

    WarStateGold* result = (WarStateGold*)wst_getState(context, entity, WAR_STATE_GOLD);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateMining* wst_getMiningState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getMiningState", true);

    WarStateMining* result = (WarStateMining*)wst_getState(context, entity, WAR_STATE_MINING);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateWood* wst_getGatherWoodState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getGatherWoodState", true);

    WarStateWood* result = (WarStateWood*)wst_getState(context, entity, WAR_STATE_WOOD);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateChopping* wst_getChoppingState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getChoppingState", true);

    WarStateChopping* result = (WarStateChopping*)wst_getState(context, entity, WAR_STATE_CHOPPING);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateDeliver* wst_getDeliverState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getDeliverState", true);

    WarStateDeliver* result = (WarStateDeliver*)wst_getState(context, entity, WAR_STATE_DELIVER);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateTrain* wst_getTrainState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getTrainState", true);

    WarStateTrain* result = (WarStateTrain*)wst_getState(context, entity, WAR_STATE_TRAIN);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateUpgrade* wst_getUpgradeState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getUpgradeState", true);

    WarStateUpgrade* result = (WarStateUpgrade*)wst_getState(context, entity, WAR_STATE_UPGRADE);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateBuild* wst_getBuildState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getBuildState", true);

    WarStateBuild* result = (WarStateBuild*)wst_getState(context, entity, WAR_STATE_BUILD);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateRepair* wst_getRepairState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getRepairState", true);

    WarStateRepair* result = (WarStateRepair*)wst_getState(context, entity, WAR_STATE_REPAIR);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateRepairing* wst_getRepairingState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getRepairingState", true);

    WarStateRepairing* result = (WarStateRepairing*)wst_getState(context, entity, WAR_STATE_REPAIRING);

    TracyCZoneEnd(ctx);
    return result;
}

WarStateCast* wst_getCastState(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_getCastState", true);

    WarStateCast* result = (WarStateCast*)wst_getState(context, entity, WAR_STATE_CAST);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_hasState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_hasState", true);

    bool result = wst_getState(context, entity, type) != NULL;

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_hasDirectState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_hasDirectState", true);

    bool result = wst_getDirectState(context, entity, type) != NULL;

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_hasNextState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_hasNextState", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    bool result = WAR_STATE_REF_IS_VALID(sm->pendingRef) && sm->pendingRef.type == type;

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isIdle(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isIdle", true);

    bool result = wst_hasDirectState(context, entity, WAR_STATE_IDLE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isMoving(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isMoving", true);

    bool result = wst_hasState(context, entity, WAR_STATE_MOVE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isPatrolling(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isPatrolling", true);

    bool result = wst_hasState(context, entity, WAR_STATE_PATROL);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isFollowing(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isFollowing", true);

    bool result = wst_hasState(context, entity, WAR_STATE_FOLLOW);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isAttacking(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isAttacking", true);

    bool result = wst_hasState(context, entity, WAR_STATE_ATTACK);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isDead(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isDead", true);

    bool result = wst_hasState(context, entity, WAR_STATE_DEATH);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isCollapsing(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isCollapsing", true);

    bool result = wst_hasState(context, entity, WAR_STATE_COLLAPSE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGatheringGold(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGatheringGold", true);

    bool result = wst_hasState(context, entity, WAR_STATE_GOLD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isMining(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isMining", true);

    bool result = wst_hasState(context, entity, WAR_STATE_MINING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGatheringWood(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGatheringWood", true);

    bool result = wst_hasState(context, entity, WAR_STATE_WOOD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isChopping(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isChopping", true);

    bool result = wst_hasState(context, entity, WAR_STATE_CHOPPING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isDelivering(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isDelivering", true);

    bool result = wst_hasState(context, entity, WAR_STATE_DELIVER);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isTraining(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isTraining", true);

    bool result = wst_hasState(context, entity, WAR_STATE_TRAIN);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isUpgrading(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isUpgrading", true);

    bool result = wst_hasState(context, entity, WAR_STATE_UPGRADE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isBuilding(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isBuilding", true);

    bool result = wst_hasState(context, entity, WAR_STATE_BUILD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isRepairing(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isRepairing", true);

    bool result = wst_hasState(context, entity, WAR_STATE_REPAIR);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isRepairing2(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isRepairing2", true);

    bool result = wst_hasState(context, entity, WAR_STATE_REPAIRING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isCasting(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isCasting", true);

    bool result = wst_hasState(context, entity, WAR_STATE_CAST);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToIdle(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToIdle", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_IDLE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToMove(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToMove", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_MOVE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToPatrol(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToPatrol", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_PATROL);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToFollow(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToFollow", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_FOLLOW);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToAttack(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToAttack", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_ATTACK);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToDie(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToDie", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_DEATH);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToCollapse(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToCollapse", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_COLLAPSE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToGatherGold(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToGatherGold", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_GOLD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToMine(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToMine", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_MINING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToGatherWood(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToGatherWood", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_WOOD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToChop(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToChop", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_CHOPPING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToDeliver(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToDeliver", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_DELIVER);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToTrain(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToTrain", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_TRAIN);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToUpgrade(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToUpgrade", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_UPGRADE);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToBuild(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToBuild", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_BUILD);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToRepair(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToRepair", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_REPAIR);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToRepair2(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToRepair2", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_REPAIRING);

    TracyCZoneEnd(ctx);
    return result;
}

bool wst_isGoingToCast(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_isGoingToCast", true);

    bool result = wst_hasNextState(context, entity, WAR_STATE_CAST);

    TracyCZoneEnd(ctx);
    return result;
}

void wst_leaveState(WarContext* context, WarEntity* entity, WarStateBase* state)
{
    TracyCZoneN(ctx, "wst_leaveState", true);

    if (!state)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unknown state %d for entity %d", state->type, entity->id);
        TracyCZoneEnd(ctx);
        return;
    }

    stateDescriptors[state->type].leaveFunc(context, entity, state);
    wst_freeStateRef(context, wst_refOf(context, state));

    TracyCZoneEnd(ctx);
}

void wst_processStateMachinePendingOps(WarContext* context)
{
    TracyCZoneN(ctx, "wst_processStateMachinePendingOps", true);

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);

    for (s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    {
        WarEntity* entity = &manager->entities[i];
        if (entity->id == 0) continue;
        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;

        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->pendingOp == WAR_FSM_OP_NONE) continue;

        switch (sm->pendingOp)
        {
            case WAR_FSM_OP_PUSH:
            {
                assert(sm->depth < WAR_STATE_STACK_DEPTH);
                sm->stack[sm->depth] = sm->pendingRef;
                sm->depth++;
                break;
            }
            case WAR_FSM_OP_POP:
            {
                assert(sm->depth > 0);
                WarStateRef topRef = sm->stack[sm->depth - 1];
                WarStateBase* top = wst_deref(context, topRef);
                if (sm->leaveCurrent && top)
                    wst_leaveState(context, entity, top);
                sm->depth--;
                if (sm->depth == 0)
                {
                    WarStateIdle* idle = wst_createIdleState(context, entity, true);
                    sm->stack[0] = wst_refOf(context, (WarStateBase*)idle);
                    sm->depth = 1;
                }
                break;
            }
            case WAR_FSM_OP_REPLACE:
            {
                assert(sm->depth > 0);
                WarStateRef topRef = sm->stack[sm->depth - 1];
                WarStateBase* top = wst_deref(context, topRef);
                if (sm->leaveCurrent && top)
                    wst_leaveState(context, entity, top);
                sm->stack[sm->depth - 1] = sm->pendingRef;
                break;
            }
            case WAR_FSM_OP_RESET:
            {
                for (s32 d = (s32)sm->depth - 1; d >= 0; d--)
                {
                    WarStateBase* s = wst_deref(context, sm->stack[d]);
                    if (s) wst_leaveState(context, entity, s);
                }
                sm->stack[0] = sm->pendingRef;
                sm->depth = 1;
                break;
            }
            default: break;
        }

        sm->pendingOp    = WAR_FSM_OP_NONE;
        sm->pendingRef   = WAR_STATE_REF_INVALID;
        sm->leaveCurrent = false;
    }

    TracyCZoneEnd(ctx);
}
