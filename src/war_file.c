#include <string.h>

#include "war_file.h"
#include "war_log.h"

WarFile* wfile_loadWarFile(WarContext* context, StringView filePath, const bool skipDecompress[])
{
    NOT_USED(context);

    TracyCZoneN(ctx, "wfile_loadWarFile", true);

    SDL_IOStream *stream = SDL_IOFromFile(wsv_data(filePath), "rb");
    if (!stream)
    {
        logError("Couldn't process the DATA.WAR file. The file doesn't exists at %s.", wsv_data(filePath));
        TracyCZoneEnd(ctx);
        return NULL;
    }

    s64 fileLength = SDL_GetIOSize(stream);

    u8* fileBuffer = (u8*)wm_alloc(fileLength);
    if (!fileBuffer)
    {
        logError("Couldn't allocate memory for the DATA.WAR file.");
        TracyCZoneEnd(ctx);
        return NULL;
    }

    size_t bytesRead = SDL_ReadIO(stream, fileBuffer, (size_t)fileLength);
    if (bytesRead != (size_t)fileLength)
    {
        logError("Couldn't read the DATA.WAR file. Wanted to read %lld bytes. Error: %s\n", fileLength, SDL_GetError());
        wm_free(fileBuffer);
        SDL_CloseIO(stream);
        TracyCZoneEnd(ctx);
        return NULL;
    }

    SDL_CloseIO(stream);

    WarFile *warFile = (WarFile*)wm_alloc(sizeof(WarFile));
    warFile->archiveID       = *(u32*)(fileBuffer + 0);
    warFile->numberOfEntries = *(u32*)(fileBuffer + 4);

    switch (warFile->archiveID)
    {
        case 0x18:
        case 0x1A:
        {
            warFile->type = WAR_FILE_TYPE_RETAIL;
            break;
        }

        case 0x19:
        {
            warFile->type = WAR_FILE_TYPE_DEMO;
            break;
        }

        default:
        {
            warFile->type = WAR_FILE_TYPE_UNKNOWN;
            break;
        }
    }

    if (warFile->type == WAR_FILE_TYPE_UNKNOWN)
    {
        logError("Couldn't process the DATA.WAR file. The file type %u is not the RETAIL or DEMO version of the game.", warFile->archiveID);
        wm_free(warFile);
        SDL_CloseIO(stream);
        TracyCZoneEnd(ctx);
        return NULL;
    }

    memcpy(warFile->offsets, fileBuffer + 8, warFile->numberOfEntries * sizeof(u32));

    for (s32 i = 0; i < (s32)warFile->numberOfEntries; ++i)
    {
        // placeholders in demo versions
        if (warFile->offsets[i] == 0xFFFFFFFF ||
            warFile->offsets[i] == 0x00000000)
        {
            warFile->resources[i].placeholder = true;
            continue;
        }

        if (skipDecompress && skipDecompress[i])
        {
            warFile->resources[i].placeholder = true;
            continue;
        }

        u32 compressedLength;

        if (i == (s32)warFile->numberOfEntries - 1)
        {
            compressedLength = (u32)fileLength - warFile->offsets[i];
        }
        else
        {
            s32 j = i + 1;
            u32 nextOffset = warFile->offsets[j];
            while (nextOffset == 0xFFFFFFFF || nextOffset == 0x00000000)
            {
                if (j + 1 >= (s32)warFile->numberOfEntries)
                {
                    nextOffset = (u32)fileLength;
                    break;
                }

                nextOffset = warFile->offsets[++j];
            }

            compressedLength = nextOffset - warFile->offsets[i];
        }

        const u8* ptr = fileBuffer + warFile->offsets[i];
        u32 size = *(const u32*)ptr;
        ptr += 4;

        u32 length = size & 0x1FFFFFFF;
        bool compressed = (size & 0xE0000000) != 0;

        u8 *data = (u8*)wm_alloc(length * sizeof(u8));
        if (!compressed)
        {
            memcpy(data, ptr, length);
        }
        else
        {
/*
decompression algorithm as described in
The File Formats of WarCraft: Orcs & Humans December 4, 2015
http://www.blizzardarchive.com/pub/Misc/Wc1Book_041215.pdf

for o:=0 to 4095 do
bufwin[o] := 0; // init our 4096 byte buffer with zero

i :=0;

while (i < filesize) do
begin;
warfile.read(cmask, 1);
i := i + 1;
for a:=0 to 7 do
begin;
if (cmask mod 2 = 1) then // uncompressed byte
begin;
    warfile.read(bufbyte, 1);
    bufwin[tmp.position mod 4096] := bufbyte;
    tmp.write(bufbyte, 1);
    i := i + 1;
end;
else // compressed block begin;
    warfile.read(offset, 2);
    numbytes := offset div 4096;
    offset := offset mod 4096;
    i := i + 2;
    for m := 0 to numbytes + 2 do
    begin;
        bufbyte := bufwin[(offset + m) mod 4096];
        bufwin[(tmp.position) mod 4096] := bufbyte;
        tmp.write(bufbyte, 1);
    end;
end;
cmask := cmask div 2;
end;
end;

tmp.size := finalsize; // Crop the file, just in case
*/
#define BUFWIN_SIZE 4096

            u8 bufwin[BUFWIN_SIZE];
            memset(bufwin, 0, BUFWIN_SIZE);

            s32 b = 0;
            s32 bufwinPos = 0;

            while (b < (s32)compressedLength && bufwinPos < (s32)length)
            {
                u8 cmask = *ptr++;
                b++;

                for (s32 a = 0; a < 8 && bufwinPos < (s32)length; ++a)
                {
                    if (cmask % 2 == 1) // uncompressed byte
                    {
                        u8 bufbyte = *ptr++;
                        b++;

                        bufwin[bufwinPos % BUFWIN_SIZE] = bufbyte;
                        data[bufwinPos] = bufbyte;
                        bufwinPos++;
                    }
                    else // compressed block begin
                    {
                        u16 offset = *(const u16*)ptr;
                        ptr += 2;
                        u16 numbytes = offset / BUFWIN_SIZE;
                        offset = offset % BUFWIN_SIZE;
                        b += 2;

                        for (s32 m = 0; m <= numbytes + 2 && bufwinPos < (s32)length; ++m)
                        {
                            u8 bufbyte = bufwin[(offset + m) % BUFWIN_SIZE];

                            bufwin[bufwinPos % BUFWIN_SIZE] = bufbyte;
                            data[bufwinPos] = bufbyte;
                            bufwinPos++;
                        }
                    }

                    cmask /= 2;
                }
            }
        }

        warFile->resources[i].placeholder = false;
        warFile->resources[i].index = i;
        warFile->resources[i].offset = warFile->offsets[i];
        warFile->resources[i].compressed = compressed;
        warFile->resources[i].compressedLength = compressedLength;
        warFile->resources[i].length = length;
        warFile->resources[i].data = data;
    }

    TracyCZoneEnd(ctx);
    return warFile;

#undef BUFWIN_SIZE
}

// -----------------------------------------------------------------------
// .w1m custom map loading
// -----------------------------------------------------------------------
#define WFILE_W1M_MAGIC   0x57314D41u
#define WFILE_W1M_VERSION 1u

// Read a .w1m stream into a pre-allocated WarResource and two u16 tile arrays.
// path_ and stream_ are the parameter names expected by the macros above.
// Returns true on success; closes stream_ and returns false on any error.
bool wfile_loadWarMapFile(StringView filePath, WarResource* levelInfoRes, WarResource* visualInfoRes, WarResource* passableInfoRes)
{
    assert(levelInfoRes);
    assert(visualInfoRes);
    assert(passableInfoRes);

// WFILE_READ_FIELD: read one field of (sz_) bytes from stream_ into (ptr_).
// On failure, closes the stream and returns false.
#define WFILE_READ_FIELD(ptr_, sz_) \
    if (SDL_ReadIO(stream, (ptr_), (sz_)) != (size_t)(sz_)) \
    { \
        logError("wfile_loadWarMapFile: unexpected EOF in '%.*s'", \
                 (int)filePath.length, filePath.data); \
        SDL_CloseIO(stream); \
        return false; \
    }

// WFILE_READ_ARRAY: read (cnt_) elements of (esz_) bytes each from stream_ into (ptr_).
#define WFILE_READ_ARRAY(ptr_, esz_, cnt_) \
    if ((cnt_) > 0 && SDL_ReadIO(stream, (ptr_), (size_t)(esz_) * (size_t)(cnt_)) != (size_t)(esz_) * (size_t)(cnt_)) \
    { \
        logError("wfile_loadWarMapFile: unexpected EOF reading array in '%.*s'", \
                 (int)filePath.length, filePath.data); \
        SDL_CloseIO(stream); \
        return false; \
    }

    SDL_IOStream* stream = SDL_IOFromFile(wsv_data(filePath), "rb");
    if (!stream)
    {
        logError("wfile_loadWarMapFile: cannot open '%.*s'",
                 (int)filePath.length, filePath.data);
        return false;
    }

    WarMapFileHeader hdr;
    WFILE_READ_FIELD(&hdr, sizeof(hdr));

    if (hdr.magic != WFILE_W1M_MAGIC)
    {
        logError("wfile_loadWarMapFile: bad magic 0x%08X in '%.*s' (expected 0x%08X)",
                 hdr.magic, (int)filePath.length, filePath.data, WFILE_W1M_MAGIC);
        SDL_CloseIO(stream);
        return false;
    }
    if (hdr.version != WFILE_W1M_VERSION)
    {
        logError("wfile_loadWarMapFile: unsupported version %u in '%.*s' (expected %u)",
                 hdr.version, (int)filePath.length, filePath.data, WFILE_W1M_VERSION);
        SDL_CloseIO(stream);
        return false;
    }

    levelInfoRes->type = WAR_RESOURCE_TYPE_LEVEL_INFO;

    u8 allowedHuman  = 0u;
    u8 allowedOrcs   = 0u;
    u8 customMapByte = 0u;
    u8 pad           = 0u;

    WFILE_READ_FIELD(&levelInfoRes->levelInfo.allowId,        sizeof(u32));
    WFILE_READ_FIELD(&allowedHuman,                           sizeof(u8));
    WFILE_READ_FIELD(&allowedOrcs,                            sizeof(u8));
    WFILE_READ_FIELD(&customMapByte,                          sizeof(u8));
    WFILE_READ_FIELD(&pad,                                    sizeof(u8));
    levelInfoRes->levelInfo.allowedHumanUnits = (allowedHuman  != 0);
    levelInfoRes->levelInfo.allowedOrcsUnits  = (allowedOrcs   != 0);
    levelInfoRes->levelInfo.customMap         = (customMapByte != 0);

    WFILE_READ_FIELD(levelInfoRes->levelInfo.allowedFeatures, sizeof(levelInfoRes->levelInfo.allowedFeatures));
    WFILE_READ_FIELD(levelInfoRes->levelInfo.allowedUpgrades, sizeof(levelInfoRes->levelInfo.allowedUpgrades));
    WFILE_READ_FIELD(&levelInfoRes->levelInfo.startX,         sizeof(u16));
    WFILE_READ_FIELD(&levelInfoRes->levelInfo.startY,         sizeof(u16));
    WFILE_READ_FIELD(&levelInfoRes->levelInfo.tilesetType,    sizeof(u16));
    WFILE_READ_FIELD(&pad, sizeof(u8));
    WFILE_READ_FIELD(&pad, sizeof(u8));
    WFILE_READ_FIELD(levelInfoRes->levelInfo.lumber,          sizeof(levelInfoRes->levelInfo.lumber));
    WFILE_READ_FIELD(levelInfoRes->levelInfo.gold,            sizeof(levelInfoRes->levelInfo.gold));
    WFILE_READ_FIELD(levelInfoRes->levelInfo.races,           sizeof(levelInfoRes->levelInfo.races));
    WFILE_READ_FIELD(&pad, sizeof(u8));
    WFILE_READ_FIELD(&pad, sizeof(u8));
    WFILE_READ_FIELD(&pad, sizeof(u8));
    WFILE_READ_FIELD(levelInfoRes->levelInfo.objectives,      MAX_OBJECTIVES_LENGTH);
    levelInfoRes->levelInfo.objectives[MAX_OBJECTIVES_LENGTH - 1] = '\0';

    WFILE_READ_FIELD(&levelInfoRes->levelInfo.startEntitiesCount, sizeof(u32));
    if (levelInfoRes->levelInfo.startEntitiesCount > MAX_ENTITIES_COUNT)
    {
        logError("wfile_loadWarMapFile: startEntitiesCount %u too large in '%.*s'",
                 levelInfoRes->levelInfo.startEntitiesCount,
                 (int)filePath.length, filePath.data);
        SDL_CloseIO(stream);
        return false;
    }
    levelInfoRes->levelInfo.startEntities = (WarLevelUnit*)wm_alloc(MAX_ENTITIES_COUNT * sizeof(WarLevelUnit));
    assert(levelInfoRes->levelInfo.startEntities);
    WFILE_READ_ARRAY(levelInfoRes->levelInfo.startEntities, sizeof(WarLevelUnit), levelInfoRes->levelInfo.startEntitiesCount);

    WFILE_READ_FIELD(&levelInfoRes->levelInfo.startRoadsCount, sizeof(u32));
    if (levelInfoRes->levelInfo.startRoadsCount > MAX_CONSTRUCTS_COUNT)
    {
        logError("wfile_loadWarMapFile: startRoadsCount %u too large in '%.*s'",
                 levelInfoRes->levelInfo.startRoadsCount,
                 (int)filePath.length, filePath.data);
        SDL_CloseIO(stream);
        return false;
    }
    levelInfoRes->levelInfo.startRoads = (WarLevelConstruct*)wm_alloc(MAX_CONSTRUCTS_COUNT * sizeof(WarLevelConstruct));
    assert(levelInfoRes->levelInfo.startRoads);
    WFILE_READ_ARRAY(levelInfoRes->levelInfo.startRoads, sizeof(WarLevelConstruct), levelInfoRes->levelInfo.startRoadsCount);

    WFILE_READ_FIELD(&levelInfoRes->levelInfo.startWallsCount, sizeof(u32));
    if (levelInfoRes->levelInfo.startWallsCount > MAX_CONSTRUCTS_COUNT)
    {
        logError("wfile_loadWarMapFile: startWallsCount %u too large in '%.*s'",
                 levelInfoRes->levelInfo.startWallsCount,
                 (int)filePath.length, filePath.data);
        SDL_CloseIO(stream);
        return false;
    }
    levelInfoRes->levelInfo.startWalls = (WarLevelConstruct*)wm_alloc(MAX_CONSTRUCTS_COUNT * sizeof(WarLevelConstruct));
    assert(levelInfoRes->levelInfo.startWalls);
    WFILE_READ_ARRAY(levelInfoRes->levelInfo.startWalls, sizeof(WarLevelConstruct), levelInfoRes->levelInfo.startWallsCount);

    WFILE_READ_FIELD(&levelInfoRes->levelInfo.startGoldminesCount, sizeof(u32));
    if (levelInfoRes->levelInfo.startGoldminesCount > MAX_CUSTOM_MAP_GOLDMINES_COUNT)
    {
        logError("wfile_loadWarMapFile: startGoldminesCount %u too large in '%.*s'",
                 levelInfoRes->levelInfo.startGoldminesCount,
                 (int)filePath.length, filePath.data);
        SDL_CloseIO(stream);
        return false;
    }
    levelInfoRes->levelInfo.startGoldmines = (WarLevelUnit*)wm_alloc(MAX_CUSTOM_MAP_GOLDMINES_COUNT * sizeof(WarLevelUnit));
    assert(levelInfoRes->levelInfo.startGoldmines);
    WFILE_READ_ARRAY(levelInfoRes->levelInfo.startGoldmines, sizeof(WarLevelUnit), levelInfoRes->levelInfo.startGoldminesCount);

    WFILE_READ_FIELD(&levelInfoRes->levelInfo.startConfigurationsCount, sizeof(u32));
    if (levelInfoRes->levelInfo.startConfigurationsCount > MAX_CUSTOM_MAP_CONFIGURATIONS_COUNT)
    {
        logError("wfile_loadWarMapFile: startConfigurationsCount %u too large in '%.*s'",
                 levelInfoRes->levelInfo.startConfigurationsCount,
                 (int)filePath.length, filePath.data);
        SDL_CloseIO(stream);
        return false;
    }
    WFILE_READ_ARRAY(levelInfoRes->levelInfo.startConfigurations, sizeof(WarCustomMapConfiguration), levelInfoRes->levelInfo.startConfigurationsCount);

    visualInfoRes->levelVisual.data = (u16*)wm_alloc(MAP_TILES_WIDTH * MAP_TILES_HEIGHT * sizeof(u16));
    assert(visualInfoRes->levelVisual.data);

    passableInfoRes->levelPassable.data = (u16*)wm_alloc(MAP_TILES_WIDTH * MAP_TILES_HEIGHT * sizeof(u16));
    assert(passableInfoRes->levelPassable.data);

    WFILE_READ_ARRAY(visualInfoRes->levelVisual.data,     sizeof(u16), MAP_TILES_WIDTH * MAP_TILES_HEIGHT);
    WFILE_READ_ARRAY(passableInfoRes->levelPassable.data, sizeof(u16), MAP_TILES_WIDTH * MAP_TILES_HEIGHT);

    SDL_CloseIO(stream);

    return true;

#undef WFILE_READ_FIELD
#undef WFILE_READ_ARRAY
}
