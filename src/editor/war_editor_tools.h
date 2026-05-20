#pragma once

#include "cimgui/cimgui.h"
#include "war_editor.h"

// ---------------------------------------------------------------------------
// Phase 7 — Terrain Editing Tools
//
// wetools_update      — call once per frame (before panels) to handle
//                       global tool state (e.g. fill-drag release when the
//                       mouse is outside the canvas).
// wetools_handleInput — call from wecanvas_drawPanel when the canvas is
//                       hovered; dispatches to the active tool.
// wetools_drawOverlay — call from wecanvas_drawPanel after igImage to draw
//                       transient overlays (fill preview rect) using the
//                       window's ImDrawList.
// ---------------------------------------------------------------------------

// Per-frame update; must be called every frame from weui_beginFrame.
void wetools_update(WarEditorContext* ctx);

// Dispatch left-click/drag at tile (tx, ty) to the active tool.
// io:          ImGuiIO pointer (caller already has it).
// canvasOrigin: screen-space top-left of the canvas image (for overlay math).
void wetools_handleInput(WarEditorContext* ctx, s32 tx, s32 ty,
                         ImGuiIO* io, vec2 canvasOrigin);

// Draw in-progress tool overlays (fill preview, etc.) into drawList.
// Call this after igImage and before igEnd of the canvas window.
void wetools_drawOverlay(WarEditorContext* ctx, ImDrawList* drawList,
                         vec2 canvasOrigin);
