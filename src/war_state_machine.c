#include <assert.h>
#include <string.h>

#include "war_state_machine.h"

#include "TracyC.h"

WarStateDescriptor stateDescriptors[WAR_STATE_COUNT] =
{
    { WAR_STATE_IDLE,       wst_enterIdleState,       wst_leaveIdleState,       wst_updateIdleState,       wst_freeIdleState       },
    { WAR_STATE_MOVE,       wst_enterMoveState,       wst_leaveMoveState,       wst_updateMoveState,       wst_freeMoveState       },
    { WAR_STATE_PATROL,     wst_enterPatrolState,     wst_leavePatrolState,     wst_updatePatrolState,     wst_freePatrolState     },
    { WAR_STATE_FOLLOW,     wst_enterFollowState,     wst_leaveFollowState,     wst_updateFollowState,     wst_freeFollowState     },
    { WAR_STATE_ATTACK,     wst_enterAttackState,     wst_leaveAttackState,     wst_updateAttackState,     wst_freeAttackState     },
    { WAR_STATE_GOLD,       wst_enterGatherGoldState, wst_leaveGatherGoldState, wst_updateGatherGoldState, wst_freeGatherGoldState },
    { WAR_STATE_MINING,     wst_enterMiningState,     wst_leaveMiningState,     wst_updateMiningState,     wst_freeMiningState     },
    { WAR_STATE_WOOD,       wst_enterGatherWoodState, wst_leaveGatherWoodState, wst_updateGatherWoodState, wst_freeGatherWoodState },
    { WAR_STATE_CHOPPING,   wst_enterChoppingState,   wst_leaveChoppingState,   wst_updateChoppingState,   wst_freeChoppingState   },
    { WAR_STATE_DELIVER,    wst_enterDeliverState,    wst_leaveDeliverState,    wst_updateDeliverState,    wst_freeDeliverState    },
    { WAR_STATE_DEATH,      wst_enterDeathState,      wst_leaveDeathState,      wst_updateDeathState,      wst_freeDeathState      },
    { WAR_STATE_COLLAPSE,   wst_enterCollapseState,   wst_leaveCollapseState,   wst_updateCollapseState,   wst_freeCollapseState   },
    { WAR_STATE_TRAIN,      wst_enterTrainState,      wst_leaveTrainState,      wst_updateTrainState,      wst_freeTrainState      },
    { WAR_STATE_UPGRADE,    wst_enterUpgradeState,    wst_leaveUpgradeState,    wst_updateUpgradeState,    wst_freeUpgradeState    },
    { WAR_STATE_BUILD,      wst_enterBuildState,      wst_leaveBuildState,      wst_updateBuildState,      wst_freeBuildState      },
    { WAR_STATE_REPAIR,     wst_enterRepairState,     wst_leaveRepairState,     wst_updateRepairState,     wst_freeRepairState     },
    { WAR_STATE_REPAIRING,  wst_enterRepairingState,  wst_leaveRepairingState,  wst_updateRepairingState,  wst_freeRepairingState  },
    { WAR_STATE_CAST,       wst_enterCastState,       wst_leaveCastState,       wst_updateCastState,       wst_freeCastState       },
    { WAR_STATE_WAIT,       wst_enterWaitState,       wst_leaveWaitState,       wst_updateWaitState,       wst_freeWaitState       },
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
    state->nextRef  = WAR_STATE_REF_INVALID;

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
    WarStateBase* state = wst_deref(context, ref);
    assert(state);
    assert(s->occupied[ref.type][ref.idx]);

    // Recursively free chained next state.
    if (WAR_STATE_REF_IS_VALID(state->nextRef))
    {
        wst_freeStateRef(context, state->nextRef);
        state->nextRef = WAR_STATE_REF_INVALID;
    }

    // Run the type-specific free callback (releases Vec2List etc.).
    stateDescriptors[ref.type].freeFunc(context, state);

    s->occupied[ref.type][ref.idx] = false;
    s->freeLists[ref.type][s->freeCounts[ref.type]++] = ref.idx;
    s->activeCounts[ref.type]--;

    TracyCZoneEnd(ctx);
}

void wst_chainNext(WarContext* context, WarStateBase* from, WarStateBase* to)
{
    from->nextRef = wst_refOf(context, to);
}

void wst_changeNextState(WarContext* context, WarEntity* entity, WarStateBase* state, bool leaveState, bool enterState)
{
    TracyCZoneN(ctx, "wst_changeNextState", true);

    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    stateMachine->nextRef    = wst_refOf(context, state);
    stateMachine->leaveState = leaveState;
    stateMachine->enterState = enterState;

    TracyCZoneEnd(ctx);
}

bool wst_changeStateNextState(WarContext* context, WarEntity* entity, WarStateBase* state)
{
    TracyCZoneN(ctx, "wst_changeStateNextState", true);

    if (WAR_STATE_REF_IS_VALID(state->nextRef))
    {
        WarStateBase* next = wst_deref(context, state->nextRef);
        wst_changeNextState(context, entity, next, true, false);
        state->nextRef = WAR_STATE_REF_INVALID;

        TracyCZoneEnd(ctx);
        return true;
    }

    TracyCZoneEnd(ctx);
    return false;
}

WarStateBase* wst_getState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_getState", true);

    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    WarStateRef ref = stateMachine->currentRef;
    while (WAR_STATE_REF_IS_VALID(ref))
    {
        WarStateBase* state = wst_deref(context, ref);
        if (ref.type == type)
        {
            TracyCZoneEnd(ctx);
            return state;
        }
        ref = state->nextRef;
    }

    TracyCZoneEnd(ctx);
    return NULL;
}

WarStateBase* wst_getDirectState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_getDirectState", true);

    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    WarStateRef ref = stateMachine->currentRef;
    WarStateBase* result = WAR_STATE_REF_IS_VALID(ref) && ref.type == type ? wst_deref(context, ref) : NULL;

    TracyCZoneEnd(ctx);
    return result;
}

WarStateBase* wst_getNextState(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_getNextState", true);

    WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
    assert(stateMachine);

    WarStateRef ref = stateMachine->nextRef;
    WarStateBase* result = WAR_STATE_REF_IS_VALID(ref) && ref.type == type ? wst_deref(context, ref) : NULL;

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

    bool result = wst_getNextState(context, entity, type) != NULL;

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

void wst_enterState(WarContext* context, WarEntity* entity, WarStateBase* state)
{
    TracyCZoneN(ctx, "wst_enterState", true);

    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d for entity %d", state->type, entity->id);
        TracyCZoneEnd(ctx);
        return;
    }

    stateDescriptors[state->type].enterFunc(context, entity, state);

    TracyCZoneEnd(ctx);
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
        logError("Unkown state %d for entity %d", state->type, entity->id);
        TracyCZoneEnd(ctx);
        return;
    }

    stateDescriptors[state->type].leaveFunc(context, entity, state);
    wst_freeState(context, state);

    TracyCZoneEnd(ctx);
}

void wst_updateStateMachine(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "wst_updateStateMachine", true);

    if (we_isComponentEnabled(context, entity, COMP_STATE_MACHINE))
    {
        WarStateMachineComponent* stateMachine = we_getStateMachineComponent(context, entity);
        assert(stateMachine);

        // the wst_enterState could potentially change state if it determine that is not ready to start the current state
        while (WAR_STATE_REF_IS_VALID(stateMachine->nextRef))
        {
            WarStateBase* current = wst_deref(context, stateMachine->currentRef);

            if (stateMachine->leaveState && current)
                wst_leaveState(context, entity, current);

            stateMachine->currentRef = stateMachine->nextRef;
            stateMachine->nextRef    = WAR_STATE_REF_INVALID;

            WarStateBase* next = wst_deref(context, stateMachine->currentRef);
            if (stateMachine->enterState)
                wst_enterState(context, entity, next);
        }

        WarStateBase* currentState = wst_deref(context, stateMachine->currentRef);
        if (!currentState)
        {
            TracyCZoneEnd(ctx);
            return;
        }

        if (currentState->type == WAR_STATE_MOVE)
        {
            // NOTE: Move state are updated in `updateMoveStates` function
            TracyCZoneEnd(ctx);
            return;
        }

        if (currentState->delay > 0)
        {
            currentState->nextUpdateGameTime = context->gameTime + currentState->delay;
            currentState->delay = 0;
        }

        if (context->gameTime >= currentState->nextUpdateGameTime)
        {
            if (!inRange(currentState->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
            {
                logError("Unkown state %d for entity %d", currentState->type, entity->id);
                TracyCZoneEnd(ctx);
                return;
            }

            stateDescriptors[currentState->type].updateFunc(context, entity, currentState);
        }
    }

    TracyCZoneEnd(ctx);
}

void wst_freeState(WarContext* context, WarStateBase* state)
{
    TracyCZoneN(ctx, "wst_freeState", true);

    if (!state)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    if (!inRange(state->type, WAR_STATE_IDLE, WAR_STATE_COUNT))
    {
        logError("Unkown state %d", state->type);
        TracyCZoneEnd(ctx);
        return;
    }

    wst_freeStateRef(context, wst_refOf(context, state));

    TracyCZoneEnd(ctx);
}
