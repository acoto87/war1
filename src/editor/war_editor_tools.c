#include "war_editor_tools.h"
#include "war_editor_autotile.h"
#include "war_editor_history.h"

// ---------------------------------------------------------------------------
// File-local fill-drag state
// ---------------------------------------------------------------------------
static bool s_fillDragging  = false;
static s32  s_fillStartTx   = 0;
static s32  s_fillStartTy   = 0;
static s32  s_fillCurTx     = 0;
static s32  s_fillCurTy     = 0;

// ---------------------------------------------------------------------------
// — Select tool state
// ---------------------------------------------------------------------------
typedef enum
{
    WE_SELECT_IDLE      = 0,
    WE_SELECT_DRAG_RECT = 1,
    WE_SELECT_DRAG_MOVE = 2,
} WeSelectState;

static WeSelectState s_selectState   = WE_SELECT_IDLE;
static s32           s_selectStartTx = 0;   // rect origin or move press tile
static s32           s_selectStartTy = 0;
static s32           s_selectCurTx   = 0;   // rect current or move current tile
static s32           s_selectCurTy   = 0;
static s32           s_moveDtx       = 0;   // running tile delta for move drag
static s32           s_moveDty       = 0;

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

// Sort an index array ascending using insertion sort (arrays are small).
static void wetools_sortIndicesAsc(s32* arr, s32 count)
{
    for (s32 i = 1; i < count; i++)
    {
        s32 key = arr[i];
        s32 j   = i - 1;
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

static bool wetools_rectsOverlap(s32 ax, s32 ay, s32 aw, s32 ah,
                                 s32 bx, s32 by, s32 bw, s32 bh)
{
    return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}

// Write selectedTileIndex to the tile at (tx, ty).
// For autotile groups: marks presence then recomputes the tile and its
// neighbors so that joining edges update immediately.
// For plain tiles: writes visual + passability directly.
// Pushes WE_OP_PAINT_TILE to ctx->history when the tile state changed.
static void wetools_paintTile(WarEditorContext* ctx, s32 tx, s32 ty)
{
    WarEditorMap* m = ctx->map;
    if (!m) return;

    s32             idx      = ty * MAP_TILES_WIDTH + tx;
    WeAutotileGroup newGroup = weautotile_detectGroup(ctx);
    WeAutotileGroup oldGroup = weautotile_groupAtTile(m, tx, ty);

    // Snapshot neighborhood before paint.
    u16 oldVis[9], oldPass[9];
    wehist_captureNeighborhood(m, tx, ty, oldVis, oldPass);

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
        m->passableData[idx] = 0u;
    }

    ctx->unsavedChanges = true;
    ctx->minimapDirty = true;

    // Snapshot neighborhood after paint; push op only if anything changed.
    u16 newVis[9], newPass[9];
    wehist_captureNeighborhood(m, tx, ty, newVis, newPass);

    bool changed = false;
    for (s32 k = 0; k < 9; k++)
    {
        if (oldVis[k] != newVis[k] || oldPass[k] != newPass[k])
        {
            changed = true;
            break;
        }
    }

    if (changed && ctx->history)
    {
        WarEditorOp op;
        memset(&op, 0, sizeof(op));
        op.type         = WE_OP_PAINT_TILE;
        op.paintTile.cx = tx;
        op.paintTile.cy = ty;
        memcpy(op.paintTile.oldVisual,   oldVis,  sizeof(oldVis));
        memcpy(op.paintTile.oldPassable, oldPass, sizeof(oldPass));
        memcpy(op.paintTile.newVisual,   newVis,  sizeof(newVis));
        memcpy(op.paintTile.newPassable, newPass, sizeof(newPass));
        wehist_push(ctx->history, op);
    }
}

// Fill the rectangular region [x0,x1] × [y0,y1] with selectedTileIndex.
// For autotile groups uses a two-pass approach:
//   Pass 1 — mark presence for every tile in the region.
//   Pass 2 — recompute visual for every tile in the region plus a 1-tile
//             border, so edges that join into pre-existing autotile tiles
//             update their bitmask correctly.
// For plain tiles writes visual + passability directly.
// Pushes WE_OP_FILL_REGION to ctx->history.
static void wetools_fillRegion(WarEditorContext* ctx,
                               s32 x0, s32 y0, s32 x1, s32 y1)
{
    WarEditorMap* m = ctx->map;
    if (!m) return;

    if (x0 > x1) { s32 tmp = x0; x0 = x1; x1 = tmp; }
    if (y0 > y1) { s32 tmp = y0; y0 = y1; y1 = tmp; }

    // Bounding box for history snapshot: fill region + 1-tile border, clamped.
    s32 bx0  = (x0 > 0)                    ? x0 - 1 : x0;
    s32 by0  = (y0 > 0)                    ? y0 - 1 : y0;
    s32 bx1  = (x1 < MAP_TILES_WIDTH  - 1) ? x1 + 1 : x1;
    s32 by1  = (y1 < MAP_TILES_HEIGHT - 1) ? y1 + 1 : y1;
    s32 bw   = bx1 - bx0 + 1;
    s32 bh   = by1 - by0 + 1;
    s32 bCnt = bw * bh;

    u16* oldVis  = (u16*)wm_alloc((u32)bCnt * sizeof(u16));
    u16* oldPass = (u16*)wm_alloc((u32)bCnt * sizeof(u16));
    u16* newVis  = (u16*)wm_alloc((u32)bCnt * sizeof(u16));
    u16* newPass = (u16*)wm_alloc((u32)bCnt * sizeof(u16));

    // Capture old state.
    for (s32 y = by0; y <= by1; y++)
    {
        for (s32 x = bx0; x <= bx1; x++)
        {
            s32 k = (y - by0) * bw + (x - bx0);
            s32 i = y * MAP_TILES_WIDTH + x;
            oldVis[k]  = m->visualData[i];
            oldPass[k] = m->passableData[i];
        }
    }

    WeAutotileGroup group = weautotile_detectGroup(ctx);

    if (group != WE_AUTOTILE_NONE)
    {
        // Pass 1: stamp presence for every tile in the fill rectangle.
        for (s32 fy = y0; fy <= y1; fy++)
        {
            for (s32 fx = x0; fx <= x1; fx++)
                weautotile_markPresence(m, fx, fy, group);
        }

        // Pass 2: recompute visual for the bbox (includes 1-tile border).
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
                s32 i             = fy * MAP_TILES_WIDTH + fx;
                m->visualData[i]  = ctx->selectedTileIndex;
                m->passableData[i] = 0u;
            }
        }
    }

    ctx->unsavedChanges = true;
    ctx->minimapDirty = true;

    // Capture new state.
    for (s32 y = by0; y <= by1; y++)
    {
        for (s32 x = bx0; x <= bx1; x++)
        {
            s32 k = (y - by0) * bw + (x - bx0);
            s32 i = y * MAP_TILES_WIDTH + x;
            newVis[k]  = m->visualData[i];
            newPass[k] = m->passableData[i];
        }
    }

    if (ctx->history)
    {
        WarEditorOp op;
        memset(&op, 0, sizeof(op));
        op.type                   = WE_OP_FILL_REGION;
        op.fillRegion.bx0         = bx0;
        op.fillRegion.by0         = by0;
        op.fillRegion.bx1         = bx1;
        op.fillRegion.by1         = by1;
        op.fillRegion.bw          = bw;
        op.fillRegion.bh          = bh;
        op.fillRegion.oldVisual   = oldVis;
        op.fillRegion.oldPassable = oldPass;
        op.fillRegion.newVisual   = newVis;
        op.fillRegion.newPassable = newPass;
        wehist_push(ctx->history, op);
        // Ownership transferred — do not free here.
    }
    else
    {
        wm_free(oldVis);
        wm_free(oldPass);
        wm_free(newVis);
        wm_free(newPass);
    }
}

// Complete and commit the current fill drag.
static void wetools_commitFill(WarEditorContext* ctx)
{
    wetools_fillRegion(ctx,
                       s_fillStartTx, s_fillStartTy,
                       s_fillCurTx,   s_fillCurTy);
    s_fillDragging = false;
}

static void wetools_setStartLocation(WarEditorContext* ctx, s32 tx, s32 ty)
{
    WarEditorMap* m = ctx->map;
    if (!m)
    {
        return;
    }

    u16 newX = (u16)tx;
    u16 newY = (u16)ty;
    if (m->startX == newX && m->startY == newY)
    {
        return;
    }

    if (ctx->history)
    {
        WarEditorOp op;
        memset(&op, 0, sizeof(op));
        op.type          = WE_OP_SET_START;
        op.setStart.oldX = m->startX;
        op.setStart.oldY = m->startY;
        op.setStart.newX = newX;
        op.setStart.newY = newY;
        wehist_push(ctx->history, op);
    }

    m->startX = newX;
    m->startY = newY;
    ctx->unsavedChanges = true;
    ctx->minimapDirty = true;
}

// Erase the tile at (tx, ty): removes autotile presence or clears plain tile.
// Pushes WE_OP_PAINT_TILE to ctx->history when the tile state changed.
static void wetools_eraseTile(WarEditorContext* ctx, s32 tx, s32 ty)
{
    WarEditorMap* m = ctx->map;
    if (!m) return;

    // Snapshot neighborhood before erase.
    u16 oldVis[9], oldPass[9];
    wehist_captureNeighborhood(m, tx, ty, oldVis, oldPass);

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
    ctx->minimapDirty = true;

    // Snapshot after erase; push op only if anything changed.
    u16 newVis[9], newPass[9];
    wehist_captureNeighborhood(m, tx, ty, newVis, newPass);

    bool changed = false;
    for (s32 k = 0; k < 9; k++)
    {
        if (oldVis[k] != newVis[k] || oldPass[k] != newPass[k])
        {
            changed = true;
            break;
        }
    }

    if (changed && ctx->history)
    {
        WarEditorOp op;
        memset(&op, 0, sizeof(op));
        op.type         = WE_OP_PAINT_TILE;
        op.paintTile.cx = tx;
        op.paintTile.cy = ty;
        memcpy(op.paintTile.oldVisual,   oldVis,  sizeof(oldVis));
        memcpy(op.paintTile.oldPassable, oldPass, sizeof(oldPass));
        memcpy(op.paintTile.newVisual,   newVis,  sizeof(newVis));
        memcpy(op.paintTile.newPassable, newPass, sizeof(newPass));
        wehist_push(ctx->history, op);
    }
}

// Return the entity ID at tile (ptx, pty), or 0 if none.
// Regular entities encode as (index+1); goldmines as (0x8000 | index).
static WarEntityId wetools_entityAtTile(WarEditorContext* ctx, s32 ptx, s32 pty)
{
    WarEditorMap* m = ctx->map;
    if (!m) return 0;

    for (u32 i = 0; i < m->startEntitiesCount; i++)
    {
        WarLevelUnit*      lu = &m->startEntities[i];
        const WarUnitData* ud = wu_getUnitData(lu->type);
        if (!ud) continue;
        if (ptx >= (s32)lu->x && ptx < (s32)lu->x + ud->sizex &&
            pty >= (s32)lu->y && pty < (s32)lu->y + ud->sizey)
            return (WarEntityId)(i + 1u);
    }

    for (u32 i = 0; i < m->startGoldminesCount; i++)
    {
        WarLevelUnit*      lu = &m->startGoldmines[i];
        const WarUnitData* ud = wu_getUnitData(lu->type);
        if (!ud) continue;
        if (ptx >= (s32)lu->x && ptx < (s32)lu->x + ud->sizex &&
            pty >= (s32)lu->y && pty < (s32)lu->y + ud->sizey)
            return (WarEntityId)(0x8000u | i);
    }

    return 0;
}

static void wetools_eraseEntityAtTile(WarEditorContext* ctx, s32 tx, s32 ty)
{
    WarEntityId hit = wetools_entityAtTile(ctx, tx, ty);
    if (!hit)
    {
        return;
    }

    WarEntityIdListClear(&ctx->selectedEntities);
    WarEntityIdListAdd(&ctx->selectedEntities, hit);
    wetools_deleteSelected(ctx);
}

static bool wetools_tryPasteAt(WarEditorContext* ctx, s32 tx, s32 ty)
{
    WarEditorMap* m = ctx->map;
    if (!m || ctx->clipboardCount <= 0)
    {
        return false;
    }

    for (s32 i = 0; i < ctx->clipboardCount; i++)
    {
        WarLevelUnit* lu = &ctx->clipboardUnits[i];
        const WarUnitData* ud = wu_getUnitData(lu->type);
        if (!ud)
        {
            return false;
        }

        s32 px = tx + (s32)ctx->clipboardDx[i];
        s32 py = ty + (s32)ctx->clipboardDy[i];
        if (!wetools_canPlace(ctx, px, py, ud->sizex, ud->sizey))
        {
            SDL_strlcpy(ctx->statusText,
                        "Paste blocked: destination overlaps blocked or occupied tiles.",
                        sizeof(ctx->statusText));
            return false;
        }

        for (s32 j = i + 1; j < ctx->clipboardCount; j++)
        {
            WarLevelUnit* lu2 = &ctx->clipboardUnits[j];
            const WarUnitData* ud2 = wu_getUnitData(lu2->type);
            if (!ud2)
            {
                continue;
            }

            s32 px2 = tx + (s32)ctx->clipboardDx[j];
            s32 py2 = ty + (s32)ctx->clipboardDy[j];
            if (wetools_rectsOverlap(px, py, ud->sizex, ud->sizey,
                                     px2, py2, ud2->sizex, ud2->sizey))
            {
                SDL_strlcpy(ctx->statusText,
                            "Paste blocked: clipboard entities would overlap each other.",
                            sizeof(ctx->statusText));
                return false;
            }
        }
    }

    for (s32 i = 0; i < ctx->clipboardCount; i++)
    {
        WarLevelUnit placed = ctx->clipboardUnits[i];
        placed.x = (u8)(tx + (s32)ctx->clipboardDx[i]);
        placed.y = (u8)(ty + (s32)ctx->clipboardDy[i]);

        if (ctx->clipboardIsGoldmine[i])
        {
            if (m->startGoldminesCount >= MAX_CUSTOM_MAP_GOLDMINES_COUNT)
            {
                continue;
            }
            m->startGoldmines[m->startGoldminesCount++] = placed;
        }
        else
        {
            if (m->startEntitiesCount >= MAX_ENTITIES_COUNT)
            {
                continue;
            }
            m->startEntities[m->startEntitiesCount++] = placed;
        }

        if (ctx->history)
        {
            WarEditorOp op;
            memset(&op, 0, sizeof(op));
            op.type = WE_OP_PLACE_ENTITY;
            op.placeEntity.entity = placed;
            op.placeEntity.isGoldmine = ctx->clipboardIsGoldmine[i];
            wehist_push(ctx->history, op);
        }
    }

    ctx->unsavedChanges = true;
    ctx->minimapDirty = true;
    ctx->pastePending = false;
    return true;
}

// Commit the select drag-rect: add all entities whose footprint intersects
// the drag rectangle to ctx->selectedEntities.
static void wetools_commitSelectDrag(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx->map;
    if (!m)
    {
        s_selectState = WE_SELECT_IDLE;
        return;
    }

    s32 rx0 = s_selectStartTx < s_selectCurTx ? s_selectStartTx : s_selectCurTx;
    s32 ry0 = s_selectStartTy < s_selectCurTy ? s_selectStartTy : s_selectCurTy;
    s32 rx1 = s_selectStartTx < s_selectCurTx ? s_selectCurTx   : s_selectStartTx;
    s32 ry1 = s_selectStartTy < s_selectCurTy ? s_selectCurTy   : s_selectStartTy;

    WarEntityIdListClear(&ctx->selectedEntities);

    for (u32 i = 0; i < m->startEntitiesCount; i++)
    {
        WarLevelUnit*      lu = &m->startEntities[i];
        const WarUnitData* ud = wu_getUnitData(lu->type);
        if (!ud) continue;

        s32 ex0 = (s32)lu->x;
        s32 ey0 = (s32)lu->y;
        s32 ex1 = ex0 + ud->sizex - 1;
        s32 ey1 = ey0 + ud->sizey - 1;

        if (rx0 <= ex1 && rx1 >= ex0 && ry0 <= ey1 && ry1 >= ey0)
        {
            WarEntityId id = (WarEntityId)(i + 1u);
            WarEntityIdListAdd(&ctx->selectedEntities, id);
        }
    }

    for (u32 i = 0; i < m->startGoldminesCount; i++)
    {
        WarLevelUnit*      lu = &m->startGoldmines[i];
        const WarUnitData* ud = wu_getUnitData(lu->type);
        if (!ud) continue;

        s32 ex0 = (s32)lu->x;
        s32 ey0 = (s32)lu->y;
        s32 ex1 = ex0 + ud->sizex - 1;
        s32 ey1 = ey0 + ud->sizey - 1;

        if (rx0 <= ex1 && rx1 >= ex0 && ry0 <= ey1 && ry1 >= ey0)
        {
            WarEntityId id = (WarEntityId)(0x8000u | i);
            WarEntityIdListAdd(&ctx->selectedEntities, id);
        }
    }

    s_selectState = WE_SELECT_IDLE;
}

// Commit the move drag: apply (s_moveDtx, s_moveDty) to all selected entities.
static void wetools_commitMove(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx->map;
    if (!m || (s_moveDtx == 0 && s_moveDty == 0))
    {
        s_selectState = WE_SELECT_IDLE;
        s_moveDtx     = 0;
        s_moveDty     = 0;
        return;
    }

    // Collect sorted entity indices before moving, push history op.
    s32 regularCount  = 0;
    s32 goldmineCount = 0;
    for (s32 i = 0; i < ctx->selectedEntities.count; i++)
    {
        WarEntityId id = ctx->selectedEntities.items[i];
        if (id & 0x8000u) goldmineCount++;
        else              regularCount++;
    }

    s32* regularIndices  = NULL;
    s32* goldmineIndices = NULL;
    if (regularCount  > 0)
        regularIndices  = (s32*)wm_alloc((u32)regularCount  * sizeof(s32));
    if (goldmineCount > 0)
        goldmineIndices = (s32*)wm_alloc((u32)goldmineCount * sizeof(s32));

    s32 rk = 0, gk = 0;
    for (s32 i = 0; i < ctx->selectedEntities.count; i++)
    {
        WarEntityId id = ctx->selectedEntities.items[i];
        if (id & 0x8000u)
            goldmineIndices[gk++] = (s32)(id & 0x7FFFu);
        else
            regularIndices[rk++]  = (s32)(id - 1u);
    }

    if (regularIndices)  wetools_sortIndicesAsc(regularIndices,  regularCount);
    if (goldmineIndices) wetools_sortIndicesAsc(goldmineIndices, goldmineCount);

    if (ctx->history)
    {
        WarEditorOp op;
        memset(&op, 0, sizeof(op));
        op.type                      = WE_OP_MOVE_BATCH;
        op.moveBatch.regularIndices  = regularIndices;
        op.moveBatch.regularCount    = regularCount;
        op.moveBatch.goldmineIndices = goldmineIndices;
        op.moveBatch.goldmineCount   = goldmineCount;
        op.moveBatch.dtx             = s_moveDtx;
        op.moveBatch.dty             = s_moveDty;
        wehist_push(ctx->history, op);
        // Ownership transferred — do not free here.
    }
    else
    {
        if (regularIndices)  wm_free(regularIndices);
        if (goldmineIndices) wm_free(goldmineIndices);
    }

    for (s32 i = 0; i < ctx->selectedEntities.count; i++)
    {
        WarEntityId id = ctx->selectedEntities.items[i];

        if (id & 0x8000u)
        {
            u32 idx = (u32)(id & 0x7FFFu);
            if (idx >= m->startGoldminesCount) continue;
            WarLevelUnit* lu = &m->startGoldmines[idx];
            s32 nx = (s32)lu->x + s_moveDtx;
            s32 ny = (s32)lu->y + s_moveDty;
            if (nx < 0) nx = 0;
            if (ny < 0) ny = 0;
            if (nx >= MAP_TILES_WIDTH)  nx = MAP_TILES_WIDTH  - 1;
            if (ny >= MAP_TILES_HEIGHT) ny = MAP_TILES_HEIGHT - 1;
            lu->x = (u8)nx;
            lu->y = (u8)ny;
        }
        else
        {
            u32 idx = (u32)(id - 1u);
            if (idx >= m->startEntitiesCount) continue;
            WarLevelUnit* lu = &m->startEntities[idx];
            s32 nx = (s32)lu->x + s_moveDtx;
            s32 ny = (s32)lu->y + s_moveDty;
            if (nx < 0) nx = 0;
            if (ny < 0) ny = 0;
            if (nx >= MAP_TILES_WIDTH)  nx = MAP_TILES_WIDTH  - 1;
            if (ny >= MAP_TILES_HEIGHT) ny = MAP_TILES_HEIGHT - 1;
            lu->x = (u8)nx;
            lu->y = (u8)ny;
        }
    }

    ctx->unsavedChanges = true;
    ctx->minimapDirty = true;
    s_selectState = WE_SELECT_IDLE;
    s_moveDtx     = 0;
    s_moveDty     = 0;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool wetools_isOccupied(WarEditorContext* ctx, s32 tx, s32 ty, s32 w, s32 h)
{
    WarEditorMap* m = ctx->map;
    if (!m) return false;

    for (u32 i = 0; i < m->startEntitiesCount; i++)
    {
        WarLevelUnit*      lu = &m->startEntities[i];
        const WarUnitData* ud = wu_getUnitData(lu->type);
        if (!ud) continue;
        if (tx < (s32)lu->x + ud->sizex && tx + w > (s32)lu->x &&
            ty < (s32)lu->y + ud->sizey && ty + h > (s32)lu->y)
            return true;
    }

    for (u32 i = 0; i < m->startGoldminesCount; i++)
    {
        WarLevelUnit*      lu = &m->startGoldmines[i];
        const WarUnitData* ud = wu_getUnitData(lu->type);
        if (!ud) continue;
        if (tx < (s32)lu->x + ud->sizex && tx + w > (s32)lu->x &&
            ty < (s32)lu->y + ud->sizey && ty + h > (s32)lu->y)
            return true;
    }

    return false;
}

bool wetools_canPlace(WarEditorContext* ctx, s32 tx, s32 ty, s32 w, s32 h)
{
    WarEditorMap* m = ctx->map;
    if (!m) return false;

    // Check that every tile in the footprint is passable.
    for (s32 fy = ty; fy < ty + h; fy++)
    {
        for (s32 fx = tx; fx < tx + w; fx++)
        {
            if (fx < 0 || fx >= MAP_TILES_WIDTH || fy < 0 || fy >= MAP_TILES_HEIGHT)
                return false;
            if (m->passableData[fy * MAP_TILES_WIDTH + fx] != 0)
                return false;
        }
    }

    // Check that no entity occupies the footprint.
    return !wetools_isOccupied(ctx, tx, ty, w, h);
}

void wetools_getMoveDelta(WarEditorContext* ctx, s32* dtx, s32* dty)
{
    (void)ctx;
    *dtx = (s_selectState == WE_SELECT_DRAG_MOVE) ? s_moveDtx : 0;
    *dty = (s_selectState == WE_SELECT_DRAG_MOVE) ? s_moveDty : 0;
}

void wetools_getSelectDragRect(WarEditorContext* ctx,
                               s32* x0, s32* y0, s32* x1, s32* y1)
{
    (void)ctx;
    *x0 = s_selectStartTx;
    *y0 = s_selectStartTy;
    *x1 = s_selectCurTx;
    *y1 = s_selectCurTy;
}

bool wetools_isSelectDragging(WarEditorContext* ctx)
{
    (void)ctx;
    return s_selectState == WE_SELECT_DRAG_RECT;
}

void wetools_deleteSelected(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx->map;
    if (!m || ctx->selectedEntities.count == 0) return;

    // Collect sorted ascending indices + entity snapshots for history.
    s32 regularCount  = 0;
    s32 goldmineCount = 0;
    for (s32 i = 0; i < ctx->selectedEntities.count; i++)
    {
        WarEntityId id = ctx->selectedEntities.items[i];
        if (id & 0x8000u) goldmineCount++;
        else              regularCount++;
    }

    s32*          regularIndices   = (regularCount  > 0) ? (s32*)wm_alloc((u32)regularCount  * sizeof(s32))          : NULL;
    s32*          goldmineIndices  = (goldmineCount > 0) ? (s32*)wm_alloc((u32)goldmineCount * sizeof(s32))          : NULL;
    WarLevelUnit* regularEntities  = (regularCount  > 0) ? (WarLevelUnit*)wm_alloc((u32)regularCount  * sizeof(WarLevelUnit)) : NULL;
    WarLevelUnit* goldmineEntities = (goldmineCount > 0) ? (WarLevelUnit*)wm_alloc((u32)goldmineCount * sizeof(WarLevelUnit)) : NULL;

    s32 rk = 0, gk = 0;
    for (s32 i = 0; i < ctx->selectedEntities.count; i++)
    {
        WarEntityId id = ctx->selectedEntities.items[i];
        if (id & 0x8000u)
            goldmineIndices[gk++] = (s32)(id & 0x7FFFu);
        else
            regularIndices[rk++]  = (s32)(id - 1u);
    }

    if (regularIndices)  wetools_sortIndicesAsc(regularIndices,  regularCount);
    if (goldmineIndices) wetools_sortIndicesAsc(goldmineIndices, goldmineCount);

    // Save entity snapshots in sorted order (before deletion).
    for (s32 i = 0; i < regularCount;  i++)
        regularEntities[i]  = m->startEntities[regularIndices[i]];
    for (s32 i = 0; i < goldmineCount; i++)
        goldmineEntities[i] = m->startGoldmines[goldmineIndices[i]];

    if (ctx->history)
    {
        WarEditorOp op;
        memset(&op, 0, sizeof(op));
        op.type                          = WE_OP_DELETE_BATCH;
        op.deleteBatch.regularIndices    = regularIndices;
        op.deleteBatch.regularEntities   = regularEntities;
        op.deleteBatch.regularCount      = regularCount;
        op.deleteBatch.goldmineIndices   = goldmineIndices;
        op.deleteBatch.goldmineEntities  = goldmineEntities;
        op.deleteBatch.goldmineCount     = goldmineCount;
        wehist_push(ctx->history, op);
        // Ownership transferred — do not free here.
    }
    else
    {
        if (regularIndices)   wm_free(regularIndices);
        if (regularEntities)  wm_free(regularEntities);
        if (goldmineIndices)  wm_free(goldmineIndices);
        if (goldmineEntities) wm_free(goldmineEntities);
    }

    // Mark which regular entities and goldmines to delete.
    bool deleteRegular[MAX_ENTITIES_COUNT];
    bool deleteGoldmine[MAX_CUSTOM_MAP_GOLDMINES_COUNT];
    memset(deleteRegular,  0, sizeof(deleteRegular));
    memset(deleteGoldmine, 0, sizeof(deleteGoldmine));

    for (s32 i = 0; i < ctx->selectedEntities.count; i++)
    {
        WarEntityId id = ctx->selectedEntities.items[i];
        if (id & 0x8000u)
        {
            u32 idx = (u32)(id & 0x7FFFu);
            if (idx < MAX_CUSTOM_MAP_GOLDMINES_COUNT)
                deleteGoldmine[idx] = true;
        }
        else
        {
            u32 idx = (u32)(id - 1u);
            if (idx < MAX_ENTITIES_COUNT)
                deleteRegular[idx] = true;
        }
    }

    // Compact startEntities in-place.
    u32 writeIdx = 0;
    for (u32 i = 0; i < m->startEntitiesCount; i++)
    {
        if (!deleteRegular[i])
        {
            m->startEntities[writeIdx] = m->startEntities[i];
            writeIdx++;
        }
    }
    m->startEntitiesCount = writeIdx;

    // Compact startGoldmines in-place.
    writeIdx = 0;
    for (u32 i = 0; i < m->startGoldminesCount; i++)
    {
        if (!deleteGoldmine[i])
        {
            m->startGoldmines[writeIdx] = m->startGoldmines[i];
            writeIdx++;
        }
    }
    m->startGoldminesCount = writeIdx;

    WarEntityIdListClear(&ctx->selectedEntities);
    ctx->unsavedChanges = true;
    ctx->minimapDirty = true;
}

void wetools_copySelected(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx->map;
    if (!m || ctx->selectedEntities.count <= 0)
    {
        ctx->clipboardCount = 0;
        SDL_strlcpy(ctx->statusText, "Copy failed: nothing selected.", sizeof(ctx->statusText));
        return;
    }

    s32 maxCopy = ctx->selectedEntities.count;
    if (maxCopy > WE_CLIPBOARD_MAX)
    {
        maxCopy = WE_CLIPBOARD_MAX;
    }

    s32 sumX = 0;
    s32 sumY = 0;
    s32 validCount = 0;

    for (s32 i = 0; i < maxCopy; i++)
    {
        WarEntityId id = ctx->selectedEntities.items[i];
        WarLevelUnit* lu = NULL;
        bool isGoldmine = false;

        if (id & 0x8000u)
        {
            u32 idx = (u32)(id & 0x7FFFu);
            if (idx < m->startGoldminesCount)
            {
                lu = &m->startGoldmines[idx];
                isGoldmine = true;
            }
        }
        else
        {
            u32 idx = (u32)(id - 1u);
            if (idx < m->startEntitiesCount)
            {
                lu = &m->startEntities[idx];
            }
        }

        if (!lu)
        {
            continue;
        }

        ctx->clipboardUnits[validCount] = *lu;
        ctx->clipboardIsGoldmine[validCount] = isGoldmine;
        sumX += (s32)lu->x;
        sumY += (s32)lu->y;
        validCount++;
    }

    if (validCount <= 0)
    {
        ctx->clipboardCount = 0;
        SDL_strlcpy(ctx->statusText, "Copy failed: selection is invalid.", sizeof(ctx->statusText));
        return;
    }

    s32 cx = sumX / validCount;
    s32 cy = sumY / validCount;

    for (s32 i = 0; i < validCount; i++)
    {
        ctx->clipboardDx[i] = (s16)((s32)ctx->clipboardUnits[i].x - cx);
        ctx->clipboardDy[i] = (s16)((s32)ctx->clipboardUnits[i].y - cy);
    }

    ctx->clipboardCount = validCount;
    ctx->pastePending = false;
    SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                 "Copied %d entr%s.",
                 validCount,
                 validCount == 1 ? "y" : "ies");
}

void wetools_beginPaste(WarEditorContext* ctx)
{
    if (!ctx || ctx->clipboardCount <= 0)
    {
        return;
    }

    ctx->activeTool = WE_TOOL_PLACE_ENTITY;

    if (ctx->isHoveringCanvas)
    {
        if (wetools_tryPasteAt(ctx, ctx->hoverTx, ctx->hoverTy))
        {
            SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                         "Pasted %d entr%s.",
                         ctx->clipboardCount,
                         ctx->clipboardCount == 1 ? "y" : "ies");
            return;
        }
    }

    ctx->pastePending = true;
    if (!ctx->isHoveringCanvas)
    {
        SDL_strlcpy(ctx->statusText,
                    "Paste armed: move over the canvas and left-click to place the clipboard.",
                    sizeof(ctx->statusText));
    }
}

bool wetools_canPaste(WarEditorContext* ctx)
{
    return ctx && ctx->clipboardCount > 0;
}

void wetools_update(WarEditorContext* ctx)
{
    ImGuiIO* io = igGetIO_Nil();

    // Complete fill drag if the mouse was released outside the canvas.
    if (s_fillDragging && !io->MouseDown[0])
        wetools_commitFill(ctx);

    // Complete select/move drag on mouse release (handles release outside canvas).
    if (ctx->activeTool == WE_TOOL_SELECT && !io->MouseDown[0])
    {
        if (s_selectState == WE_SELECT_DRAG_RECT)
            wetools_commitSelectDrag(ctx);
        else if (s_selectState == WE_SELECT_DRAG_MOVE)
            wetools_commitMove(ctx);
    }

    // Keyboard shortcuts
    if (!io->WantTextInput)
    {
        if (igIsKeyPressed_Bool(ImGuiKey_G, false))
            ctx->showGrid = !ctx->showGrid;
        if (igIsKeyPressed_Bool(ImGuiKey_P, false))
            ctx->showPassability = !ctx->showPassability;
        if (igIsKeyPressed_Bool(ImGuiKey_L, false))
            ctx->showStartLocation = !ctx->showStartLocation;

        // Delete selected entities
        if (igIsKeyPressed_Bool(ImGuiKey_Delete, false))
            wetools_deleteSelected(ctx);

        // Escape: deselect all
        if (igIsKeyPressed_Bool(ImGuiKey_Escape, false))
            WarEntityIdListClear(&ctx->selectedEntities);

        if (io->KeyCtrl && igIsKeyPressed_Bool(ImGuiKey_C, false))
            wetools_copySelected(ctx);

        if (io->KeyCtrl && igIsKeyPressed_Bool(ImGuiKey_V, false))
            wetools_beginPaste(ctx);
    }
}

void wetools_handleInput(WarEditorContext* ctx, s32 tx, s32 ty,
                         ImGuiIO* io, vec2 canvasOrigin)
{
    (void)canvasOrigin; // reserved for future tool-specific use

    WarEditorMap* m = ctx->map;
    if (!m) return;

    // Right-click: cancel placement tool and clear entity selection.
    if (io->MouseClicked[1])
    {
        if (ctx->activeTool == WE_TOOL_PLACE_ENTITY ||
            ctx->activeTool == WE_TOOL_SET_START ||
            ctx->activeTool == WE_TOOL_ERASE_ENTITY)
        {
            ctx->activeTool = WE_TOOL_SELECT;
        }
        WarEntityIdListClear(&ctx->selectedEntities);
        return;
    }

    switch (ctx->activeTool)
    {
        case WE_TOOL_PENCIL:
            if (io->MouseDown[0])
                wetools_paintTile(ctx, tx, ty);
            break;

        case WE_TOOL_ERASE:
            if (io->MouseDown[0])
                wetools_eraseTile(ctx, tx, ty);
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

        case WE_TOOL_PLACE_ENTITY:
        {
            if (io->MouseClicked[0])
            {
                if (ctx->pastePending)
                {
                    wetools_tryPasteAt(ctx, tx, ty);
                    break;
                }

                const WarUnitData* ud = wu_getUnitData(ctx->selectedUnitType);
                if (ud && ud->resourceIndex > 0)
                {
                    bool isGoldmine = (ctx->selectedUnitType == WAR_UNIT_GOLDMINE);

                    if (!isGoldmine)
                    {
                        if (wetools_canPlace(ctx, tx, ty, ud->sizex, ud->sizey) &&
                            m->startEntitiesCount < MAX_ENTITIES_COUNT)
                        {
                            WarLevelUnit* lu = &m->startEntities[m->startEntitiesCount];
                            lu->x            = (u8)tx;
                            lu->y            = (u8)ty;
                            lu->type         = ctx->selectedUnitType;
                            lu->player       = ctx->activePlayer;
                            lu->resourceKind = WAR_RESOURCE_NONE;
                            lu->amount       = 0;
                            m->startEntitiesCount++;
                            ctx->unsavedChanges = true;
                            ctx->minimapDirty = true;

                            if (ctx->history)
                            {
                                WarEditorOp op;
                                memset(&op, 0, sizeof(op));
                                op.type                   = WE_OP_PLACE_ENTITY;
                                op.placeEntity.entity     = *lu;
                                op.placeEntity.isGoldmine = false;
                                wehist_push(ctx->history, op);
                            }
                        }
                    }
                    else
                    {
                        if (wetools_canPlace(ctx, tx, ty, ud->sizex, ud->sizey) &&
                            m->startGoldminesCount < MAX_CUSTOM_MAP_GOLDMINES_COUNT)
                        {
                            WarLevelUnit* lu = &m->startGoldmines[m->startGoldminesCount];
                            lu->x            = (u8)tx;
                            lu->y            = (u8)ty;
                            lu->type         = ctx->selectedUnitType;
                            lu->player       = 0;
                            lu->resourceKind = WAR_RESOURCE_GOLD;
                            lu->amount       = 2500;
                            m->startGoldminesCount++;
                            ctx->unsavedChanges = true;
                            ctx->minimapDirty = true;

                            if (ctx->history)
                            {
                                WarEditorOp op;
                                memset(&op, 0, sizeof(op));
                                op.type                   = WE_OP_PLACE_ENTITY;
                                op.placeEntity.entity     = *lu;
                                op.placeEntity.isGoldmine = true;
                                wehist_push(ctx->history, op);
                            }
                        }
                    }
                }
            }
            break;
        }

        case WE_TOOL_SET_START:
            if (io->MouseClicked[0])
            {
                wetools_setStartLocation(ctx, tx, ty);
            }
            break;

        case WE_TOOL_ERASE_ENTITY:
            if (io->MouseDown[0])
            {
                wetools_eraseEntityAtTile(ctx, tx, ty);
            }
            break;

        case WE_TOOL_SELECT:
        {
            if (s_selectState == WE_SELECT_IDLE && io->MouseClicked[0])
            {
                WarEntityId hit = wetools_entityAtTile(ctx, tx, ty);
                if (hit)
                {
                    // Click on an entity: add to or start a new selection.
                    if (!WarEntityIdListContains(&ctx->selectedEntities, hit, we_equalsEntityId))
                    {
                        WarEntityIdListClear(&ctx->selectedEntities);
                        WarEntityIdListAdd(&ctx->selectedEntities, hit);
                    }
                    s_selectState   = WE_SELECT_DRAG_MOVE;
                    s_selectStartTx = tx;
                    s_selectStartTy = ty;
                    s_selectCurTx   = tx;
                    s_selectCurTy   = ty;
                    s_moveDtx       = 0;
                    s_moveDty       = 0;
                }
                else
                {
                    // Click on empty canvas: start rubber-band rect.
                    WarEntityIdListClear(&ctx->selectedEntities);
                    s_selectState   = WE_SELECT_DRAG_RECT;
                    s_selectStartTx = tx;
                    s_selectStartTy = ty;
                    s_selectCurTx   = tx;
                    s_selectCurTy   = ty;
                }
            }
            else if (s_selectState == WE_SELECT_DRAG_RECT && io->MouseDown[0])
            {
                s_selectCurTx = tx;
                s_selectCurTy = ty;
            }
            else if (s_selectState == WE_SELECT_DRAG_MOVE && io->MouseDown[0])
            {
                s_moveDtx     = tx - s_selectStartTx;
                s_moveDty     = ty - s_selectStartTy;
                s_selectCurTx = tx;
                s_selectCurTy = ty;
            }
            break;
        }
    }
}

void wetools_drawOverlay(WarEditorContext* ctx, ImDrawList* drawList,
                         vec2 canvasOrigin)
{
    f32 zoom = ctx->cameraZoom;
    f32 camX = ctx->cameraOffset.x;
    f32 camY = ctx->cameraOffset.y;

    // Fill-drag preview
    if (s_fillDragging && ctx->activeTool == WE_TOOL_FILL)
    {
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

    // Select drag-rect preview
    if (s_selectState == WE_SELECT_DRAG_RECT && ctx->activeTool == WE_TOOL_SELECT)
    {
        s32 x0 = s_selectStartTx < s_selectCurTx ? s_selectStartTx : s_selectCurTx;
        s32 y0 = s_selectStartTy < s_selectCurTy ? s_selectStartTy : s_selectCurTy;
        s32 x1 = s_selectStartTx < s_selectCurTx ? s_selectCurTx   : s_selectStartTx;
        s32 y1 = s_selectStartTy < s_selectCurTy ? s_selectCurTy   : s_selectStartTy;

        f32 sx0 = canvasOrigin.x + ((f32)(x0 * MEGA_TILE_WIDTH)       - camX) * zoom;
        f32 sy0 = canvasOrigin.y + ((f32)(y0 * MEGA_TILE_HEIGHT)      - camY) * zoom;
        f32 sx1 = canvasOrigin.x + ((f32)((x1 + 1) * MEGA_TILE_WIDTH) - camX) * zoom;
        f32 sy1 = canvasOrigin.y + ((f32)((y1 + 1) * MEGA_TILE_HEIGHT)- camY) * zoom;

        // Semi-transparent cyan fill + cyan border
        ImDrawList_AddRectFilled(drawList,
                                 (ImVec2_c){ sx0, sy0 },
                                 (ImVec2_c){ sx1, sy1 },
                                 0x40FFFF00u,  // ABGR: 0x40 alpha, cyan (R=0,G=FF,B=FF → ABGR 0x40FFFF00)
                                 0.0f, 0);
        ImDrawList_AddRect(drawList,
                           (ImVec2_c){ sx0, sy0 },
                           (ImVec2_c){ sx1, sy1 },
                           0xCFFFFF00u,  // ABGR: 0xCF alpha, cyan
                           0.0f, 1.5f, 0);
    }
}
