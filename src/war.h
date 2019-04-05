#define FRAMES_PER_SECONDS 30
#define SECONDS_PER_FRAME (1.0f/FRAMES_PER_SECONDS)

#define MOVE_EPSILON 0.5f
#define MOVE_WAIT_INTENTS 2
#define MOVE_WAIT_TIME 1.0f
            
#define MAP_EDGE_SCROLL_GAP 5.0f

#define TREE_MAX_WOOD 100
#define GOLDMINE_MAX_GOLD 300
#define UNIT_MAX_CARRY_WOOD 100
#define UNIT_MAX_CARRY_GOLD 100
#define WALL_MAX_HP 60

// #define DEBUG_RENDER_MAP_GRID
// #define DEBUG_RENDER_PASSABLE_INFO
// #define DEBUG_RENDER_UNIT_PATHS
// #define DEBUG_RENDER_UNIT_INFO
// #define DEBUG_RENDER_UNIT_STATS
// #define DEBUG_RENDER_UNIT_ANIMATIONS
// #define DEBUG_RENDER_MAP_ANIMATIONS

#define isButtonPressed(input, btn) (input->buttons[btn].pressed)
#define wasButtonPressed(input, btn) (input->buttons[btn].wasPressed)
#define isKeyPressed(input, key) (input->keys[key].pressed)
#define wasKeyPressed(input, key) (input->keys[key].wasPressed)

#define getScaledSpeed(context, t) ((t) * (context)->globalSpeed)
#define getScaledTime(context, t) ((t) / (context)->globalSpeed)

void setWindowSize(WarContext* context, u32 width, u32 height);
void setGlobalScale(WarContext* context, f32 scale);
void setGlobalSpeed(WarContext* context, f32 speed);

vec2 getDirFromArrowKeys(WarContext* context, WarInput* input)
{
    vec2 dir = VEC2_ZERO;

    if (isKeyPressed(input, WAR_KEY_LEFT))
        dir.x = -1;
    else if (isKeyPressed(input, WAR_KEY_RIGHT))
        dir.x = 1;

    if (isKeyPressed(input, WAR_KEY_DOWN))
        dir.y = 1;
    else if (isKeyPressed(input, WAR_KEY_UP))
        dir.y = -1;

    dir = vec2Normalize(dir);
    return dir;
}

vec2 getDirFromMousePos(WarContext* context, WarInput* input)
{
    WarMap *map = context->map;
    rect mapPanel = map->mapPanel;

    vec2 dir = VEC2_ZERO;

    if (input->pos.x < mapPanel.x + MAP_EDGE_SCROLL_GAP)
        dir.x = -1;
    else if (input->pos.x > mapPanel.x + mapPanel.width - MAP_EDGE_SCROLL_GAP)
        dir.x = 1;

    if (input->pos.y < mapPanel.y + MAP_EDGE_SCROLL_GAP)
        dir.y = -1;
    else if (input->pos.y > mapPanel.y + mapPanel.height - MAP_EDGE_SCROLL_GAP)
        dir.y = 1;

    dir = vec2Normalize(dir);
    return dir;
}

void printDirection(WarUnitDirection direction)
{
    switch (direction)
    {        
        case WAR_DIRECTION_NORTH:      logDebug("DIRECTION_NORTH\n");      break;
        case WAR_DIRECTION_NORTH_EAST: logDebug("DIRECTION_NORTH_EAST\n"); break;
        case WAR_DIRECTION_EAST:       logDebug("DIRECTION_EAST\n");       break;
        case WAR_DIRECTION_SOUTH_EAST: logDebug("DIRECTION_SOUTH_EAST\n"); break;
        case WAR_DIRECTION_SOUTH:      logDebug("DIRECTION_SOUTH\n");      break;
        case WAR_DIRECTION_SOUTH_WEST: logDebug("DIRECTION_SOUTH_WEST\n"); break;
        case WAR_DIRECTION_WEST:       logDebug("DIRECTION_WEST\n");       break;
        case WAR_DIRECTION_NORTH_WEST: logDebug("DIRECTION_NORTH_WEST\n"); break;
        default:                       logError("Unkown direction\n");     break;
    }
}