#pragma once

#include "war_editor.h"
#include "war_editor_map.h"

// ---------------------------------------------------------------------------
// WeAutotileGroup — identifies which auto-tiling tileset group a tile belongs to.
//
// passableData encoding used by the autotile system:
//   0   = passable terrain (generic)
//   1   = impassable terrain (generic)
//   2   = road presence
//   3   = wall presence
//   4   = ruin presence
//   128 = tree presence  (game convention from war_map.c)
//
// Dungeon tileset has no autotile groups; all functions return WE_AUTOTILE_NONE
// when ctx->map->tilesetType == MAP_TILESET_DUNGEON.
// ---------------------------------------------------------------------------

typedef enum
{
    WE_AUTOTILE_NONE  = 0,
    WE_AUTOTILE_TREES = 1,
    WE_AUTOTILE_ROADS = 2,
    WE_AUTOTILE_WALLS = 3,
    WE_AUTOTILE_RUINS = 4,
} WeAutotileGroup;

// Detect autotile group from ctx->selectedTileIndex and ctx->map->tilesetType.
// Iterates treesData / roadsData / wallsData / ruinsData to find a tile index match.
// Returns WE_AUTOTILE_NONE if dungeon tileset, no map, or no match found.
WeAutotileGroup weautotile_detectGroup(WarEditorContext* ctx);

// Get the autotile group of an existing tile at (tx, ty) from its passableData value.
// Returns WE_AUTOTILE_NONE if the tile has no autotile presence.
WeAutotileGroup weautotile_groupAtTile(WarEditorMap* m, s32 tx, s32 ty);

// Mark tile (tx, ty) as present in the given group.
// Sets passableData only; does NOT set visualData (call weautotile_recompute for that).
void weautotile_markPresence(WarEditorMap* m, s32 tx, s32 ty, WeAutotileGroup group);

// Clear tile (tx, ty) — sets visualData = 0 and passableData = 0.
void weautotile_clearPresence(WarEditorMap* m, s32 tx, s32 ty);

// Recompute visualData for tile (tx, ty) using a neighbor bitmask lookup.
// Only acts if the tile already has presence for the given group (i.e.,
// weautotile_groupAtTile returns the same group).
// Uses game-identical bitmask encoding:
//   Trees/Ruins — 8-directional; out-of-bounds counts as present.
//   Roads/Walls — 4-directional; out-of-bounds counts as connected.
void weautotile_recompute(WarEditorContext* ctx, s32 tx, s32 ty, WeAutotileGroup group);
