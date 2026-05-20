#include "war_editor_tools.h"
#include "war_editor_autotile.h"

// ---------------------------------------------------------------------------
// File-local fill-drag state
// ---------------------------------------------------------------------------
static bool s_fillDragging  = false;
static s32  s_fillStartTx   = 0;
static s32  s_fillStartTy   = 0;
static s32  s_fillCurTx     = 0;
static s32  s_fillCurTy     = 0;

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Recompute all 8 neighbors of (tx, ty) for the given autotile group.
// weautotile_recompute guards itself — tiles that don't carry group presence
// are silently skipped.  Calling this for all 8 neighbors is correct for both
// 8-directional (trees/ruins) and 4-directional (roads/walls) groups.
static void wetools_recomputeNeighbors(WarEditorContext* ctx,
                                       s32 tx, s32 ty,
                                       WeAutotileGroup group)
{
    s32 x0 = (tx > 0)                    ? tx - 1 : tx;
    s32 x1 = (tx < MAP_TILES_WIDTH - 1)  ? tx + 1 : tx;
    s32 y0 = (ty > 0)                    ? ty - 1 : ty;
    s32 y1 = (ty < MAP_TILES_HEIGHT - 1) ? ty + 1 : ty;

    for (s32 ny = y0; ny <= y1; ny++)
    {
        for (s32 nx = x0; nx <= x1; nx++)
        {
            if (nx == tx && ny == ty) continue;
            weautotile_recompute(ctx, nx, ny, group);
        }
    }
}

// Write selectedTileIndex to the tile at (tx, ty).
// For autotile groups: marks presence then recomputes the tile and its
// neighbors so that joining edges update immediately.
// For plain tiles: writes visual + passability directly.
//
// TODO(Phase 10): push WE_OP_PAINT_TILE to ctx->history.
static void wetools_paintTile(WarEditorContext* ctx, s32 tx, s32 ty)
{
    WarEditorMap* m = ctx->map;
    if (!m) return;

    s32             idx      = ty * MAP_TILES_WIDTH + tx;
    WeAutotileGroup newGroup = weautotile_detectGroup(ctx);
    WeAutotileGroup oldGroup = weautotile_groupAtTile(m, tx, ty);

    if (newGroup != WE_AUTOTILE_NONE)
    {
        // When overpainting a different autotile group, clear the old
        // presence first so neighboring tiles in the old group update.
        if (oldGroup != WE_AUTOTILE_NONE && oldGroup != newGroup)
        {
            weautotile_clearPresence(m, tx, ty);
            wetools_recomputeNeighbors(ctx, tx, ty, oldGroup);
        }

        weautotile_markPresence(m, tx, ty, newGroup);
        weautotile_recompute(ctx, tx, ty, newGroup);
        wetools_recomputeNeighbors(ctx, tx, ty, newGroup);
    }
    else
    {
        // Plain tile: retire any previous autotile presence so neighbors heal.
        if (oldGroup != WE_AUTOTILE_NONE)
        {
            weautotile_clearPresence(m, tx, ty);
            wetools_recomputeNeighbors(ctx, tx, ty, oldGroup);
        }

        m->visualData[idx]   = ctx->selectedTileIndex;
        m->passableData[idx] = (ctx->selectedTileIndex == 0) ? 0u : 1u;
    }

    ctx->unsavedChanges = true;
}

// Fill the rectangular region [x0,x1] × [y0,y1] with selectedTileIndex.
// For autotile groups uses a two-pass approach:
//   Pass 1 — mark presence for every tile in the region.
//   Pass 2 — recompute visual for every tile in the region plus a 1-tile
//             border, so edges that join into pre-existing autotile tiles
//             update their bitmask correctly.
// For plain tiles writes visual + passability directly.
//
// TODO(Phase 10): push WE_OP_FILL_REGION to ctx->history.
static void wetools_fillRegion(WarEditorContext* ctx,
                               s32 x0, s32 y0, s32 x1, s32 y1)
{
    WarEditorMap* m = ctx->map;
    if (!m) return;

    if (x0 > x1) { s32 tmp = x0; x0 = x1; x1 = tmp; }
    if (y0 > y1) { s32 tmp = y0; y0 = y1; y1 = tmp; }

    WeAutotileGroup group = weautotile_detectGroup(ctx);

    if (group != WE_AUTOTILE_NONE)
    {
        // Pass 1: stamp presence for every tile in the fill rectangle.
        for (s32 fy = y0; fy <= y1; fy++)
        {
            for (s32 fx = x0; fx <= x1; fx++)
                weautotile_markPresence(m, fx, fy, group);
        }

        // Pass 2: recompute visual for the region plus a 1-tile border so
        // edge tiles pick up the correct bitmask from their new neighbors.
        s32 bx0 = (x0 > 0)                   ? x0 - 1 : x0;
        s32 by0 = (y0 > 0)                   ? y0 - 1 : y0;
        s32 bx1 = (x1 < MAP_TILES_WIDTH  - 1) ? x1 + 1 : x1;
        s32 by1 = (y1 < MAP_TILES_HEIGHT - 1) ? y1 + 1 : y1;

        for (s32 fy = by0; fy <= by1; fy++)
        {
            for (s32 fx = bx0; fx <= bx1; fx++)
                weautotile_recompute(ctx, fx, fy, group);
        }
    }
    else
    {
        for (s32 fy = y0; fy <= y1; fy++)
        {
            for (s32 fx = x0; fx <= x1; fx++)
            {
                s32 idx             = fy * MAP_TILES_WIDTH + fx;
                m->visualData[idx]  = ctx->selectedTileIndex;
                m->passableData[idx] = (ctx->selectedTileIndex == 0) ? 0u : 1u;
            }
        }
    }

    ctx->unsavedChanges = true;
}

// Complete and commit the current fill drag.
static void wetools_commitFill(WarEditorContext* ctx)
{
    wetools_fillRegion(ctx,
                       s_fillStartTx, s_fillStartTy,
                       s_fillCurTx,   s_fillCurTy);
    s_fillDragging = false;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void wetools_update(WarEditorContext* ctx)
{
    ImGuiIO* io = igGetIO_Nil();

    // Complete fill drag if the mouse was released outside the canvas.
    if (s_fillDragging && !io->MouseDown[0])
        wetools_commitFill(ctx);

    // Keyboard shortcuts: G = toggle grid, P = toggle passability.
    if (!io->WantTextInput)
    {
        if (igIsKeyPressed_Bool(ImGuiKey_G, false))
            ctx->showGrid = !ctx->showGrid;
        if (igIsKeyPressed_Bool(ImGuiKey_P, false))
            ctx->showPassability = !ctx->showPassability;
    }
}

void wetools_handleInput(WarEditorContext* ctx, s32 tx, s32 ty,
                         ImGuiIO* io, vec2 canvasOrigin)
{
    (void)canvasOrigin; // reserved for future tool-specific use

    WarEditorMap* m = ctx->map;
    if (!m) return;

    switch (ctx->activeTool)
    {
        case WE_TOOL_PENCIL:
            if (io->MouseDown[0])
                wetools_paintTile(ctx, tx, ty);
            break;

        case WE_TOOL_ERASE:
            if (io->MouseDown[0])
            {
                WeAutotileGroup group = weautotile_groupAtTile(m, tx, ty);
                if (group != WE_AUTOTILE_NONE)
                {
                    weautotile_clearPresence(m, tx, ty);
                    wetools_recomputeNeighbors(ctx, tx, ty, group);
                }
                else
                {
                    s32 idx             = ty * MAP_TILES_WIDTH + tx;
                    m->visualData[idx]  = 0;
                    m->passableData[idx] = 0;
                }
                ctx->unsavedChanges = true;
            }
            break;

        case WE_TOOL_FILL:
            if (io->MouseDown[0] && !s_fillDragging)
            {
                // Begin drag
                s_fillDragging = true;
                s_fillStartTx  = tx;
                s_fillStartTy  = ty;
                s_fillCurTx    = tx;
                s_fillCurTy    = ty;
            }
            else if (s_fillDragging && io->MouseDown[0])
            {
                // Track current tile while dragging
                s_fillCurTx = tx;
                s_fillCurTy = ty;
            }
            else if (s_fillDragging && !io->MouseDown[0])
            {
                // Released inside canvas
                wetools_commitFill(ctx);
            }
            break;

        case WE_TOOL_SELECT:
        case WE_TOOL_PLACE_ENTITY:
            break; // handled in Phase 8/9
    }
}

void wetools_drawOverlay(WarEditorContext* ctx, ImDrawList* drawList,
                         vec2 canvasOrigin)
{
    if (!s_fillDragging || ctx->activeTool != WE_TOOL_FILL)
        return;

    // Convert tile-space corners to screen space using the camera transform.
    f32 zoom = ctx->cameraZoom;
    f32 camX = ctx->cameraOffset.x;
    f32 camY = ctx->cameraOffset.y;

    s32 x0 = s_fillStartTx < s_fillCurTx ? s_fillStartTx : s_fillCurTx;
    s32 y0 = s_fillStartTy < s_fillCurTy ? s_fillStartTy : s_fillCurTy;
    s32 x1 = s_fillStartTx < s_fillCurTx ? s_fillCurTx   : s_fillStartTx;
    s32 y1 = s_fillStartTy < s_fillCurTy ? s_fillCurTy   : s_fillStartTy;

    f32 sx0 = canvasOrigin.x + ((f32)(x0 * MEGA_TILE_WIDTH)       - camX) * zoom;
    f32 sy0 = canvasOrigin.y + ((f32)(y0 * MEGA_TILE_HEIGHT)      - camY) * zoom;
    f32 sx1 = canvasOrigin.x + ((f32)((x1 + 1) * MEGA_TILE_WIDTH) - camX) * zoom;
    f32 sy1 = canvasOrigin.y + ((f32)((y1 + 1) * MEGA_TILE_HEIGHT)- camY) * zoom;

    // Semi-transparent yellow fill + opaque yellow border
    ImDrawList_AddRectFilled(drawList,
                             (ImVec2_c){ sx0, sy0 },
                             (ImVec2_c){ sx1, sy1 },
                             0x50FFFF00u,  // ABGR: 0x50 alpha, yellow
                             0.0f, 0);
    ImDrawList_AddRect(drawList,
                       (ImVec2_c){ sx0, sy0 },
                       (ImVec2_c){ sx1, sy1 },
                       0xCFFFFF00u,  // ABGR: 0xCF alpha, yellow
                       0.0f, 1.5f, 0);
}
