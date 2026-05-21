#pragma once

#include "war_editor.h"

// ---------------------------------------------------------------------------
// Phase 4+6 — Terrain + entity canvas (read-only SDL3 offscreen render target)
//
// Rendering pipeline each frame:
//   1. weui_beginFrame → wecanvas_drawPanel  (builds ImGui draw commands,
//      resizes canvas target when panel size changes)
//   2. wecanvas_render                       (renders terrain + entities to
//      canvas target in two sub-passes)
//   3. weui_endFrame → RenderDrawData        (blits canvas target to screen)
//
// Phase 6 entity rendering:
//   • Unit/building sprites decoded from DATA.WAR palette pair (191, 217)
//   • Sprite textures cached in a file-local static array (max 64 entries)
//   • Player color chips (3 px × zoom) drawn at the unit tile origin
//   • Roads, walls, and trees are baked into visualData and rendered by the
//     terrain pass; no separate entity overlay is needed for those
// ---------------------------------------------------------------------------

// 8.2 — Sprite texture cache: decode frame 0 of the DATA.WAR sprite resource and
// cache the result.  Also used by the entity palette panel for thumbnails.
SDL_Texture* wecanvas_getSpriteTexture(WarEditorContext* ctx, s32 resourceIndex);

// 4.2 — Allocate an SDL_TEXTUREACCESS_TARGET texture of size (w × h).
// Returns NULL on failure and logs the SDL error.
SDL_Texture* wecanvas_createTarget(SDL_Renderer* renderer, int w, int h);

// 4.3 — Render the map to ctx->canvasTarget (terrain + entities + overlays).
// Sets the SDL render target, clears it, renders all visible content, then
// resets the render target to NULL so subsequent SDL calls go to the window.
void wecanvas_render(WarEditorContext* ctx);

// 7.6 — Semi-transparent red quads over tiles where passableData != 0.
//        Must be called with ctx->canvasTarget set as the active render target.
void wecanvas_renderPassability(WarEditorContext* ctx);

// 7.7 — Thin grid lines over every tile boundary in the visible viewport.
//        Must be called with ctx->canvasTarget set as the active render target.
void wecanvas_renderGrid(WarEditorContext* ctx);

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

// 8.4 — Ghost preview of the selected unit type at the hover tile.
// Renders with 50% alpha; red tint when the position is already occupied.
// Must be called while ctx->canvasTarget is the active render target.
void wecanvas_renderEntityGhost(WarEditorContext* ctx);

// 9.3 — Yellow selection border for every entity in ctx->selectedEntities.
// Applies move-drag offset from wetools_getMoveDelta when a move is in progress.
// Must be called while ctx->canvasTarget is the active render target.
void wecanvas_renderSelection(WarEditorContext* ctx);
