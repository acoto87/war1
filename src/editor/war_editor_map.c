#include "war_editor_map.h"
#include "war_editor_history.h"

// -----------------------------------------------------------------------
// Tileset resource indices per WarMapTilesetType (0=forest,1=swamp,2=dungeon)
//
// Sourced from war_database.h:
//   DB_ENTRY_TYPE_TILESET index → param1 = tiles resource index
//   DB_ENTRY_TYPE_TILES   index → param1 = primary palette, param2 = secondary palette
//   DB_ENTRY_TYPE_PALETTE indices for each tileset type
//
//   tilesetIndex: forest=189, swamp=192, dungeon=195
//   tilesIndex:   forest=190, swamp=193, dungeon=196
//   paletteIndex: forest=191, swamp=194, dungeon=197
//   secondary palette (shared by all): 217
// -----------------------------------------------------------------------

#define WEMAP_SECONDARY_PALETTE_INDEX  217

typedef struct
{
    s32 tilesetIndex;
    s32 tilesIndex;
    s32 paletteIndex;
} WemapTilesetInfo;

static const WemapTilesetInfo s_tilesetInfos[3] =
{
    { 189, 190, 191 },  // MAP_TILESET_FOREST
    { 192, 193, 194 },  // MAP_TILESET_SWAMP
    { 195, 196, 197 },  // MAP_TILESET_DUNGEON
};

// -----------------------------------------------------------------------
// Editor-local resource helpers
// These mirror wres_* functions but accept WarEditorContext* directly to
// avoid the WarContext/WarEditorContext layout mismatch.
// -----------------------------------------------------------------------

static WarResource* wemap_getOrCreateResource(WarEditorContext* ctx, s32 index)
{
    assert(index >= 0 && index < MAX_RESOURCES_COUNT);
    if (!ctx->resources[index])
        ctx->resources[index] = (WarResource*)wm_alloc(sizeof(WarResource));
    return ctx->resources[index];
}

// Load a PALETTE resource from DATA.WAR raw bytes.
// Mirrors wres_loadPaletteResource, omitting the index-255 UI hack.
static void wemap_loadPaletteRes(WarEditorContext* ctx, s32 index)
{
    if (ctx->resources[index] &&
        ctx->resources[index]->type == WAR_RESOURCE_TYPE_PALETTE)
        return;

    WarRawResource* raw = &ctx->warFile->resources[index];
    if (raw->placeholder)
    {
        logWarning("wemap_loadPaletteRes: placeholder at index %d", index);
        return;
    }

    u32 copyLen = (raw->length < (u32)PALETTE_LENGTH) ? raw->length : (u32)PALETTE_LENGTH;

    WarResource* res = wemap_getOrCreateResource(ctx, index);
    res->type = WAR_RESOURCE_TYPE_PALETTE;
    memset(res->paletteData.colors, 0, PALETTE_LENGTH);

    for (u32 i = 0; i < copyLen; ++i)
        res->paletteData.colors[i] = raw->data[i] * 4;
}

// Load a TILES resource from DATA.WAR.
// param1 = primary palette index, param2 = secondary palette index.
// Mirrors wres_loadTiles.
static void wemap_loadTilesRes(WarEditorContext* ctx, s32 index, s32 param1, s32 param2)
{
    if (ctx->resources[index] &&
        ctx->resources[index]->type == WAR_RESOURCE_TYPE_TILES)
        return;

    WarRawResource* raw = &ctx->warFile->resources[index];
    if (raw->placeholder)
    {
        logWarning("wemap_loadTilesRes: placeholder at index %d", index);
        return;
    }

    WarResource* res      = wemap_getOrCreateResource(ctx, index);
    res->type             = WAR_RESOURCE_TYPE_TILES;
    res->tilesData.palette1 = (u16)param1;
    res->tilesData.palette2 = (u16)param2;
    res->tilesData.data     = (u8*)wm_alloc(raw->length);
    memcpy(res->tilesData.data, raw->data, raw->length);
}

// Build a merged RGBA palette from two loaded palette resources.
// Mirrors wres_getPalette exactly.
static void wemap_getPalette(WarEditorContext* ctx,
                              s32 palette1Index, s32 palette2Index,
                              u8* paletteData)
{
    memset(paletteData, 0, PALETTE_LENGTH);

    if (palette1Index)
    {
        WarResource* r1 = wemap_getOrCreateResource(ctx, palette1Index);
        memcpy(paletteData, r1->paletteData.colors, PALETTE_LENGTH);
    }

    if (palette2Index)
    {
        WarResource* r2   = wemap_getOrCreateResource(ctx, palette2Index);
        u8*          p2   = r2->paletteData.colors;

        for (s32 i = 0; i < 128; ++i)
        {
            if (paletteData[i * 3 + 0] == 252 &&
                paletteData[i * 3 + 1] == 0   &&
                paletteData[i * 3 + 2] == 252)
            {
                paletteData[i * 3 + 0] = p2[i * 3 + 0];
                paletteData[i * 3 + 1] = p2[i * 3 + 1];
                paletteData[i * 3 + 2] = p2[i * 3 + 2];
            }
        }

        for (s32 i = 128; i < 256; ++i)
        {
            if (!(p2[i * 3 + 0] == 252 &&
                  p2[i * 3 + 1] == 0   &&
                  p2[i * 3 + 2] == 252))
            {
                paletteData[i * 3 + 0] = p2[i * 3 + 0];
                paletteData[i * 3 + 1] = p2[i * 3 + 1];
                paletteData[i * 3 + 2] = p2[i * 3 + 2];
            }
        }
    }
}

// Build a TILESET resource (RGBA pixel atlas) from a pre-loaded TILES resource.
// tilesIndex must already be loaded (WAR_RESOURCE_TYPE_TILES) with palette indices set.
// Mirrors wres_loadTileset exactly.
static void wemap_loadTilesetRes(WarEditorContext* ctx, s32 index, s32 tilesIndex)
{
    if (ctx->resources[index] &&
        ctx->resources[index]->type == WAR_RESOURCE_TYPE_TILESET)
        return;

    WarRawResource* raw = &ctx->warFile->resources[index];
    if (raw->placeholder)
    {
        logWarning("wemap_loadTilesetRes: placeholder at index %d", index);
        return;
    }

    WarResource* tiles = wemap_getOrCreateResource(ctx, tilesIndex);

    // Scratch zone for the temporary indexed-colour tile buffer.
    memzone_t* scratch = mz_init(TILESET_WIDTH * TILESET_HEIGHT + 1024);
    u8* data = (u8*)mz_alloc(scratch, TILESET_WIDTH * TILESET_HEIGHT);

    u32 tilesCount = raw->length / 8;
    for (u32 i = 0; i < tilesCount; i++)
    {
        for (s32 my = 0; my < 2; my++)
        {
            for (s32 mx = 0; mx < 2; mx++)
            {
                u16 offset = readu16(raw->data, i * 8 + (u32)(my * 2 + mx) * 2);
                bool flipX = (offset & 0x02) != 0;
                bool flipY = (offset & 0x01) != 0;
                offset = (u16)((offset & 0xFFFC) << 1);

                static const s32 flip[] = { 7, 6, 5, 4, 3, 2, 1, 0, 8 };

                s32 ix = mx + (s32)(i % (u32)TILESET_TILES_PER_ROW) * 2;
                s32 iy = my + (s32)(i / (u32)TILESET_TILES_PER_ROW) * 2;

                for (s32 y = 0; y < 8; ++y)
                {
                    for (s32 x = 0; x < 8; ++x)
                    {
                        s32 fy       = flipY ? flip[y] : y;
                        s32 fx       = flipX ? flip[x] : x;
                        s32 srcIdx   = (s32)offset + fy * 8 + fx;
                        s32 destIdx  = (y + iy * 8) * TILESET_WIDTH + ix * 8 + x;
                        data[destIdx] = tiles->tilesData.data[srcIdx];
                    }
                }
            }
        }
    }

    u8 paletteData[PALETTE_LENGTH];
    wemap_getPalette(ctx, tiles->tilesData.palette1, tiles->tilesData.palette2, paletteData);

    WarResource* res = wemap_getOrCreateResource(ctx, index);
    res->type = WAR_RESOURCE_TYPE_TILESET;
    res->tilesetData.tilesCount = tilesCount;

    for (s32 i = 0; i < TILESET_WIDTH * TILESET_HEIGHT; i++)
    {
        res->tilesetData.data[i * 4 + 0] = paletteData[data[i] * 3 + 0];
        res->tilesetData.data[i * 4 + 1] = paletteData[data[i] * 3 + 1];
        res->tilesetData.data[i * 4 + 2] = paletteData[data[i] * 3 + 2];
        res->tilesetData.data[i * 4 + 3] = data[i] > 0 ? 255 : 0;
    }

    mz_destroy(scratch);
}

// -----------------------------------------------------------------------
// Level info loading helpers (mirror wres_load* for WarEditorContext*)
// -----------------------------------------------------------------------

// Scan the assets[] database table for the entry whose .index == index.
// Returns NULL if not found.
static const DatabaseEntry* wemap_findDbEntry(s32 index)
{
    for (s32 i = 0; i < (s32)arrayLength(assets); ++i)
    {
        if (assets[i].index == index)
            return &assets[i];
    }
    return NULL;
}

// Load a LEVEL_VISUAL resource (4096 u16 tile indices) from DATA.WAR.
// Mirrors wres_loadLevelVisual.
static void wemap_loadLevelVisualRes(WarEditorContext* ctx, s32 index)
{
    if (ctx->resources[index] &&
        ctx->resources[index]->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL)
        return;

    WarRawResource* raw = &ctx->warFile->resources[index];
    if (raw->placeholder)
    {
        logWarning("wemap_loadLevelVisualRes: placeholder at index %d", index);
        return;
    }

    WarResource* res = wemap_getOrCreateResource(ctx, index);
    res->type = WAR_RESOURCE_TYPE_LEVEL_VISUAL;
    for (s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
        res->levelVisual.data[i] = readu16(raw->data, i * 2);
}

// Load a LEVEL_PASSABLE resource (4096 u16 passability flags) from DATA.WAR.
// Mirrors wres_loadLevelPassable.
static void wemap_loadLevelPassableRes(WarEditorContext* ctx, s32 index)
{
    if (ctx->resources[index] &&
        ctx->resources[index]->type == WAR_RESOURCE_TYPE_LEVEL_PASSABLE)
        return;

    WarRawResource* raw = &ctx->warFile->resources[index];
    if (raw->placeholder)
    {
        logWarning("wemap_loadLevelPassableRes: placeholder at index %d", index);
        return;
    }

    WarResource* res = wemap_getOrCreateResource(ctx, index);
    res->type = WAR_RESOURCE_TYPE_LEVEL_PASSABLE;
    for (s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
        res->levelPassable.data[i] = readu16(raw->data, i * 2);
}

// Load a LEVEL_INFO resource from DATA.WAR.
// tilesetType and isCustomMap come from the DatabaseEntry params.
// Mirrors wres_loadLevelInfo but uses wemap_getOrCreateResource.
static void wemap_loadLevelInfoRes(WarEditorContext* ctx, s32 index,
                                   WarMapTilesetType tilesetType, bool isCustomMap)
{
    if (ctx->resources[index] &&
        ctx->resources[index]->type == WAR_RESOURCE_TYPE_LEVEL_INFO)
        return;

    WarRawResource* raw = &ctx->warFile->resources[index];
    if (raw->placeholder)
    {
        logWarning("wemap_loadLevelInfoRes: placeholder at index %d", index);
        return;
    }

    u32 allowId = readu32(raw->data, 0);

    WarResource* resource = wemap_getOrCreateResource(ctx, index);
    resource->type = WAR_RESOURCE_TYPE_LEVEL_INFO;
    resource->levelInfo.allowId           = allowId;
    resource->levelInfo.allowedHumanUnits = 1;
    resource->levelInfo.allowedOrcsUnits  = 1;
    resource->levelInfo.tilesetType       = tilesetType;
    resource->levelInfo.customMap         = isCustomMap;

    memset(resource->levelInfo.allowedFeatures, 0, MAX_FEATURES_COUNT);
    for (s32 f = 0; f < MAX_FEATURES_COUNT; f++)
    {
        if (allowId & (1u << f))
            resource->levelInfo.allowedFeatures[f] = 1;
    }

    for (s32 upgrade = 0; upgrade < MAX_UPGRADES_COUNT; upgrade++)
    {
        bool allowedUpgrade = true;
        if (upgrade >= 3 && upgrade <= 8)
        {
            u8 allowed = resource->levelInfo.allowedFeatures[15 + (upgrade + 3) % 6];
            allowedUpgrade = (allowed > 0);
        }
        if (allowedUpgrade)
        {
            for (s32 player = 0; player < 5; player++)
            {
                resource->levelInfo.allowedUpgrades[upgrade][player] =
                    readu8(raw->data, 4 + upgrade * 5 + player);
            }
        }
    }

    for (s32 i = 0; i < 5; i++)
        resource->levelInfo.lumber[i] = readu32(raw->data, 0x5C + i * 4);

    for (s32 i = 0; i < 5; i++)
        resource->levelInfo.gold[i] = readu32(raw->data, 0x70 + i * 4);

    resource->levelInfo.startX = readu16(raw->data, 0xCC) / 2;
    resource->levelInfo.startY = readu16(raw->data, 0xCE) / 2;

    u32 race = readu32(raw->data, 0x84);
    resource->levelInfo.races[0] = (race >= (1u << 16)) ? WAR_RACE_HUMANS : WAR_RACE_ORCS;
    for (s32 i = 1; i < 4; i++)
    {
        resource->levelInfo.races[i] =
            (resource->levelInfo.races[0] == WAR_RACE_HUMANS) ? WAR_RACE_ORCS : WAR_RACE_HUMANS;
    }

    u32 objectivesOffset = readu16(raw->data, 0x94);
    if (objectivesOffset)
        strcpy(resource->levelInfo.objectives, (char*)(raw->data + objectivesOffset));

    u16 nextLevelIndex = readu16(raw->data, 0xCA);
    if (nextLevelIndex != 0 && nextLevelIndex != 0xFFFF)
        resource->levelInfo.nextLevelIndex = (u16)(nextLevelIndex - 2);

    u16 visualIndex = readu16(raw->data, 0xD0);
    if (visualIndex != 0 && visualIndex != 0xFFFF)
        resource->levelInfo.visualIndex = (u16)(visualIndex - 2);

    u16 passableIndex = readu16(raw->data, 0xD2);
    if (passableIndex != 0 && passableIndex != 0xFFFF)
        resource->levelInfo.passableIndex = (u16)(passableIndex - 2);

    u16 tilesetIndex = readu16(raw->data, 0xD4);
    if (tilesetIndex != 0 && tilesetIndex != 0xFFFF)
        resource->levelInfo.tilesetIndex = (u16)(tilesetIndex - 2);

    u16 tilesIndex = readu16(raw->data, 0xD6);
    if (tilesIndex != 0 && tilesIndex != 0xFFFF)
        resource->levelInfo.tilesIndex = (u16)(tilesIndex - 2);

    u16 paletteIndex = readu16(raw->data, 0xD8);
    if (paletteIndex != 0 && paletteIndex != 0xFFFF)
        resource->levelInfo.paletteIndex = (u16)(paletteIndex - 2);

    // Dynamic section starts at 0xE3; scan past non-FFFFFFFF bytes.
    s32 offset = 0xE3;
    while (readu32(raw->data, offset) != 0xFFFFFFFF)
        offset++;
    offset += 4;  // skip 0xFFFFFFFF marker

    // offset of the units and construction information
    offset = readu16(raw->data, offset);

    if (isCustomMap)
    {
        s32 goldminesOffset = readu16(raw->data, offset);
        wres_loadCustomStartGoldmines(resource, raw, goldminesOffset);
        offset += 2;
        offset += 2;  // skip 0xFFFF marker

        while (offset < goldminesOffset)
        {
            WarCustomMapConfiguration* config =
                &resource->levelInfo.startConfigurations[resource->levelInfo.startConfigurationsCount];
            config->startEntitiesCount = 0;

            s32 offset0 = readu16(raw->data, offset);
            offset0 = wres_loadCustomStartEntities(resource, raw, offset0, config, 0);
            offset0 = wres_loadStartRoads(resource, raw, offset0 + 2);
            offset += 2;

            s32 offset1 = readu16(raw->data, offset);
            offset1 = wres_loadCustomStartEntities(resource, raw, offset1, config, 1);
            offset1 = wres_loadStartRoads(resource, raw, offset1 + 2);
            offset += 2;

            offset += 2;  // skip 0xFFFF marker
            resource->levelInfo.startConfigurationsCount++;
        }
    }
    else
    {
        offset = wres_loadStartEntities(resource, raw, offset);
        offset += 2;  // skip 0xFFFF marker
        offset = wres_loadStartRoads(resource, raw, offset);
        offset += 2;  // skip 0xFFFF marker
        offset = wres_loadStartWalls(resource, raw, offset);
        offset += 2;  // skip 0xFFFF marker
    }
}

// -----------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------

// Base tile index per tileset type (forest=0, swamp=0, dungeon=0).
// All three happen to be 0 (the plain ground tile), but the explicit fill
// documents that tile 0 is the intended ground tile for each tileset.
static const u16 s_baseTiles[3] = { 0u, 0u, 0u };

bool wemap_createEmpty(WarEditorContext* ctx)
{
    WarEditorMap* m = (WarEditorMap*)wm_alloc(sizeof(WarEditorMap));
    if (!m)
    {
        logError("wemap_createEmpty: wm_alloc failed");
        return false;
    }

    memset(m, 0, sizeof(WarEditorMap));

    m->customMap   = true;
    m->tilesetType = (u16)MAP_TILESET_FOREST;

    // Pre-fill resource indices so wemap_buildTerrainSprite can be called
    // immediately without re-reading tilesetType.
    m->tilesetIndex = (u16)s_tilesetInfos[MAP_TILESET_FOREST].tilesetIndex;
    m->tilesIndex   = (u16)s_tilesetInfos[MAP_TILESET_FOREST].tilesIndex;
    m->paletteIndex = (u16)s_tilesetInfos[MAP_TILESET_FOREST].paletteIndex;

    // Explicitly fill visualData with the base tile for this tileset.
    // passableData remains zero (0 = passable).
    u16 baseTile = s_baseTiles[m->tilesetType < 3 ? m->tilesetType : 0];
    for (s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
        m->visualData[i] = baseTile;

    ctx->map = m;
    return true;
}

void wemap_free(WarEditorContext* ctx)
{
    if (!ctx->map)
        return;

    WarEditorMap* m = ctx->map;

    if (m->terrainSprite.texture)
    {
        SDL_DestroyTexture(m->terrainSprite.texture);
        m->terrainSprite.texture = NULL;
    }

    wm_free(m);
    ctx->map = NULL;
}

bool wemap_buildTerrainSprite(WarEditorContext* ctx)
{
    if (!ctx->warFile)
    {
        logWarning("wemap_buildTerrainSprite: warFile not loaded; skipping");
        return false;
    }

    WarEditorMap* m = ctx->map;
    if (!m)
    {
        logError("wemap_buildTerrainSprite: ctx->map is NULL");
        return false;
    }

    s32 tilesetType = (s32)m->tilesetType;
    if (tilesetType < 0 || tilesetType >= 3)
        tilesetType = MAP_TILESET_FOREST;

    s32 tilesetIdx = s_tilesetInfos[tilesetType].tilesetIndex;
    s32 tilesIdx   = s_tilesetInfos[tilesetType].tilesIndex;
    s32 paletteIdx = s_tilesetInfos[tilesetType].paletteIndex;

    // 1. Load palette resources (must come before tileset build).
    wemap_loadPaletteRes(ctx, paletteIdx);
    wemap_loadPaletteRes(ctx, WEMAP_SECONDARY_PALETTE_INDEX);

    // 2. Load tiles resource.
    wemap_loadTilesRes(ctx, tilesIdx, paletteIdx, WEMAP_SECONDARY_PALETTE_INDEX);

    // 3. Build the RGBA tileset pixel atlas.
    wemap_loadTilesetRes(ctx, tilesetIdx, tilesIdx);

    WarResource* tilesetRes = ctx->resources[tilesetIdx];
    if (!tilesetRes || tilesetRes->type != WAR_RESOURCE_TYPE_TILESET)
    {
        logError("wemap_buildTerrainSprite: tileset resource %d unavailable", tilesetIdx);
        return false;
    }

    // 4. Destroy any previous terrain texture.
    if (m->terrainSprite.texture)
    {
        SDL_DestroyTexture(m->terrainSprite.texture);
        m->terrainSprite.texture = NULL;
    }

    // 5. Upload to GPU.
    SDL_Texture* tex = SDL_CreateTexture(
        ctx->renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        TILESET_WIDTH, TILESET_HEIGHT);

    if (!tex)
    {
        logError("wemap_buildTerrainSprite: SDL_CreateTexture failed: %s", SDL_GetError());
        return false;
    }

    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    SDL_UpdateTexture(tex, NULL, tilesetRes->tilesetData.data, TILESET_WIDTH * 4);

    // 6. Fill the WarSprite descriptor (single frame covering the full atlas).
    WarSprite* s    = &m->terrainSprite;
    s->texture      = tex;
    s->frameWidth   = TILESET_WIDTH;
    s->frameHeight  = TILESET_HEIGHT;
    s->framesCount  = 1;

    WarSpriteFrame* f = &s->frames[0];
    f->dx   = 0;
    f->dy   = 0;
    f->w    = (u16)TILESET_WIDTH;
    f->h    = (u16)TILESET_HEIGHT;
    f->off  = 0;
    f->data = NULL;  // no CPU-side pixel copy needed for terrain rendering

    // 7. Cache resource indices on the map.
    m->tilesetIndex = (u16)tilesetIdx;
    m->tilesIndex   = (u16)tilesIdx;
    m->paletteIndex = (u16)paletteIdx;

    logInfo("wemap_buildTerrainSprite: done (tileset=%d, tiles=%d, palette=%d)",
            tilesetIdx, tilesIdx, paletteIdx);
    return true;
}

bool wemap_importFromLevelInfo(WarEditorContext* ctx, s32 levelInfoIndex)
{
    if (!ctx->warFile)
    {
        logWarning("wemap_importFromLevelInfo: warFile not loaded");
        return false;
    }

    if (!ctx->map)
    {
        logError("wemap_importFromLevelInfo: ctx->map is NULL — call wemap_createEmpty first");
        return false;
    }

    // Look up the DatabaseEntry to obtain tilesetType (param1) and isCustomMap (param2).
    const DatabaseEntry* entry = wemap_findDbEntry(levelInfoIndex);
    if (!entry)
    {
        logError("wemap_importFromLevelInfo: no DatabaseEntry for resource index %d", levelInfoIndex);
        return false;
    }

    WarMapTilesetType tilesetType = (WarMapTilesetType)entry->param1;
    bool isCustomMap              = (bool)entry->param2;

    // 1. Load levelInfo resource.
    wemap_loadLevelInfoRes(ctx, levelInfoIndex, tilesetType, isCustomMap);

    WarResource* res = ctx->resources[levelInfoIndex];
    if (!res || res->type != WAR_RESOURCE_TYPE_LEVEL_INFO)
    {
        logError("wemap_importFromLevelInfo: failed to load levelInfo at index %d", levelInfoIndex);
        return false;
    }

    // 2. Copy all levelInfo fields into ctx->map.
    WarEditorMap* m = ctx->map;

    m->allowId           = res->levelInfo.allowId;
    m->allowedHumanUnits = res->levelInfo.allowedHumanUnits;
    m->allowedOrcsUnits  = res->levelInfo.allowedOrcsUnits;
    m->customMap         = res->levelInfo.customMap;
    m->tilesetType       = (u16)res->levelInfo.tilesetType;
    m->startX            = res->levelInfo.startX;
    m->startY            = res->levelInfo.startY;

    memcpy(m->allowedFeatures, res->levelInfo.allowedFeatures, sizeof(m->allowedFeatures));
    memcpy(m->allowedUpgrades, res->levelInfo.allowedUpgrades, sizeof(m->allowedUpgrades));
    memcpy(m->lumber,          res->levelInfo.lumber,          sizeof(m->lumber));
    memcpy(m->gold,            res->levelInfo.gold,            sizeof(m->gold));
    memcpy(m->races,           res->levelInfo.races,           sizeof(m->races));
    strncpy(m->objectives, res->levelInfo.objectives, MAX_OBJECTIVES_LENGTH - 1);
    m->objectives[MAX_OBJECTIVES_LENGTH - 1] = '\0';

    m->startEntitiesCount = res->levelInfo.startEntitiesCount;
    memcpy(m->startEntities, res->levelInfo.startEntities,
           sizeof(WarLevelUnit) * m->startEntitiesCount);

    m->startRoadsCount = res->levelInfo.startRoadsCount;
    memcpy(m->startRoads, res->levelInfo.startRoads,
           sizeof(WarLevelConstruct) * m->startRoadsCount);

    m->startWallsCount = res->levelInfo.startWallsCount;
    memcpy(m->startWalls, res->levelInfo.startWalls,
           sizeof(WarLevelConstruct) * m->startWallsCount);

    m->startGoldminesCount = res->levelInfo.startGoldminesCount;
    memcpy(m->startGoldmines, res->levelInfo.startGoldmines,
           sizeof(WarLevelUnit) * m->startGoldminesCount);

    m->startConfigurationsCount = res->levelInfo.startConfigurationsCount;
    memcpy(m->startConfigurations, res->levelInfo.startConfigurations,
           sizeof(WarCustomMapConfiguration) * m->startConfigurationsCount);

    // 3. Load visual and passable tile arrays.
    u16 visualIdx   = res->levelInfo.visualIndex;
    u16 passableIdx = res->levelInfo.passableIndex;

    if (visualIdx == 0 || visualIdx >= MAX_RESOURCES_COUNT)
    {
        logWarning("wemap_importFromLevelInfo: invalid visualIndex %d", (s32)visualIdx);
    }
    else
    {
        wemap_loadLevelVisualRes(ctx, (s32)visualIdx);
        WarResource* vis = ctx->resources[(s32)visualIdx];
        if (vis && vis->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL)
            memcpy(m->visualData, vis->levelVisual.data,
                   sizeof(u16) * MAP_TILES_WIDTH * MAP_TILES_HEIGHT);
    }

    if (passableIdx == 0 || passableIdx >= MAX_RESOURCES_COUNT)
    {
        logWarning("wemap_importFromLevelInfo: invalid passableIndex %d", (s32)passableIdx);
    }
    else
    {
        wemap_loadLevelPassableRes(ctx, (s32)passableIdx);
        WarResource* pass = ctx->resources[(s32)passableIdx];
        if (pass && pass->type == WAR_RESOURCE_TYPE_LEVEL_PASSABLE)
            memcpy(m->passableData, pass->levelPassable.data,
                   sizeof(u16) * MAP_TILES_WIDTH * MAP_TILES_HEIGHT);
    }

    // 4. Cache resource indices (used for future reference; buildTerrainSprite
    //    re-derives them from tilesetType via the lookup table).
    m->tilesetIndex = res->levelInfo.tilesetIndex;
    m->tilesIndex   = res->levelInfo.tilesIndex;
    m->paletteIndex = res->levelInfo.paletteIndex;

    // 5. Rebuild the GPU terrain texture.
    bool ok = wemap_buildTerrainSprite(ctx);

    logInfo("wemap_importFromLevelInfo: imported level %d (%s), tilesetType=%d, entities=%d",
            levelInfoIndex, entry->name, (s32)tilesetType, m->startEntitiesCount);
    return ok;
}

void wemap_newMap(WarEditorContext* ctx)
{
    // Free the existing map (destroys terrain texture and frees memory).
    wemap_free(ctx);

    // Allocate a fresh blank map; log but continue on failure — the map
    // pointer will be NULL and downstream code handles that gracefully.
    if (!wemap_createEmpty(ctx))
    {
        logError("wemap_newMap: wemap_createEmpty failed");
    }

    // Rebuild terrain sprite (requires warFile; logs a warning internally
    // and returns false if it is not available, which is acceptable here).
    wemap_buildTerrainSprite(ctx);

    // Reset editor state to "unsaved new map" defaults.
    ctx->currentFilePath[0] = '\0';
    ctx->mapName[0]         = '\0';
    ctx->unsavedChanges     = false;
    ctx->cameraOffset.x     = 0.0f;
    ctx->cameraOffset.y     = 0.0f;
    ctx->cameraZoom         = 1.0f;

    WarEntityIdListClear(&ctx->selectedEntities);

    if (ctx->history)
        wehist_clear(ctx->history);

    SDL_strlcpy(ctx->statusText, "New map created.", sizeof(ctx->statusText));
    logInfo("wemap_newMap: new blank map created");
}
