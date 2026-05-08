# State Machine Subsystem

**Module Prefix:** `wst_`  
**Primary Files:** `war_state_machine.h`, `war_state_machine.c`, `war_state_machine_*.c`  
**Language:** C99/C11  

---

## Overview

The State Machine subsystem is the behavioral backbone of the War1-C game engine. It manages the lifecycle of every entity in the game—units (peons, footmen), buildings (townhalls, barracks, goldmines), walls, and towers—by orchestrating their behavior through a hierarchical, composable state system.

Each entity maintains a **state stack**: a linear chain of states that can be queued and transitioned smoothly. The engine supports 19 distinct state types (Idle, Move, Attack, Mining, Building, etc.), each with pluggable enter/update/leave/free callbacks. The subsystem is built on **Data-Oriented Design (DOD)** principles with **minimal dynamic allocation at runtime**—all state objects are allocated once at state creation and freed at state exit.

### Pipeline Position

```
Game Loop (main update)
    └─> Entity Update
            └─> State Machine Update (per entity)
                    └─> State Callbacks (enter, update, leave)
                            └─> Sub-systems (pathfinding, animations, audio, etc.)
```

The state machine is called once per frame for each enabled entity. It coordinates state transitions, time-based updates, and resource cleanup.

---

## Memory & State Management

### Fixed-Size State Objects

All states inherit from a base `WarState` structure that holds:
- **Type** (`WarStateType`): Identifies which state handlers to invoke.
- **Entity ID** (`s32 entityId`): Backlink to the owning entity.
- **Timing** (`nextUpdateTime`, `delay`): For throttled state updates.
- **State Chain** (`nextState*`): Single-level pointer for queuing the next state.
- **State Data** (anonymous union): Type-specific fields (max 248 bytes for 256-byte total struct).

### Memory Layout & Alignment

```c
struct _WarState
{
    WarStateType type;           // 4 bytes
    s32 entityId;                // 4 bytes
    f32 nextUpdateTime;          // 4 bytes
    f32 delay;                   // 4 bytes
    struct _WarState* nextState; // 8 bytes
    union { ... } [state data];  // up to 248 bytes
};
// Total: 256 bytes (cache-friendly, single allocation per state)
```

**Key Design Decisions:**
- **Single allocation per state**: `wst_createState()` allocates one `WarState` struct. Nested data structures (arrays, lists) are allocated inside the union if needed.
- **State data is union-based**: Each state type uses only the fields it needs. Unused union members are padding, trading memory size for allocation simplicity.
- **No object pools**: States are allocated on-demand and freed at transition. The system expects moderate churn (unit state changes every few frames).
- **Arena allocation optional**: The codebase can use `wm_alloc()` / `wm_free()` which may wrap arena allocators if configured.

### State Lifecycle

1. **Creation**: `wst_createXxxState()` allocates and initializes state.
2. **Queuing**: `wst_changeNextState()` queues state to transition next frame.
3. **Entry**: `wst_enterState()` is called when the state becomes current (setup, animation start, etc.).
4. **Update**: `wst_updateXxxState()` is called each frame (logic, decision-making, transitions).
5. **Exit**: `wst_leaveState()` is called when transitioning away (cleanup, cleanup, disable collision, etc.).
6. **Free**: `wst_freeState()` deallocates the state and any nested lists/arrays.

---

## Core API / Functions

### State Creation

#### `WarState* wst_createState(WarContext* context, WarEntity* entity, WarStateType type)`

Generic state factory. Rarely called directly; use type-specific creators instead.

**Inputs:**
- `context`: Game context (time, map, entities).
- `entity`: Entity that will own this state.
- `type`: `WarStateType` enum (e.g., `WAR_STATE_IDLE`).

**Returns:** Heap-allocated `WarState*`.

**Side Effects:** Allocates memory; entity is not yet updated.

**Performance:** O(1), single malloc.

---

#### Type-Specific Creators

**Motion:**
- `WarState* wst_createMoveState(WarContext* ctx, WarEntity* entity, s32 posCount, vec2 positions[])`
  - Allocates `vec2List` inside the move state.
  - Positions: waypoints to traverse in order.
  
- `WarState* wst_createPatrolState(WarContext* ctx, WarEntity* entity, s32 posCount, vec2 positions[])`
  - Allocates `vec2List` for cyclic patrol points.

- `WarState* wst_createFollowState(WarContext* ctx, WarEntity* entity, WarEntityId targetId, vec2 targetTile, s32 distance)`
  - Follows an entity or a fixed tile position.
  - `distance`: Range at which follower stops.

**Combat:**
- `WarState* wst_createAttackState(WarContext* ctx, WarEntity* entity, WarEntityId targetId, vec2 targetTile)`
  - Target can be entity or tile; entity takes precedence if both exist.

**Resource Gathering:**
- `WarState* wst_createGatherGoldState(WarContext* ctx, WarEntity* entity, WarEntityId goldmineId)`
- `WarState* wst_createMiningState(WarContext* ctx, WarEntity* entity, WarEntityId goldmineId)`
  - Mining includes active harvesting inside the building.
- `WarState* wst_createGatherWoodState(WarContext* ctx, WarEntity* entity, WarEntityId forestId, vec2 position)`
- `WarState* wst_createChoppingState(WarContext* ctx, WarEntity* entity, WarEntityId forestId, vec2 position)`
- `WarState* wst_createDeliverState(WarContext* ctx, WarEntity* entity, WarEntityId townHallId)`

**Building & Training:**
- `WarState* wst_createTrainState(WarContext* ctx, WarEntity* entity, WarUnitType unit, f32 buildTime)`
  - Building creates units in a building. Allocates inside the state.
- `WarState* wst_createUpgradeState(WarContext* ctx, WarEntity* entity, WarUpgradeType upgrade, f32 buildTime)`
- `WarState* wst_createBuildState(WarContext* ctx, WarEntity* entity, f32 buildTime)`
  - Worker building a new structure.
- `WarState* wst_createRepairState(WarContext* ctx, WarEntity* entity, WarEntityId buildingId)`
- `WarState* wst_createRepairingState(WarContext* ctx, WarEntity* entity, WarEntityId buildingId)`

**Special:**
- `WarState* wst_createDeathState(WarContext* ctx, WarEntity* entity)`
- `WarState* wst_createCollapseState(WarContext* ctx, WarEntity* entity)`
  - Building destruction sequence.
- `WarState* wst_createWaitState(WarContext* ctx, WarEntity* entity, f32 waitTime)`
  - Pause for specified duration.
- `WarState* wst_createCastState(WarContext* ctx, WarEntity* entity, WarSpellType spell, WarEntityId targetId, vec2 targetTile)`

---

### State Transition & Query

#### `void wst_changeNextState(WarContext* ctx, WarEntity* entity, WarState* state, bool callLeave, bool callEnter)`

Queue a state transition. Does not immediately change state—transitions happen at the start of `wst_updateStateMachine()`.

**Inputs:**
- `state`: The state to queue.
- `callLeave`: Call `leave` callback on current state before transition.
- `callEnter`: Call `enter` callback on the new state after transition.

**Side Effects:** Modifies entity's state machine component; may queue multiple states in a chain.

**Performance:** O(1).

---

#### `bool wst_changeStateNextState(WarContext* ctx, WarEntity* entity, WarState* state)`

Promote `state->nextState` (if it exists) to the next state to transition.

**Returns:** `true` if a chained state was promoted; `false` if `state->nextState` is NULL.

**Usage Pattern:** Often called from within a state's update logic to implement chained behaviors (e.g., "after move, attack").

---

#### `WarState* wst_getState(WarEntity* entity, WarStateType type)`

Search the state chain (current + queued) for the first matching state.

**Returns:** Pointer to state or NULL if not found.

**Performance:** O(n) where n = state chain depth (typically 1–3).

---

#### `WarState* wst_getDirectState(WarEntity* entity, WarStateType type)`

Get the **current** state if its type matches.

**Returns:** Current state if type matches, else NULL.

**Performance:** O(1).

---

#### `WarState* wst_getNextState(WarEntity* entity, WarStateType type)`

Get the **queued next** state if its type matches.

**Returns:** Next state if type matches, else NULL.

**Performance:** O(1).

---

#### Query Macros

```c
// Convenience macros for common queries:
#define getIdleState(e)        wst_getDirectState(e, WAR_STATE_IDLE)
#define getMoveState(e)        wst_getState(e, WAR_STATE_MOVE)
#define getAttackState(e)      wst_getState(e, WAR_STATE_ATTACK)
#define isIdle(e)              wst_hasDirectState(e, WAR_STATE_IDLE)
#define isMoving(e)            wst_hasState(e, WAR_STATE_MOVE)
#define isAttacking(e)         wst_hasState(e, WAR_STATE_ATTACK)
#define isGoingToAttack(e)     wst_hasNextState(e, WAR_STATE_ATTACK)
#define setDelay(s, seconds)   ((s)->delay = (seconds))
```

---

### State Machine Update

#### `void wst_updateStateMachine(WarContext* ctx, WarEntity* entity)`

**Primary update entry point.** Called once per frame per entity.

**Workflow:**
1. If state machine is disabled, skip.
2. Process all queued state transitions (while `stateMachine->nextState` exists):
   - Call `leave` callback on current state (if `wst_leaveState` flag is true).
   - Swap current ← next.
   - Call `enter` callback on new current state (if `wst_enterState` flag is true).
3. If current state has a `delay` > 0, schedule its next update (`nextUpdateTime = now + delay`).
4. If `now >= nextUpdateTime`, call the `update` callback.

**Side Effects:** Mutates entity state; calls callbacks; may trigger cascading updates.

**Performance:** O(d + 1) where d = transition depth. Typically O(1) per frame.

**Key Insight:** The `delay` field throttles state updates without skipping them. Useful for animation frames, AI decision intervals, etc.

---

#### `void wst_enterState(WarContext* ctx, WarEntity* entity, WarState* state)`

Dispatch to the appropriate enter handler based on state type.

**Inputs:**
- `state`: State being entered.

**Side Effects:** Invokes `stateDescriptors[state->type].enterStateFunc()`.

**Performance:** O(1) (function pointer lookup).

---

#### `void wst_leaveState(WarContext* ctx, WarEntity* entity, WarState* state)`

Dispatch to the appropriate leave handler, then free the state.

**Inputs:**
- `state`: State being exited (can be NULL; no-op if so).

**Side Effects:**
- Calls leave handler.
- Calls `wst_freeState()` to clean up.

**Performance:** O(1) + cleanup cost.

---

#### `void wst_freeState(WarContext* ctx, WarState* state)`

Recursively free state and its chained next states.

**Side Effects:**
- Calls `freeStateFunc` for the state type.
- Recursively frees `state->nextState`.
- Deallocates the state struct itself.

**Performance:** O(n) where n = state chain depth.

---

### Query Helpers

#### `bool wst_isInsideBuilding(WarEntity* entity)`

Check if entity is currently inside a building (mining, delivering, repairing inside).

**Returns:** `true` if entity is in mining, delivering, or repairing state and marked `insideBuilding`.

**Usage:** Attacking units use this to pause attacks while targets are inside.

---

## Data Structures

### `WarState` (256 bytes)

Core state object containing shared fields and a type-specific union.

```c
struct _WarState
{
    WarStateType type;                  // 4 bytes: state identifier
    s32 entityId;                       // 4 bytes: owning entity ID
    f32 nextUpdateTime;                 // 4 bytes: earliest time for next update
    f32 delay;                          // 4 bytes: delay before next update (seconds)
    struct _WarState* nextState;        // 8 bytes: chained next state (for queued transitions)

    union                               // 224 bytes: state-specific data
    {
        struct { bool lookAround; } idle;
        struct { s32 posIdx; vec2List positions; s32 pathIdx; WarMapPath path; s32 waitCnt; bool checkAttacks; } move;
        struct { s32 posIdx; vec2List positions; s32 dir; } patrol;
        struct { s32 targetId; vec2 targetTile; s32 distance; } follow;
        struct { f32 waitTime; } wait;
        struct { s32 targetId; vec2 targetTile; } attack;
        struct { s32 goldmineId; } gold;
        struct { s32 goldmineId; f32 miningTime; } mine;
        struct { s32 forestId; vec2 position; } wood;
        struct { s32 forestId; vec2 position; } chop;
        struct { s32 townHallId; bool insideBuilding; } deliver;
        struct { WarUnitType unit; f32 buildTime; f32 totalBuildTime; bool cancelled; } train;
        struct { WarUpgradeType upgrade; f32 buildTime; f32 totalBuildTime; bool cancelled; } upgrade;
        struct { WarEntityId workerId; f32 buildTime; f32 totalBuildTime; bool cancelled; } build;
        struct { WarEntityId buildingId; } repair;
        struct { WarEntityId buildingId; bool insideBuilding; } repairing;
        struct { WarSpellType spell; WarEntityId targetId; vec2 targetTile; } cast;
    };
};
```

**Alignment:** 8-byte aligned (pointers require it).

**Cache Locality:** Fixed 256-byte size ensures predictable cache line usage.

---

### `WarStateDescriptor`

Callback table for a state type.

```c
typedef struct
{
    WarStateType type;
    void (*enterStateFunc)(WarContext* context, WarEntity* entity, WarState* state);
    void (*leaveStateFunc)(WarContext* context, WarEntity* entity, WarState* state);
    void (*updateStateFunc)(WarContext* context, WarEntity* entity, WarState* state);
    void (*freeStateFunc)(WarContext* context, WarState* state);
} WarStateDescriptor;
```

**Instantiation:** Global array `stateDescriptors[WAR_STATE_COUNT]` populated in `war_state_machine.c`.

---

### `WarStateMachineComponent`

Entity component holding current and queued states.

```c
// Pseudo-code; defined in war_entities.h
struct WarStateMachineComponent
{
    bool enabled;
    WarState* currentState;
    WarState* nextState;
    bool wst_leaveState;
    bool wst_enterState;
};
```

**Fields:**
- `enabled`: Gate state machine updates.
- `currentState`: Active state.
- `nextState`: State queued for transition.
- `wst_leaveState`, `wst_enterState`: Flags controlling callback invocation.

---

## State Types (19 Total)

| State | Enum | Purpose | Union Field |
|-------|------|---------|-------------|
| Idle | `WAR_STATE_IDLE` | Unit standing still, looking around | `idle` |
| Move | `WAR_STATE_MOVE` | Unit traversing waypoints | `move` |
| Patrol | `WAR_STATE_PATROL` | Unit cycling patrol points | `patrol` |
| Follow | `WAR_STATE_FOLLOW` | Unit chasing target entity/tile | `follow` |
| Attack | `WAR_STATE_ATTACK` | Unit in combat | `attack` |
| Gather Gold | `WAR_STATE_GOLD` | Worker moving to goldmine | `gold` |
| Mining | `WAR_STATE_MINING` | Worker extracting gold inside | `mine` |
| Gather Wood | `WAR_STATE_WOOD` | Worker moving to forest | `wood` |
| Chopping | `WAR_STATE_CHOP` | Worker harvesting wood | `chop` |
| Deliver | `WAR_STATE_DELIVER` | Worker returning to townhall | `deliver` |
| Death | `WAR_STATE_DEATH` | Unit dying sequence | (none) |
| Collapse | `WAR_STATE_COLLAPSE` | Building destruction sequence | (none) |
| Train | `WAR_STATE_TRAIN` | Building training a unit | `train` |
| Upgrade | `WAR_STATE_UPGRADE` | Building researching upgrade | `upgrade` |
| Build | `WAR_STATE_BUILD` | Worker constructing building | `build` |
| Repair | `WAR_STATE_REPAIR` | Worker moving to building to repair | `repair` |
| Repairing | `WAR_STATE_REPAIRING` | Worker repairing building | `repairing` |
| Cast | `WAR_STATE_CAST` | Unit casting spell | `cast` |
| Wait | `WAR_STATE_WAIT` | Unit paused for duration | `wait` |

---

## Usage Examples

### Example 1: Command Unit to Move

```c
// Player clicks map tile; commander creates a move state.
WarState* moveState = wst_createMoveState(
    context,
    unit,
    2,
    (vec2[]){ unitCurrentPos, targetTile }
);
wst_changeNextState(context, unit, moveState, true, true);  // Leave idle, enter move
```

**Flow:**
1. State is queued.
2. Next `wst_updateStateMachine()` call:
   - Leaves idle state (animation stop, collision free).
   - Enters move state (pathfinding, collision set, walk animation).
3. Each frame, move state's update is called to advance along path.
4. When destination reached, automatically transitions to idle.

---

### Example 2: Attack with Fallback

```c
// Attacker targets enemy.
WarState* attackState = wst_createAttackState(context, attacker, enemy->id, enemyPos);
wst_changeNextState(context, attacker, attackState, true, true);

// In attack state's update:
if (!wu_unitInRange(attacker, enemy, range))
{
    // Out of range: queue follow as next state, with attack as fallback.
    WarState* followState = wst_createFollowState(context, attacker, enemy->id, enemyPos, range);
    followState->nextState = attackState;  // Restore attack after follow
    wst_changeNextState(context, attacker, followState, false, true);  // Enter follow
}
```

**Flow:** Attack → Follow (because out of range) → Attack (once in range).

---

### Example 3: Gather Gold Workflow

```c
// Worker ordered to gather gold from goldmine.
WarState* gatherState = wst_createGatherGoldState(context, worker, goldmine->id);
wst_changeNextState(context, worker, gatherState, true, true);

// In gather gold state's update:
// 1. Check if goldmine is in range.
// 2. If not, queue follow state with gather as next (chained).
// 3. If yes, transition to mining state.
// In mining state's update:
// 4. Extract gold; when full or goldmine empty, transition to deliver.
// In deliver state:
// 5. Move to townhall; unload; go back to gather gold (loop).
```

---

### Example 4: State Machine in Game Loop

```c
// Main game loop (simplified).
void gameUpdate(WarContext* context)
{
    // ... input, physics, etc.

    // Update all entities' state machines.
    for (s32 i = 0; i < context->entities.count; i++)
    {
        WarEntity* entity = &context->entities.items[i];
        if (entity->alive)
        {
            wst_updateStateMachine(context, entity);
        }
    }

    // ... rendering, audio, etc.
}
```

---

## Dependencies

### Internal Modules

- **`war_entities.h`** – Entity and component definitions.
- **`war_units.h`** – Unit queries (position, stats, animation).
- **`war_map.h`** – Pathfinding, tile collision, map coordinate conversion.
- **`war_pathfinder.h`** – A* pathfinding, obstacle queries.
- **`war_actions.h`** – Animation and action system integration.
- **`war_math.h`** – Vector math, utility macros.
- **`war_alloc.h`** – Memory allocation (`wm_alloc`, `wm_free`).
- **`war_log.h`** – Logging utilities.

### External Modules

- **`shl/list.h`** – Generic list container for state chaining.
- **`shl/array.h`** – Generic array for position lists in move/patrol states.

---

## State Implementation Pattern

Each state type (e.g., `war_state_machine_idle.c`) follows a template:

```c
// 1. Creator
WarState* wst_createXxxState(WarContext* context, WarEntity* entity, ...)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_XXX);
    state->xxx.field1 = value1;
    state->xxx.field2 = value2;
    return state;
}

// 2. Enter (initialization)
void wst_enterXxxState(WarContext* context, WarEntity* entity, WarState* state)
{
    // Set up collision, animation, initial data.
}

// 3. Leave (cleanup)
void wst_leaveXxxState(WarContext* context, WarEntity* entity, WarState* state)
{
    // Restore collision, stop animation, clean up temporary data.
}

// 4. Update (per-frame logic)
void wst_updateXxxState(WarContext* context, WarEntity* entity, WarState* state)
{
    // Decision logic, state transitions, effect application.
    // Use setDelay(state, duration) to throttle next update.
}

// 5. Free (resource deallocation)
void wst_freeXxxState(WarContext* context, WarState* state)
{
    // Free any nested allocations (e.g., vec2List in move state).
}
```

---

## Performance Considerations

### Tight Inner Loop Avoidance

- **Don't call state update inside nested loops.** State machine is called once per entity per frame. Calling it multiple times per frame creates redundant work.
- State delays (`setDelay()`) are preferred over manual frame counting; the engine handles throttling.

### Update Frequency Optimization

```c
// Good: throttle expensive updates with delay.
void wst_updateIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    // Do expensive search (every frame if no delay set).
    WarEntity* enemy = we_getNearEnemy(context, entity);
    
    // Throttle to 20 frames (at 60 FPS ≈ 333ms).
    state->delay = 0.33f;
}
```

### State Chain Depth

- State chains are kept shallow (typically 1–3 levels: idle → move → attack).
- Deep chains (>10) degrade performance slightly due to linked-list traversal in `wst_getState()`.

### Memory Efficiency

- `WarState` is 256 bytes; allocated per state change, freed at transition.
- No object pooling; relies on allocator efficiency (may use arenas).
- Union-based state data avoids over-allocation for simple states.

### Callback Overhead

- All state callbacks are O(1) lookups (function pointers from `stateDescriptors` array).
- No virtual function dispatch or RTTI; stateless branching.

---

## Debugging & Inspection

### Logging State Changes

States can log transitions by inspecting callbacks:

```c
void logStateChange(WarEntity* entity, WarState* oldState, WarState* newState)
{
    const char* oldName = getStateTypeName(oldState ? oldState->type : WAR_STATE_IDLE);
    const char* newName = getStateTypeName(newState->type);
    logInfo("Entity %d: %s -> %s", entity->id, oldName, newName);
}
```

### State Inspection in Debugger

```c
// Example in GDB:
(gdb) p entity->stateMachine.currentState->type
$1 = WAR_STATE_MOVE

(gdb) p entity->stateMachine.currentState->move.positionIndex
$2 = 2

(gdb) p entity->stateMachine.nextState
$3 = (WarState *) 0x0  // NULL: no transition queued
```

### Profiling State Updates

States can be instrumented with Tracy for profiling:

```c
void wst_updateMoveState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZone(ctx, 1);
    // ... move logic ...
    TracyCZoneEnd(ctx);
}
```

---

## Key Design Insights

1. **State Queuing, Not Immediate Transition:** Calling `wst_changeNextState()` doesn't immediately change state. This prevents reentrancy issues and ensures callbacks are called in the correct order.

2. **Delay Throttling:** The `delay` field enables light animation-frame-based scheduling without explicit counters. Useful for pacing AI decisions or animation updates.

3. **Chained States & Fallback:** States can queue a "next state" before transitioning. Example: Attack → Follow (if out of range) → Attack again. This chains behavior naturally.

4. **Minimal Leave/Enter Flags:** The `wst_leaveState` and `wst_enterState` booleans give callers fine-grained control. Some transitions (e.g., move → attack) skip leaving the previous state to preserve animation momentum.

5. **Union-Based Data:** All state data fit in a 256-byte union. This is a design constraint but ensures predictable memory layout and cache efficiency.

6. **Global Descriptor Array:** All state types are registered in a global `stateDescriptors[]` array. Adding a new state requires adding one row to the array and implementing the five callback functions.

---

## Extending the State Machine

### Adding a New State Type

1. **Define the enum** in `war_log.h`:
   ```c
   enum WarStateType {
       // ... existing states ...
       WAR_STATE_CUSTOM = X,
       WAR_STATE_COUNT
   };
   ```

2. **Add union field** in `war_state_machine.h`:
   ```c
   struct _WarState {
       // ...
       union {
           // ... existing ...
           struct {
               s32 customField1;
               f32 customField2;
           } custom;
       };
   };
   ```

3. **Create file** `war_state_machine_custom.c`:
   ```c
   WarState* wst_createCustomState(...) { ... }
   void wst_enterCustomState(...) { ... }
   void wst_leaveCustomState(...) { ... }
   void wst_updateCustomState(...) { ... }
   void wst_freeCustomState(...) { ... }
   ```

4. **Register in descriptor** in `war_state_machine.c`:
   ```c
   WarStateDescriptor stateDescriptors[WAR_STATE_COUNT] = {
       // ...
       { WAR_STATE_CUSTOM, wst_enterCustomState, wst_leaveCustomState, wst_updateCustomState, wst_freeCustomState },
   };
   ```

5. **Include in `war_state_machine.h`** forward declarations for the new callbacks.

6. **Include** `war_state_machine_custom.c` in `war_state_machine.h` (unity build).

---

## Summary

The State Machine subsystem is a **lightweight, data-oriented hierarchical state management system** optimized for game entity behavior. It uses **fixed-size allocations**, **no object pooling**, and **pluggable callbacks** to manage 19 state types across units, buildings, and environmental objects. The design prioritizes **simplicity, predictability, and cache locality** while supporting complex behavioral chains and smooth transitions.

Key strengths:
- **Zero dynamic allocation per frame** (states allocated at transition, freed at exit).
- **O(1) state dispatch** via function pointer arrays.
- **Simple callback model** (enter, update, leave, free).
- **Composable state chains** for fallback behaviors.
- **Throttled updates** via delay field, no manual frame counting.

Minimal overhead and tight integration with pathfinding, animation, collision, and audio systems make it suitable for real-time RTS gameplay with dozens of concurrent entities.
