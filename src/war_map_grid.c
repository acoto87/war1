#include "war_map_grid.h"
#include "war_entities.h"
#include "war_state_machine.h"
#include "war_units.h"

void wgrid_clear(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    WarMapGrid* grid = &map->grid;

    // Initialize the head array to -1 (indicating empty)
    for (s32 i = 0; i < MAP_GRID_CELLS; i++)
    {
        grid->head[i] = -1;
    }

    // Initialize the next array to -1 (indicating no next entity)
    for (s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    {
        grid->next[i] = -1;
    }

    grid->dirty = true;
}

void wgrid_build(WarContext* context)
{
    TracyCZoneN(ctx, "wgrid_build", 1);

    WarMap* map = context->map;
    assert(map);

    WarMapGrid* grid = &map->grid;

    // Initialize the head array to -1 (indicating empty)
    for (s32 i = 0; i < MAP_GRID_CELLS; i++)
    {
        grid->head[i] = -1;
    }

    // Initialize the next array to -1 (indicating no next entity)
    for (s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    {
        grid->next[i] = -1;
    }

    WarEntityManager* entityManager = &map->entityManager;
    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);

    for (s32 unitIndex = 0; unitIndex < units->count; unitIndex++)
    {
        WarEntity* entity = units->items[unitIndex];
        if (!entity)
            continue;

        s32 i = (s32)(entity - entityManager->entities);
        assert(inRange(i, 0, MAX_ENTITIES_COUNT));

        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        if (!transform)
        {
            continue; // Skip entities without a transform component
        }

        if (wst_isGoingToDie(context, entity))
        {
            continue; // Skip units that are about to be removed this frame.
        }

        if (wst_isCollapsing(context, entity) || wst_isGoingToCollapse(context, entity))
        {
            continue; // Skip collapsed and collapsing building
        }

        vec2 entityTile = wu_getUnitCenterPosition(context, entity, true);
        vec2 gridTile = wgrid_tileFromMapTile(entityTile);
        s32 cellIndex = wgrid_getTileIndex(gridTile);

        // Ensure the cell index is within bounds
        if (cellIndex < 0)
        {
            continue;
        }

        // Insert the entity into the linked list for the corresponding grid cell
        grid->next[i] = grid->head[cellIndex];
        grid->head[cellIndex] = i;
    }

    grid->dirty = false;

    TracyCZoneEnd(ctx);
}

void wgrid_rebuildIfDirty(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    if (map->grid.dirty)
    {
        wgrid_build(context);
    }
}

vec2 wgrid_tileFromMapTile(vec2 tilePosition)
{
    // Convert map tile coordinates to grid tile coordinates
    s32 gridX = (s32)(tilePosition.x / MAP_GRID_TILE_SIZE);
    s32 gridY = (s32)(tilePosition.y / MAP_GRID_TILE_SIZE);

    // Return the grid tile position as a vec2
    return vec2i(gridX, gridY);
}

s32 wgrid_getTileIndex(vec2 tile)
{
    // Calculate the grid tile index based on the grid tile coordinates
    s32 gridX = (s32)tile.x;
    s32 gridY = (s32)tile.y;

    // Ensure the grid coordinates are within bounds
    if (gridX < 0 || gridX >= MAP_GRID_TILES_WIDTH || gridY < 0 || gridY >= MAP_GRID_TILES_HEIGHT)
    {
        return -1; // Return -1 for out-of-bounds tiles
    }

    // Calculate and return the grid tile index
    return gridY * MAP_GRID_TILES_WIDTH + gridX;
}
