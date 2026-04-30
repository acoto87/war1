# War1-C: Comprehensive Project Analysis

**Author:** Alejandro Coto Gutierrez
**Repository:** https://github.com/acoto87/war1
**License:** zlib
**Date:** April 2026 (updated from March 2026)

---

## 1. What Is This Project?

**War1-C** is a ground-up reimplementation of *Warcraft: Orcs & Humans* (1994) written in C99/C11. It reads the original game's `DATA.WAR` asset file (from the DOS version) and recreates the full gameplay experience using modern cross-platform libraries.

| Layer | Technology |
|-------|-------------|
| Windowing/Input/Events | SDL3 |
| 2D Rendering | SDL3 Renderer (hardware-accelerated) |
| Audio | SDL3 Audio + TinySoundFont (MIDI synthesis) |
| Data Structures | shl (author's own single-header libraries) |
| Build System | `nob.c` (C-based build script) |
| Profiling | Tracy C (opt-in via `TRACY_ENABLE`) |
| Memory | Custom arena allocators (`memzone_t`) |

> **Breaking change since March 2026:** GLFW, NanoVG (OpenGL ES 2.0), and Miniaudio have all been replaced with SDL3 (PR #9). This is the single largest architectural change in the project's history — approximately 100,000 lines of dependency code removed and replaced.

**Targets:** Windows 64-bit (MSVC), Linux x86_64, Raspberry Pi ARM64.
*(Windows 32-bit dropped; Raspberry Pi 32-bit replaced with ARM64.)*

**Codebase:** ~35,000+ lines of C across ~56 source files (after the war_types.h breakup and war_units.c expansion), compiled as a **unity build** (single translation unit — all `.c` files `#include`d into `war1.c`).

---

## 2. Implemented Features

### Core Engine
- **Game loop:** Classic `input → update → render → present` at 60 FPS using `SDL_PollEvent` / `SDL_RenderPresent`
- **Scene system:** Download → Blizzard Splash → Main Menu → Custom Game Menu → Briefing → Map (gameplay)
- **Asset pipeline:** Full `DATA.WAR` archive parser with 583 indexed entries (supports both retail and demo versions)
- **Custom bitmap font system** recreated pixel-by-pixel from the original game
- **Build system:** `nob.c` C-based build script with `--cc` and `--target` flags, replacing per-platform shell scripts
- **CI/CD:** GitHub Actions pipeline building on linux64 (gcc + clang), arm64, and win64 (MSVC) on every push and PR
- **Memory management:** Custom arena allocator with `permanentZone` (512 MB) and `frameZone` (4 MB); all subsystems route allocations through `wm_alloc()`/`wm_free()`
- **Profiling:** Tracy C profiler integrated with 41 zones across game loop, AI, pathfinder, entity system, and animations
- **Logging:** Thread-safe logging via SDL3's `SDL_LogMessage`, routing all `logInfo`/`logError`/`logWarning` calls through a single `wlog_log()` function

### Gameplay — Fully Functional
- **All 30+ unit types** from the original game (both Human and Orc rosters)
- **All buildings** (Farm, Barracks, Town Hall, Church/Temple, Tower, Lumber Mill, Stable/Kennel, Blacksmith, Keep/Spire, Gold Mine)
- **Full unit state machine** with 19 states: Idle, Move, Patrol, Follow, Attack, Gold Gathering, Mining, Wood Gathering, Chopping, Deliver, Death, Collapse, Train, Upgrade, Build, Repair, Repairing, Cast, Wait
- **Resource management:** Gold mining (workers enter mine, extract, deliver) and wood chopping (chop trees, carry lumber, deliver)
- **Building construction** with placement validation, build progress, and completion
- **Unit training** from buildings with resource cost enforcement
- **Upgrades** (weapons, armor, spells) from appropriate buildings
- **Combat:** Melee and ranged attacks with proper damage calculation
- **Projectile system:** Arrows, fireballs, catapult rocks with splash damage
- **All 10 spells:** Healing, Far Sight/Dark Vision, Invisibility, Unholy Armor, Rain of Fire, Poison Cloud, Raise Dead, Summon (Spider, Scorpion, Daemon, Water Elemental)
- **A\* pathfinding** with static and dynamic entity avoidance
- **Fog of war** with three states (unknown, fog, visible), timed decay, and sight radius
- **Full audio:** All 45 MIDI tracks, all sound effects, spatial audio attenuation, separate volume controls
- **Complete HUD:** Minimap, unit portraits, contextual command buttons, resource counters, status bar
- **Campaign:** All 24 maps defined (12 Human, 12 Orc) with briefing text and voiceovers
- **Custom games:** Custom game menu scene, map selection, 21+ maps across Forest, Swamp, and Dungeon tilesets
- **Cheat system:** All original cheats + custom development cheats (god mode, unit spawning, map editing, music/volume control, speed/scale adjustment, add unit)

---

## 3. What Is Left to Implement

### Critical Missing Features

| Feature | Status | Notes |
|---------|--------|-------|
| **AI System** | ~20–25% done | Request/Wait/Sleep commands execute. Attack/Defend commands are defined but fall through to `logWarning`. Building creation code is a stub returning `true` without doing anything. Still hardcoded to `players[1]` only. |
| **Campaign Win/Loss Conditions** | ~8% done | Only maps 01 (Human) and 02 (Orc) check objectives. Remaining 22 maps never detect victory or defeat. |
| **Save/Load** | 0% | `wmm_handleGameOverSave` calls `NOT_IMPLEMENTED()`. No serialization code exists. |
| **Cinematics** | 0% | All 40 FLIC animation files catalogued. `shl/flic.h` reader exists in `deps/`. No playback scene. |
| **Multiplayer** | 0% | Main menu "Multi Player" button is a dead end. `war_net.c` only handles HTTP asset download. |
| **Dialogues** | 0% | No dialogue system. |

### Gameplay Polish (from `todo.md`)

- Palette variation for unit portraits per tileset
- NPC explosion animation on repeated clicking
- Ctrl+click to select all units of same type on screen
- Rally points for trained units
- Animated water/blue-water tiles
- Zoom feature
- State machine return values (so Follow can tell Attack that target is unreachable)
- Better diagonal movement cost in pathfinding
- Actions system description (documentation)
- Idle units do not retaliate when attacked
- Worker collision near town hall during resource delivery

### Technical Debt

- Component dictionary instead of each entity having all components
- Remove BFS pathfinding implementation (superseded by A\*)
- Multi-sprite support for entities
- Separate animation rendering pass (render animations above everything else)
- Better minimap layering
- Dialogue system
- Cutscene system (FLIC playback)
- `rewriteAppendCheatTextInput` and `downloadFileFromUrl` to use `wstr`/`StringView`-friendly implementations

---

## 4. Confirmed Bugs

### Fixed Since March 2026

The following bugs from the previous analysis have been resolved:

1. ~~**Copy-paste bug in palette colors**~~ — Fixed in `wres_getPalette()`. Green and blue channels are now correctly indexed from `palette2`.
2. ~~**Wrong unit type for Orc Blacksmith**~~ — Fixed: `wcmd_buildBlacksmithOrcs` now passes `WAR_UNIT_BLACKSMITH_ORCS`.
3. ~~**Wrong unit type for Raider training**~~ — Fixed: `wcmd_trainRaider` now passes `WAR_UNIT_RAIDER`.
4. ~~**Logic bug in projectile type check**~~ — Fixed: condition is now `&&` (`type != FIREBALL && type != RAIN_OF_FIRE`).
5. ~~**Integer division kills audio interpolation**~~ — Fixed: `wa_changeSampleRate` now uses `1.0f / (f32)factor` with proper linear interpolation between samples.
6. ~~**Wrong loop variable in UI button reset**~~ — Fixed: inner loop now correctly uses `buttons->items[j]`.
7. ~~**`sizeof` instead of `arrayLength`**~~ — Fixed: `war_render.c` now uses `arrayLength(colors)`.
8. ~~**~84 instances of unbounded `sprintf`/`strcpy`/`vsprintf`**~~ — Dramatically reduced; only 2 remaining (see below).

### Active Bugs

#### 1. Duplicate macro definition in `war_units.c`
- **File:** `src/war_units.c:6,14`
- **Issue:** `#define WSV_INIT(s) { s, sizeof(s) - 1 }` is defined twice at lines 6 and 14. Under `-Wpedantic` this is a benign redefinition, but it is dead code that will confuse editors and should be removed.

#### 2. Two remaining `strcpy` calls
- **File:** `src/war_net.c` — `strcpy(cut, urlStr + shift)` — buffer overflow risk if `urlStr` is malformed.
- **File:** `src/war_resources.c` — `strcpy(resource->levelInfo.objectives, ...)` — relies on the destination buffer being large enough for the raw level data.
- **Recommendation:** Replace both with `strncpy` or a length-checked copy.

#### 3. AI building creation is a silent no-op
- **File:** `src/war_ai.c:176–187`
- **Issue:** `wai_tryCreateUnit()` for building types immediately `return true` with a TODO comment. A `WAR_AI_COMMAND_REQUEST` for any building type will mark itself complete without spawning anything. The AI scripting will silently succeed without creating any buildings.

#### 4. AI hardcoded to a single player index
- **File:** `src/war_ai.c:127,360`
- **Issue:** Both `wai_initAIPlayers` and `wai_updateAIPlayers` hardcode `players[1]`. On any map with more than one AI opponent this is wrong. The multi-player AI will not run.

#### 5. AI Attack/Defend commands unimplemented
- **File:** `src/war_ai.c:264–269`
- **Issue:** `WAR_AI_COMMAND_ATTACK` and `WAR_AI_COMMAND_DEFEND` hit the `default` case which logs a warning and returns `true`. Any AI script that issues attack or defend commands will silently do nothing.

#### 6. Memory leak on unknown file type
- **File:** `src/war_file.c:40-42`
- **Issue:** Returns `NULL` without freeing the allocated `warFile` struct or closing the file handle on unrecognised file types.

### Pre-Existing High-Risk Issues (still unresolved)

7. **Potential crash from NULL entity manager** (`src/war_entities.c`): `getEntityManager()` can return `NULL`; callers dereference without checking.
8. **Possible crash with 0 selected entities** (`src/war_commands.c`): `selectedEntities.items[0]` accessed in code paths where `selectedEntities.count` could be 0.
9. **VLA buffer overflow in HTTP request** (`src/war_net.c`): Buffer may be too small for multi-line HTTP headers.
10. **AI commands never freed** (`src/war_ai.c:316–331`): `WarAICommandListRemoveAt` removes from list but the `WarAICommand*` pointer is never `wm_free()`d. Permanent zone leak per completed command.
11. **Entities never fully freed** (`src/war_entities.c`): Components cleaned up but the `WarEntity` struct itself is never returned to the allocator.

### Bugs from `todo.md` (Unresolved)

12. **Cursor escapes window** — OS cursor appears outside game window.
13. **Pathfinding edge case** — Last path position occupied by another entity; behavior undefined.
14. **Memory leaks in animation removal**.
15. **Linux rendering** — Global scale renders incorrectly on Linux after a scale change.
16. **UI drag bug** — Dragging from a button to the map panel starts a selection rectangle.
17. **Idle units don't retaliate** when attacked.
18. **Worker collision near town hall** during resource delivery.
19. **Invisible worker bug** — Worker enters mine but doesn't mine; remains commandable.
20. **Audio saturation** from rapid order commands.
21. **Sprite antialiasing lines** visible between sprites on Windows.

---

## 5. Architecture Analysis

### Current Architecture

```
war1.c (entry point + unity build root)
  ├── war_game.c (game loop: input → update → render → present)
  │     ├── war_scenes.c (scene stack management)
  │     │     ├── war_scene_blizzard.c
  │     │     ├── war_scene_download.c
  │     │     ├── war_scene_menu.c (main menu + custom game menu)
  │     │     └── war_scene_briefing.c
  │     └── war_map.c (main gameplay scene)
  │           ├── war_entities.c (ECS-like entity/component system)
  │           ├── war_commands.c (command dispatch)
  │           ├── war_state_machine*.c (19 behavior states)
  │           ├── war_pathfinder.c (A*)
  │           ├── war_ai.c (enemy AI — partial)
  │           ├── war_projectiles.c
  │           └── war_map_ui.c / war_map_menu.c
  ├── war_resources.c + war_file.c (DATA.WAR asset loading)
  ├── war_render.c + war_font.c (SDL3 renderer)
  ├── war_audio.c (SDL3 Audio + TinySoundFont, separate thread)
  ├── war_alloc.c (arena allocator — permanentZone + frameZone)
  └── war_log.c (thread-safe SDL3-backed logging)
```

**Key design patterns:**
- **Entity-Component System** (ECS-like): `WarEntity` has components (sprite, unit, button, animation, etc.)
- **Hierarchical State Machine**: Units driven by enter/update/leave callbacks per state
- **Scene Stack**: Scenes managed as a stack with push/pop semantics
- **Unity Build**: Entire codebase compiled as one translation unit via `war1.c`
- **Arena Allocation**: All game memory allocated from two `memzone_t` arenas; no scattered `malloc`/`free` in game code
- **Forward Declaration Header**: `war_fwd.h` centralises all forward declarations for the now-decomposed type system

### Architectural Strengths

1. **Clean state machine decomposition** — Each of the 19 unit states is in its own file with clear enter/update/leave contracts. Well-organised and easy to extend.

2. **Unified arena memory model** — The `wm_alloc`/`wm_free`/`wm_realloc` API backed by `memzone_t` is now threaded throughout the entire codebase including third-party libraries (TinySoundFont, stb_image, shl). This eliminates fragmentation, enables debug auditing with `memzone_audit.h`, and makes leak tracking possible.

3. **SDL3 unification** — Replacing three separate dependencies (GLFW, NanoVG, Miniaudio) with a single SDL3 layer dramatically reduces the dependency surface. SDL3 handles windowing, input, 2D rendering, audio, threading, and timing under one API with better long-term maintenance guarantees.

4. **Self-contained build system** — `nob.c` is a single C file that bootstraps with one compiler command and drives the full build. Combined with GitHub Actions CI (linux64 GCC+Clang, arm64, win64 MSVC), every commit is compile-verified across all supported targets.

5. **Accurate original game data integration** — The 583-entry `war_database.h` maps every asset in `DATA.WAR` by index. Faithfully reads original game assets including custom map loading.

6. **Tracy profiler integration** — 41 instrumented zones across the game loop, AI, pathfinding, entity system, and animations allow real profiling sessions without code modification. Zero overhead when `TRACY_ENABLE` is not defined.

7. **Thread-safe logging** — The previous global `__log__` state and mutex is gone; `wlog_log()` delegates to `SDL_LogMessage` which is thread-safe by design, fixing the audio thread vs main thread stdout corruption.

### Architectural Weaknesses & Improvement Ideas

#### 1. Monolithic Entity Structure (unchanged)
**Problem:** `WarEntity` contains ALL component types as direct struct members. Every entity carries the full weight of every possible component regardless of its type.

**Recommendation:** Move to a proper ECS with components stored in separate arrays/pools indexed by entity ID. Already in `todo.md`. Would reduce memory footprint and improve cache coherence significantly. The arena allocator makes this refactor safer — a per-component-type pool can be carved from the permanent zone.

#### 2. Unity Build Limits Development Velocity
**Problem:** `war1.c` compiles everything as a single translation unit. Any change recompiles all 35 000+ lines. `nob.c` uses `--check` mode for compile verification but cannot do incremental builds. This is manageable for a solo project, but will become a bottleneck for contributors.

**Note:** The `nob.c` build system does compile the project; the unity build is a deliberate design choice. The issue is that there is no way to compile only the changed files. A CMakeLists.txt companion (compile-only mode) could enable IDE integration and faster iteration without abandoning the unity build for release.

#### 3. AI Architecture Needs a Rethink
**Problem:** The current AI is a linear command script (`WarAICustomData::commands` array walked by index). This works for simple build-order sequences but cannot represent reactive behaviour (defend when attacked, retreat when losing). Attack/Defend commands exist in the enum but are unimplemented. Building creation is a stub. The system is hard-coded to `players[1]`.

**Recommendation:** The scripted command queue is a solid foundation. The next step is:
1. Fix the multi-player loop (iterate all players with `player->ai != NULL`).
2. Implement building creation (find a free worker, find a buildable tile, issue a repair state).
3. Implement attack: select military units and issue a move-to-attack order towards the enemy base.
4. Add a reactive layer: a periodic `wai_scanThreats()` function that inserts defend commands when friendly units take damage.

#### 4. `war_units.c` Centralisation Risk
**Problem:** `war_units.c` has grown to ~2 771 lines and is the single source of truth for all unit stats, upgrade data, upgrade names, feature names, and build time constants. While well-structured, any mistake here silently corrupts game balance data for all unit types.

**Recommendation:** Add compile-time `static_assert` guards verifying array lengths against the corresponding enum `_COUNT` values. This catches off-by-one errors in stat tables at compile time.

#### 5. Unsafe String Handling (nearly resolved, 2 remaining)
**Problem:** The original ~84 instances of `sprintf`/`strcpy`/`vsprintf` have been resolved almost entirely. Two `strcpy` calls remain:
- `src/war_net.c`: URL parsing without bounds check.
- `src/war_resources.c`: Level objectives text copy.

**Recommendation:** Replace both with `strncpy` + explicit null-termination, or a safer helper.

#### 6. No Tests (unchanged)
**Problem:** Zero test files. No test framework. Critical systems like pathfinding, state machines, combat damage, resource management, and DATA.WAR parsing have no automated verification. The CI only checks that the code *compiles* — it does not verify correctness.

**Recommendation:** Add unit tests for:
- Pathfinding (A\* correctness with known obstacle maps)
- Damage calculation
- Resource cost enforcement
- State machine transitions
- DATA.WAR file parsing

---

## 6. Recent Change Log Analysis (Last ~25 Commits)

### What Changed and Why It Matters

| PR / Commit | Change | Impact |
|---|---|---|
| PR #9 | **SDL3 migration** — removed GLFW, NanoVG (OpenGL ES), Miniaudio | Eliminates ~100 kloc of deps; single unified API; better portability |
| PR #8 + String 1 | **`nob.c` + GitHub Actions CI** | Every commit now compile-verified on 3 platforms/4 compilers |
| PR #10 | **Unity build cleanup** — each `.c` file got proper guards and headers | Fixes include ordering; eliminates order-dependent compile errors |
| PR #11 | **Prefix refactor** — `io.c`/`log.c` absorbed into `war_log.c`; `__log__` global removed | Thread-safety; cleaner API surface |
| PR #12 | **`wstr.h` / `memzone.h`** added to shl; Linux SDL3 lib added | Foundation for string refactor and arena allocator |
| PR #13 | **StringView + cheat refactor** — `str.c`/`str.h` removed; `shl/wstr.h` and `shl/wav.h` adopted | Safer, non-owning string passing throughout cheat and UI text paths |
| PR #14 | **Memory + audio refactor** — `war_alloc.c` introduced; Tracy added; all allocs routed through `wm_*` | Arena model; debug auditing; profiling infrastructure |
| PR #15 | **Full function prefix rename** across all 85 source files | Naming conventions now consistently applied; greatly aids code search |
| PR #16 | **`war_types.h` decomposed** — types moved into module headers; `war_fwd.h` for forward decls; `war_units.c` fully populated | Reduced header coupling; `war_units.c` now ~2771 lines of authoritative unit data |
| AI commits | **AI command system** — request/wait/sleep execute; attack/defend stubbed | Foundation for AI is solid; execution layer is partially complete |
| Custom game commits | **Custom game menu + map loading** | Enables AI development and testing without playing campaign maps |

---

## 7. Potential New Development Directions

### High-Value, Feasible
1. **Complete the AI system** — The command infrastructure exists. The highest-leverage work is: (a) multi-player loop, (b) building-construction execution, (c) basic army attack logic. These three items would make AI opponents functional in custom games and set up campaign AI.

2. **Implement all campaign win/loss conditions** — Only 2 of 24 maps have objectives. The original game's objectives are well-documented and the framework already handles the two working cases. Filling in the remaining 22 is largely mechanical.

3. **Integrate FLIC cinematics** — `shl/flic.h` reader already exists. Needs: a playback scene, frame rendering via SDL3 textures, and campaign sequence integration.

4. **Save/Load system** — `wmm_handleGameOverSave` exists as a stub. With the arena allocator, a binary snapshot of the permanent zone is technically feasible, though a structured approach (serialising `WarMap` + entity list + player state) would be more robust and portable.

### Medium-Term
5. **State machine return values** — The most impactful open state machine issue: Follow cannot currently tell Attack that a target is unreachable, causing an infinite Follow↔Attack loop. Fixing this would resolve one of the most noticeable AI/gameplay bugs.

6. **Component dictionary (ECS refactor)** — Already in `todo.md`. Now that the arena allocator is in place, a per-component-type pool approach is feasible without changing the existing allocation API.

7. **Map editor** — Cheat system already has tree/wall/road/ruins editing. Expanding this into a proper editor with save-to-file would be high value for the modding community.

8. **WebAssembly port** — SDL3 has Emscripten support. The arena allocator (one large static buffer) maps naturally to WASM. A browser-playable version would greatly increase accessibility.

### Ambitious / Long-Term
9. **Multiplayer (LAN/Online)** — The command system is centralised and the game loop is deterministic, which is the correct foundation for lockstep networking. The `war_net.c` HTTP stack would need to be repurposed or a separate networking layer added.

10. **Higher resolution rendering** — The game renders at 320×200 scaled up via SDL3's logical presentation. Mode using upscaling filters (xBRZ, HQ2x) could modernise visuals without touching the game logic.

11. **Steam Deck / Console ports** — SDL3 is portable to all major platforms including consoles. The main work would be controller input mapping and resolution adaptation.

---

## 8. Summary of Priorities

| Priority | Task | Impact |
|----------|------|--------|
| **P0** | Fix AI building-creation stub (currently returns `true` silently) | AI correctness |
| **P0** | Fix AI hardcoded to `players[1]` | Multi-AI maps |
| **P0** | Fix 2 remaining `strcpy` calls | Security/stability |
| **P0** | Fix duplicate `WSV_INIT` macro in `war_units.c` | Code hygiene |
| **P1** | Implement AI Attack/Defend command execution | Makes AI fight back |
| **P1** | Complete campaign win/loss conditions (22 remaining maps) | Campaign completable |
| **P2** | Fix state machine return values (Follow → Attack unreachable loop) | Gameplay polish |
| **P2** | Integrate FLIC cinematics playback | Campaign polish |
| **P2** | Implement Save/Load | Player quality of life |
| **P3** | ECS refactor (component dictionary) | Memory and cache efficiency |
| **P3** | Fix memory leaks (AI commands never freed, entities never freed) | Stability for long sessions |
| **P3** | Add compile-time `static_assert` guards in `war_units.c` | Data correctness |
| **P4** | Add unit tests | Long-term maintainability |
| **P4** | WebAssembly port | Accessibility |
| **P5** | Multiplayer | Feature expansion |

---

## Conclusion

This project has made substantial architectural improvements since the March 2026 snapshot. The SDL3 migration eliminated three large dependency stacks in favour of a single well-maintained library. The `nob.c` build system and GitHub Actions CI — previously absent entirely — now provide multi-platform compile verification on every commit. The arena-based memory model and Tracy profiler integration put the project on a professional footing for performance work. The naming conventions and `war_types.h` decomposition have significantly improved code navigability.

The core gameplay loop remains fully functional. The highest-leverage remaining work is completing the AI system to make single-player against an opponent actually playable, then filling in the 22 missing campaign objective checks to make the full campaign completable. With those two items done, this would be a faithfully playable recreation of the original game.

The foundational architecture is sound. The main gaps are feature completeness (AI, campaign, cinematics, save/load) rather than structural problems.
