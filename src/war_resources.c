const char* features[MAX_FEATURES_COUNT*2] = {
	// Units. 0 - 6
	"unit-footman", "unit-grunt",
	"unit-peasant", "unit-peon",
	"unit-human-catapult", "unit-orc-catapult",
	"unit-knight", "unit-raider",
	"unit-archer", "unit-spearman",
	"unit-conjurer", "unit-warlock",
	"unit-cleric", "unit-necrolyte",
	// Constructing buildings. 7 - 14
	"unit-human-farm", "unit-orc-farm",
	"unit-human-barracks", "unit-orc-barracks",
	"unit-human-church", "unit-orc-temple",
	"unit-human-tower", "unit-orc-tower",
	"unit-human-town-hall", "unit-orc-town-hall",
	"unit-human-lumber-mill", "unit-orc-lumber-mill",
	"unit-human-stable", "unit-orc-kennel",
	"unit-human-blacksmith", "unit-orc-blacksmith",
	// Cleric/Necrolyte spells. 15 - 17
	"upgrade-healing", "upgrade-raise-dead",
	"upgrade-holy-vision", "upgrade-dark-vision",
	"upgrade-invisibility", "upgrade-unholy-armor",
	// Conjurer/Warlock spells. 18 - 20
	"upgrade-scorpion", "upgrade-spider",
	"upgrade-rain-of-fire", "upgrade-poison-cloud",
	"upgrade-water-elemental", "upgrade-daemon",
	// Roads and walls. 21 - 22
	"unit-road", "unit-road",
	"unit-wall", "unit-wall"
};

const char* upgradeNames[MAX_UPGRADES_COUNT*2] = {
    // Basic upgrades
    "upgrade-spear", "upgrade-arrow",
    "upgrade-axe", "upgrade-sword",
    "upgrade-wolves", "upgrade-horse",
    // Spells and summons
    "upgrade-spider", "upgrade-scorpion",
    "upgrade-poison-cloud", "upgrade-rain-of-fire",
    "upgrade-daemon", "upgrade-water-elemental",
    "upgrade-raise-dead", "upgrade-healing",
    "upgrade-dark-vision", "upgrade-far-seeing",
    "upgrade-unholy-armor", "upgrade-invisibility",
    // Shield upgrades
    "upgrade-orc-shield", "upgrade-human-shield"
};

s32 createTexture(WarContext *context)
{
    if (context->texturesCount >= MAX_TEXTURES_COUNT)
    {
        printf("Max textures count reached!\n");
        return -1;
    }

    s32 textureIndex = context->texturesCount;

    GLuint tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    context->textures[textureIndex] = tex;
    context->texturesCount++;
    return textureIndex;
}

void setTextureData(WarContext *context, u32 textureIndex, u32 width, u32 height, u8* pixels)
{
    assert(textureIndex >= 0 && textureIndex < context->texturesCount);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void getPalette(WarContext *context, s32 palette1Index, s32 palette2Index, u8 *paletteData)
{
    memset(paletteData, 0, PALETTE_LENGTH);

    if (palette1Index)
    {
        u8 *palette1Data = context->resources[palette1Index]->paletteData.colors;
        memcpy(paletteData, palette1Data, PALETTE_LENGTH);
    }

    // probably only the palettes 191, 194, 197 need this to use a second palette for the background
    // for now leave it whenevenr there is specified a second palette on the entry
    if (palette2Index)
    {
        u8 *palette2Data = context->resources[palette2Index]->paletteData.colors;
        
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

WarResource* getOrCreateResource(WarContext *context, s32 index)
{
    assert(index >= 0 && index < MAX_RESOURCES_COUNT);
    if (!context->resources[index])
    {
        context->resources[index] = (WarResource*)calloc(1, sizeof(WarResource));
    }
    return context->resources[index];
}

void loadPaletteResource(WarContext *context, DatabaseEntry *entry)
{
    s32 index = entry->index;
    
    WarRawResource rawResource = context->warFile->resources[index];
    if (rawResource.length < PALETTE_LENGTH)
    {
        rawResource.data = (u8*)realloc(rawResource.data, PALETTE_LENGTH);
        memset(rawResource.data + rawResource.length, 0, PALETTE_LENGTH - rawResource.length);
    }

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_PALETTE;

    for (s32 i = 0; i < PALETTE_LENGTH; ++i)
    {
        resource->paletteData.colors[i] = rawResource.data[i] * 4;
    }

    context->resourcesCount++;
}

void loadImageResource(WarContext *context, DatabaseEntry *entry)
{
    u8 paletteData[PALETTE_LENGTH];
    getPalette(context, entry->param1, entry->param2, paletteData);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    u16 width = readu16(rawResource.data, 0);
    u16 height = readu16(rawResource.data, 2);

    u8 *pixels = (u8*)malloc(width * height * 4);
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

    context->resourcesCount++;       
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
        frame->data = (u8*)calloc(frameWidth * frameHeight * 4, sizeof(u8));

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
    
    context->resourcesCount++;
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

    // next level resource index
    u16 nextLevelIndex = readu16(rawResource.data, 0xCA);
    if (nextLevelIndex = 0 || nextLevelIndex == 0xFFFF) nextLevelIndex = 0;
    resource->levelInfo.nextLevelIndex = nextLevelIndex;

    // Here are some chunk numbers, please see Terrain Data.
    // Chunk number seem to be off (+2). Probably array base trouble.
    // 0x00D0 - 0x00D1: Word: Chunk number with terrain tile data.
    // 0x00D2 - 0x00D3: Word: Chunk number with terrain flags information.
    // 0x00D4 - 0x00D6: Word: Chunk number of tileset palette.
    // 0x00D6 - 0x00D7: Word: Chunk number of tileset info.
    // 0x00D8 - 0x00D9: Word: Chunk number of tileset image data.

    // visual resource index
    u16 visualIndex = readu16(rawResource.data, 0xD0);
    if (visualIndex == 0 || visualIndex == 0xFFFF) visualIndex = 0;
    resource->levelInfo.visualIndex = visualIndex;

    // passable resource index
    u16 passableIndex = readu16(rawResource.data, 0xD2);
    if (passableIndex == 0 || passableIndex == 0xFFFF) passableIndex = 0;
    resource->levelInfo.passableIndex = passableIndex;

    // palette resource index
    u16 paletteIndex = readu16(rawResource.data, 0xD4);
    if (paletteIndex == 0 || paletteIndex == 0xFFFF) paletteIndex = 0;
    resource->levelInfo.paletteIndex = paletteIndex;

    // tileset resource index
    u16 tilesetIndex = readu16(rawResource.data, 0xD6);
    if (tilesetIndex == 0 || tilesetIndex == 0xFFFF) tilesetIndex = 0;
    resource->levelInfo.tilesetIndex = tilesetIndex;

    // tiles resource index
    u16 tilesIndex = readu16(rawResource.data, 0xD8);
    if (tilesIndex == 0 || tilesIndex == 0xFFFF) tilesIndex = 0;
    resource->levelInfo.tilesIndex = tilesIndex;

    // 0xE3 start of dynamic data
    // TODO: Needs figuring out how this is actually stored and what the data means
    // For now, just search for the next chunk
    int offset = 0xE3;
    while (readu32(rawResource.data, offset) != 0xFFFFFFFF) 
    {
        offset++;
    }

    // skip marker 0xFFFF
    offset += 4;
    offset = readu16(rawResource.data, offset);

    // starting units
    resource->levelInfo.startEntitiesCount = 0;
    while (offset < rawResource.length)
    {
        u16 val = readu16(rawResource.data, offset);
        if (val == 0xFFFF)
        {
            break;
        }

        WarLevelUnit *startEntity = &resource->levelInfo.startEntities[resource->levelInfo.startEntitiesCount];
        startEntity->x = readu8(rawResource.data, offset + 0) / 2;
        startEntity->y = readu8(rawResource.data, offset + 1) / 2;
        startEntity->type = readu8(rawResource.data, offset + 2);
        startEntity->player = readu8(rawResource.data, offset + 3);

        offset += 4;

        if (startEntity->type == WAR_UNIT_GOLDMINE)
        {
            startEntity->value = readu16(rawResource.data, offset);
            offset += 2;
        }

        resource->levelInfo.startEntitiesCount++;
    }

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
    for(s32 i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
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
    for(s32 i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
    {
        resource->levelPassable.data[i] = readu16(rawResource.data, i * 2);
    }
}

void loadTileset(WarContext *context, DatabaseEntry *entry)
{
    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_TILESET;
    resource->tilesetData.tilesCount = rawResource.length / 8;

    WarResource *tiles = getOrCreateResource(context, entry->param1);

    u8 paletteData[PALETTE_LENGTH];
    getPalette(context, tiles->tilesData.pal1, tiles->tilesData.pal2, paletteData);

#define TILES_PER_ROW 16

    s32 numtiles = resource->tilesetData.tilesCount;
    s32 w = TILES_PER_ROW * 16;
    s32 h = ((numtiles + TILES_PER_ROW - 1) / TILES_PER_ROW) * 16;
    u8 *data1 = (u8*)calloc(w * h, sizeof(u8));

    for(s32 i = 0; i < numtiles; i++)
    {
        WarTilesetTile *tilesetTile = &resource->tilesetData.tiles[i];
        memset(tilesetTile->tilesetData, 0, sizeof(tilesetTile->tilesetData));

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

                s32 ix = mx + (i % TILES_PER_ROW) * 2;
                s32 iy = my + (i / TILES_PER_ROW) * 2;

                for (s32 y = 0; y < 8; ++y) {
                    for (s32 x = 0; x < 8; ++x) {
                        data1[(y + iy * 8) * w + ix * 8 + x] = tiles->tilesData.data[offset + (flipY ? flip[y] : y) * 8 + (flipX ? flip[x] : x)];
                    }
                }




                // for (s32 py = 0; py < 8; py++)
                // {
                //     for (s32 px = 0; px < 8; px++)
                //     {
                //         u8 ci = tiles->tilesData.data[offset++];
                        
                //         u8 r = paletteData[ci * 3 + 0];
                //         u8 g = paletteData[ci * 3 + 1];
                //         u8 b = paletteData[ci * 3 + 2];
                //         u8 a = 255;

                //         s32 xPos = (flipX ? (mx * 8) + ((7 + (mx * 8)) - px) : px);
                //         s32 yPos = (flipY ? (my * 8) + ((7 + (my * 8)) - py) : py);

                //         tilesetTile->tilesetData[((my*8 + yPos) * 16 + (mx*8 + xPos)) * 4 + 0] = r;
                //         tilesetTile->tilesetData[((my*8 + yPos) * 16 + (mx*8 + xPos)) * 4 + 1] = g;
                //         tilesetTile->tilesetData[((my*8 + yPos) * 16 + (mx*8 + xPos)) * 4 + 2] = b;
                //         tilesetTile->tilesetData[((my*8 + yPos) * 16 + (mx*8 + xPos)) * 4 + 3] = a;


                //         // s32 z1 = offset / (TILES_WIDTH * TILES_HEIGHT);
                //         // s32 z2 = (py * 8 + px) * 4;

                //         // u8 r = tiles->tilesData.tiles[z1][z2 + 0];
                //         // u8 g = tiles->tilesData.tiles[z1][z2 + 1];
                //         // u8 b = tiles->tilesData.tiles[z1][z2 + 2];
                //         // u8 a = tiles->tilesData.tiles[z1][z2 + 3];

                //         // s32 xPos = (flipX ? (mx * 8) + ((7 + (mx * 8)) - px) : px);
                //         // s32 yPos = (flipY ? (my * 8) + ((7 + (my * 8)) - py) : py);

                //         // tilesetTile->tilesetData[((my*8 + yPos) * 16 + (mx*8 + xPos)) * 4 + 0] = r;
                //         // tilesetTile->tilesetData[((my*8 + yPos) * 16 + (mx*8 + xPos)) * 4 + 1] = g;
                //         // tilesetTile->tilesetData[((my*8 + yPos) * 16 + (mx*8 + xPos)) * 4 + 2] = b;
                //         // tilesetTile->tilesetData[((my*8 + yPos) * 16 + (mx*8 + xPos)) * 4 + 3] = a;
                //     }
                // }
            }
        }
    }

    u8 *data2 = (u8*)calloc(w * h * 4, sizeof(u8));
    
    for(s32 i = 0; i < w * h; i++)
    {
        data2[i * 4 + 0] = paletteData[data1[i] * 3 + 0];
        data2[i * 4 + 1] = paletteData[data1[i] * 3 + 1];
        data2[i * 4 + 2] = paletteData[data1[i] * 3 + 2];

        if (data1[i] > 0)
        {
            data2[i * 4 + 3] = 255;
        }
    }
    

    //if (index == 189 && i < 10)
    {
        char fp[30];
        sprintf(fp, "output_%d.bmp", index);

        u8 *output_pixels = (u8*)calloc(w*2*h*2*4, sizeof(u8));
        stbir_resize_uint8(
            data2, w, h , 0,
            output_pixels, w*2, h*2, 
            0, 4);

        stbi_write_bmp(fp, w*2, h*2, 4, output_pixels);

        free(output_pixels);
    }

    free(data1);
    free(data2);
}

void loadTiles(WarContext *context, DatabaseEntry *entry)
{
    u8 paletteData[PALETTE_LENGTH];
    getPalette(context, entry->param1, entry->param2, paletteData);

    s32 index = entry->index;
    WarRawResource rawResource = context->warFile->resources[index];

    WarResource *resource = getOrCreateResource(context, index);
    resource->type = WAR_RESOURCE_TYPE_TILES;
    resource->tilesData.tilesCount = rawResource.length / (TILES_WIDTH * TILES_HEIGHT);

    resource->tilesData.pal1 = entry->param1;
    resource->tilesData.pal2 = entry->param2;
    resource->tilesData.data = (u8*)calloc(rawResource.length, sizeof(u8));
    memcpy(resource->tilesData.data, rawResource.data, rawResource.length);
    
    for(s32 i = 0; i < resource->tilesData.tilesCount; i++)
    {
        for(s32 j = 0; j < TILES_WIDTH * TILES_HEIGHT; j++)
        {
            u32 colorIndex = readu8(rawResource.data, i * 64 + j);
            resource->tilesData.tiles[i][j * 4 + 0] = paletteData[colorIndex * 3 + 0];
            resource->tilesData.tiles[i][j * 4 + 1] = paletteData[colorIndex * 3 + 1];
            resource->tilesData.tiles[i][j * 4 + 2] = paletteData[colorIndex * 3 + 2];

            if (resource->tilesData.tiles[i][j * 4 + 0] > 0 ||
                resource->tilesData.tiles[i][j * 4 + 1] > 0 ||
                resource->tilesData.tiles[i][j * 4 + 2] > 0)
            {
                resource->tilesData.tiles[i][j * 4 + 3] = 255;
            }
        }

        //if (i < 50)
        // {
        //     char fp[30];
        //     sprintf(fp, "output_%d.bmp", i);

        //     u8 output_pixels[128*128*4];
        //     stbir_resize_uint8(
        //         resource->tilesData.tiles[i] , 8, 8 , 0,
        //         output_pixels, 128, 128, 
        //         0, 4);

        //     stbi_write_bmp(fp, 128, 128, 4, output_pixels);
        // }
    }
}