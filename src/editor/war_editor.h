#pragma once

#include "SDL3/SDL.h"

#include "war_entities.h"

// Forward declarations of editor-only types (defined in later phases).
struct _WarEditorMap;
typedef struct _WarEditorMap WarEditorMap;

struct _WarEditorHistory;
typedef struct _WarEditorHistory WarEditorHistory;

// -------------------------------------------------------------------------
// Tool types (Phase 7 adds full toolbox; stub enum used from Phase 2 onward)
// -------------------------------------------------------------------------
typedef enum
{
    WE_TOOL_SELECT        = 0,
    WE_TOOL_PENCIL        = 1,
    WE_TOOL_FILL          = 2,
    WE_TOOL_ERASE         = 3,
    WE_TOOL_PLACE_ENTITY  = 4,
    WE_TOOL_SET_START     = 5,
    WE_TOOL_ERASE_ENTITY  = 6,
} WarEditorToolType;

#define WE_CLIPBOARD_MAX 256

// -------------------------------------------------------------------------
// WarEditorContext — top-level singleton for the editor process.
// Analogous to WarContext in the game, but carries only editor-relevant data.
// -------------------------------------------------------------------------
typedef struct _WarEditorContext
{
    // SDL3
    SDL_Window*     window;
    SDL_Renderer*   renderer;

    // Timing
    f32             time;
    f32             deltaTime;
    u32             fps;
    u32             frameCount;

    // DATA.WAR resources (reuses existing WarFile / WarResource types)
    WarFile*        warFile;
    WarResource*    resources[MAX_RESOURCES_COUNT];

    // Bitmap fonts (two, same as game — for any in-canvas text)
    WarSprite       fontSprites[2];

    // Editor lifecycle
    bool            running;
    bool            unsavedChanges;
    bool            quitRequested;          // set by SDL_EVENT_QUIT; checked in weui_beginFrame
    char            currentFilePath[512];   // empty = new unsaved map
    char            mapName[64];

    // Canvas
    SDL_Texture*    canvasTarget;           // offscreen SDL_TEXTUREACCESS_TARGET
    int             canvasPanelW;           // tracked for resize detection
    int             canvasPanelH;
    vec2            cameraOffset;           // scroll in map pixels
    f32             cameraZoom;             // 0.5x – 4.0x

    // Active map data (editor-owned; NOT a WarMap)
    WarEditorMap*   map;

    // Active tool
    WarEditorToolType activeTool;
    u16               selectedTileIndex;    // terrain brush
    WarUnitType       selectedUnitType;     // entity placement
    u8                activePlayer;         // which player to assign to placed entities

    // Hover state — set by wecanvas_drawPanel each frame, consumed by wecanvas_render
    bool              isHoveringCanvas;
    s32               hoverTx;
    s32               hoverTy;

    // Selection (indices into map->startEntities)
    WarEntityIdList   selectedEntities;

    // Undo / redo
    WarEditorHistory* history;

    // Render state stack (shared with wr_* functions — must match WarContext layout)
    WarRenderState  renderState[MAX_RENDER_STATE_STACK];
    s32             renderStateTop;

    // Status bar text — updated each frame by canvas panel or other subsystems
    char            statusText[128];

    // UI overlay flags
    bool            showGrid;
    bool            showPassability;
    bool            showMinimap;
    bool            showStartLocation;

    // Minimap cache state (Phase 15)
    SDL_Texture*     minimapTexture;
    bool             minimapDirty;
    f32              minimapLastRebuildTime;

    // Recent files (persisted to war1_editor.cfg)
    char            recentFiles[5][512];
    int             recentFileCount;

    // Copy/paste clipboard (Phase 17)
    s32             clipboardCount;
    bool            clipboardIsGoldmine[WE_CLIPBOARD_MAX];
    WarLevelUnit    clipboardUnits[WE_CLIPBOARD_MAX];
    s16             clipboardDx[WE_CLIPBOARD_MAX];
    s16             clipboardDy[WE_CLIPBOARD_MAX];
    bool            pastePending;

} WarEditorContext;

// -------------------------------------------------------------------------
// Lifecycle
// -------------------------------------------------------------------------
bool we_init(WarEditorContext* ctx);
void we_run(WarEditorContext* ctx);
void we_quit(WarEditorContext* ctx);
