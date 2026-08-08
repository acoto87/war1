#include "war_resources.h"

#include <assert.h>

#ifdef SHL_MZ_DEBUG
#include "shl/memzone_audit.h"
#else
#include "shl/memzone.h"
#endif
#include "shl/memory_buffer.h"
#include "shl/wstr.h"
#include "TracyC.h"

#include "war_log.h"
#include "war_audio.h"

#define readu8(arr, index) (arr[index])
#define reads16(arr, index) (*(s16*)((arr) + (index)))
#define readu16(arr, index) (*(u16*)((arr) + (index)))
#define reads32(arr, index) (*(s32*)((arr) + (index)))
#define readu32(arr, index) (*(u32*)((arr) + (index)))

static void buildRgbaLut(const u8* paletteData, u32* rgba)
{
    rgba[0] = 0u;
    for (s32 c = 1; c < 256; ++c)
    {
        u8 r = paletteData[c * 3 + 0];
        u8 g = paletteData[c * 3 + 1];
        u8 b = paletteData[c * 3 + 2];
        rgba[c] = (u32)r | ((u32)g << 8) | ((u32)b << 16) | 0xFF000000u;
    }
}

WarResource* wres_getOrCreateResource(WarContext* context, s32 index)
{
    assert(index >= 0 && index < MAX_RESOURCES_COUNT);

    WarResource* resource = &context->resources[index];

    if (resource->type == WAR_RESOURCE_TYPE_UNKNOWN)
    {
        logDebug("Creating resource: %d", index);
    }

    return resource;
}

void wres_getPalette(WarContext* context, s32 palette1Index, s32 palette2Index, u8 *paletteData)
{
    memset(paletteData, 0, PALETTE_LENGTH);

    if (palette1Index)
    {
        WarResource* palette1 = wres_getOrCreateResource(context, palette1Index);
        u8 *palette1Data = palette1->paletteData.colors;
        memcpy(paletteData, palette1Data, PALETTE_LENGTH);
    }

    // probably only the palettes 191, 194, 197 need this to use a second palette for the background
    // for now leave it whenever there is specified a second palette on the entry
    if (palette2Index)
    {
        WarResource* palette2 = wres_getOrCreateResource(context, palette2Index);
        u8 *palette2Data = palette2->paletteData.colors;

        for (s32 i = 0; i < 128; ++i)
        {
            if (paletteData[i * 3 + 0] == 252 &&
                paletteData[i * 3 + 1] == 0 &&
                paletteData[i * 3 + 2] == 252)
            {
                paletteData[i * 3 + 0] = palette2Data[i * 3 + 0];
                paletteData[i * 3 + 1] = palette2Data[i * 3 + 1];
                paletteData[i * 3 + 2] = palette2Data[i * 3 + 2];
            }
        }

        for (s32 i = 128; i < 256; ++i)
        {
            if (!(palette2Data[i * 3 + 0] == 252 &&
                  palette2Data[i * 3 + 1] == 0 &&
                  palette2Data[i * 3 + 2] == 252))
            {
                paletteData[i * 3 + 0] = palette2Data[i * 3 + 0];
                paletteData[i * 3 + 1] = palette2Data[i * 3 + 1];
                paletteData[i * 3 + 2] = palette2Data[i * 3 + 2];
            }
        }
    }
}

void wres_loadPaletteResource(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadPaletteResource", true);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    if (rawResource.length < PALETTE_LENGTH)
    {
        rawResource.data = (u8*)wm_realloc(rawResource.data, PALETTE_LENGTH);
        memset(rawResource.data + rawResource.length, 0, PALETTE_LENGTH - rawResource.length);
    }

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_PALETTE;

    for (s32 i = 0; i < PALETTE_LENGTH; ++i)
    {
        resource->paletteData.colors[i] = rawResource.data[i] * 4;
    }

    // this is a hack in palette #255 because the UI graphics of the unit info
    // is wrong when using the palette as it came from the file,
    // or at least I didn't knew how to decode it.
    if (index == 255)
    {
        resource->paletteData.colors[0] = 0;
        resource->paletteData.colors[1] = 0;
        resource->paletteData.colors[2] = 0;

        resource->paletteData.colors[190 * 3 + 0] = 176;
        resource->paletteData.colors[190 * 3 + 1] = 184;
        resource->paletteData.colors[190 * 3 + 2] = 208;

        resource->paletteData.colors[191 * 3 + 0] = 208;
        resource->paletteData.colors[191 * 3 + 1] = 208;
        resource->paletteData.colors[191 * 3 + 2] = 236;

        resource->paletteData.colors[214 * 3 + 0] = 204;
        resource->paletteData.colors[214 * 3 + 1] = 212;
        resource->paletteData.colors[214 * 3 + 2] = 180;

        resource->paletteData.colors[208 * 3 + 0] = 32;
        resource->paletteData.colors[208 * 3 + 1] = 32;
        resource->paletteData.colors[208 * 3 + 2] = 44;

        resource->paletteData.colors[211 * 3 + 0] = 80;
        resource->paletteData.colors[211 * 3 + 1] = 104;
        resource->paletteData.colors[211 * 3 + 2] = 104;

        resource->paletteData.colors[184 * 3 + 0] = 12;
        resource->paletteData.colors[184 * 3 + 1] = 20;
        resource->paletteData.colors[184 * 3 + 2] = 12;

        resource->paletteData.colors[185 * 3 + 0] = 28;
        resource->paletteData.colors[185 * 3 + 1] = 32;
        resource->paletteData.colors[185 * 3 + 2] = 32;

        resource->paletteData.colors[186 * 3 + 0] = 56;
        resource->paletteData.colors[186 * 3 + 1] = 64;
        resource->paletteData.colors[186 * 3 + 2] = 68;

        resource->paletteData.colors[189 * 3 + 0] = 144;
        resource->paletteData.colors[189 * 3 + 1] = 156;
        resource->paletteData.colors[189 * 3 + 2] = 176;
    }
    // this palette has an ugly pink color (color #150)
    // that is showing in the unit portraits, so change it to dark gray
    else if (index == 217)
    {
        resource->paletteData.colors[96 * 3 + 0] = 108;
        resource->paletteData.colors[96 * 3 + 1] = 72;
        resource->paletteData.colors[96 * 3 + 2] = 40;

        resource->paletteData.colors[150 * 3 + 0] = 46;
        resource->paletteData.colors[150 * 3 + 1] = 54;
        resource->paletteData.colors[150 * 3 + 2] = 54;

        resource->paletteData.colors[158 * 3 + 0] = 40;
        resource->paletteData.colors[158 * 3 + 1] = 64;
        resource->paletteData.colors[158 * 3 + 2] = 112;

        resource->paletteData.colors[215 * 3 + 0] = 32;
        resource->paletteData.colors[215 * 3 + 1] = 20;
        resource->paletteData.colors[215 * 3 + 2] = 24;
    }

    TracyCZoneEnd(ctx);
}

void wres_loadImageResource(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadImageResource", true);

    u8 paletteData[PALETTE_LENGTH];
    wres_getPalette(context, entry->param1, entry->param2, paletteData);

    u32 rgba[PALETTE_LENGTH/3];
    buildRgbaLut(paletteData, rgba);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    u16 width = readu16(rawResource.data, 0);
    u16 height = readu16(rawResource.data, 2);

    u8 *pixels = (u8*)wm_alloc(width * height * 4 * sizeof(u8));
    for (s32 i = 0; i < width * height; ++i)
    {
        u32 colorIndex = readu8(rawResource.data, 4 + i);
        memcpy(&pixels[i*4], &rgba[colorIndex], sizeof(u32));
    }

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_IMAGE;
    resource->imageData.width = width;
    resource->imageData.height = height;
    resource->imageData.pixels = pixels;

    TracyCZoneEnd(ctx);
}

void wres_loadSpriteResource(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadSpriteResource", true);

    u8 paletteData[PALETTE_LENGTH];
    wres_getPalette(context, entry->param1, entry->param2, paletteData);

    u32 rgba[PALETTE_LENGTH/3];
    buildRgbaLut(paletteData, rgba);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    u16 framesCount = readu16(rawResource.data, 0);
    u8 frameWidth = readu8(rawResource.data, 2);
    u8 frameHeight = readu8(rawResource.data, 3);

    WarSpriteFrame* frames = (WarSpriteFrame*)wm_alloc(framesCount * sizeof(WarSpriteFrame));
    assert(frames);

    u32 framePixelSize = (u32)frameWidth * (u32)frameHeight * 4u;
    u8* pixelPool = (u8*)wm_alloc(framesCount * framePixelSize);
    assert(pixelPool);

    for (s32 i = 0; i < framesCount; ++i)
    {
        WarSpriteFrame* frame = &frames[i];
        frame->dx = readu8(rawResource.data, 4 + i * 8 + 0);
        frame->dy = readu8(rawResource.data, 4 + i * 8 + 1);
        frame->w = readu8(rawResource.data, 4 + i * 8 + 2);
        frame->h = readu8(rawResource.data, 4 + i * 8 + 3);
        frame->off = readu32(rawResource.data, 4 + i * 8 + 4);
        frame->data = pixelPool + (u32)i * framePixelSize;

        // found in war1tool.c, don't know if is needed
        // if (off < 0) {  // High bit of width
        //     off &= 0x7FFFFFFF;
        //     w += 256;
        // }
    }

    for (s32 i = 0; i < framesCount; ++i)
    {
        WarSpriteFrame* frame = &frames[i];
        u32 off = frame->off;
        for (s32 y = frame->dy; y < (frame->dy + frame->h); ++y)
        {
            for (s32 x = frame->dx; x < (frame->dx + frame->w); ++x)
            {
                u32 pixel = (x + y * frameWidth) * 4;
                u32 colorIndex = rawResource.data[off++];
                memcpy(&frame->data[pixel], &rgba[colorIndex], sizeof(u32));
            }
        }
    }

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_SPRITE;
    resource->spriteData.frames = frames;
    resource->spriteData.framesCount = framesCount;
    resource->spriteData.frameWidth = frameWidth;
    resource->spriteData.frameHeight = frameHeight;

    TracyCZoneEnd(ctx);
}

s32 wres_loadStartEntities(WarResource* resource, WarRawResource* rawResource, s32 offset)
{
    resource->levelInfo.startEntitiesCount = 0;
    resource->levelInfo.startEntities = (WarLevelUnit*)wm_alloc(MAX_ENTITIES_COUNT * sizeof(WarLevelUnit));
    assert(resource->levelInfo.startEntities);

    while (offset < (s32)rawResource->length)
    {
        u16 val = readu16(rawResource->data, offset);
        if (val == 0xFFFF)
        {
            break;
        }

        WarLevelUnit* startEntity = &resource->levelInfo.startEntities[resource->levelInfo.startEntitiesCount];
        startEntity->x = readu8(rawResource->data, offset + 0) / 2;
        startEntity->y = readu8(rawResource->data, offset + 1) / 2;
        startEntity->type = readu8(rawResource->data, offset + 2);
        startEntity->player = readu8(rawResource->data, offset + 3);

        offset += 4;

        if (startEntity->type == WAR_UNIT_GOLDMINE)
        {
            startEntity->resourceKind = WAR_RESOURCE_GOLD;
            startEntity->amount = readu16(rawResource->data, offset);
            offset += 2;
        }

        resource->levelInfo.startEntitiesCount++;
    }

    return offset;
}

s32 wres_loadStartRoads(WarResource* resource, WarRawResource* rawResource, s32 offset)
{
    resource->levelInfo.startRoadsCount = 0;
    resource->levelInfo.startRoads = (WarLevelConstruct*)wm_alloc(MAX_CONSTRUCTS_COUNT * sizeof(WarLevelConstruct));
    assert(resource->levelInfo.startRoads);

    while (offset < (s32)rawResource->length)
    {
        u16 val = readu16(rawResource->data, offset);
        if (val == 0xFFFF)
        {
            break;
        }

        WarLevelConstruct* construct = &resource->levelInfo.startRoads[resource->levelInfo.startRoadsCount];
        construct->type = WAR_CONSTRUCT_ROAD;
        construct->x1 = readu8(rawResource->data, offset + 0) / 2;
        construct->y1 = readu8(rawResource->data, offset + 1) / 2;
        construct->x2 = readu8(rawResource->data, offset + 2) / 2;
        construct->y2 = readu8(rawResource->data, offset + 3) / 2;
        construct->player = readu8(rawResource->data, offset + 4);

        offset += 5;

        resource->levelInfo.startRoadsCount++;
    }

    return offset;
}

s32 wres_loadStartWalls(WarResource* resource, WarRawResource* rawResource, s32 offset)
{
    resource->levelInfo.startWallsCount = 0;
    resource->levelInfo.startWalls = (WarLevelConstruct*)wm_alloc(MAX_CONSTRUCTS_COUNT * sizeof(WarLevelConstruct));
    assert(resource->levelInfo.startWalls);

    while (offset < (s32)rawResource->length)
    {
        u16 val = readu16(rawResource->data, offset);
        if (val == 0xFFFF)
        {
            break;
        }

        WarLevelConstruct* construct = &resource->levelInfo.startWalls[resource->levelInfo.startWallsCount];
        construct->type = WAR_CONSTRUCT_WALL;
        construct->x1 = readu8(rawResource->data, offset + 0) / 2;
        construct->y1 = readu8(rawResource->data, offset + 1) / 2;
        construct->x2 = readu8(rawResource->data, offset + 2) / 2;
        construct->y2 = readu8(rawResource->data, offset + 3) / 2;
        construct->player = readu8(rawResource->data, offset + 4);

        offset += 5;

        resource->levelInfo.startWallsCount++;
    }

    return offset;
}

s32 wres_loadCustomStartGoldmines(WarResource* resource, WarRawResource* rawResource, s32 offset)
{
    resource->levelInfo.startGoldminesCount = 0;
    resource->levelInfo.startGoldmines = (WarLevelUnit*)wm_alloc(MAX_CUSTOM_MAP_GOLDMINES_COUNT * sizeof(WarLevelUnit));
    assert(resource->levelInfo.startGoldmines);

    while (offset < (s32)rawResource->length)
    {
        u16 val = readu16(rawResource->data, offset);
        if (val == 0xFFFF)
        {
            break;
        }

        WarLevelUnit* startGoldmine = &resource->levelInfo.startGoldmines[resource->levelInfo.startGoldminesCount];
        startGoldmine->x = readu8(rawResource->data, offset + 0) / 2;
        startGoldmine->y = readu8(rawResource->data, offset + 1) / 2;
        startGoldmine->type = WAR_UNIT_GOLDMINE;
        startGoldmine->player = 4;

        offset += 4;

        resource->levelInfo.startGoldminesCount++;
    }

    return offset;
}

s32 wres_loadCustomStartEntities(WarResource* resource, WarRawResource* rawResource, s32 offset, WarCustomMapConfiguration* configuration, u8 player)
{
    NOT_USED(resource);

    configuration->startEntities = (WarLevelUnit*)wm_alloc(MAX_CUSTOM_MAP_ENTITIES_COUNT * sizeof(WarLevelUnit));
    assert(configuration->startEntities);

    while (offset < (s32)rawResource->length)
    {
        u16 val = readu16(rawResource->data, offset);
        if (val == 0xFFFF)
        {
            break;
        }

        WarLevelUnit* startEntity = &configuration->startEntities[configuration->startEntitiesCount];
        startEntity->x = readu8(rawResource->data, offset + 0) / 2;
        startEntity->y = readu8(rawResource->data, offset + 1) / 2;
        startEntity->type = readu8(rawResource->data, offset + 2);
        startEntity->player = player;

        offset += 4;

        configuration->startEntitiesCount++;
    }

    return offset;
}

void wres_loadLevelInfo(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadLevelInfo", true);

    s32 index = entry->index;
    WarMapTilesetType tilesetType = (WarMapTilesetType)entry->param1;
    bool isCustomMap = entry->param2;

    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    u32 allowId = readu32(rawResource.data, 0);

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_LEVEL_INFO;
    resource->levelInfo.allowId = allowId;
    resource->levelInfo.allowedHumanUnits = 1;
    resource->levelInfo.allowedOrcsUnits = 1;
    resource->levelInfo.tilesetType = tilesetType;
    resource->levelInfo.customMap = isCustomMap;

    memset(resource->levelInfo.allowedFeatures, 0, MAX_FEATURES_COUNT);
    for(s32 f = 0; f < MAX_FEATURES_COUNT; f++)
    {
        // the feature is allowed if the corresponding bit is set
        if (allowId & (1 << f))
        {
            resource->levelInfo.allowedFeatures[f] = 1;
        }
    }

    // 0x0004 - 0x0008: 5xByte: Upgrade: Ranged Weapons, arrows / spears.
    // 0x0009 - 0x000D: 5xByte: Upgrade: Melee Weapons, swords / axes.
    // 0x000E - 0x0012: 5xByte: Upgrade: Rider speed, horses / wolves.
    // 0x0013 - 0x0017: 5xByte: Spell: summon scorpions / summon spiders.
    // 0x0018 - 0x001C: 5xByte: Spell: rain of fire / cloud of poison.
    // 0x001D - 0x0021: 5xByte: Spell: summon water elemental / summon daemon.
    // 0x0022 - 0x0026: 5xByte: Spell: healing / raise dead.
    // 0x0027 - 0x002B: 5xByte: Spell: far seeing / dark vision.
    // 0x002C - 0x0030: 5xByte: Spell: invisibility / unholy armor.
    // 0x0031 - 0x0035: 5xByte: Upgrade: Shields.
    for(s32 upgrade = 0; upgrade < MAX_UPGRADES_COUNT; upgrade++)
    {
        bool allowedUpgrade = true;
        if (upgrade >= 3 && upgrade <= 8)
        {
            // if it's a spell upgrade check the allowed features, because spells may not be allowed.
            // offset of spells in allowed features is 15
            // usefully, they are not in the same order in the allowid as they
            // are in the list of researched stuff, that's the (upgrade + 3) % 6 are for.
            u8 allowed = resource->levelInfo.allowedFeatures[15 + (upgrade + 3) % 6];
            allowedUpgrade = (allowed > 0);
        }

        if (allowedUpgrade)
        {
            for(s32 player = 0; player < 5; player++)
            {
                u8 value = readu8(rawResource.data, 4 + upgrade * 5 + player);
                resource->levelInfo.allowedUpgrades[upgrade][player] = value;
            }
        }
    }

    // 0x005C - 0x0069: 5xDWord: Lumber for each player.
    for(s32 i = 0; i < 5; i++)
    {
        resource->levelInfo.lumber[i] = readu32(rawResource.data, 0x5C + i * 4);
    }

	// 0x0070 - 0x0083: 5xDWord: Gold for each player.
    for(s32 i = 0; i < 5; i++)
    {
        resource->levelInfo.gold[i] = readu32(rawResource.data, 0x70 + i * 4);
    }

    // 0xCC, 0xCE => Starting position of unit (divide by 2) (ushort)
    resource->levelInfo.startX = readu16(rawResource.data, 0xCC) / 2;
    resource->levelInfo.startY = readu16(rawResource.data, 0xCE) / 2;

    // 0x84 - 0x88: players info
    u32 race = readu32(rawResource.data, 0x84);
    resource->levelInfo.races[0] = (race >= (1 << 16)) ? WAR_RACE_HUMANS : WAR_RACE_ORCS;

    for(s32 i = 1; i < 4; i++)
    {
        resource->levelInfo.races[i] = (resource->levelInfo.races[0] == WAR_RACE_HUMANS) ? WAR_RACE_ORCS : WAR_RACE_HUMANS;
    }

    // 0x94: objectives
    u32 objectivesOffset = readu16(rawResource.data, 0x94);
    if (objectivesOffset)
    {
        strcpy(resource->levelInfo.objectives, (char*)(rawResource.data + objectivesOffset));
    }

    // next level resource index: *0xCA - 2
    u16 nextLevelIndex = readu16(rawResource.data, 0xCA);
    if (nextLevelIndex != 0 && nextLevelIndex != 0xFFFF)
    {
        resource->levelInfo.nextLevelIndex = nextLevelIndex - 2;
    }

    // visual resource index: *0xD0 - 2
    u16 visualIndex = readu16(rawResource.data, 0xD0);
    if (visualIndex != 0 && visualIndex != 0xFFFF)
    {
        resource->levelInfo.visualIndex = visualIndex - 2;
    }

    // passable resource index: *0xD2 - 2
    u16 passableIndex = readu16(rawResource.data, 0xD2);
    if (passableIndex != 0 && passableIndex != 0xFFFF)
    {
        resource->levelInfo.passableIndex = passableIndex - 2;
    }

    // tileset resource index: *0xD4 - 2
    u16 tilesetIndex = readu16(rawResource.data, 0xD4);
    if (tilesetIndex != 0 && tilesetIndex != 0xFFFF)
    {
        resource->levelInfo.tilesetIndex = tilesetIndex - 2;
    }

    // tiles resource index: *0xD6 - 2
    u16 tilesIndex = readu16(rawResource.data, 0xD6);
    if (tilesIndex != 0 && tilesIndex != 0xFFFF)
    {
        resource->levelInfo.tilesIndex = tilesIndex - 2;
    }

    // palette resource index: *0xD8 - 2
    u16 paletteIndex = readu16(rawResource.data, 0xD8);
    if (paletteIndex != 0 && paletteIndex != 0xFFFF)
    {
        resource->levelInfo.paletteIndex = paletteIndex - 2;
    }

    // dynamic data: 0xE3
    s32 offset = 0xE3;
    while (readu32(rawResource.data, offset) != 0xFFFFFFFF)
    {
        offset++;
    }

    // skip marker 0xFFFF
    offset += 4;

    // offset of the units and construction information
    offset = readu16(rawResource.data, offset);

    if (isCustomMap)
    {
        // It seems that the structure of custom map is different of the structure for the levels.
        // It has the the placeholders for the possible configurations of starting positions.
        // Warcraft 1 only supports one vs one custom maps, and every custom map I got has four
        // starting positions, so there are 6 possible configurations.
        //
        // offset of gold mines
        // FF FF
        // offset of player 1 in first configuration
        // offset of player 2 in first configuration
        // FF FF
        // offset of player 1 in second configuration
        // offset of player 2 in second configuration
        // FF FF
        // ... the above repeats 4 more times for a total of 6 configurations
        //
        // the goldmines data is in the form of a collection of entities without
        // the gold amount data that ends with a 0xFFFF marker.
        // the players configuration data is just the collection of starting entities
        // followed by a marker 0xFFFF and then the collection of roads for that player.
        // The starting entities are usually (at least in the DATA file I have) just
        // a placeholder town hall and a placeholder farm that, I'm guessing at the
        // start of the map it gets replaced by the actual race townhall and farm
        //
        s32 goldminesOffset = readu16(rawResource.data, offset);
        wres_loadCustomStartGoldmines(resource, &rawResource, goldminesOffset);
        offset += 2;

        // skip marker 0xFFFF
        offset += 2;

        while (offset < goldminesOffset)
        {
            WarCustomMapConfiguration* configuration = &resource->levelInfo.startConfigurations[resource->levelInfo.startConfigurationsCount];
            configuration->startEntitiesCount = 0;

            s32 offset0 = readu16(rawResource.data, offset);
            offset0 = wres_loadCustomStartEntities(resource, &rawResource, offset0, configuration, 0);
            offset0 = wres_loadStartRoads(resource, &rawResource, offset0 + 2);
            offset += 2;

            s32 offset1 = readu16(rawResource.data, offset);
            offset1 = wres_loadCustomStartEntities(resource, &rawResource, offset1, configuration, 1);
            offset1 = wres_loadStartRoads(resource, &rawResource, offset1 + 2);
            offset += 2;

            // skip marker 0xFFFF
            offset += 2;

            resource->levelInfo.startConfigurationsCount++;
        }
    }
    else
    {
        // starting units
        offset = wres_loadStartEntities(resource, &rawResource, offset);

        // skip marker 0xFFFF
        offset += 2;

        // roads
        offset = wres_loadStartRoads(resource, &rawResource, offset);

        // skip marker 0xFFFF
        offset += 2;

        // walls
        offset = wres_loadStartWalls(resource, &rawResource, offset);

        // skip marker 0xFFFF
        offset += 2;
    }

    TracyCZoneEnd(ctx);
}

void wres_loadLevelVisual(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadLevelVisual", true);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_LEVEL_VISUAL;

    resource->levelVisual.data = (u16*)wm_alloc(MAP_TILES_WIDTH * MAP_TILES_HEIGHT * sizeof(u16));
    assert(resource->levelVisual.data);

    for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        resource->levelVisual.data[i] = readu16(rawResource.data, i * 2);
    }

    TracyCZoneEnd(ctx);
}

void wres_loadLevelPassable(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadLevelPassable", true);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_LEVEL_PASSABLE;

    resource->levelPassable.data = (u16*)wm_alloc(MAP_TILES_WIDTH * MAP_TILES_HEIGHT * sizeof(u16));
    assert(resource->levelPassable.data);

    for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        // 128 -> wood, 64 -> water, 16 -> bridge, 0 -> empty
        resource->levelPassable.data[i] = readu16(rawResource.data, i * 2);
    }

    TracyCZoneEnd(ctx);
}

void wres_loadTileset(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadTileset", true);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    WarResource *tiles = wres_getOrCreateResource(context, entry->param1);
    assert(tiles);
    assert(tiles->type == WAR_RESOURCE_TYPE_TILES);

    // Local scratch zone for the temporary indexed-colour tile buffer.
    u8 *data = (u8*)wm_alloc(TILESET_WIDTH * TILESET_HEIGHT);
    assert(data);

    u32 tilesCount = rawResource.length / 8;
    for(u32 i = 0; i < tilesCount; i++)
    {
        for(s32 my = 0; my < 2; my++)
        {
            for(s32 mx = 0; mx < 2; mx++)
            {
                u16 offset = readu16(rawResource.data, i * 8 + (my * 2 + mx) * 2);
                bool flipX = (offset & 0x02);
                bool flipY = (offset & 0x01);
                offset = (u16)((offset & 0xFFFC) << 1);

                static const s32 flip[] = {
                    7, 6, 5, 4, 3, 2, 1, 0, 8
                };

                s32 ix = mx + (i % TILESET_TILES_PER_ROW) * 2;
                s32 iy = my + (i / TILESET_TILES_PER_ROW) * 2;

                for (s32 y = 0; y < 8; ++y)
                {
                    for (s32 x = 0; x < 8; ++x)
                    {
                        s32 fy = (flipY ? flip[y] : y);
                        s32 fx = (flipX ? flip[x] : x);
                        s32 srcValueIndex = offset + fy * 8 + fx;
                        s32 destValueIndex = (y + iy * 8) * TILESET_WIDTH + ix * 8 + x;
                        data[destValueIndex] = tiles->tilesData.data[srcValueIndex];
                    }
                }
            }
        }
    }

    u8 paletteData[PALETTE_LENGTH];
    wres_getPalette(context, tiles->tilesData.palette1, tiles->tilesData.palette2, paletteData);

    u32 rgba[PALETTE_LENGTH/3];
    buildRgbaLut(paletteData, rgba);

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_TILESET;
    resource->tilesetData.tilesCount = rawResource.length / 8;
    resource->tilesetData.data = (u8*)wm_alloc(TILESET_WIDTH * TILESET_HEIGHT * 4 * sizeof(u8));
    assert(resource->tilesetData.data);

    for(s32 i = 0; i < TILESET_WIDTH * TILESET_HEIGHT; i++)
    {
        u8 colorIndex = data[i];
        memcpy(&resource->tilesetData.data[i * 4], &rgba[colorIndex], sizeof(u32));
    }

    // #if __DEBUG__
    // {
    //     char path[32];
    //     StringView fp = wsv_fromCStringFormat(path, sizeof(path), "tileset_%d.png", index);
    //     stbi_write_png(wsv_data(fp), TILESET_WIDTH, TILESET_HEIGHT, 4, resource->tilesetData.data, TILESET_WIDTH * 4);
    // }
    // #endif

    wm_free(data);

    TracyCZoneEnd(ctx);
}

void wres_loadTiles(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadTiles", true);

    u8 paletteData[PALETTE_LENGTH];
    wres_getPalette(context, entry->param1, entry->param2, paletteData);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_TILES;
    resource->tilesData.palette1 = entry->param1;
    resource->tilesData.palette2 = entry->param2;
    resource->tilesData.data = (u8*)wm_alloc(rawResource.length * sizeof(u8));
    memcpy(resource->tilesData.data, rawResource.data, rawResource.length);

    TracyCZoneEnd(ctx);
}

void wres_loadText(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadText", true);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_TEXT;
    resource->textData.length = rawResource.length;
    resource->textData.text = (char *)wm_alloc(resource->textData.length * sizeof(char));
    memcpy(resource->textData.text, rawResource.data, resource->textData.length);

    TracyCZoneEnd(ctx);
}

void wres_loadXmi(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadXmi", true);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    u8* xmiData = rawResource.data;
    size32 xmiLength = rawResource.length;

    size32 midLength;
    u8* midData = x2m_transcode(xmiData, xmiLength, &midLength);
    if (!midData)
    {
        logError("Can't convert XMI file of resource %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    WarResource* resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_XMID;
    resource->audio.data = midData;
    assert(midLength <= INT32_MAX);
    resource->audio.length = (s32)midLength;

    TracyCZoneEnd(ctx);
}

void wres_loadWave(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadWave", true);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    mw_audio_buffer buffer = {0};
    if (!mw_read_memory((const void*)rawResource.data, rawResource.length, &buffer))
    {
        logError("Failed to read WAV data for resource %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    mw_audio_buffer resampledBuffer = {0};
    if (!mw_resample_pcm(&buffer, &resampledBuffer, 44100))
    {
        logError("Failed to resample WAV data for resource %d", index);
        mw_free_buffer(&buffer);
        TracyCZoneEnd(ctx);
        return;
    }

    WarResource* resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_WAVE;
    resource->audio.data = resampledBuffer.data;
    resource->audio.length = resampledBuffer.data_length;

    mw_free_buffer(&buffer);

    TracyCZoneEnd(ctx);
}

void wres_loadVoc(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadVoc", true);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    mv_audio_buffer buffer = {0};
    if (!mv_read_memory((const void*)rawResource.data, rawResource.length, &buffer))
    {
        logError("Failed to read VOC data for resource %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    mv_audio_buffer resampledBuffer = {0};
    if (!mv_resample_pcm(&buffer, &resampledBuffer, 44100))
    {
        logError("Failed to resample VOC data for resource %d", index);
        mv_free_buffer(&buffer);
        TracyCZoneEnd(ctx);
        return;
    }

    WarResource* resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_VOC;
    resource->audio.data = resampledBuffer.data;
    resource->audio.length = resampledBuffer.data_length;

    mv_free_buffer(&buffer);

    TracyCZoneEnd(ctx);
}

void wres_loadCursor(WarContext* context, DatabaseEntry* entry)
{
    TracyCZoneN(ctx, "wres_loadCursor", true);

    u8 paletteData[PALETTE_LENGTH];
    wres_getPalette(context, entry->param1, entry->param2, paletteData);

    u32 rgba[PALETTE_LENGTH/3];
    buildRgbaLut(paletteData, rgba);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.placeholder)
    {
        logInfo("Placeholder resource found at: %d", index);
        TracyCZoneEnd(ctx);
        return;
    }

    u16 hotx = readu16(rawResource.data, 0);
    u16 hoty = readu16(rawResource.data, 2);
    u16 width = readu16(rawResource.data, 4);
    u16 height = readu16(rawResource.data, 6);

    u8 *pixels = (u8*)wm_alloc(width * height * 4 * sizeof(u8));
    for (s32 i = 0; i < width * height; ++i)
    {
        u32 colorIndex = readu8(rawResource.data, 8 + i);
        memcpy(&pixels[i*4], &rgba[colorIndex], sizeof(u32));
    }

    WarResource *resource = wres_getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_CURSOR;
    resource->cursor.hotx = hotx;
    resource->cursor.hoty = hoty;
    resource->cursor.width = width;
    resource->cursor.height = height;
    resource->cursor.pixels = pixels;

    TracyCZoneEnd(ctx);
}

void wres_loadResource(WarContext *context, DatabaseEntry *entry)
{
    TracyCZoneN(ctx, "wres_loadResource", true);

    switch (entry->type)
    {
        case DB_ENTRY_TYPE_PALETTE:
        {
            wres_loadPaletteResource(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_IMAGE:
        {
            wres_loadImageResource(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_SPRITE:
        {
            wres_loadSpriteResource(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_LEVEL_INFO:
        {
            wres_loadLevelInfo(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_LEVEL_VISUAL:
        {
            wres_loadLevelVisual(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_LEVEL_PASSABLE:
        {
            wres_loadLevelPassable(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_TILESET:
        {
            wres_loadTileset(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_TILES:
        {
            wres_loadTiles(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_TEXT:
        {
            wres_loadText(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_XMID:
        {
            wres_loadXmi(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_WAVE:
        {
            wres_loadWave(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_VOC:
        {
            wres_loadVoc(context, entry);
            break;
        }

        case DB_ENTRY_TYPE_CURSOR:
        {
            wres_loadCursor(context, entry);
            break;
        }

        default:
        {
            logDebug("DB entries of type %d aren't handled yet.", entry->type);
            break;
        }
    }

    TracyCZoneEnd(ctx);
}
