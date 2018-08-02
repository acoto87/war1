#pragma once

#define MAX_RESOURCES_COUNT 583
#define MAX_TEXTURES_COUNT 583
#define MAX_ENTITIES_COUNT 100
#define MAX_SPRITE_FRAME_COUNT 100
#define PALETTE_LENGTH 768

#define MINI_TILE_WIDTH 8
#define MINI_TILE_HEIGHT 8
#define MEGA_TILE_WIDTH (MINI_TILE_WIDTH * 2)
#define MEGA_TILE_HEIGHT (MINI_TILE_HEIGHT * 2)
#define MAP_WIDTH 64
#define MAP_HEIGHT 64

#define TILESET_WIDTH_PX 512
#define TILESET_HEIGHT_PX 256
#define TILESET_TILES_PER_ROW (TILESET_WIDTH_PX/MEGA_TILE_WIDTH)

#define MAX_OBJECTIVES_LENGTH 512
#define MAX_PLAYERS_COUNT 5
#define MAX_FEATURES_COUNT 32
#define MAX_UPGRADES_COUNT 10
#define MAX_CONSTRUCTS_COUNT 100
#define MAX_TILES_COUNT 1024

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
} WarUnitType;

typedef struct
{
    s32 textureIndex;
    u32 width, height;
    s32 count;
    GLuint vao, vbo, ibo;
} WarSprite;

typedef struct 
{
    u8 dx;
    u8 dy;
    u8 w;
    u8 h;
    u32 off;
    u8 *data;
} WarSpriteFrame;

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
            u8 *pixels;
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
            u16 data[MAP_WIDTH * MAP_HEIGHT];
        } levelVisual;

        struct
        {
            u16 data[MAP_WIDTH * MAP_HEIGHT];
        } levelPassable;

        struct 
        {
            u32 tilesCount;
            u8 data[TILESET_WIDTH_PX * TILESET_HEIGHT_PX * 4];
        } tilesetData;

        struct
        {
            u16 palette1, palette2;
            u8 *data;
        } tilesData;
    };
} WarResource;

typedef u32 WarEntityId;

typedef enum
{
    WAR_ENTITY_TYPE_NONE,
    WAR_ENTITY_TYPE_IMAGE,
    WAR_ENTITY_TYPE_UNIT,
} WarEntityType;

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
    s32 resourceIndex;
    s32 frameIndex;
    WarSprite sprite;
} WarSpriteComponent;

typedef struct
{
    bool enabled;
    bool loop;
    f32 offset;
    f32 duration;
} WarAnimationComponent;

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
    WarEntityId id;
    WarEntityType type;
    WarTransformComponent transform;
    WarSpriteComponent sprite;
    WarAnimationComponent anim;
    WarUnitComponent unit;
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
    s32 levelInfoIndex;

    WarSprite sprite;

    s32 scrollSpeed;
    vec2 pos, dir;

    WarMapTilesetType tilesetType;
    WarMapTileState tileStates[MAP_WIDTH * MAP_HEIGHT];

    WarEntity* entities[MAX_ENTITIES_COUNT];
    bool selectedEntities[MAX_ENTITIES_COUNT];
} WarMap;

typedef struct 
{
    f32 time;
    f32 deltaTime;
    u32 fps;

    u32 windowWidth;
    u32 windowHeight;
    char *windowTitle;
    GLFWwindow *window;

    GLuint shaderProgram;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint positionLocation;
    GLint texCoordLocation;
    GLint modelLocation;
    GLint viewLocation;
    GLint projLocation;
    GLint texLocation;

    char *warFilePath;
    WarFile *warFile;

    u32 resourcesCount;
    WarResource *resources[MAX_RESOURCES_COUNT];

    u32 texturesCount;
    GLuint textures[MAX_TEXTURES_COUNT];

    u32 staticEntityId;
    WarScene *currentScene;
    WarMap *map;
} WarContext;

typedef struct
{
    vec2 position;
    vec2 texCoords;
} WarVertex;