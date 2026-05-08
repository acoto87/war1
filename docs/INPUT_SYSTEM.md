# Input Subsystem Documentation

## Overview

The input subsystem is the engine's frame-synchronous translation layer between raw SDL3 events and the rest of the game runtime. Its job is to normalize mouse and keyboard input into a compact `WarInput` snapshot stored directly inside `WarContext`, then expose that snapshot to higher-level systems such as scenes, UI, map selection, camera scrolling, and commands.

In the main loop, the input pipeline is:

```c
wg_beginInputFrame(context);

while (SDL_PollEvent(&event))
{
    wg_processGameEvent(context, &event);
}

wg_updateGame(context);
wg_renderGame(context);
wg_presentGame(context);
```

That ordering matters:

- `wg_beginInputFrame()` clears one-frame edge flags.
- `wg_processGameEvent()` folds all SDL events for the frame into `context->input`.
- Scene, UI, and map code consume `held`, `justPressed`, and `justReleased` during `wg_updateGame()`.

This subsystem sits early in the engine pipeline, before gameplay update and rendering, and provides the canonical input state used everywhere else.

## Memory & State Management

### Ownership Model

`WarInput` is embedded directly in `WarContext`:

```c
struct _WarContext
{
    // ...
    WarInput input;
    // ...
};
```

There is no standalone input manager allocation, no per-frame heap churn, and no teardown routine for input-specific memory. The input state has the same lifetime as the owning `WarContext`, which is allocated once during startup and destroyed at shutdown.

### Fixed-Size Storage

The subsystem uses compile-time-sized arrays:

- `buttons[WAR_MOUSE_COUNT]`
- `keys[WAR_KEY_COUNT]`

This keeps input state contiguous and predictable in memory. Runtime growth, hash lookups, and pointer chasing are avoided in the hot path.

### Frame Model

Each key and mouse button stores three bits of temporal state in `WarInputState`:

- `held`: current stable state
- `justPressed`: edge that became true this frame only
- `justReleased`: edge that became true this frame only

`wg_beginInputFrame()` clears only the transient edge flags. The `held` bits persist until an actual release event or a focus-loss reset arrives.

### Cache Locality

The input snapshot is intentionally small:

- mouse position (`vec2`)
- button states in a dense array
- key states in a dense array
- a few gesture/UI-capture fields (`capturedUIButtonId`, `mapDragActive`, `mapDragStartPos`, `mapDragRect`)

No explicit packing pragmas or custom alignment attributes are used. The performance strategy is instead to keep the data plain, contiguous, and resident inside the already-hot `WarContext`.

### Logical Coordinate Normalization

The renderer uses SDL logical presentation at `320x200`. SDL3 does not automatically rewrite incoming event coordinates to that logical space, so `wg_processGameEvent()` explicitly calls:

```c
SDL_ConvertEventToRenderCoordinates(context->renderer, event);
```

As a result, every consumer reads `input->pos` in the same coordinate system used by UI layout, map panels, and cursor rendering.

### Reset-on-Focus-Loss Behavior

When the window loses focus, is minimized, or becomes hidden, the subsystem clears:

- every `held` / `justPressed` / `justReleased` bit
- `capturedUIButtonId`
- `mapDragActive`
- `mapDragStartPos`
- `mapDragRect`

This prevents sticky keys, stuck mouse buttons, and stale drag gestures after alt-tab or minimize/restore cycles.

## Core API / Functions

## Public Functions

### `void wg_setInputButton(WarContext* context, s32 button, bool pressed)`

**Defined in:** `src/war_game.c`  
**Declared in:** `src/war_game.h`

```c
void wg_setInputButton(WarContext* context, s32 button, bool pressed);
```

**Purpose**

Updates one logical mouse button slot in `context->input.buttons`.

**Inputs**

- `context`: owning game context
- `button`: expected to be a valid `WarMouseButtons` index such as `WAR_MOUSE_LEFT` or `WAR_MOUSE_RIGHT`
- `pressed`: new stable state for that button

**Outputs**

- No return value

**Side Effects**

- Mutates `context->input.buttons[button]`
- Sets `justPressed` only on a `false -> true` transition
- Sets `justReleased` only on a `true -> false` transition

**Performance Notes**

- O(1)
- No bounds checking is performed; callers must pass a valid enum index
- Repeated press events while already held do not retrigger `justPressed`

### `void wg_setInputKey(WarContext* context, s32 key, bool pressed)`

**Defined in:** `src/war_game.c`  
**Declared in:** `src/war_game.h`

```c
void wg_setInputKey(WarContext* context, s32 key, bool pressed);
```

**Purpose**

Updates one logical keyboard slot in `context->input.keys`.

**Inputs**

- `context`: owning game context
- `key`: expected to be a valid `WarKeys` index
- `pressed`: new stable state for that key

**Outputs**

- No return value

**Side Effects**

- Mutates `context->input.keys[key]`
- Emits one-frame `justPressed` / `justReleased` edges based on state transitions

**Performance Notes**

- O(1)
- No bounds checking is performed
- SDL key repeat does not create repeated `justPressed` edges because the transition test is state-based

### `void wg_beginInputFrame(WarContext* context)`

**Defined in:** `src/war_game.c`  
**Declared in:** `src/war_game.h`

```c
void wg_beginInputFrame(WarContext* context);
```

**Purpose**

Begins a new frame of input processing by clearing the transient edge flags on all keys and mouse buttons.

**Inputs**

- `context`: owning game context

**Outputs**

- No return value

**Side Effects**

- Sets every `buttons[i].justPressed` and `buttons[i].justReleased` to `false`
- Sets every `keys[i].justPressed` and `keys[i].justReleased` to `false`
- Leaves all `held` bits unchanged

**Performance Notes**

- O(`WAR_MOUSE_COUNT + WAR_KEY_COUNT`)
- In practice, constant-time and cheap enough to run every frame
- Must run before SDL event polling if downstream systems rely on one-frame edge semantics

### `void wg_processGameEvent(WarContext* context, SDL_Event* event)`

**Defined in:** `src/war_game.c`  
**Declared in:** `src/war_game.h`

```c
void wg_processGameEvent(WarContext* context, SDL_Event* event);
```

**Purpose**

Consumes one SDL event and folds it into the engine's logical input state.

**Inputs**

- `context`: owning game context
- `event`: mutable SDL event received from `SDL_PollEvent()`

**Outputs**

- No return value

**Side Effects**

- Rewrites `event` coordinates into logical render space with `SDL_ConvertEventToRenderCoordinates()`
- Updates `input->pos` on mouse motion and mouse button events
- Updates logical mouse button state via `wg_setInputButton()`
- Updates logical key state via `wg_setInputKey()`
- Appends text input to the active cheat console when cheat text entry is enabled and visible
- Grabs or releases the mouse when window focus changes
- Clears all held/edge state and drag/UI-capture state on focus loss, minimize, or hide

**Performance Notes**

- O(1) for most event types
- `SDL_EVENT_TEXT_INPUT` is O(text length)
- Safe to call once per SDL event in the outer loop
- This is not an inner-loop gameplay function; it belongs in the event pump only

**Behavior Details**

- Left and right mouse buttons are mapped to `WAR_MOUSE_LEFT` and `WAR_MOUSE_RIGHT`
- SDL left/right Shift, Ctrl, and Alt are collapsed into one logical slot each: `WAR_KEY_SHIFT`, `WAR_KEY_CTRL`, `WAR_KEY_ALT`
- Modifier keys are derived from `event->key.mod`, which avoids dropping the logical modifier state when one physical side is released while the other is still held

## Query Macros

These macros are the main read-side API used by gameplay code:

```c
#define isButtonHeld(input, btn) ((input)->buttons[btn].held)
#define isButtonJustPressed(input, btn) ((input)->buttons[btn].justPressed)
#define isButtonJustReleased(input, btn) ((input)->buttons[btn].justReleased)

#define isKeyHeld(input, key) ((input)->keys[key].held)
#define isKeyJustPressed(input, key) ((input)->keys[key].justPressed)
#define isKeyJustReleased(input, key) ((input)->keys[key].justReleased)

#define isMapDragging(input) ((input)->mapDragActive)
```

**Purpose**

Provide branch-free, inline access to the current input snapshot.

**Inputs / Outputs**

- Input: pointer to `WarInput` plus a valid enum index
- Output: boolean expression result

**Side Effects**

- None

**Performance Notes**

- Compile to direct field access
- Intended for hot call sites in UI, scene, and map update code
- No bounds checking

## Internal Helpers Relevant to Behavior

These are not declared in `war_game.h`, but they define important subsystem behavior.

### `static WarKeys wg_getWarKeyFromSDLKey(SDL_Keycode key)`

Maps SDL keycodes to the engine's compact `WarKeys` enum. Unsupported keys return `WAR_KEY_NONE` and are ignored by the rest of the subsystem.

### `static void wg_appendCheatTextInput(WarContext* context, StringView text)`

Routes SDL text input into the cheat-entry text buffer owned by the active scene or map. Only printable ASCII codepoints (`32..126`) are accepted, and only while cheat entry is both enabled and visible.

This helper is the only place where the input path performs string manipulation. The core `WarInput` snapshot itself remains allocation-free.

## Data Structures

### `WarInputState`

```c
struct _WarInputState
{
    bool held;
    bool justPressed;
    bool justReleased;
};
```

**Purpose**

Represents one logical button/key slot with both stable and edge-triggered state.

**Field Notes**

- `held`: true while the logical input remains down
- `justPressed`: true for one frame on the press edge
- `justReleased`: true for one frame on the release edge

**Layout Notes**

- Plain old data, contiguous in arrays
- No explicit bit-packing; the implementation favors simplicity and direct access over manual compression

### `WarInput`

```c
struct _WarInput
{
    vec2 pos;
    WarInputState buttons[WAR_MOUSE_COUNT];
    WarInputState keys[WAR_KEY_COUNT];
    WarEntityId capturedUIButtonId;
    bool mapDragActive;
    vec2 mapDragStartPos;
    rect mapDragRect;
};
```

**Purpose**

Stores the entire input snapshot for the current frame plus a small amount of cross-system gesture/UI ownership state.

**Field Notes**

- `pos`: latest mouse position in logical render coordinates
- `buttons`: logical mouse button state array
- `keys`: logical keyboard state array
- `capturedUIButtonId`: UI button currently owning the active mouse press
- `mapDragActive`: true while a map-panel drag selection is in progress
- `mapDragStartPos`: drag origin in screen/logical coordinates
- `mapDragRect`: current drag rectangle in screen/logical coordinates

**Design Notes**

- `capturedUIButtonId` lets the UI layer preserve click ownership across hover changes
- `mapDrag*` state lives in input rather than map-local temporary variables so drag gestures remain part of the same frame snapshot that all systems read

### `WarMouseButtons`

```c
typedef enum _WarMouseButtons
{
    WAR_MOUSE_LEFT,
    WAR_MOUSE_RIGHT,

    WAR_MOUSE_COUNT
} WarMouseButtons;
```

**Purpose**

Defines the compact index space for `WarInput.buttons`.

### `WarKeys`

```c
typedef enum _WarKeys
{
    WAR_KEY_NONE,
    // printable keys
    // navigation keys
    // function keys
    WAR_KEY_SHIFT,
    WAR_KEY_CTRL,
    WAR_KEY_ALT,

    WAR_KEY_COUNT
} WarKeys;
```

**Purpose**

Defines the compact index space for `WarInput.keys`.

**Design Notes**

- Physical left/right modifier keys are merged into one logical slot per modifier
- The enum acts as a translation boundary: gameplay code never needs SDL keycodes directly

## Engine Integration Notes

The input subsystem is intentionally small, but it carries state used by multiple downstream systems.

### UI Integration

`wui_updateUIButtons()` uses:

- `input->pos` for hit testing
- `isButtonJustPressed()` to capture a button
- `isButtonHeld()` to maintain pressed visuals
- `isButtonJustReleased()` to fire click handlers
- `capturedUIButtonId` to keep ownership stable during a click-drag-release sequence

### Map Integration

`war_map.c` uses input for:

- edge scrolling from mouse position
- keyboard scrolling from arrow keys
- left-drag selection rectangles
- right-click command issuing
- modifier-aware selection extension using Ctrl

The map layer also updates `mapDragActive`, `mapDragStartPos`, and `mapDragRect` as part of drag selection.

### Scene Integration

Scene code uses frame-edge queries for discrete actions such as:

- `Enter` to continue from briefing/download screens
- `Space` or mouse click to advance
- `Esc`, `Tab`, arrow keys, and editing keys in map UI flows

Because `justPressed` and `justReleased` are cleared once per frame, scene logic can safely treat those queries as one-shot triggers.

## Usage Example

```c
WarContext* context = wm_alloc(sizeof(WarContext));
bool running = wg_initGame(context);

while (running)
{
    wg_beginInputFrame(context);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        wg_processGameEvent(context, &event);

        if (event.type == SDL_EVENT_QUIT)
        {
            running = false;
        }
    }

    WarInput* input = &context->input;

    if (isKeyHeld(input, WAR_KEY_CTRL) &&
        isKeyJustReleased(input, WAR_KEY_P))
    {
        context->paused = !context->paused;
    }

    wg_updateGame(context);
    wg_renderGame(context);
    wg_presentGame(context);
}

wg_quitGame(context);
```

Minimal consumer-side usage inside a gameplay module:

```c
WarInput* input = &context->input;

if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
{
    // begin selection or activate UI
}

if (isButtonJustReleased(input, WAR_MOUSE_RIGHT))
{
    // finish a right-click action
}

if (isKeyHeld(input, WAR_KEY_LEFT))
{
    // scroll map left while held
}
```

## Dependencies

### External Dependencies

- `SDL3/SDL_events.h`
- `SDL3/SDL.h`
- `math.h` for `floorf()`
- `assert.h`
- `shl/wstr.h` for cheat text insertion

### Internal Headers

- `src/war_game.h`
- `src/war.h`
- `src/war_fwd.h`
- `src/war_enums.h`
- `src/common.h`
- `src/war_math.h`

### Internal Engine Modules Touched by the Input Path

- `src/war1.c` for main-loop integration
- `src/war_ui.c` for UI cursor and button handling
- `src/war_map.c` for camera movement, selection drag, and commands
- `src/war_scene_briefing.c` and `src/war_scene_download.c` for scene-level key/button triggers

## Performance Summary

- No dynamic allocation in the core `WarInput` snapshot path
- Constant-time state updates per input event
- Constant-time read access through macros
- One small linear clear per frame over fixed-size arrays
- Logical-coordinate conversion is performed once per SDL event, which keeps all downstream input consumers free of per-call coordinate remapping

This is a good fit for the rest of the engine's DOD-oriented architecture: small state, predictable memory access, and explicit frame boundaries.
