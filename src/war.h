#define FRAMES_PER_SECONDS 60
#define SECONDS_PER_FRAME (1.0f/FRAMES_PER_SECONDS)

#define MOVE_EPSILON 0.5f
#define MOVE_WAIT_INTENTS 2
#define MOVE_WAIT_TIME 1.0f

// #define DEBUG_RENDER_MAP_GRID
// #define DEBUG_RENDER_PASSABLE_INFO
// #define DEBUG_RENDER_UNIT_PATHS
// #define DEBUG_RENDER_UNIT_INFO
// #define DEBUG_RENDER_UNIT_STATS
// #define DEBUG_RENDER_UNIT_ANIMATIONS
// #define DEBUG_RENDER_MAP_ANIMATIONS

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

void printDirection(WarUnitDirection direction)
{
    switch (direction)
    {        
        case WAR_DIRECTION_NORTH:      logDebug("DIRECTION_NORTH");      break;
        case WAR_DIRECTION_NORTH_EAST: logDebug("DIRECTION_NORTH_EAST"); break;
        case WAR_DIRECTION_EAST:       logDebug("DIRECTION_EAST");       break;
        case WAR_DIRECTION_SOUTH_EAST: logDebug("DIRECTION_SOUTH_EAST"); break;
        case WAR_DIRECTION_SOUTH:      logDebug("DIRECTION_SOUTH");      break;
        case WAR_DIRECTION_SOUTH_WEST: logDebug("DIRECTION_SOUTH_WEST"); break;
        case WAR_DIRECTION_WEST:       logDebug("DIRECTION_WEST");       break;
        case WAR_DIRECTION_NORTH_WEST: logDebug("DIRECTION_NORTH_WEST"); break;
        default:                       logError("Unkown direction");     break;
    }
}