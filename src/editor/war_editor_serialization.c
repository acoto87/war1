#include "war_editor_serialization.h"
#include "war_editor_map.h"

// -----------------------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------------------

// WSAVE_WRITE: write exactly one element of (sz) bytes from (ptr) to (f).
// On failure, logs an error and returns false from the enclosing function.
#define WSAVE_WRITE(ptr, sz) \
    if (fwrite((ptr), (sz), 1, (f)) != 1) \
    { \
        logError("wesave_saveMap: write error on '%s'", path); \
        fclose(f); \
        return false; \
    }

// WLOAD_FIELD: read exactly one element of (sz) bytes into (ptr) from (f).
// On failure, logs an error and returns false from the enclosing function.
#define WLOAD_FIELD(ptr, sz) \
    if (fread((ptr), (sz), 1, (f)) != 1) \
    { \
        logError("wesave_loadMap: unexpected EOF in '%s'", path); \
        fclose(f); \
        return false; \
    }

// WLOAD_ARRAY: read (cnt) elements of (esz) bytes each into (ptr) from (f).
// Skips fread when cnt == 0 (valid; fread with count=0 would return 0, not 1).
// On failure, logs an error and returns false from the enclosing function.
#define WLOAD_ARRAY(ptr, esz, cnt) \
    if ((cnt) > 0 && fread((ptr), (esz), (size_t)(cnt), (f)) != (size_t)(cnt)) \
    { \
        logError("wesave_loadMap: unexpected EOF reading array in '%s'", path); \
        fclose(f); \
        return false; \
    }

// -----------------------------------------------------------------------
// Save
// -----------------------------------------------------------------------

bool wesave_saveMap(const char* path, const WarEditorContext* ctx)
{
    if (!ctx->map)
    {
        logError("wesave_saveMap: ctx->map is NULL");
        return false;
    }

    FILE* f = fopen(path, "wb");
    if (!f)
    {
        logError("wesave_saveMap: cannot open '%s' for writing: %s", path, strerror(errno));
        return false;
    }

    // --- Header ---
    W1MFileHeader header;
    memset(&header, 0, sizeof(header));
    header.magic   = W1M_MAGIC;
    header.version = W1M_VERSION;
    WSAVE_WRITE(&header, sizeof(header));

    const WarEditorMap* m = ctx->map;

    // --- Fixed-size levelInfo fields (written in documented byte order) ---
    u8 customMap = 1u;
    u8 pad       = 0u;

    WSAVE_WRITE(&m->allowId,              sizeof(u32));
    WSAVE_WRITE(&m->allowedHumanUnits,    sizeof(u8));
    WSAVE_WRITE(&m->allowedOrcsUnits,     sizeof(u8));
    WSAVE_WRITE(&customMap,               sizeof(u8));
    WSAVE_WRITE(&pad,                     sizeof(u8));           // byte 23: pad
    WSAVE_WRITE(m->allowedFeatures,       sizeof(m->allowedFeatures));
    WSAVE_WRITE(m->allowedUpgrades,       sizeof(m->allowedUpgrades));
    WSAVE_WRITE(&m->startX,               sizeof(u16));
    WSAVE_WRITE(&m->startY,               sizeof(u16));
    WSAVE_WRITE(&m->tilesetType,          sizeof(u16));
    WSAVE_WRITE(&pad,                     sizeof(u8));           // 2-byte alignment pad
    WSAVE_WRITE(&pad,                     sizeof(u8));
    WSAVE_WRITE(m->lumber,                sizeof(m->lumber));
    WSAVE_WRITE(m->gold,                  sizeof(m->gold));
    WSAVE_WRITE(m->races,                 sizeof(m->races));
    WSAVE_WRITE(&pad,                     sizeof(u8));           // 3-byte pad → align to 4
    WSAVE_WRITE(&pad,                     sizeof(u8));
    WSAVE_WRITE(&pad,                     sizeof(u8));
    WSAVE_WRITE(m->objectives,            MAX_OBJECTIVES_LENGTH);

    // --- Variable-length arrays ---
    WSAVE_WRITE(&m->startEntitiesCount, sizeof(u32));
    if (m->startEntitiesCount > 0)
    {
        WSAVE_WRITE(m->startEntities, sizeof(WarLevelUnit) * m->startEntitiesCount);
    }

    WSAVE_WRITE(&m->startRoadsCount, sizeof(u32));
    if (m->startRoadsCount > 0)
    {
        WSAVE_WRITE(m->startRoads, sizeof(WarLevelConstruct) * m->startRoadsCount);
    }

    WSAVE_WRITE(&m->startWallsCount, sizeof(u32));
    if (m->startWallsCount > 0)
    {
        WSAVE_WRITE(m->startWalls, sizeof(WarLevelConstruct) * m->startWallsCount);
    }

    WSAVE_WRITE(&m->startGoldminesCount, sizeof(u32));
    if (m->startGoldminesCount > 0)
    {
        WSAVE_WRITE(m->startGoldmines, sizeof(WarLevelUnit) * m->startGoldminesCount);
    }

    WSAVE_WRITE(&m->startConfigurationsCount, sizeof(u32));
    if (m->startConfigurationsCount > 0)
    {
        WSAVE_WRITE(m->startConfigurations,
                    sizeof(WarCustomMapConfiguration) * m->startConfigurationsCount);
    }

    // --- Tile data ---
    WSAVE_WRITE(m->visualData,   sizeof(u16) * MAP_TILES_WIDTH * MAP_TILES_HEIGHT);
    WSAVE_WRITE(m->passableData, sizeof(u16) * MAP_TILES_WIDTH * MAP_TILES_HEIGHT);

    fclose(f);

    logInfo("wesave_saveMap: saved '%s' (entities=%u, tilesetType=%d)",
            path, m->startEntitiesCount, (s32)m->tilesetType);
    return true;
}

#undef WSAVE_WRITE

// -----------------------------------------------------------------------
// Load
// -----------------------------------------------------------------------

bool wesave_loadMap(const char* path, WarEditorContext* ctx)
{
    FILE* f = fopen(path, "rb");
    if (!f)
    {
        logError("wesave_loadMap: cannot open '%s': %s", path, strerror(errno));
        return false;
    }

    // --- Validate header ---
    W1MFileHeader header;
    WLOAD_FIELD(&header, sizeof(header));

    if (header.magic != W1M_MAGIC)
    {
        logError("wesave_loadMap: bad magic 0x%08X in '%s' (expected 0x%08X)",
                 header.magic, path, W1M_MAGIC);
        fclose(f);
        return false;
    }
    if (header.version != W1M_VERSION)
    {
        logError("wesave_loadMap: unsupported version %u in '%s' (expected %u)",
                 header.version, path, W1M_VERSION);
        fclose(f);
        return false;
    }

    // --- Replace current map with a fresh empty map ---
    wemap_free(ctx);
    if (!wemap_createEmpty(ctx))
    {
        logError("wesave_loadMap: wemap_createEmpty failed");
        fclose(f);
        return false;
    }

    WarEditorMap* m = ctx->map;

    // --- Fixed-size levelInfo fields ---
    u8 customMapByte = 0u;
    u8 pad           = 0u;

    WLOAD_FIELD(&m->allowId,              sizeof(u32));
    WLOAD_FIELD(&m->allowedHumanUnits,    sizeof(u8));
    WLOAD_FIELD(&m->allowedOrcsUnits,     sizeof(u8));
    WLOAD_FIELD(&customMapByte,           sizeof(u8));
    m->customMap = (customMapByte != 0);
    WLOAD_FIELD(&pad,                     sizeof(u8));           // byte 23: pad
    WLOAD_FIELD(m->allowedFeatures,       sizeof(m->allowedFeatures));
    WLOAD_FIELD(m->allowedUpgrades,       sizeof(m->allowedUpgrades));
    WLOAD_FIELD(&m->startX,               sizeof(u16));
    WLOAD_FIELD(&m->startY,               sizeof(u16));
    WLOAD_FIELD(&m->tilesetType,          sizeof(u16));
    WLOAD_FIELD(&pad,                     sizeof(u8));           // 2-byte alignment pad
    WLOAD_FIELD(&pad,                     sizeof(u8));
    WLOAD_FIELD(m->lumber,                sizeof(m->lumber));
    WLOAD_FIELD(m->gold,                  sizeof(m->gold));
    WLOAD_FIELD(m->races,                 sizeof(m->races));
    WLOAD_FIELD(&pad,                     sizeof(u8));           // 3-byte pad
    WLOAD_FIELD(&pad,                     sizeof(u8));
    WLOAD_FIELD(&pad,                     sizeof(u8));
    WLOAD_FIELD(m->objectives,            MAX_OBJECTIVES_LENGTH);
    m->objectives[MAX_OBJECTIVES_LENGTH - 1] = '\0';

    // --- Variable-length arrays ---
    WLOAD_FIELD(&m->startEntitiesCount, sizeof(u32));
    if (m->startEntitiesCount > MAX_ENTITIES_COUNT)
    {
        logError("wesave_loadMap: startEntitiesCount %u > MAX_ENTITIES_COUNT in '%s'",
                 m->startEntitiesCount, path);
        fclose(f);
        return false;
    }
    WLOAD_ARRAY(m->startEntities, sizeof(WarLevelUnit), m->startEntitiesCount);

    WLOAD_FIELD(&m->startRoadsCount, sizeof(u32));
    if (m->startRoadsCount > MAX_CONSTRUCTS_COUNT)
    {
        logError("wesave_loadMap: startRoadsCount %u > MAX_CONSTRUCTS_COUNT in '%s'",
                 m->startRoadsCount, path);
        fclose(f);
        return false;
    }
    WLOAD_ARRAY(m->startRoads, sizeof(WarLevelConstruct), m->startRoadsCount);

    WLOAD_FIELD(&m->startWallsCount, sizeof(u32));
    if (m->startWallsCount > MAX_CONSTRUCTS_COUNT)
    {
        logError("wesave_loadMap: startWallsCount %u > MAX_CONSTRUCTS_COUNT in '%s'",
                 m->startWallsCount, path);
        fclose(f);
        return false;
    }
    WLOAD_ARRAY(m->startWalls, sizeof(WarLevelConstruct), m->startWallsCount);

    WLOAD_FIELD(&m->startGoldminesCount, sizeof(u32));
    if (m->startGoldminesCount > MAX_CUSTOM_MAP_GOLDMINES_COUNT)
    {
        logError("wesave_loadMap: startGoldminesCount %u > MAX_CUSTOM_MAP_GOLDMINES_COUNT in '%s'",
                 m->startGoldminesCount, path);
        fclose(f);
        return false;
    }
    WLOAD_ARRAY(m->startGoldmines, sizeof(WarLevelUnit), m->startGoldminesCount);

    WLOAD_FIELD(&m->startConfigurationsCount, sizeof(u32));
    if (m->startConfigurationsCount > MAX_CUSTOM_MAP_CONFIGURATIONS_COUNT)
    {
        logError("wesave_loadMap: startConfigurationsCount %u > MAX_CUSTOM_MAP_CONFIGURATIONS_COUNT"
                 " in '%s'", m->startConfigurationsCount, path);
        fclose(f);
        return false;
    }
    WLOAD_ARRAY(m->startConfigurations,
                sizeof(WarCustomMapConfiguration), m->startConfigurationsCount);

    // --- Tile data ---
    WLOAD_ARRAY(m->visualData,   sizeof(u16), MAP_TILES_WIDTH * MAP_TILES_HEIGHT);
    WLOAD_ARRAY(m->passableData, sizeof(u16), MAP_TILES_WIDTH * MAP_TILES_HEIGHT);

    fclose(f);

    // --- Rebuild terrain sprite from the loaded tilesetType ---
    bool ok = wemap_buildTerrainSprite(ctx);

    logInfo("wesave_loadMap: loaded '%s' (tilesetType=%d, entities=%u)",
            path, (s32)m->tilesetType, m->startEntitiesCount);
    return ok;
}

#undef WLOAD_FIELD
#undef WLOAD_ARRAY
