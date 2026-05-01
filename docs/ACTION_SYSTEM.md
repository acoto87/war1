# War1-C Action System Documentation

## Overview

The Action System is the core animation and behavior framework in War1-C. It manages all animated sequences for game units, including walking, attacking, harvesting, repairing, building, and dying animations. The system is data-driven, frame-based, and directional-aware, meaning animations can adapt based on unit direction (8 directions).

Each unit type (Footman, Grunt, Peasant, Peon, Archer, etc.) has pre-defined action definitions that describe exactly what happens frame-by-frame during each animation sequence.

---

## Architecture

### Core Concepts

The action system operates on three levels:

1. **Action Definition (WarUnitActionDef)** - Static, pre-defined animation data loaded once at game start
2. **Action State (WarUnitAction)** - Per-unit runtime state tracking progress through an action
3. **Action Step (WarUnitActionStep)** - Individual atomic commands executed during an action (frame, wait, sound, attack)

### Key Types

#### `WarUnitActionStepType` - Enum of Step Commands

Each step in an action is one of these types:

```c
typedef enum _WarUnitActionStepType
{
    WAR_ACTION_STEP_NONE,            // No-op (default)
    WAR_ACTION_STEP_FRAME,           // Display a sprite frame
    WAR_ACTION_STEP_WAIT,            // Pause for N frames
    WAR_ACTION_STEP_ATTACK,          // Trigger attack damage event
    WAR_ACTION_STEP_SOUND_SWORD,     // Play sword swing sound
    WAR_ACTION_STEP_SOUND_FIST,      // Play punch sound
    WAR_ACTION_STEP_SOUND_FIREBALL,  // Play fireball sound
    WAR_ACTION_STEP_SOUND_CHOPPING,  // Play chopping/harvesting sound
    WAR_ACTION_STEP_SOUND_CATAPULT,  // Play catapult launch sound
    WAR_ACTION_STEP_SOUND_ARROW,     // Play arrow shot sound
    WAR_ACTION_STEP_SOUND_LIGHTNING, // Play lightning sound
} WarUnitActionStepType;
```

#### `WarUnitActionType` - Enum of Action Categories

```c
typedef enum _WarUnitActionType
{
    WAR_ACTION_TYPE_NONE = -1,
    WAR_ACTION_TYPE_IDLE,       // Standing/waiting (loops)
    WAR_ACTION_TYPE_WALK,       // Movement animation (loops)
    WAR_ACTION_TYPE_ATTACK,     // Combat swing/cast (loops)
    WAR_ACTION_TYPE_DEATH,      // Unit dies (loops at end frame)
    WAR_ACTION_TYPE_HARVEST,    // Worker gathering resources (loops)
    WAR_ACTION_TYPE_REPAIR,     // Worker repairing buildings (loops)
    WAR_ACTION_TYPE_BUILD,      // Worker building structure (loops)
    WAR_ACTION_TYPE_COUNT
} WarUnitActionType;
```

#### `WarUnitActionStatus` - Execution State

```c
typedef enum _WarUnitActionStatus
{
    WAR_ACTION_NOT_STARTED,  // Never begun or explicitly reset
    WAR_ACTION_RUNNING,      // Currently playing
    WAR_ACTION_FINISHED,     // Completed (for non-looping actions)
} WarUnitActionStatus;
```

#### `WarUnitActionStep` - Single Step in an Action

```c
struct _WarUnitActionStep
{
    WarUnitActionStepType type;  // What to do
    s32 param;                   // Type-dependent parameter (frame index, duration, etc.)
};
```

#### `WarUnitActionDef` - Full Animation Definition

```c
struct _WarUnitActionDef
{
    WarUnitActionType type;      // IDLE, WALK, ATTACK, etc.
    bool directional;            // Does direction affect frame offset?
    bool loop;                   // Should this repeat when finished?
    WarUnitActionStepList steps; // Array of all steps to execute
};
```

Directional actions (walk, attack, etc.) adapt the sprite frame index based on unit direction. Non-directional actions (build) always show the same animation regardless of facing.

#### `WarUnitAction` - Runtime Action State

```c
struct _WarUnitAction
{
    WarUnitActionStatus status;       // NOT_STARTED, RUNNING, or FINISHED
    f32 scale;                        // Speed multiplier (1.0 = normal, 2.0 = twice as fast)
    f32 waitCount;                    // Frames remaining in current WAIT step
    s32 stepIndex;                    // Position in action's step array
    WarUnitActionStepType lastActionStep;  // Last ATTACK step played this frame
    WarUnitActionStepType lastSoundStep;   // Last SOUND step played this frame
};
```

---

## API Reference

### Initialization

#### `void wact_initUnitActionDefs(void)`

**Purpose:** Bootstrap all unit action definitions at game start.

**When to call:** Once during game initialization (called from `wg_initGame()`).

**What it does:**
- Initializes the global action definition table for all unit types
- Pre-computes frame sequences based on spritesheet layout
- Sets up timing parameters (walk speed, attack speed, etc.)

**Example:**
```c
void wg_initGame(WarContext* context)
{
    // ... other init code ...
    wact_initUnitActionDefs();  // Action system ready
    // ... rest of init ...
}
```

---

### Per-Unit Setup

#### `void wact_addUnitActions(WarEntity* entity)`

**Purpose:** Initialize action state for a newly created unit.

**Parameters:**
- `entity` - The unit entity to set up

**When to call:** When a unit is spawned (called during entity creation).

**What it does:**
- Allocates and zeroes the `actions` array on the unit component
- Sets all actions to NOT_STARTED status
- Sets scale to 1.0 and clears frame counters

**Example:**
```c
// In unit creation code
WarEntity* footman = we_createEntity(context, ...);
wu_initUnit(footman, WAR_UNIT_FOOTMAN);
wact_addUnitActions(footman);  // Ready to animate
```

---

### Action Control

#### `void wact_setAction(WarContext* context, WarEntity* entity, WarUnitActionType type, bool reset, f32 scale)`

**Purpose:** Transition a unit to a new action or update the current action's parameters.

**Parameters:**
- `context` - Game context (for potential future updates)
- `entity` - Unit to animate
- `type` - Target action type (IDLE, WALK, ATTACK, etc.), or WAR_ACTION_TYPE_NONE to stop
- `reset` - If true, restart from step 0; if false, resume or keep current progress
- `scale` - Speed multiplier (1.0 = normal, 2.0 = 2x speed). Use -1.0 to keep current scale.

**What it does:**
- Changes the unit's `actionType` to the specified type
- If `reset` is true, resets `stepIndex` to 0 and status to NOT_STARTED
- If `scale >= 0`, updates the action's `scale` multiplier

**Common usage patterns:**

```c
// Start walking from the beginning (restart animation)
wact_setAction(context, unit, WAR_ACTION_TYPE_WALK, true, 1.0f);

// Start attacking from the beginning
wact_setAction(context, unit, WAR_ACTION_TYPE_ATTACK, true, 1.0f);

// Transition to idle without restarting (resume if paused)
wact_setAction(context, unit, WAR_ACTION_TYPE_IDLE, false, 1.0f);

// Stop animating
wact_setAction(context, unit, WAR_ACTION_TYPE_NONE, false, 1.0f);

// Keep current scale, just resume/restart
wact_setAction(context, unit, WAR_ACTION_TYPE_WALK, true, -1.0f);
```

---

#### `void wact_updateAction(WarContext* context, WarEntity* entity)`

**Purpose:** Advance an action by one frame and apply animation effects.

**Parameters:**
- `context` - Game context (includes `deltaTime`)
- `entity` - Unit to update

**When to call:** Every game loop frame (called from game update).

**What it does:**
1. Checks if the unit has an active action
2. Decrements the wait counter based on delta time
3. When wait expires, advances to the next step
4. Processes all non-WAIT steps (FRAME, ATTACK, SOUND)
5. Applies directional frame offset for directional actions
6. Triggers sound effects and attack damage callbacks
7. Handles looping or finishing

**Frame-by-Frame Breakdown:**

```
Loop iteration 1:
  - Current step: WAIT(6)
  - waitCount -= deltaTime
  - waitCount > 0? Yes, return early
  - No frame change

Loop iteration 2-6:
  - waitCount becomes <= 0
  - Advance to next step

Loop iteration 7:
  - Current step: FRAME(5)
  - Process FRAME: display frame 5 (with directional offset)
  - stepIndex++
  - Process next non-WAIT steps...
  - Eventually reach next WAIT, set waitCount
```

---

### Query

#### `s32 wact_getActionDuration(WarEntity* entity, WarUnitActionType type)`

**Purpose:** Calculate the total duration of an action in frames.

**Parameters:**
- `entity` - Unit (used to look up the correct action definition by unit type)
- `type` - Action type to query (IDLE, WALK, ATTACK, etc.)

**Returns:** Total frame count (sum of all WAIT step durations) for that action.

**Use cases:**
- Predict when an action will finish
- Delay other tasks until an action completes
- Time enemy AI decisions

**Example:**
```c
s32 attackFrames = wact_getActionDuration(unit, WAR_ACTION_TYPE_ATTACK);
// Player will be vulnerable for attackFrames/FRAMES_PER_SECONDS seconds
schedulePlayerAction(attackFrames);
```

---

## Data-Driven Action Definition

### How Actions Are Built

Actions are built at initialization time by combining **action definition functions** that construct `WarUnitActionDef` objects. Each unit type gets custom actions based on its spritesheet layout.

#### Frame Sequence Generation: `wact_getFrameNumbers()`

The spritesheet for each unit contains:
- Walk frames (consecutive)
- Attack frames (consecutive)
- Death frames (consecutive)

These are interleaved in a specific order. The `wact_getFrameNumbers()` function takes the frame counts and spreads them across a visual grid:

```
Spritesheet layout (example: Footman with 5 directions, 5 walk, 5 attack, 3 death):

Direction: N   NE   E   SE   S   SW   W   NW
Frame 1:   W1  W2   W3  W4  W5   A1  A2   A3
Frame 2:   A4  A5   D1  D2  D3   W1  W2   W3
...

This function remaps them so each direction gets the correct subset.
```

#### Action Definition Builders

Several helper functions create standard actions:

##### `createWalkActionDef()` - Oscillating Walk Animation

```c
static WarUnitActionDef createWalkActionDef(
    s32 nframes,              // Number of walk frames (e.g., 5)
    s32 frames[],             // Frame indices
    s32 walkSpeed,            // Delay between each frame (e.g., 6)
    bool directional          // Does direction matter?
)
```

Generates a looping walk by oscillating forward and backward through frames:
```
Sequence: [0, W, 0, W, 0]
Then:     [NW, NW-1, NW-2, NW-1]
Loop
```

**Example setup:**
```c
s32 walkFrames[] = {0, 5, 10, 15, 20};  // 5 walk frame offsets
WarUnitActionDef walkDef = createWalkActionDef(5, walkFrames, 6, true);
// Result: Smooth 8-directional walk with 6-frame timing
```

##### `createAttackActionDef()` - Combat Swing with Sound & Damage

```c
static WarUnitActionDef createAttackActionDef(
    s32 nframes,              // Number of attack frames (e.g., 5)
    s32 frames[],             // Frame indices
    s32 attackSpeed,          // Delay between frames
    s32 attackSound,          // Sound to play (e.g., WAR_ACTION_STEP_SOUND_SWORD)
    s32 coolOffTime,          // Recovery frames after animation
    bool directional
)
```

Generates an attack that:
- Cycles through frames
- At the midpoint frame, triggers `WAR_ACTION_STEP_ATTACK` (damage event)
- At the midpoint frame, plays the attack sound
- Pads shorter attacks to normalize duration (5 frames max)
- Adds cool-off time before next attack

**Example:**
```c
s32 attackFrames[] = {25, 30, 35, 40, 45};
WarUnitActionDef attackDef = createAttackActionDef(
    5, attackFrames, 6, WAR_ACTION_STEP_SOUND_SWORD, 1, true
);
// Result: Sword swing that damages at frame 3, plays "sword" sound, then waits 1 frame before idle
```

##### `createHarvestActionDef()` - Resource Gathering

```c
static WarUnitActionDef createHarvestActionDef(
    s32 nframes,              // Number of harvest frames
    s32 frames[],
    s32 harvestSpeed,         // Delay between frames
    s32 harvestSound,         // Sound effect
    s32 coolOffTime,
    bool directional
)
```

Similar to attack but includes:
- Initial 5-frame wait before starting (picking up stance)
- Harvest sound and resource delivery trigger at midpoint
- Used for chopping trees, mining gold, etc.

##### `createDeathActionDef()` - Unit Destruction

```c
static WarUnitActionDef createDeathActionDef(
    s32 nframes,
    s32 frames[],
    s32 waitTime,             // Delay between frames
    bool directional,
    bool doWait101Step        // Some units use special 100+ frame wait for decay
)
```

Non-looping action that:
- Plays through all frames once
- Optionally adds a long pause before final frame (body decay)
- Loops at the final frame (unit stays dead)

---

## Real-World Examples

### Example 1: Making a Unit Walk

```c
WarEntity* grunt = // ... find or create unit ...

// Start walking
wact_setAction(context, grunt, WAR_ACTION_TYPE_WALK, true, 1.0f);

// Each game frame:
wact_updateAction(context, grunt);
// - Advances through walk animation steps
// - Updates sprite frame based on grunt's direction
// - Loops seamlessly
```

### Example 2: Attack Sequence

```c
WarEntity* footman = // ... unit to attack ...
WarEntity* target = // ... enemy unit ...

// Start attacking
wact_setAction(context, footman, WAR_ACTION_TYPE_ATTACK, true, 1.0f);

// In game loop:
wact_updateAction(context, footman);
// When the midpoint frame is reached:
//   - WAR_ACTION_STEP_ATTACK is processed
//   - Attack callback fires, dealing damage to target
//   - Sword sound effect plays
//   - Animation continues through recovery frames
```

### Example 3: Worker Harvesting

```c
WarEntity* peasant = // ... worker unit ...

// Start harvesting (chopping trees or mining)
wact_setAction(context, peasant, WAR_ACTION_TYPE_HARVEST, true, 1.0f);

// Loop:
wact_updateAction(context, peasant);
// - Waits 5 frames (pickup pose)
// - Cycles through chop frames
// - At midpoint, "chopping" sound plays
// - Cooloff period before next harvest
// - Loops back to start
```

### Example 4: Speed Variation

```c
WarEntity* knight = // ... mounted unit ...

// Normal speed
wact_setAction(context, knight, WAR_ACTION_TYPE_WALK, true, 1.0f);

// Get faster (fleeing or upgraded)
wact_setAction(context, knight, WAR_ACTION_TYPE_WALK, false, 1.5f);
// All wait times are multiplied by 1.5, so animation plays 1.5x faster

// Back to normal
wact_setAction(context, knight, WAR_ACTION_TYPE_WALK, false, 1.0f);
```

### Example 5: Querying Animation Duration

```c
s32 deathFrames = wact_getActionDuration(unit, WAR_ACTION_TYPE_DEATH);
// If deathFrames == 60 and FRAMES_PER_SECONDS == 50
// Then death takes 60/50 = 1.2 seconds

// Schedule cleanup after death animation
f32 deathDuration = (f32)deathFrames / FRAMES_PER_SECONDS;
scheduleEntityRemoval(entity, deathDuration);
```

---

## Directional Animation

Actions can be marked as `directional` or non-directional.

### Directional Actions (walk, attack, idle, etc.)

The sprite frame offset is computed from the unit's current direction:

```c
if (actionDef->directional)
{
    frameIndex += (4 - ABS(4 - (s32)unit->direction));
    
    // Mirror horizontally for certain directions
    transform->scale.x = inRange(unit->direction, WAR_DIRECTION_SOUTH_WEST, WAR_DIRECTION_COUNT) ? -1.0f : 1.0f;
}
```

**Direction mapping (8 compass directions):**
```
0 = NORTH       (up)         -> no offset, no flip
1 = NORTH_EAST  (up-right)   -> offset 1, no flip
2 = EAST        (right)      -> offset 2, no flip
3 = SOUTH_EAST  (down-right) -> offset 3, no flip
4 = SOUTH       (down)       -> offset 4, flip X
5 = SOUTH_WEST  (down-left)  -> offset 3, flip X
6 = WEST        (left)       -> offset 2, flip X
7 = NORTH_WEST  (up-left)    -> offset 1, flip X
```

So a walk animation with 5 frames becomes an 8-directional walk by selecting different frame offsets and flipping.

### Non-Directional Actions (build)

Frame offset is **always 0**, regardless of direction. The animation plays the same way from any angle.

---

## Action Definition Tables

### Global Action Definition Storage

```c
static WarUnitActionDef gUnitActionDefs[WAR_UNIT_COUNT][WAR_ACTION_TYPE_COUNT];
```

This 2D array stores all pre-built action definitions:
- **First dimension:** Unit type (Footman, Grunt, Peasant, etc.)
- **Second dimension:** Action type (IDLE, WALK, ATTACK, etc.)

Each cell is a `WarUnitActionDef` with a complete sequence of steps.

### Per-Unit Type Initialization

Each unit type gets its own init function:

```c
static void initFootmanGruntActionDefs(WarUnitFrameNumbers frames)
{
    // Set animation timing parameters for this unit type
    s32 walkSpeed = 6;        // Fast walk
    s32 attackSpeed = 6;      // Fast attack
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;

    // Build action definitions
    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_SWORD, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    // Register for both Footman and Grunt (they share animations)
    WarUnitType unitTypes[] = {WAR_UNIT_FOOTMAN, WAR_UNIT_GRUNT};
    for (s32 i = 0; i < arrayLength(unitTypes); i++)
    {
        WarUnitType unitType = unitTypes[i];
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_IDLE] = idleDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_WALK] = walkDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_ATTACK] = attackDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_DEATH] = deathDef;
    }
}
```

This pattern is repeated for every unit type: Peasants (with harvest/repair), Archers, Catapults, Cavalry, Mages, Demons, etc.

---

## Integration Points

### Game Loop Integration

```c
void wg_updateGame(WarContext* context)
{
    // ... other updates ...

    // Update all entities
    for (s32 i = 0; i < context->entities.count; i++)
    {
        WarEntity* entity = context->entities.items[i];
        
        // Update action/animation
        wact_updateAction(context, entity);
        
        // Update sprite based on action
        wr_updateEntitySprite(context, entity);
    }

    // ... render ...
}
```

### State Machine Integration

Actions are used by the **State Machine** (FSM) to animate units as they transition between behaviors:

```c
void wst_idle(WarContext* context, WarEntity* entity)
{
    // While idle, play idle animation
    wact_setAction(context, entity, WAR_ACTION_TYPE_IDLE, false, 1.0f);
}

void wst_move(WarContext* context, WarEntity* entity)
{
    // While moving, play walk animation
    wact_setAction(context, entity, WAR_ACTION_TYPE_WALK, true, 1.0f);
}

void wst_attack(WarContext* context, WarEntity* entity)
{
    // While attacking, play attack animation
    wact_setAction(context, entity, WAR_ACTION_TYPE_ATTACK, true, 1.0f);
    
    // When attack animation finishes, FSM transitions back to idle/move
}
```

---

## Performance Notes

### Pre-computation
- All action definitions are built **once** at game startup (`wact_initUnitActionDefs`)
- Runtime `wact_updateAction` only advances counters and indices—very fast
- No per-frame allocation or searching

### Memory Efficiency
- Shared action definitions across unit types (Footman & Grunt share walk/attack)
- Tight `WarUnitAction` struct (3 integers + 2 floats = 20 bytes per unit per action type)
- Step arrays are reasonably sized (typically 20-100 steps per action)

### Delta-Time Scaling
- Wait times are scaled by delta time: `waitCount -= wmap_getMapScaledSpeed(context, deltaTime)`
- Allows smooth frame-rate independence
- Speed multiplier (`scale`) provides further control

---

## Common Pitfalls

### 1. Forgetting to Initialize
```c
// WRONG: entity->unit.actions not initialized
wact_setAction(context, entity, WAR_ACTION_TYPE_WALK, true, 1.0f);

// CORRECT: Always call wact_addUnitActions after creating a unit
WarEntity* unit = we_createEntity(...);
wu_initUnit(unit, unitType);
wact_addUnitActions(unit);  // <-- Don't forget!
```

### 2. Wrong Scale Parameter
```c
// WRONG: Using 0 as "keep current scale"
wact_setAction(context, unit, WAR_ACTION_TYPE_WALK, true, 0.0f);  // Freezes animation!

// CORRECT: Use -1.0f to preserve scale, or provide positive value
wact_setAction(context, unit, WAR_ACTION_TYPE_WALK, true, -1.0f);  // Keep scale
wact_setAction(context, unit, WAR_ACTION_TYPE_WALK, true, 1.0f);   // Reset to normal
```

### 3. Not Calling Update
```c
// WRONG: Set action but never call update
wact_setAction(context, unit, WAR_ACTION_TYPE_WALK, true, 1.0f);
// ... unit never animates!

// CORRECT: Call update in game loop
for (frame in game)
{
    wact_setAction(context, unit, WAR_ACTION_TYPE_WALK, true, 1.0f);
    wact_updateAction(context, unit);  // <-- Necessary!
}
```

### 4. Direction Changes During Action
```c
// CORRECT: Direction changes apply immediately on next update
unit->transform.direction = WAR_DIRECTION_EAST;
wact_updateAction(context, unit);  // Frame offset recalculated with new direction
```

---

## Extension Points

### Adding a New Action Type

1. Add the enum value to `WarUnitActionType` in `war_enums.h`:
```c
typedef enum _WarUnitActionType
{
    // ... existing ...
    WAR_ACTION_TYPE_CAST,   // NEW: Magic casting animation
    WAR_ACTION_TYPE_COUNT
} WarUnitActionType;
```

2. Create a builder function in `war_actions.c`:
```c
static WarUnitActionDef createCastActionDef(...)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_CAST, directional, true);
    // Add steps...
    return def;
}
```

3. Register it in the unit init functions:
```c
gUnitActionDefs[unitType][WAR_ACTION_TYPE_CAST] = castDef;
```

4. Use it in the FSM:
```c
void wst_cast(WarContext* context, WarEntity* entity)
{
    wact_setAction(context, entity, WAR_ACTION_TYPE_CAST, true, 1.0f);
}
```

### Adding a New Sound Step

1. Add to `WarUnitActionStepType` enum:
```c
WAR_ACTION_STEP_SOUND_MAGIC,
```

2. Handle in `wact_updateAction`:
```c
case WAR_ACTION_STEP_SOUND_MAGIC:
{
    action->lastSoundStep = step.type;
    break;
}
```

3. Add sound playback logic in sprite/audio rendering layer (check `action->lastSoundStep`)

---

## Summary

The Action System provides:
- ✅ **Data-driven animations** - Defined at startup, not hard-coded
- ✅ **8-directional support** - Directional actions adapt to unit facing
- ✅ **Synchronized events** - Damage and sounds trigger at exact animation frames
- ✅ **Speed control** - Frame-rate independent with multiplier support
- ✅ **Loop support** - Both one-shot and repeating animations
- ✅ **Per-unit customization** - Each unit type has unique timing and frame sequences

All animations flow through `wact_setAction` (start/change) and `wact_updateAction` (advance), making the system simple, efficient, and easy to extend.
