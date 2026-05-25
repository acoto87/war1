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
//                       transient overlays (fill preview rect, select rect)
//                       using the window's ImDrawList.
// ---------------------------------------------------------------------------

// Per-frame update; must be called every frame from weui_beginFrame.
void wetools_update(WarEditorContext* ctx);

// Dispatch left-click/drag at tile (tx, ty) to the active tool.
// io:          ImGuiIO pointer (caller already has it).
// canvasOrigin: screen-space top-left of the canvas image (for overlay math).
void wetools_handleInput(WarEditorContext* ctx, s32 tx, s32 ty,
                         ImGuiIO* io, vec2 canvasOrigin);

// Draw in-progress tool overlays (fill preview, select rect) into drawList.
// Call this after igImage and before igEnd of the canvas window.
void wetools_drawOverlay(WarEditorContext* ctx, ImDrawList* drawList,
                         vec2 canvasOrigin);

// ---------------------------------------------------------------------------
// Phase 8/9 helpers
// ---------------------------------------------------------------------------

// Returns true if any tile in the [tx, tx+w-1] × [ty, ty+h-1] region is
// occupied by an existing entity in the map.
bool wetools_isOccupied(WarEditorContext* ctx, s32 tx, s32 ty, s32 w, s32 h);

// Returns true if the entity footprint is valid for placement:
//   - every tile in the footprint is passable (passableData == 0)
//   - no existing entity occupies the footprint
bool wetools_canPlace(WarEditorContext* ctx, s32 tx, s32 ty, s32 w, s32 h);

// Fills *dtx / *dty with the current move-drag tile delta.
// Both are zero when no move drag is in progress.
void wetools_getMoveDelta(WarEditorContext* ctx, s32* dtx, s32* dty);

// Fills *x0/*y0/*x1/*y1 with the current selection drag-rect tile corners.
// Only meaningful when wetools_isSelectDragging returns true.
void wetools_getSelectDragRect(WarEditorContext* ctx,
                               s32* x0, s32* y0, s32* x1, s32* y1);

// Returns true while a rubber-band selection rect drag is in progress.
bool wetools_isSelectDragging(WarEditorContext* ctx);

// Delete all entities listed in ctx->selectedEntities, compact the arrays,
// and clear the selection.
void wetools_deleteSelected(WarEditorContext* ctx);

// Copy selected entities into the editor clipboard.
void wetools_copySelected(WarEditorContext* ctx);

// Enter paste mode; the next left click in the canvas places the clipboard.
void wetools_beginPaste(WarEditorContext* ctx);

// Returns true when clipboard has at least one entity.
bool wetools_canPaste(WarEditorContext* ctx);
