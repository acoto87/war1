#pragma once

#define MAX_RESOURCES_COUNT 583
#define MAX_TEXTURES_COUNT 583
#define MAX_ENTITIES_COUNT 100
#define MAX_SPRITE_FRAME_COUNT 100
#define MAX_CONSTRUCTS_COUNT 100

// all palettes have 768 colors
#define PALETTE_LENGTH 768

// size of mini-tiles from the tiles resources
#define MINI_TILE_WIDTH 8
#define MINI_TILE_HEIGHT 8

// size of the mega-tiles (2x2 mini-tiles) from the tileset
#define MEGA_TILE_WIDTH (MINI_TILE_WIDTH * 2)
#define MEGA_TILE_HEIGHT (MINI_TILE_HEIGHT * 2)

// size of the map in pixels
#define MAP_WIDTH (64*MEGA_TILE_WIDTH)
#define MAP_HEIGHT (64*MEGA_TILE_HEIGHT)

// size of the map in tiles
#define MAP_TILES_WIDTH (MAP_WIDTH/MEGA_TILE_WIDTH)
#define MAP_TILES_HEIGHT (MAP_HEIGHT/MEGA_TILE_HEIGHT)

// size of the viewport of the map in pixels
#define MAP_VIEWPORT_WIDTH 240
#define MAP_VIEWPORT_HEIGHT 176

// size of the minimap in pixels
#define MINIMAP_WIDTH 64
#define MINIMAP_HEIGHT 64

// ratio of the minimap and map sizes
#define MINIMAP_MAP_WIDTH_RATIO ((f32)MINIMAP_WIDTH/MAP_WIDTH)
#define MINIMAP_MAP_HEIGHT_RATIO ((f32)MINIMAP_HEIGHT/MAP_HEIGHT)

// size of the viewport of the minimap in pixels
#define MINIMAP_VIEWPORT_WIDTH (MAP_VIEWPORT_WIDTH*MINIMAP_MAP_WIDTH_RATIO)
#define MINIMAP_VIEWPORT_HEIGHT (MAP_VIEWPORT_HEIGHT*MINIMAP_MAP_HEIGHT_RATIO)

// size of the tileset for the terrain of the maps
#define TILESET_WIDTH 512
#define TILESET_HEIGHT 256
#define TILESET_TILES_PER_ROW (TILESET_WIDTH/MEGA_TILE_WIDTH)

#define MAX_OBJECTIVES_LENGTH 512
#define MAX_PLAYERS_COUNT 5
#define MAX_FEATURES_COUNT 32
#define MAX_UPGRADES_COUNT 10
#define MAX_TILES_COUNT 1024

#define isButtonPressed(input, btn) (input->buttons[btn].pressed)
#define wasButtonPressed(input, btn) (input->buttons[btn].wasPressed)
#define isKeyPressed(input, key) (input->keys[key].pressed)
#define wasKeyPressed(input, key) (input->keys[key].wasPressed)

typedef struct 
{
    bool placeholder;
    bool compressed;
    u32 offset;
    u32 compressedLength;
    u32 length;
    u32 index;
    u8 *data;
} WarRawResource;

typedef enum 
{
    WAR_FILE_TYPE_UNKOWN,
    WAR_FILE_TYPE_DEMO,
    WAR_FILE_TYPE_RETAIL
} WarFileType;

typedef struct
{
    // File Header
    // Version          Archive ID          Number of Entries 
    // PreRelease                           EF 01 00 00 (495)   little-endian
    // DOS Retail       18 00 00 00 (24)    47 02 00 00 (583)   little-endian
    // DOS Shareware    19 00 00 00 (25)    47 02 00 00 (583)   little-endian
    // Mac Retail       00 00 00 1A (26)    00 00 02 47 (583)   big-endian
    // Mac Shareware    00 00 00 19 (25)    00 00 02 47 (583)   big-endian
	u32 archiveID;
    u32 numberOfEntries;

    // File Table
    u32 offsets[MAX_RESOURCES_COUNT];

    WarFileType type;
    WarRawResource resources[MAX_RESOURCES_COUNT];
} WarFile;

typedef enum
{
    WAR_RESOURCE_TYPE_UNKNOWN,
    WAR_RESOURCE_TYPE_IMAGE,
    WAR_RESOURCE_TYPE_PALETTE,
    WAR_RESOURCE_TYPE_XMID,
    WAR_RESOURCE_TYPE_CURSOR,
    WAR_RESOURCE_TYPE_UI,
    WAR_RESOURCE_TYPE_TEXT,
    WAR_RESOURCE_TYPE_LEVEL_INFO,
    WAR_RESOURCE_TYPE_LEVEL_VISUAL,
    WAR_RESOURCE_TYPE_LEVEL_PASSABLE,
    WAR_RESOURCE_TYPE_SPRITE,
    WAR_RESOURCE_TYPE_WAVE,
    WAR_RESOURCE_TYPE_VOC,
    WAR_RESOURCE_TYPE_TILESET,
    WAR_RESOURCE_TYPE_TILES
} WarResourceType;

typedef enum
{
    WAR_LEVEL_TYPE_UNKOWN,
    WAR_LEVEL_TYPE_1,
    WAR_LEVEL_TYPE_2,
    WAR_LEVEL_TYPE_3
} WarLevelInfoType;

typedef enum
{
    WAR_RACE_NEUTRAL,
    WAR_RACE_HUMANS,
    WAR_RACE_ORCS
} WarRace;

typedef enum
{
    WAR_DIRECTION_NORTH,
    WAR_DIRECTION_NORTH_EAST,
    WAR_DIRECTION_EAST,
    WAR_DIRECTION_SOUTH_EAST,
    WAR_DIRECTION_SOUTH,
    WAR_DIRECTION_SOUTH_WEST,
    WAR_DIRECTION_WEST,
    WAR_DIRECTION_NORTH_WEST,
} WarUnitDirection;

typedef enum
{
    // units
    WAR_UNIT_FOOTMAN,
    WAR_UNIT_GRUNT,
    WAR_UNIT_PEASANT,
    WAR_UNIT_PEON,
    WAR_UNIT_CATAPULT_HUMANS,
    WAR_UNIT_CATAPULT_ORCS,
    WAR_UNIT_KNIGHT,
    WAR_UNIT_RAIDER,
    WAR_UNIT_ARCHER,
    WAR_UNIT_SPEARMAN,
    WAR_UNIT_CONJURER,
    WAR_UNIT_WARLOCK,
    WAR_UNIT_CLERIC,
    WAR_UNIT_NECROLYTE,
    WAR_UNIT_MEDIVH,
    WAR_UNIT_LOTHAR,
    WAR_UNIT_WOUNDED,
    WAR_UNIT_GRIZELDA,
    WAR_UNIT_GARONA,
    WAR_UNIT_OGRE,
    WAR_UNIT_20,
    WAR_UNIT_SPIDER,
    WAR_UNIT_SLIME,
    WAR_UNIT_FIREELEMENTAL,
    WAR_UNIT_SCORPION,
    WAR_UNIT_BRIGAND,
    WAR_UNIT_26,
    WAR_UNIT_SKELETON,
    WAR_UNIT_DAEMON,
    WAR_UNIT_DRAGON_CYCLOPS_GIANT,
    WAR_UNIT_30,
    WAR_UNIT_WATERELEMENTAL,

    // buildings
    WAR_UNIT_FARM_HUMANS,
    WAR_UNIT_FARM_ORCS,
    WAR_UNIT_BARRACKS_HUMANS,
    WAR_UNIT_BARRACKS_ORCS,
    WAR_UNIT_CHURCH,
    WAR_UNIT_TEMPLE,
    WAR_UNIT_TOWER_HUMANS,
    WAR_UNIT_TOWER_ORCS,
    WAR_UNIT_TOWNHALL_HUMANS,
    WAR_UNIT_TOWNHALL_ORCS,
    WAR_UNIT_LUMBERMILL_HUMANS,
    WAR_UNIT_LUMBERMILL_ORCS,
    WAR_UNIT_STABLES,
    WAR_UNIT_KENNEL,
    WAR_UNIT_BLACKSMITH_HUMANS,
    WAR_UNIT_BLACKSMITH_ORCS,
    WAR_UNIT_STORMWIND,
    WAR_UNIT_BLACKROCK,

    // neutral
    WAR_UNIT_GOLDMINE,

	WAR_UNIT_51,
	WAR_UNIT_PEASANT_WITH_WOOD,
	WAR_UNIT_PEON_WITH_WOOD,
    WAR_UNIT_PEASANT_WITH_GOLD,
	WAR_UNIT_PEON_WITH_GOLD,

    // others
    WAR_UNIT_ORC_CORPSE,

    WAR_UNIT_COUNT
} WarUnitType;

typedef struct 
{
    u8 dx;
    u8 dy;
    u8 w;
    u8 h;
    u32 off;
    u8* data;
} WarSpriteFrame;

typedef struct
{
    s32 image;
    s32 frameWidth;
    s32 frameHeight;
    s32 framesCount;
    WarSpriteFrame frames[MAX_SPRITE_FRAME_COUNT];
} WarSprite;

typedef enum
{
    WAR_ANIM_STATUS_NOT_STARTED,
    WAR_ANIM_STATUS_RUNNING,
    WAR_ANIM_STATUS_FINISHED
} WarAnimationStatus;

typedef struct
{
    char* name;
    bool loop;
    bool flipX;
    bool flipY;
    WarAnimationStatus status;
    f32 frameDelay;
    s32 frameCount;
    s32 frames[MAX_SPRITE_FRAME_COUNT];

    f32 animTime;
} WarSpriteAnimation;

internal bool equalsSpriteAnimation(const WarSpriteAnimation* anim1, const WarSpriteAnimation* anim2)
{
    return strcmp(anim1->name, anim2->name) == 0;
}

shlDeclareList(WarSpriteAnimationList, WarSpriteAnimation*)
shlDefineList(WarSpriteAnimationList, WarSpriteAnimation*, equalsSpriteAnimation, NULL)

typedef struct
{
    u8 x, y;
    WarUnitType type;
    u8 player;
    u16 value;
} WarLevelUnit;

typedef enum
{
    WAR_CONSTRUCT_UNKOWN,
    WAR_CONSTRUCT_ROAD,
    WAR_CONSTRUCT_WALL
} WarConstructType;

typedef struct
{
    WarConstructType type;
    u8 x1, y1;
    u8 x2, y2;
    u8 player;
} WarLevelConstruct;

typedef struct
{
    struct
    {
        u16 index;
        bool flipX;
        bool flipY;
    } tiles[4];
} WarTilesetTile;

typedef struct 
{
    WarResourceType type;
    
    union
    {
        struct
        {
            u8 colors[PALETTE_LENGTH];
        } paletteData;

        struct 
        {
            u16 width;
            u16 height;
            u8* pixels;
        } imageData;

        struct
        {
            u16 framesCount;
            u8 frameWidth;
            u8 frameHeight;
            WarSpriteFrame frames[MAX_SPRITE_FRAME_COUNT];
        } spriteData;

        struct
        {
            u32 allowId;
            bool allowedHumanUnits;
            bool allowedOrcsUnits;
            u8 allowedFeatures[MAX_FEATURES_COUNT];
            u8 allowedUpgrades[MAX_UPGRADES_COUNT][MAX_PLAYERS_COUNT];
            u16 startX;
            u16 startY;
            u16 nextLevelIndex;
            u16 visualIndex;
            u16 passableIndex;
            u16 paletteIndex;
            u16 tilesIndex;
            u16 tilesetIndex;
            u32 lumber[MAX_PLAYERS_COUNT];
            u32 gold[MAX_PLAYERS_COUNT];
            WarRace races[MAX_PLAYERS_COUNT];
            char objectives[MAX_OBJECTIVES_LENGTH];

            u32 startEntitiesCount;
            WarLevelUnit startEntities[MAX_ENTITIES_COUNT];

            u32 startRoadsCount;
            WarLevelConstruct startRoads[MAX_CONSTRUCTS_COUNT];

            u32 startWallsCount;
            WarLevelConstruct startWalls[MAX_CONSTRUCTS_COUNT];
        } levelInfo;    

        struct
        {
            u16 data[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
        } levelVisual;

        struct
        {
            u16 data[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
        } levelPassable;

        struct 
        {
            u32 tilesCount;
            u8 data[TILESET_WIDTH * TILESET_HEIGHT * 4];
        } tilesetData;

        struct
        {
            u16 palette1, palette2;
            u8* data;
        } tilesData;

        struct
        {
            u32 length;
            char* text;
        } textData;
    };
} WarResource;

typedef u32 WarEntityId;

typedef enum
{
    WAR_ENTITY_TYPE_NONE,
    WAR_ENTITY_TYPE_IMAGE,
    WAR_ENTITY_TYPE_UNIT,
    WAR_ENTITY_TYPE_ROAD,
    WAR_ENTITY_TYPE_WALL
} WarEntityType;

typedef enum
{
    WAR_ROAD_PIECE_LEFT,
    WAR_ROAD_PIECE_TOP,
    WAR_ROAD_PIECE_RIGHT,
    WAR_ROAD_PIECE_BOTTOM,
    WAR_ROAD_PIECE_BOTTOM_LEFT,
    WAR_ROAD_PIECE_VERTICAL,
    WAR_ROAD_PIECE_BOTTOM_RIGHT,
    WAR_ROAD_PIECE_T_LEFT,
    WAR_ROAD_PIECE_T_BOTTOM,
    WAR_ROAD_PIECE_T_RIGHT,
    WAR_ROAD_PIECE_CROSS,
    WAR_ROAD_PIECE_TOP_LEFT,
    WAR_ROAD_PIECE_HORIZONTAL,
    WAR_ROAD_PIECE_T_TOP,
    WAR_ROAD_PIECE_TOP_RIGHT,
} WarRoadPieceType;

typedef struct
{
    WarRoadPieceType type;
    u8 tilex, tiley;
    u8 player;
} WarRoadPiece;

internal bool roadPieceEquals(const WarRoadPiece p1, const WarRoadPiece p2)
{
    return p1.type == p2.type && p1.player == p2.player &&
           p1.tilex == p2.tilex && p1.tiley == p2.tiley;
}

shlDeclareList(WarRoadPieceList, WarRoadPiece)
shlDefineList(WarRoadPieceList, WarRoadPiece, roadPieceEquals, (WarRoadPiece){0})

typedef enum
{
    WAR_STATE_IDLE,
    WAR_STATE_MOVE,
    WAR_STATE_ATTACK,
    WAR_STATE_ATTACK_MOVE,
    WAR_STATE_BUILD,
    WAR_STATE_TO_MINE,
    WAR_STATE_MINING,
    WAR_STATE_TO_CHOP,
    WAR_STATE_CHOPPING,
    WAR_STATE_BACK_RESOURCES,
    WAR_STATE_DEAD
} WarStateType;

typedef struct
{
    WarStateType type;
    s32 entityIndex;

    union
    {
        struct
        {
            bool lookAround;
        } idle;

        struct
        {
            f32 x, y;
        } move;

        struct
        {
            s32 targetIndex;
        } attack;

        struct
        {
            s32 targetIndex;
            s32 x, y;
        } attackMove;
    };
} WarState;

typedef struct
{
    bool enabled;
    vec2 position;
    vec2 rotation;
    vec2 scale;
} WarTransformComponent;

typedef struct
{
    bool enabled;
    bool animationsEnabled;
    s32 resourceIndex;
    s32 currentAnimIndex;
    WarSpriteAnimationList animations;
    WarSprite sprite;
} WarSpriteComponent;

typedef struct
{
    bool enabled;
    WarUnitType type;
    WarUnitDirection direction;

    s32 tilex, tiley;
    s32 sizex, sizey;
    u8 player;
    u16 value;
} WarUnitComponent;

typedef struct
{
    bool enabled;
    WarRoadPieceList pieces;
} WarRoadComponent;

typedef struct
{
    bool enabled;
    f32 nextUpdateTime;
    WarState* currentState;
} WarStateMachineComponent;

typedef struct
{
    bool enabled;
    WarEntityId id;
    WarEntityType type;
    WarTransformComponent transform;
    WarSpriteComponent sprite;
    WarRoadComponent road;
    WarUnitComponent unit;
    WarStateMachineComponent stateMachine;
} WarEntity;

typedef struct
{
    WarEntity entities[MAX_ENTITIES_COUNT];
} WarScene;

typedef enum
{
    MAP_TILESET_FOREST,
    MAP_TILESET_SWAMP,
    MAP_TILESET_DUNGEON
} WarMapTilesetType;

typedef enum
{
    MAP_TILE_STATE_UNKOWN,
    MAP_TILE_STATE_FOG,
    MAP_TILE_STATE_VISIBLE
} WarMapTileState;

typedef struct
{
    s32 index;
    u32 gold;
    u32 wood;
} WarPlayerInfo;

typedef struct
{
    s32 levelInfoIndex;
    s32 scrollSpeed;

    // viewport in map coordinates, 
    // this is the portion of the map that the player see
    rect viewport;
    
    // panels of the ui, in screen coordinates
    rect leftTopPanel;
    rect leftBottomPanel;
    rect topPanel;
    rect bottomPanel;
    rect rightPanel;
    rect mapPanel;
    rect minimapPanel;

    WarSprite sprite;
    WarSprite minimapSprite;

    WarMapTilesetType tilesetType;
    WarMapTileState tileStates[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];

    WarEntity* entities[MAX_ENTITIES_COUNT];
    bool selectedEntities[MAX_ENTITIES_COUNT];

    WarPlayerInfo players[MAX_PLAYERS_COUNT];
} WarMap;

typedef enum
{
    WAR_MOUSE_LEFT,
    WAR_MOUSE_RIGHT,

    WAR_MOUSE_COUNT
} WarMouseButtons;

typedef enum
{
    WAR_KEY_CTRL,
    WAR_KEY_SHIFT,
    WAR_KEY_LEFT,
    WAR_KEY_RIGHT,
    WAR_KEY_DOWN,
    WAR_KEY_UP,

    WAR_KEY_COUNT
} WarKeys;

typedef struct
{
    // indicates if the key is pressed in the current frame
    bool pressed; 

    // indicate if the key was pressed in the previous frame
    bool wasPressed; 
} WarKeyButtonState;

typedef struct
{
    // mouse position
    vec2 pos;

    // state of the mouse buttons
    WarKeyButtonState buttons[WAR_MOUSE_COUNT];

    // state of the keys
    WarKeyButtonState keys[WAR_KEY_COUNT];

    // drag
    bool dragging;
    vec2 dragPos;
} WarInput;

typedef struct 
{
    f32 time;
    f32 deltaTime;
    u32 fps;

    f32 globalScale;

    u32 originalWindowWidth;
    u32 originalWindowHeight;
    u32 windowWidth;
    u32 windowHeight;
    u32 framebufferWidth;
    u32 framebufferHeight;
    f32 devicePixelRatio;
    char* windowTitle;
    GLFWwindow* window;

    char* warFilePath;
    WarFile* warFile;

    WarResource *resources[MAX_RESOURCES_COUNT];

    NVGcontext* gfx;

    u32 staticEntityId;
    WarScene* currentScene;
    WarMap* map;

    WarInput input;
} WarContext;
