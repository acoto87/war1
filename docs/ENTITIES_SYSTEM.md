# Entities Subsystem Documentation

## Overview

The **Entities subsystem** implements a component-based entity management system at the heart of War1-C's game object lifecycle. It provides fixed-size entity storage (up to 100 entities), efficient O(1) lookups via hash maps, and embedded components for cache-friendly memory access. Every game object—units (grunts, footmen), buildings (barracks, farms), projectiles (arrows), UI elements, and decorations—is an entity managed by this subsystem.

The subsystem sits centrally in the engine pipeline:
- **Initialization:** Called during scene entry to set up the entity manager
- **Per-frame updates:** Other subsystems (state machines, animations, audio) query entities
- **Per-frame cleanup:** Entities marked for removal are processed with full lifecycle cleanup
- **Integration:** The pathfinder, AI system, UI system, and rendering system all query this subsystem for entity lookups

### Design Philosophy
- **Data-Oriented Design (DOD):** Components are embedded as value types (not pointers) directly in the `WarEntity` struct for cache locality
- **Fixed-size pools:** No dynamic allocation per-frame; all entities pre-allocated at startup
- **Zero-copy entity passing:** Entity IDs (16-bit) passed between subsystems, not entity pointers
- **Global state:** Single `WarEntityManager` instance manages all entities

---

## Memory & State Management

### Fixed-Size Pool Allocation

The entity system uses a **pre-allocated fixed-size array** of `WarEntity` structures:

```c
#define MAX_ENTITIES_COUNT 100

typedef struct {
    WarEntity entities[MAX_ENTITIES_COUNT];
    // ... (maps and metadata)
} WarEntityManager;
```

- All `MAX_ENTITIES_COUNT` entities are allocated at startup via `we_initEntityManager()`
- Each entity is initialized with default values (all components disabled)
- Entity allocations are **never freed** during runtime; instead, entities are marked as inactive
- This approach guarantees O(1) access by index and zero allocation overhead per-frame

### Entity ID Management

Entities are assigned globally unique 16-bit IDs (`WarEntityId`) via an auto-incrementing static counter:

```c
static u16 staticEntityId = 1;  // Never reused within a session
```

- Each `we_createEntity()` call increments the counter
- IDs are **never reused**, even after entity removal (ensures handles remain valid)
- ID collisions are avoided through wraparound at `U16_MAX` (unlikely in practice)

### Component Storage: Embedded Value Types

All components are **embedded directly in the `WarEntity` struct** as value types, not references:

```c
struct WarEntity {
    WarEntityId id;
    WarEntityType type;
    bool active;
    // Components embedded as value types:
    WarTransformComponent transform;
    WarSpriteComponent sprite;
    WarUnitComponent unit;  // Large: ~150 bytes
    WarStateMachineComponent stateMachine;
    WarButtonComponent button;
    WarTextComponent text;
    WarAnimationsComponent animations;
    // ... (14+ more component types)
};
```

**Memory Benefits:**
- Single cache line fetch retrieves entity + all components
- No pointer indirection (eliminates dereference penalties)
- Predictable memory layout for SIMD operations
- Total entity struct size: ~2 KB per entity (worst-case)

### Lookup Infrastructure: Hash Maps

The entity manager maintains **three hash maps** for efficient lookups:

```c
typedef struct {
    WarEntity entities[MAX_ENTITIES_COUNT];
    WarEntityMap entitiesByType;      // WarEntityType → List<WarEntityId>
    WarUnitMap unitsByType;           // WarRaceType → List<WarEntityId>
    WarEntityIdMap entitiesById;      // WarEntityId → WarEntity*
} WarEntityManager;
```

- **`entitiesByType`:** Group entities by type (e.g., all grunts, all barracks). Enables efficient batch operations.
- **`unitsByType`:** Separate grouping by race (Orc/Human). Used for team/faction queries.
- **`entitiesById`:** Direct ID → entity mapping. O(1) lookup by ID (average case).

### UI Entity Tracking

UI entities (buttons, text labels, health bars) are tracked separately in a **linked list**:

```c
extern List uiEntities;  // List of WarEntityId
```

- Enables efficient batch rendering without iterating all entities
- UI lookups by name use linear search: `O(n)` where `n` ≈ 50 UI entities
- Potential optimization: introduce name hash map for UI entities

### Entity Removal & Cleanup

When an entity is removed via `we_removeEntity()` or `we_removeEntityById()`, a full cleanup is performed:

```c
void we_removeEntity(WarEntityId entityId) {
    WarEntity* entity = we_findEntity(entityId);
    if (!entity || !entity->active) return;

    // 1. Remove from manager maps
    // 2. Clean up entity lists (animations, spell timers)
    // 3. Free sprite resources
    // 4. Teardown state machine FSM
    // 5. Free MIDI data for audio components
    // 6. Mark entity inactive
}
```

**Thread Safety:** Entity removal is guarded by mutex when called from concurrent threads.

---

## Core API / Functions

### Entity Manager Initialization

#### `void we_initEntityManager()`
- **Purpose:** Initialize the global entity manager with pre-allocated pool
- **Side effects:** Populates `WarEntityManager` with `MAX_ENTITIES_COUNT` inactive entities; initializes all hash maps
- **Performance:** O(n) startup cost (n = MAX_ENTITIES_COUNT); **call once at scene entry**
- **Notes:** Must be called before any entity creation

---

### Entity Creation

#### `WarEntityId we_createEntity(WarEntityType type)`
- **Purpose:** Allocate and initialize a new generic entity
- **Inputs:** Entity type (e.g., `ENTITY_TYPE_UNIT`, `ENTITY_TYPE_BUILDING`, `ENTITY_TYPE_PROJECTILE`)
- **Outputs:** Globally unique `WarEntityId` (16-bit handle)
- **Side effects:** 
  - Increments static entity ID counter
  - Sets entity as active
  - Initializes transform component to origin (0, 0)
  - Does **not** initialize type-specific components (caller must attach via `we_add*Component()`)
- **Performance:** O(1) allocation (no search required)

#### `WarEntityId we_createUnit(WarRaceType race, const char* unitType, int x, int y)`
- **Purpose:** Create a fully initialized unit (grunt, footman, archer, etc.)
- **Inputs:** 
  - `race`: RACE_ORC or RACE_HUMAN
  - `unitType`: String name (e.g., "grunt", "footman")
  - `x`, `y`: Initial tile coordinates
- **Outputs:** Entity ID of created unit
- **Side effects:** 
  - Creates entity with type `ENTITY_TYPE_UNIT`
  - Attaches and initializes WarUnitComponent with full stats (HP, damage, armor, etc.)
  - Attaches WarStateMachineComponent with initial state `WAR_STATE_IDLE`
  - Adds unit to `unitsByType` map
  - Initializes sprite and animation components
- **Performance:** O(1); initialization is linear in component count (~20 components)

#### `WarEntityId we_createDude(WarRaceType race, const char* dudeName, const char* spriteKey, int x, int y)`
- **Purpose:** Create a simple decorative or NPC dude (non-interactive unit)
- **Inputs:** 
  - `race`: RACE_ORC or RACE_HUMAN
  - `dudeName`: Name identifier
  - `spriteKey`: Resource key for sprite
  - `x`, `y`: Position
- **Outputs:** Entity ID
- **Side effects:** Creates entity with minimal components (transform, sprite, animation)
- **Performance:** O(1); fewer components than full units

#### `WarEntityId we_createBuilding(WarRaceType race, const char* buildingType, int x, int y)`
- **Purpose:** Create a building entity (barracks, farm, blacksmith, etc.)
- **Inputs:** Similar to `we_createUnit`
- **Outputs:** Entity ID
- **Side effects:** 
  - Creates entity with type `ENTITY_TYPE_BUILDING`
  - Attaches WarBuildingComponent with faction and type info
  - Initializes sprite with correct building graphic
- **Performance:** O(1)

---

### Entity Lookup

#### `WarEntity* we_findEntity(WarEntityId entityId)`
- **Purpose:** Retrieve entity pointer by ID
- **Inputs:** Entity ID (16-bit handle)
- **Outputs:** Pointer to `WarEntity`, or NULL if not found
- **Side effects:** None (read-only)
- **Performance:** O(1) average case via hash map lookup
- **Notes:** Returned pointer valid only until next `we_removeEntity()` call

#### `WarEntity* we_findUIEntity(const char* name)`
- **Purpose:** Retrieve UI entity by name
- **Inputs:** UI element name (e.g., "healthBarFill", "minimapButton")
- **Outputs:** Pointer to `WarEntity`, or NULL if not found
- **Side effects:** None
- **Performance:** **O(n)** linear scan of UI entity list (n ≈ 50); name stored as String, not hashed
- **Optimization:** Could use hash map for names if UI count grows beyond ~100

#### `WarEntity* we_findEntityUnderCursor(int mouseX, int mouseY)`
- **Purpose:** Retrieve entity at specific screen coordinates (for mouse picking)
- **Inputs:** Screen-space mouse coordinates
- **Outputs:** Pointer to topmost entity at coordinates, or NULL
- **Side effects:** None
- **Performance:** O(visible_entity_count); typically O(1) on sparse maps
- **Usage:** Called every frame during mouse event processing

#### `List we_findEntitiesByType(WarEntityType type)`
- **Purpose:** Get all entities of a specific type
- **Inputs:** Entity type filter
- **Outputs:** `List` of `WarEntityId` handles
- **Side effects:** Returns reference to internal list (do not modify)
- **Performance:** O(1) map lookup; result list size varies by entity count
- **Usage:** Batch operations (e.g., "find all buildings to render")

#### `List we_findUnitsByType(WarRaceType race)`
- **Purpose:** Get all units of a specific race/faction
- **Inputs:** Race type (RACE_ORC or RACE_HUMAN)
- **Outputs:** `List` of `WarEntityId` handles
- **Side effects:** None
- **Performance:** O(1) map lookup
- **Usage:** Team/faction-specific queries (e.g., "count all human units")

#### `int we_countEntitiesByType(WarEntityType type)`
- **Purpose:** Count entities of specific type
- **Inputs:** Entity type
- **Outputs:** Integer count
- **Side effects:** None
- **Performance:** O(1) if count cached; O(n) if counted per-call
- **Note:** Check implementation to verify caching

---

### Component Attachment

Each component has dedicated add/remove functions. Example pattern:

#### `void we_addTransformComponent(WarEntityId entityId, int x, int y, float rotation)`
- **Purpose:** Attach transform component to entity
- **Inputs:** 
  - `entityId`: Target entity
  - `x`, `y`: Initial position
  - `rotation`: Initial rotation angle
- **Outputs:** None (void)
- **Side effects:** Entity's transform component is initialized
- **Performance:** O(1)

#### `void we_addSpriteComponent(WarEntityId entityId, const char* spriteKey, int frameIndex)`
- **Purpose:** Attach sprite component to entity
- **Inputs:** 
  - `spriteKey`: Resource key (e.g., "unit_grunt_idle")
  - `frameIndex`: Initial frame in sprite sheet
- **Outputs:** None
- **Side effects:** 
  - Entity sprite initialized from resource cache
  - Sprite resource reference count incremented
- **Performance:** O(1) if sprite already loaded; O(k) if cache miss (k = sprite load time)
- **Notes:** Avoid calling inside tight loop with many unique sprite keys

#### `void we_addUnitComponent(WarEntityId entityId, const char* unitType, WarRaceType race)`
- **Purpose:** Attach full unit stats and AI component
- **Inputs:** 
  - `unitType`: "grunt", "footman", etc.
  - `race`: Faction
- **Outputs:** None
- **Side effects:** 
  - Unit component initialized with base stats (HP, damage, armor, sight range)
  - Entity added to `unitsByType` map
  - State machine initialized to IDLE
- **Performance:** O(1)
- **Notes:** Called automatically by `we_createUnit()`; rarely called manually

#### Component Removal Pattern

Each component type has a corresponding remove function:

```c
void we_removeSpriteComponent(WarEntityId entityId);
void we_removeUnitComponent(WarEntityId entityId);
void we_removeButtonComponent(WarEntityId entityId);
// ... (one for each component type)
```

- **Purpose:** Detach a component from entity
- **Side effects:** 
  - Component cleanup (free lists, decrement sprite resource refs, stop animations)
  - Component data zeroed
  - Entity lists updated
- **Performance:** O(1) cleanup; O(n) if component maintains internal list to clean
- **Notes:** Does **not** destroy entity; only removes the component

---

### Entity Removal & Lifecycle

#### `void we_removeEntity(WarEntityId entityId)`
- **Purpose:** Mark entity as inactive and perform full cleanup
- **Inputs:** Entity ID
- **Outputs:** None
- **Side effects:** 
  - Entity marked as inactive
  - All components cleaned up (resources freed, lists destroyed)
  - Entity removed from manager maps
  - State machine torn down if present
- **Performance:** O(component_count); typically O(1) in practice
- **Thread safety:** Guarded by mutex if called from concurrent thread
- **Notes:** After this call, entity ID remains allocated (can be queried but returns inactive entity)

#### `void we_removeEntityById(WarEntityId entityId)`
- **Purpose:** Synonym for `we_removeEntity()`; same semantics
- **Inputs:** Entity ID
- **Outputs:** None

---

### Global Manager Queries

#### `WarEntityManager* we_getEntityManager()`
- **Purpose:** Retrieve pointer to global entity manager
- **Inputs:** None
- **Outputs:** Pointer to `WarEntityManager`
- **Side effects:** None
- **Performance:** O(1)
- **Usage:** Rarely needed; mostly for internal subsystem coordination

#### `int we_getEntityCount()`
- **Purpose:** Get total count of active entities
- **Inputs:** None
- **Outputs:** Integer count
- **Side effects:** None
- **Performance:** O(1) if count maintained; O(n) if counted per-call

---

## Data Structures

### WarEntity

The central entity struct, containing all embedded components:

```c
typedef struct {
    WarEntityId id;                                 // Unique 16-bit handle
    WarEntityType type;                             // Type enum
    bool active;                                    // Is entity active?
    bool isUI;                                      // Is UI entity?
    
    // Transform: position, rotation, scale
    WarTransformComponent transform;
    
    // Visual: sprite, animation state
    WarSpriteComponent sprite;
    WarAnimationsComponent animations;
    
    // Game object: unit-specific stats and behavior
    WarUnitComponent unit;                          // ~150 bytes; large but essential
    
    // AI & behavior: state machine
    WarStateMachineComponent stateMachine;
    
    // UI-specific components
    WarButtonComponent button;
    WarTextComponent text;
    WarImageComponent image;
    WarRectComponent rect;
    WarTooltipComponent tooltip;
    
    // Specialized decorative components
    WarRoadComponent road;
    WarWallComponent wall;
    WarForestComponent forest;
    
    // Collision & sight
    WarCollisionComponent collision;
    WarSightComponent sight;
} WarEntity;
```

**Memory Layout:** ~2 KB per entity; all components co-located for cache efficiency.

### WarTransformComponent

Position, rotation, scale:

```c
typedef struct {
    int x, y;                                       // Tile grid coordinates
    float rotation;                                 // Angle in radians
    float scaleX, scaleY;                           // Visual scale (1.0 = 100%)
    int screenX, screenY;                           // Cached screen space (updated by render)
} WarTransformComponent;
```

**Notes:** 
- Position stored in **tile grid coordinates** (not pixels)
- `screenX`, `screenY` are computed during render for mouse picking

### WarSpriteComponent

Sprite graphics and animation state:

```c
typedef struct {
    String spriteKey;                               // Resource key (e.g., "unit_grunt_idle")
    WarSpriteResource* sprite;                      // Pointer to cached sprite resource
    int frameIndex;                                 // Current frame in sprite sheet
    float frameTime;                                // Elapsed time in current frame
    int layer;                                      // Rendering z-order
} WarSpriteComponent;
```

**Performance:** `sprite` pointer cached to avoid hash map lookup per frame.

### WarUnitComponent

Game-object stats, resources, AI actions (large component):

```c
typedef struct {
    WarRaceType race;                               // RACE_ORC or RACE_HUMAN
    String unitType;                                // "grunt", "footman", etc.
    u8 hp, maxHp;                                   // Health
    u8 damage, armor;                               // Combat stats
    u8 sightRange;                                  // Visibility radius
    u8 tileWidth, tileHeight;                       // Footprint on grid
    
    List actionQueue;                               // Queue of pending actions
    WarAction currentAction;                        // Action in-progress
    
    WarGameActionType currentActionType;            // GATHER_WOOD, BUILD, ATTACK, etc.
    
    // Resource gathering state
    int gatheringGold, gatheringWood;               // Collected resources
    int gatheringTarget;                            // Target entity ID
    
    // Pathfinding state
    List path;                                      // Tile path from A*
    int pathIndex;                                  // Current waypoint index
    
    // Behavior state
    WarPlayerType ownerPlayer;                      // Owner (human or AI)
    
    // Spell/ability timers (per-unit cooldowns)
    float spellCooldown[8];                         // Array of ability cooldowns
    
    // References to manager for queries
    WarEntityManager* manager;
} WarUnitComponent;
```

**Size:** ~150 bytes; large but rarely accessed in tight loops.

**Notes:** 
- Includes full action queue for command buffering
- Spell timers enable per-unit ability cooldowns
- All lists embedded, no separate heap allocation

### WarStateMachineComponent

Finite state machine for unit behavior:

```c
typedef struct {
    WarStateType currentState;                      // Current FSM state
    float stateTime;                                // Time in current state
    WarStateContext context;                        // State-specific data
} WarStateMachineComponent;
```

**States:** IDLE, MOVE, ATTACK, GATHER, BUILD, DIE, ATTACK_BUILDING, etc.

**Transitions:** Driven by unit actions and external events.

### WarButtonComponent

UI button state and handlers:

```c
typedef struct {
    bool isHot;                                     // Mouse over?
    bool isActive;                                  // Pressed?
    void (*onClickHandler)(WarEntity* entity);     // Click callback
    String tooltipText;                             // Hover text
} WarButtonComponent;
```

### WarTextComponent

Rendered text:

```c
typedef struct {
    String text;                                    // Text content
    String fontKey;                                 // Font resource key
    WarAlignment alignment;                         // Text alignment
    WarColor color;                                 // Text color (RGBA)
    bool highlighted;                               // Selection highlight
} WarTextComponent;
```

### WarAnimationsComponent

Active animations queue:

```c
typedef struct {
    List activeAnimations;                          // List of WarAnimation structs
    float globalAnimationTime;                      // Global time accumulator
} WarAnimationsComponent;
```

### WarEntityManager

Global entity manager struct:

```c
typedef struct {
    WarEntity entities[MAX_ENTITIES_COUNT];         // Entity pool
    WarEntityMap entitiesByType;                    // WarEntityType → List<WarEntityId>
    WarUnitMap unitsByType;                         // WarRaceType → List<WarEntityId>
    WarEntityIdMap entitiesById;                    // WarEntityId → WarEntity*
    int activeEntityCount;                          // Count of active entities
} WarEntityManager;
```

---

## Usage Example

### Basic Entity Lifecycle

```c
// Pseudo-code: Entity creation, updates, removal in game loop

void initGameScene() {
    // 1. Initialize entity manager at scene start
    we_initEntityManager();
    
    // 2. Create some units and buildings
    WarEntityId gruntId = we_createUnit(RACE_ORC, "grunt", 10, 5);
    WarEntityId farmId = we_createBuilding(RACE_ORC, "farm", 15, 10);
    WarEntityId arrowId = we_createEntity(ENTITY_TYPE_PROJECTILE);
    
    // 3. Customize entities if needed
    WarEntity* grunt = we_findEntity(gruntId);
    if (grunt) {
        grunt->unit.hp = 50;  // Damage it
        grunt->transform.rotation = 1.57f;  // 90 degrees
    }
}

void updateGameLoop(float deltaTime) {
    // 1. Query all units of a type
    List humanUnits = we_findUnitsByType(RACE_HUMAN);
    for (int i = 0; i < list_length(humanUnits); i++) {
        WarEntityId unitId = list_get(humanUnits, i);
        WarEntity* unit = we_findEntity(unitId);
        if (unit && unit->active) {
            // Update unit behavior via state machine
            wst_updateStateMachine(unit->stateMachine, deltaTime);
            
            // Update animations
            wanim_updateAnimations(&unit->animations, deltaTime);
        }
    }
    
    // 2. Check for mouse picking
    WarEntity* pickedEntity = we_findEntityUnderCursor(inputX, inputY);
    if (pickedEntity && pickedEntity->isUI) {
        // Handle UI interaction
    }
    
    // 3. Render all entities
    List allEntities = /* iterate entity pool */;
    for (int i = 0; i < MAX_ENTITIES_COUNT; i++) {
        WarEntity* entity = &manager->entities[i];
        if (entity->active && !entity->isUI) {
            wr_drawEntity(entity);
        }
    }
}

void cleanupEntity() {
    // Full removal: cleanup + deactivation
    we_removeEntity(gruntId);  // Grunt removed, cleaned up
    
    // After removal:
    // - Entity sprite resources freed
    // - State machine torn down
    // - Action queue cleared
    // - Entity marked inactive
    // - Entity ID can still be queried, but returns inactive state
}
```

### Component Attachment Pattern

```c
// Create an entity
WarEntityId myEntity = we_createEntity(ENTITY_TYPE_DECORATION);

// Attach components manually
we_addTransformComponent(myEntity, 5, 10, 0.0f);
we_addSpriteComponent(myEntity, "tree_sprite", 0);
we_addAnimationsComponent(myEntity);

// Query and use
WarEntity* entity = we_findEntity(myEntity);
entity->sprite.frameIndex = 2;
entity->transform.rotation = 3.14f;
```

---

## Dependencies

### Internal Dependencies (War1-C Modules)

- **`war_units.h` / `war_units.c`:** Unit type definitions, stat tables, unit creation helpers
- **`war_resources.h` / `war_resources.c`:** Sprite resource caching, resource loading
- **`war_animations.h` / `war_animations.c`:** Animation playback for entities
- **`war_state_machine.h` / `war_state_machine.c`:** FSM tick, state transitions
- **`war_alloc.h` / `war_alloc.c`:** Memory allocation (wm_alloc/wm_free)
- **`war_render.h` / `war_render.c`:** Drawing entities (wr_drawEntity)
- **`war_audio.h` / `war_audio.c`:** MIDI playback for units
- **`war_actions.h`:** Unit action definitions
- **`war_fwd.h`:** Forward declarations and type definitions

### External Dependencies

- **SHL Libraries:** `shl/list.h`, `shl/map.h` (hash maps, linked lists)
- **STB Libraries:** `stb/ds.h` (dynamic arrays if used)
- **SDL3:** `SDL3/SDL.h` (threading primitives for mutex)

---

## Performance Considerations

### Cache Locality: Embedded Components

All components stored inline in `WarEntity` guarantee a single cache line fetch retrieves entity + all components. This is significantly faster than pointer-chased component access:

```c
// Good: Single cache line fetch
WarEntity* entity = we_findEntity(id);
int hp = entity->unit.hp;
int x = entity->transform.x;

// Bad (if components were separate): Multiple cache misses
WarUnitComponent* unit = we_findUnitComponent(id);
WarTransformComponent* transform = we_findTransformComponent(id);
```

### O(1) Lookups via Hash Maps

All three lookup maps (`entitiesByType`, `unitsByType`, `entitiesById`) use SHL hash map with average O(1) performance:

```c
// O(1) average: fetch all units of race
List humanUnits = we_findUnitsByType(RACE_HUMAN);
```

**Potential Bottleneck:** UI entity name lookup is O(n) linear scan. For large UI counts (100+), add a name-based hash map.

### Avoid Per-Frame Allocations

All entity structures pre-allocated at startup. No `malloc/free` calls during gameplay, eliminating allocation/fragmentation overhead.

### Batch Rendering Optimization

UI entities tracked separately in `uiEntities` list, avoiding iteration over all 100 entities when rendering UI-only.

### State Machine Update Overhead

Each unit's state machine is ticked every frame via `wst_updateStateMachine()`. For large unit counts (50+), consider:
- Spatial partitioning to skip distant units
- Async state updates on separate thread

### Component Size Trade-off

`WarUnitComponent` is large (~150 bytes). If memory is critical, consider:
- Storing unit stats in separate array (trade cache locality for memory density)
- Lazy-loading spell cooldowns only for units with abilities

---

## Patterns & Idioms

### Batch Operations on Entity Type

```c
List allBuildings = we_findEntitiesByType(ENTITY_TYPE_BUILDING);
for (int i = 0; i < list_length(allBuildings); i++) {
    WarEntityId id = list_get(allBuildings, i);
    WarEntity* building = we_findEntity(id);
    // Process building...
}
```

### Safe Entity Queries with Active Check

```c
WarEntity* entity = we_findEntity(entityId);
if (entity && entity->active) {
    // Safe to use entity
} else {
    // Entity removed or invalid ID
}
```

### Entity Creation and Customization

```c
WarEntityId unitId = we_createUnit(RACE_ORC, "grunt", 10, 5);
WarEntity* unit = we_findEntity(unitId);
if (unit) {
    unit->unit.hp = 30;  // Custom health
    unit->transform.rotation = 0.5f;  // Rotation
}
```

### Deferred Entity Removal

Instead of removing entities immediately, mark them for deferred cleanup:

```c
// Mark for removal (doesn't cleanup yet)
entity->active = false;

// Later, in cleanup phase:
// we_removeEntity(entityId);  // Full cleanup
```

---

## Known Limitations & Future Improvements

1. **UI Entity Name Lookup:** O(n) linear scan. Consider hash map for names if UI count grows beyond 100.

2. **Fixed Entity Pool:** Hard limit of 100 entities. Dynamic resizing would require pointer stability (problematic for embedded components).

3. **Component Removal Fragmentation:** Removing components leaves "holes" in component data. No defragmentation; acceptable for small entity counts.

4. **No Archetype System:** Entities store all 14+ component types even if unused. Archetype-based systems (only allocate components actually used) could reduce memory per entity.

5. **Single-Threaded Hot Path:** Entity lookups and updates assume single-threaded access. Contention on entity manager mutex could become a bottleneck with concurrent subsystems.

---

## See Also

- **UI Subsystem:** `docs/UI_Subsystem.md` — Entity creation for UI elements
- **State Machines:** `war_state_machine.h` — FSM architecture for entity behavior
- **Pathfinding:** `war_pathfinder.h` — A* routing for unit movement
- **Rendering:** `war_render.h` — Drawing entities to screen
