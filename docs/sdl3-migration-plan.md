# SDL3 Migration Plan: Replacing nanovg and miniaudio

## Overview

This document describes a detailed implementation plan for replacing the following
libraries with SDL3:

- **nanovg** (2D vector graphics renderer)
- **miniaudio** (audio device management / playback)
- **GLFW** (window, input, OpenGL context) — removed as a consequence of replacing nanovg
- **GLAD** (OpenGL ES 2.0 loader) — removed as a consequence of replacing nanovg

It also analyses whether **TinySoundFont (tsf)** and **TML** can be replaced, with a
recommendation to keep them.

---

## Codebase Summary

The project is a Warcraft 1 remake written in C using a **unity build** pattern (all
`.c` files are `#include`'d into `war1.c`). The current dependency stack is:

| Library | Role |
|---|---|
| **GLFW** | Window creation, input handling, OpenGL context, event loop |
| **GLAD** | OpenGL ES 2.0 function loader |
| **nanovg** | 2D vector graphics on top of OpenGL ES 2.0 (images, rects, lines, transforms, batched sprite rendering) |
| **miniaudio** | Audio device initialization and callback-based playback |
| **TinySoundFont (tsf)** | SoundFont2 MIDI synthesizer |
| **TML** | MIDI file parser (companion to tsf) |

---

## Part 1: Replace nanovg (and GLFW + GLAD) with SDL3

**Why this also replaces GLFW and GLAD:** nanovg sits on top of OpenGL ES 2.0 which
is loaded via GLAD and windowed via GLFW. SDL3 provides its own window management,
input handling, and a built-in 2D GPU-accelerated renderer (`SDL_Renderer`). Replacing
nanovg with `SDL_Renderer` makes GLFW, GLAD, and direct OpenGL calls unnecessary.

### 1.1 — Window, Context, and Event Loop (replace GLFW)

**Files affected:** `war1.c`, `war_game.c`, `war_types.h`

- Replace `GLFWwindow*` in `WarContext` with `SDL_Window*` + `SDL_Renderer*`.
- Remove GLFW initialization (`glfwInit`, `glfwCreateWindow`, `glfwWindowHint`, etc.)
  and replace with `SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)`, `SDL_CreateWindow`,
  `SDL_CreateRenderer`.
- Replace the main loop's `glfwWindowShouldClose` with `SDL_EVENT_QUIT` event polling.
- Replace `glfwSwapBuffers` / `glfwPollEvents` with `SDL_RenderPresent` /
  `SDL_PollEvent`.
- Replace `glfwGetTime()` with `SDL_GetTicks()` or `SDL_GetPerformanceCounter()` for
  timing.
- Replace `glfwSetWindowSize`, `glfwGetFramebufferSize`, `glfwSetWindowTitle` with
  SDL3 equivalents.
- Remove `devicePixelRatio` manual calculation — SDL3 handles high-DPI natively.
- Remove the `glad` dependency and all `glViewport` / `glClear` / `glClearColor` calls
  — `SDL_Renderer` handles this.
- Remove `glutils.h` entirely.

### 1.2 — Input Handling (replace GLFW input)

**Files affected:** `war_game.c`, `war_types.h`, `war1.c`

- Replace the polling-based GLFW input (`glfwGetKey`, `glfwGetMouseButton`,
  `glfwGetCursorPos`) with SDL3's event-driven model (`SDL_EVENT_KEY_DOWN/UP`,
  `SDL_EVENT_MOUSE_BUTTON_DOWN/UP`, `SDL_EVENT_MOUSE_MOTION`).
- Map `GLFW_KEY_*` constants to `SDL_SCANCODE_*` or `SDLK_*` equivalents in the
  `WAR_KEY_*` enum or via a translation layer.
- Replace `glfwSetCharCallback` (`inputCharCallback`) with `SDL_EVENT_TEXT_INPUT`
  events.
- Replace `glfwSetInputMode(GLFW_CURSOR_HIDDEN)` with `SDL_HideCursor()`.
- The existing `WarInput` abstraction is a clean boundary — the internal
  `setInputKey` / `setInputButton` functions can be reused; only the source of data
  changes.

### 1.3 — 2D Rendering (replace nanovg)

**Files affected:** `war_render.c`, `war_sprites.c`, `war_font.c`, `war_entities.c`,
`war_scenes.c`, `war_map.c`, `war_map_ui.c`, `war_cheats_panel.c`, `war_types.h`

This is the **largest** part of the migration. nanovg is used for image/texture
management, sprite rendering, primitive drawing, alpha blending, font rendering, and
a canvas-like transform stack.

#### 1.3.1 — Image/Texture Management

- `nvgCreateImageRGBA` / `nvgUpdateImage` / `nvgDeleteImage` / `nvgImageSize` →
  Replace with `SDL_CreateTexture` + `SDL_UpdateTexture` + `SDL_DestroyTexture` +
  `SDL_GetTextureSize`.
- The `WarSprite.image` field (currently an `int` nanovg image handle) becomes
  `SDL_Texture*`.
- Change `NVGcontext* gfx` in `WarContext` to `SDL_Renderer*`.
- Pixel format: nanovg uses RGBA; SDL3 uses `SDL_PIXELFORMAT_RGBA32`. Compatible.
- Set `SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST)` (equivalent of
  `NVG_IMAGE_NEAREST`).

#### 1.3.2 — Sprite Rendering (batched and single)

The custom `NVGimageBatch` / `NVGimageGridBatch` batching system builds raw vertex
arrays and calls nanovg's internal `renderTriangles`. Replace with:

- **Single sprites:** `SDL_RenderTexture` with `SDL_FRect` source and destination
  rects.
- **Batched sprites:** `SDL_RenderGeometry` (closest equivalent to the current
  raw-vertex approach), or simply iterate and call `SDL_RenderTexture` per quad —
  SDL3's renderer batches internally.
- `nvgRenderSubImage` / `nvgRenderImage` → `SDL_RenderTexture` with src/dst
  `SDL_FRect`.

#### 1.3.3 — Primitive Drawing (rects, lines)

| nanovg call | SDL3 equivalent |
|---|---|
| `nvgFillRect` | `SDL_SetRenderDrawColor` + `SDL_RenderFillRect` |
| `nvgStrokeRect` | `SDL_SetRenderDrawColor` + `SDL_RenderRect` |
| `nvgFillRects` | `SDL_RenderFillRects` |
| `nvgStrokeRects` | `SDL_RenderRects` |
| `nvgStrokeLine` | `SDL_RenderLine` |
| `nvgStrokePolyline` | `SDL_RenderLines` |

`NVGcolor` → `SDL_FColor` or `SDL_Color`. The color constants (`NVG_WHITE`,
`NVG_BLACK`, etc.) need to be redefined using SDL types.

#### 1.3.4 — Alpha / Blending

- `nvgGlobalAlpha` → `SDL_SetTextureAlphaModFloat` per-texture, or
  `SDL_SetRenderDrawBlendMode` for primitives.
- Alpha compositing (which nanovg handles internally) → `SDL_SetTextureBlendMode(
  texture, SDL_BLENDMODE_BLEND)`.

#### 1.3.5 — Font Rendering

The game uses **custom bitmap fonts** (loaded as sprites from PNG files), not
nanovg's built-in text rendering. The font rendering in `war_font.c` uses nanovg's
save/scale/translate for positioning and the image batch system for drawing glyphs.

- Replace with SDL3 texture rendering: each glyph is a sub-rect of the font texture,
  rendered via `SDL_RenderTexture` with src/dst rects.
- The `NVGfontParams` struct and associated enums (`NVGwrap`, `NVGtrim`, `NVGalign`)
  should be renamed to game-specific types (e.g., `WarFontParams`).

#### 1.3.6 — Transform Stack

nanovg provides a canvas-like state machine with `nvgSave` / `nvgRestore` /
`nvgScale` / `nvgTranslate` / `nvgTransformPoint`. SDL3's `SDL_Renderer` does **not**
have an equivalent transform stack.

Two approaches:

**Option A (Recommended): SDL Logical Presentation**
Use `SDL_SetRenderLogicalPresentation(renderer, 320, 200,
SDL_LOGICAL_PRESENTATION_INTEGER_SCALE)`. SDL3 then handles the upscaling from the
game's native 320×200 to the window size automatically. This **eliminates most
transform code entirely** and removes the need for a custom transform stack.

**Option B: Manual Transform Stack**
Implement a lightweight 2D transform stack (~50 lines of code) that maintains the
current scale/translate state. All rendering functions apply the current transform to
destination rects before calling SDL. More code, more control.

### 1.4 — Build System Changes

**Files affected:** `nob.c`, build scripts

- Remove GLFW, GLAD, nanovg from include paths and link flags.
- Add SDL3 include path and link against `SDL3` (or `SDL3-static`).
- Remove the GLFW DLL copy step for Windows builds; add SDL3 DLL copy.
- Remove from `deps/include/`: `nanovg/`, `glad/`, `GLFW/`, `KHR/`.
- Remove from `deps/lib/`: `libglfw*`, `glfw3.dll`.
- Add SDL3 libraries to `deps/lib/` (or rely on system-installed SDL3).

### 1.5 — Removed Dependencies Summary

| Dependency | Reason for removal |
|---|---|
| **GLFW** | SDL3 provides window, input, and context management |
| **GLAD** | SDL3 abstracts away OpenGL entirely when using `SDL_Renderer` |
| **nanovg** | SDL3's `SDL_Renderer` provides equivalent 2D rendering |
| **OpenGL ES 2.0 (direct calls)** | Abstracted behind SDL3 |

---

## Part 2: Replace miniaudio with SDL3 Audio

**Files affected:** `war_audio.c`, `war_types.h`, `war1.c`

### 2.1 — Audio Device Initialization

- Replace `ma_device_config_init` / `ma_device_init` / `ma_device_start` with
  `SDL_OpenAudioDevice` + `SDL_AudioSpec`.
- Current setup: mono, 16-bit signed (S16), 44100 Hz, callback-based.
  SDL3 supports this exactly.
- Replace `ma_device sfx` in `WarContext` with `SDL_AudioDeviceID` (and optionally
  `SDL_AudioStream*`).
- Replace `ma_device_uninit` with `SDL_CloseAudioDevice`.

### 2.2 — Audio Callback

The current miniaudio callback signature:

```c
void audioDataCallback(ma_device* sfx, void* output, const void* input, u32 sampleCount)
```

Maps to SDL3's audio stream callback:

```c
void audioCallback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount)
```

SDL3 uses push-based audio streams by default (preferred) or pull-based callbacks.
The existing callback-based approach maps naturally to SDL3 audio streams with a
callback. The internal mixing logic (iterating audio entities, mixing MIDI and WAV)
remains unchanged — only the buffer is written to the SDL3 stream instead of the
miniaudio output pointer.

### 2.3 — Threading (optional improvement)

The current `pthread_mutex_t` usage for thread-safe entity deletion (audio thread vs.
game thread) can optionally be replaced with `SDL_Mutex*` + `SDL_LockMutex` /
`SDL_UnlockMutex`. The existing `mutex.h` already provides a Windows implementation
of pthreads, so this is optional but would simplify cross-platform support.

---

## Part 3: Analysis of TinySoundFont (tsf) + TML

### Conclusion: Keep TinySoundFont and TML as-is

#### Reasoning

1. **Purpose:** TSF is a SoundFont2 synthesizer that converts MIDI events into audio
   samples in real-time. TML is a MIDI file parser. Together they enable the game to
   play the original Warcraft 1 MIDI music using a SoundFont2 instrument bank
   (`GMGeneric.SF2`).

2. **SDL3 has no MIDI synthesizer.** SDL3's audio subsystem provides device management
   and audio streaming, but does NOT include:
   - MIDI file parsing
   - SoundFont2 loading
   - Real-time MIDI synthesis

3. **The usage is deeply integrated.** The `playMidi()` function in `war_audio.c`
   manually steps through MIDI messages and issues TSF commands (`tsf_channel_note_on`,
   `tsf_channel_note_off`, `tsf_channel_set_presetnumber`, `tsf_channel_set_pitchwheel`,
   `tsf_channel_midi_control`) and renders audio blocks with `tsf_render_short`. There
   is no SDL3 equivalent for this pipeline.

4. **Alternatives would be worse:**
   - Pre-rendering MIDI to WAV: Loses per-channel volume control; increases asset size.
   - FluidSynth: Much heavier dependency; TSF is a single-header library.
   - SDL_mixer MIDI support: Relies on external backends (Timidity, FluidSynth) and adds
     significant complexity.

5. **TSF and TML are header-only, zero-dependency libraries.** They produce raw PCM
   samples that can be fed to any audio output system. The synthesized samples from
   `tsf_render_short()` will simply be written into the SDL3 audio stream buffer instead
   of the miniaudio output buffer — no changes needed inside the TSF/TML code.

---

## Recommended Implementation Phases

| Phase | Scope | Files changed |
|---|---|---|
| **1 — SDL3 Audio** | Replace miniaudio | `war_audio.c`, `war_types.h`, `war1.c` |
| **2 — SDL3 Window + Input** | Replace GLFW | `war_game.c`, `war_types.h`, `war1.c` |
| **3 — SDL3 Renderer** | Replace nanovg + GLAD + OpenGL | `war_render.c`, `war_sprites.c`, `war_font.c`, `war_entities.c`, `war_scenes.c`, `war_map.c`, `war_map_ui.c`, `war_cheats_panel.c`, `war_types.h`, `nob.c` |
| **4 — Cleanup** | Remove old deps, update build | `deps/`, `nob.c`, `README.md` |

Doing phases in this order allows each phase to produce a fully working build before
the next phase begins.

---

## Open Questions

1. **SDL3 vendoring:** Should SDL3 be vendored in `deps/` (like current dependencies)
   or linked as a system library? Vendoring gives reproducibility; linking to the system
   library reduces repo size.

2. **SDL_Renderer vs SDL_GPU:** SDL3 includes both `SDL_Renderer` (high-level 2D API)
   and `SDL_GPU` (low-level GPU API). Given the game's 2D nature, `SDL_Renderer` is the
   natural fit — confirm this is the expected approach.

3. **Render scaling approach:**
   - **Option A (Recommended):** Use `SDL_SetRenderLogicalPresentation(renderer, 320,
     200, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE)` — SDL handles upscaling, most
     transform code is eliminated.
   - **Option B:** Manually apply `globalScale` to every render call, as today.

4. **Raspberry Pi target:** The current build supports ARM32 (RPi). SDL3 supports this
   platform. Should the RPi target use SDL3's KMSDRM backend or is X11 sufficient?

5. **Migration style:** Single large migration or incremental phases with each phase
   producing a working build?
