#pragma once

#include "SDL3/SDL.h"
#include "cimgui/cimgui.h"
#include "cimgui/cimgui_impl.h"

// Forward declaration — war_editor.h is included by war_editor_ui.c, not here,
// to avoid a circular include chain.
struct _WarEditorContext;
typedef struct _WarEditorContext WarEditorContext;

// Initialize cimgui context and SDL3 backends.
// Must be called after the SDL_Window and SDL_Renderer are created.
void weui_init(SDL_Window* window, SDL_Renderer* renderer);

// Shut down cimgui and SDL3 backends.
void weui_shutdown(void);

// Begin a new cimgui frame: call ImGui_Impl*_NewFrame, igNewFrame,
// sync DPI scale, draw full-window dockspace, main menu bar, and toolbar.
// ctx is passed for read-only access to editor state (tool, unsaved flag, etc.).
void weui_beginFrame(WarEditorContext* ctx);

// Render the accumulated cimgui draw lists via the SDL3 renderer backend.
void weui_endFrame(SDL_Renderer* renderer);
