#pragma once

#include "war_editor.h"

// ---------------------------------------------------------------------------
// Phase 4 — Terrain canvas (read-only SDL3 offscreen render target)
//
// Rendering pipeline each frame:
//   1. weui_beginFrame → wecanvas_drawPanel  (builds ImGui draw commands,
//      resizes canvas target when panel size changes)
//   2. wecanvas_render                       (renders terrain to canvas target)
//   3. weui_endFrame → RenderDrawData        (blits canvas target to screen)
// ---------------------------------------------------------------------------

// 4.2 — Allocate an SDL_TEXTUREACCESS_TARGET texture of size (w × h).
// Returns NULL on failure and logs the SDL error.
SDL_Texture* wecanvas_createTarget(SDL_Renderer* renderer, int w, int h);

// 4.3 — Render the map view (terrain, then future layers) into ctx->canvasTarget.
// Sets the SDL render target, clears it, renders all visible content, then
// resets the render target to NULL so subsequent SDL calls go to the window.
void wecanvas_render(WarEditorContext* ctx);

// 4.5 — Build the cimgui canvas panel window.
//   • Detects panel resize and recreates ctx->canvasTarget accordingly.
//   • Records igImage for the canvas texture.
//   • Handles pan (middle-mouse) and zoom (mouse-wheel) when hovered (4.6 / 4.7).
//   • Writes current tile coordinate into statusBuf (4.8).
void wecanvas_drawPanel(WarEditorContext* ctx, char* statusBuf, s32 statusBufLen);

// Coordinate helpers (4.8)
// screenToMap: convert absolute screen position to map-space pixels.
// mapToTile:   convert map-space pixels to tile (column, row) integers.
vec2  wecanvas_screenToMap(WarEditorContext* ctx, vec2 screenPos, vec2 canvasOrigin);
vec2  wecanvas_mapToTile(vec2 mapPos);
