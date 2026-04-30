#pragma once

#include "war_sprites.h"
#include "war_units.h"
#include "war_database.h"

struct _WarRawResource
{
    bool placeholder;
    bool compressed;
    u32 offset;
    u32 compressedLength;
    u32 length;
    u32 index;
    u8 *data;
};

enum _WarFileType
{
    WAR_FILE_TYPE_UNKNOWN,
    WAR_FILE_TYPE_DEMO,
    WAR_FILE_TYPE_RETAIL
};

struct _WarFile
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
};

enum _WarResourceType
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
};

enum _WarLevelInfoType
{
    WAR_LEVEL_TYPE_UNKOWN,
    WAR_LEVEL_TYPE_1,
    WAR_LEVEL_TYPE_2,
    WAR_LEVEL_TYPE_3
};

struct _WarLevelUnit
{
    u8 x, y;
    WarUnitType type;
    u8 player;
    WarResourceKind resourceKind;
    u16 amount;
};

enum _WarConstructType
{
    WAR_CONSTRUCT_UNKOWN,
    WAR_CONSTRUCT_ROAD,
    WAR_CONSTRUCT_WALL
};

struct _WarLevelConstruct
{
    WarConstructType type;
    u8 x1, y1;
    u8 x2, y2;
    u8 player;
};

struct _WarTilesetTile
{
    struct
    {
        u16 index;
        bool flipX;
        bool flipY;
    } tiles[4];
};

struct _WarCustomMapConfiguration
{
    u32 startEntitiesCount;
    WarLevelUnit startEntities[MAX_CUSTOM_MAP_ENTITIES_COUNT];
};

struct _WarResource
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
};

WarResource* wres_getOrCreateResource(WarContext* context, s32 index);
void wres_loadResource(WarContext* context, DatabaseEntry* entry);
