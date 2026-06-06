#pragma once

#include "war_editor.h"

// ---------------------------------------------------------------------------
// Terrain + entity canvas (read-only SDL3 offscreen render target)
//
// Rendering pipeline each frame:
//   1. weui_beginFrame → wecanvas_drawPanel  (builds ImGui draw commands,
//      resizes canvas target when panel size changes)
//   2. wecanvas_render                       (renders terrain + entities to
//      canvas target in two sub-passes)
//   3. weui_endFrame → RenderDrawData        (blits canvas target to screen)
//
// entity rendering:
//   • Unit/building sprites decoded from DATA.WAR palette pair (191, 217)
//   • Sprite textures cached in a file-local static array (max 64 entries)
//   • Player color chips (3 px × zoom) drawn at the unit tile origin
//   • Roads, walls, and trees are baked into visualData and rendered by the
//     terrain pass; no separate entity overlay is needed for those
// ---------------------------------------------------------------------------

// — Sprite texture cache: decode frame 0 of the DATA.WAR sprite resource and
// cache the result.  Also used by the entity palette panel for thumbnails.
SDL_Texture* wecanvas_getSpriteTexture(WarEditorContext* ctx, s32 resourceIndex);

// — Allocate an SDL_TEXTUREACCESS_TARGET texture of size (w × h).
// Returns NULL on failure and logs the SDL error.
SDL_Texture* wecanvas_createTarget(SDL_Renderer* renderer, int w, int h);

// — Render the map to ctx->canvasTarget (terrain + entities + overlays).
// Sets the SDL render target, clears it, renders all visible content, then
// resets the render target to NULL so subsequent SDL calls go to the window.
void wecanvas_render(WarEditorContext* ctx);

// — Semi-transparent red quads over tiles where passableData != 0.
//        Must be called with ctx->canvasTarget set as the active render target.
void wecanvas_renderPassability(WarEditorContext* ctx);

// — Thin grid lines over every tile boundary in the visible viewport.
//        Must be called with ctx->canvasTarget set as the active render target.
void wecanvas_renderGrid(WarEditorContext* ctx);

// — Build the cimgui canvas panel window.
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

// — Ghost preview of the selected unit type at the hover tile.
// Renders with 50% alpha; red tint when the position is already occupied.
// Must be called while ctx->canvasTarget is the active render target.
void wecanvas_renderEntityGhost(WarEditorContext* ctx);

// — Yellow selection border for every entity in ctx->selectedEntities.
// Applies move-drag offset from wetools_getMoveDelta when a move is in progress.
// Must be called while ctx->canvasTarget is the active render target.
void wecanvas_renderSelection(WarEditorContext* ctx);

// 13.9 — Lime-green X + tile border at the map start location (startX, startY).
// Only rendered when ctx->showStartLocation is true; skipped when off-screen.
// Must be called while ctx->canvasTarget is the active render target.
void wecanvas_renderStartLocation(WarEditorContext* ctx);
