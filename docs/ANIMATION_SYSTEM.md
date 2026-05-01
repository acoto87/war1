# War1-C Animation System Documentation

## Overview

The Animation System is a sprite frame sequence engine for one-off visual effects and overlays. While the **Action System** handles unit animations (walk, attack, idle), the **Animation System** manages temporary visual effects that play independently on top of units or the map.

Common animations include:
- **Building Damage Overlays** - Visual indicators when a building is damaged
- **Explosions** - Detonations from catapult shots, buildings collapsing
- **Spells** - Visual effects for magical attacks (fireball, lightning)
- **Environmental Effects** - Poison clouds, rain of fire, building collapse sequences
- **Custom Frame Sequences** - Any sprite sheet that needs to cycle through frames

Unlike the Action System which loops indefinitely and is tightly integrated with unit state machines, animations are **temporal** — they play once (or loop a fixed number of times) and self-destruct when finished.

---

## Architecture

### Core Concept: Sprite Animation as a Component

Animations are managed as a **component** that can be attached to any entity. An entity can have **multiple simultaneous animations**, allowing layering of effects (e.g., a building that is both damaged AND being repaired).

```
Entity
├── Transform Component
├── Sprite Component (unit sprite)
├── Unit Component (stats, health)
└── Animations Component
    ├── Animation 1 (damage overlay)
    ├── Animation 2 (collapse effect)
    └── Animation 3 (custom effect)
```

Each animation plays independently, with its own frame counter, duration, and playback status.

### Key Types

#### `WarAnimationStatus` - Execution State

```c
typedef enum _WarAnimationStatus
{
    WAR_ANIM_STATUS_NOT_STARTED,  // Created but not yet playing
    WAR_ANIM_STATUS_RUNNING,      // Currently playing
    WAR_ANIM_STATUS_FINISHED,     // Complete (will be removed)
} WarAnimationStatus;
```

#### `WarSpriteAnimation` - Single Animation Instance

```c
struct _WarSpriteAnimation
{
    String name;              // Unique identifier (e.g., "explosion_123.45_67.89")
    bool loop;                // Should this repeat or play once?
    f32 loopDelay;            // Pause (in seconds) between loop iterations

    vec2 offset;              // Position offset from entity origin
    vec2 scale;               // Scaling (1.0 = normal, 2.0 = 2x)

    f32 frameDelay;           // Duration of each frame (in seconds)
    s32List frames;           // Sequence of frame indices to display

    WarSprite sprite;         // Sprite resource (contains frame dimensions)

    f32 animTime;             // Current playback time (0.0 to 1.0 normalized)
    f32 loopTime;             // Countdown for loop delay
    WarAnimationStatus status;// NOT_STARTED, RUNNING, or FINISHED
};
```

#### `WarAnimationsComponent` - Container for All Animations on an Entity

```c
struct _WarAnimationsComponent
{
    bool enabled;                        // Can animations play?
    WarSpriteAnimationList animations;   // List of active animations
};
```

---

## API Reference

### Creation

#### `WarSpriteAnimation* wanim_createAnimation(WarContext* context, String name, WarSprite sprite, f32 frameDelay, bool loop)`

**Purpose:** Create a custom animation from scratch.

**Parameters:**
- `context` - Game context
- `name` - Unique identifier for this animation (e.g., "my_effect_001")
- `sprite` - Sprite resource (dimensions and frame data)
- `frameDelay` - Duration per frame in seconds (e.g., 0.1f = 10 FPS)
- `loop` - Repeat after finishing?

**Returns:** Pointer to newly allocated `WarSpriteAnimation`

**What it does:**
- Allocates memory for the animation
- Initializes status to NOT_STARTED
- Sets offset and scale to default (origin, 1x)
- Prepares an empty frame list

**Example:**
```c
WarSprite explosionSprite = wspr_createSpriteFromResourceIndex(
    context, imageResourceRef(WAR_EXPLOSION_RESOURCE)
);
WarSpriteAnimation* explosion = wanim_createAnimation(
    context, 
    "explosion_hit",
    explosionSprite,
    0.1f,  // 100ms per frame = 10 FPS
    false  // Play once
);
```

---

#### `WarSpriteAnimation* wanim_createAnimationFromResourceIndex(WarContext* context, String name, WarSpriteResourceRef spriteResourceRef, f32 frameDelay, bool loop)`

**Purpose:** Create an animation directly from a resource ID (convenience wrapper).

**Parameters:**
- `context` - Game context
- `name` - Animation identifier
- `spriteResourceRef` - Resource reference ID
- `frameDelay` - Duration per frame
- `loop` - Repeat?

**Returns:** New animation instance

**What it does:**
- Loads the sprite from the resource database
- Calls `wanim_createAnimation` with the loaded sprite

**Example:**
```c
WarSpriteAnimation* damage = wanim_createAnimationFromResourceIndex(
    context,
    "damage_indicator",
    imageResourceRef(WAR_BUILDING_DAMAGE_1_RESOURCE),
    0.2f,
    true  // Damage indicator loops
);
```

---

### Frame Setup

#### `void wanim_addAnimationFrame(WarSpriteAnimation* animation, s32 frameIndex)`

**Purpose:** Add a single frame to an animation's playback sequence.

**Parameters:**
- `animation` - Animation to modify
- `frameIndex` - Index in the sprite sheet to display

**What it does:**
- Appends `frameIndex` to the animation's frame list

**Example:**
```c
// Create a 6-frame explosion
for (s32 i = 0; i < 6; i++)
{
    wanim_addAnimationFrame(explosion, i);
}
```

---

#### `void wanim_addAnimationFrames(WarSpriteAnimation* animation, s32 count, s32 frameIndices[])`

**Purpose:** Add multiple frames in one call.

**Parameters:**
- `animation` - Animation to modify
- `count` - Number of frames
- `frameIndices[]` - Array of frame indices (or NULL for 0..count-1 sequence)

**What it does:**
- If `frameIndices` is provided, adds those exact frames
- If `frameIndices` is NULL, adds frames 0, 1, 2, ..., count-1

**Example:**
```c
// Method 1: Explicit frame list
s32 frames[] = {0, 1, 2, 3, 4, 5};
wanim_addAnimationFrames(explosion, 6, frames);

// Method 2: Sequential (0..5)
wanim_addAnimationFrames(explosion, 6, NULL);  // Same result
```

---

#### `void wanim_addAnimationFramesRange(WarSpriteAnimation* animation, s32 from, s32 to)`

**Purpose:** Add a range of frames (forward or backward).

**Parameters:**
- `animation` - Animation to modify
- `from` - Starting frame index
- `to` - Ending frame index

**What it does:**
- If `from < to`, adds frames `from, from+1, ..., to`
- If `from > to`, adds frames `from, from-1, ..., to` (reverse)

**Example:**
```c
// Forward: frames 0, 1, 2, 3, 4
wanim_addAnimationFramesRange(anim, 0, 4);

// Reverse: frames 10, 9, 8, 7
wanim_addAnimationFramesRange(anim, 10, 7);

// Ping-pong effect: forward then backward
wanim_addAnimationFramesRange(anim, 0, 5);      // 0..5
wanim_addAnimationFramesRange(anim, 4, 1);      // 4,3,2,1
```

---

### Animation Attachment

#### `void wanim_addAnimation(WarEntity* entity, WarSpriteAnimation* animation)`

**Purpose:** Attach a created animation to an entity.

**Parameters:**
- `entity` - Target entity
- `animation` - Animation to attach

**What it does:**
- Adds the animation to the entity's animations component
- Animation will be updated/rendered each frame

**Example:**
```c
WarEntity* target = // ... unit being attacked ...
WarSpriteAnimation* explosion = wanim_createAnimation(...);
wanim_addAnimation(target, explosion);  // Animation plays on top of unit
```

---

### Positioning & Transform

**Animation Position:**
- Animations are positioned using `offset` (relative to entity origin)
- Scaled using the `scale` field
- Can be set directly after creation:

```c
WarSpriteAnimation* effect = wanim_createAnimation(...);
effect->offset = vec2f(50.0f, -30.0f);  // Offset from entity
effect->scale = vec2f(1.5f, 1.5f);      // 1.5x size
wanim_addAnimation(entity, effect);
```

---

### Query & Lookup

#### `WarSpriteAnimation* wanim_findAnimation(WarContext* context, WarEntity* entity, StringView name)`

**Purpose:** Find an animation by name on an entity.

**Parameters:**
- `context` - Game context
- `entity` - Entity to search
- `name` - Animation name

**Returns:** Pointer to animation, or NULL if not found

**Example:**
```c
WarSpriteAnimation* damage = wanim_findAnimation(context, building, "damage_overlay");
if (damage)
{
    logInfo("Damage animation found");
}
```

---

#### `bool wanim_containsAnimation(WarContext* context, WarEntity* entity, StringView name)`

**Purpose:** Check if an entity has a specific animation.

**Parameters:**
- Same as `wanim_findAnimation`

**Returns:** true if animation exists, false otherwise

**Example:**
```c
if (wanim_containsAnimation(context, building, "collapse"))
{
    // Building is collapsing, don't allow repairs
}
```

---

#### `f32 wanim_getAnimationDuration(WarSpriteAnimation* animation)`

**Purpose:** Calculate total playback duration.

**Parameters:**
- `animation` - Animation to query

**Returns:** Duration in seconds = `frameDelay * frameCount`

**Example:**
```c
f32 duration = wanim_getAnimationDuration(explosion);
// Schedule cleanup or next action after explosion finishes
scheduleAfterDelay(duration, nextAction);
```

---

### Removal & Cleanup

#### `void wanim_removeAnimation(WarContext* context, WarEntity* entity, StringView name)`

**Purpose:** Manually remove an animation from an entity.

**Parameters:**
- `context` - Game context
- `entity` - Entity containing animation
- `name` - Name of animation to remove

**What it does:**
- Finds the animation by name
- Removes it from the animations list
- Memory is freed

**Example:**
```c
// Cancel a damage overlay
wanim_removeAnimation(context, building, "damage_overlay");
```

---

#### `void wanim_freeAnimation(WarSpriteAnimation* animation)`

**Purpose:** Free animation memory.

**Parameters:**
- `animation` - Animation to free

**What it does:**
- Deallocates the frame list
- Deallocates the animation struct

**Note:** Called automatically when `FINISHED` status is detected during update. Manual calls are rare.

---

### Update & Playback

#### `void wanim_updateAnimations(WarContext* context)`

**Purpose:** Advance all animations in the game by one frame.

**When to call:** Every game loop frame (called from main game update)

**What it does:**
1. Iterates all entities
2. For each entity's animation list, calls `wanim_updateAnimation`
3. Automatically removes finished non-looping animations
4. Handles loop delays and auto-removal

**Example:**
```c
void wg_updateGame(WarContext* context)
{
    // ... other updates ...
    wanim_updateAnimations(context);  // Advance all animations
    // ... render ...
}
```

**Internal Logic:**
```
For each animation:
  - If FINISHED and not looping → remove and continue
  - Set status to RUNNING
  - If in loop delay → decrement, return (wait)
  - Calculate progress: dt = deltaTime / totalDuration
  - Scale dt by map speed
  - Increment animTime by dt
  - If animTime >= 1.0 → FINISHED
    - If looping → reset and set loopDelay countdown
    - Else → will be removed next update
```

---

## Pre-Made Animation Factories

The animation system includes factory functions for common visual effects. These handle all frame setup and positioning automatically.

### Building Damage Overlay

#### `WarSpriteAnimation* wanim_createDamageAnimation(WarContext* context, WarEntity* entity, String name, int damageLevel)`

**Purpose:** Create a damage indicator overlay on a building.

**Parameters:**
- `context` - Game context
- `entity` - Building entity to damage
- `name` - Identifier for this effect
- `damageLevel` - 1 or 2 (damage level determines sprite)

**What it does:**
- Creates animation with 4 frames (damage indicator phases)
- Loops continuously
- Positions at the building's sprite center
- Frame delay: 0.2s (5 FPS, slow pulsing)
- **Automatically attached to entity**

**Returns:** The animation (so you can remove it later if needed)

**Example:**
```c
// Show damage on building
wanim_createDamageAnimation(context, damaged_building, "damage_lvl1", 1);

// Later, when healed
wanim_removeAnimation(context, damaged_building, "damage_lvl1");
```

---

### Building Collapse

#### `WarSpriteAnimation* wanim_createCollapseAnimation(WarContext* context, WarEntity* entity, String name)`

**Purpose:** Play a building collapse/explosion effect.

**Parameters:**
- `context` - Game context
- `entity` - Building to collapse
- `name` - Identifier

**What it does:**
- Creates a 17-frame collapse sequence
- Non-looping (plays once)
- Frame delay: 0.1s (10 FPS)
- Dynamically scales animation to match building size
- Calculates offset so explosion is centered on building
- **Automatically attached to entity**

**Returns:** The animation

**Example:**
```c
// Building is destroyed
wanim_createCollapseAnimation(context, barracks, "collapse_effect");
// Animation plays, then automatically removed when finished
```

---

### General Explosions

#### `WarSpriteAnimation* wanim_createExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position)`

**Purpose:** Spawn an explosion effect at a specific world position.

**Parameters:**
- `context` - Game context
- `entity` - Entity to attach animation to (often a dummy entity or projectile)
- `position` - World coordinates for explosion center

**What it does:**
- Creates a 6-frame explosion
- Non-looping
- Frame delay: 0.1s
- Names animation based on position (e.g., "explosion_123.45_67.89")
- Positions sprite at world coordinates
- **Automatically attached**

**Returns:** The animation

**Example:**
```c
// Catapult shot lands
vec2 hitPos = vec2f(100.0f, 200.0f);
wanim_createExplosionAnimation(context, world_entity, hitPos);
```

---

#### `WarSpriteAnimation* wanim_createRainOfFireExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position)`

**Purpose:** Spawn a "Rain of Fire" spell explosion effect.

**Parameters:**
- `context` - Game context
- `entity` - Entity to attach to
- `position` - World coordinates

**What it does:**
- Creates a 3-frame (frames 3, 4, 5) "rain of fire" explosion
- Non-looping
- Frame delay: 0.1s
- Positioned at world coordinates
- **Automatically attached**

**Returns:** The animation

**Example:**
```c
// Rain of Fire spell cast
wanim_createRainOfFireExplosionAnimation(context, caster, spellCenter);
```

---

### Spell Effects

#### `WarSpriteAnimation* wanim_createSpellAnimation(WarContext* context, WarEntity* entity, vec2 position)`

**Purpose:** Display a generic spell cast effect.

**Parameters:**
- `context` - Game context
- `entity` - Entity to attach to
- `position` - World coordinates

**What it does:**
- Creates a 6-frame spell effect
- Non-looping
- Frame delay: 0.4s (slower than explosions)
- Positioned at world coordinates
- **Automatically attached**

**Returns:** The animation

**Example:**
```c
// Fireball spell at target location
wanim_createSpellAnimation(context, caster, targetPos);
```

---

#### `WarSpriteAnimation* wanim_createPoisonCloudAnimation(WarContext* context, WarEntity* entity, vec2 position)`

**Purpose:** Create a lingering poison cloud effect.

**Parameters:**
- `context` - Game context
- `entity` - Entity to attach to
- `position` - World coordinates

**What it does:**
- Creates a 4-frame poison cloud
- **Loops continuously** (unlike most effects)
- Frame delay: 0.5s (slow, dreamy animation)
- Positioned at world coordinates
- **Automatically attached**

**Returns:** The animation

**Example:**
```c
// Poison spell creates cloud at location
wanim_createPoisonCloudAnimation(context, spell_source, cloudPos);
// Cloud persists and loops until manually removed
```

---

## Integration Points

### Game Loop

```c
void wg_updateGame(WarContext* context)
{
    // ... update game logic ...
    
    // Update all animations (must be called every frame)
    wanim_updateAnimations(context);
    
    // Render everything (animations rendered based on their status/frames)
    wr_render(context);
}
```

### Rendering Integration

The animation system provides frame and positioning data. Rendering code uses:
- `animation->sprite.frameIndex` - which frame to display
- `animation->offset` - position offset
- `animation->scale` - scaling factor

The render system draws animations **on top of** the entity's normal sprite, allowing layering.

### Example: Projectile Impact

```c
// Projectile hits target
void handleProjectileImpact(WarContext* context, WarProjectile* proj, WarEntity* target)
{
    // Deal damage
    wu_damageUnit(target, proj->damage);
    
    // Create visual feedback
    if (proj->type == WAR_PROJECTILE_FIREBALL)
    {
        wanim_createExplosionAnimation(context, target, proj->position);
    }
    else if (proj->type == WAR_PROJECTILE_ARROW)
    {
        wanim_createSpellAnimation(context, target, proj->position);
    }
    
    // Remove projectile
    we_destroyEntity(context, proj->entity);
}
```

### Example: Building Destruction

```c
// Building health reaches zero
void handleBuildingDeath(WarContext* context, WarEntity* building)
{
    // Create collapse effect
    wanim_createCollapseAnimation(context, building, "collapse");
    
    // Remove after animation completes
    // (automatic: animation is removed when FINISHED)
}
```

---

## Animation Lifecycle

### Timeline

```
1. Created
   wanim_createAnimation() or wanim_createDamageAnimation(), etc.
   Status: NOT_STARTED
   animTime: 0

2. Attached
   wanim_addAnimation(entity, animation)
   Now managed by entity's animations component

3. First Update
   wanim_updateAnimations() called
   Status: RUNNING
   First frame displayed

4. Playing
   Each update increments animTime
   Frame index determined by animTime * frameCount
   animTime goes from 0 to 1

5. Completion
   animTime >= 1.0
   Status: FINISHED
   If loop: restart with loopDelay
   If no loop: marked for removal

6. Cleanup
   Next wanim_updateAnimations() call
   Animation removed from entity
   Memory freed
```

---

## Performance Considerations

### Memory
- Each animation: ~100-200 bytes base + frame list
- Frame list: 4 bytes per frame × count
- Example: 6-frame explosion = ~125 + 24 = ~150 bytes

### CPU
- `wanim_updateAnimations()` is O(entities × animations per entity)
- Update is trivial: counter increment, no allocations
- Profiled with Tracy (see code: `TracyCZoneN`)

### Typical Scene Load
- Explosions: 50-100 active simultaneous → minimal impact
- Damage overlays on buildings: constant, low cost
- Spell effects: burst of animations during battles

---

## Common Patterns

### Pattern 1: Simple Explosion on Impact

```c
vec2 impact_pos = projectile->position;
wanim_createExplosionAnimation(context, dummy_entity, impact_pos);
// That's it! Animation plays automatically
```

### Pattern 2: Damage Feedback Loop

```c
// Building takes damage
wu_takeDamage(building, 20);

// Show damage state
int damageLevel = building->health < 50 ? 2 : 1;
wanim_createDamageAnimation(context, building, "damage_indicator", damageLevel);

// Later, building repaired
wu_repair(building, 30);
wanim_removeAnimation(context, building, "damage_indicator");
```

### Pattern 3: Multi-Frame Custom Effect

```c
WarSpriteAnimation* custom = wanim_createAnimation(
    context,
    "my_effect",
    customSprite,
    0.15f,  // 150ms per frame
    false   // Play once
);

// Add frames manually
wanim_addAnimationFramesRange(custom, 0, 10);      // Frames 0-10
wanim_addAnimationFramesRange(custom, 9, 0);       // Reverse: 9-0 (ping-pong)

// Position and attach
custom->offset = vec2f(50.0f, 30.0f);
custom->scale = vec2f(2.0f, 2.0f);
wanim_addAnimation(building, custom);
```

### Pattern 4: Looping Environmental Effect

```c
// Poison cloud at a location (loops indefinitely)
WarSpriteAnimation* cloud = wanim_createPoisonCloudAnimation(
    context,
    world_entity,
    poison_center
);

// Later, when effect expires or area clears
wanim_removeAnimation(context, world_entity, "poison_cloud_...");
```

### Pattern 5: Check Animation Status

```c
// Only allow repairs while NOT collapsing
if (!wanim_containsAnimation(context, building, "collapse"))
{
    allowRepairs(building);
}
else
{
    blockRepairs(building);  // Building is collapsing
}
```

---

## Comparison: Animation vs Action Systems

| Aspect | Animation System | Action System |
|--------|------------------|---------------|
| **Purpose** | Temporary effects | Unit state/animation |
| **Duration** | Seconds (typically) | Indefinite loops |
| **Integration** | Overlay on entity | Core unit behavior |
| **Multiple Instances** | Yes (multiple on one entity) | One active per unit |
| **Looping** | Optional, with delay | Built-in via FSM |
| **Directional** | No (static orientation) | Yes (8 directions) |
| **Removal** | Automatic when finished | Via FSM state change |
| **Use Case** | Explosions, damage, spells | Walk, attack, idle |

**When to use Animation System:**
- One-off visual effects
- Temporary overlays
- Environmental effects
- Spell/ability visuals
- Building destruction

**When to use Action System:**
- Continuous unit animations
- FSM-driven behaviors
- Animation tied to game state
- Directional playback

---

## Extension Points

### Adding a New Pre-Made Effect

1. Create a factory function in `war_animations.c`:

```c
WarSpriteAnimation* wanim_createLightningAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_LIGHTNING_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);
    
    String name = wstr_make();
    wstr_appendFormat(&name, "lightning_%.2f_%.2f", position.x, position.y);
    
    WarSpriteAnimation* anim = wanim_createAnimation(context, name, sprite, 0.08f, false);
    
    // Positioning and frame setup...
    wanim_addAnimationFramesRange(anim, 0, 7);
    
    wanim_addAnimation(entity, anim);
    return anim;
}
```

2. Add declaration to `war_animations.h`:

```c
WarSpriteAnimation* wanim_createLightningAnimation(WarContext* context, WarEntity* entity, vec2 position);
```

3. Call from game logic:

```c
wanim_createLightningAnimation(context, caster, target_pos);
```

---

## Summary

The Animation System provides:
- ✅ **Flexible sprite sequencing** - Any sprite can be a multi-frame animation
- ✅ **Position & scale control** - Overlay effects anywhere
- ✅ **Looping with delays** - Natural pause between loops
- ✅ **Auto-cleanup** - Finished animations self-remove
- ✅ **Multiple simultaneous effects** - Entity can have many animations
- ✅ **Pre-built factories** - Common effects ready to use (explosions, spells, damage)
- ✅ **Delta-time independent** - Frame-rate agnostic timing
- ✅ **Performance optimized** - O(n) per entity per frame

All animations flow through `wanim_createAnimation()` (or factories), `wanim_addAnimation()` (attach), and `wanim_updateAnimations()` (advance), making the system simple, efficient, and easy to extend for new effects.
