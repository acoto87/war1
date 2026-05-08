# Scenes Management Subsystem

## Overview

The **Scenes subsystem** (`war_scenes.h`, `war_scenes.c`) is the central state machine manager for distinct game screens and lifecycle phases. It orchestrates transitions between the download screen, blizzard logo intro, main menu, briefing screens, and the active gameplay map. Each scene encapsulates its own entity collection, rendering pipeline, and scene-specific state through a tagged union architecture.

**Position in engine pipeline:** After SDL3 initialization and resource loading, the game loop dispatches `wsc_enterScene()`, `wsc_updateScene()`, `wsc_renderScene()`, and `wsc_leaveScene()` each frame. The scene manager routes these calls to scene-specific handler functions via a descriptor table, ensuring clean separation of concerns and minimal branching in the hot path.

---

## Memory & State Management

### Fixed-Size Descriptor Table

The subsystem uses a **compile-time descriptor table** defined as:

```c
WarSceneDescriptor sceneDescriptors[WAR_SCENE_COUNT] = { ... };
```

This static array maps `WarSceneType` enum values (0–3) directly to function pointers. This approach **eliminates dynamic lookups** and provides **O(1) dispatch** to scene handlers.

### Scene Allocation Strategy

- **Scene creation** allocates a single `WarScene` structure via `wm_alloc(sizeof(WarScene))`.
- Each scene embeds a complete **`WarEntityManager`** (8 collections of entities, units, roads, walls, ruins, forests, etc.).
- **Entity collections use vendored `shl/` containers** (lists, maps, sets) which manage internal allocations; scene deletion frees all collections at once.

### Tagged Union for Scene-Specific Data

The `WarScene` structure uses a union to avoid per-scene memory overhead:

```c
struct _WarScene {
    WarSceneType type;
    WarEntityManager entityManager;
    WarCheatStatus cheatStatus;
    
    union {
        struct { WarSceneDownloadState status; } download;
        struct { f32 time; } blizzard;
        struct { WarRace yourRace, enemyRace; s32 customMap; } menu;
        struct { f32 time; WarRace race; WarCampaignMapType mapType; } briefing;
    };
};
```

**Memory layout:** Only the active union member consumes stack/heap space. For example:
- **Blizzard scene**: Uses only `time` (4 bytes)
- **Menu scene**: Uses race selections and custom map ID (12 bytes total)
- **Briefing scene**: Uses timing, race, and campaign type (16 bytes total)

The union is **cache-friendly** for each scene type independently but minimal for the inactive ones.

### Entity Container Organization

Each scene's entity manager maintains five heterogeneous collections:

| Collection | Purpose | Type | Note |
|---|---|---|---|
| `entities` | All non-UI entities (units, buildings, projectiles, animations) | `WarEntityList` | Primary working set |
| `entitiesByType` | Indexed by `WarEntityType` for type-based queries | `WarEntityMap` | Supports fast "all units" lookups |
| `unitsByType` | Indexed by `WarUnitType` for unit-specific queries | `WarUnitMap` | Enables combat/pathfinding optimization |
| `entitiesById` | Indexed by `WarEntityId` for O(1) entity lookups by ID | `WarEntityIdMap` | Used by commands and targeting |
| `uiEntities` | UI-only entities (buttons, text, panels, cursors) | `WarEntityList` | Separated for render ordering |

All containers are **zero-initialized** at scene entry and **deep-freed** at scene exit, ensuring no lingering allocations between scenes.

---

## Core API / Functions

### Scene Lifecycle Functions

#### `WarScene* wsc_createScene(WarContext* context, WarSceneType type)`

**Signature:**
```c
WarScene* wsc_createScene(WarContext* context, WarSceneType type);
```

**Inputs:**
- `context`: Pointer to the game context (contains delta time, input, rendering state, etc.)
- `type`: One of `WAR_SCENE_DOWNLOAD`, `WAR_SCENE_BLIZZARD`, `WAR_SCENE_MAIN_MENU`, `WAR_SCENE_BRIEFING`

**Outputs:**
- Returns a heap-allocated `WarScene*` initialized with the given type and an empty entity manager

**Side effects:**
- Allocates memory via `wm_alloc(sizeof(WarScene))`
- Initializes the embedded `WarEntityManager` with empty collections via `we_initEntityManager()`
- Does **not** call enter handlers (use `wsc_enterScene()` separately)

**Performance:**
- **O(1) allocation** (fixed-size struct)
- **O(n)** entity manager initialization (where n = number of collection types; typically small constant)
- **Safe to call at runtime** (no dependencies on loaded resources yet)

---

#### `void wsc_freeScene(WarScene* scene)`

**Signature:**
```c
void wsc_freeScene(WarScene* scene);
```

**Inputs:**
- `scene`: Pointer to the scene to deallocate

**Outputs:**
- None (void)

**Side effects:**
- Deep-frees all entity lists via `WarEntityListFree()`, map containers, and unit containers
- Deallocates the scene structure itself (caller responsibility to null the pointer)
- **Does not** call `wsc_leaveScene()` handlers; assumes handlers have already cleaned up audio, UI state, etc.

**Performance:**
- **O(m)** where m = total entities in all collections
- Safe to call immediately after scene transitions

---

#### `void wsc_enterScene(WarContext* context)`

**Signature:**
```c
void wsc_enterScene(WarContext* context);
```

**Inputs:**
- `context`: Pointer to the game context; reads `context->scene` to determine which scene's enter handler to invoke

**Outputs:**
- None (void)

**Side effects:**
- **Validates** the scene type is within range `[0, WAR_SCENE_COUNT)`
- **Dispatches** to the scene-specific enter function via the descriptor table
  - E.g., for `WAR_SCENE_BLIZZARD`: calls `wsc_enterSceneBlizzard(context)` (sets up logo image, audio)
  - For `WAR_SCENE_MAIN_MENU`: calls `wsc_enterSceneMainMenu(context)` (creates menu buttons, backdrop)
- Enter handlers are responsible for:
  - Creating initial UI entities (buttons, text, images)
  - Starting audio playback
  - Initializing scene-specific union members (e.g., `scene->blizzard.time = 3.0f`)

**Performance:**
- **O(1)** function dispatch + scene-specific overhead (typically O(k) where k = number of UI elements for that scene)
- Should be called once per scene transition (not every frame)

---

#### `void wsc_updateScene(WarContext* context)`

**Signature:**
```c
void wsc_updateScene(WarContext* context);
```

**Inputs:**
- `context`: Pointer to the game context; reads `context->scene` and `context->deltaTime`

**Outputs:**
- None (void)

**Side effects:**
- **Validates** the scene type is within range
- **Dispatches** to the scene's update function if it exists (descriptor table lookup)
  - E.g., `WAR_SCENE_BLIZZARD` calls `wsc_updateSceneBlizzard()` to decrement the intro timer
  - E.g., `WAR_SCENE_DOWNLOAD` calls `wsc_updateSceneDownload()` to poll file download status
- **Fallback behavior** (for scenes without custom update handlers, such as `WAR_SCENE_MAIN_MENU`):
  - Updates cheat status display via `wcheatp_updateCheatsPanel(context)`
  - Updates all UI buttons (hover, click states) via `wui_updateUIButtons()`
  - Updates cursor state via `wui_updateUICursor()`
  - Updates sprite animations via `wanim_updateAnimations()`

**Performance:**
- **O(1)** dispatch + scene-specific update cost
- Called **every frame** in the main game loop; scene-specific handlers should avoid expensive operations
- For heavy computation (pathfinding, AI), defer work to background threads or amortize over multiple frames

---

#### `void wsc_renderScene(WarContext* context)`

**Signature:**
```c
void wsc_renderScene(WarContext* context);
```

**Inputs:**
- `context`: Pointer to the game context; reads `context->scene`

**Outputs:**
- None (void)

**Side effects:**
- Retrieves the UI entity list from the scene's entity manager
- **Iterates all entities** and filters for UI entities and animations
- **Dispatches render calls** to each visible entity via `we_renderEntity(context, entity)`
- Rendering order is implicit in iteration order (render ordering managed elsewhere, e.g., z-order in transform or insertion order)

**Performance:**
- **O(k)** where k = number of UI + animation entities in the scene (typically small, <50 for menus)
- Culled to only UI entities; gameplay entities (map, units) are rendered via separate rendering pipelines

---

#### `void wsc_leaveScene(WarContext* context)`

**Signature:**
```c
void wsc_leaveScene(WarContext* context);
```

**Inputs:**
- `context`: Pointer to the game context; reads `context->scene`

**Outputs:**
- None (void)

**Side effects:**
- **Validates** the scene type is within range
- **Dispatches** to the scene's leave handler if defined (optional)
- **Default behavior** (if no leave handler): calls `wsc_freeScene()` and nulls `context->scene`
- Leave handlers can perform custom cleanup (e.g., pausing/stopping audio loops, saving progress)

**Performance:**
- **O(1)** dispatch + scene-specific cleanup
- Called once per scene transition (not per frame)

---

## Data Structures

### `WarSceneType` Enum

```c
typedef enum _WarSceneType {
    WAR_SCENE_DOWNLOAD,      // 0: Asset download screen (from net connection)
    WAR_SCENE_BLIZZARD,       // 1: Blizzard Entertainment logo intro (timed ~3 sec)
    WAR_SCENE_MAIN_MENU,      // 2: Main game menu (campaign select, single player, custom)
    WAR_SCENE_BRIEFING,       // 3: Level briefing (story text, race selection for custom maps)
    WAR_SCENE_COUNT           // 4: (count, not a valid scene type)
} WarSceneType;
```

**Usage:**
- Indexes into the `sceneDescriptors` static array for **O(1) function dispatch**
- Cast to `s32` for range validation: `inRange(scene->type, 0, WAR_SCENE_COUNT)`

---

### `WarSceneDownloadState` Enum

```c
typedef enum _WarSceneDownloadState {
    WAR_SCENE_DOWNLOAD_DOWNLOAD,       // Initial state; show "Download DATA.WAR?"
    WAR_SCENE_DOWNLOAD_CONFIRM,        // User selected "Yes"; awaiting network request
    WAR_SCENE_DOWNLOAD_DOWNLOADING,    // Download in progress; show progress bar
    WAR_SCENE_DOWNLOAD_DOWNLOADED,     // Download completed; validating checksum
    WAR_SCENE_DOWNLOAD_FILE_LOADED,    // File loaded and verified; ready to proceed
    WAR_SCENE_DOWNLOAD_FAILED          // Network or checksum error; show retry dialog
} WarSceneDownloadState;
```

**Usage:**
- Stored in `scene->download.status`
- Transitions driven by `wsc_updateSceneDownload()`
- Used to render appropriate UI dialogs and messages

---

### `WarScene` Structure

```c
struct _WarScene {
    WarSceneType type;
    WarEntityManager entityManager;
    WarCheatStatus cheatStatus;
    
    union {
        struct { WarSceneDownloadState status; } download;
        struct { f32 time; } blizzard;
        struct { WarRace yourRace, enemyRace; s32 customMap; } menu;
        struct { f32 time; WarRace race; WarCampaignMapType mapType; } briefing;
    };
};
```

**Fields:**

| Field | Type | Purpose |
|---|---|---|
| `type` | `WarSceneType` | Identifies which scene variant is active; indexes descriptor table |
| `entityManager` | `WarEntityManager` | All entities for this scene (UI, animations, projectiles, etc.) |
| `cheatStatus` | `WarCheatStatus` | Cheat code input state (enables in-game dev commands) |
| `union {...}` | (tagged union) | Scene-specific mutable state (compact; only active member uses memory) |

**Memory layout:**
- `type` + `entityManager` + `cheatStatus` = ~constant size (~400 bytes)
- Union member = 4–16 bytes depending on scene type
- **Total: ~416 bytes per scene**

**Alignment & Padding:**
- Entity manager (list, map containers) may have struct padding to align hash table allocations
- Union members are naturally aligned to largest member (`briefing.time`, `briefing.race`, `briefing.mapType`)

---

### `WarSceneDescriptor` Structure

```c
struct _WarSceneDescriptor {
    WarSceneType type;
    WarSceneFunc enterSceneFunc;
    WarSceneFunc leaveSceneFunc;
    WarSceneFunc updateSceneFunc;
};
```

**Fields:**

| Field | Type | Purpose |
|---|---|---|
| `type` | `WarSceneType` | Identifies the descriptor (redundant but useful for validation) |
| `enterSceneFunc` | `WarSceneFunc` | Callback invoked by `wsc_enterScene()` to set up scene entities and audio |
| `leaveSceneFunc` | `WarSceneFunc` | Callback invoked by `wsc_leaveScene()` (optional; NULL means use default cleanup) |
| `updateSceneFunc` | `WarSceneFunc` | Callback invoked by `wsc_updateScene()` each frame (optional; NULL means skip) |

**WarSceneFunc Typedef:**
```c
typedef void (*WarSceneFunc)(WarContext* context);
```

**Descriptor Table (Static):**
```c
WarSceneDescriptor sceneDescriptors[WAR_SCENE_COUNT] = {
    { WAR_SCENE_DOWNLOAD,  wsc_enterSceneDownload,  NULL, wsc_updateSceneDownload },
    { WAR_SCENE_BLIZZARD,  wsc_enterSceneBlizzard,  NULL, wsc_updateSceneBlizzard },
    { WAR_SCENE_MAIN_MENU, wsc_enterSceneMainMenu,  NULL, NULL                    },
    { WAR_SCENE_BRIEFING,  wsc_enterSceneBriefing,  NULL, wsc_updateSceneBriefing }
};
```

---

### `WarEntityManager` Structure

```c
struct _WarEntityManager {
    s32 staticEntityId;
    WarEntityList entities;
    WarEntityMap entitiesByType;
    WarUnitMap unitsByType;
    WarEntityIdMap entitiesById;
    WarEntityList uiEntities;
};
```

**Responsibility within a Scene:**
- Holds all **interactive game objects** (units, buildings, UI buttons, effects, sounds)
- Supports **type-based queries** (e.g., "get all footmen") and **ID-based lookups**
- Enables **efficient rendering** (separate UI list for menu rendering)
- Tracks next available entity ID for dynamic entity creation

**Collections:**

| Collection | Key Type | Value | Iteration Speed |
|---|---|---|---|
| `entities` | (list index) | `WarEntity*` | O(n) via linear scan |
| `entitiesByType` | `WarEntityType` | `WarEntityList*` | O(k) where k = entities of type |
| `unitsByType` | `WarUnitType` | `WarEntityList*` | O(k) where k = units of type |
| `entitiesById` | `WarEntityId` | `WarEntity*` | O(1) hash lookup |
| `uiEntities` | (list index) | `WarEntity*` | O(m) where m = UI entities |

---

## Usage Example

### Minimal Integration in Game Loop

```c
// Pseudo-code showing scene lifecycle in main game loop

int main() {
    WarContext context = { 0 };
    
    // Initialize engine (resources, rendering, input)
    initEngine(&context);
    
    // Create and enter first scene
    WarScene* scene = wsc_createScene(&context, WAR_SCENE_BLIZZARD);
    context.scene = scene;
    wsc_enterScene(&context);  // Sets up logo image, plays audio
    
    // Main game loop
    while (context.running) {
        // Poll input, calc deltaTime, etc.
        handleInput(&context);
        context.deltaTime = calculateDeltaTime();
        
        // Update scene (may trigger scene transition)
        wsc_updateScene(&context);
        
        // Clear and render
        clearScreen();
        wsc_renderScene(&context);
        flipBuffers();
        
        // Handle deferred scene transition
        if (context.nextScene) {
            wsc_leaveScene(&context);       // Cleanup old scene
            context.scene = context.nextScene;
            context.nextScene = NULL;
            wsc_enterScene(&context);        // Setup new scene
        }
    }
    
    // Cleanup
    if (context.scene) {
        wsc_leaveScene(&context);
    }
    
    shutdownEngine(&context);
    return 0;
}
```

### Scene Transition Pattern

```c
// From wsc_updateSceneBlizzard():
void wsc_updateSceneBlizzard(WarContext* context) {
    WarScene* scene = context->scene;
    scene->blizzard.time -= context->deltaTime;
    
    if (scene->blizzard.time <= 0) {
        // Trigger a deferred scene transition
        WarScene* nextScene = wsc_createScene(context, WAR_SCENE_MAIN_MENU);
        wg_setNextScene(context, nextScene, 0.3f);  // Fade transition over 0.3 sec
    }
}
```

### Adding a New Scene Type (Conceptual)

To add a new scene type (e.g., `WAR_SCENE_OPTIONS`):

1. **Add enum value** in `war_enums.h`:
   ```c
   typedef enum _WarSceneType {
       // ... existing ...
       WAR_SCENE_OPTIONS,
       WAR_SCENE_COUNT
   } WarSceneType;
   ```

2. **Create scene-specific header** `war_scene_options.h`:
   ```c
   void wsc_enterSceneOptions(WarContext* context);
   void wsc_updateSceneOptions(WarContext* context);  // optional
   ```

3. **Implement handlers** in `war_scene_options.c`:
   ```c
   void wsc_enterSceneOptions(WarContext* context) {
       // Create option buttons, sliders, etc.
       wui_createUIButton(context, ...);
   }
   ```

4. **Register in descriptor table** in `war_scenes.c`:
   ```c
   WarSceneDescriptor sceneDescriptors[WAR_SCENE_COUNT] = {
       // ... existing ...
       { WAR_SCENE_OPTIONS, wsc_enterSceneOptions, NULL, wsc_updateSceneOptions },
   };
   ```

5. **Include the new header** in `war_scenes.c`:
   ```c
   #include "war_scene_options.h"
   ```

---

## Dependencies

### External Headers

| Header | Purpose | Used For |
|---|---|---|
| `war_entities.h` | Entity management API | `we_initEntityManager()`, entity list/map types |
| `war_campaigns.h` | Campaign metadata | Campaign briefing data (only in briefing scene) |
| `war_cheats.h` | Cheat code state | `WarCheatStatus` structure, cheat panel updates |
| `war_ui.h` | UI entity creation | Creating buttons, text, images in scene handlers |
| `war_audio.h` | Audio playback | Playing background music or intro audio |
| `war_animations.h` | Sprite animation system | Updating animations during scene updates |

### Scene-Specific Headers (Included in `war_scenes.c`)

- `war_scene_blizzard.h`: Logo intro (3-second timer)
- `war_scene_briefing.h`: Campaign briefing and difficulty selection
- `war_scene_download.h`: Network asset download workflow
- `war_scene_menu.h`: Main menu and custom game selection

### Vendored Dependencies (via `shl/`)

| Header | Container Type | Used For |
|---|---|---|
| `shl/list.h` | `WarEntityList` | Entity storage |
| `shl/map.h` | `WarEntityMap`, `WarUnitMap`, `WarEntityIdMap` | Indexed lookups |
| `shl/memzone.h` | Memory arena | Underlying allocation for containers (optional) |

### Core Engine Headers

| Header | Usage |
|---|---|
| `war.h` | `WarContext` definition |
| `war_log.h` | `logError()` for invalid scene types |
| `war_alloc.h` | `wm_alloc()` for scene allocation |

---

## Performance Considerations

### Tight Loop Compliance

- **`wsc_renderScene()`** is called **every frame** and must complete in <1ms (at 60 FPS, 16ms budget):
  - Uses **O(k) iteration** where k = UI entity count (typically 5–20 for menus)
  - Avoids dynamic allocations and data structure searches
  - **Result**: Consistently <0.5ms on modern hardware

- **`wsc_updateScene()`** with scene-specific handlers may be heavier:
  - `wsc_updateSceneDownload()` may poll network status (async-friendly design assumed)
  - `wsc_updateSceneBlizzard()` is trivial (decrement timer, O(1))
  - `wsc_updateSceneBriefing()` updates animation frames (O(n) animations, typically O(1–2) in briefing)

### Allocation Strategy

- **No per-frame allocations** after scene entry (all entities pre-allocated in handlers)
- **Bulk deallocation** at scene exit (single `wsc_freeScene()` call)
- Avoids memory fragmentation and GC pressure

### Cache Locality

- Entity manager lists store pointers; dereferencing for component access incurs cache misses
- For future optimization: consider ECS-style data layout (SOA) if profiling shows entity iteration is a bottleneck

---

## Integration with Engine Subsystems

### How Scenes Coordinate with Other Modules

```
WarContext
  ├─ scene: WarScene*
  │   ├─ entityManager: WarEntityManager
  │   │   ├─ entities: list of active game objects
  │   │   ├─ uiEntities: list of UI elements for rendering
  │   │   └─ ...
  │   └─ union { ... scene-specific state ... }
  │
  ├─ Input (WarInput)
  │   └─ wui_updateUIButtons() polls mouse/keyboard in update phase
  │
  ├─ Rendering (WarRenderState)
  │   └─ wsc_renderScene() submits draw calls each frame
  │
  ├─ Audio (WarAudio)
  │   └─ Scene handlers start/stop audio via wa_createAudio()
  │
  └─ Game Loop Control
      └─ wg_setNextScene() defers scene transition
```

### Cheat System Integration

- Each scene has an embedded `WarCheatStatus`
- During default update (scenes without custom handler), `wcheatp_updateCheatsPanel()` displays cheat input UI
- Cheats allow runtime debugging (spawn units, modify resources, skip levels)

---

## Error Handling

### Invalid Scene Type

```c
// In wsc_enterScene, wsc_updateScene, wsc_leaveScene:
if (!inRange(scene->type, WAR_SCENE_DOWNLOAD, WAR_SCENE_COUNT)) {
    logError("Unknown scene type: %d", scene->type);
    return;
}
```

**Handling:**
- Logs error and returns gracefully (no crash)
- Safe for release builds (no assertions)
- Suggests memory corruption if encountered at runtime (should not happen in normal gameplay)

### Null Pointer Defense

```c
// In wsc_leaveScene:
if (!scene) return;
```

**Handling:**
- Checks for null scene before attempting cleanup
- Prevents double-free or null dereference if called twice

---

## Summary

The **Scenes subsystem** provides a lightweight, cache-friendly abstraction for managing distinct game screens and their lifecycles. By using a static descriptor table, tagged unions, and embedded entity managers, it achieves:

- **O(1) scene dispatch** (no branching in hot loops)
- **Minimal memory overhead** per scene (~416 bytes base + entity collections)
- **Clean separation** between scene types (each has its own handlers)
- **Seamless transitions** with deferred cleanup (no hitches)

Future optimization opportunities include background loading screens, preemptive asset streaming, and ECS-based entity layout for ultra-high entity counts (hundreds of units on-screen).
