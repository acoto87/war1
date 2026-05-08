# War1-C UI Subsystem Documentation

## Overview

The UI subsystem (`war_ui.c`/`war_ui.h`) is a component-based entity system that manages all user interface elements in the War1-C game engine. It handles the creation, lifecycle, and rendering of UI entities such as text labels, buttons, images, rectangles, and cursors.

The subsystem is designed with **Data-Oriented Design (DOD)** principles, leveraging a component-based architecture where UI elements are represented as entities with attached components (`WarUIComponent`, `WarTextComponent`, `WarButtonComponent`, etc.). This approach maximizes cache locality and enables efficient batch processing.

The `war_map_ui.c` module builds on the core UI system to create domain-specific UI layouts for the game map interface, including resource displays, unit status bars, minimap, and command panel. The `war_map_menu.c` module provides menu construction and event handling for in-game menus (pause, options, game-over, quit confirmation).

### Position in Engine Pipeline

```
WarContext (main game state)
    ├── UI Subsystem (wui_*)
    │   ├── UI Entities (Text, Button, Image, Rect, Cursor)
    │   ├── Input Handling (Button clicks, hot-keys)
    │   └── Rendering (wui_renderUIEntities)
    │
    ├── Map UI Layer (wmui_*)
    │   ├── Map-specific UI (panels, minimap, HUD)
    │   └── Unit/Resource displays
    │
    └── Map Menu Layer (wmm_*)
        └── Interactive menus (pause, options, game-over)
```

---

## Memory & State Management

### Fixed-Size, Pre-Allocated Design

The UI subsystem uses the **entity pool** from `war_entities.c`, which manages a fixed array of `WarEntity` objects (up to `MAX_ENTITIES_COUNT = 100`). UI entities are allocated from this same pool, avoiding dynamic allocation at runtime.

**Key Characteristics:**

1. **Unity Build with Single Allocation Context**: All entities (including UI) share a single memory arena managed by `war_entities.c`. When the game initializes, the entity pool is pre-allocated once and reused throughout the game lifetime.

2. **Component Packing**: Each `WarEntity` embeds all possible components (transform, sprite, text, button, UI, etc.) as value types. This ensures cache-friendly layout—accessing related component data for a single entity keeps memory access localized.

   ```c
   struct _WarEntity
   {
       bool enabled;
       WarEntityId id;
       WarEntityType type;
       WarTransformComponent transform;      // 1 byte + 8 bytes (vec2) + 8 bytes = 17 bytes
       WarSpriteComponent sprite;
       // ... (other components)
       WarUIComponent ui;                     // 1 byte + String (pointer + len)
       WarTextComponent text;                 // Multiple fields for text rendering
       WarRectComponent rect;                 // vec2 size + WarColor
       WarButtonComponent button;             // State, sprites, handlers
       WarCursorComponent cursor;
       // ... (other components)
   };
   ```

3. **Component Enabled Flags**: Each component has an `enabled` boolean flag. Disabled components are skipped during update and render passes, avoiding unnecessary processing.

4. **Named Lookup via Entity ID Map**: UI entities are indexed by name (via `we_findUIEntity`), which internally uses a hash map (`WarEntityIdMap`) for O(1) lookup. This enables efficient retrieval of UI elements by string identifier without scanning the entire entity list.

5. **String Management**: UI text and button tooltips use the `wstr` (wide string) type from `shl/wstr.h`, which manages dynamic string data separately from the entity component. This avoids embedding variable-length data in the fixed entity structure.

---

## Core API / Functions

### Entity Creation Functions

#### `WarEntity* wui_createUIText(...)`
- **Signature**: `WarEntity* wui_createUIText(WarContext* context, String name, s32 fontIndex, f32 fontSize, String text, vec2 position)`
- **Purpose**: Creates a text UI entity with the specified font, size, and initial text.
- **Side Effects**: Adds a new entity to the entity pool; allocates/registers the entity name in the UI name map.
- **Performance**: O(1) if entity pool has space; O(log n) for name registration in the map.
- **Parameters**:
  - `context`: Game context holding entity pool and UI state.
  - `name`: Unique identifier for the UI entity (used for lookup).
  - `fontIndex`: Index into the font resource array (0, 1, etc.).
  - `fontSize`: Font size in pixels.
  - `text`: Initial text string (can be empty).
  - `position`: Screen position in pixels.

#### `WarEntity* wui_createUIRect(...)`
- **Signature**: `WarEntity* wui_createUIRect(WarContext* context, String name, vec2 position, vec2 size, WarColor color)`
- **Purpose**: Creates a solid-color rectangle UI element.
- **Side Effects**: Similar to `wui_createUIText`; allocates entity and registers name.
- **Performance**: O(1) entity creation + O(log n) name registration.

#### `WarEntity* wui_createUIImage(...)`
- **Signature**: `WarEntity* wui_createUIImage(WarContext* context, String name, WarSpriteResourceRef spriteResourceRef, vec2 position)`
- **Purpose**: Creates an image UI element from a sprite resource.
- **Side Effects**: Allocates entity; the sprite resource must already be loaded in the resource manager.
- **Performance**: O(1) creation; sprite lookup is O(1).

#### `WarEntity* wui_createUITextButton(...)`
- **Signature**: `WarEntity* wui_createUITextButton(WarContext* context, String name, s32 fontIndex, f32 fontSize, String text, WarSpriteResourceRef backgroundNormalRef, WarSpriteResourceRef backgroundPressedRef, WarSpriteResourceRef foregroundRef, vec2 position)`
- **Purpose**: Creates a button with text overlay, with separate sprites for normal and pressed states.
- **Side Effects**: Allocates entity and attaches text and button components.
- **Performance**: O(1) creation; sprite resource lookups are O(1).
- **Note**: Text alignment is automatically set to centered (horizontal and vertical).

#### `WarEntity* wui_createUIImageButton(...)`
- **Signature**: `WarEntity* wui_createUIImageButton(WarContext* context, String name, WarSpriteResourceRef backgroundNormalRef, WarSpriteResourceRef backgroundPressedRef, WarSpriteResourceRef foregroundRef, vec2 position)`
- **Purpose**: Creates a button with image foreground (no text).
- **Side Effects**: Similar to `wui_createUITextButton` but without text component setup.

#### `WarEntity* wui_createUICursor(...)`
- **Signature**: `WarEntity* wui_createUICursor(WarContext* context, String name, WarCursorType type, vec2 position)`
- **Purpose**: Creates a cursor entity with specified type (arrow, target, scroll, etc.).
- **Side Effects**: Loads cursor resource (hotspot coordinates) and initializes cursor component.
- **Performance**: O(1) creation; resource lookup and hotspot registration are O(1).

---

### Entity Modification Functions

#### `void wui_setUIText(WarEntity* uiText, String text)`
- **Purpose**: Updates the text content of a text UI entity.
- **Side Effects**: Frees the old string and assigns the new one.
- **Performance**: O(1); delegates to string management system.

#### `void wui_clearUIText(WarEntity* uiText)`
- **Purpose**: Clears (empties) the text of a UI text entity.
- **Side Effects**: Frees the string and marks the text component as disabled.

#### `void wui_setUIImage(WarEntity* uiImage, s32 frameIndex)`
- **Purpose**: Updates the sprite frame of an image UI entity.
- **Side Effects**: Updates `sprite.frameIndex` and enables/disables the sprite component based on frame validity.
- **Performance**: O(1).

#### `void wui_setUIRectWidth(WarEntity* uiRect, s32 width)`
- **Purpose**: Updates the width of a rectangle UI entity.
- **Side Effects**: Sets `rect.size.x` and enables/disables the rect component.

#### `void wui_setUITooltip(WarEntity* uiButton, s32 highlightIndex, s32 highlightCount, String text)`
- **Purpose**: Attaches a tooltip to a button (displayed on hover).
- **Side Effects**: Stores tooltip text and highlight indices in the button component.
- **Performance**: O(1).

#### `void wui_clearUITooltip(WarEntity* uiButton)`
- **Purpose**: Removes the tooltip from a button.
- **Side Effects**: Frees tooltip string.

---

### State Query & Lookup Functions

#### `bool wui_isUIEntity(WarEntity* entity)`
- **Purpose**: Checks if an entity is a UI entity (of types TEXT, IMAGE, BUTTON, RECT, CURSOR, MINIMAP).
- **Side Effects**: None (read-only).
- **Performance**: O(1).

#### `WarEntity* we_findUIEntity(WarContext* context, StringView name)`
- **Purpose**: Finds a UI entity by name.
- **Side Effects**: None.
- **Performance**: O(1) average (hash map lookup).
- **Note**: Returns `NULL` if entity not found. Called internally by `*ByName` functions.

---

### Lookup & Modification by Name

These functions combine entity lookup with modification:

#### `void wui_setUIButtonStatusByName(WarContext* context, StringView name, bool enabled)`
#### `void wui_setUIButtonInteractiveByName(WarContext* context, StringView name, bool interactive)`
#### `void wui_setUIButtonHotKeyByName(WarContext* context, StringView name, WarKeys key)`
#### `void wui_setUIEntityStatusByName(WarContext* context, StringView name, bool enabled)`

- **Purpose**: Modify UI entity properties by name without requiring a direct entity pointer.
- **Side Effects**: Updates entity component state.
- **Performance**: O(1) average (hash map lookup + field update).
- **Safety**: Silently ignore if entity not found.

---

### Input & Update Functions

#### `void wui_updateUICursor(WarContext* context)`
- **Purpose**: Updates the cursor position to follow the mouse input.
- **Side Effects**: Modifies the transform component of the cursor entity.
- **Performance**: O(1); reads input state and updates one entity.
- **Notes**:
  - Called once per frame after input polling.
  - Always uses `WAR_CURSOR_ARROW` (other cursor types set explicitly via `wui_changeCursorType`).
  - Adjusts cursor position by the cursor's hotspot to ensure the visual cursor aligns with the click point.

#### `void wui_updateUIButtons(WarContext* context, bool hotKeysEnabled)`
- **Purpose**: Updates button state (hot, active) based on mouse input and keyboard hot-keys.
- **Side Effects**: Updates button component state (`.hot`, `.active`); may invoke button click handlers.
- **Performance**: O(n) where n = number of UI buttons; typically small (~10-20 buttons).
- **Behavior**:
  - Scans all button entities for ones that are enabled, have interactive flag set.
  - Checks if mouse is over button bounds (using transform position and sprite dimensions).
  - Calls `.clickHandler` when button is clicked (mouse down followed by release).
  - Supports keyboard hot-keys; if `hotKeysEnabled == true` and a button's hot-key is pressed, invoke its handler.
  - Ensures button actions only fire once per click (tracks `.active` state to distinguish press and release).
  - Only one button can be "hot" (hovered) at a time; toggles all others to non-hot.
- **Thread Safety**: Assumes input state is finalized before calling; no thread coordination needed.

#### `void wui_changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type)`
- **Purpose**: Changes the cursor sprite and hotspot to a new cursor type.
- **Side Effects**: Removes and re-adds cursor component with new type and hotspot.
- **Performance**: O(1); updates cursor component.
- **Example Usage**: In game input handling, change cursor to `WAR_CURSOR_TARGET` when initiating a targeting action, then revert to `WAR_CURSOR_ARROW` when cancelled.

---

### Rendering

#### `void wui_renderUIEntities(WarContext* context)`
- **Purpose**: Renders all enabled UI entities to the screen.
- **Side Effects**: Calls SDL3 draw functions; updates framebuffer.
- **Performance**: O(n) where n = number of UI entities; typically 30-50 visible UI elements.
- **Notes**:
  - Iterates over all UI entities and delegates rendering to `we_renderEntity`.
  - Rendering order depends on the order of entities in the list (back-to-front; entities added later render on top).
  - UI rendering is typically done after map and game objects, so UI appears on top.

---

## Data Structures

### `WarUIComponent`
```c
struct _WarUIComponent
{
    bool enabled;
    String name;
};
```
- **Purpose**: Marks an entity as a UI element and provides a unique identifier.
- **Fields**:
  - `enabled`: Whether this UI element is visible/interactive.
  - `name`: String identifier for lookup and debugging.
- **Size**: ~17 bytes (1 byte enabled + 16 bytes String (pointer + length)).

### `WarTextComponent`
```c
struct _WarTextComponent
{
    bool enabled;
    String text;
    s32 fontIndex;
    f32 fontSize;
    f32 lineHeight;
    WarColor fontColor;
    WarColor highlightColor;
    s32 highlightIndex;
    s32 highlightCount;
    vec2 boundings;
    WarTextAlignment horizontalAlign;
    WarTextAlignment verticalAlign;
    WarTextAlignment lineAlign;
    WarTextWrapping wrapping;
    WarTextTrimming trimming;
    bool multiline;
};
```
- **Purpose**: Stores text rendering state for text UI entities.
- **Key Fields**:
  - `text`: The string content (managed by `wstr`).
  - `fontIndex`: Which font (0, 1, etc.) to use from the resource table.
  - `fontSize`: Point size for rendering.
  - `fontColor`, `highlightColor`: Text and highlight colors (used for shortcut key highlighting).
  - `highlightIndex`, `highlightCount`: Character range to highlight (e.g., hotkey letters).
  - `boundings`: Max size for text layout (for centering/alignment).
  - `*Align`: Horizontal, vertical, and line alignment.
  - `wrapping`, `trimming`: Text wrapping and overflow behavior.
  - `multiline`: Whether text can span multiple lines.
- **Size**: ~80+ bytes (including String and color fields).

### `WarRectComponent`
```c
struct _WarRectComponent
{
    bool enabled;
    vec2 size;
    WarColor color;
};
```
- **Purpose**: Represents a solid-color rectangle UI element.
- **Fields**:
  - `size`: Width and height in pixels.
  - `color`: RGBA color.

### `WarButtonComponent`
```c
struct _WarButtonComponent
{
    bool enabled;
    bool interactive;
    bool hot;
    bool active;
    WarKeys hotKey;
    s32 highlightIndex;
    s32 highlightCount;
    String tooltip;
    s32 gold;
    s32 wood;
    WarSprite normalSprite;
    WarSprite pressedSprite;
    WarClickHandler clickHandler;
};
```
- **Purpose**: Represents an interactive button with state and event handling.
- **Fields**:
  - `enabled`, `interactive`: Button is present and can be interacted with.
  - `hot`, `active`: Mouse is over button; button is pressed.
  - `hotKey`: Keyboard shortcut (e.g., `WAR_KEY_ESCAPE`).
  - `tooltip`: Help text displayed on hover.
  - `gold`, `wood`: Resource cost (displayed in tooltip).
  - `normalSprite`, `pressedSprite`: Sprite frames for visual states.
  - `clickHandler`: Function pointer called when button is clicked; signature: `void (*)(WarContext*, WarEntity*)`.
- **Size**: ~100+ bytes (includes two WarSprite structs and String).

### `WarCursorComponent`
```c
struct _WarCursorComponent
{
    bool enabled;
    WarCursorType type;
    vec2 hot;
};
```
- **Purpose**: Represents the game cursor.
- **Fields**:
  - `type`: Cursor appearance (arrow, target, scroll, etc.).
  - `hot`: Hotspot offset from cursor position (where the click registers).

### `WarTransformComponent`
```c
struct _WarTransformComponent
{
    bool enabled;
    vec2 position;
    vec2 rotation;
    vec2 scale;
};
```
- **Purpose**: Spatial transform for all entities (UI and game objects).
- **Fields**:
  - `position`: Screen position (pixels for UI; world units for game objects).
  - `rotation`: Rotation vector (typically zero for UI).
  - `scale`: Scale factors (typically 1.0 for UI).

### Alignment Enums

```c
typedef enum
{
    WAR_TEXT_ALIGN_LEFT,
    WAR_TEXT_ALIGN_CENTER,
    WAR_TEXT_ALIGN_RIGHT,
    WAR_TEXT_ALIGN_JUSTIFY,
    WAR_TEXT_ALIGN_TOP,
    WAR_TEXT_ALIGN_MIDDLE,
    WAR_TEXT_ALIGN_BOTTOM,
} WarTextAlignment;

typedef enum
{
    WAR_TEXT_WRAPPING_NO_WRAP,
    WAR_TEXT_WRAPPING_WORD_WRAP,
    WAR_TEXT_WRAPPING_CHARACTER_WRAP,
} WarTextWrapping;

typedef enum
{
    WAR_TEXT_TRIMMING_NONE,
    WAR_TEXT_TRIMMING_CHARACTER_ELLIPSIS,
    WAR_TEXT_TRIMMING_WORD_ELLIPSIS,
} WarTextTrimming;
```

---

## Map UI Layer (`war_map_ui.c`)

The Map UI layer builds on the core UI system to create in-game HUD elements specific to the active map.

### Key Functions

#### `void wmui_createMapUI(WarContext* context)`
- **Purpose**: Initializes all HUD UI elements for the map.
- **Side Effects**: Creates ~50+ UI entities for panels, text, buttons, and bars.
- **Performance**: O(1) at startup (amortized over initialization).
- **Creates**:
  - **Panels**: Images for top, bottom, left, right, and corner UI panels.
  - **Resource Display**: Gold and lumber text labels with icons.
  - **Unit Info**: Portraits, name, life/mana bars for selected unit(s).
  - **Command Panel**: Button grid for issuing unit commands.
  - **Minimap**: Interactive map overview.
  - **Status Bar**: Dynamic text for game messages and tooltips.

#### `WarEntity* wmui_createUIMinimap(WarContext* context, String name, vec2 position)`
- **Purpose**: Creates a minimap UI entity.
- **Returns**: Entity with MINIMAP type; can be rendered specially.

#### `void wmui_updateGoldText(WarContext* context)` / `void wmui_updateWoodText(WarContext* context)`
- **Purpose**: Updates the displayed resource counts.
- **Called**: Every frame or when resources change.
- **Performance**: O(1); format and update one text entity.

#### `void wmui_updateSelectedUnitsInfo(WarContext* context)`
- **Purpose**: Updates unit info display (portraits, stats, name) based on selected unit(s).
- **Called**: Every frame or when selection changes.
- **Performance**: O(1); updates fixed number of UI elements.

#### `void wmui_setStatus(WarContext* context, s32 highlightIndex, s32 highlightCount, s32 gold, s32 wood, String text)`
- **Purpose**: Displays a status message with optional resource info and text highlighting.
- **Example**: "Build Barracks (Gold: 100, Lumber: 50)" — highlights resource keywords.

#### `void wmui_setFlashStatus(WarContext* context, f32 duration, String text)`
- **Purpose**: Displays a temporary status message that fades out after `duration` seconds.

#### `void wmui_setLifeBar(...)`/`wmui_setManaBar(...)`/`wmui_setPercentBar(...)`
- **Purpose**: Updates width of health/mana/progress bars based on unit stats.
- **Performance**: O(1); updates rect width.

#### `void wmui_renderMapUI(WarContext* context)`/`wmui_renderSelectionRect(...)`/`wmui_renderCommand(...)`
- **Purpose**: Render custom map UI layers (selection rectangle, command visuals).

---

## Map Menu Layer (`war_map_menu.c`)

The Map Menu layer provides UI construction and event handling for in-game menus.

### Key Functions

#### `void wmm_createMenu(WarContext* context)`
- **Purpose**: Creates the main pause/in-game menu UI (Save, Options, Objectives, Restart, Quit buttons).
- **Creates**: ~10-15 UI entities; buttons are initially hidden.

#### `void wmm_createOptionsMenu(WarContext* context)`
- **Purpose**: Creates the options menu (volume sliders, game speed, scroll speed settings).

#### `void wmm_createGameOverMenu(...)` / `wmm_createQuitMenu(...)`
- **Purpose**: Create specialized menus for end-game scenarios.

#### `void wmm_showOrHideMenu(WarContext* context, bool status)`
- **Purpose**: Show/hide menu UI entities by toggling their `.enabled` flag.

#### Button Handlers (e.g., `wmm_handleMenu`, `wmm_handleOptions`, `wmm_handleQuit`)
- **Purpose**: Callbacks invoked when menu buttons are clicked.
- **Signature**: `void handler(WarContext* context, WarEntity* entity)`.
- **Responsibilities**: Update game state, navigate between menus, adjust settings.

---

## Usage Example

### Initialization (Pseudo-code)

```c
// In wg_initializeGame()
void game_init(WarContext* context)
{
    // 1. Initialize core UI subsystem (entity pool already created)
    // (implicit, handled by entity module)

    // 2. Create map-specific UI
    wmui_createMapUI(context);

    // 3. Create menus
    wmm_createMenu(context);
    wmm_createOptionsMenu(context);
    // ...

    // 4. Hide menus initially
    wmm_showOrHideMenu(context, false);
}
```

### Main Game Loop (Pseudo-code)

```c
void game_loop(WarContext* context)
{
    while (context->running)
    {
        // Process input
        WarInput* input = poll_input();
        context->input = *input;

        // Update UI
        bool hotKeysEnabled = !menu_is_open(context);
        wui_updateUICursor(context);
        wui_updateUIButtons(context, hotKeysEnabled);

        // Update map UI (resource display, unit info)
        wmui_updateGoldText(context);
        wmui_updateWoodText(context);
        wmui_updateSelectedUnitsInfo(context);

        // Render game world
        render_map(context);
        render_game_entities(context);

        // Render UI on top
        wui_renderUIEntities(context);
        wmui_renderMapUI(context);

        // Present framebuffer
        SDL_RenderPresent(context->renderer);
    }
}
```

### Creating Custom UI (Pseudo-code)

```c
// Create a simple dialog
void show_dialog(WarContext* context)
{
    // Create background panel
    WarEntity* panel = wui_createUIRect(
        context,
        wstr_fromCString("dialogPanel"),
        vec2i(100, 100),
        vec2i(200, 150),
        WAR_COLOR_RGBA(50, 50, 50, 200)
    );

    // Create title text
    WarEntity* title = wui_createUIText(
        context,
        wstr_fromCString("dialogTitle"),
        1,      // font index
        10,     // size
        wstr_fromCString("Confirm Action"),
        vec2i(110, 110)
    );
    setUITextColor(title, WAR_COLOR_WHITE);

    // Create OK button
    WarEntity* okBtn = wui_createUITextButton(
        context,
        wstr_fromCString("dialogOkBtn"),
        0, 6,
        wstr_fromCString("OK"),
        imageResourceRef(239),  // normal
        imageResourceRef(240),  // pressed
        invalidResourceRef(),   // foreground
        vec2i(130, 220)
    );
    setUIButtonClickHandler(okBtn, on_dialog_ok);

    // Create Cancel button
    WarEntity* cancelBtn = wui_createUITextButton(
        context,
        wstr_fromCString("dialogCancelBtn"),
        0, 6,
        wstr_fromCString("Cancel"),
        imageResourceRef(239),
        imageResourceRef(240),
        invalidResourceRef(),
        vec2i(230, 220)
    );
    setUIButtonClickHandler(cancelBtn, on_dialog_cancel);
}

void on_dialog_ok(WarContext* context, WarEntity* entity)
{
    // Handle OK action
    // Find and hide dialog entities
    wui_setUIEntityStatusByName(context, wsv_fromCString("dialogPanel"), false);
    wui_setUIEntityStatusByName(context, wsv_fromCString("dialogTitle"), false);
    // ...
}
```

---

## Dependencies

### External Dependencies
- **SDL3**: Rendering backend (SDL.h for frame presentation).
- **shl/wstr.h**: Wide string management for dynamic text storage.
- **shl/set.h**: Hash sets for tracking button update state.
- **shl/map.h**: Hash maps for entity name lookup.

### Internal Dependencies
- **war_entities.c/.h**: Entity pool management and component storage.
- **war_resources.c/.h**: Sprite and cursor resource loading.
- **war_audio.c/.h**: UI click sound effects.
- **war_render.c/.h**: Rendering primitives (sprites, text, rects).
- **war_font.c/.h**: Font management and glyph rendering.
- **war_map.c/.h**: Map state (resources, players, panel dimensions).
- **war_units.c/.h**: Unit type definitions and stats.
- **war.h**: Core game types and constants.

### Module Prefix Conventions
- `wui_`: Core UI functions
- `wmui_`: Map UI functions
- `wmm_`: Map menu functions
- `we_`: Entity functions (used for entity lookup and creation)
- `wr_`: Rendering functions
- `wspr_`: Sprite functions
- `wres_`: Resource functions
- `wa_`: Audio functions

---

## Performance Considerations

### Optimization Strategies

1. **Fixed Entity Pool**: All UI entities allocated upfront; no dynamic allocation per-frame.
2. **Component-Disabled Skipping**: Only enabled components are processed during updates and rendering, reducing CPU overhead for hidden UI.
3. **Hash Map Lookup**: Named entity lookup is O(1) average, enabling efficient "find-and-update" patterns.
4. **Batch Button Updates**: `wui_updateUIButtons` processes all buttons in a single O(n) pass, with early exit on first matching hot-key.
5. **Minimal String Copies**: Text is stored once in `wstr` and referenced; avoiding repeated allocations.

### Avoid in Tight Loops

- **Don't call entity creation functions every frame**. Create UI once during initialization; update properties as needed.
- **Don't query `we_findUIEntity` in performance-critical paths without caching**. Cache entity pointers after lookup.
- **Avoid frequent string formatting**. Update text only when values change (e.g., resource count updates).

### Profiling with Tracy

Use Tracy markers to identify bottlenecks:

```c
ZoneScoped;  // Mark start of wui_updateUIButtons
// ... function body ...
ZoneEnd;     // (implicit at scope exit)
```

---

## Common Patterns & Idioms

### Pattern 1: Set UI Property by Name
```c
wui_setUIButtonStatusByName(context, wsv_fromCString("btnSave"), false);
setUIEntityStatus(wui_findUIEntity(context, wsv_fromCString("btnSave")), false);
```

### Pattern 2: Create Text with Formatting
```c
WarEntity* goldText = wui_createUIText(
    context,
    wstr_fromCString("txtGold"),
    0, 6,
    wstr_fromCStringFormat("GOLD:%*d", 6, context->map->players[0].gold),
    vec2i(135, 2)
);
```

### Pattern 3: Update Button Handler Dynamically
```c
WarEntity* btn = we_findUIEntity(context, wsv_fromCString("myButton"));
setUIButtonClickHandler(btn, new_handler_function);
```

### Pattern 4: Conditional UI Visibility
```c
// Show command buttons only when a unit is selected
bool hasSelection = (context->selectedUnits.count > 0);
for (s32 i = 0; i < 6; i++)
{
    String btnName = wstr_fromCStringFormat("btnCommand%d", i);
    wui_setUIEntityStatusByName(context, wstr_view(&btnName), hasSelection);
}
```

---

## Error Handling & Safety

### Assertions & Checks
- **Entity Lookup Failures**: `we_findUIEntity` returns `NULL` if entity not found; callers using `*ByName` helpers silently ignore missing entities.
- **Type Assumptions**: Most functions assume correct entity type (e.g., `wui_setUIText` assumes entity has text component); invalid type access causes undefined behavior—use `wui_isUIEntity` for validation.
- **Resource Validity**: Resource references (spriteResourceRef) are assumed valid at creation time; loading failures are handled by the resource module.

### Best Practices
- Always validate entity pointer before use:
  ```c
  WarEntity* entity = we_findUIEntity(context, wsv_fromCString("myEntity"));
  if (entity)
  {
      wui_setUIText(entity, ...);
  }
  ```
- Use named lookups (`*ByName`) for convenience; cache pointers for repeated access.
- Clear tooltips and text when no longer needed to avoid stale string references.

---

## Future Extensions

### Potential Enhancements
1. **UI Layout System**: Automated positioning/sizing based on anchor points and size policies.
2. **Event System**: Generic event broadcasting for non-button UI interactions.
3. **Animation Support**: Time-based UI element animations (fade, scale, slide).
4. **Theme System**: Configurable UI colors, fonts, and sprite sets per faction/theme.
5. **Accessibility**: Screen reader integration; high-contrast modes.

