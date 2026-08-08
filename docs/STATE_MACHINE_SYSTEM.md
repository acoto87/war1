# State Machine Subsystem

**Module prefix:** `wst_`

**Primary files:** `src/war_state_machine.h`, `src/war_state_machine.c`, `src/war_state_machine_*.c`

**Storage:** `WarStateMachineComponent` and `WarStateStorage` in `src/war_entities.h`

**Tests:** `tests/war_state_machine_test.c`, `tests/war_test_context.c`

## Overview

The War1-C finite state machine is a fixed-capacity pushdown behavior system. Units and buildings retain behavior as a stack of typed state references:

```text
stack[0]  root order or persistent behavior
stack[1]  temporary child task
stack[2]  nested child task
stack[3]  active top state
```

Only the top state is active. Lower states remain allocated but paused until their child pops. This supports behavior such as:

```text
PATROL
  MOVE

ATTACK
  FOLLOW
    MOVE

GOLD
  FOLLOW
    MOVE
```

The subsystem is data-oriented:

- Each state type has its own fixed pool of 512 typed objects.
- Entities store `WarStateRef` values rather than persistent raw state pointers.
- References include a generation to reject stale slots after reuse.
- Each entity has a fixed stack depth of four.
- Each entity retains one deferred transition request, not a transition queue.
- State updates run in global type-oriented batches rather than through one per-entity dispatcher.

Direct player and AI orders usually `RESET` the stack. Temporary tasks usually `PUSH` and later `POP`. `REPLACE` changes only the active top while preserving lower context.

## Core Data Model

### State Base and Typed States

Every concrete state starts with `WarStateBase`:

```c
struct _WarStateBase
{
    WarStateType type;
    WarEntityId  entityId;
    f64 nextUpdateGameTime;
    f32 delay;
};
```

Concrete types embed the base as their first field:

```c
struct _WarStateFollow
{
    WarStateBase base;
    WarEntityId targetEntityId;
    vec2 targetPosition;
    s32 targetDistance;
};
```

Static assertions in `war_state_machine.h` enforce that `base` is at offset zero. There is no common union and no `nextState` pointer.

### State References

```c
struct _WarStateRef
{
    WarStateType type;
    s32 idx;
    u32 generation;
};
```

`WAR_STATE_REF_INVALID` is the canonical empty reference. `WAR_STATE_REF_IS_VALID(ref)` checks only the shape of a reference. Use `wst_deref()` to verify bounds, occupancy, and generation before accessing the state.

Important ownership rules:

- Store `WarStateRef` for retained ownership or deferred work.
- A raw pointer returned by a creator is convenient only while the slot is known to remain allocated.
- `wst_deref()` returns `NULL` for stale or released references.
- Releasing a slot increments its generation and skips generation zero.
- `wst_refOf()` accepts only an allocated, exactly aligned object in the correct typed pool.

### Per-Type State Pools

`WarStateStorage` contains one array per state type:

```c
#define MAX_STATES_PER_TYPE 512

struct _WarStateStorage
{
    WarStateIdle      idle[MAX_STATES_PER_TYPE];
    WarStateMove      move[MAX_STATES_PER_TYPE];
    /* ... one array for every state type ... */
    WarStateWait      wait[MAX_STATES_PER_TYPE];

    u32  generations[WAR_STATE_COUNT][MAX_STATES_PER_TYPE];
    bool occupied[WAR_STATE_COUNT][MAX_STATES_PER_TYPE];
    s32  freeLists[WAR_STATE_COUNT][MAX_STATES_PER_TYPE];
    s32  freeCounts[WAR_STATE_COUNT];
    s32  activeCounts[WAR_STATE_COUNT];
};
```

Allocation and release are O(1):

- `wst_allocState()` pops a per-type free-list slot, zeroes the concrete object, and initializes its base.
- Pool exhaustion logs a warning and returns `WAR_STATE_REF_INVALID`.
- Type-specific creators return `NULL` when allocation fails.
- Release marks the slot free, advances its generation, and pushes it onto the free list.
- `activeCounts[type]` counts allocated slots, including active, paused, pending, and temporarily unowned states.

`wst_freeStateRef()` is ownership-aware. It does not release a reference while that reference is on any entity stack or retained by a pending transition.

### State Machine Component

```c
struct _WarStateMachineComponent
{
    WarStateRef stack[WAR_STATE_STACK_DEPTH];
    u8 depth;

    WarTransitionRequest pending;
    u64 nextTransitionSequence;
};
```

The active state is `stack[depth - 1]`. Stack indices are bottom-to-top. `WAR_STATE_STACK_DEPTH` is currently four.

## State Catalog

There are 19 state types.

| State | Purpose | Registered lifecycle hooks |
|---|---|---|
| `WAR_STATE_IDLE` | Stand still and optionally acquire enemies | enter, exit |
| `WAR_STATE_MOVE` | Traverse inline waypoints with flow fields and RVO | enter, exit |
| `WAR_STATE_PATROL` | Retain a cyclic patrol order and push MOVE children | enter |
| `WAR_STATE_FOLLOW` | Approach an entity or stored position | none |
| `WAR_STATE_ATTACK` | Attack an entity or position; push range helpers | none |
| `WAR_STATE_GOLD` | Begin or resume a gold gathering cycle | none |
| `WAR_STATE_MINING` | Extract gold while inside a mine | enter, exit |
| `WAR_STATE_WOOD` | Move toward a selected tree | none |
| `WAR_STATE_CHOPPING` | Harvest wood at a tree | enter |
| `WAR_STATE_DELIVER` | Return carried resources to a depot | none |
| `WAR_STATE_DEATH` | Unit death sequence and removal | enter |
| `WAR_STATE_COLLAPSE` | Building collapse sequence and removal | enter |
| `WAR_STATE_TRAIN` | Transactional unit production | enter, exit |
| `WAR_STATE_UPGRADE` | Transactional research | enter, exit |
| `WAR_STATE_BUILD` | Transactional construction progress | enter, exit |
| `WAR_STATE_REPAIR` | Approach a building to repair | none |
| `WAR_STATE_REPAIRING` | Repair or construct from inside a building | enter, exit |
| `WAR_STATE_CAST` | Approach and execute a spell | none registered |
| `WAR_STATE_WAIT` | Wait until an absolute game time | enter, exit |

Creator declarations are in `war_state_machine.h`. All creators return typed pointers, for example:

```c
WarStateMove* wst_createMoveState(
    WarContext* context,
    WarEntity* entity,
    s32 positionCount,
    vec2 positions[],
    bool checkForAttacks);

WarStateTrain* wst_createTrainState(
    WarContext* context,
    WarEntity* entity,
    WarUnitType unitToBuild,
    f32 buildTime,
    s32 goldCost,
    s32 woodCost,
    WarAICommand* aiCommand);
```

MOVE and PATROL store up to 64 waypoints inline. They do not allocate waypoint lists.

## Lifecycle Descriptor

Lifecycle behavior is registered in `stateDescriptors`:

```c
typedef struct
{
    WarStateType type;
    void (*onEnter)(WarContext*, WarEntity*, WarStateBase*);
    void (*onPause)(WarContext*, WarEntity*, WarStateBase*, WarStatePauseReason);
    bool (*validate)(WarContext*, WarEntity*, WarStateBase*);
    void (*onResume)(WarContext*, WarEntity*, WarStateBase*, WarStateResumeReason);
    void (*onExit)(WarContext*, WarEntity*, WarStateBase*, WarStateExitReason);
    bool (*canInterrupt)(WarContext*, WarEntity*, WarStateBase*, WarInterruptKind);
    u32 defaultInterruptMask;
} WarStateDescriptor;
```

Lifecycle dispatch functions are:

- `wst_enterState()`
- `wst_pauseState()`
- `wst_validateState()`
- `wst_resumeState()`
- `wst_exitState()`

`wst_exitState()` invokes the optional exit callback and then releases the state slot. Rejected, displaced, and removal-time pending candidates are released directly because they never entered. A pending TRAIN, UPGRADE, or BUILD selected for transactional cancellation is an exception: cancellation deliberately exits it so transaction cleanup and refunds can run.

Current implementation status:

- Enter and exit hooks are registered only for the states listed in the catalog.
- All current `onPause`, `validate`, and `onResume` descriptor entries are `NULL`.
- State updates are not descriptor callbacks. Each type has a dedicated batch updater.
- Exit reasons are passed to registered callbacks, but current exit implementations do not branch on them.
- PUSH calls validation before installing a child. REPLACE and RESET currently do not validate candidates.

## Deferred Transitions

### Transition Request

```c
struct _WarTransitionRequest
{
    WarStateRef stateRef;
    WarStateRef cancellationTargetRef;
    WarStateOp operation;
    WarStateResult result;
    WarTransitionCause cause;
    u64 sequence;
    bool cancellation;
};
```

The public wrappers are:

```c
bool wst_pushState(WarContext*, WarEntity*, WarStateBase*, WarTransitionCause);
bool wst_popState(WarContext*, WarEntity*, WarTransitionCause, WarStateResult);
bool wst_replaceState(WarContext*, WarEntity*, WarStateBase*, WarTransitionCause);
bool wst_resetState(WarContext*, WarEntity*, WarStateBase*, WarTransitionCause);
bool wst_resetStateForCancellation(
    WarContext*, WarEntity*, WarStateBase*, WarTransitionCause);
```

These functions submit requests. They do not immediately mutate the stack. `wst_processPendingTransitions()` consumes at most one request per enabled entity each time it runs.

Before creating a state for an interrupting command or reaction, call:

```c
bool wst_canSubmitTransition(
    WarContext* context,
    WarEntity* entity,
    WarInterruptKind interrupt);
```

The check is advisory: raw `wst_submitTransition()` does not enforce the interrupt mask itself. Command and autonomous code must perform the check before allocating the candidate.

### Causes and Priority

Cause values are the arbitration priority:

| Cause | Priority | Typical producer |
|---|---:|---|
| `WAR_TRANSITION_CAUSE_INITIALIZATION` | 10 | Initial IDLE installation |
| `WAR_TRANSITION_CAUSE_COMPLETION` | 20 | State success or failure |
| `WAR_TRANSITION_CAUSE_AUTONOMOUS` | 30 | Aggro or automatic reaction |
| `WAR_TRANSITION_CAUSE_AI_ORDER` | 40 | Explicit AI command |
| `WAR_TRANSITION_CAUSE_PLAYER_ORDER` | 50 | Explicit player command |
| `WAR_TRANSITION_CAUSE_STATUS` | 60 | Status interruption |
| `WAR_TRANSITION_CAUSE_SCRIPT` | 70 | Scenario or forced script |
| `WAR_TRANSITION_CAUSE_LIFECYCLE` | 80 | Death, collapse, removal-related behavior |

Arbitration rules:

- An empty pending slot accepts the request.
- A higher cause replaces a lower cause.
- Equal causes keep the first submitted request.
- A player cancellation may replace the exact pending TRAIN, UPGRADE, or BUILD transaction it targets.
- Rejected or displaced unowned candidate states are released automatically.
- Every valid submission consumes the entity-local sequence number, even if it loses arbitration.
- Sequence is deterministic metadata; it is not currently used as the equal-priority tiebreaker.

A `true` return means that the request became pending. It does not mean the transition has already committed.

### Operation Semantics

#### PUSH

Use PUSH for a temporary child task that should return to its parent:

1. Validate the child.
2. Pause the current top with `WAR_STATE_PAUSE_CHILD_PUSHED`.
3. Append and enter the child.

At full depth, the current implementation replaces only the active top while preserving the lower three entries. Treat this as an abnormal capacity condition and do not design behavior that depends on it. Do not PUSH onto an intentionally empty stack; use REPLACE or RESET.

#### POP

Use POP when the active state finishes:

1. Map the result to an exit reason.
2. Exit and release the top.
3. Resume the parent with child success or child failure.
4. If the stack becomes empty, attempt to allocate and enter a default IDLE state. IDLE pool exhaustion can leave the machine empty.

Every POP must specify a `WarStateResult`.

#### REPLACE

Use REPLACE to change only the active top while retaining lower context:

1. Exit and release the current top with `WAR_STATE_EXIT_REPLACED`.
2. Preserve the existing depth and lower entries.
3. Install and enter the replacement.

REPLACE on an empty stack installs the candidate as the root.

#### RESET

Use RESET for a new root order or lifecycle transition:

1. Exit and release all states from top to bottom.
2. Install the candidate at depth one.
3. Enter the new root.

Player and AI commands normally reset the old behavior stack. Lifecycle causes outrank ordinary commands through transition arbitration.

## State Results

POP results are defined by `WarStateResult`:

```text
NONE
SUCCESS
CANCELLED
TARGET_INVALID
TARGET_HIDDEN
BLOCKED
NO_PATH
NO_RESOURCE
NO_DESTINATION
INTERRUPTED
CONTAINER_DESTROYED
```

Current mapping:

- `SUCCESS` maps to `WAR_STATE_EXIT_COMPLETED` and child-success resume.
- `CANCELLED` maps to `WAR_STATE_EXIT_CANCELLED`.
- Invalid or hidden targets map to `WAR_STATE_EXIT_TARGET_INVALID`.
- Blocked, no path, no resource, no destination, and interrupted map to `WAR_STATE_EXIT_FAILED`.
- Container destruction maps to `WAR_STATE_EXIT_CONTAINER_DESTROYED`.
- `NONE` maps to `WAR_STATE_EXIT_FAILED`.
- Every non-success result maps to child-failure resume.

The exact result exists in the pending POP request but is not retained after commit. Parent resume currently receives only success or failure, and no state currently registers an `onResume` callback.

## Interruption Policy

`wst_canSubmitTransition()` evaluates the active descriptor's custom callback, when present, or its default interrupt mask.

Current default policy:

- IDLE, MOVE, PATROL, FOLLOW, gathering states, repair states, and WAIT accept all interrupt kinds.
- ATTACK rejects autonomous interruption but accepts player, AI, status, script, and lifecycle interruption.
- DEATH and COLLAPSE accept lifecycle interruption only.
- TRAIN and UPGRADE accept player, AI, and lifecycle interruption.
- BUILD accepts player, AI, status, script, and lifecycle interruption.
- CAST currently has mask zero.

`wst_canInterruptCast()` contains spell-specific logic but is not currently registered in the CAST descriptor. Through the descriptor path, CAST is therefore non-interruptible.

## Query APIs

The API deliberately distinguishes what the entity is doing now from context retained below a child.

### Active Top

```c
WarStateBase* wst_getActiveState(WarContext*, WarEntity*);
WarStateBase* wst_getActiveStateOfType(WarContext*, WarEntity*, WarStateType);
bool wst_isActiveState(WarContext*, WarEntity*, WarStateType);
WarStateBase* wst_peekAt(WarContext*, WarEntity*, u8 stackIndex);
```

Use active queries for rendering, collision, command gating, and behavior decisions.

Typed examples:

```c
WarStateMove* wst_getActiveMoveState(context, entity);
bool wst_isActivelyMoving(context, entity);
bool wst_isActivelyAttacking(context, entity);
```

### Stack Membership

```c
WarStateBase* wst_findStateInStack(WarContext*, WarEntity*, WarStateType);
bool wst_containsState(WarContext*, WarEntity*, WarStateType);
```

Use stack membership when retained parent context matters.

Typed examples such as `wst_getMoveState()` and predicates such as `wst_isMoving()` search the stack, including paused states. `wst_getIdleState()` is a current exception: despite its placement with stack helpers, it checks only the active state. Use `wst_findStateInStack(..., WAR_STATE_IDLE)` when paused IDLE membership matters.

Pending helpers such as `wst_isGoingToMove()` inspect candidate-bearing PUSH, REPLACE, and RESET requests. A pending POP has no candidate state type.

## Update Architecture

Production does not call a per-entity `wst_updateStateMachine()` function. `war_map.c` runs this sequence:

1. Input and selection processing.
2. AI command processing.
3. `wst_processPendingTransitions()`.
4. One batch updater for each state type.
5. Spatial-grid, action, animation, projectile, spell, and related updates.

State batches run in enum order:

```text
IDLE, MOVE, PATROL, FOLLOW, ATTACK,
GOLD, MINING, WOOD, CHOPPING, DELIVER,
DEATH, COLLAPSE, TRAIN, UPGRADE, BUILD,
REPAIR, REPAIRING, CAST, WAIT
```

Consequences:

- A transition already pending before the transition pass can enter and update in the same frame.
- A transition submitted by a state update normally commits on the next frame.
- Only the exact active top state updates. Paused states remain allocated but are skipped.
- Ordinary batch updaters scan their type's 512 slots, test occupancy, resolve the owner, check active pointer identity, and apply delay scheduling.
- `delay` is converted into `nextUpdateGameTime` by `wst_isNextUpdateTime()` and then cleared.
- MOVE is a coordinated multi-pass batch and does not use `wst_isNextUpdateTime()`.

Transition processing skips disabled state-machine components. Current type batch loops do not independently test the enabled flag, so disabling a component does not reliably pause an already active state.

## MOVE and Staged Stuck Recovery

MOVE uses inline waypoints, destination flow fields, sampled RVO, final-arrival slowdown, and staged recovery.

The seven MOVE subpasses are:

1. Clear per-frame RVO diagnostics.
2. Submit eligible autonomous ATTACK reactions.
3. Compute preferred velocities from flow fields.
4. Gather neighbors and compute adjusted RVO velocities.
5. Integrate positions and detect waypoint arrival.
6. Track progress and perform staged recovery.
7. Retain adjusted velocity for the next RVO update.

Final-segment preferred speed scales down inside three map tiles, with a minimum scale of 0.25. Position integration clamps an overshooting step to the waypoint.

`WarMoveProgress` stores:

```c
struct _WarMoveProgress
{
    f32 bestDistanceSq;
    f32 noProgressTime;
    f32 lowVelocityTime;
    u8 recoveryAttempt;
};
```

Recovery behavior:

- A waypoint or goal-position change resets progress tracking.
- Improving the best linear distance by at least four pixels resets both timers and the recovery attempt.
- At 0.5 seconds without meaningful progress, MOVE recomputes the destination flow field.
- At 1.5 seconds, MOVE enables stronger avoidance by increasing its RVO radius from `0.45` to `0.60` map tiles.
- At 3.0 seconds, MOVE zeroes its velocities and requests POP with `WAR_STATE_RESULT_BLOCKED`.
- A root MOVE that pops falls back to IDLE.
- A child MOVE pops back to its parent with child-failure resume semantics.
- `lowVelocityTime` is currently diagnostic; recovery thresholds use `noProgressTime`.

Terminal outcomes:

- Fewer than two waypoints: `NO_DESTINATION`.
- Missing flow field: `NO_PATH`.
- Final waypoint reached: `SUCCESS`.
- Three seconds without meaningful progress: `BLOCKED`.

The FSM debug text displays MOVE waypoint progress, retained velocity, stuck time, and recovery stage.

## Transactional States and Cancellation

TRAIN, UPGRADE, and BUILD retain explicit transaction fields for costs, output commitment, cancellation, and one-time refunds.

Use `wst_resetStateForCancellation()` for their cancellation path. It can target a transactional state already on the stack or still pending. Cancellation marks the transaction before applying the replacement RESET. A pending transactional target is exited even though it never entered so its refund logic can run.

Refunds occur only when all conditions hold:

```text
cancelled
transactionApplied
output not committed
not already refunded
```

Do not infer refunds from a generic exit reason. The state-specific transaction flags enforce exactly-once behavior.

Committed BUILD output has a completion-wins exception. If cancellation targets an on-stack BUILD whose output is already committed, transition processing discards the cancellation replacement and rewrites the request to a completion POP so the completed building is preserved. The rewritten POP currently retains result `NONE`, which maps to failed-exit and child-failure resume semantics.

## Entity Removal

Entity removal tears down the FSM before unit, transform, action, and related components because exit callbacks may need those components.

State-machine removal:

1. Exits and releases stacked states top-to-bottom with `WAR_STATE_EXIT_REMOVED`.
2. Clears the stack and depth.
3. Releases pending candidate and cancellation references.
4. Removes the component from dense storage and repairs the swapped owner's component index.

Pending states were never entered, so they are released without exit callbacks. Removal is not automatically a transactional cancellation or refund.

## Canonical Usage

### Player Move Resets the Root Order

```c
if (!wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
{
    return;
}

vec2 position = wu_getUnitCenterPosition(context, entity);
WarStateMove* move = wst_createMoveState(
    context,
    entity,
    2,
    arrayArg(vec2, position, targetPosition),
    false);

if (move)
{
    wst_resetState(
        context,
        entity,
        (WarStateBase*)move,
        WAR_TRANSITION_CAUSE_PLAYER_ORDER);
}
```

### Push a Temporary Child

```c
WarStateFollow* follow = wst_createFollowState(
    context,
    entity,
    targetEntityId,
    targetPosition,
    desiredDistance);

if (follow)
{
    wst_pushState(
        context,
        entity,
        (WarStateBase*)follow,
        WAR_TRANSITION_CAUSE_COMPLETION);
}
```

### Pop with an Explicit Result

```c
wst_popState(
    context,
    entity,
    WAR_TRANSITION_CAUSE_COMPLETION,
    WAR_STATE_RESULT_TARGET_INVALID);
```

### Replace Only the Active State

```c
WarStateMining* mining = wst_createMiningState(context, entity, goldmine->id);
if (mining)
{
    wst_replaceState(
        context,
        entity,
        (WarStateBase*)mining,
        WAR_TRANSITION_CAUSE_COMPLETION);
}
```

Once a valid candidate has been submitted, do not manually free it when submission loses. Transition submission releases rejected or displaced unowned candidates.

## Debugging and Tests

Press `Ctrl+Shift+T` in a map to toggle state-machine debug rendering. With exactly one entity selected, the debug panel shows:

- Entity name and ID.
- Stack depth and entries from top to bottom.
- State-specific details.
- Pending operation and candidate.

World-space overlays visualize active IDLE, MOVE, PATROL, FOLLOW, ATTACK, and resource-flow states.

The FSM test harness uses a fixed 30 Hz timestep. Run the required game compile-check:

```sh
nob.exe build --cc msvc --target win64 --check
```

The test command runs from `build/<target>` and does not copy `DATA.WAR`. Before testing a clean target directory, either run a full game build without `--check` or copy `assets/DATA.WAR` into `build/<target>`.

Then run the tests:

```sh
nob.exe test --cc msvc --target win64
```

The test suite covers transition arbitration, generational references, pool exhaustion, lifecycle calls, transactional cancellation, economic ownership, MOVE completion, arrival slowdown, and staged stuck recovery.

Current harness caveats:

- The test fixture updates actions before FSM states, while production updates actions after FSM states.
- `--filter` is forwarded by `nob` but currently ignored by `tests/test_main.c`.
- Inspect Unity's printed test summary; the test process's aggregate counters are not currently wired to Unity failures.

## Adding a New State Type

Adding a state touches every ordinal-indexed registry. Keep enum order, descriptor order, pool mapping, and size tables synchronized.

1. Add `WAR_STATE_<NAME>` before `WAR_STATE_COUNT` in `src/war_enums.h`.
2. Add the forward declaration and typedef in `src/war_fwd.h`.
3. Define `WarState<Name>` in `src/war_state_machine.h` with `WarStateBase base` first.
4. Add a zero-offset static assertion for the concrete type.
5. Add a 512-entry typed array to `WarStateStorage` in `src/war_entities.h`.
6. Add the descriptor row in `stateDescriptors` in exact enum order.
7. Add the type to `wst_getTypeArray()` and `stateTypeSizes` in `src/war_state_machine.c`.
8. Declare the creator, applicable lifecycle hooks, and update APIs in `src/war_state_machine.h`. Ordinary states use a single-state updater plus a batch updater; coordinated states such as MOVE may use only a custom batch updater.
9. Create `src/war_state_machine_<name>.c`. Do not create a private state header unless there is a concrete need.
10. Make the creator allocate with `wst_allocState()`, check the reference, dereference it, initialize fields, and return the typed pointer.
11. Make the batch updater scan only its occupied pool slots and update only the active state. Use `wst_isNextUpdateTime()` unless coordinated global subpasses are required.
12. Add the production update pass in `src/war_map.c`.
13. Add the state name and useful details to `src/war_state_machine_debug.c`; add world visualization when useful.
14. Include the new `.c` before `war_state_machine.c` in `src/war1.c`, `src/war1_editor.c`, and `tests/test_main.c`.
15. Add the update pass to `tests/war_test_context.c`.
16. Add and register tests in `tests/war_state_machine_test.c`.
17. Run the required game compile-check, the editor compile-check, and the FSM tests.

Never add a new unity-build state source to `nob.c`.

When the state is included by the editor unity entry point, also verify:

```sh
nob.exe editor --cc msvc --target win64 --check
```

## Current Limitations

The current implementation intentionally documents these incomplete areas rather than presenting the planned architecture as finished:

- Pause, validate, and resume callback slots exist, but no state registers them.
- Parents receive only child success or failure, not the exact `WarStateResult`.
- Interrupt masks are advisory unless callers use `wst_canSubmitTransition()`.
- CAST's custom interrupt function is implemented but not registered.
- Full-stack PUSH replaces the active top rather than rejecting the request.
- Status and script causes exist, but there is no complete status subsystem using them yet.
- Inside-building behavior is still inferred from MINING, DELIVER, and REPAIRING stack state rather than a dedicated occupancy component.

## Summary

The War1-C FSM is a pooled, type-batched pushdown behavior system. Its defining invariants are:

- Typed fixed pools, not per-state heap allocation.
- Generational `WarStateRef` ownership, not retained raw pointers.
- A fixed bottom-to-top stack with one active top.
- One deferred request per entity with explicit cause priority.
- PUSH for temporary children, POP with explicit results, REPLACE for the active top, and RESET for new root orders.
- Separate active-state and stack-membership queries.
- Global per-type update passes, with MOVE using coordinated RVO subpasses.

Code that follows these invariants preserves parent behavior, avoids stale-state access, and keeps transition outcomes deterministic across update passes.
