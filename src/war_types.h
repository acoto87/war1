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
#define MEGA_TILE_WIDTH (MINI_TILE_WIDTH*2)
#define MEGA_TILE_HEIGHT (MINI_TILE_HEIGHT*2)

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

#define directionByIndex(i) ((WarUnitDirection)(WAR_DIRECTION_NORTH + i))

shlDefineCreateArray(s32, s32)
shlDefineFreeArray(s32, s32)

bool equalsS32(const s32 a, const s32 b)
{
    return a == b;
}

shlDeclareList(s32List, s32)
shlDefineList(s32List, s32)

#define s32ListDefaultOptions (s32ListOptions){0, equalsS32, NULL}

bool equalsVec2(const vec2 v1, const vec2 v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

shlDeclareList(vec2List, vec2)
shlDefineList(vec2List, vec2)

#define vec2ListDefaultOptions (vec2ListOptions){VEC2_ZERO, equalsVec2, NULL}

//
// Forward references to other structs that need a reference to these ones.
// See WarButtonComponent, WarState.building.
//
struct _WarContext;
struct _WarEntity;

//
// Resources
//
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

    WAR_DIRECTION_COUNT
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
    WAR_UNIT_SPIDER,
    WAR_UNIT_SLIME,
    WAR_UNIT_FIRE_ELEMENTAL,
    WAR_UNIT_SCORPION,
    WAR_UNIT_BRIGAND,
    WAR_UNIT_THE_DEAD,
    WAR_UNIT_SKELETON,
    WAR_UNIT_DAEMON,
    WAR_UNIT_WATER_ELEMENTAL,
    WAR_UNIT_DRAGON_CYCLOPS_GIANT,
    WAR_UNIT_26,
    WAR_UNIT_30,

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
    WAR_UNIT_STABLE,
    WAR_UNIT_KENNEL,
    WAR_UNIT_BLACKSMITH_HUMANS,
    WAR_UNIT_BLACKSMITH_ORCS,
    WAR_UNIT_STORMWIND,
    WAR_UNIT_BLACKROCK,

    // neutral
    WAR_UNIT_GOLDMINE,

	WAR_UNIT_51,

    // others
    WAR_UNIT_HUMAN_CORPSE,
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

typedef struct
{
    s32 resourceIndex;
    s32 frameIndicesCount;
    s32 frameIndices[MAX_SPRITE_FRAME_COUNT];
} WarSpriteResourceRef;

WarSpriteResourceRef createSpriteResourceRef(s32 resourceIndex, s32 frameIndicesCount, s32 frameIndices[])
{
    WarSpriteResourceRef spriteResourceRef = (WarSpriteResourceRef){0};
    spriteResourceRef.resourceIndex = resourceIndex;
    spriteResourceRef.frameIndicesCount = frameIndicesCount;
    if (frameIndices)
    {
        memcpy(spriteResourceRef.frameIndices, frameIndices, frameIndicesCount * sizeof(s32));
    }
    return spriteResourceRef;
}

#define invalidResourceRef() createSpriteResourceRef(-1, 0, NULL)
#define imageResourceRef(resourceIndex) createSpriteResourceRef(resourceIndex, 0, NULL)
#define spriteResourceRef(resourceIndex, spriteIndex) createSpriteResourceRef(resourceIndex, 1, arrayArg(s32, spriteIndex))

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

    vec2 offset;
    vec2 scale;

    f32 frameDelay;
    s32List frames;
    WarSprite sprite;

    f32 animTime;
    WarAnimationStatus status;
} WarSpriteAnimation;

bool equalsSpriteAnimation(const WarSpriteAnimation* anim1, const WarSpriteAnimation* anim2)
{
    return strcmp(anim1->name, anim2->name) == 0;
}

shlDeclareList(WarSpriteAnimationList, WarSpriteAnimation*)
shlDefineList(WarSpriteAnimationList, WarSpriteAnimation*)

#define WarSpriteAnimationListDefaultOptions (WarSpriteAnimationListOptions){NULL, equalsSpriteAnimation, freeAnimation}

typedef enum
{
    WAR_RESOURCE_NONE,
    WAR_RESOURCE_GOLD,
    WAR_RESOURCE_WOOD
} WarResourceKind;

typedef struct
{
    u8 x, y;
    WarUnitType type;
    u8 player;
    WarResourceKind resourceKind;
    u16 amount;
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

//
// Entities
//
typedef u16 WarEntityId;

bool equalsEntityId(const WarEntityId id1, const WarEntityId id2)
{
    return id1 == id2;
}

shlDeclareList(WarEntityIdList, WarEntityId)
shlDefineList(WarEntityIdList, WarEntityId)

#define WarEntityIdListDefaultOptions (WarEntityIdListOptions){0, equalsEntityId, NULL}

typedef enum
{
    WAR_FEATURE_UNIT_FOOTMAN,
    WAR_FEATURE_UNIT_GRUNT = WAR_FEATURE_UNIT_FOOTMAN,
    WAR_FEATURE_UNIT_PEASANT,
    WAR_FEATURE_UNIT_PEON = WAR_FEATURE_UNIT_PEASANT,
    WAR_FEATURE_UNIT_CATAPULT,
    WAR_FEATURE_UNIT_KNIGHT,
    WAR_FEATURE_UNIT_RAIDER = WAR_FEATURE_UNIT_KNIGHT,
    WAR_FEATURE_UNIT_ARCHER,
    WAR_FEATURE_UNIT_SPEARMAN = WAR_FEATURE_UNIT_ARCHER,
    WAR_FEATURE_UNIT_CONJURER,
    WAR_FEATURE_UNIT_WARLOCK = WAR_FEATURE_UNIT_CONJURER,
    WAR_FEATURE_UNIT_CLERIC,
    WAR_FEATURE_UNIT_NECROLYTE = WAR_FEATURE_UNIT_CLERIC,
    WAR_FEATURE_UNIT_FARM,
    WAR_FEATURE_UNIT_BARRACKS,
    WAR_FEATURE_UNIT_CHURCH,
    WAR_FEATURE_UNIT_TEMPLE = WAR_FEATURE_UNIT_CHURCH,
    WAR_FEATURE_UNIT_TOWER,
    WAR_FEATURE_UNIT_TOWN_HALL,
    WAR_FEATURE_UNIT_LUMBER_MILL,
    WAR_FEATURE_UNIT_STABLE,
    WAR_FEATURE_UNIT_KENNEL = WAR_FEATURE_UNIT_STABLE,
    WAR_FEATURE_UNIT_BLACKSMITH,
    WAR_FEATURE_SPELL_HEALING,
    WAR_FEATURE_SPELL_RAISE_DEAD = WAR_FEATURE_SPELL_HEALING,
    WAR_FEATURE_SPELL_FAR_SIGHT,
    WAR_FEATURE_SPELL_DARK_VISION = WAR_FEATURE_SPELL_FAR_SIGHT,
    WAR_FEATURE_SPELL_INVISIBILITY,
    WAR_FEATURE_SPELL_UNHOLY_ARMOR = WAR_FEATURE_SPELL_INVISIBILITY,
    WAR_FEATURE_SPELL_SCORPION,
    WAR_FEATURE_SPELL_SPIDER = WAR_FEATURE_SPELL_SCORPION,
    WAR_FEATURE_SPELL_RAIN_OF_FIRE,
    WAR_FEATURE_SPELL_POISON_CLOUD = WAR_FEATURE_SPELL_RAIN_OF_FIRE,
    WAR_FEATURE_SPELL_WATER_ELEMENTAL,
    WAR_FEATURE_SPELL_DAEMON = WAR_FEATURE_SPELL_WATER_ELEMENTAL,
    WAR_FEATURE_UNIT_ROAD,
    WAR_FEATURE_UNIT_WALL,
} WarFeatureType;

typedef enum
{
    WAR_UPGRADE_ARROWS,
    WAR_UPGRADE_SPEARS = WAR_UPGRADE_ARROWS,
    WAR_UPGRADE_SWORDS,
    WAR_UPGRADE_AXES = WAR_UPGRADE_SWORDS,
    WAR_UPGRADE_HORSES,
    WAR_UPGRADE_WOLVES = WAR_UPGRADE_HORSES,
    WAR_UPGRADE_SCORPIONS,
    WAR_UPGRADE_SPIDERS = WAR_UPGRADE_SCORPIONS,
    WAR_UPGRADE_RAIN_OF_FIRE,
    WAR_UPGRADE_POISON_CLOUD = WAR_UPGRADE_RAIN_OF_FIRE,
    WAR_UPGRADE_WATER_ELEMENTAL,
    WAR_UPGRADE_DAEMON = WAR_UPGRADE_WATER_ELEMENTAL,
    WAR_UPGRADE_HEALING,
    WAR_UPGRADE_RAISE_DEAD = WAR_UPGRADE_HEALING,
    WAR_UPGRADE_FAR_SIGHT,
    WAR_UPGRADE_DARK_VISION = WAR_UPGRADE_FAR_SIGHT,
    WAR_UPGRADE_INVISIBILITY,
    WAR_UPGRADE_UNHOLY_ARMOR = WAR_UPGRADE_INVISIBILITY,
    WAR_UPGRADE_SHIELD,
} WarUpgradeType;

typedef enum
{
    WAR_COMMAND_NONE, // 0

    // unit commands
    WAR_COMMAND_MOVE, // 1
    WAR_COMMAND_STOP,
    WAR_COMMAND_HARVEST,
    WAR_COMMAND_DELIVER,
    WAR_COMMAND_REPAIR,
    WAR_COMMAND_ATTACK,

    // train commands
    WAR_COMMAND_TRAIN_FOOTMAN, // 7
    WAR_COMMAND_TRAIN_GRUNT,
    WAR_COMMAND_TRAIN_PEASANT,
    WAR_COMMAND_TRAIN_PEON,
    WAR_COMMAND_TRAIN_CATAPULT_HUMANS,
    WAR_COMMAND_TRAIN_CATAPULT_ORCS,
    WAR_COMMAND_TRAIN_KNIGHT,
    WAR_COMMAND_TRAIN_RAIDER,
    WAR_COMMAND_TRAIN_ARCHER,
    WAR_COMMAND_TRAIN_SPEARMAN,
    WAR_COMMAND_TRAIN_CONJURER,
    WAR_COMMAND_TRAIN_WARLOCK,
    WAR_COMMAND_TRAIN_CLERIC,
    WAR_COMMAND_TRAIN_NECROLYTE, 

    // spell commands
    WAR_COMMAND_SPELL_HEALING, // 21
    WAR_COMMAND_SPELL_FAR_SIGHT,
    WAR_COMMAND_SPELL_INVISIBILITY,
    WAR_COMMAND_SPELL_RAIN_OF_FIRE,
    WAR_COMMAND_SPELL_POISON_CLOUD,
    WAR_COMMAND_SPELL_RAISE_DEAD,
    WAR_COMMAND_SPELL_DARK_VISION,
    WAR_COMMAND_SPELL_UNHOLY_ARMOR,

    // summons
    WAR_COMMAND_SUMMON_SPIDER, // 29
    WAR_COMMAND_SUMMON_SCORPION,
    WAR_COMMAND_SUMMON_DAEMON,
    WAR_COMMAND_SUMMON_WATER_ELEMENTAL,

    // build commands
    WAR_COMMAND_BUILD_BASIC, // 33
    WAR_COMMAND_BUILD_ADVANCED,
    WAR_COMMAND_BUILD_FARM_HUMANS,
    WAR_COMMAND_BUILD_FARM_ORCS,
    WAR_COMMAND_BUILD_BARRACKS_HUMANS,
    WAR_COMMAND_BUILD_BARRACKS_ORCS,
    WAR_COMMAND_BUILD_CHURCH,
    WAR_COMMAND_BUILD_TEMPLE,
    WAR_COMMAND_BUILD_TOWER_HUMANS,
    WAR_COMMAND_BUILD_TOWER_ORCS,
    WAR_COMMAND_BUILD_TOWNHALL_HUMANS,
    WAR_COMMAND_BUILD_TOWNHALL_ORCS,
    WAR_COMMAND_BUILD_LUMBERMILL_HUMANS,
    WAR_COMMAND_BUILD_LUMBERMILL_ORCS,
    WAR_COMMAND_BUILD_STABLE,
    WAR_COMMAND_BUILD_KENNEL,
    WAR_COMMAND_BUILD_BLACKSMITH_HUMANS,
    WAR_COMMAND_BUILD_BLACKSMITH_ORCS,
    WAR_COMMAND_BUILD_ROAD,
    WAR_COMMAND_BUILD_WALL,

    // upgrades
    WAR_COMMAND_UPGRADE_SWORDS, // 53
    WAR_COMMAND_UPGRADE_AXES,
    WAR_COMMAND_UPGRADE_SHIELD_HUMANS,
    WAR_COMMAND_UPGRADE_SHIELD_ORCS,
    WAR_COMMAND_UPGRADE_ARROWS,
    WAR_COMMAND_UPGRADE_SPEARS,
    WAR_COMMAND_UPGRADE_HORSES,
    WAR_COMMAND_UPGRADE_WOLVES,
    WAR_COMMAND_UPGRADE_SCORPION,
    WAR_COMMAND_UPGRADE_SPIDER,
    WAR_COMMAND_UPGRADE_RAIN_OF_FIRE,
    WAR_COMMAND_UPGRADE_POISON_CLOUD,
    WAR_COMMAND_UPGRADE_WATER_ELEMENTAL,
    WAR_COMMAND_UPGRADE_DAEMON,
    WAR_COMMAND_UPGRADE_HEALING,
    WAR_COMMAND_UPGRADE_RAISE_DEAD,
    WAR_COMMAND_UPGRADE_FAR_SIGHT,
    WAR_COMMAND_UPGRADE_DARK_VISION,
    WAR_COMMAND_UPGRADE_INVISIBILITY,
    WAR_COMMAND_UPGRADE_UNHOLY_ARMOR,

    // cancel
    WAR_COMMAND_CANCEL // 73
} WarUnitCommandType;

typedef struct
{
    WarUnitCommandType type;

    union
    {
        struct 
        {
            WarUnitType unitToTrain;
            WarUnitType buildingUnit;
        } train;

        struct 
        {
            WarUpgradeType upgradeToBuild;
            WarUnitType buildingUnit;
        } upgrade;

        struct
        {
            WarUnitType buildingToBuild;
        } build;
    };
} WarUnitCommand;

typedef enum
{
    WAR_ENTITY_TYPE_NONE,
    WAR_ENTITY_TYPE_IMAGE,
    WAR_ENTITY_TYPE_UNIT,
    WAR_ENTITY_TYPE_ROAD,
    WAR_ENTITY_TYPE_WALL,
    WAR_ENTITY_TYPE_RUIN,
    WAR_ENTITY_TYPE_FOREST,
    WAR_ENTITY_TYPE_TEXT,
    WAR_ENTITY_TYPE_RECT,
    WAR_ENTITY_TYPE_BUTTON,

    WAR_ENTITY_TYPE_COUNT
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
    s32 tilex, tiley;
    u8 player;
} WarRoadPiece;

#define WarRoadPieceEmpty (WarRoadPiece){0}
#define createRoadPiece(x, y, player) ((WarRoadPiece){0, (x), (y), (player)})

bool equalsRoadPiece(const WarRoadPiece r1, const WarRoadPiece r2)
{
    return r1.type == r2.type && r1.player == r2.player &&
           r1.tilex == r2.tilex && r1.tiley == r2.tiley;
}

shlDeclareList(WarRoadPieceList, WarRoadPiece)
shlDefineList(WarRoadPieceList, WarRoadPiece)

#define WarRoadPieceListDefaultOptions (WarRoadPieceListOptions){WarRoadPieceEmpty, equalsRoadPiece, NULL}

typedef enum
{
    WAR_WALL_PIECE_LEFT,
    WAR_WALL_PIECE_TOP,
    WAR_WALL_PIECE_RIGHT,
    WAR_WALL_PIECE_BOTTOM,
    WAR_WALL_PIECE_BOTTOM_LEFT,
    WAR_WALL_PIECE_VERTICAL,
    WAR_WALL_PIECE_BOTTOM_RIGHT,
    WAR_WALL_PIECE_T_LEFT,
    WAR_WALL_PIECE_T_BOTTOM,
    WAR_WALL_PIECE_T_RIGHT,
    WAR_WALL_PIECE_CROSS,
    WAR_WALL_PIECE_TOP_LEFT,
    WAR_WALL_PIECE_HORIZONTAL,
    WAR_WALL_PIECE_T_TOP,
    WAR_WALL_PIECE_TOP_RIGHT,
} WarWallPieceType;

typedef struct
{
    WarWallPieceType type;
    s32 hp;
    s32 maxhp;
    s32 tilex, tiley;
    u8 player;
} WarWallPiece;

#define WarWallPieceEmpty (WarWallPiece){0}
#define createWallPiece(x, y, player) ((WarWallPiece){0, 0, 0, (x), (y), (player)})

bool equalsWallPiece(const WarWallPiece w1, const WarWallPiece w2)
{
    return w1.type == w2.type && w1.player == w2.player &&
           w1.tilex == w2.tilex && w1.tiley == w2.tiley;
}

shlDeclareList(WarWallPieceList, WarWallPiece)
shlDefineList(WarWallPieceList, WarWallPiece)

#define WarWallPieceListDefaultOptions (WarWallPieceListOptions){WarWallPieceEmpty, equalsWallPiece, NULL}

typedef enum 
{
    WAR_RUIN_PIECE_NONE,
    WAR_RUIN_PIECE_TOP_LEFT,
    WAR_RUIN_PIECE_TOP,
    WAR_RUIN_PIECE_TOP_RIGHT,
    WAR_RUIN_PIECE_LEFT,
    WAR_RUIN_PIECE_CENTER,
    WAR_RUIN_PIECE_RIGHT,
    WAR_RUIN_PIECE_BOTTOM_LEFT,
    WAR_RUIN_PIECE_BOTTOM,
    WAR_RUIN_PIECE_BOTTOM_RIGHT,
    WAR_RUIN_PIECE_TOP_LEFT_INSIDE,
    WAR_RUIN_PIECE_TOP_RIGHT_INSIDE,
    WAR_RUIN_PIECE_BOTTOM_LEFT_INSIDE,
    WAR_RUIN_PIECE_BOTTOM_RIGHT_INSIDE,
    WAR_RUIN_PIECE_DIAG_1,
    WAR_RUIN_PIECE_DIAG_2,
} WarRuinPieceType;

typedef struct
{
    WarRuinPieceType type;
    s32 tilex, tiley;
} WarRuinPiece;

#define WarRuinPieceEmpty (WarRuinPiece){0}
#define createRuinPiece(x, y) ((WarRuinPiece){0, (x), (y)})

bool equalsRuinPiece(const WarRuinPiece r1, const WarRuinPiece r2)
{
    return r1.type == r2.type && 
           r1.tilex == r2.tilex && r1.tiley == r2.tiley;
}

shlDeclareList(WarRuinPieceList, WarRuinPiece)
shlDefineList(WarRuinPieceList, WarRuinPiece)

#define WarRuinPieceListDefaultOptions (WarRuinPieceListOptions){WarRuinPieceEmpty, equalsRuinPiece, NULL}

typedef enum 
{
    WAR_TREE_NONE,
    WAR_TREE_TOP_LEFT,
    WAR_TREE_TOP,
    WAR_TREE_TOP_RIGHT,
    WAR_TREE_LEFT,
    WAR_TREE_CENTER,
    WAR_TREE_RIGHT,
    WAR_TREE_BOTTOM_LEFT,
    WAR_TREE_BOTTOM,
    WAR_TREE_BOTTOM_RIGHT,
    WAR_TREE_TOP_LEFT_INSIDE,
    WAR_TREE_TOP_RIGHT_INSIDE,
    WAR_TREE_BOTTOM_LEFT_INSIDE,
    WAR_TREE_BOTTOM_RIGHT_INSIDE,
    WAR_TREE_TOP_END,
    WAR_TREE_BOTTOM_END,
    WAR_TREE_VERTICAL,
    WAR_TREE_DIAG_1,
    WAR_TREE_DIAG_2,
    WAR_TREE_CHOPPED,
} WarTreeTileType;

typedef struct
{
    WarTreeTileType type;
    s32 tilex, tiley;
    s32 amount;
} WarTree;

#define WarTreeEmpty (WarTree){0}
#define createTree(x, y, amount) ((WarTree){0, (x), (y), (amount)})

bool equalsTree(const WarTree t1, const WarTree t2)
{
    return t1.tilex == t2.tilex && t1.tiley == t2.tiley;
}

s32 compareTreesByPosition(const WarTree t1, const WarTree t2)
{
    // order by 'x' asc, then by 'y' desc
    return t1.tilex == t2.tilex ? t2.tiley - t1.tiley : t1.tilex - t2.tilex;
}

shlDeclareList(WarTreeList, WarTree)
shlDefineList(WarTreeList, WarTree)

#define WarTreeListDefaultOptions (WarTreeListOptions){WarTreeEmpty, equalsTree, NULL}

typedef enum
{
    WAR_ACTION_STEP_NONE,
    WAR_ACTION_STEP_UNBREAKABLE,
    WAR_ACTION_STEP_FRAME,
    WAR_ACTION_STEP_WAIT,
    WAR_ACTION_STEP_MOVE,
    WAR_ACTION_STEP_ATTACK,
    WAR_ACTION_STEP_SOUND_SWORD,
    WAR_ACTION_STEP_SOUND_FIST,
    WAR_ACTION_STEP_SOUND_FIREBALL,
    WAR_ACTION_STEP_SOUND_CHOPPING,
    WAR_ACTION_STEP_SOUND_CATAPULT,
    WAR_ACTION_STEP_SOUND_ARROW,
    WAR_ACTION_STEP_SOUND_LIGHTNING,
} WarUnitActionStepType;

typedef enum
{
    WAR_UNBREAKABLE_BEGIN,
    WAR_UNBREAKABLE_END,
} WarUnbreakableParam;

typedef struct
{
    WarUnitActionStepType type;
    s32 param;
} WarUnitActionStep;

#define WarUnitActionStepEmpty (WarUnitActionStep){WAR_ACTION_STEP_NONE}

bool equalsActionStep(const WarUnitActionStep step1, const WarUnitActionStep step2)
{
    return step1.type == step2.type && step1.param == step2.param;
}

shlDeclareList(WarUnitActionStepList, WarUnitActionStep)
shlDefineList(WarUnitActionStepList, WarUnitActionStep)

#define WarUnitActionStepListDefaultOptions (WarUnitActionStepListOptions){WarUnitActionStepEmpty, equalsActionStep, NULL}

typedef enum
{
    WAR_ACTION_TYPE_NONE = -1,
    WAR_ACTION_TYPE_IDLE,
    WAR_ACTION_TYPE_WALK,
    WAR_ACTION_TYPE_ATTACK,
    WAR_ACTION_TYPE_DEATH,
    WAR_ACTION_TYPE_HARVEST,
    WAR_ACTION_TYPE_REPAIR,
    WAR_ACTION_TYPE_BUILD
} WarUnitActionType;

typedef enum
{
    WAR_ACTION_NOT_STARTED,
    WAR_ACTION_RUNNING,
    WAR_ACTION_FINISHED,
} WarUnitActionStatus;

typedef struct
{
    WarUnitActionType type;
    WarUnitActionStatus status;    
    bool unbreakable;
    bool directional;
    bool loop;
    f32 scale;
    f32 waitCount;
    s32 stepIndex;
    WarUnitActionStepList steps;
    WarUnitActionStepType lastActionStep;
    WarUnitActionStepType lastSoundStep;
} WarUnitAction;

bool equalsAction(const WarUnitAction* a1, const WarUnitAction* a2)
{
    return a1->type == a2->type;
}

void freeAction(WarUnitAction* a)
{
    free((void*)a);
}

shlDeclareList(WarUnitActionList, WarUnitAction*)
shlDefineList(WarUnitActionList, WarUnitAction*)

#define WarUnitActionListDefaultOptions (WarUnitActionListOptions){NULL, equalsAction, freeAction}

typedef struct
{
    vec2List nodes;
} WarMapPath;

typedef enum
{
    PATH_FINDING_BFS,
    PATH_FINDING_ASTAR
} PathFindingType;

typedef enum
{
    PATH_FINDER_DATA_EMPTY = 0,
    PATH_FINDER_DATA_STATIC = 1,
    PATH_FINDER_DATA_DYNAMIC = 2,
} WarPathFinderDataType;

typedef struct
{
    PathFindingType type;
    s32 width, height;
    u16* data;
} WarPathFinder;

typedef enum
{
    WAR_STATE_IDLE,
    WAR_STATE_MOVE,
    WAR_STATE_PATROL,
    WAR_STATE_FOLLOW,
    WAR_STATE_ATTACK,
    WAR_STATE_GOLD,
    WAR_STATE_MINING,
    WAR_STATE_WOOD,
    WAR_STATE_CHOPPING,
    WAR_STATE_DELIVER,
    WAR_STATE_DEATH,
    WAR_STATE_COLLAPSE,
    WAR_STATE_TRAIN,
    WAR_STATE_UPGRADE,
    WAR_STATE_BUILD,
    WAR_STATE_REPAIR,
    WAR_STATE_REPAIRING,
    WAR_STATE_WAIT,

    WAR_STATE_COUNT
} WarStateType;

typedef struct _WarState
{
    WarStateType type;
    s32 entityId;
    f32 nextUpdateTime;
    f32 delay;
    struct _WarState* nextState;

    union
    {
        struct
        {
            bool lookAround;
        } idle;

        struct
        {
            s32 positionIndex;
            vec2List positions;
            
            s32 pathNodeIndex;
            WarMapPath path;

            s32 waitCount;
        } move;

        struct
        {
            s32 positionIndex;
            vec2List positions;
            s32 dir;
        } patrol;

        struct
        {
            s32 targetEntityId;
            s32 distance;
        } follow;

        struct
        {
            f32 waitTime;
        } wait;

        struct
        {
            s32 targetEntityId;
            vec2 targetTile;
        } attack;

        struct
        {
            s32 goldmineId;
        } gold;

        struct
        {
            s32 goldmineId;
            f32 miningTime;
        } mine;

        struct
        {
            s32 forestId;
            vec2 position;
        } wood;

        struct
        {
            s32 forestId;
            vec2 position;
            f32 chopTime;
        } chop;

        struct
        {
            s32 townHallId;
            bool insideBuilding;
        } deliver;

        struct
        {
            struct _WarEntity* entityToBuild;
            f32 buildTime; // how much time has passed since start building (in seconds)
            f32 totalBuildTime; // total time to build (in seconds)
            bool cancelled;
        } train;

        struct
        {
            WarUpgradeType upgradeToBuild;
            f32 buildTime; // how much time has passed since start building (in seconds)
            f32 totalBuildTime; // total time to build (in seconds)
            bool cancelled;
        } upgrade;

        struct
        {
            WarEntityId workerId; // the worker that is building the building            
            f32 buildTime; // how much time has passed since start building (in seconds)
            f32 totalBuildTime; // total time to build (in seconds)
            bool cancelled;
        } build;

        struct
        {
            WarEntityId buildingId;
        } repair;

        struct
        {
            WarEntityId buildingId;
            bool insideBuilding;
            f32 repairTime;
        } repairing;
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
    s32 resourceIndex;
    s32 frameIndex;
    WarSprite sprite;
} WarSpriteComponent;

typedef struct
{
    bool enabled;
    WarUnitType type;
    WarUnitDirection direction;

    // position in tiles
    s32 tilex, tiley;
    // size in tiles
    s32 sizex, sizey;

    // index of the player this unit belongs to
    u8 player;

    // the units that can carry resources are
    // peasants, peons, goldmines and trees
    WarResourceKind resourceKind;
    s32 amount;

    // indicate if the unit is building something
    bool building;
    f32 buildPercent;
  
    // hit points, magic and armor
    s32 maxhp;
    s32 hp;
    s32 maxMagic;
    s32 magic;
    s32 armor;
    s32 range;
    s32 minDamage;
    s32 rndDamage;
    s32 decay;

    // index of the array of speeds of the unit
    s32 speed;

    // the current and action list for the unit
    s32 actionIndex;
    WarUnitActionList actions;
} WarUnitComponent;

typedef struct
{
    bool enabled;
    WarSpriteAnimationList animations;
} WarAnimationsComponent;

typedef struct
{
    bool enabled;
    WarRoadPieceList pieces;
} WarRoadComponent;

typedef struct
{
    bool enabled;
    WarWallPieceList pieces;
} WarWallComponent;

typedef struct
{
    bool enabled;
    WarRuinPieceList pieces;
} WarRuinComponent;

typedef struct
{
    bool enabled;
    WarTreeList trees;
} WarForestComponent;

typedef struct
{
    bool enabled;
    WarState* currentState;
    WarState* nextState;
    bool leaveState;
    bool enterState;
} WarStateMachineComponent;

typedef struct
{
    bool enabled;
    char* name;
} WarUIComponent;

typedef struct
{
    bool enabled;
    char* text;
    WarSprite font;
    f32 fontSize;
    u8Color fontColor;
    s32 highlightIndex;
} WarTextComponent;

typedef struct
{
    bool enabled;
    vec2 size;
    u8Color color;
} WarRectComponent;

typedef void (*WarClickHandler)(struct _WarContext* context, struct _WarEntity* entity);

typedef struct
{
    bool enabled;
    bool hot;
    bool active;
    s32 highlightIndex;
    char tooltip[100];
    s32 gold;
    s32 wood;
    WarSprite normalSprite;
    WarSprite pressedSprite;
    WarClickHandler clickHandler;
} WarButtonComponent;

typedef struct _WarEntity
{
    bool enabled;
    WarEntityId id;
    WarEntityType type;
    WarTransformComponent transform;
    WarSpriteComponent sprite;
    WarRoadComponent road;
    WarWallComponent wall;
    WarRuinComponent ruin;
    WarForestComponent forest;
    WarUnitComponent unit;
    WarStateMachineComponent stateMachine;
    WarAnimationsComponent animations;
    WarUIComponent ui;
    WarTextComponent text;
    WarRectComponent rect;
    WarButtonComponent button;
} WarEntity;

bool equalsEntity(const WarEntity* e1, const WarEntity* e2)
{
    return e1->id == e2->id;
}

void freeEntity(WarEntity* e)
{
    free((void*)e);
}

shlDeclareList(WarEntityList, WarEntity*)
shlDefineList(WarEntityList, WarEntity*)

#define WarEntityListDefaultOptions (WarEntityListOptions){NULL, equalsEntity, freeEntity}
#define WarEntityListNonFreeOptions (WarEntityListOptions){NULL, equalsEntity, NULL}

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
    s32 allowed;
    s32 level;
} WarUpgrade;

typedef struct
{
    s32 index;
    WarRace race;
    s32 gold;
    s32 wood;
    bool features[MAX_FEATURES_COUNT];
    WarUpgrade upgrades[MAX_UPGRADES_COUNT];
} WarPlayerInfo;

#define isHuman(player) ((player)->race == WAR_RACE_HUMANS)
#define isOrc(player) ((player)->race == WAR_RACE_ORCS)
#define isNeutral(player) ((player)->race == WAR_RACE_NEUTRAL)

#define isFeatureAllowed(player, feature) \
    ((player)->features[(feature)])

#define hasAnyUpgrade(player, upgrade) \
    ((player)->upgrades[(upgrade)].allowed > 0 && \
     (player)->upgrades[(upgrade)].level > 0)
#define hasRemainingUpgrade(player, upgrade) \
    ((player)->upgrades[(upgrade)].level < (player)->upgrades[(upgrade)].allowed)
#define getUpgradeLevel(player, upgrade) \
    ((player)->upgrades[(upgrade)].level)
#define checkUpgradeLevel(player, upgrade) \
    ((player)->upgrades[(upgrade)].level <= (player)->upgrades[(upgrade)].allowed)

typedef struct
{
    bool enabled;
    f32 startTime;
    f32 duration;
    char text[100];
} WarFlashStatus;

typedef struct
{
    s32 levelInfoIndex;

    // scroll
    s32 scrollSpeed;
    bool isScrolling;
    bool wasScrolling;

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

    WarEntityList entities;
    WarEntityIdList selectedEntities;

    WarPathFinder finder;

    WarPlayerInfo players[MAX_PLAYERS_COUNT];
    WarSpriteAnimationList animations;

    WarUnitCommand command;

    WarFlashStatus flashStatus;
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

    WAR_KEY_P,
    WAR_KEY_T,
    WAR_KEY_R,
    WAR_KEY_U,
    WAR_KEY_W,
    WAR_KEY_G,

    WAR_KEY_1,
    WAR_KEY_2,
    WAR_KEY_3,
    WAR_KEY_4,
    WAR_KEY_5,
    WAR_KEY_6,
    WAR_KEY_7,
    WAR_KEY_8,
    WAR_KEY_9,
    WAR_KEY_0,

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
    // current mouse position
    vec2 pos;

    // state of the mouse buttons
    WarKeyButtonState buttons[WAR_MOUSE_COUNT];

    // state of the keys
    WarKeyButtonState keys[WAR_KEY_COUNT];

    // drag
    bool isDragging;
    bool wasDragging;
    vec2 dragPos;
    rect dragRect;
} WarInput;

typedef struct _WarContext
{
    f32 time;
    f32 deltaTime;
    u32 fps;

    bool paused;

    f32 globalScale;
    f32 globalSpeed;

    s32 originalWindowWidth;
    s32 originalWindowHeight;
    s32 windowWidth;
    s32 windowHeight;
    s32 framebufferWidth;
    s32 framebufferHeight;
    f32 devicePixelRatio;
    char windowTitle[256];
    GLFWwindow* window;

    char* warFilePath;
    WarFile* warFile;

    char* fontPath;
    WarSprite fontSprite;
    
    s32 staticEntityId;

    NVGcontext* gfx;
    NVGLUframebuffer* fb;
    
    WarResource *resources[MAX_RESOURCES_COUNT];

    WarInput input;

    WarMap* map;

    // DEBUG:
    bool editingTrees;
    WarEntity* debugForest;
    
    bool editingRoads;
    WarEntity* debugRoad;

    bool editingWalls;
    WarEntity* debugWall;

    bool editingRuins;
    WarEntity* debugRuin;
} WarContext;
