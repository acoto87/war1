#pragma once

inline void addEntityToSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    // subtitute this with a set data structure that doesn't allow duplicates
    if (!WarEntityIdListContains(&map->selectedEntities, id))
        WarEntityIdListAdd(&map->selectedEntities, id);
}

inline void removeEntityFromSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    WarEntityIdListRemove(&map->selectedEntities, id);
}

inline vec2 vec2ScreenToMapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;
    assert(map);

    rect mapPanel = map->mapPanel;
    rect viewport = map->viewport;

    v = vec2Translatef(v, -mapPanel.x, -mapPanel.y);
    v = vec2Translatef(v, viewport.x, viewport.y);
    return v;
}

inline vec2 vec2ScreenToMinimapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;
    assert(map);

    rect minimapPanel = map->minimapPanel;
    vec2 minimapPanelSize = vec2f(minimapPanel.width, minimapPanel.height);

    vec2 minimapViewportSize = vec2f(MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT);

    v = vec2Translatef(v, -minimapPanel.x, -minimapPanel.y);
    v = vec2Translatef(v, -minimapViewportSize.x / 2, -minimapViewportSize.y / 2);
    v = vec2Clampv(v, VEC2_ZERO, vec2Subv(minimapPanelSize, minimapViewportSize));
    return v;
}

inline rect rectScreenToMapCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;
    assert(map);

    rect mapPanel = map->mapPanel;
    rect viewport = map->viewport;

    r = rectTranslatef(r, -mapPanel.x, -mapPanel.y);
    r = rectTranslatef(r, viewport.x, viewport.y);
    return r;
}

inline vec2 vec2MapToScreenCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;
    assert(map);

    v = vec2Translatef(v, -map->viewport.x, -map->viewport.y);
    v = vec2Translatef(v, map->mapPanel.x, map->mapPanel.y);
    return v;
}

inline rect rectMapToScreenCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;
    assert(map);

    r = rectTranslatef(r, -map->viewport.x, -map->viewport.y);
    r = rectTranslatef(r, map->mapPanel.x, map->mapPanel.y);
    return r;
}

inline vec2 vec2MapToTileCoordinates(vec2 v)
{
    v.x = floorf(v.x / MEGA_TILE_WIDTH);
    v.y = floorf(v.y / MEGA_TILE_HEIGHT);
    return v;
}

inline vec2 vec2TileToMapCoordinates(vec2 v, bool centeredInTile)
{
    v.x *= MEGA_TILE_WIDTH;
    v.y *= MEGA_TILE_HEIGHT;

    if (centeredInTile)
    {
        v.x += halfi(MEGA_TILE_WIDTH);
        v.y += halfi(MEGA_TILE_HEIGHT);
    }

    return v;
}
