# War1 — Technical Specification

> A comprehensive technical specification for the War1 codebase: a C-based remake of *Warcraft: Orcs & Humans* (Blizzard Entertainment, 1994).

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Repository Structure](#2-repository-structure)
3. [Key Technologies and Dependencies](#3-key-technologies-and-dependencies)
4. [Build System](#4-build-system)
5. [Architecture Overview](#5-architecture-overview)
6. [Core Data Types and Structures](#6-core-data-types-and-structures)
7. [Subsystems](#7-subsystems)
   - 7.1 [Main Entry Point and Game Loop](#71-main-entry-point-and-game-loop)
   - 7.2 [Asset Pipeline (DATA.WAR)](#72-asset-pipeline-datawar)
   - 7.3 [Rendering](#73-rendering)
   - 7.4 [Audio](#74-audio)
   - 7.5 [Entity-Component System](#75-entity-component-system)
   - 7.6 [State Machine (Unit AI)](#76-state-machine-unit-ai)
   - 7.7 [Pathfinding](#77-pathfinding)
   - 7.8 [Map System](#78-map-system)
   - 7.9 [Scene Management](#79-scene-management)
   - 7.10 [Command System](#710-command-system)
   - 7.11 [AI System](#711-ai-system)
   - 7.12 [UI System](#712-ui-system)
   - 7.13 [Networking](#713-networking)
   - 7.14 [Cheat System](#714-cheat-system)
   - 7.15 [Campaign and Level Data](#715-campaign-and-level-data)
8. [Utility Modules](#8-utility-modules)
9. [Cross-Platform Support](#9-cross-platform-support)
10. [CI/CD](#10-cicd)
11. [Current Feature Status](#11-current-feature-status)
12. [Glossary](#12-glossary)

---

## 1. Project Overview

**War1** is an open-source, from-scratch remake of the classic 1994 RTS game *Warcraft: Orcs & Humans*. It is written entirely in **C (C99 standard)** and uses the original game's asset file (`DATA.WAR`) for sprites, music, sound effects, level data, and tilesets.

| Attribute       | Value                                           |
| --------------- | ----------------------------------------------- |
| Language        | C (C99)                                         |
| License         | zlib License                                    |
| Author          | Alejandro Coto Gutiérrez                        |
| Rendering API   | OpenGL ES 2.0 (via GLAD + NanoVG)               |
| Windowing       | GLFW                                            |
| Audio           | Miniaudio + TinySoundFont (SoundFont2 MIDI)     |
| Target FPS      | 60                                              |
| Original Resolution | 320×200 (scaled 3× by default to 960×600)   |

The project does **not** distribute any Blizzard assets. Users must supply a `DATA.WAR` file from the original game (retail or shareware/demo) to play.

---

## 2. Repository Structure

```
war1/
├── .github/
│   └── workflows/
│       └── build.yml              # CI: Linux (gcc/clang) + Windows (MSVC --check)
├── assets/                        # Runtime assets (fonts, SoundFont)
│   ├── GMGeneric.SF2              # General MIDI SoundFont for music playback
│   ├── war1_font_1.png            # In-game bitmap font (style 1)
│   └── war1_font_2.png            # In-game bitmap font (style 2)
├── deps/
│   ├── include/                   # Third-party header-only/single-file libraries
│   │   ├── GLFW/                  # GLFW windowing library headers
│   │   ├── KHR/                   # Khronos platform header
│   │   ├── glad/                  # OpenGL loader (glad.h + glad.c)
│   │   ├── miniaudio/             # Audio playback library (single-header)
│   │   ├── nanovg/                # 2D vector graphics on OpenGL (nanovg.c/.h)
│   │   ├── shl/                   # Custom single-header data structure library
│   │   ├── stb/                   # stb_image_write, stb_image_resize
│   │   └── TinySoundFont/         # SoundFont2 MIDI synthesizer (tsf.h, tml.h)
│   └── lib/                       # Pre-compiled GLFW libraries per platform
│       ├── arm32/
│       ├── linux64/
│       ├── win32/
│       └── win64/
├── pics/                          # Screenshots/GIFs for README
├── src/                           # All game source code (~29,000 lines)
│   ├── war1.c                     # Main entry point — unity build root
│   ├── war_types.h                # Central type definitions (~2,300 lines)
│   ├── war.h                      # Game constants, macros, and core declarations
│   ├── war_game.c                 # Game initialization, main loop callbacks
│   ├── war_file.c                 # DATA.WAR file parser
│   ├── war_database.h             # Asset index/catalog of all 583 DATA.WAR entries
│   ├── war_resources.c            # Resource loading/decoding from DATA.WAR
│   ├── war_render.c               # NanoVG-based sprite batch rendering
│   ├── war_audio.c                # Audio engine (MIDI music + WAV/VOC SFX)
│   ├── war_entities.c/h           # Entity creation, component management
│   ├── war_units.c/h              # Unit stat database, behaviors (75 KB data)
│   ├── war_map.c/h                # Map logic, tile states, fog of war, coordinate transforms
│   ├── war_pathfinder.c/h         # A* and BFS pathfinding on tile grid
│   ├── war_state_machine.c/h      # State machine dispatcher
│   ├── war_state_machine_*.c      # Individual state implementations (18 files)
│   ├── war_commands.c/h           # Player command execution (train/build/upgrade/spell)
│   ├── war_campaigns.c            # Campaign progression data
│   ├── war_ai.c/h                 # Computer player AI
│   ├── war_net.c/h                # HTTP networking (demo DATA.WAR download)
│   ├── war_cheats.c/h             # Cheat code system
│   ├── war_cheats_panel.c         # Cheat UI overlay
│   ├── war_ui.c/h                 # UI widget creation and rendering
│   ├── war_map_ui.c/h             # In-game HUD (minimap, command panel, resources)
│   ├── war_map_menu.c/h           # In-game menu overlays
│   ├── war_scenes.c/h             # Scene lifecycle management
│   ├── war_scene_blizzard.c       # Blizzard logo splash scene
│   ├── war_scene_menu.c/h         # Main menu / single player / custom game
│   ├── war_scene_briefing.c       # Campaign briefing screen
│   ├── war_scene_download.c       # DATA.WAR download scene
│   ├── war_sprites.c              # Sprite sheet creation from resources
│   ├── war_font.c                 # Bitmap font rendering
│   ├── war_animations.c           # Sprite animation system
│   ├── war_actions.c              # Unit action step sequencing
│   ├── war_projectiles.c          # Projectile entity logic
│   ├── war_roads.c                # Road tile auto-connect logic
│   ├── war_walls.c                # Wall tile logic and damage
│   ├── war_ruins.c                # Building ruin placement
│   ├── war_trees.c                # Forest / tree tile logic and chopping
│   ├── war_math.h                 # Vec2, rect, color math utilities
│   ├── utils.h                    # Primitive typedefs, bit operations, memory helpers
│   ├── log.h                      # Thread-safe logging system
│   ├── io.h                       # File I/O utilities
│   ├── glutils.h                  # OpenGL error checking / diagnostics
│   └── mutex.h                    # Cross-platform pthread mutex abstraction
├── nob.c                          # Build system entry point (nob build tool)
├── nob.h                          # nob library header
├── build-gcc-linux64.sh           # Legacy build script (Linux 64-bit)
├── build-gcc-win32.bat            # Legacy build script (Windows 32-bit)
├── build-gcc-win64.bat            # Legacy build script (Windows 64-bit)
├── build-gcc-rpi.sh               # Legacy build script (Raspberry Pi)
├── run-*.sh / run-*.bat           # Run scripts per platform
├── debug-*.sh / debug-*.bat       # Debug launch scripts
├── cinematics.txt                 # Reference notes on FLIC cinematic format
├── todo.md                        # Feature TODO list
├── README.md                      # Project overview and usage guide
└── LICENSE                        # zlib license
```

### Source Code Statistics

| Category                  | Lines of Code (approx.) |
| ------------------------- | ----------------------- |
| Total (`src/`)            | ~29,150                 |
| Largest file: `war_map.c` | ~2,724                  |
| Type definitions          | ~2,292                  |
| Unit data (`war_units.h`) | ~1,950                  |
| Entity system             | ~2,246                  |
| Build system (`nob.c`)    | ~477                    |

---

## 3. Key Technologies and Dependencies

### Graphics Stack

| Component | Library | Role |
| --------- | ------- | ---- |
| Windowing & Input | [GLFW 3](https://www.glfw.org/) | Window creation, OpenGL context, keyboard/mouse input |
| OpenGL Loader | [GLAD](https://glad.dav1d.de/) | Loads OpenGL ES 2.0 function pointers at runtime |
| 2D Rendering | [NanoVG](https://github.com/memononen/nanovg) | Hardware-accelerated 2D vector/sprite rendering on top of OpenGL ES 2.0 |
| Image I/O | [stb_image_write](https://github.com/nothings/stb), [stb_image_resize](https://github.com/nothings/stb) | Screenshot export and image scaling |

### Audio Stack

| Component | Library | Role |
| --------- | ------- | ---- |
| Audio I/O | [Miniaudio](https://github.com/mackron/miniaudio) | Cross-platform audio device initialization and PCM playback |
| MIDI Synthesis | [TinySoundFont](https://github.com/schellingb/TinySoundFont) (`tsf.h` + `tml.h`) | SoundFont2-based MIDI synthesizer for music playback |

### Data Structures

| Component | Library | Role |
| --------- | ------- | ---- |
| Collections | [shl](https://github.com/acoto87/shl) | Single-header generic data structures: array, list, queue, stack, set, map, binary heap, memory buffer |

### All libraries are **header-only or single-file** C libraries vendored directly in `deps/include/`. No package manager is used. GLFW is the only pre-compiled dependency, with platform-specific static/shared libraries in `deps/lib/`.

---

## 4. Build System

### Primary: `nob.c` (Nob Build Tool)

The project uses the [nob](https://github.com/tsoding/nob.h) build paradigm — a self-rebuilding C build program:

1. **Bootstrap** `nob.c` with any C compiler to produce the `nob` binary.
2. **Run** `./nob build --cc <compiler> --target <platform>` to compile the project.
3. `nob` automatically rebuilds itself if `nob.c` changes (`NOB_GO_REBUILD_URSELF`).

**Supported toolchains:**
- **GCC** — Full build and link (all platforms)
- **Clang** — Full build and link (Linux)
- **MSVC** — Compile-check only (`--check`), no linking (missing import lib)

**Supported targets:**

| Target     | Platform          | Output                      |
| ---------- | ----------------- | --------------------------- |
| `linux64`  | Linux x86_64      | `build/linux64/war1`        |
| `arm32`    | Raspberry Pi      | `build/arm32/war1`          |
| `win32`    | Windows x86       | `build/win32/war1.exe`      |
| `win64`    | Windows x86_64    | `build/win64/war1.exe`      |

**Compiler flags (GCC/Clang):**
- `-std=c99 -Wall -Wno-misleading-indentation`
- `-Ideps/include`
- Debug: `-g -D__DEBUG__=1`
- Release: `-O2`

**Linker dependencies:**
- Linux: `-lglfw -lpthread -lm -ldl -no-pie`
- Windows: `-lglfw3 -lopengl32 -lws2_32`

### Unity Build

The project uses a **unity build** (single translation unit) approach. The file `src/war1.c` is the sole compilation unit that `#include`s all other `.c` files, as well as all `.h` headers and third-party library implementations. This means:

- Only two source files are compiled: `src/war1.c` and `deps/include/glad/glad.c`
- All symbols share a single translation unit — no separate linking of object files
- Build times are very fast for a project of this size

### Legacy Scripts

Shell/batch scripts (`build-gcc-*.sh`, `build-gcc-*.bat`) are retained for convenience but `nob.c` is the canonical build entry point.

---

## 5. Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                        war1.c (main)                            │
│   ┌──────────────────────────────────────────────────────────┐  │
│   │                    GLFW Main Loop                        │  │
│   │   inputGame() → updateGame() → renderGame() → present() │  │
│   └──────────────────────────────────────────────────────────┘  │
│                              │                                  │
│                    ┌─────────┴─────────┐                        │
│                    ▼                   ▼                         │
│             ┌─────────────┐    ┌──────────────┐                 │
│             │   Scenes    │    │     Maps     │                 │
│             │  (menus,    │    │  (gameplay   │                 │
│             │   splash)   │    │   levels)    │                 │
│             └─────────────┘    └──────────────┘                 │
│                                       │                         │
│           ┌───────────────────────────┼──────────────────┐      │
│           ▼               ▼           ▼          ▼       ▼      │
│    ┌───────────┐  ┌────────────┐ ┌────────┐ ┌──────┐ ┌─────┐   │
│    │ Entities  │  │  State     │ │  Path  │ │  AI  │ │ UI  │   │
│    │ (ECS-ish) │  │  Machine   │ │ Finder │ │      │ │     │   │
│    └───────────┘  └────────────┘ └────────┘ └──────┘ └─────┘   │
│           │               │                                     │
│    ┌──────┴──────┐  ┌─────┴──────┐                              │
│    ▼             ▼  ▼            ▼                               │
│ ┌────────┐ ┌─────────┐ ┌──────────────┐ ┌──────────────┐       │
│ │Sprites │ │Animation│ │   Commands   │ │   Actions    │       │
│ │& Render│ │ System  │ │(train/build/ │ │(step-based   │       │
│ │        │ │         │ │ upgrade/cast)│ │ sequences)   │       │
│ └────────┘ └─────────┘ └──────────────┘ └──────────────┘       │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                   Foundation Layer                        │   │
│  │  war_file.c │ war_resources.c │ war_audio.c │ war_net.c  │   │
│  │  war_math.h │ utils.h │ log.h │ io.h │ mutex.h          │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │              Third-Party Libraries (vendored)            │   │
│  │  GLFW │ GLAD │ NanoVG │ Miniaudio │ TinySoundFont │ shl │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

### Design Principles

- **Single global context (`WarContext`)**: A central struct holds all game state — window handles, graphics context, audio device, current scene/map, resources, input state, timing, and configuration.
- **Scene/Map duality**: The game alternates between **Scenes** (menus, splash screens, briefings) and **Maps** (actual gameplay levels). Both share the same entity manager infrastructure.
- **Entity–Component pattern**: Game objects are `WarEntity` structs composed of optional components (transform, sprite, unit, road, wall, audio, UI, state machine, animations, etc.).
- **Finite State Machine per unit**: Each unit's behavior is governed by a hierarchical state machine with ~18 distinct states.
- **Data-driven design**: Unit stats, campaign data, and asset metadata are defined as large static arrays/tables in header files.
- **No dynamic dispatch/OOP**: Everything is achieved through C enums, switch statements, and function pointer tables.

---

## 6. Core Data Types and Structures

### Primitive Type Aliases (from `utils.h`)

```c
typedef int8_t   s8;    typedef uint8_t   u8;
typedef int16_t  s16;   typedef uint16_t  u16;
typedef int32_t  s32;   typedef uint32_t  u32;
typedef int64_t  s64;   typedef uint64_t  u64;
typedef float    f32;   typedef double    f64;
```

### Key Enumerations (from `war_types.h`)

| Enum | Purpose | Notable Values |
| ---- | ------- | -------------- |
| `WarUnitType` | All unit and building types | `WAR_UNIT_FOOTMAN` through `WAR_UNIT_GOLDMINE`, plus corpses (~54 values) |
| `WarEntityType` | Entity classification | `UNIT`, `ROAD`, `WALL`, `FOREST`, `PROJECTILE`, `AUDIO`, `CURSOR`, `MINIMAP`, etc. (18 types) |
| `WarStateType` | Unit FSM states | `IDLE`, `MOVE`, `FOLLOW`, `PATROL`, `ATTACK`, `DEATH`, `COLLAPSE`, `GOLD`, `MINING`, `WOOD`, `CHOP`, `DELIVER`, `TRAIN`, `UPGRADE`, `BUILD`, `REPAIR`, `REPAIRING`, `CAST` (18 states) |
| `WarUnitCommandType` | Player-issued commands | `MOVE`, `STOP`, `ATTACK`, `TRAIN_*`, `BUILD_*`, `UPGRADE_*`, `SPELL_*`, `CANCEL` (74 commands) |
| `WarRace` | Faction | `NEUTRAL`, `HUMANS`, `ORCS` |
| `WarResourceType` | DATA.WAR resource types | `IMAGE`, `PALETTE`, `XMID`, `SPRITE`, `WAVE`, `VOC`, `LEVEL_INFO`, `TILESET`, `TILES`, etc. |
| `WarSceneType` | Scene types | `BLIZZARD`, `MENU`, `DOWNLOAD`, `BRIEFING` |
| `WarCampaignMapType` | Campaign level IDs | `HUMANS_01`..`HUMANS_12`, `ORCS_01`..`ORCS_12`, `CUSTOM` |
| `WarSpellType` | Magic spells | `HEALING`, `FAR_SIGHT`, `INVISIBILITY`, `RAIN_OF_FIRE`, `POISON_CLOUD`, `RAISE_DEAD`, `DARK_VISION`, `UNHOLY_ARMOR`, plus 4 summon types |
| `WarUpgradeType` | Research upgrades | `ARROWS`, `SPEARS`, `SWORDS`, `AXES`, `HORSES`, `WOLVES`, etc. |
| `WarFeatureType` | Level feature flags | Controls which units, buildings, and spells are available per map |

### Key Structures

| Structure | File | Purpose |
| --------- | ---- | ------- |
| `WarContext` | `war_types.h` | Global game state — window, graphics, audio, input, resources, current scene/map, timing, configuration |
| `WarEntity` | `war_types.h` | Game entity with optional components (transform, sprite, unit, road, wall, forest, state machine, animations, UI, audio, projectile, etc.) |
| `WarMap` | `war_types.h` | Active gameplay level — tile grid, fog of war, entity manager, pathfinder, viewport, player info, AI, selection state |
| `WarScene` | `war_types.h` | Menu/UI screen — entity manager, type, cheat status |
| `WarState` | `war_types.h` | Unit FSM state instance — type-specific data via tagged union |
| `WarFile` | `war_types.h` | Parsed DATA.WAR file archive header + raw resource table |
| `WarResource` | `war_types.h` | Decoded game resource — tagged union for each resource type (image, palette, sprite, level info, audio, tileset, etc.) |
| `WarPlayerInfo` | `war_types.h` | Per-player game state — race, resources (gold/wood), upgrade levels, feature flags |
| `WarInput` | `war_types.h` | Input state — mouse position, button states, key states, drag state |
| `WarPathFinder` | `war_types.h` | Pathfinding grid — type (BFS/A*), dimensions, tile occupancy data |
| `WarMapPath` | `war_types.h` | Result of a pathfinding query — list of `vec2` nodes |
| `WarSprite` | `war_types.h` | Sprite sheet — NanoVG image handle, frame dimensions, frame data array |
| `WarSpriteAnimation` | `war_types.h` | Animation definition — frame list, timing, loop config, offset/scale |
| `WarUnitAction` | `war_types.h` | Step-based action sequence (idle, walk, attack, death, harvest, repair, build) |

---

## 7. Subsystems

### 7.1 Main Entry Point and Game Loop

**File:** `src/war1.c`

The `main()` function:
1. Initializes the logging system
2. Sets up GLFW error callback
3. Initializes GLFW
4. Calls `initGame()` to set up the `WarContext`
5. Enters the main loop:
   ```
   while (!glfwWindowShouldClose) {
       inputGame()    // Poll GLFW events, update input state
       updateGame()   // Update current scene/map logic
       renderGame()   // Render current scene/map
       presentGame()  // Swap buffers
   }
   ```
6. Cleans up audio, NanoVG, GLFW on exit

**Timing:** Target is 60 FPS (`SECONDS_PER_FRAME = 1/60`). The window title displays real-time FPS and frame time.

### 7.2 Asset Pipeline (DATA.WAR)

**Files:** `war_file.c`, `war_resources.c`, `war_database.h`

The `DATA.WAR` file is the original game's archive format:

1. **Header:** Archive ID (distinguishes retail vs. demo) + entry count (583 entries)
2. **File table:** Array of 583 offsets into the archive
3. **Resources:** Compressed blobs at each offset

**Loading process:**
- `loadWarFile()` reads the archive header and raw resource entries
- `loadResource()` decodes each entry based on the catalog in `war_database.h`
- Resource types include: palettes (768-byte color tables), images, sprites (multi-frame), level info/visual/passable data, tilesets, XMID music, WAV/VOC sound effects, UI images, cursor data, and text

**Supported archive versions:**
- Archive ID `0x18` / `0x1A` → Retail
- Archive ID `0x19` → Demo/Shareware

### 7.3 Rendering

**Files:** `war_render.c`, `war_sprites.c`, `war_font.c`, `glutils.h`

- Uses **OpenGL ES 2.0** profile (via GLAD loader)
- All 2D rendering goes through **NanoVG** (GLES2 implementation)
- Custom **sprite batch system** (`NVGimageBatch`) for efficient rendering of tilesets and sprite sheets — batches multiple quads into a single draw call
- Sprites are uploaded to NanoVG as GPU textures with nearest-neighbor filtering for pixel-art fidelity
- **Bitmap font renderer** (`war_font.c`) renders text using two custom PNG font atlases, with support for horizontal/vertical alignment, wrapping, highlighting, and multiline text
- The game renders at the original 320×200 resolution and scales up via `globalScale` (default 3×)

### 7.4 Audio

**File:** `war_audio.c`

The audio system handles two types of audio:

| Type | Format | Playback Method |
| ---- | ------ | --------------- |
| Music | XMID (MIDI) | TinySoundFont (`tsf.h`) synthesizes MIDI events using `GMGeneric.SF2` SoundFont |
| Sound Effects | WAV/VOC | Direct PCM playback via Miniaudio |

- Audio device initialized via `ma_device_init()` at 44,100 Hz
- A custom audio callback mixes MIDI synthesis and PCM sound effects
- Music and sound volumes are independently controllable
- Thread-safe via mutex protection on the audio context

### 7.5 Entity-Component System

**Files:** `war_entities.c`, `war_entities.h`

Entities follow a **component-based architecture** where each `WarEntity` can have any combination of:

| Component | Purpose |
| --------- | ------- |
| `TransformComponent` | Position in world space |
| `SpriteComponent` | Visual appearance (sprite sheet + current frame) |
| `UnitComponent` | Unit type, HP, mana, speed, damage, armor, player owner, resource carrying, action state |
| `RoadComponent` | Road tile pieces |
| `WallComponent` | Wall tile pieces (with HP) |
| `RuinComponent` | Ruin tile pieces |
| `ForestComponent` | Tree tiles (with wood amount) |
| `StateMachineComponent` | FSM state stack (current + next) |
| `AnimationsComponent` | Active sprite animations |
| `UIComponent` | UI element name and enabled state |
| `TextComponent` | Text rendering properties |
| `RectComponent` | Colored rectangle |
| `ButtonComponent` | Interactive button (normal/pressed sprites, click handler, hot key) |
| `AudioComponent` | Audio playback state |
| `CursorComponent` | Mouse cursor type |
| `ProjectileComponent` | Projectile trajectory (source, target, speed, type) |
| `PoisonCloudComponent` | Area-of-effect damage |
| `SightComponent` | Far Sight/Dark Vision reveal |

The `WarEntityManager` maintains lists of entities organized by type for efficient iteration.

### 7.6 State Machine (Unit AI)

**Files:** `war_state_machine.c`, `war_state_machine.h`, `war_state_machine_*.c` (18 files)

Each game unit is driven by a **finite state machine** with the following states:

| State | File | Behavior |
| ----- | ---- | -------- |
| **Idle** | `_idle.c` | Standing still, optionally looking around; detects nearby enemies |
| **Move** | `_move.c` | Follows a pathfinding route to a destination |
| **Follow** | `_follow.c` | Continuously follows a target entity |
| **Patrol** | `_patrol.c` | Moves back and forth between patrol points |
| **Attack** | `_attack.c` | Engages an enemy (melee or ranged) |
| **Death** | `_death.c` | Plays death animation, transitions to corpse |
| **Collapse** | `_collapse.c` | Building destruction animation |
| **Wait** | `_wait.c` | Timed pause state |
| **Gather Gold** | `_gather_gold.c` | Moves to gold mine |
| **Mining** | `_mining.c` | Extracts gold inside a mine |
| **Gather Wood** | `_gather_wood.c` | Moves to nearest accessible tree |
| **Chopping** | `_chopping.c` | Chops wood from a tree |
| **Deliver** | `_deliver.c` | Returns resources to town hall |
| **Train** | `_train.c` | Building produces a unit over time |
| **Upgrade** | `_upgrade.c` | Building researches an upgrade |
| **Build** | `_build.c` | Under-construction building state |
| **Repair** | `_repair.c` | Unit moves to building to repair |
| **Repairing** | `_repairing.c` | Actively repairing a building |
| **Cast** | `_cast.c` | Casting a spell at target |

Each state implements four callbacks via `WarStateDescriptor`:
- `enterStateFunc` — Called once on state entry
- `leaveStateFunc` — Called once on state exit
- `updateStateFunc` — Called every frame
- `freeStateFunc` — Cleanup

States can be **chained** (a current state and a next state), allowing hierarchical behaviors (e.g., "move then attack").

### 7.7 Pathfinding

**Files:** `war_pathfinder.c`, `war_pathfinder.h`

The pathfinder operates on the 64×64 tile grid:

- **Two algorithms:** BFS and A* (selectable via `PathFindingType` enum)
- **Tile occupancy:** Each tile stores a 16-bit value encoding:
  - Lower 4 bits: occupancy type (`EMPTY`, `STATIC`, `DYNAMIC`)
  - Upper 12 bits: entity ID of the occupant
- **Dynamic avoidance:** Units close to the pathfinding source are treated as static obstacles to prevent overlap
- **Path re-routing:** `reRoutePath()` can recompute a sub-section of an existing path
- **Helper functions:** `findEmptyPosition()`, `isPositionAccesible()`, `pathExists()`
- **8-directional movement** with support for diagonal paths

### 7.8 Map System

**Files:** `war_map.c`, `war_map.h`

The map system manages active gameplay levels:

- **Map size:** 64×64 mega-tiles (each mega-tile is 16×16 pixels = 2×2 mini-tiles of 8×8)
- **Total map dimensions:** 1024×1024 pixels
- **Viewport:** 240×176 pixels visible area (scrollable)
- **Minimap:** 64×64 pixel overview

**Tile state tracking:**
- Each tile has a `WarMapTile` state: `UNKNOWN`, `FOG`, or `VISIBLE`
- Fog of war updates every 1.0 second based on unit sight ranges

**Coordinate systems and transforms:**
- Screen → Map coordinates
- Screen → Minimap coordinates
- Map → Tile coordinates
- Tile → Map coordinates
- Minimap → Viewport coordinates

**Map scrolling:**
- Arrow keys or mouse edge scrolling (5px gap threshold)
- Configurable scroll speed

### 7.9 Scene Management

**Files:** `war_scenes.c`, `war_scenes.h`, `war_scene_*.c`

The game uses a **scene graph** with distinct scene types:

| Scene | File | Purpose |
| ----- | ---- | ------- |
| Blizzard | `war_scene_blizzard.c` | Splash screen with Blizzard logo |
| Main Menu | `war_scene_menu.c` | Single player, custom game, quit options |
| Briefing | `war_scene_briefing.c` | Campaign mission briefing text |
| Download | `war_scene_download.c` | DATA.WAR file download UI |

Each scene type registers callbacks via `WarSceneDescriptor`:
- `enterSceneFunc` — Setup entities and UI
- `leaveSceneFunc` — Cleanup
- `updateSceneFunc` — Per-frame logic

Transitions between scenes use `setNextScene()` with an optional delay.

### 7.10 Command System

**Files:** `war_commands.c`, `war_commands.h`

The command system handles all player interactions with units and buildings:

- **Unit commands:** Move, Stop, Attack, Harvest, Deliver, Repair
- **Training commands:** 14 unit types (Footman, Grunt, Peasant, Peon, Knight, Raider, Archer, Spearman, Catapult, Conjurer, Warlock, Cleric, Necrolyte)
- **Building commands:** 18 building types including roads and walls
- **Upgrade commands:** 20 research upgrades (weapons, armor, spells)
- **Spell commands:** 8 spells + 4 summons
- **Cancel command:** Abort current training/upgrade/build

Each command is a function `void commandFunc(WarContext*, WarEntity*)` that validates resources, triggers state transitions, and updates the UI.

### 7.11 AI System

**Files:** `war_ai.c`, `war_ai.h`

The AI system provides computer-controlled player behavior:

- `WarAI` struct manages AI state per player
- `WarAICommand` queue-based command execution:
  - `createUnitRequest` — Request to train specific unit types
  - `createWaitForUnit` — Wait until a unit type exists
  - `createSleepForTime` — Timed delay between actions
- `initAIPlayers()` — Setup AI for all non-human players
- `updateAIPlayers()` — Called each frame to process AI decisions

The AI is noted as being in early development with plans for increasingly sophisticated behavior.

### 7.12 UI System

**Files:** `war_ui.c`, `war_ui.h`, `war_map_ui.c`, `war_map_ui.h`

The UI system provides in-game interface widgets:

**Widget types:**
- `UIText` — Text labels with font, size, color, alignment, wrapping
- `UIRect` — Colored rectangles
- `UIImage` — Sprite-based images
- `UICursor` — Custom mouse cursor
- `UITextButton` / `UIImageButton` — Interactive buttons with normal/pressed states, tooltips, hot keys, and click handlers

**In-game HUD (`war_map_ui`):**
- Minimap display
- Resource counters (gold, wood)
- Unit selection panel
- Command button grid
- Unit info/stats display

**Menu system (`war_map_menu`):**
- Pause menu
- Win/Lose result screens
- Options overlay

### 7.13 Networking

**Files:** `war_net.c`, `war_net.h`

Minimal HTTP client for downloading the demo `DATA.WAR` file:

- Cross-platform socket abstraction (Winsock on Windows, BSD sockets on Unix)
- `connectToHost()` — TCP connection establishment
- `requestResource()` — HTTP GET request
- `readResponse()` / `parseHeadersFromResponse()` — HTTP response parsing
- `downloadFileFromUrl()` — High-level download function
- Response buffer: 2 KB read chunks, 5 MB max response size

### 7.14 Cheat System

**Files:** `war_cheats.c`, `war_cheats.h`, `war_cheats_panel.c`

The cheat system supports both original game cheats and custom extensions:

**Original cheats:**
- Gold/lumber increase
- Spell/upgrade unlock
- God mode
- Instant win/lose
- Fog of war toggle
- Level skip (Human/Orc campaigns)
- Speed adjustment

**Custom cheats:**
- Music/sound toggle and volume control
- Global scale and speed adjustment
- Map editing mode
- Rain of Fire trigger
- Unit spawning

Cheats are entered via a text input overlay panel and matched against a descriptor table mapping text patterns to handler functions.

### 7.15 Campaign and Level Data

**Files:** `war_campaigns.c`, `war_database.h`

- 24 campaign levels: 12 Human + 12 Orc missions
- 21 custom/skirmish maps (Forest, Swamp, Dungeon variants)
- Level data stored in DATA.WAR includes: starting units, buildings, roads, walls, gold mines, resource amounts, race assignments, objectives, allowed features/upgrades
- Win/lose conditions checked per-map
- Campaign progression via `nextLevelIndex` in level info

---

## 8. Utility Modules

| Module | File | Capabilities |
| ------ | ---- | ------------ |
| **Math** | `war_math.h` | `vec2` type with full vector math (add, subtract, normalize, distance, dot product, lerp), `rect` type, color utilities, trigonometry helpers, `min`/`max`/`clamp`/`abs` macros |
| **Logging** | `log.h` | Thread-safe logging with severity levels (CRITICAL, ERROR, WARNING, INFO, DEBUG), timestamped output with file/line info, variadic printf-style API |
| **File I/O** | `io.h` | Binary file reading (u8, u16, u32, byte arrays), text file loading, file length |
| **OpenGL Utils** | `glutils.h` | GL version/extension printing, error checking, shader program log output |
| **Mutex** | `mutex.h` | Cross-platform pthread mutex wrapper (pthreads on Unix, WinAPI `CreateMutex` on Windows) |
| **Type Aliases** | `utils.h` | Fixed-width integer aliases, color type (`u8Color`), bit flag macros, memory allocation wrappers, array utilities |

---

## 9. Cross-Platform Support

| Platform | Compiler | Build | Run | Notes |
| -------- | -------- | ----- | --- | ----- |
| Linux x86_64 | GCC, Clang | ✅ | ✅ | Primary development platform |
| Windows x86 | GCC (MinGW) | ✅ | ✅ | Requires glfw3.dll copied to output |
| Windows x64 | GCC (MinGW) | ✅ | ✅ | Requires glfw3.dll copied to output |
| Windows x64 | MSVC | ✅ (check only) | ❌ | Missing glfw3.lib for linking |
| Raspberry Pi (ARM32) | GCC | ✅ | ✅ | ARM build target |

**Platform abstractions:**
- `mutex.h` — Windows `CreateMutex` vs. POSIX `pthread_mutex`
- `war_net.h` — Winsock2 vs. BSD sockets
- `utils.h` — `_access` vs. `access`, `_stricmp` vs. `strcasecmp`
- GLFW handles window/input/OpenGL context portably

---

## 10. CI/CD

**File:** `.github/workflows/build.yml`

The project uses **GitHub Actions** with the following matrix:

| Job | Runner | Compiler | Mode |
| --- | ------ | -------- | ---- |
| Linux (GCC) | `ubuntu-latest` | GCC | Full build |
| Linux (Clang) | `ubuntu-latest` | Clang | Full build |
| Windows (MSVC) | `windows-latest` | MSVC | Compile check only |

**Triggers:** Push to `master` branch, Pull requests

---

## 11. Current Feature Status

### ✅ Implemented
- Complete DATA.WAR file parsing (retail + demo)
- Unit movement with 8-directional pathfinding (A* and BFS)
- Collision detection and tile occupancy tracking
- Unit training and building construction
- Resource gathering (gold mining, wood chopping, delivery)
- Per-map feature specification (allowed units/buildings/spells)
- Win/lose condition evaluation
- Fog of war with periodic visibility updates
- Enemy reactive behavior (attack on proximity, chase)
- Full spell system (8 spells + 4 summons)
- Cheat system (original + custom extensions)
- Campaign progression (24 levels)
- Custom/skirmish game mode
- Audio playback (MIDI music + WAV sound effects)
- In-game demo DATA.WAR download

### 🔧 In Progress / Planned
- AI (basic scripted AI exists, more sophisticated AI planned)
- Cinematics (FLIC file reader exists, integration pending)
- Save/Load game state
- Multiplayer networking (socket infrastructure exists)

---

## 12. Glossary

| Term | Definition |
| ---- | ---------- |
| **DATA.WAR** | The original game's archive file containing all assets |
| **Mega-tile** | 16×16 pixel tile (2×2 mini-tiles), the basic map unit |
| **Mini-tile** | 8×8 pixel tile, the smallest tile unit in the tileset |
| **Tileset** | Collection of tile graphics for a terrain type (forest, swamp, dungeon) |
| **Unity build** | Compilation technique where all source files are included into a single translation unit |
| **XMID** | Extended MIDI format used for music in the DATA.WAR archive |
| **VOC** | Creative Labs audio format used for some sound effects |
| **SoundFont** | SF2 format instrument bank used to synthesize MIDI music |
| **nob** | A C-based self-rebuilding build system (similar to make but written in C) |
| **FSM** | Finite State Machine — the behavioral model for unit AI |
| **ECS** | Entity-Component System — architectural pattern for game objects |
| **NanoVG** | Lightweight 2D vector graphics library running on OpenGL |
