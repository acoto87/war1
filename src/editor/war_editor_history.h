#pragma once

#include "war_editor_map.h"

// ---------------------------------------------------------------------------
// — Undo / Redo history stack
//
// The stack holds up to WE_HISTORY_MAX operations.  Each operation stores
// enough data to apply the change forward (redo) or in reverse (undo).
//
// Memory policy:
//   WePaintTileOp    — fully inline (3×3 neighborhood snapshots, 9 tiles each)
//   WeFillRegionOp   — old/new visual+passable arrays heap-allocated via wm_alloc
//   WePlaceEntityOp  — fully inline (one WarLevelUnit + isGoldmine flag)
//   WeDeleteBatchOp  — sorted index arrays + entity arrays heap-allocated
//   WeMoveEntityOp   — sorted index arrays heap-allocated; delta inline
//
// wehist_clear frees all heap allocations and resets count/cursor to 0.
// Call it on new-map and after load-map to start fresh.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Operation type
// ---------------------------------------------------------------------------
typedef enum
{
    WE_OP_NONE         = 0,
    WE_OP_PAINT_TILE   = 1,
    WE_OP_FILL_REGION  = 2,
    WE_OP_PLACE_ENTITY = 3,
    WE_OP_DELETE_BATCH = 4,
    WE_OP_MOVE_BATCH   = 5,
    WE_OP_EDIT_ENTITY  = 6,
    WE_OP_SET_START    = 7,
    WE_OP_EDIT_MAP     = 8,
    WE_OP_EDIT_MAP_NAME = 9,
} WarEditorOpType;

#define WE_HISTORY_MAP_EDIT_MAX_BYTES 512

// ---------------------------------------------------------------------------
// Per-operation data structs
// ---------------------------------------------------------------------------

// 3×3 neighborhood snapshot centered on (cx, cy).
// k = (ny - cy + 1) * 3 + (nx - cx + 1), so:
//   k=0: (cx-1,cy-1)  k=1: (cx,cy-1)  k=2: (cx+1,cy-1)
//   k=3: (cx-1,cy  )  k=4: (cx,cy  )  k=5: (cx+1,cy  )
//   k=6: (cx-1,cy+1)  k=7: (cx,cy+1)  k=8: (cx+1,cy+1)
// Out-of-bounds neighbors are clamped to the nearest valid tile.
typedef struct
{
    s32 cx, cy;
    u16 oldVisual[9];
    u16 oldPassable[9];
    u16 newVisual[9];
    u16 newPassable[9];
} WePaintTileOp;

// Bounding-box snapshot: the fill region plus a 1-tile border, clamped to map.
// bw = bx1 - bx0 + 1, bh = by1 - by0 + 1; array length = bw * bh.
typedef struct
{
    s32  bx0, by0, bx1, by1;
    s32  bw,  bh;
    u16* oldVisual;
    u16* oldPassable;
    u16* newVisual;
    u16* newPassable;
} WeFillRegionOp;

// Single entity placement snapshot.
typedef struct
{
    WarLevelUnit entity;
    bool         isGoldmine;
} WePlaceEntityOp;

// Batch delete snapshot: sorted-ascending index arrays + saved entity data.
// Undo re-inserts entities at their original positions (ascending order).
// Redo removes entities at those positions (descending order).
typedef struct
{
    s32*          regularIndices;
    WarLevelUnit* regularEntities;
    s32           regularCount;
    s32*          goldmineIndices;
    WarLevelUnit* goldmineEntities;
    s32           goldmineCount;
} WeDeleteBatchOp;

// Batch move snapshot: sorted-ascending index arrays + tile delta.
typedef struct
{
    s32* regularIndices;
    s32  regularCount;
    s32* goldmineIndices;
    s32  goldmineCount;
    s32  dtx, dty;
} WeMoveEntityOp;

// Single entity field edit snapshot (used by Inspector edits).
typedef struct
{
    s32          index;
    bool         isGoldmine;
    WarLevelUnit oldEntity;
    WarLevelUnit newEntity;
} WeEditEntityOp;

// Map start-location change snapshot.
typedef struct
{
    u16 oldX;
    u16 oldY;
    u16 newX;
    u16 newY;
} WeSetStartOp;

// Generic map-field edit snapshot.
// offset is the byte offset from the beginning of WarEditorMap.
typedef struct
{
    u32 offset;
    u16 size;
    u8  oldData[WE_HISTORY_MAP_EDIT_MAX_BYTES];
    u8  newData[WE_HISTORY_MAP_EDIT_MAX_BYTES];
} WeEditMapOp;

typedef struct
{
    char oldName[64];
    char newName[64];
} WeEditMapNameOp;

// ---------------------------------------------------------------------------
// Operation union
// ---------------------------------------------------------------------------
typedef struct
{
    WarEditorOpType type;
    union
    {
        WePaintTileOp   paintTile;
        WeFillRegionOp  fillRegion;
        WePlaceEntityOp placeEntity;
        WeDeleteBatchOp deleteBatch;
        WeMoveEntityOp  moveBatch;
        WeEditEntityOp  editEntity;
        WeSetStartOp    setStart;
        WeEditMapOp     editMap;
        WeEditMapNameOp editMapName;
    };
} WarEditorOp;

// ---------------------------------------------------------------------------
// History stack
// ---------------------------------------------------------------------------
#define WE_HISTORY_MAX 512

struct _WarEditorHistory
{
    WarEditorOp ops[WE_HISTORY_MAX];
    s32         count;
    s32         cursor;
};

// ---------------------------------------------------------------------------
// Neighborhood helpers (also used by war_editor_tools.c)
// ---------------------------------------------------------------------------

// Capture the visual and passable values of the 3×3 neighborhood around (cx, cy).
// Out-of-bounds neighbors are clamped to the nearest in-bounds tile.
void wehist_captureNeighborhood(WarEditorMap* m, s32 cx, s32 cy,
                                 u16* outVisual, u16* outPassable);

// Restore the visual and passable values of the 3×3 neighborhood around (cx, cy)
// from previously captured arrays.  Out-of-bounds entries are silently skipped.
void wehist_restoreNeighborhood(WarEditorMap* m, s32 cx, s32 cy,
                                 const u16* visual, const u16* passable);

// ---------------------------------------------------------------------------
// Stack operations
// ---------------------------------------------------------------------------

// Push op onto the history stack.
// Truncates any forward (redo) tail first, freeing heap allocations.
// When the stack is full, evicts the oldest entry (also freeing its heap data).
void wehist_push(WarEditorHistory* h, WarEditorOp op);

// Apply the inverse of ops[--cursor] to map.  No-op when cursor == 0.
void wehist_undo(WarEditorHistory* h, WarEditorContext* ctx);

// Re-apply ops[cursor++] to map.  No-op when cursor == count.
void wehist_redo(WarEditorHistory* h, WarEditorContext* ctx);

// Move the history cursor to targetCursor by applying undo/redo steps.
// targetCursor is clamped to [0, count].
void wehist_seek(WarEditorHistory* h, WarEditorContext* ctx, s32 targetCursor);

// Free all heap-allocated op data and reset count and cursor to 0.
void wehist_clear(WarEditorHistory* h);
