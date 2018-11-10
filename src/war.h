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