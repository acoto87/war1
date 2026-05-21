#include "war_editor_history.h"

// ---------------------------------------------------------------------------
// Internal: free heap-allocated data in a single op (no-op for inline ops).
// ---------------------------------------------------------------------------
static void wehist_freeOp(WarEditorOp* op)
{
    switch (op->type)
    {
        case WE_OP_FILL_REGION:
            if (op->fillRegion.oldVisual)    wm_free(op->fillRegion.oldVisual);
            if (op->fillRegion.oldPassable)  wm_free(op->fillRegion.oldPassable);
            if (op->fillRegion.newVisual)    wm_free(op->fillRegion.newVisual);
            if (op->fillRegion.newPassable)  wm_free(op->fillRegion.newPassable);
            break;

        case WE_OP_DELETE_BATCH:
            if (op->deleteBatch.regularIndices)   wm_free(op->deleteBatch.regularIndices);
            if (op->deleteBatch.regularEntities)  wm_free(op->deleteBatch.regularEntities);
            if (op->deleteBatch.goldmineIndices)  wm_free(op->deleteBatch.goldmineIndices);
            if (op->deleteBatch.goldmineEntities) wm_free(op->deleteBatch.goldmineEntities);
            break;

        case WE_OP_MOVE_BATCH:
            if (op->moveBatch.regularIndices)  wm_free(op->moveBatch.regularIndices);
            if (op->moveBatch.goldmineIndices) wm_free(op->moveBatch.goldmineIndices);
            break;

        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// Neighborhood capture / restore
//
// k layout for center (cx, cy):
//   k = (ny - cy + 1) * 3 + (nx - cx + 1)
//   k=0: (cx-1,cy-1)  k=1: (cx,cy-1)  k=2: (cx+1,cy-1)
//   k=3: (cx-1,cy  )  k=4: (cx,cy  )  k=5: (cx+1,cy  )
//   k=6: (cx-1,cy+1)  k=7: (cx,cy+1)  k=8: (cx+1,cy+1)
// ---------------------------------------------------------------------------
void wehist_captureNeighborhood(WarEditorMap* m, s32 cx, s32 cy,
                                 u16* outVisual, u16* outPassable)
{
    for (s32 k = 0; k < 9; k++)
    {
        s32 nx = cx + (k % 3) - 1;
        s32 ny = cy + (k / 3) - 1;

        if (nx < 0) nx = 0;
        if (ny < 0) ny = 0;
        if (nx >= MAP_TILES_WIDTH)  nx = MAP_TILES_WIDTH  - 1;
        if (ny >= MAP_TILES_HEIGHT) ny = MAP_TILES_HEIGHT - 1;

        s32 idx          = ny * MAP_TILES_WIDTH + nx;
        outVisual[k]     = m->visualData[idx];
        outPassable[k]   = m->passableData[idx];
    }
}

void wehist_restoreNeighborhood(WarEditorMap* m, s32 cx, s32 cy,
                                 const u16* visual, const u16* passable)
{
    for (s32 k = 0; k < 9; k++)
    {
        s32 nx = cx + (k % 3) - 1;
        s32 ny = cy + (k / 3) - 1;

        if (nx < 0) nx = 0;
        if (ny < 0) ny = 0;
        if (nx >= MAP_TILES_WIDTH)  nx = MAP_TILES_WIDTH  - 1;
        if (ny >= MAP_TILES_HEIGHT) ny = MAP_TILES_HEIGHT - 1;

        s32 idx                = ny * MAP_TILES_WIDTH + nx;
        m->visualData[idx]     = visual[k];
        m->passableData[idx]   = passable[k];
    }
}

// ---------------------------------------------------------------------------
// Internal: apply a fill-region snapshot (forward or backward).
// ---------------------------------------------------------------------------
static void wehist_applyFillSnapshot(WarEditorMap* m,
                                      s32 bx0, s32 by0, s32 bx1, s32 by1, s32 bw,
                                      const u16* visual, const u16* passable)
{
    for (s32 y = by0; y <= by1; y++)
    {
        for (s32 x = bx0; x <= bx1; x++)
        {
            s32 k                  = (y - by0) * bw + (x - bx0);
            s32 idx                = y * MAP_TILES_WIDTH + x;
            m->visualData[idx]     = visual[k];
            m->passableData[idx]   = passable[k];
        }
    }
}

// ---------------------------------------------------------------------------
// Internal: apply a tile-delta (clamped) to entities at the given indices.
// ---------------------------------------------------------------------------
static void wehist_applyMoveDelta(WarEditorMap* m,
                                   const s32* regularIndices, s32 regularCount,
                                   const s32* goldmineIndices, s32 goldmineCount,
                                   s32 dtx, s32 dty)
{
    for (s32 i = 0; i < regularCount; i++)
    {
        s32 idx = regularIndices[i];
        if (idx < 0 || (u32)idx >= m->startEntitiesCount) continue;
        WarLevelUnit* lu = &m->startEntities[idx];
        s32 nx = (s32)lu->x + dtx;
        s32 ny = (s32)lu->y + dty;
        if (nx < 0) nx = 0;
        if (ny < 0) ny = 0;
        if (nx >= MAP_TILES_WIDTH)  nx = MAP_TILES_WIDTH  - 1;
        if (ny >= MAP_TILES_HEIGHT) ny = MAP_TILES_HEIGHT - 1;
        lu->x = (u8)nx;
        lu->y = (u8)ny;
    }

    for (s32 i = 0; i < goldmineCount; i++)
    {
        s32 idx = goldmineIndices[i];
        if (idx < 0 || (u32)idx >= m->startGoldminesCount) continue;
        WarLevelUnit* lu = &m->startGoldmines[idx];
        s32 nx = (s32)lu->x + dtx;
        s32 ny = (s32)lu->y + dty;
        if (nx < 0) nx = 0;
        if (ny < 0) ny = 0;
        if (nx >= MAP_TILES_WIDTH)  nx = MAP_TILES_WIDTH  - 1;
        if (ny >= MAP_TILES_HEIGHT) ny = MAP_TILES_HEIGHT - 1;
        lu->x = (u8)nx;
        lu->y = (u8)ny;
    }
}

// ---------------------------------------------------------------------------
// Internal: re-insert entities at their original ascending positions.
// Used by undo of WE_OP_DELETE_BATCH.
// ---------------------------------------------------------------------------
static void wehist_reinsertRegular(WarEditorMap* m,
                                    const s32* indices,
                                    const WarLevelUnit* entities,
                                    s32 count)
{
    for (s32 i = 0; i < count; i++)
    {
        s32 idx = indices[i];
        if (idx < 0 || idx > (s32)m->startEntitiesCount)
            idx = (s32)m->startEntitiesCount;

        if (m->startEntitiesCount < MAX_ENTITIES_COUNT)
        {
            u32 cnt = m->startEntitiesCount;
            memmove(&m->startEntities[idx + 1],
                    &m->startEntities[idx],
                    sizeof(WarLevelUnit) * ((u32)cnt - (u32)idx));
            m->startEntities[idx] = entities[i];
            m->startEntitiesCount++;
        }
    }
}

static void wehist_reinsertGoldmine(WarEditorMap* m,
                                     const s32* indices,
                                     const WarLevelUnit* entities,
                                     s32 count)
{
    for (s32 i = 0; i < count; i++)
    {
        s32 idx = indices[i];
        if (idx < 0 || idx > (s32)m->startGoldminesCount)
            idx = (s32)m->startGoldminesCount;

        if (m->startGoldminesCount < MAX_CUSTOM_MAP_GOLDMINES_COUNT)
        {
            u32 cnt = m->startGoldminesCount;
            memmove(&m->startGoldmines[idx + 1],
                    &m->startGoldmines[idx],
                    sizeof(WarLevelUnit) * ((u32)cnt - (u32)idx));
            m->startGoldmines[idx] = entities[i];
            m->startGoldminesCount++;
        }
    }
}

// ---------------------------------------------------------------------------
// Internal: remove entities at descending-sorted positions.
// Used by redo of WE_OP_DELETE_BATCH.  Indices array is ascending, so we
// iterate from the end to the start.
// ---------------------------------------------------------------------------
static void wehist_removeRegular(WarEditorMap* m, const s32* indices, s32 count)
{
    for (s32 i = count - 1; i >= 0; i--)
    {
        s32 idx = indices[i];
        if (idx < 0 || (u32)idx >= m->startEntitiesCount) continue;
        u32 cnt = m->startEntitiesCount;
        memmove(&m->startEntities[idx],
                &m->startEntities[idx + 1],
                sizeof(WarLevelUnit) * ((u32)cnt - (u32)idx - 1u));
        m->startEntitiesCount--;
    }
}

static void wehist_removeGoldmine(WarEditorMap* m, const s32* indices, s32 count)
{
    for (s32 i = count - 1; i >= 0; i--)
    {
        s32 idx = indices[i];
        if (idx < 0 || (u32)idx >= m->startGoldminesCount) continue;
        u32 cnt = m->startGoldminesCount;
        memmove(&m->startGoldmines[idx],
                &m->startGoldmines[idx + 1],
                sizeof(WarLevelUnit) * ((u32)cnt - (u32)idx - 1u));
        m->startGoldminesCount--;
    }
}

// ---------------------------------------------------------------------------
// Internal: apply (or reverse) a single operation to the map.
// direction: +1 = forward (redo), -1 = backward (undo)
// ---------------------------------------------------------------------------
static void wehist_applyOp(WarEditorOp* op, WarEditorMap* m, s32 direction)
{
    switch (op->type)
    {
        case WE_OP_PAINT_TILE:
        {
            const u16* vis  = (direction > 0) ? op->paintTile.newVisual   : op->paintTile.oldVisual;
            const u16* pass = (direction > 0) ? op->paintTile.newPassable : op->paintTile.oldPassable;
            wehist_restoreNeighborhood(m, op->paintTile.cx, op->paintTile.cy, vis, pass);
            break;
        }

        case WE_OP_FILL_REGION:
        {
            const u16* vis  = (direction > 0) ? op->fillRegion.newVisual   : op->fillRegion.oldVisual;
            const u16* pass = (direction > 0) ? op->fillRegion.newPassable : op->fillRegion.oldPassable;
            wehist_applyFillSnapshot(m,
                op->fillRegion.bx0, op->fillRegion.by0,
                op->fillRegion.bx1, op->fillRegion.by1,
                op->fillRegion.bw,
                vis, pass);
            break;
        }

        case WE_OP_PLACE_ENTITY:
        {
            if (direction > 0)
            {
                // Redo: re-append the entity.
                if (op->placeEntity.isGoldmine)
                {
                    if (m->startGoldminesCount < MAX_CUSTOM_MAP_GOLDMINES_COUNT)
                    {
                        m->startGoldmines[m->startGoldminesCount] = op->placeEntity.entity;
                        m->startGoldminesCount++;
                    }
                }
                else
                {
                    if (m->startEntitiesCount < MAX_ENTITIES_COUNT)
                    {
                        m->startEntities[m->startEntitiesCount] = op->placeEntity.entity;
                        m->startEntitiesCount++;
                    }
                }
            }
            else
            {
                // Undo: remove the last-placed entity (it was appended at the end).
                if (op->placeEntity.isGoldmine)
                {
                    if (m->startGoldminesCount > 0)
                        m->startGoldminesCount--;
                }
                else
                {
                    if (m->startEntitiesCount > 0)
                        m->startEntitiesCount--;
                }
            }
            break;
        }

        case WE_OP_DELETE_BATCH:
        {
            if (direction > 0)
            {
                // Redo: re-delete (remove in descending order).
                wehist_removeRegular(m, op->deleteBatch.regularIndices,
                                     op->deleteBatch.regularCount);
                wehist_removeGoldmine(m, op->deleteBatch.goldmineIndices,
                                      op->deleteBatch.goldmineCount);
            }
            else
            {
                // Undo: re-insert in ascending order.
                wehist_reinsertRegular(m, op->deleteBatch.regularIndices,
                                       op->deleteBatch.regularEntities,
                                       op->deleteBatch.regularCount);
                wehist_reinsertGoldmine(m, op->deleteBatch.goldmineIndices,
                                        op->deleteBatch.goldmineEntities,
                                        op->deleteBatch.goldmineCount);
            }
            break;
        }

        case WE_OP_MOVE_BATCH:
        {
            s32 dtx = (direction > 0) ?  op->moveBatch.dtx : -op->moveBatch.dtx;
            s32 dty = (direction > 0) ?  op->moveBatch.dty : -op->moveBatch.dty;
            wehist_applyMoveDelta(m,
                op->moveBatch.regularIndices,  op->moveBatch.regularCount,
                op->moveBatch.goldmineIndices, op->moveBatch.goldmineCount,
                dtx, dty);
            break;
        }

        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// Stack operations
// ---------------------------------------------------------------------------

void wehist_push(WarEditorHistory* h, WarEditorOp op)
{
    if (!h) return;

    // Truncate any redo tail, freeing heap data.
    if (h->cursor < h->count)
    {
        for (s32 i = h->cursor; i < h->count; i++)
            wehist_freeOp(&h->ops[i]);
        h->count = h->cursor;
    }

    // When full, evict the oldest entry.
    if (h->count == WE_HISTORY_MAX)
    {
        wehist_freeOp(&h->ops[0]);
        memmove(&h->ops[0], &h->ops[1],
                sizeof(WarEditorOp) * (WE_HISTORY_MAX - 1));
        h->count--;
        h->cursor--;
    }

    h->ops[h->count] = op;
    h->count++;
    h->cursor++;
}

void wehist_undo(WarEditorHistory* h, WarEditorMap* m)
{
    if (!h || !m || h->cursor == 0) return;

    h->cursor--;
    wehist_applyOp(&h->ops[h->cursor], m, -1);
}

void wehist_redo(WarEditorHistory* h, WarEditorMap* m)
{
    if (!h || !m || h->cursor == h->count) return;

    wehist_applyOp(&h->ops[h->cursor], m, +1);
    h->cursor++;
}

void wehist_clear(WarEditorHistory* h)
{
    if (!h) return;

    for (s32 i = 0; i < h->count; i++)
        wehist_freeOp(&h->ops[i]);

    h->count  = 0;
    h->cursor = 0;
}
