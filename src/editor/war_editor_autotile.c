#include "war_editor_autotile.h"

// ---------------------------------------------------------------------------
// Neighbor direction tables — identical to the game's tile-type lookup code
// ---------------------------------------------------------------------------

// 8-directional (trees + ruins): NW, N, NE, E, SE, S, SW, W
static const s32 s_dir8X[8] = { -1,  0,  1, 1, 1, 0, -1, -1 };
static const s32 s_dir8Y[8] = { -1, -1, -1, 0, 1, 1,  1,  0 };

// 4-directional (roads + walls): N, E, S, W
static const s32 s_dir4X[4] = {  0, 1,  0, -1 };
static const s32 s_dir4Y[4] = { -1, 0,  1,  0 };

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Decode a passableData value to the autotile group it represents.
static WeAutotileGroup weautotile_groupFromPassable(u16 passable)
{
    if (passable == 128) return WE_AUTOTILE_TREES;
    if (passable == 2)   return WE_AUTOTILE_ROADS;
    if (passable == 3)   return WE_AUTOTILE_WALLS;
    if (passable == 4)   return WE_AUTOTILE_RUINS;
    return WE_AUTOTILE_NONE;
}

// ---------------------------------------------------------------------------
// weautotile_detectGroup
// ---------------------------------------------------------------------------

WeAutotileGroup weautotile_detectGroup(WarEditorContext* ctx)
{
    if (!ctx->map) return WE_AUTOTILE_NONE;

    u16 tilesetType = ctx->map->tilesetType;
    if (tilesetType == MAP_TILESET_DUNGEON) return WE_AUTOTILE_NONE;

    u16  sel      = ctx->selectedTileIndex;
    bool isForest = (tilesetType == MAP_TILESET_FOREST);

    // Check trees — skip index 0 (WAR_TREE_NONE maps to tile 0 = blank terrain)
    s32 treeCount = (s32)arrayLength(treesData);
    for (s32 i = 1; i < treeCount; i++)
    {
        s32 ti = isForest ? treesData[i].tileIndexForest : treesData[i].tileIndexSwamp;
        if ((u16)ti == sel) return WE_AUTOTILE_TREES;
    }

    // Check roads
    s32 roadCount = (s32)arrayLength(roadsData);
    for (s32 i = 0; i < roadCount; i++)
    {
        s32 ti = isForest ? roadsData[i].tileIndexForest : roadsData[i].tileIndexSwamp;
        if ((u16)ti == sel) return WE_AUTOTILE_ROADS;
    }

    // Check walls
    s32 wallCount = (s32)arrayLength(wallsData);
    for (s32 i = 0; i < wallCount; i++)
    {
        s32 ti = isForest ? wallsData[i].tileForest : wallsData[i].tileSwamp;
        if ((u16)ti == sel) return WE_AUTOTILE_WALLS;
    }

    // Check ruins
    s32 ruinCount = (s32)arrayLength(ruinsData);
    for (s32 i = 0; i < ruinCount; i++)
    {
        s32 ti = isForest ? ruinsData[i].tileIndexForest : ruinsData[i].tileIndexSwamp;
        if ((u16)ti == sel) return WE_AUTOTILE_RUINS;
    }

    return WE_AUTOTILE_NONE;
}

// ---------------------------------------------------------------------------
// weautotile_groupAtTile
// ---------------------------------------------------------------------------

WeAutotileGroup weautotile_groupAtTile(WarEditorMap* m, s32 tx, s32 ty)
{
    if (!m) return WE_AUTOTILE_NONE;
    s32 idx = ty * MAP_TILES_WIDTH + tx;
    return weautotile_groupFromPassable(m->passableData[idx]);
}

// ---------------------------------------------------------------------------
// weautotile_markPresence
// ---------------------------------------------------------------------------

void weautotile_markPresence(WarEditorMap* m, s32 tx, s32 ty, WeAutotileGroup group)
{
    if (!m) return;
    s32 idx = ty * MAP_TILES_WIDTH + tx;
    switch (group)
    {
        case WE_AUTOTILE_TREES: m->passableData[idx] = 128; break;
        case WE_AUTOTILE_ROADS: m->passableData[idx] = 2;   break;
        case WE_AUTOTILE_WALLS: m->passableData[idx] = 3;   break;
        case WE_AUTOTILE_RUINS: m->passableData[idx] = 4;   break;
        default:                                             break;
    }
}

// ---------------------------------------------------------------------------
// weautotile_clearPresence
// ---------------------------------------------------------------------------

void weautotile_clearPresence(WarEditorMap* m, s32 tx, s32 ty)
{
    if (!m) return;
    s32 idx = ty * MAP_TILES_WIDTH + tx;
    m->visualData[idx]   = 0;
    m->passableData[idx] = 0;
}

// ---------------------------------------------------------------------------
// weautotile_recompute
// ---------------------------------------------------------------------------

void weautotile_recompute(WarEditorContext* ctx, s32 tx, s32 ty, WeAutotileGroup group)
{
    if (!ctx->map) return;

    WarEditorMap* m = ctx->map;
    s32 idx = ty * MAP_TILES_WIDTH + tx;

    // Only recompute tiles that actually belong to this group.
    if (weautotile_groupFromPassable(m->passableData[idx]) != group) return;

    u16  tilesetType = m->tilesetType;
    bool isForest    = (tilesetType == MAP_TILESET_FOREST);

    if (group == WE_AUTOTILE_TREES || group == WE_AUTOTILE_RUINS)
    {
        // 8-directional bitmask
        s32 bitmask = 0;
        for (s32 d = 0; d < 8; d++)
        {
            s32 nx = tx + s_dir8X[d];
            s32 ny = ty + s_dir8Y[d];

            // Out-of-bounds counts as present (identical to game logic)
            if (nx < 0 || nx >= MAP_TILES_WIDTH || ny < 0 || ny >= MAP_TILES_HEIGHT)
            {
                bitmask |= (1 << d);
            }
            else
            {
                u16 np = m->passableData[ny * MAP_TILES_WIDTH + nx];
                if (weautotile_groupFromPassable(np) == group)
                    bitmask |= (1 << d);
            }
        }

        if (group == WE_AUTOTILE_TREES)
        {
            WarTreeTileType  ttype = treeTileTypeMap[bitmask];
            const WarTreeData* td  = wu_getTreeData(ttype);
            s32 tileIdx = isForest ? td->tileIndexForest : td->tileIndexSwamp;
            m->visualData[idx] = (u16)tileIdx;
        }
        else // WE_AUTOTILE_RUINS
        {
            WarRuinPieceType ruinType = ruinTileTypeMap[bitmask];
            if (ruinType == WAR_RUIN_PIECE_NONE)
            {
                // Bitmask maps to no valid ruin shape; show blank visual.
                m->visualData[idx] = 0;
                return;
            }
            const WarRuinData* rd = wu_getRuinData(ruinType);
            s32 tileIdx = isForest ? rd->tileIndexForest : rd->tileIndexSwamp;
            m->visualData[idx] = (u16)tileIdx;
        }
    }
    else // WE_AUTOTILE_ROADS or WE_AUTOTILE_WALLS
    {
        // 4-directional bitmask; out-of-bounds counts as connected
        s32 bitmask = 0;
        for (s32 d = 0; d < 4; d++)
        {
            s32 nx = tx + s_dir4X[d];
            s32 ny = ty + s_dir4Y[d];

            if (nx < 0 || nx >= MAP_TILES_WIDTH || ny < 0 || ny >= MAP_TILES_HEIGHT)
            {
                bitmask |= (1 << d);
            }
            else
            {
                u16 np = m->passableData[ny * MAP_TILES_WIDTH + nx];
                if (weautotile_groupFromPassable(np) == group)
                    bitmask |= (1 << d);
            }
        }

        if (group == WE_AUTOTILE_ROADS)
        {
            WarRoadPieceType   rtype = roadTileTypeMap[bitmask];
            const WarRoadData* rd    = wu_getRoadData(rtype);
            s32 tileIdx = isForest ? rd->tileIndexForest : rd->tileIndexSwamp;
            m->visualData[idx] = (u16)tileIdx;
        }
        else // WE_AUTOTILE_WALLS
        {
            WarWallPieceType   wtype = wallTileTypeMap[bitmask];
            const WarWallData* wd    = wu_getWallData(wtype);
            s32 tileIdx = isForest ? wd->tileForest : wd->tileSwamp;
            m->visualData[idx] = (u16)tileIdx;
        }
    }
}
