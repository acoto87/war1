#pragma once

#include "war.h"

#define MAP_GRID_TILE_SIZE 4
#define MAP_GRID_TILES_WIDTH (MAP_TILES_WIDTH/MAP_GRID_TILE_SIZE)
#define MAP_GRID_TILES_HEIGHT (MAP_TILES_HEIGHT/MAP_GRID_TILE_SIZE)
#define MAP_GRID_CELLS (MAP_GRID_TILES_WIDTH * MAP_GRID_TILES_HEIGHT)

struct _WarMapGrid
{
    // true when entity positions have changed and the grid needs to be rebuilt before the next query
    bool dirty;

    // head of the linked list for each cell in the spatial grid
    s32 head[MAP_GRID_CELLS];

    // next array for the linked list of entities in the spatial grid
    s32 next[MAX_ENTITIES_COUNT];
};

void wgrid_clear(WarContext* context);
void wgrid_build(WarContext* context);
void wgrid_rebuildIfDirty(WarContext* context);
vec2 wgrid_tileFromMapTile(vec2 tilePosition);
s32 wgrid_getTileIndex(vec2 tile);
