#pragma once

#define MAX_RESOURCES_COUNT 583
#define MAX_TEXTURES_COUNT 583
#define MAX_ENTITIES_COUNT 100
#define MAX_SPRITE_FRAME_COUNT 100
#define MAX_CONSTRUCTS_COUNT 100
#define MAX_CUSTOM_MAP_GOLDMINES_COUNT 10
#define MAX_CUSTOM_MAP_CONFIGURATIONS_COUNT 10
#define MAX_CUSTOM_MAP_ENTITIES_COUNT 100

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
#define MAX_FEATURES_COUNT 22
#define MAX_UPGRADES_COUNT 10
#define MAX_TILES_COUNT 1024

#define directionByIndex(i) ((WarUnitDirection)(WAR_DIRECTION_NORTH + i))

shlDefineCreateArray(s32, s32)
shlDefineFreeArray(s32, s32)

bool equalsS32(const s32 a, const s32 b)
{
    return a == b;
}

bool compareS32(const s32 a, const s32 b)
{
    return a - b;
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

bool equalsRect(const rect r1, const rect r2)
{
    return r1.x == r2.x && r1.y == r2.y &&
           r1.width == r2.width && r1.height == r2.height;
}

shlDeclareList(rectList, rect)
shlDefineList(rectList, rect)

#define rectListDefaultOptions (rectListOptions){RECT_EMPTY, equalsRect, NULL}

shlDeclareMap(SSMap, const char*, char*)
shlDefineMap(SSMap, const char*, char*)

#define SSMapDefaultOptions (SSMapOptions){NULL, strHashFNV32, strEquals, NULL}

//
// Forward references to other structs that need a reference to these ones.
// See WarButtonComponent, WarState.building.
//
struct _WarContext;
struct _WarEntity;

// Input
typedef enum
{
    WAR_MOUSE_LEFT,
    WAR_MOUSE_RIGHT,

    WAR_MOUSE_COUNT
} WarMouseButtons;

typedef enum _WarKeys
{
    WAR_KEY_NONE,

    WAR_KEY_SPACE,
    WAR_KEY_APOSTROPHE,
    WAR_KEY_ASTERISK,
    WAR_KEY_PLUS,
    WAR_KEY_COMMA,
    WAR_KEY_MINUS,
    WAR_KEY_PERIOD,
    WAR_KEY_SLASH,

    WAR_KEY_0,
    WAR_KEY_1,
    WAR_KEY_2,
    WAR_KEY_3,
    WAR_KEY_4,
    WAR_KEY_5,
    WAR_KEY_6,
    WAR_KEY_7,
    WAR_KEY_8,
    WAR_KEY_9,

    WAR_KEY_SEMICOLON,
    WAR_KEY_EQUAL,

    WAR_KEY_A,
    WAR_KEY_B,
    WAR_KEY_C,
    WAR_KEY_D,
    WAR_KEY_E,
    WAR_KEY_F,
    WAR_KEY_G,
    WAR_KEY_H,
    WAR_KEY_I,
    WAR_KEY_J,
    WAR_KEY_K,
    WAR_KEY_L,
    WAR_KEY_M,
    WAR_KEY_N,
    WAR_KEY_O,
    WAR_KEY_P,
    WAR_KEY_Q,
    WAR_KEY_R,
    WAR_KEY_S,
    WAR_KEY_T,
    WAR_KEY_U,
    WAR_KEY_V,
    WAR_KEY_W,
    WAR_KEY_X,
    WAR_KEY_Y,
    WAR_KEY_Z,

    WAR_KEY_LEFT_BRACKET,
    WAR_KEY_BACKSLASH,
    WAR_KEY_RIGHT_BRACKET,
    WAR_KEY_GRAVE_ACCENT,

    WAR_KEY_ESC,
    WAR_KEY_ENTER,
    WAR_KEY_TAB,
    WAR_KEY_BACKSPACE,
    WAR_KEY_INSERT,
    WAR_KEY_DELETE,

    WAR_KEY_RIGHT,
    WAR_KEY_LEFT,
    WAR_KEY_DOWN,
    WAR_KEY_UP,

    WAR_KEY_PAGE_UP,
    WAR_KEY_PAGE_DOWN,
    WAR_KEY_HOME,
    WAR_KEY_END,

    WAR_KEY_F1,
    WAR_KEY_F2,
    WAR_KEY_F3,
    WAR_KEY_F4,
    WAR_KEY_F5,
    WAR_KEY_F6,
    WAR_KEY_F7,
    WAR_KEY_F8,
    WAR_KEY_F9,
    WAR_KEY_F10,
    WAR_KEY_F11,
    WAR_KEY_F12,

    WAR_KEY_SHIFT,
    WAR_KEY_CTRL,
    WAR_KEY_ALT,

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

typedef struct
{
    s32 spriteWidth;
    s32 spriteHeight;
    s32 lineHeight;
    s32 advance;
    rect data[95];
} WarFontData;

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
    WAR_FILE_TYPE_UNKNOWN,
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
#define imageResourceRef(resourceIndex) createSpriteResourceRef((resourceIndex), 0, NULL)
#define imageResourceRefFromPlayer(player, hIdx, oIdx) imageResourceRef((player)->race == WAR_RACE_HUMANS ? (hIdx) : (oIdx))
#define spriteResourceRef(resourceIndex, spriteIndex) createSpriteResourceRef((resourceIndex), 1, arrayArg(s32, (spriteIndex)))
#define spriteResourceRefFromPlayer(player, hIdx, oIdx, spriteIndex) createSpriteResourceRef((player)->race == WAR_RACE_HUMANS ? (hIdx) : (oIdx), 1, arrayArg(s32, (spriteIndex)))

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
    f32 loopDelay;

    vec2 offset;
    vec2 scale;

    f32 frameDelay;
    s32List frames;
    WarSprite sprite;

    f32 animTime;
    f32 loopTime;
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
    u32 startEntitiesCount;
    WarLevelUnit startEntities[MAX_CUSTOM_MAP_ENTITIES_COUNT];
} WarCustomMapConfiguration;

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
            bool customMap;
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
            u16 tilesetType;
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

            u32 startGoldminesCount;
            WarLevelUnit startGoldmines[MAX_CUSTOM_MAP_GOLDMINES_COUNT];

            u32 startConfigurationsCount;
            WarCustomMapConfiguration startConfigurations[MAX_CUSTOM_MAP_CONFIGURATIONS_COUNT];
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

        struct
        {
            u8* data;
            s32 length;
        } audio;

        struct
        {
            u16 hotx;
            u16 hoty;
            u16 width;
            u16 height;
            u8* pixels;
        } cursor;
    };
} WarResource;

typedef enum
{
    WAR_CAMPAIGN_HUMANS_01 = 117,
    WAR_CAMPAIGN_ORCS_01,
    WAR_CAMPAIGN_HUMANS_02,
    WAR_CAMPAIGN_ORCS_02,
    WAR_CAMPAIGN_HUMANS_03,
    WAR_CAMPAIGN_ORCS_03,
    WAR_CAMPAIGN_HUMANS_04,
    WAR_CAMPAIGN_ORCS_04,
    WAR_CAMPAIGN_HUMANS_05,
    WAR_CAMPAIGN_ORCS_05,
    WAR_CAMPAIGN_HUMANS_06,
    WAR_CAMPAIGN_ORCS_06,
    WAR_CAMPAIGN_HUMANS_07,
    WAR_CAMPAIGN_ORCS_07,
    WAR_CAMPAIGN_HUMANS_08,
    WAR_CAMPAIGN_ORCS_08,
    WAR_CAMPAIGN_HUMANS_09,
    WAR_CAMPAIGN_ORCS_09,
    WAR_CAMPAIGN_HUMANS_10,
    WAR_CAMPAIGN_ORCS_10,
    WAR_CAMPAIGN_HUMANS_11,
    WAR_CAMPAIGN_ORCS_11,
    WAR_CAMPAIGN_HUMANS_12,
    WAR_CAMPAIGN_ORCS_12,
    WAR_CAMPAIGN_CUSTOM
} WarCampaignMapType;

typedef enum
{
    WAR_LEVEL_RESULT_NONE,
    WAR_LEVEL_RESULT_WIN,
    WAR_LEVEL_RESULT_LOSE
} WarLevelResult;

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

shlDeclareSet(WarEntityIdSet, WarEntityId)
shlDefineSet(WarEntityIdSet, WarEntityId)

#define WarEntityIdSetDefaultOptions (WarEntityIdSetOptions){0, hashEntityId, equalsEntityId, NULL}

typedef enum
{
    WAR_FEATURE_UNIT_FOOTMAN,
    WAR_FEATURE_UNIT_GRUNT,
    WAR_FEATURE_UNIT_PEASANT,
    WAR_FEATURE_UNIT_PEON,
    WAR_FEATURE_UNIT_CATAPULT_HUMANS,
    WAR_FEATURE_UNIT_CATAPULT_ORCS,
    WAR_FEATURE_UNIT_KNIGHT,
    WAR_FEATURE_UNIT_RAIDER,
    WAR_FEATURE_UNIT_ARCHER,
    WAR_FEATURE_UNIT_SPEARMAN,
    WAR_FEATURE_UNIT_CONJURER,
    WAR_FEATURE_UNIT_WARLOCK,
    WAR_FEATURE_UNIT_CLERIC,
    WAR_FEATURE_UNIT_NECROLYTE,
    WAR_FEATURE_UNIT_FARM_HUMANS,
    WAR_FEATURE_UNIT_FARM_ORCS,
    WAR_FEATURE_UNIT_BARRACKS_HUMANS,
    WAR_FEATURE_UNIT_BARRACKS_ORCS,
    WAR_FEATURE_UNIT_CHURCH,
    WAR_FEATURE_UNIT_TEMPLE,
    WAR_FEATURE_UNIT_TOWER_HUMANS,
    WAR_FEATURE_UNIT_TOWER_ORCS,
    WAR_FEATURE_UNIT_TOWN_HALL_HUMANS,
    WAR_FEATURE_UNIT_TOWN_HALL_ORCS,
    WAR_FEATURE_UNIT_LUMBER_MILL_HUMANS,
    WAR_FEATURE_UNIT_LUMBER_MILL_ORCS,
    WAR_FEATURE_UNIT_STABLE,
    WAR_FEATURE_UNIT_KENNEL,
    WAR_FEATURE_UNIT_BLACKSMITH_HUMANS,
    WAR_FEATURE_UNIT_BLACKSMITH_ORCS,
    WAR_FEATURE_SPELL_HEALING,
    WAR_FEATURE_SPELL_RAISE_DEAD,
    WAR_FEATURE_SPELL_FAR_SIGHT,
    WAR_FEATURE_SPELL_DARK_VISION,
    WAR_FEATURE_SPELL_INVISIBILITY,
    WAR_FEATURE_SPELL_UNHOLY_ARMOR,
    WAR_FEATURE_SPELL_SCORPION,
    WAR_FEATURE_SPELL_SPIDER,
    WAR_FEATURE_SPELL_RAIN_OF_FIRE,
    WAR_FEATURE_SPELL_POISON_CLOUD,
    WAR_FEATURE_SPELL_WATER_ELEMENTAL,
    WAR_FEATURE_SPELL_DAEMON,
    WAR_FEATURE_UNIT_ROAD,
    WAR_FEATURE_UNIT_WALL,
} WarFeatureType;

typedef enum
{
    WAR_UPGRADE_ARROWS,
    WAR_UPGRADE_SPEARS,
    WAR_UPGRADE_SWORDS,
    WAR_UPGRADE_AXES,
    WAR_UPGRADE_HORSES,
    WAR_UPGRADE_WOLVES,
    WAR_UPGRADE_SCORPIONS,
    WAR_UPGRADE_SPIDERS,
    WAR_UPGRADE_RAIN_OF_FIRE,
    WAR_UPGRADE_POISON_CLOUD,
    WAR_UPGRADE_WATER_ELEMENTAL,
    WAR_UPGRADE_DAEMON,
    WAR_UPGRADE_HEALING,
    WAR_UPGRADE_RAISE_DEAD,
    WAR_UPGRADE_FAR_SIGHT,
    WAR_UPGRADE_DARK_VISION,
    WAR_UPGRADE_INVISIBILITY,
    WAR_UPGRADE_UNHOLY_ARMOR,
    WAR_UPGRADE_SHIELD,
} WarUpgradeType;

typedef enum
{
    // spells
    WAR_SPELL_HEALING,
    WAR_SPELL_FAR_SIGHT,
    WAR_SPELL_INVISIBILITY,
    WAR_SPELL_RAIN_OF_FIRE,
    WAR_SPELL_POISON_CLOUD,
    WAR_SPELL_RAISE_DEAD,
    WAR_SPELL_DARK_VISION,
    WAR_SPELL_UNHOLY_ARMOR,

    // summons
    WAR_SUMMON_SPIDER,
    WAR_SUMMON_SCORPION,
    WAR_SUMMON_DAEMON,
    WAR_SUMMON_WATER_ELEMENTAL,
} WarSpellType;

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
    WAR_ENTITY_TYPE_CURSOR,
    WAR_ENTITY_TYPE_AUDIO,
    WAR_ENTITY_TYPE_PROJECTILE,
    WAR_ENTITY_TYPE_RAIN_OF_FIRE,
    WAR_ENTITY_TYPE_POISON_CLOUD,
    WAR_ENTITY_TYPE_SIGHT,
    WAR_ENTITY_TYPE_MINIMAP,
    WAR_ENTITY_TYPE_ANIMATION,

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
    WAR_STATE_CAST,
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
            bool checkForAttacks;
        } move;

        struct
        {
            s32 positionIndex;
            vec2List positions;
            s32 dir;
        } patrol;

        struct
        {
            // the follow state can follow an entity or a point
            s32 targetEntityId;
            vec2 targetTile;

            // the range distance (in tiles) in which the follower stops
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
        } chop;

        struct
        {
            s32 townHallId;
            bool insideBuilding;
        } deliver;

        struct
        {
            WarUnitType unitToBuild;
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
        } repairing;

        struct
        {
            WarSpellType spellType;
            WarEntityId targetEntityId;
            vec2 targetTile;
        } cast;
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
    s32 maxMana;
    s32 mana;
    s32 armor;
    s32 range;
    s32 minDamage;
    s32 rndDamage;
    s32 decay;
    bool invisible;
    bool invulnerable;
    bool hasBeenSeen;

    // index of the array of speeds of the unit
    s32 speed;

    // the current and action list for the unit
    s32 actionIndex;
    WarUnitActionList actions;

    // time remainder (in seconds) until mana is affected
    f32 manaTime;
    // time remainder (in seconds) until the unit invisiblity ceases
    f32 invisibilityTime;
    // time remainder (in seconds) until the unit invulnerability ceases
    f32 invulnerabilityTime;
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

typedef enum
{
    WAR_TEXT_ALIGN_LEFT,
    WAR_TEXT_ALIGN_CENTER,
    WAR_TEXT_ALIGN_RIGHT,
    WAR_TEXT_ALIGN_TOP,
    WAR_TEXT_ALIGN_MIDDLE,
    WAR_TEXT_ALIGN_BOTTOM,

    WAR_TEXT_ALIGN_COUNT
} WarTextAlignment;

typedef enum
{
    WAR_TEXT_WRAP_NONE,
    WAR_TEXT_WRAP_CHAR
} WarTextWrapping;

typedef enum
{
    WAR_TEXT_TRIM_NONE,
    WAR_TEXT_TRIM_SPACES,
    WAR_TEXT_TRIM_ALL
} WarTextTrimming;

typedef struct
{
    bool enabled;
    char* text;
    s32 fontIndex;
    f32 fontSize;
    f32 lineHeight;
    u8Color fontColor;
    u8Color highlightColor;
    s32 highlightIndex;
    s32 highlightCount;
    vec2 boundings;
    WarTextAlignment horizontalAlign;
    WarTextAlignment verticalAlign;
    WarTextAlignment lineAlign;
    WarTextWrapping wrapping;
    WarTextTrimming trimming;
    bool multiline;
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
    bool interactive;
    bool hot;
    bool active;
    WarKeys hotKey;
    s32 highlightIndex;
    s32 highlightCount;
    char tooltip[100];
    s32 gold;
    s32 wood;
    WarSprite normalSprite;
    WarSprite pressedSprite;
    WarClickHandler clickHandler;
} WarButtonComponent;

typedef enum
{
    WAR_MUSIC_00 = 0,
    WAR_MUSIC_01 = 1,
    WAR_MUSIC_02 = 2,
    WAR_MUSIC_03 = 3,
    WAR_MUSIC_04 = 4,
    WAR_MUSIC_05 = 5,
    WAR_MUSIC_06 = 6,
    WAR_MUSIC_07 = 7,
    WAR_MUSIC_08 = 8,
    WAR_MUSIC_09 = 9,
    WAR_MUSIC_10 = 10,
    WAR_MUSIC_11 = 11,
    WAR_MUSIC_12 = 12,
    WAR_MUSIC_13 = 13,
    WAR_MUSIC_14 = 14,
    WAR_MUSIC_15 = 15,
    WAR_MUSIC_16 = 16,
    WAR_MUSIC_17 = 17,
    WAR_MUSIC_18 = 18,
    WAR_MUSIC_19 = 19,
    WAR_MUSIC_20 = 20,
    WAR_MUSIC_21 = 21,
    WAR_MUSIC_22 = 22,
    WAR_MUSIC_23 = 23,
    WAR_MUSIC_24 = 24,
    WAR_MUSIC_25 = 25,
    WAR_MUSIC_26 = 26,
    WAR_MUSIC_27 = 27,
    WAR_MUSIC_28 = 28,
    WAR_MUSIC_29 = 29,
    WAR_MUSIC_30 = 30,
    WAR_MUSIC_31 = 31,
    WAR_MUSIC_32 = 32,
    WAR_MUSIC_33 = 33,
    WAR_MUSIC_34 = 34,
    WAR_MUSIC_35 = 35,
    WAR_MUSIC_36 = 36,
    WAR_MUSIC_37 = 37,
    WAR_MUSIC_38 = 38,
    WAR_MUSIC_39 = 39,
    WAR_MUSIC_40 = 40,
    WAR_MUSIC_41 = 41,
    WAR_MUSIC_42 = 42,
    WAR_MUSIC_43 = 43,
    WAR_MUSIC_44 = 44,
    WAR_LOGO = 472,
    WAR_INTRO_DOOR = 473,
    WAR_BUILDING = 474,
    WAR_EXPLOSION = 475,
    WAR_CATAPULT_ROCK_FIRED = 476,
    WAR_TREE_CHOPPING_1 = 477,
    WAR_TREE_CHOPPING_2 = 478,
    WAR_TREE_CHOPPING_3 = 479,
    WAR_TREE_CHOPPING_4 = 480,
    WAR_BUILDING_COLLAPSE_1 = 481,
    WAR_BUILDING_COLLAPSE_2 = 482,
    WAR_BUILDING_COLLAPSE_3 = 483,
    WAR_UI_CHIME = 484,
    WAR_UI_CLICK = 485,
    WAR_UI_CANCEL = 486,
    WAR_SWORD_ATTACK_1 = 487,
    WAR_SWORD_ATTACK_2 = 488,
    WAR_SWORD_ATTACK_3 = 489,
    WAR_FIST_ATTACK = 490,
    WAR_CATAPULT_FIRE_EXPLOSION = 491,
    WAR_FIREBALL = 492,
    WAR_ARROW_SPEAR = 493,
    WAR_ARROW_SPEAR_HIT = 494,
    WAR_ORC_HELP_1 = 495,                           // "The humans draw near"
    WAR_ORC_HELP_2 = 496,                           // "The pale dogs approach"
    WAR_HUMAN_HELP_1 = 497,                         // "The Orcs are approaching"
    WAR_HUMAN_HELP_2 = 498,                         // "There are enemies nearby"
    WAR_ORC_DEAD = 499,
    WAR_HUMAN_DEAD = 500,
    WAR_ORC_WORK_COMPLETE = 501,                    // "Work completed"
    WAR_HUMAN_WORK_COMPLETE = 502,                  // "Work completed"
    WAR_ORC_HELP_3 = 503,                           // "We are being attacked"
    WAR_ORC_HELP_4 = 504,                           // "They're destroying our city"
    WAR_HUMAN_HELP_3 = 505,                         // "We are under attack"
    WAR_HUMAN_HELP_4 = 506,                         // "The town is under attack"
    WAR_ORC_READY = 507,                            // "Your command, master"
    WAR_HUMAN_READY = 508,                          // "Your command"
    WAR_ORC_ACKNOWLEDGEMENT_1 = 509,
    WAR_ORC_ACKNOWLEDGEMENT_2 = 510,
    WAR_ORC_ACKNOWLEDGEMENT_3 = 511,
    WAR_ORC_ACKNOWLEDGEMENT_4 = 512,
    WAR_HUMAN_ACKNOWLEDGEMENT_1 = 513,              // "Yes"
    WAR_HUMAN_ACKNOWLEDGEMENT_2 = 514,              // "Yes, mylord"
    WAR_ORC_SELECTED_1 = 515,
    WAR_ORC_SELECTED_2 = 516,
    WAR_ORC_SELECTED_3 = 517,
    WAR_ORC_SELECTED_4 = 518,
    WAR_ORC_SELECTED_5 = 519,
    WAR_HUMAN_SELECTED_1 = 520,                     // "Yes?"
    WAR_HUMAN_SELECTED_2 = 521,                     // "Your will, sire?"
    WAR_HUMAN_SELECTED_3 = 522,                     // "Mylord?"
    WAR_HUMAN_SELECTED_4 = 523,                     // "My liege?"
    WAR_HUMAN_SELECTED_5 = 524,                     // "Your bidding?"
    WAR_ORC_ANNOYED_1 = 525,
    WAR_ORC_ANNOYED_2 = 526,
    WAR_ORC_ANNOYED_3 = 527,                        // "Stop poking me"
    WAR_HUMAN_ANNOYED_1 = 528,                      // "What?!"
    WAR_HUMAN_ANNOYED_2 = 529,                      // "What do you want?!"
    WAR_HUMAN_ANNOYED_3 = 530,                      // "Why do you keep touching me?!"
    WAR_DEAD_SPIDER_SCORPION = 531,
    WAR_NORMAL_SPELL = 532,
    WAR_BUILD_ROAD = 533,
    WAR_ORC_TEMPLE = 534,
    WAR_HUMAN_CHURCH = 535,
    WAR_ORC_KENNEL = 536,
    WAR_HUMAN_STABLE = 537,
    WAR_BLACKSMITH = 538,
    WAR_FIRE_CRACKLING = 539,
    WAR_CANNON = 540,
    WAR_CANNON2 = 541,
    WAR_CAMPAIGNS_HUMAN_ENDING_1 = 542,
    WAR_CAMPAIGNS_HUMAN_ENDING_2 = 543,
    WAR_CAMPAIGNS_ORC_ENDING_1 = 544,
    WAR_CAMPAIGNS_ORC_ENDING_2 = 545,
    WAR_INTRO_1 = 546,                              // "In the age of chaos..."
    WAR_INTRO_2 = 547,                              // "The kingdom of Azeroth was..."
    WAR_INTRO_3 = 548,                              // "No one knew where these..."
    WAR_INTRO_4 = 549,                              // "With an ingenious..."
    WAR_INTRO_5 = 550,                              // "Welcome to the World of Warcraft"
    WAR_CAMPAIGNS_HUMAN_01_INTRO = 551,
    WAR_CAMPAIGNS_HUMAN_02_INTRO = 552,
    WAR_CAMPAIGNS_HUMAN_03_INTRO = 553,
    WAR_CAMPAIGNS_HUMAN_04_INTRO = 554,
    WAR_CAMPAIGNS_HUMAN_05_INTRO = 555,
    WAR_CAMPAIGNS_HUMAN_06_INTRO = 556,
    WAR_CAMPAIGNS_HUMAN_07_INTRO = 557,
    WAR_CAMPAIGNS_HUMAN_08_INTRO = 558,
    WAR_CAMPAIGNS_HUMAN_09_INTRO = 559,
    WAR_CAMPAIGNS_HUMAN_10_INTRO = 560,
    WAR_CAMPAIGNS_HUMAN_11_INTRO = 561,
    WAR_CAMPAIGNS_HUMAN_12_INTRO = 562,
    WAR_CAMPAIGNS_ORC_01_INTRO = 563,
    WAR_CAMPAIGNS_ORC_02_INTRO = 564,
    WAR_CAMPAIGNS_ORC_03_INTRO = 565,
    WAR_CAMPAIGNS_ORC_04_INTRO = 566,
    WAR_CAMPAIGNS_ORC_05_INTRO = 567,
    WAR_CAMPAIGNS_ORC_06_INTRO = 568,
    WAR_CAMPAIGNS_ORC_07_INTRO = 569,
    WAR_CAMPAIGNS_ORC_08_INTRO = 570,
    WAR_CAMPAIGNS_ORC_09_INTRO = 571,
    WAR_CAMPAIGNS_ORC_10_INTRO = 572,
    WAR_CAMPAIGNS_ORC_11_INTRO = 573,
    WAR_CAMPAIGNS_ORC_12_INTRO = 574,
    WAR_HUMAN_DEFEAT = 575,                         // "Your failure in battle"
    WAR_ORC_DEFEAT = 576,                           // "You pitiful"
    WAR_ORC_VICTORY_1 = 577,                        // "The feel of bones"
    WAR_ORC_VICTORY_2 = 578,                        // "If only the worthless"
    WAR_ORC_VICTORY_3 = 579,                        // "Gaze upon the destruction"
    WAR_HUMAN_VICTORY_1 = 580,                      // "The forces of darkness"
    WAR_HUMAN_VICTORY_2 = 581,                      // "Even these children"
    WAR_HUMAN_VICTORY_3 = 582,                      // "Cheers of victory"
} WarAudioId;

typedef enum
{
    WAR_AUDIO_MIDI,
    WAR_AUDIO_WAVE
} WarAudioType;

typedef struct
{
    bool enabled;
    WarAudioType type;
    s32 resourceIndex;
    bool loop;
    f32 playbackTime;
    s32 sampleIndex;
    tml_message* firstMessage;
    tml_message* currentMessage;
} WarAudioComponent;

typedef enum
{
    WAR_CURSOR_ARROW = 263,
    WAR_CURSOR_INVALID = 264,
    WAR_CURSOR_YELLOW_CROSSHAIR = 265,
    WAR_CURSOR_RED_CROSSHAIR = 266,
    WAR_CURSOR_YELLOW_CROSSHAIR_2 = 267,
    WAR_CURSOR_MAGNIFYING_GLASS = 268,
    WAR_CURSOR_GREEN_CROSSHAIR = 269,
    WAR_CURSOR_WATCH = 270,
    WAR_CURSOR_ARROW_UP = 271,
    WAR_CURSOR_ARROW_UP_RIGHT = 272,
    WAR_CURSOR_ARROW_RIGHT = 273,
    WAR_CURSOR_ARROW_BOTTOM_RIGHT = 274,
    WAR_CURSOR_ARROW_BOTTOM = 275,
    WAR_CURSOR_ARROW_BOTTOM_LEFT = 276,
    WAR_CURSOR_ARROW_LEFT = 277,
    WAR_CURSOR_ARROW_UP_LEFT = 278
} WarCursorType;

typedef struct
{
    bool enabled;
    WarCursorType type;
    vec2 hot;
} WarCursorComponent;

typedef enum
{
    WAR_PROJECTILE_ARROW,
    WAR_PROJECTILE_CATAPULT,
    WAR_PROJECTILE_FIREBALL,
    WAR_PROJECTILE_FIREBALL_2,
    WAR_PROJECTILE_WATER_ELEMENTAL,
    WAR_PROJECTILE_RAIN_OF_FIRE
} WarProjectileType;

typedef struct
{
    bool enabled;
    WarProjectileType type;
    WarEntityId sourceEntityId;
    WarEntityId targetEntityId;
    vec2 origin;
    vec2 target;
    s32 speed;
} WarProjectileComponent;

typedef struct
{
    bool enabled;
    vec2 position;
    f32 time; // time in seconds left of the spell
    f32 damageTime; // time in seconds left to inflict damage
    char animName[30];
} WarPoisonCloudComponent;

typedef struct
{
    bool enabled;
    vec2 position;
    f32 time; // time in seconds left of the spell
} WarSightComponent;

typedef void (*WarRenderFunc)(struct _WarContext* context, struct _WarEntity* entity);
typedef s32 (*WarRenderCompareFunc)(const struct _WarEntity* e1, const struct _WarEntity* e2);

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
    WarAudioComponent audio;
    WarCursorComponent cursor;
    WarProjectileComponent projectile;
    WarPoisonCloudComponent poisonCloud;
    WarSightComponent sight;
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

uint32_t hashEntityType(const WarEntityType type)
{
    return type;
}

bool equalsEntityType(const WarEntityType t1, const WarEntityType t2)
{
    return t1 == t2;
}

void freeEntityList(WarEntityList* list)
{
    WarEntityListFree(list);
}

shlDeclareMap(WarEntityMap, WarEntityType, WarEntityList*)
shlDefineMap(WarEntityMap, WarEntityType, WarEntityList*)

uint32_t hashUnitType(const WarUnitType type)
{
    return type;
}

bool equalsUnitType(const WarUnitType t1, const WarUnitType t2)
{
    return t1 == t2;
}

shlDeclareMap(WarUnitMap, WarUnitType, WarEntityList*)
shlDefineMap(WarUnitMap, WarUnitType, WarEntityList*)

uint32_t hashEntityId(const WarEntityId id)
{
    return id;
}

shlDeclareMap(WarEntityIdMap, WarEntityId, WarEntity*)
shlDefineMap(WarEntityIdMap, WarEntityId, WarEntity*)

typedef enum
{
    MAP_TILESET_FOREST,
    MAP_TILESET_SWAMP,
    MAP_TILESET_DUNGEON
} WarMapTilesetType;

typedef enum
{
    WAR_FOG_PIECE_NONE,
    WAR_FOG_PIECE_TOP_LEFT,
    WAR_FOG_PIECE_TOP,
    WAR_FOG_PIECE_TOP_RIGHT,
    WAR_FOG_PIECE_LEFT,
    WAR_FOG_PIECE_CENTER,
    WAR_FOG_PIECE_RIGHT,
    WAR_FOG_PIECE_BOTTOM_LEFT,
    WAR_FOG_PIECE_BOTTOM,
    WAR_FOG_PIECE_BOTTOM_RIGHT
} WarFogPieceType;

typedef enum
{
    WAR_FOG_BOUNDARY_NONE,
    WAR_FOG_BOUNDARY_UNKOWN,
    WAR_FOG_BOUNDARY_FOG,
} WarFogBoundaryType;

typedef enum
{
    MAP_TILE_STATE_UNKOWN = 1,
    MAP_TILE_STATE_FOG = 2,
    MAP_TILE_STATE_VISIBLE = 4
} WarMapTileState;

typedef struct
{
    WarMapTileState state;
    WarFogPieceType type;
    WarFogBoundaryType boundary;
} WarMapTile;

typedef struct
{
    s32 allowed;
    s32 level;
} WarUpgrade;

typedef enum
{
    WAR_AI_COMMAND_STATUS_CREATED,
    WAR_AI_COMMAND_STATUS_STARTED,
    WAR_AI_COMMAND_STATUS_COMPLETED,
} WarAICommandStatus;

typedef enum
{
    WAR_AI_COMMAND_REQUEST,
    WAR_AI_COMMAND_WAIT,
    WAR_AI_COMMAND_ATTACK,
    WAR_AI_COMMAND_DEFEND,
    WAR_AI_COMMAND_SLEEP,

    WAR_AI_COMMAND_COUNT
} WarAICommandType;

typedef struct
{
    u32 id;
    WarAICommandType type;
    WarAICommandStatus status;

    union
    {
        struct
        {
            WarUnitType unitType;
            bool checkExisting;
            s32 count;
        } request;

        struct
        {
            WarUnitType unitType;
            s32 count;
        } wait;

        struct
        {
            s32 time;
        } sleep;
    };
} WarAICommand;

shlDeclareQueue(WarAICommandQueue, WarAICommand*)
shlDefineQueue(WarAICommandQueue, WarAICommand*)

shlDeclareList(WarAICommandList, WarAICommand*)
shlDefineList(WarAICommandList, WarAICommand*)

bool aiCommandEquals(const WarAICommand* command1, const WarAICommand* command2)
{
    return command1->id == command2->id;
}

void aiCommandFree(WarAICommand* command)
{
    free((void*)command);
}

#define WarAICommandListDefaultOptions ((WarAICommandListOptions){NULL, aiCommandEquals, aiCommandFree})
#define WarAICommandQueueDefaultOptions ((WarAICommandQueueOptions){NULL, aiCommandEquals, aiCommandFree})

typedef struct
{
    u32 staticCommandId;
    WarAICommandQueue currentCommands;
    WarAICommandList nextCommands;
} WarAI;

typedef struct
{
    u8 index;
    WarRace race;
    s32 gold;
    s32 wood;
    bool godMode;
    bool features[MAX_FEATURES_COUNT];
    WarUpgrade upgrades[MAX_UPGRADES_COUNT];
    WarAI* ai;
} WarPlayerInfo;

typedef WarLevelResult (*WarCheckObjectivesFunc)(struct _WarContext* context);

#define isHumanPlayer(player) ((player)->race == WAR_RACE_HUMANS)
#define isOrcPlayer(player) ((player)->race == WAR_RACE_ORCS)
#define isNeutralPlayer(player) ((player)->race == WAR_RACE_NEUTRAL)

#define isFeatureAllowed(player, feature) ((player)->features[(feature)/2])
#define setFeatureAllowed(player, feature, allowed) ((player)->features[(feature/2)] = (allowed))

#define incrementUpgradeLevel(player, upgrade) ((player)->upgrades[(upgrade)/2].level++)
#define hasAnyUpgrade(player, upgrade) \
    ((player)->upgrades[(upgrade)/2].allowed > 0 && \
     (player)->upgrades[(upgrade)/2].level > 0)
#define hasRemainingUpgrade(player, upgrade) \
    ((player)->upgrades[(upgrade)/2].level < (player)->upgrades[(upgrade)/2].allowed)
#define getUpgradeLevel(player, upgrade) \
    ((player)->upgrades[(upgrade)/2].level)
#define checkUpgradeLevel(player, upgrade) \
    ((player)->upgrades[(upgrade)/2].level <= (player)->upgrades[(upgrade)/2].allowed)
#define setUpgradeAllowed(player, upgrade, value) \
    ((player)->upgrades[(upgrade)/2].allowed = (value))

#define STATUS_TEXT_MAX_LENGTH 40
#define CHEAT_TEXT_MAX_LENGTH 32
#define CHEAT_FEEDBACK_TEXT_MAX_LENGTH 50

typedef enum
{
    WAR_CHEAT_NONE,

    // original cheats
    WAR_CHEAT_GOLD,         // Pot of Gold: Gives 10000 gold and 5000 lumber to the player
    WAR_CHEAT_SPELLS,       // Eye of newt: All spells are now be able to be casted
    WAR_CHEAT_UPGRADES,     // Iron forge: Research all upgrades
    WAR_CHEAT_END,          // Ides of March: Brings player to final campaign sequence
    WAR_CHEAT_ENABLE,       // Corwin of Amber: Enables cheats
    WAR_CHEAT_GOD_MODE,     // There can be only one: Your units stop taking damage and deal 255 Damage
    WAR_CHEAT_WIN,          // Yours truly: Win current mission
    WAR_CHEAT_LOSS,         // Crushing defeat: Instant loss
    WAR_CHEAT_FOG,          // Sally Shears: Disables fog of war
    WAR_CHEAT_SKIP_HUMAN,   // Human #: Skip to human level (enter 1-12 for #)
    WAR_CHEAT_SKIP_ORC,     // Orc #: Skip to orc level (enter 1-12 for #)
    WAR_CHEAT_SPEED,        // Hurry up guys: Speeds up building/unit production,

    // custom cheats
    WAR_CHEAT_MUSIC,        // Music #: Set volume music (enter 1-45 for #)
                            // Music {on|off}: Enable or disable music
    WAR_CHEAT_SOUND,        // Sound {on|off}: Enable or disable sounds
    WAR_CHEAT_MUSIC_VOL,    // Music volume #: Set volume of music (enter 0-100 for #)
    WAR_CHEAT_SOUND_VOL,    // Sound volume #: Set volume of sounds (enter 0-100 for #)
    WAR_CHEAT_GLOBAL_SCALE, // Scale #: Set global scale (enter 1-5 for #)
    WAR_CHEAT_GLOBAL_SPEED, // Speed #: Set global speed (enter 1-5 for #)
    WAR_CHEAT_EDIT,         // Edit #: Make things editable (enter trees, walls, ruins, roads for #)
    WAR_CHEAT_ADD_UNIT,     // Add unit #: Add unit to the map (enter unit name for #)
    WAR_CHEAT_RAIN_OF_FIRE, // Rain of fire: Player can throw rain of fire projectiles

    WAR_CHEAT_COUNT
} WarCheat;

typedef struct
{
    bool enabled;
    f32 startTime;
    f32 duration;
    char text[STATUS_TEXT_MAX_LENGTH];
} WarFlashStatus;

typedef struct
{
    bool enabled;
    bool visible;
    s32 position;
    char text[CHEAT_TEXT_MAX_LENGTH];
    bool feedback;
    f32 feedbackTime;
    char feedbackText[CHEAT_FEEDBACK_TEXT_MAX_LENGTH];
} WarCheatStatus;

typedef enum
{
    WAR_SPEED_SLOWEST,
    WAR_SPEED_SLOW,
    WAR_SPEED_NORMAL,
    WAR_SPEED_FASTER,
    WAR_SPEED_FASTEST
} WarMapSpeed;

typedef struct
{
    WarMapSpeed gameSpeed;
    s32 musicVol;
    s32 sfxVol;
    WarMapSpeed mouseScrollSpeed;
    WarMapSpeed keyScrollSpeed;
} WarMapSettings;

typedef struct
{
    s32 staticEntityId;

    WarEntityList entities;
    WarEntityMap entitiesByType;
    WarUnitMap unitsByType;
    WarEntityIdMap entitiesById;
    WarEntityList uiEntities;
} WarEntityManager;

typedef struct
{
    bool playing;
    bool custom;
    WarLevelResult result;

    s32 levelInfoIndex;
    f32 objectivesTime;

    // scroll
    bool isScrolling;
    bool wasScrolling;

    WarMapSettings settings;
    WarMapSettings settings2;

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
    rect menuPanel;
    rect messagePanel;
    rect saveLoadPanel;

    WarSprite sprite;
    WarSprite minimapSprite;
    WarSprite blackSprite;

    WarMapTilesetType tilesetType;
    WarMapTile tiles[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];

    WarEntityManager entityManager;
    WarEntityIdList selectedEntities;

    WarEntity* forest;
    WarEntity* wall;
    WarEntity* road;
    WarEntity* ruin;

    bool editingTrees;
    bool editingWalls;
    bool editingRoads;
    bool editingRuins;
    bool editingRainOfFire;
    bool addingUnit;
    WarUnitType addingUnitType;

    bool hurryUp;
    bool fowEnabled;

    WarPathFinder finder;
    WarUnitCommand command;
    WarFlashStatus flashStatus;
    WarCheatStatus cheatStatus;
    WarPlayerInfo players[MAX_PLAYERS_COUNT];
} WarMap;

typedef enum
{
    WAR_SCENE_DOWNLOAD,
    WAR_SCENE_BLIZZARD,
    WAR_SCENE_MAIN_MENU,
    WAR_SCENE_BRIEFING,

    WAR_SCENE_COUNT
} WarSceneType;

typedef enum
{
    WAR_SCENE_DOWNLOAD_DOWNLOAD,
    WAR_SCENE_DOWNLOAD_CONFIRM,
    WAR_SCENE_DOWNLOAD_DOWNLOADING,
    WAR_SCENE_DOWNLOAD_DOWNLOADED,
    WAR_SCENE_DOWNLOAD_FILE_LOADED,
    WAR_SCENE_DOWNLOAD_FAILED,
} WarSceneDownloadState;

typedef struct
{
    WarSceneType type;
    WarEntityManager entityManager;

    WarCheatStatus cheatStatus;

    union
    {
        struct
        {
            WarSceneDownloadState status;
        } download;

        struct
        {
            f32 time;
        } blizzard;

        struct
        {
            WarRace yourRace;
            WarRace enemyRace;
            s32 customMap;
        } menu;

        struct
        {
            f32 time;
            WarRace race;
            WarCampaignMapType mapType;
        } briefing;
    };
} WarScene;

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

    WarFile* warFile;
    WarResource* resources[MAX_RESOURCES_COUNT];
    WarSprite fontSprites[2];

    NVGcontext* gfx;
    // NVGLUframebuffer* fb;

    ma_device sfx;
    tsf* soundFont;
    // this is shortcut to disable all audios in the map
    // to avoid crashes when freeing the map and the audio thread
    // trying to reproduce audios
    bool audioEnabled;
    bool musicEnabled;
    bool soundEnabled;
    f32 musicVolume;
    f32 soundVolume;

    bool cheatsEnabled;

    // this is a mutex used to make the deletion of the entities thread-safe
    // since the audio thread will delete audio entities, that could lead
    // to inconsistent states in the internal lists when the game try to also
    // delete other entities.
    pthread_mutex_t __mutex;

    WarInput input;

    f32 transitionDelay;
    WarScene* scene;
    WarScene* nextScene;
    WarMap* map;
    WarMap* nextMap;
} WarContext;
