#define BUILDING_DAMAGE_1_RESOURCE 352
#define BUILDING_DAMAGE_2_RESOURCE 353
#define BUILDING_COLLAPSE_RESOURCE 356

WarResource* getOrCreateResource(WarContext* context, s32 index)
{
    assert(index >= 0 && index < MAX_RESOURCES_COUNT);
    if (!context->resources[index])
    {
        logInfo("Creating resource: %d", index);
        context->resources[index] = (WarResource*)xcalloc(1, sizeof(WarResource));
    }
    return context->resources[index];
}

void getPalette(WarContext* context, s32 palette1Index, s32 palette2Index, u8 *paletteData)
{
    memset(paletteData, 0, PALETTE_LENGTH);

    if (palette1Index)
    {
        WarResource* palette1 = getOrCreateResource(context, palette1Index);
        u8 *palette1Data = palette1->paletteData.colors;
        memcpy(paletteData, palette1Data, PALETTE_LENGTH);
    }

    // probably only the palettes 191, 194, 197 need this to use a second palette for the background
    // for now leave it whenevenr there is specified a second palette on the entry
    if (palette2Index)
    {
        WarResource* palette2 = getOrCreateResource(context, palette2Index);
        u8 *palette2Data = palette2->paletteData.colors;
        
        for (s32 i = 0; i < 128; ++i)
        {
            if (paletteData[i * 3 + 0] == 252 && 
                paletteData[i * 3 + 1] == 0 && 
                paletteData[i * 3 + 2] == 252)
            {
                paletteData[i * 3 + 0] = palette2Data[i * 3 + 0];
                paletteData[i * 3 + 0] = palette2Data[i * 3 + 1];
                paletteData[i * 3 + 0] = palette2Data[i * 3 + 2];
            }
        }

        for (s32 i = 128; i < 256; ++i)
        {
            if (!(paletteData[i * 3 + 0] == 252 && 
                  paletteData[i * 3 + 1] == 0 && 
                  paletteData[i * 3 + 2] == 252))
            {
                paletteData[i * 3 + 0] = palette2Data[i * 3 + 0];
                paletteData[i * 3 + 1] = palette2Data[i * 3 + 1];
                paletteData[i * 3 + 2] = palette2Data[i * 3 + 2];
            }
        }
    }
}

void loadPaletteResource(WarContext *context, DatabaseEntry *entry)
{
    s32 index = entry->index;
    
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.length < PALETTE_LENGTH)
    {
        rawResource.data = (u8*)xrealloc(rawResource.data, PALETTE_LENGTH);
        memset(rawResource.data + rawResource.length, 0, PALETTE_LENGTH - rawResource.length);
    }

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_PALETTE;

    for (s32 i = 0; i < PALETTE_LENGTH; ++i)
    {
        resource->paletteData.colors[i] = rawResource.data[i] * 4;
    }
}

void loadImageResource(WarContext *context, DatabaseEntry *entry)
{
    u8 paletteData[PALETTE_LENGTH];
    getPalette(context, entry->param1, entry->param2, paletteData);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    u16 width = readu16(rawResource.data, 0);
    u16 height = readu16(rawResource.data, 2);

    u8 *pixels = (u8*)xmalloc(width * height * 4);
    for (int i = 0; i < width * height; ++i)
    {
        u32 colorIndex = readu8(rawResource.data, 4 + i);
        pixels[i * 4 + 0] = readu8(paletteData, colorIndex * 3 + 0);
        pixels[i * 4 + 1] = readu8(paletteData, colorIndex * 3 + 1);
        pixels[i * 4 + 2] = readu8(paletteData, colorIndex * 3 + 2);
        pixels[i * 4 + 3] = 255;
    }

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_IMAGE;
    resource->imageData.width = width;
    resource->imageData.height = height;
    resource->imageData.pixels = pixels;
}

void loadSpriteResource(WarContext *context, DatabaseEntry *entry)
{
    u8 paletteData[PALETTE_LENGTH];
    getPalette(context, entry->param1, entry->param2, paletteData);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    u16 framesCount = readu16(rawResource.data, 0);
    u8 frameWidth = readu8(rawResource.data, 2);
    u8 frameHeight = readu8(rawResource.data, 3);

    WarResource *resource = getOrCreateResource(context, index);
    for (s32 i = 0; i < framesCount; ++i)
    {
        WarSpriteFrame *frame = &resource->spriteData.frames[i];
        frame->dx = readu8(rawResource.data, 4 + i * 8 + 0);
        frame->dy = readu8(rawResource.data, 4 + i * 8 + 1);
        frame->w = readu8(rawResource.data, 4 + i * 8 + 2);
        frame->h = readu8(rawResource.data, 4 + i * 8 + 3);
        frame->off = readu32(rawResource.data, 4 + i * 8 + 4);
        frame->data = (u8*)xcalloc(frameWidth * frameHeight * 4, sizeof(u8));

        // found in war1tool.c, don't know if is needed
        // if (off < 0) {  // High bit of width
        //     off &= 0x7FFFFFFF;
        //     w += 256;
        // }
    }

    for (s32 i = 0; i < framesCount; ++i)
    {
        WarSpriteFrame *frame = &resource->spriteData.frames[i];

        u32 off = frame->off;
        for (int y = frame->dy; y < (frame->dy + frame->h); ++y)
        {
            for (int x = frame->dx; x < (frame->dx + frame->w); ++x)
            {
                u32 pixel = (x + y * frameWidth) * 4;
                u32 colorIndex = rawResource.data[off++];

                if (colorIndex == 96)
                {
                    // Shadow
                    frame->data[pixel + 0] = 0;
                    frame->data[pixel + 1] = 0;
                    frame->data[pixel + 2] = 0;
                    frame->data[pixel + 3] = 127;
                    continue;
                }

                frame->data[pixel + 0] = readu8(paletteData, colorIndex * 3 + 0);
                frame->data[pixel + 1] = readu8(paletteData, colorIndex * 3 + 1);
                frame->data[pixel + 2] = readu8(paletteData, colorIndex * 3 + 2);

                if (frame->data[pixel + 0] > 0 ||
                    frame->data[pixel + 1] > 0 ||
                    frame->data[pixel + 2] > 0)
                {
                    frame->data[pixel + 3] = 255;
                }
            }
        }
    }
    
    resource->type = WAR_RESOURCE_TYPE_SPRITE;
    resource->spriteData.framesCount = framesCount;
    resource->spriteData.frameWidth = frameWidth;
    resource->spriteData.frameHeight = frameHeight;
}

void loadLevelInfo(WarContext *context, DatabaseEntry *entry)
{
    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];
    
    u32 allowId = readu32(rawResource.data, 0);
    
    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_LEVEL_INFO;
    resource->levelInfo.allowId = allowId;
    resource->levelInfo.allowedHumanUnits = 1;
    resource->levelInfo.allowedOrcsUnits = 1;

    memset(resource->levelInfo.allowedFeatures, 0, MAX_FEATURES_COUNT);
    for(s32 f = 0; f < MAX_FEATURES_COUNT; f++)
    {
        // the feature is allowed if the corresponding bit is set
        if (allowId & (1 << (s32)f))
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
            // if it's a spell upgrade check the allowed features, because  spells may not be allowed. 
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

    // starting units
    u32 startEntitiesCount = 0;
    while (offset < rawResource.length)
    {
        u16 val = readu16(rawResource.data, offset);
        if (val == 0xFFFF)
        {
            break;
        }

        WarLevelUnit *startEntity = &resource->levelInfo.startEntities[startEntitiesCount];
        startEntity->x = readu8(rawResource.data, offset + 0) / 2;
        startEntity->y = readu8(rawResource.data, offset + 1) / 2;
        startEntity->type = readu8(rawResource.data, offset + 2);
        startEntity->player = readu8(rawResource.data, offset + 3);

        offset += 4;

        if (startEntity->type == WAR_UNIT_GOLDMINE)
        {
            startEntity->resourceKind = WAR_RESOURCE_GOLD;
            startEntity->amount = readu16(rawResource.data, offset);
            offset += 2;
        }

        startEntitiesCount++;
    }

    resource->levelInfo.startEntitiesCount = startEntitiesCount;

    // skip marker 0xFFFF
    offset += 2;

    // roads
    resource->levelInfo.startRoadsCount = 0;
    while (offset < rawResource.length)
    {
        u16 val = readu16(rawResource.data, offset);
        if (val == 0xFFFF)
        {
            break;
        }

        WarLevelConstruct *construct = &resource->levelInfo.startRoads[resource->levelInfo.startRoadsCount];
        construct->type = WAR_CONSTRUCT_ROAD;
        construct->x1 = readu8(rawResource.data, offset + 0) / 2;
        construct->y1 = readu8(rawResource.data, offset + 1) / 2;
        construct->x2 = readu8(rawResource.data, offset + 2) / 2;
        construct->y2 = readu8(rawResource.data, offset + 3) / 2;
        construct->player = readu8(rawResource.data, offset + 4);

        resource->levelInfo.startRoadsCount++;
        offset += 5;
    }

    // skip marker 0xFFFF
    offset += 2;

    // walls
    resource->levelInfo.startWallsCount = 0;
    while (offset < rawResource.length)
    {
        u16 val = readu16(rawResource.data, offset);
        if (val == 0xFFFF)
        {
            break;
        }

        WarLevelConstruct *construct = &resource->levelInfo.startWalls[resource->levelInfo.startWallsCount];
        construct->type = WAR_CONSTRUCT_WALL;
        construct->x1 = readu8(rawResource.data, offset + 0) / 2;
        construct->y1 = readu8(rawResource.data, offset + 1) / 2;
        construct->x2 = readu8(rawResource.data, offset + 2) / 2;
        construct->y2 = readu8(rawResource.data, offset + 3) / 2;
        construct->player = readu8(rawResource.data, offset + 4);

        resource->levelInfo.startWallsCount++;
        offset += 5;
    }
}

void loadLevelVisual(WarContext *context, DatabaseEntry *entry)
{
    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_LEVEL_VISUAL;
    for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        resource->levelVisual.data[i] = readu16(rawResource.data, i * 2);
    }
}

void loadLevelPassable(WarContext *context, DatabaseEntry *entry)
{
    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_LEVEL_PASSABLE;
    for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        // 128 -> wood, 64 -> water, 16 -> bridge, 0 -> empty
        u16 data = readu16(rawResource.data, i * 2);
        resource->levelPassable.data[i] = data > 0 ? PATH_FINDER_DATA_STATIC : PATH_FINDER_DATA_EMPTY;
    }
}

void loadTileset(WarContext *context, DatabaseEntry *entry)
{
    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    WarResource *tiles = getOrCreateResource(context, entry->param1);

    u8 *data = (u8*)xcalloc(TILESET_WIDTH * TILESET_HEIGHT, sizeof(u8));
    u32 tilesCount = rawResource.length / 8;
    for(s32 i = 0; i < tilesCount; i++)
    {
        for(s32 my = 0; my < 2; my++)
        {
            for(s32 mx = 0; mx < 2; mx++)
            {
                u16 offset = readu16(rawResource.data, i * 8 + (my * 2 + mx) * 2);
                bool flipX = (offset & 0x02);
                bool flipY = (offset & 0x01);
                offset = (u16)((offset & 0xFFFC) << 1);

                local const s32 flip[] = {
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
    getPalette(context, tiles->tilesData.palette1, tiles->tilesData.palette2, paletteData);

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_TILESET;
    resource->tilesetData.tilesCount = rawResource.length / 8;

    for(s32 i = 0; i < TILESET_WIDTH * TILESET_HEIGHT; i++)
    {
        resource->tilesetData.data[i * 4 + 0] = paletteData[data[i] * 3 + 0];
        resource->tilesetData.data[i * 4 + 1] = paletteData[data[i] * 3 + 1];
        resource->tilesetData.data[i * 4 + 2] = paletteData[data[i] * 3 + 2];
        resource->tilesetData.data[i * 4 + 3] = data[i] > 0 ? 255 : 0;
    }
    
    free(data);

#if _DEBUG
    {
        char fp[30];
        sprintf(fp, "output_%d.png", index);

        stbi_write_png(fp, TILESET_WIDTH, TILESET_HEIGHT, 4, resource->tilesetData.data, TILESET_WIDTH * 4);
    }
#endif
}

void loadTiles(WarContext *context, DatabaseEntry *entry)
{
    u8 paletteData[PALETTE_LENGTH];
    getPalette(context, entry->param1, entry->param2, paletteData);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_TILES;
    resource->tilesData.palette1 = entry->param1;
    resource->tilesData.palette2 = entry->param2;
    resource->tilesData.data = (u8*)xcalloc(rawResource.length, sizeof(u8));
    memcpy(resource->tilesData.data, rawResource.data, rawResource.length);
}

void loadText(WarContext *context, DatabaseEntry *entry)
{
    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_TEXT;
    resource->textData.length = rawResource.length;
    resource->textData.text = (char *)xcalloc(resource->textData.length, sizeof(char));
    memcpy(resource->textData.text, rawResource.data, resource->textData.length);
}