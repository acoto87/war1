#include <assert.h>
#include <stdint.h>
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

static bool wst_isStateTypeValid(WarStateType type)
{
    return type >= WAR_STATE_IDLE && type < WAR_STATE_COUNT;
}

static bool wst_stateRefsEqual(WarStateRef left, WarStateRef right)
{
    return left.type == right.type &&
           left.idx == right.idx &&
           left.generation == right.generation;
}

static bool wst_isAllocatedStateRef(const WarStateStorage* storage, WarStateRef ref)
{
    if (!wst_isStateTypeValid(ref.type))
    {
        return false;
    }

    if (ref.idx < 0 || ref.idx >= MAX_STATES_PER_TYPE || ref.generation == 0)
    {
        return false;
    }

    return storage->occupied[ref.type][ref.idx] &&
           storage->generations[ref.type][ref.idx] == ref.generation;
}

static bool wst_isStateRefOwned(WarContext* context, WarStateRef ref);

WarStateBase* wst_deref(WarContext* context, WarStateRef ref)
{
    WarEntityManager* manager = we_getEntityManager(context);
    if (!manager)
    {
        return NULL;
    }

    WarStateStorage* s = &manager->stateStorage;
    if (!wst_isAllocatedStateRef(s, ref))
    {
        return NULL;
    }

    char* base = (char*)wst_getTypeArray(s, ref.type);
    return (WarStateBase*)(base + (size_t)ref.idx * stateTypeSizes[ref.type]);
}

WarStateRef wst_refOf(WarContext* context, const WarStateBase* state)
{
    if (!state || !wst_isStateTypeValid(state->type))
    {
        return WAR_STATE_REF_INVALID;
    }

    WarEntityManager* manager = we_getEntityManager(context);
    if (!manager)
    {
        return WAR_STATE_REF_INVALID;
    }

    WarStateStorage* s = &manager->stateStorage;
    char* arrayBase = (char*)wst_getTypeArray(s, state->type);
    const uintptr_t arrayAddress = (uintptr_t)arrayBase;
    const uintptr_t stateAddress = (uintptr_t)state;
    const size_t stateSize = stateTypeSizes[state->type];
    const size_t arraySize = stateSize * MAX_STATES_PER_TYPE;

    if (stateAddress < arrayAddress || stateAddress >= arrayAddress + arraySize)
    {
        return WAR_STATE_REF_INVALID;
    }

    const size_t offset = (size_t)(stateAddress - arrayAddress);
    if (offset % stateSize != 0)
    {
        return WAR_STATE_REF_INVALID;
    }

    const s32 idx = (s32)(offset / stateSize);
    WarStateRef ref =
    {
        .type = state->type,
        .idx = idx,
        .generation = s->generations[state->type][idx]
    };

    if (!wst_isAllocatedStateRef(s, ref))
    {
        return WAR_STATE_REF_INVALID;
    }

    return ref;
}

void wst_initStorage(WarStateStorage* s)
{
    memset(s->occupied, 0, sizeof(s->occupied));
    memset(s->activeCounts, 0, sizeof(s->activeCounts));
    for (s32 t = 0; t < WAR_STATE_COUNT; t++)
    {
        s->freeCounts[t] = MAX_STATES_PER_TYPE;
        for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
        {
            s->generations[t][i] = 1;
            s->freeLists[t][i] = MAX_STATES_PER_TYPE - 1 - i;
        }
    }
}

WarStateRef wst_allocState(WarContext* context, WarStateType type, WarEntityId entityId)
{
    TracyCZoneN(ctx, "wst_allocState", true);

    if (!wst_isStateTypeValid(type))
    {
        logError("Unknown state type %d in wst_allocState", type);
        TracyCZoneEnd(ctx);
        return WAR_STATE_REF_INVALID;
    }

    WarEntityManager* manager = we_getEntityManager(context);
    if (!manager)
    {
        TracyCZoneEnd(ctx);
        return WAR_STATE_REF_INVALID;
    }

    WarStateStorage* s = &manager->stateStorage;
    if (s->freeCounts[type] <= 0)
    {
        logWarning("State storage exhausted for type %d", type);
        TracyCZoneEnd(ctx);
        return WAR_STATE_REF_INVALID;
    }

    if (s->freeCounts[type] > MAX_STATES_PER_TYPE)
    {
        logError("Corrupt state free count for type %d", type);
        TracyCZoneEnd(ctx);
        return WAR_STATE_REF_INVALID;
    }

    const s32 freeListIndex = s->freeCounts[type] - 1;
    const s32 idx = s->freeLists[type][freeListIndex];
    if (idx < 0 || idx >= MAX_STATES_PER_TYPE || s->occupied[type][idx])
    {
        logError("Corrupt state free-list for type %d", type);
        TracyCZoneEnd(ctx);
        return WAR_STATE_REF_INVALID;
    }

    s->freeCounts[type] = freeListIndex;
    s->occupied[type][idx] = true;
    s->activeCounts[type]++;

    WarStateBase* state = (WarStateBase*)((char*)wst_getTypeArray(s, type) + (size_t)idx * stateTypeSizes[type]);
    memset(state, 0, stateTypeSizes[type]);
    state->type     = type;
    state->entityId = entityId;

    TracyCZoneEnd(ctx);
    return (WarStateRef)
    {
        .type = type,
        .idx = idx,
        .generation = s->generations[type][idx]
    };
}

static void wst_releaseStateRef(WarContext* context, WarStateRef ref)
{
    TracyCZoneN(ctx, "wst_releaseStateRef", true);

    WarEntityManager* manager = we_getEntityManager(context);
    if (!manager)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    WarStateStorage* s = &manager->stateStorage;
    if (!wst_isAllocatedStateRef(s, ref))
    {
        TracyCZoneEnd(ctx);
        return;
    }

    if (s->freeCounts[ref.type] >= MAX_STATES_PER_TYPE ||
        s->activeCounts[ref.type] <= 0)
    {
        logError("Corrupt state counts for type %d", ref.type);
        TracyCZoneEnd(ctx);
        return;
    }

    s->occupied[ref.type][ref.idx] = false;
    s->generations[ref.type][ref.idx]++;
    if (s->generations[ref.type][ref.idx] == 0)
    {
        s->generations[ref.type][ref.idx] = 1;
    }
    s->freeLists[ref.type][s->freeCounts[ref.type]] = ref.idx;
    s->freeCounts[ref.type]++;
    s->activeCounts[ref.type]--;

    TracyCZoneEnd(ctx);
}

static bool wst_shouldReplacePending(const WarTransitionRequest* pending, const WarTransitionRequest* request)
{
    if (pending->operation == WAR_STATE_OP_NONE)
    {
        return true;
    }

    if (request->cause != pending->cause)
    {
        return request->cause > pending->cause;
    }

    bool pendingHasState = pending->operation == WAR_STATE_OP_PUSH ||
                           pending->operation == WAR_STATE_OP_REPLACE ||
                           pending->operation == WAR_STATE_OP_RESET;
    bool pendingIsTransactional = pending->stateRef.type == WAR_STATE_TRAIN ||
                                  pending->stateRef.type == WAR_STATE_UPGRADE ||
                                  pending->stateRef.type == WAR_STATE_BUILD;

    // A player can cancel the exact transaction that its earlier order would
    // commit. Other equal-priority requests still keep first-submitted order.
    if (request->cause == WAR_TRANSITION_CAUSE_PLAYER_ORDER &&
        request->cancellation &&
        pendingHasState &&
        pendingIsTransactional &&
        wst_stateRefsEqual(request->cancellationTargetRef, pending->stateRef))
    {
        return true;
    }

    // Requests are sequenced at submission, so an equal-priority request is
    // always later even when the per-entity sequence counter wraps.
    return false;
}

static bool wst_operationHasState(WarStateOp operation)
{
    return operation == WAR_STATE_OP_PUSH ||
           operation == WAR_STATE_OP_REPLACE ||
           operation == WAR_STATE_OP_RESET;
}

static bool wst_isTransactionalStateType(WarStateType type)
{
    return type == WAR_STATE_TRAIN ||
           type == WAR_STATE_UPGRADE ||
           type == WAR_STATE_BUILD;
}

static bool wst_isTransitionCauseValid(WarTransitionCause cause)
{
    switch (cause)
    {
        case WAR_TRANSITION_CAUSE_INITIALIZATION:
        case WAR_TRANSITION_CAUSE_COMPLETION:
        case WAR_TRANSITION_CAUSE_AUTONOMOUS:
        case WAR_TRANSITION_CAUSE_AI_ORDER:
        case WAR_TRANSITION_CAUSE_PLAYER_ORDER:
        case WAR_TRANSITION_CAUSE_STATUS:
        case WAR_TRANSITION_CAUSE_SCRIPT:
        case WAR_TRANSITION_CAUSE_LIFECYCLE:
            return true;
        default:
            return false;
    }
}

static bool wst_isCanonicalInvalidRef(WarStateRef ref)
{
    return wst_stateRefsEqual(ref, WAR_STATE_REF_INVALID);
}

static bool wst_isStateRefOnMachineStack(const WarStateMachineComponent* sm, WarStateRef ref)
{
    const u8 depth = MIN(sm->depth, WAR_STATE_STACK_DEPTH);
    for (u8 d = 0; d < depth; d++)
    {
        if (wst_stateRefsEqual(sm->stack[d], ref))
        {
            return true;
        }
    }

    return false;
}

static bool wst_isStateRefOnStack(WarContext* context, WarStateRef ref)
{
    WarEntityManager* manager = we_getEntityManager(context);
    if (!manager)
    {
        return false;
    }

    WarStateMachineStorage* stateMachines = &manager->stateMachines;
    for (s32 i = 0; i < stateMachines->count; i++)
    {
        WarStateMachineComponent* sm = &stateMachines->dense[i];
        if (wst_isStateRefOnMachineStack(sm, ref))
        {
            return true;
        }
    }

    return false;
}

static bool wst_isStateRefPendingElsewhere(WarContext* context, const WarStateMachineComponent* owner, WarStateRef ref)
{
    WarEntityManager* manager = we_getEntityManager(context);
    if (!manager)
    {
        return false;
    }

    WarStateMachineStorage* stateMachines = &manager->stateMachines;
    for (s32 i = 0; i < stateMachines->count; i++)
    {
        WarStateMachineComponent* sm = &stateMachines->dense[i];
        if (sm != owner &&
            sm->pending.operation != WAR_STATE_OP_NONE &&
            (wst_stateRefsEqual(sm->pending.stateRef, ref) ||
             wst_stateRefsEqual(sm->pending.cancellationTargetRef, ref)))
        {
            return true;
        }
    }

    return false;
}

static bool wst_isStateRefOwned(WarContext* context, WarStateRef ref)
{
    if (wst_isStateRefOnStack(context, ref))
    {
        return true;
    }

    WarEntityManager* manager = we_getEntityManager(context);
    if (!manager)
    {
        return false;
    }

    WarStateMachineStorage* stateMachines = &manager->stateMachines;
    for (s32 i = 0; i < stateMachines->count; i++)
    {
        WarStateMachineComponent* sm = &stateMachines->dense[i];
        if (sm->pending.operation != WAR_STATE_OP_NONE &&
            (wst_stateRefsEqual(sm->pending.stateRef, ref) ||
             wst_stateRefsEqual(sm->pending.cancellationTargetRef, ref)))
        {
            return true;
        }
    }

    return false;
}

void wst_freeStateRef(WarContext* context, WarStateRef ref)
{
    TracyCZoneN(ctx, "wst_freeStateRef", true);

    if (!wst_isStateRefOwned(context, ref))
    {
        wst_releaseStateRef(context, ref);
    }

    TracyCZoneEnd(ctx);
}

static void wst_freeUnownedStateRef(WarContext* context, WarStateRef ref)
{
    if (!wst_deref(context, ref) || wst_isStateRefOwned(context, ref))
    {
        return;
    }

    wst_releaseStateRef(context, ref);
}

static void wst_freeUnownedTransitionRefs(WarContext* context, const WarTransitionRequest* request)
{
    wst_freeUnownedStateRef(context, request->stateRef);
    if (!wst_stateRefsEqual(request->cancellationTargetRef, request->stateRef))
    {
        wst_freeUnownedStateRef(context, request->cancellationTargetRef);
    }
}

static bool wst_isCancellationTargetValid(WarContext* context, WarEntity* entity, WarStateMachineComponent* sm, const WarTransitionRequest* request)
{
    if (!request->cancellation)
    {
        return !WAR_STATE_REF_IS_VALID(request->cancellationTargetRef);
    }

    if (request->operation != WAR_STATE_OP_RESET ||
        !WAR_STATE_REF_IS_VALID(request->cancellationTargetRef) ||
        !wst_isTransactionalStateType(request->cancellationTargetRef.type) ||
        wst_stateRefsEqual(request->stateRef, request->cancellationTargetRef))
    {
        return false;
    }

    WarStateBase* target = wst_deref(context, request->cancellationTargetRef);
    if (!target || target->entityId != entity->id)
    {
        return false;
    }

    return wst_isStateRefOnMachineStack(sm, request->cancellationTargetRef) ||
           wst_stateRefsEqual(sm->pending.stateRef, request->cancellationTargetRef) ||
           wst_stateRefsEqual(sm->pending.cancellationTargetRef, request->cancellationTargetRef);
}

static bool wst_isTransitionRequestValid(WarContext* context, WarEntity* entity, WarStateMachineComponent* sm, const WarTransitionRequest* request)
{
    if (!wst_isTransitionCauseValid(request->cause))
    {
        return false;
    }

    if (!wst_isCancellationTargetValid(context, entity, sm, request))
    {
        return false;
    }

    if (request->operation == WAR_STATE_OP_POP)
    {
        return wst_isCanonicalInvalidRef(request->stateRef);
    }

    if (!wst_operationHasState(request->operation))
    {
        return false;
    }

    WarStateBase* state = wst_deref(context, request->stateRef);
    if (!state || state->entityId != entity->id)
    {
        return false;
    }

    if (wst_isStateRefOnStack(context, request->stateRef))
    {
        return false;
    }

    return !wst_isStateRefPendingElsewhere(context, sm, request->stateRef);
}

bool wst_submitTransition(WarContext* context, WarEntity* entity, WarTransitionRequest request)
{
    if (!entity)
    {
        wst_freeUnownedTransitionRefs(context, &request);
        return false;
    }

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    if (!sm)
    {
        wst_freeUnownedTransitionRefs(context, &request);
        return false;
    }

    if (!wst_isTransitionRequestValid(context, entity, sm, &request))
    {
        wst_freeUnownedTransitionRefs(context, &request);
        return false;
    }

    request.sequence = sm->nextTransitionSequence;
    sm->nextTransitionSequence++;

    WarTransitionRequest* pending = &sm->pending;

    // WarTransitionCause values carry priority. The pending request wins ties;
    // sequence records per-entity submission order.
    if (wst_shouldReplacePending(pending, &request))
    {
        WarTransitionRequest replaced = *pending;
        *pending = request;
        wst_freeUnownedTransitionRefs(context, &replaced);
        return true;
    }

    wst_freeUnownedTransitionRefs(context, &request);
    return false;
}

bool wst_pushState(WarContext* context, WarEntity* entity, WarStateBase* state, WarTransitionCause cause)
{
    if (!state)
    {
        return false;
    }

    return wst_submitTransition(context, entity, (WarTransitionRequest){
        .stateRef = wst_refOf(context, state),
        .cancellationTargetRef = WAR_STATE_REF_INVALID,
        .operation = WAR_STATE_OP_PUSH,
        .cause = cause
    });
}

bool wst_popState(WarContext* context, WarEntity* entity, WarTransitionCause cause)
{
    return wst_submitTransition(context, entity, (WarTransitionRequest){
        .stateRef = WAR_STATE_REF_INVALID,
        .cancellationTargetRef = WAR_STATE_REF_INVALID,
        .operation = WAR_STATE_OP_POP,
        .cause = cause
    });
}

bool wst_replaceState(WarContext* context, WarEntity* entity, WarStateBase* state, WarTransitionCause cause)
{
    if (!state)
    {
        return false;
    }

    return wst_submitTransition(context, entity, (WarTransitionRequest){
        .stateRef = wst_refOf(context, state),
        .cancellationTargetRef = WAR_STATE_REF_INVALID,
        .operation = WAR_STATE_OP_REPLACE,
        .cause = cause
    });
}

bool wst_resetState(WarContext* context, WarEntity* entity, WarStateBase* state, WarTransitionCause cause)
{
    if (!state)
    {
        return false;
    }

    return wst_submitTransition(context, entity, (WarTransitionRequest){
        .stateRef = wst_refOf(context, state),
        .cancellationTargetRef = WAR_STATE_REF_INVALID,
        .operation = WAR_STATE_OP_RESET,
        .cause = cause
    });
}

static WarStateRef wst_findCancellationTarget(WarContext* context, WarStateMachineComponent* sm)
{
    const u8 depth = MIN(sm->depth, WAR_STATE_STACK_DEPTH);
    for (s32 i = (s32)depth - 1; i >= 0; i--)
    {
        WarStateRef ref = sm->stack[i];
        if (wst_isTransactionalStateType(ref.type) && wst_deref(context, ref))
        {
            return ref;
        }
    }

    WarTransitionRequest* pending = &sm->pending;
    if (wst_operationHasState(pending->operation) &&
        wst_isTransactionalStateType(pending->stateRef.type) &&
        wst_deref(context, pending->stateRef))
    {
        return pending->stateRef;
    }

    if (pending->cancellation &&
        wst_isTransactionalStateType(pending->cancellationTargetRef.type) &&
        wst_deref(context, pending->cancellationTargetRef))
    {
        return pending->cancellationTargetRef;
    }

    return WAR_STATE_REF_INVALID;
}

bool wst_resetStateForCancellation(WarContext* context, WarEntity* entity, WarStateBase* state, WarTransitionCause cause)
{
    if (!state)
    {
        return false;
    }

    if (!entity)
    {
        wst_freeStateRef(context, wst_refOf(context, state));
        return false;
    }

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    if (!sm)
    {
        wst_freeStateRef(context, wst_refOf(context, state));
        return false;
    }

    WarStateRef cancellationTargetRef = wst_findCancellationTarget(context, sm);
    return wst_submitTransition(context, entity, (WarTransitionRequest)
    {
        .stateRef = wst_refOf(context, state),
        .cancellationTargetRef = cancellationTargetRef,
        .operation = WAR_STATE_OP_RESET,
        .cause = cause,
        .cancellation = true
    });
}

WarStateBase* wst_currentState(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    if (!sm || sm->depth == 0) return NULL;
    return wst_deref(context, sm->stack[sm->depth - 1]);
}

bool wst_isCurrentState(WarContext* context, WarEntity* entity, WarState* state)
{
    return wst_currentState(context, entity) == state;
}

bool wst_hasStateInStack(WarContext* context, WarEntity* entity, WarStateType type)
{
    TracyCZoneN(ctx, "wst_hasStateInStack", true);

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    for (u8 i = 0; i < sm->depth; i++)
    {
        if (sm->stack[i].type == type && wst_deref(context, sm->stack[i]))
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

bool wst_isNextUpdateTime(WarContext* context, WarState* state)
{
    if (state->delay > 0)
    {
        state->nextUpdateGameTime = context->gameTime + state->delay;
        state->delay = 0;
    }
    return context->gameTime >= state->nextUpdateGameTime;
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

WarStateIdle* wst_getIdleState(WarContext* context, WarEntity* entity) { return (WarStateIdle*)wst_getDirectState(context, entity, WAR_STATE_IDLE); }
WarStateMove* wst_getMoveState(WarContext* context, WarEntity* entity) { return (WarStateMove*)wst_getState(context, entity, WAR_STATE_MOVE); }
WarStatePatrol* wst_getPatrolState(WarContext* context, WarEntity* entity) { return (WarStatePatrol*)wst_getState(context, entity, WAR_STATE_PATROL); }
WarStateFollow* wst_getFollowState(WarContext* context, WarEntity* entity) { return (WarStateFollow*)wst_getState(context, entity, WAR_STATE_FOLLOW); }
WarStateAttack* wst_getAttackState(WarContext* context, WarEntity* entity) { return (WarStateAttack*)wst_getState(context, entity, WAR_STATE_ATTACK); }
WarStateDeath* wst_getDeathState(WarContext* context, WarEntity* entity) { return (WarStateDeath*)wst_getState(context, entity, WAR_STATE_DEATH); }
WarStateCollapse* wst_getCollapseState(WarContext* context, WarEntity* entity) { return (WarStateCollapse*)wst_getState(context, entity, WAR_STATE_COLLAPSE); }
WarStateGold* wst_getGatherGoldState(WarContext* context, WarEntity* entity) { return (WarStateGold*)wst_getState(context, entity, WAR_STATE_GOLD); }
WarStateMining* wst_getMiningState(WarContext* context, WarEntity* entity) { return (WarStateMining*)wst_getState(context, entity, WAR_STATE_MINING); }
WarStateWood* wst_getGatherWoodState(WarContext* context, WarEntity* entity) { return (WarStateWood*)wst_getState(context, entity, WAR_STATE_WOOD); }
WarStateChopping* wst_getChoppingState(WarContext* context, WarEntity* entity) { return (WarStateChopping*)wst_getState(context, entity, WAR_STATE_CHOPPING); }
WarStateDeliver* wst_getDeliverState(WarContext* context, WarEntity* entity) { return (WarStateDeliver*)wst_getState(context, entity, WAR_STATE_DELIVER); }
WarStateTrain* wst_getTrainState(WarContext* context, WarEntity* entity) { return (WarStateTrain*)wst_getState(context, entity, WAR_STATE_TRAIN); }
WarStateUpgrade* wst_getUpgradeState(WarContext* context, WarEntity* entity) { return (WarStateUpgrade*)wst_getState(context, entity, WAR_STATE_UPGRADE); }
WarStateBuild* wst_getBuildState(WarContext* context, WarEntity* entity) { return (WarStateBuild*)wst_getState(context, entity, WAR_STATE_BUILD); }
WarStateRepair* wst_getRepairState(WarContext* context, WarEntity* entity) { return (WarStateRepair*)wst_getState(context, entity, WAR_STATE_REPAIR); }
WarStateRepairing* wst_getRepairingState(WarContext* context, WarEntity* entity) { return (WarStateRepairing*)wst_getState(context, entity, WAR_STATE_REPAIRING); }
WarStateCast* wst_getCastState(WarContext* context, WarEntity* entity) { return (WarStateCast*)wst_getState(context, entity, WAR_STATE_CAST); }

bool wst_hasState(WarContext* context, WarEntity* entity, WarStateType type)
{
    return wst_getState(context, entity, type) != NULL;
}

bool wst_hasDirectState(WarContext* context, WarEntity* entity, WarStateType type)
{
    return wst_getDirectState(context, entity, type) != NULL;
}

bool wst_hasNextState(WarContext* context, WarEntity* entity, WarStateType type)
{
    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    assert(sm);

    WarTransitionRequest* pending = &sm->pending;
    return wst_operationHasState(pending->operation) &&
           pending->stateRef.type == type &&
           wst_deref(context, pending->stateRef) != NULL;
}

bool wst_isIdle(WarContext* context, WarEntity* entity) { return wst_hasDirectState(context, entity, WAR_STATE_IDLE); }
bool wst_isMoving(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_MOVE); }
bool wst_isPatrolling(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_PATROL); }
bool wst_isFollowing(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_FOLLOW); }
bool wst_isAttacking(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_ATTACK); }
bool wst_isDead(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_DEATH); }
bool wst_isCollapsing(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_COLLAPSE); }
bool wst_isGatheringGold(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_GOLD); }
bool wst_isMining(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_MINING); }
bool wst_isGatheringWood(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_WOOD); }
bool wst_isChopping(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_CHOPPING); }
bool wst_isDelivering(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_DELIVER); }
bool wst_isTraining(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_TRAIN); }
bool wst_isUpgrading(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_UPGRADE); }
bool wst_isBuilding(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_BUILD); }
bool wst_isRepairing(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_REPAIR); }
bool wst_isRepairing2(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_REPAIRING); }
bool wst_isCasting(WarContext* context, WarEntity* entity) { return wst_hasState(context, entity, WAR_STATE_CAST); }
bool wst_isGoingToIdle(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_IDLE); }
bool wst_isGoingToMove(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_MOVE); }
bool wst_isGoingToPatrol(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_PATROL); }
bool wst_isGoingToFollow(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_FOLLOW); }
bool wst_isGoingToAttack(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_ATTACK); }
bool wst_isGoingToDie(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_DEATH); }
bool wst_isGoingToCollapse(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_COLLAPSE); }
bool wst_isGoingToGatherGold(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_GOLD); }
bool wst_isGoingToMine(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_MINING); }
bool wst_isGoingToGatherWood(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_WOOD); }
bool wst_isGoingToChop(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_CHOPPING); }
bool wst_isGoingToDeliver(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_DELIVER); }
bool wst_isGoingToTrain(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_TRAIN); }
bool wst_isGoingToUpgrade(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_UPGRADE); }
bool wst_isGoingToBuild(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_BUILD); }
bool wst_isGoingToRepair(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_REPAIR); }
bool wst_isGoingToRepair2(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_REPAIRING); }
bool wst_isGoingToCast(WarContext* context, WarEntity* entity) { return wst_hasNextState(context, entity, WAR_STATE_CAST); }

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

    const WarStateRef ref = wst_refOf(context, state);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return;
    }

    stateDescriptors[state->type].leaveFunc(context, entity, state);
    wst_releaseStateRef(context, ref);

    TracyCZoneEnd(ctx);
}

static bool wst_installIdleState(WarContext* context, WarEntity* entity, WarStateMachineComponent* sm)
{
    WarStateIdle* idle = wst_createIdleState(context, entity, true);
    if (!idle)
    {
        logError("Unable to restore IDLE state for entity %d", entity->id);
        return false;
    }

    WarStateRef idleRef = wst_refOf(context, (WarStateBase*)idle);
    if (!WAR_STATE_REF_IS_VALID(idleRef))
    {
        return false;
    }

    sm->stack[0] = idleRef;
    sm->depth = 1;
    return true;
}

static bool wst_commitCancellation(WarContext* context, WarEntity* entity, WarStateMachineComponent* sm, const WarTransitionRequest* request)
{
    if (!request->cancellation)
    {
        return true;
    }

    WarStateBase* target = wst_deref(context, request->cancellationTargetRef);
    if (!target || target->entityId != entity->id || !wst_isTransactionalStateType(target->type))
    {
        return false;
    }

    switch (target->type)
    {
        case WAR_STATE_TRAIN:
            ((WarStateTrain*)target)->cancelled = true;
            break;
        case WAR_STATE_UPGRADE:
            ((WarStateUpgrade*)target)->cancelled = true;
            break;
        case WAR_STATE_BUILD:
            ((WarStateBuild*)target)->cancelled = true;
            break;
        default:
            return false;
    }

    if (!wst_isStateRefOnMachineStack(sm, request->cancellationTargetRef))
    {
        wst_leaveState(context, entity, target);
    }

    return true;
}

static void wst_resolveCommittedBuildCancellation(WarContext* context, WarStateMachineComponent* sm, WarTransitionRequest* request)
{
    if (!request->cancellation ||
        request->cancellationTargetRef.type != WAR_STATE_BUILD ||
        !wst_isStateRefOnMachineStack(sm, request->cancellationTargetRef))
    {
        return;
    }

    WarStateBuild* build = (WarStateBuild*)wst_deref(context, request->cancellationTargetRef);
    if (!build || !build->outputCommitted)
    {
        return;
    }

    WarStateRef cancellationOutcomeRef = request->stateRef;
    request->stateRef = WAR_STATE_REF_INVALID;
    request->cancellationTargetRef = WAR_STATE_REF_INVALID;
    request->operation = WAR_STATE_OP_POP;
    request->cause = WAR_TRANSITION_CAUSE_COMPLETION;
    request->cancellation = false;

    wst_freeUnownedStateRef(context, cancellationOutcomeRef);
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

        WarTransitionRequest* pending = &sm->pending;
        if (pending->operation == WAR_STATE_OP_NONE)
        {
            continue;
        }

        WarTransitionRequest request = *pending;
        *pending = (WarTransitionRequest)
        {
            .stateRef = WAR_STATE_REF_INVALID,
            .cancellationTargetRef = WAR_STATE_REF_INVALID
        };

        if (sm->depth > WAR_STATE_STACK_DEPTH)
        {
            logError("Invalid state stack depth %d for entity %d", sm->depth, entity->id);
            sm->depth = WAR_STATE_STACK_DEPTH;
        }

        if (wst_operationHasState(request.operation))
        {
            WarStateBase* candidate = wst_deref(context, request.stateRef);
            if (!candidate || candidate->entityId != entity->id)
            {
                wst_freeUnownedTransitionRefs(context, &request);
                continue;
            }
        }

        wst_resolveCommittedBuildCancellation(context, sm, &request);

        if (!wst_commitCancellation(context, entity, sm, &request))
        {
            wst_freeUnownedTransitionRefs(context, &request);
            continue;
        }

        switch (request.operation)
        {
            case WAR_STATE_OP_PUSH:
            {
                if (sm->depth < WAR_STATE_STACK_DEPTH)
                {
                    sm->stack[sm->depth] = request.stateRef;
                    sm->depth++;
                }
                else
                {
                    // Preserve the historical overflow behavior: a PUSH on a
                    // full stack safely replaces only the active top state.
                    assert(sm->depth == WAR_STATE_STACK_DEPTH);
                    WarStateRef topRef = sm->stack[sm->depth - 1];
                    WarStateBase* top = wst_deref(context, topRef);
                    if (top)
                    {
                        wst_leaveState(context, entity, top);
                    }
                    sm->stack[sm->depth - 1] = request.stateRef;
                }
                break;
            }
            case WAR_STATE_OP_POP:
            {
                if (sm->depth > 0)
                {
                    WarStateRef topRef = sm->stack[sm->depth - 1];
                    WarStateBase* top = wst_deref(context, topRef);
                    if (top)
                    {
                        wst_leaveState(context, entity, top);
                    }
                    sm->depth--;
                    sm->stack[sm->depth] = WAR_STATE_REF_INVALID;
                }

                if (sm->depth == 0)
                {
                    // POP on an empty stack is defined as restoring IDLE.
                    wst_installIdleState(context, entity, sm);
                }
                break;
            }
            case WAR_STATE_OP_REPLACE:
            {
                if (sm->depth > 0)
                {
                    WarStateRef topRef = sm->stack[sm->depth - 1];
                    WarStateBase* top = wst_deref(context, topRef);
                    if (top)
                    {
                        wst_leaveState(context, entity, top);
                    }
                    sm->stack[sm->depth - 1] = request.stateRef;
                }
                else
                {
                    // REPLACE on an empty stack installs the candidate.
                    sm->stack[0] = request.stateRef;
                    sm->depth = 1;
                }
                break;
            }
            case WAR_STATE_OP_RESET:
            {
                for (s32 d = (s32)sm->depth - 1; d >= 0; d--)
                {
                    WarStateBase* s = wst_deref(context, sm->stack[d]);
                    if (s) wst_leaveState(context, entity, s);
                    sm->stack[d] = WAR_STATE_REF_INVALID;
                }
                sm->stack[0] = request.stateRef;
                sm->depth = 1;
                break;
            }
            default:
                wst_freeUnownedTransitionRefs(context, &request);
                break;
        }
        wst_freeUnownedStateRef(context, request.cancellationTargetRef);
    }

    TracyCZoneEnd(ctx);
}
