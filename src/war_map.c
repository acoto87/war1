void addEntityToSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;

    // subtitute this with a set data structure that doesn't allow duplicates
    if (!WarEntityIdListContains(&map->selectedEntities, id))
        WarEntityIdListAdd(&map->selectedEntities, id);
}

void removeEntityFromSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    WarEntityIdListRemove(&map->selectedEntities, id);
}

void clearSelection(WarContext* context)
{
    WarMap* map = context->map;
    WarEntityIdListClear(&map->selectedEntities);
}

vec2 vec2ScreenToMapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    rect mapPanel = map->mapPanel;
    rect viewport = map->viewport;

    v = vec2Translatef(v, -mapPanel.x, -mapPanel.y);
    v = vec2Translatef(v, viewport.x, viewport.y);
    return v;
}

vec2 vec2ScreenToMinimapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    rect minimapPanel = map->minimapPanel;

    v = vec2Translatef(v, -minimapPanel.x, -minimapPanel.y);
    return v;
}

rect rectScreenToMapCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;

    rect mapPanel = map->mapPanel;
    rect viewport = map->viewport;

    r = rectTranslatef(r, -mapPanel.x, -mapPanel.y);
    r = rectTranslatef(r, viewport.x, viewport.y);
    return r;
}

vec2 vec2MapToScreenCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    v = vec2Translatef(v, -map->viewport.x, -map->viewport.y);
    v = vec2Translatef(v, map->mapPanel.x, map->mapPanel.y);
    return v;
}

rect rectMapToScreenCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;

    r = rectTranslatef(r, -map->viewport.x, -map->viewport.y);
    r = rectTranslatef(r, map->mapPanel.x, map->mapPanel.y);
    return r;
}

vec2 vec2MapToTileCoordinates(vec2 v)
{
    v.x = floorf(v.x / MEGA_TILE_WIDTH);
    v.y = floorf(v.y / MEGA_TILE_HEIGHT);
    return v;
}

vec2 vec2TileToMapCoordinates(vec2 v, bool centeredInTile)
{
    v.x *= MEGA_TILE_WIDTH;
    v.y *= MEGA_TILE_HEIGHT;

    if (centeredInTile)
    {
        v.x += halfi(MEGA_TILE_WIDTH);
        v.y += halfi(MEGA_TILE_HEIGHT);
    }

    return v;
}

vec2 vec2MinimapToViewportCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    rect minimapPanel = map->minimapPanel;
    vec2 minimapPanelSize = vec2f(minimapPanel.width, minimapPanel.height);

    vec2 minimapViewportSize = vec2f(MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT);

    v = vec2Translatef(v, -minimapViewportSize.x / 2, -minimapViewportSize.y / 2);
    v = vec2Clampv(v, VEC2_ZERO, vec2Subv(minimapPanelSize, minimapViewportSize));
    return v;
}

WarMapTile* getMapTileState(WarMap* map, s32 x, s32 y)
{
    assert(inRange(x, 0, MAP_TILES_WIDTH) && inRange(y, 0, MAP_TILES_HEIGHT));
    return &map->tiles[y * MAP_TILES_WIDTH + x];
}

void setMapTileState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState tileState)
{
    if (startX <= 0)
        startX = 0;

    if (startY <= 0)
        startY = 0;

    if (startX + width >= MAP_TILES_WIDTH)
        width = MAP_TILES_WIDTH - startX;

    if (startY + height >= MAP_TILES_HEIGHT)
        height = MAP_TILES_HEIGHT - startY;

    s32 endX = startX + width;
    s32 endY = startY + height;

    for(s32 y = startY; y < endY; y++)
    {
        for(s32 x = startX; x < endX; x++)
        {
            // exclude the corners of the area to get a more "rounded" shape
            if ((y == startY || y == endY - 1) && (x == startX || x == endX - 1))
                continue;

            WarMapTile* tile = getMapTileState(map, x, y);
            tile->state = tileState;
        }
    }
}

void setUnitMapTileState(WarMap* map, WarEntity* entity, WarMapTileState tileState)
{
    assert(isUnit(entity));

    s32 sight = getUnitSightRange(entity);

    vec2 position = getUnitPosition(entity, true);
    vec2 unitSize = getUnitSize(entity);
    rect unitRect = rectv(position, unitSize);
    unitRect = rectExpand(unitRect, sight, sight);

    setMapTileState(map, unitRect.x, unitRect.y, unitRect.width, unitRect.height, tileState);
}

bool isTileInState(WarMap* map, s32 x, s32 y, WarMapTileState state)
{
    if (!map->fowEnabled)
    {
        switch (state)
        {
            case MAP_TILE_STATE_UNKOWN: return false;
            case MAP_TILE_STATE_FOG: return false;
            case MAP_TILE_STATE_VISIBLE: return true;
            default:
            {
                logError("Unkown state: %d. Defaulting to true.", state);
                return true;
            }
        }
    }

    WarMapTile* tile = getMapTileState(map, x, y);
    return tile->state == state;
}

bool isAnyTileInStates(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state)
{
    if (!map->fowEnabled)
    {
        switch (state)
        {
            case MAP_TILE_STATE_UNKOWN: return false;
            case MAP_TILE_STATE_FOG: return false;
            case MAP_TILE_STATE_VISIBLE: return true;
            default:
            {
                logError("Unkown state: %d. Defaulting to true.", state);
                return true;
            }
        }
    }

    if (startX <= 0)
        startX = 0;

    if (startY <= 0)
        startY = 0;

    if (startX + width >= MAP_TILES_WIDTH)
        width = MAP_TILES_WIDTH - startX;

    if (startY + height >= MAP_TILES_HEIGHT)
        height = MAP_TILES_HEIGHT - startY;

    s32 endX = startX + width;
    s32 endY = startY + height;

    for(s32 y = startY; y < endY; y++)
    {
        for(s32 x = startX; x < endX; x++)
        {
            WarMapTile* tile = getMapTileState(map, x, y);
            if (tile->state == state)
            {
                return true;
            }
        }
    }

    return false;
}

bool isAnyUnitTileInStates(WarMap* map, WarEntity* entity, WarMapTileState state)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    vec2 position = getUnitPosition(entity, true);
    return isAnyTileInStates(map, position.x, position.y, unit->sizex, unit->sizey, state);
}

bool areAllTilesInState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state)
{
    if (!map->fowEnabled)
    {
        switch (state)
        {
            case MAP_TILE_STATE_UNKOWN: return false;
            case MAP_TILE_STATE_FOG: return false;
            case MAP_TILE_STATE_VISIBLE: return true;
            default:
            {
                logError("Unkown state: %d. Defaulting to true.", state);
                return true;
            }
        }
    }

    if (startX <= 0)
        startX = 0;

    if (startY <= 0)
        startY = 0;

    if (startX + width >= MAP_TILES_WIDTH)
        width = MAP_TILES_WIDTH - startX;

    if (startY + height >= MAP_TILES_HEIGHT)
        height = MAP_TILES_HEIGHT - startY;

    s32 endX = startX + width;
    s32 endY = startY + height;

    for(s32 y = startY; y < endY; y++)
    {
        for(s32 x = startX; x < endX; x++)
        {
            WarMapTile* tile = getMapTileState(map, x, y);
            if (tile->state != state)
            {
                return false;
            }
        }
    }

    return true;
}

bool areAllUnitTilesInState(WarMap* map, WarEntity* entity, WarMapTileState state)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    vec2 position = getUnitPosition(entity, true);
    return areAllTilesInState(map, position.x, position.y, unit->sizex, unit->sizey, state);
}

u8Color getMapTileAverage(WarResource* levelVisual, WarResource* tileset, s32 x, s32 y)
{
    s32 index = y * MAP_TILES_WIDTH + x;
    u16 tileIndex = levelVisual->levelVisual.data[index];

    s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
    s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

    s32 r = 0, g = 0, b = 0;

    for(s32 ty = 0; ty < MEGA_TILE_HEIGHT; ty++)
    {
        for(s32 tx = 0; tx < MEGA_TILE_WIDTH; tx++)
        {
            s32 pixel = (tilePixelY + ty) * TILESET_WIDTH + (tilePixelX + tx);
            r += tileset->tilesetData.data[pixel * 4 + 0];
            g += tileset->tilesetData.data[pixel * 4 + 1];
            b += tileset->tilesetData.data[pixel * 4 + 2];
        }
    }

    r /= 256;
    g /= 256;
    b /= 256;

    u8Color color = {0};
    color.r = (u8)r;
    color.g = (u8)g;
    color.b = (u8)b;
    color.a = 255;
    return color;
}

void updateMinimapTile(WarContext* context, WarResource* levelVisual, WarResource* tileset, s32 x, s32 y)
{
    WarMap* map = context->map;
    WarSpriteFrame* minimapFrame = &map->minimapSprite.frames[1];

    u8Color color = U8COLOR_BLACK;

    s32 index = y * MAP_TILES_WIDTH + x;
    WarMapTile* tile = &map->tiles[index];

    if (!map->fowEnabled ||
        tile->state == MAP_TILE_STATE_VISIBLE ||
        tile->state == MAP_TILE_STATE_FOG)
    {
        color = getMapTileAverage(levelVisual, tileset, x, y);
    }

    minimapFrame->data[index * 4 + 0] = color.r;
    minimapFrame->data[index * 4 + 1] = color.g;
    minimapFrame->data[index * 4 + 2] = color.b;
    minimapFrame->data[index * 4 + 3] = color.a;
}

s32 getMapTileIndex(WarContext* context, s32 x, s32 y)
{
    WarMap* map = context->map;

    WarResource* levelInfo = getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource* levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
    assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

    WarMapTilesetType tilesetType = map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    return levelVisual->levelVisual.data[y * MAP_TILES_WIDTH + x];
}

void setMapTileIndex(WarContext* context, s32 x, s32 y, s32 tile)
{
    WarMap* map = context->map;

    WarResource* levelInfo = getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource* levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
    assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

    WarResource* tileset = getOrCreateResource(context, levelInfo->levelInfo.tilesetIndex);
    assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

    levelVisual->levelVisual.data[y * MAP_TILES_WIDTH + x] = tile;

    updateMinimapTile(context, levelVisual, tileset, x, y);
}

f32 getMapScaledSpeed(WarContext* context, f32 t)
{
    WarMap* map = context->map;

    t = getScaledSpeed(context, t);

    if (map->settings.gameSpeed < WAR_SPEED_NORMAL)
        t *= 1.0f - (WAR_SPEED_NORMAL - map->settings.gameSpeed) * 0.25f;
    else if (map->settings.gameSpeed > WAR_SPEED_NORMAL)
        t *= 1.0f + (map->settings.gameSpeed - WAR_SPEED_NORMAL) * 0.5f;

    return t;
}

f32 getMapScaledTime(WarContext* context, f32 t)
{
    WarMap* map = context->map;

    t = getScaledTime(context, t);

    if (map->settings.gameSpeed < WAR_SPEED_NORMAL)
        t /= 1.0f - (WAR_SPEED_NORMAL - map->settings.gameSpeed) * 0.25f;
    else if (map->settings.gameSpeed > WAR_SPEED_NORMAL)
        t /= 1.0f + (map->settings.gameSpeed - WAR_SPEED_NORMAL) * 0.5f;

    return t;
}

WarMap* createMap(WarContext* context, s32 levelInfoIndex)
{
    WarMap *map = (WarMap*)xcalloc(1, sizeof(WarMap));
    map->levelInfoIndex = levelInfoIndex;

    initEntityManager(&map->entityManager);

    WarEntityIdListInit(&map->selectedEntities, WarEntityIdListDefaultOptions);

    return map;
}

WarMap* createCustomMap(WarContext* context, s32 levelInfoIndex, WarRace yourRace, WarRace enemyRace)
{
    WarMap* map = createMap(context, levelInfoIndex);

    WarResource* levelInfo = getOrCreateResource(context, levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO && levelInfo->levelInfo.customMap);

    levelInfo->levelInfo.startEntitiesCount = 0;
    memset(levelInfo->levelInfo.startEntities, 0, sizeof(levelInfo->levelInfo.startEntities));

    levelInfo->levelInfo.races[0] = yourRace;
    levelInfo->levelInfo.races[1] = enemyRace;

    for (s32 i = 0; i < levelInfo->levelInfo.startGoldminesCount; i++)
    {
        WarLevelUnit* startUnitConf = &levelInfo->levelInfo.startGoldmines[i];

        WarLevelUnit* startUnit = &levelInfo->levelInfo.startEntities[levelInfo->levelInfo.startEntitiesCount];
        startUnit->x = startUnitConf->x;
        startUnit->y = startUnitConf->y;
        startUnit->type = startUnitConf->type;
        startUnit->player = startUnitConf->player;
        startUnit->resourceKind = WAR_RESOURCE_GOLD;
        startUnit->amount = randomi(20000, 30000);

        levelInfo->levelInfo.startEntitiesCount++;
    }

    s32 configurationIndex = randomi(0, levelInfo->levelInfo.startConfigurationsCount);
    WarCustomMapConfiguration* configuration = &levelInfo->levelInfo.startConfigurations[configurationIndex];

    for (s32 i = 0; i < configuration->startEntitiesCount; i++)
    {
        WarLevelUnit* startUnitConf = &configuration->startEntities[i];

        WarLevelUnit* startUnit = &levelInfo->levelInfo.startEntities[levelInfo->levelInfo.startEntitiesCount];
        startUnit->x = startUnitConf->x;
        startUnit->y = startUnitConf->y;
        startUnit->player = startUnitConf->player;
        startUnit->type = startUnit->player == 0
            ? getUnitTypeForRace(startUnitConf->type, yourRace)
            : getUnitTypeForRace(startUnitConf->type, enemyRace);

        levelInfo->levelInfo.startEntitiesCount++;
    }

    return map;
}

void freeMap(WarContext* context, WarMap* map)
{
    freeSprite(context, map->sprite);
    freeSprite(context, map->minimapSprite);
    freeSprite(context, map->blackSprite);

    WarEntityManager* manager = &map->entityManager;
    WarEntityListFree(&manager->entities);
    WarEntityMapFree(&manager->entitiesByType);
    WarUnitMapFree(&manager->unitsByType);
    WarEntityIdMapFree(&manager->entitiesById);
    WarEntityListFree(&manager->uiEntities);

    WarEntityIdListFree(&map->selectedEntities);

    // these are already free when the lists and maps are
    // freeEntity(map->forest);
    // freeEntity(map->wall);
    // freeEntity(map->road);
    // freeEntity(map->ruin);

    free(map->finder.data);
}

void enterMap(WarContext* context)
{
    WarMap* map = context->map;

    s32 levelInfoIndex = map->levelInfoIndex;

    WarResource* levelInfo = getOrCreateResource(context, levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource* levelPassable = getOrCreateResource(context, levelInfo->levelInfo.passableIndex);
    assert(levelPassable && levelPassable->type == WAR_RESOURCE_TYPE_LEVEL_PASSABLE);

    map->playing = true;
    map->custom = levelInfo->levelInfo.customMap;
    map->tilesetType = levelInfo->levelInfo.tilesetType;
    map->fowEnabled = true;
    map->result = WAR_LEVEL_RESULT_NONE;
    map->objectivesTime = 1;

    map->settings.gameSpeed = WAR_SPEED_NORMAL;
    map->settings.mouseScrollSpeed = WAR_SPEED_NORMAL;
    map->settings.keyScrollSpeed = WAR_SPEED_NORMAL;

    map->leftTopPanel = recti(0, 0, 72, 72);
    map->leftBottomPanel = recti(0, 72, 72, 128);
    map->rightPanel = recti(312, 0, 8, 200);
    map->topPanel = recti(72, 0, 240, 12);
    map->bottomPanel = recti(72, 188, 240, 12);
    map->mapPanel = recti(72, 12, MAP_VIEWPORT_WIDTH, MAP_VIEWPORT_HEIGHT);
    map->minimapPanel = recti(3, 6, MINIMAP_WIDTH, MINIMAP_HEIGHT);
    map->menuPanel = recti(84, 32, 152, 136);
    map->messagePanel = recti(17, 76, 286, 48);
    map->saveLoadPanel = recti(48, 27, 223, 146);

    s32 startX = levelInfo->levelInfo.startX * MEGA_TILE_WIDTH;
    s32 startY = levelInfo->levelInfo.startY * MEGA_TILE_HEIGHT;
    map->viewport = recti(startX, startY, MAP_VIEWPORT_WIDTH, MAP_VIEWPORT_HEIGHT);

    map->finder = initPathFinder(PATH_FINDING_ASTAR, MAP_TILES_WIDTH, MAP_TILES_HEIGHT, levelPassable->levelPassable.data);

    // create the black sprite
    {
        u8 data[MEGA_TILE_WIDTH * MEGA_TILE_HEIGHT * 4];
        memset(data, 0, MEGA_TILE_WIDTH * MEGA_TILE_HEIGHT * 4);
        for (s32 i = 0; i < MEGA_TILE_WIDTH * MEGA_TILE_HEIGHT; i++)
            data[4 * i + 3] = 255;

        map->blackSprite = createSprite(context, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT, data);
    }

    // set the initial state for the tiles
    {
        for (s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
        {
            WarMapTile* tile = &map->tiles[i];

            tile->state = MAP_TILE_STATE_UNKOWN;
            tile->type = WAR_FOG_PIECE_NONE;
            tile->boundary = WAR_FOG_BOUNDARY_NONE;
        }
    }

    // create the map sprite
    {
        WarResource* levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
        assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

        WarResource* tileset = getOrCreateResource(context, levelInfo->levelInfo.tilesetIndex);
        assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

        // DEBUG:
        // print level visual data to console to see the sprites of the map
        //
        // for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
        // {
        //     for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        //     {
        //         // index of the tile in the tilesheet
        //         u16 tileIndex = levelVisual->levelVisual.data[y * MAP_TILES_WIDTH + x];
        //         printf("%d ", tileIndex);
        //     }

        //     printf("\n");
        // }

        map->sprite = createSprite(context, TILESET_WIDTH, TILESET_HEIGHT, tileset->tilesetData.data);

        // the minimap sprite will be a 2 frames sprite
        // the first one will be the frame that actually render
        // the second one will be the minimap for the terrain, created at startup time,
        // that way I only have to memcpy to the first frame and do the work only for the units
        // that way I also don't have to allocate memory for the minimap each frame
        WarSpriteFrame minimapFrames[2];

        for(s32 i = 0; i < 2; i++)
        {
            minimapFrames[i].dx = 0;
            minimapFrames[i].dy = 0;
            minimapFrames[i].w = MINIMAP_WIDTH;
            minimapFrames[i].h = MINIMAP_HEIGHT;
            minimapFrames[i].off = 0;
            minimapFrames[i].data = (u8*)xcalloc(MINIMAP_WIDTH * MINIMAP_HEIGHT * 4, sizeof(u8));

            // make the frame black
            for (s32 k = 0; k < MINIMAP_WIDTH * MINIMAP_HEIGHT; k++)
                minimapFrames[i].data[k * 4 + 3] = 255;
        }

        for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
        {
            for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
            {
                u8Color color = getMapTileAverage(levelVisual, tileset, x, y);
                s32 index = y * MAP_TILES_WIDTH + x;
                minimapFrames[1].data[index * 4 + 0] = color.r;
                minimapFrames[1].data[index * 4 + 1] = color.g;
                minimapFrames[1].data[index * 4 + 2] = color.b;
                minimapFrames[1].data[index * 4 + 3] = color.a;
            }
        }

        map->minimapSprite = createSpriteFromFrames(context, MINIMAP_WIDTH, MINIMAP_HEIGHT, arrayLength(minimapFrames), minimapFrames);
    }

    // create the forest entities
    {
        bool processed[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
        for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
            processed[i] = false;

        u16* passableData = levelPassable->levelPassable.data;
        for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
        {
            if (!processed[i] && passableData[i] == 128)
            {
                s32 x = i % MAP_TILES_WIDTH;
                s32 y = i / MAP_TILES_WIDTH;

                WarTreeList trees;
                WarTreeListInit(&trees, WarTreeListDefaultOptions);
                WarTreeListAdd(&trees, createTree(x, y, TREE_MAX_WOOD));
                processed[i] = true;

                for(s32 j = 0; j < trees.count; j++)
                {
                    WarTree tree = trees.items[j];
                    for(s32 d = 0; d < dirC; d++)
                    {
                        s32 xx = tree.tilex + dirX[d];
                        s32 yy = tree.tiley + dirY[d];
                        if (isInside(map->finder, xx, yy))
                        {
                            s32 k = yy * MAP_TILES_WIDTH + xx;
                            if (!processed[k] && passableData[k] == 128)
                            {
                                // mark it processed right away, to not process it later
                                processed[k] = true;

                                WarTree newTree = createTree(xx, yy, TREE_MAX_WOOD);
                                WarTreeListAdd(&trees, newTree);
                            }
                        }
                    }
                }

                WarEntity* forest = createEntity(context, WAR_ENTITY_TYPE_FOREST, true);
                addSpriteComponent(context, forest, map->sprite);
                addForestComponent(context, forest, trees);

                for (s32 i = 0; i < trees.count; i++)
                {
                    WarTree* tree = &trees.items[i];
                    setStaticEntity(map->finder, tree->tilex, tree->tiley, 1, 1, forest->id);
                }

                determineTreeTiles(context, forest);
            }
        }

        map->forest = createForest(context);;
    }

    // create the starting roads
    {
        WarEntity* road = createRoad(context);

        for(s32 i = 0; i < levelInfo->levelInfo.startRoadsCount; i++)
        {
            WarLevelConstruct *construct = &levelInfo->levelInfo.startRoads[i];
            if (construct->type == WAR_CONSTRUCT_ROAD)
            {
                addRoadPiecesFromConstruct(road, construct);
            }
        }

        determineRoadTypes(context, road);

        map->road = road;
    }

    // create the starting walls
    {
        WarEntity* wall = createWall(context);

        for(s32 i = 0; i < levelInfo->levelInfo.startWallsCount; i++)
        {
            WarLevelConstruct *construct = &levelInfo->levelInfo.startWalls[i];
            if (construct->type == WAR_CONSTRUCT_WALL)
            {
                addWallPiecesFromConstruct(wall, construct);
            }
        }

        determineWallTypes(context, wall);

        for(s32 i = 0; i < wall->wall.pieces.count; i++)
        {
            WarWallPiece* piece = &wall->wall.pieces.items[i];
            piece->hp = WAR_WALL_MAX_HP;
            piece->maxhp = WAR_WALL_MAX_HP;
        }

        addStateMachineComponent(context, wall);

        WarState* idleState = createIdleState(context, wall, false);
        changeNextState(context, wall, idleState, true, true);

        map->wall = wall;
    }

    // create ruins
    {
        map->ruin = createRuins(context);
    }

    // create players info
    {
        for (s32 i = 0; i < MAX_PLAYERS_COUNT; i++)
        {
            WarPlayerInfo* player = &map->players[i];

            player->index = i;
            player->race = levelInfo->levelInfo.races[i];
            player->gold = levelInfo->levelInfo.gold[i];
            player->wood = levelInfo->levelInfo.lumber[i];
            player->godMode = false;

            for (s32 j = 0; j < MAX_FEATURES_COUNT; j++)
            {
                player->features[j] = levelInfo->levelInfo.allowedFeatures[j];
            }

            for (s32 j = 0; j < MAX_UPGRADES_COUNT; j++)
            {
                player->upgrades[j].allowed = levelInfo->levelInfo.allowedUpgrades[j][i];
                player->upgrades[j].level = 0;
            }
        }
    }

    // create the starting entities
    {
        for (s32 i = 0; i < levelInfo->levelInfo.startEntitiesCount; i++)
        {
            WarLevelUnit startUnit = levelInfo->levelInfo.startEntities[i];
            createUnit(context, startUnit.type, startUnit.x, startUnit.y, startUnit.player,
                       startUnit.resourceKind, startUnit.amount, true);
        }
    }

    // init AI
    initAI(context);

    // add ui entities
    createMapUI(context);
    createMenu(context);
    createOptionsMenu(context);
    createObjectivesMenu(context);
    createRestartMenu(context);
    createGameOverMenu(context);
    createQuitMenu(context);
    createDemoEndMenu(context);
    createUICursor(context, "cursor", WAR_CURSOR_ARROW, VEC2_ZERO);

    if (!isDemo(context))
        createAudio(context, WAR_MUSIC_00, true);
}

void leaveMap(WarContext* context)
{
    if (context->map)
    {
        freeMap(context, context->map);
        context->map = NULL;
    }
}

void updateViewport(WarContext *context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    map->wasScrolling = false;

    vec2 dir = VEC2_ZERO;
    bool wasScrolling = map->isScrolling;
    bool mouseScroll = false;
    bool keyScroll = false;

    // if there was a click in the minimap, then update the position of the viewport
    if (isButtonPressed(input, WAR_MOUSE_LEFT))
    {
        // check if the click is inside the minimap panel
        if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
        {
            vec2 minimapSize = vec2i(MINIMAP_WIDTH, MINIMAP_HEIGHT);
            vec2 offset = vec2ScreenToMinimapCoordinates(context, input->pos);
            offset = vec2MinimapToViewportCoordinates(context, offset);

            map->viewport.x = offset.x * MAP_WIDTH / minimapSize.x;
            map->viewport.y = offset.y * MAP_HEIGHT / minimapSize.y;
        }
        // check if it was at the edge of the map to scroll also and update the position of the viewport
        else if(!input->isDragging)
        {
            dir = getDirFromMousePos(context);
            mouseScroll = true;
        }
    }
    // check for the arrows keys and update the position of the viewport
    else
    {
        // don't scroll with arrow keys if Control or Shift are pressed
        // don't scroll with arrow keys if the cheat status is active
        if (!isKeyPressed(input, WAR_KEY_CTRL) &&
            !isKeyPressed(input, WAR_KEY_SHIFT) &&
            !cheatsEnabledAndVisible(map))
        {
            dir = getDirFromArrowKeys(context);
            keyScroll = true;
        }
    }

    map->isScrolling = !vec2IsZero(dir);
    if (map->isScrolling)
    {
        assert(mouseScroll || keyScroll);

        f32 scrollSpeed = 0.0f;
        if (mouseScroll)
            scrollSpeed = getMapScrollSpeed(map->settings.mouseScrollSpeed);
        else if (keyScroll)
            scrollSpeed = getMapScrollSpeed(map->settings.keyScrollSpeed);

        map->viewport.x += scrollSpeed * dir.x * context->deltaTime;
        map->viewport.x = clamp(map->viewport.x, 0.0f, MAP_WIDTH - map->viewport.width);

        map->viewport.y += scrollSpeed * dir.y * context->deltaTime;
        map->viewport.y = clamp(map->viewport.y, 0.0f, MAP_HEIGHT - map->viewport.height);
    }
    else
    {
        map->wasScrolling = wasScrolling;
    }
}

void updateDragRect(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    input->wasDragging = false;
    input->dragRect = RECT_EMPTY;

    if (map->isScrolling)
    {
        input->isDragging = false;
        input->dragPos = VEC2_ZERO;
        return;
    }

    if (isButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
        {
            if (!input->isDragging)
            {
                input->dragPos = input->pos;
                input->isDragging = true;
            }
        }
    }
    else if(wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (input->isDragging)
        {
            input->isDragging = false;
            input->wasDragging = true;
            input->dragRect = rectpf(input->dragPos.x, input->dragPos.y, input->pos.x, input->pos.y);
        }
    }
}

void updateSelection(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if(wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        // if it was scrolling last frame, don't perform any selection this frame
        if (!map->wasScrolling)
        {
            // check if the click is inside the map panel
            if(input->wasDragging || rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
            {
                WarEntityList newSelectedEntities;
                WarEntityListInit(&newSelectedEntities, WarEntityListNonFreeOptions);

                rect pointerRect = rectScreenToMapCoordinates(context, input->dragRect);

                // select the entities inside the dragging rect
                WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
                for(s32 i = 0; i < units->count; i++)
                {
                    WarEntity* entity = units->items[i];
                    if (entity)
                    {
                        WarUnitComponent* unit = &entity->unit;
                        if (unit->enabled)
                        {
                            // don't select dead units or corpses
                            if (isDead(entity) || isGoingToDie(entity) || isCorpseUnit(entity))
                            {
                                continue;
                            }

                            // don't select collased buildings
                            if (isCollapsing(entity) || isGoingToCollapse(entity))
                            {
                                continue;
                            }

                            // don't select workers inside buildings
                            if (isWorkerUnit(entity) && isInsideBuilding(entity))
                            {
                                continue;
                            }

                            // don't select non-visible units
                            if (!isUnitPartiallyVisible(map, entity))
                            {
                                continue;
                            }

                            rect unitRect = getUnitRect(entity);
                            if (rectIntersects(pointerRect, unitRect))
                            {
                                WarEntityListAdd(&newSelectedEntities, entity);
                            }
                        }
                    }
                }

                // include the already selected entities if the Ctrl key is pressed
                if (isKeyPressed(input, WAR_KEY_CTRL))
                {
                    // the max number of selected entities is 4, so there is no much
                    // throuble looking for the actual entities here, it will also
                    // improve when a hash is make for looking up the entities
                    for (s32 i = 0; i < map->selectedEntities.count; i++)
                    {
                        WarEntity* entity = findEntity(context, map->selectedEntities.items[i]);
                        if (entity)
                            WarEntityListAdd(&newSelectedEntities, entity);
                    }
                }

                bool areDudesSelected = false;
                bool areBuildingSelected = false;

                // calculate the number of dudes and buildings in the selection
                for (s32 i = 0; i < newSelectedEntities.count; i++)
                {
                    WarEntity* entity = newSelectedEntities.items[i];
                    if (isDudeUnit(entity))
                        areDudesSelected = true;
                    else if (isBuildingUnit(entity))
                        areBuildingSelected = true;
                }

                if (areDudesSelected)
                {
                    // remove all new selected buildings
                    for (s32 i = newSelectedEntities.count - 1; i >= 0; i--)
                    {
                        WarEntity* entity = newSelectedEntities.items[i];
                        if (isBuildingUnit(entity))
                            WarEntityListRemoveAt(&newSelectedEntities, i);
                    }
                }
                else if (areBuildingSelected)
                {
                    // remove all other new selected buildings
                    WarEntityListRemoveAtRange(&newSelectedEntities, 1, newSelectedEntities.count - 1);
                }

                if (areDudesSelected)
                {
                    if (newSelectedEntities.count == 1)
                    {
                        WarEntity* newSelectedEntity = newSelectedEntities.items[0];
                        if (isFriendlyUnit(context, newSelectedEntity))
                        {
                            playDudeSelectionSound(context, newSelectedEntity);
                        }
                        else
                        {
                            createAudio(context, WAR_UI_CLICK, false);
                        }
                    }
                }
                else if (areBuildingSelected)
                {
                    WarEntity* newSelectedEntity = newSelectedEntities.items[0];
                    if (isFriendlyUnit(context, newSelectedEntity))
                    {
                        playBuildingSelectionSound(context, newSelectedEntity);
                    }
                }

                // if the new selected entity is the same one, don't clear the command, otherwise do
                if (newSelectedEntities.count == 1 && map->selectedEntities.count == 1)
                {
                    WarEntity* newSelectedEntity = newSelectedEntities.items[0];
                    WarEntityId selectedEntityId = map->selectedEntities.items[0];
                    if (selectedEntityId != newSelectedEntity->id)
                    {
                        map->command.type = WAR_COMMAND_NONE;
                    }
                }
                else
                {
                    map->command.type = WAR_COMMAND_NONE;
                }

                // clear the current selection
                clearSelection(context);

                // and add the new selection
                s32 selectedEntitiesCount = min(newSelectedEntities.count, 4);
                for (s32 i = 0; i < selectedEntitiesCount; i++)
                {
                    WarEntity* entity = newSelectedEntities.items[i];
                    addEntityToSelection(context, entity->id);
                }

                WarEntityListFree(&newSelectedEntities);
            }
        }
    }
}

void updateTreesEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (!map->editingTrees)
        return;

    if (wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = vec2ScreenToMapCoordinates(context, input->pos);
            pointerPos =  vec2MapToTileCoordinates(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntityId entityId = getTileEntityId(map->finder, x, y);
            WarEntity* entity = findEntity(context, entityId);
            if (!entity)
            {
                entity = map->forest;

                plantTree(context, entity, x, y);
                determineAllTreeTiles(context);
            }
            else if (entity->type == WAR_ENTITY_TYPE_FOREST)
            {
                WarTree* tree = getTreeAtPosition(entity, x, y);
                if (tree)
                {
                    chopTree(context, entity, tree, TREE_MAX_WOOD);
                    determineAllTreeTiles(context);
                }
            }
        }
    }
}

void updateRoadsEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (!map->editingRoads)
        return;

    if (wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = vec2ScreenToMapCoordinates(context, input->pos);
            pointerPos =  vec2MapToTileCoordinates(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntity* road = map->road;

            WarRoadPiece* piece = getRoadPieceAtPosition(road, x, y);
            if (!piece)
            {
                addRoadPiece(road, x, y, 0);
                determineRoadTypes(context, road);
            }
            else
            {
                removeRoadPiece(road, piece);
                determineRoadTypes(context, road);
            }
        }
    }
}

void updateWallsEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (!map->editingWalls)
        return;

    if (wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = vec2ScreenToMapCoordinates(context, input->pos);
            pointerPos =  vec2MapToTileCoordinates(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntity* wall = map->wall;

            WarWallPiece* piece = getWallPieceAtPosition(wall, x, y);
            if (!piece)
            {
                WarWallPiece* piece = addWallPiece(wall, x, y, 0);
                piece->hp = WAR_WALL_MAX_HP;
                piece->maxhp = WAR_WALL_MAX_HP;

                determineWallTypes(context, wall);
            }
            else
            {
                setFreeTiles(map->finder, piece->tilex, piece->tiley, 1, 1);

                removeWallPiece(wall, piece);
                determineWallTypes(context, wall);
            }
        }
    }
}

void updateRuinsEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (!map->editingRuins)
        return;

    if (wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = vec2ScreenToMapCoordinates(context, input->pos);
            pointerPos =  vec2MapToTileCoordinates(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntity* ruin = map->ruin;

            WarRuinPiece* piece = getRuinPieceAtPosition(ruin, x, y);
            if (!piece)
            {
                addRuinsPieces(context, ruin, x, y, 2);
                determineRuinTypes(context, ruin);
            }
            else
            {
                removeRuinPiece(ruin, piece);
                determineRuinTypes(context, ruin);
            }
        }
    }
}

void updateRainOfFireEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (!map->editingRainOfFire)
        return;

    if (wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
        {
            rect viewport = map->viewport;

            vec2 target = vec2ScreenToMapCoordinates(context, input->pos);
            vec2 origin = vec2f(target.x, viewport.y);

            createProjectile(context, WAR_PROJECTILE_RAIN_OF_FIRE, 0, 0, origin, target);
        }

    }
}

void updateAddUnit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (!map->addingUnit)
        return;

    if (wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = vec2ScreenToMapCoordinates(context, input->pos);
            pointerPos =  vec2MapToTileCoordinates(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntityId entityId = getTileEntityId(map->finder, x, y);
            if (!entityId)
            {
                WarRace addingUnitRace = getUnitTypeRace(map->addingUnitType);
                for (s32 i = 0; i < MAX_PLAYERS_COUNT; i++)
                {
                    if (map->players[i].race == addingUnitRace)
                    {
                        createUnit(context, map->addingUnitType, x, y, map->players[i].index, WAR_RESOURCE_NONE, 0, true);
                        break;
                    }
                }
            }
        }
    }
}

void updateCommandButtons(WarContext* context)
{
    WarMap* map = context->map;

    WarEntity* commandButtons[6] =
    {
        findUIEntity(context, "btnCommand0"),
        findUIEntity(context, "btnCommand1"),
        findUIEntity(context, "btnCommand2"),
        findUIEntity(context, "btnCommand3"),
        findUIEntity(context, "btnCommand4"),
        findUIEntity(context, "btnCommand5")
    };

    WarEntity* commandTexts[4] =
    {
        findUIEntity(context, "txtCommand0"),
        findUIEntity(context, "txtCommand1"),
        findUIEntity(context, "txtCommand2"),
        findUIEntity(context, "txtCommand3")
    };

    for (s32 i = 0; i < arrayLength(commandButtons); i++)
        commandButtons[i]->button.enabled = false;

    for (s32 i = 0; i < arrayLength(commandTexts); i++)
        clearUIText(commandTexts[i]);

    s32 selectedEntitiesCount = map->selectedEntities.count;
    if (selectedEntitiesCount == 0)
        return;

    WarEntity* entity = findEntity(context, map->selectedEntities.items[0]);
    assert(entity && isUnit(entity));

    // if the selected unit is a farm,
    // just show the text about the food consumtion
    if (entity->unit.type == WAR_UNIT_FARM_HUMANS ||
        entity->unit.type == WAR_UNIT_FARM_ORCS)
    {
        if (!entity->unit.building)
        {
            s32 farmsCount = getNumberOfBuildingsOfType(context, 0, entity->unit.type, true);
            s32 dudesCount = getTotalNumberOfDudes(context, 0);

            setUIText(commandTexts[0], "FOOD USAGE:");
            setUITextHighlight(commandTexts[0], NO_HIGHLIGHT, 0);
            setUITextFormat(commandTexts[1], "GROWN %d", farmsCount * 4 + 1);
            setUITextHighlight(commandTexts[1], NO_HIGHLIGHT, 0);
            setUITextFormat(commandTexts[2], " USED %d", dudesCount);
            setUITextHighlight(commandTexts[2], NO_HIGHLIGHT, 0);

            return;
        }
    }

    // if the selected unit is a goldmine,
    // just add the text with the remaining gold
    if (entity->unit.type == WAR_UNIT_GOLDMINE)
    {
        s32 gold = entity->unit.amount;

        setUIText(commandTexts[0], "GOLD LEFT");
        setUITextHighlight(commandTexts[0], NO_HIGHLIGHT, 0);
        setUITextFormat(commandTexts[3], "%d", gold);
        setUITextHighlight(commandTexts[3], NO_HIGHLIGHT, 0);
        return;
    }

    // determine the commands for the selected unit(s)
    WarUnitCommandType commands[6] = {0};
    getUnitCommands(context, entity, commands);

    if (selectedEntitiesCount > 1)
    {
        WarUnitCommandType selectedCommands[6] = {0};
        for (s32 i = 1; i < selectedEntitiesCount; i++)
        {
            WarEntity* selectedEntity = findEntity(context, map->selectedEntities.items[i]);
            assert(selectedEntity && isUnit(selectedEntity));

            memset(selectedCommands, 0, sizeof(selectedCommands));
            getUnitCommands(context, selectedEntity, selectedCommands);

            for (s32 j = 0; j < arrayLength(commands); j++)
            {
                if (commands[j] != selectedCommands[j])
                {
                    commands[j] = WAR_COMMAND_NONE;
                }
            }
        }
    }

    for (s32 i = 0; i < arrayLength(commands); i++)
    {
        if (commands[i] != WAR_COMMAND_NONE)
        {
            WarUnitCommandData commandData = getUnitCommandData(context, entity, commands[i]);
            setUIImage(commandButtons[i], commandData.frameIndex);
            setUITooltip(commandButtons[i], commandData.highlightIndex, commandData.highlightCount, commandData.tooltip);
            commandButtons[i]->button.enabled = true;
            commandButtons[i]->button.gold = commandData.gold;
            commandButtons[i]->button.wood = commandData.wood;
            commandButtons[i]->button.hotKey = commandData.hotKey;
            commandButtons[i]->button.clickHandler = commandData.clickHandler;
        }
    }
}

void updateCommandFromRightClick(WarContext* context)
{
    WarMap* map = context->map;
    WarUnitCommand* command = &map->command;
    WarInput* input = &context->input;

    if (wasButtonPressed(input, WAR_MOUSE_RIGHT))
    {
        if (command->type == WAR_COMMAND_NONE)
        {
            s32 selEntitiesCount = map->selectedEntities.count;
            if (selEntitiesCount > 0)
            {
                // if the right click was on the map
                if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, targetTile.x, targetTile.y);
                    WarEntity* targetEntity = findEntity(context, targetEntityId);
                    if (targetEntity)
                    {
                        if (isUnitOfType(targetEntity, WAR_UNIT_GOLDMINE))
                        {
                            if (!isUnitUnknown(map, targetEntity))
                                executeHarvestCommand(context, targetEntity, targetTile);
                            else
                                executeMoveCommand(context, targetPoint);
                        }
                        else if (isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST))
                        {
                            if (isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y))
                            {
                                executeHarvestCommand(context, targetEntity, targetTile);
                            }
                            else
                            {
                                WarTree* tree = findAccesibleTree(context, targetEntity, targetTile);
                                if (tree)
                                {
                                    targetTile = vec2i(tree->tilex, tree->tiley);
                                    executeHarvestCommand(context, targetEntity, targetTile);
                                }
                                else
                                {
                                    executeMoveCommand(context, targetPoint);
                                }
                            }
                        }
                        else if (isUnitOfType(targetEntity, WAR_UNIT_TOWNHALL_HUMANS) ||
                                 isUnitOfType(targetEntity, WAR_UNIT_TOWNHALL_ORCS))
                        {
                            if (!isUnitUnknown(map, targetEntity))
                            {
                                if (isEnemyUnit(context, targetEntity))
                                {
                                    executeAttackCommand(context, targetEntity, targetTile);
                                }
                                else
                                {
                                    executeDeliverCommand(context, targetEntity);
                                }
                            }
                            else
                            {
                                executeMoveCommand(context, targetPoint);
                            }
                        }
                        else if (isWall(targetEntity))
                        {
                            // it doesn't matter if the wall piece is visible or not,
                            // the unit will walk to it
                            executeMoveCommand(context, targetPoint);
                        }
                        else
                        {
                            if (isEnemyUnit(context, targetEntity))
                            {
                                executeAttackCommand(context, targetEntity, targetTile);
                            }
                            else
                            {
                                executeFollowCommand(context, targetEntity);
                            }
                        }
                    }
                    else
                    {
                        executeMoveCommand(context, targetPoint);
                    }
                }
                // if the right click was on the minimap
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 offset = vec2ScreenToMinimapCoordinates(context, input->pos);
                    vec2 targetPoint = vec2TileToMapCoordinates(offset, true);

                    executeMoveCommand(context, targetPoint);
                }
            }
        }
        else
        {
            cancel(context, NULL);
        }
    }
}

void updateStatus(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarCheatStatus* cheatStatus = &map->cheatStatus;
    WarFlashStatus* flashStatus = &map->flashStatus;

    WarEntity* statusCursor = findUIEntity(context, "txtStatusCursor");
    assert(statusCursor);

    WarEntity* statusTextUI = findUIEntity(context, "txtStatus");
    assert(statusTextUI);

    WarEntity* cheatFeedbackText = findUIEntity(context, "txtCheatFeedbackText");
    assert(cheatFeedbackText);

    if (cheatStatus->enabled)
    {
        if (cheatStatus->feedback)
        {
            setUIEntityStatus(cheatFeedbackText, true);
            setUIText(cheatFeedbackText, cheatStatus->feedbackText);

            cheatStatus->feedbackTime -= context->deltaTime;
            if (cheatStatus->feedbackTime <= 0)
            {
                cheatStatus->feedbackTime = 0;
                cheatStatus->feedback = false;
            }
        }
        else
        {
            setUIEntityStatus(cheatFeedbackText, false);
        }

        if (cheatStatus->visible)
        {
            if (wasKeyPressed(input, WAR_KEY_ESC) ||
                wasKeyPressed(input, WAR_KEY_ENTER))
            {
                if (wasKeyPressed(input, WAR_KEY_ENTER))
                {
                    applyCheat(context, cheatStatus->text);
                }

                setCheatsPanelVisible(context, false);
                return;
            }

            if (wasKeyPressed(input, WAR_KEY_TAB))
            {
                s32 length = strlen(cheatStatus->text);
                if (TAB_WIDTH <= STATUS_TEXT_MAX_LENGTH - length)
                {
                    strInsertAt(cheatStatus->text, cheatStatus->position, '\t');
                    cheatStatus->position++;
                }
            }
            else if (wasKeyPressed(input, WAR_KEY_BACKSPACE))
            {
                if (cheatStatus->position > 0)
                {
                    strRemoveAt(cheatStatus->text, cheatStatus->position - 1);
                    cheatStatus->position--;
                }
            }
            else if (wasKeyPressed(input, WAR_KEY_DELETE))
            {
                s32 length = strlen(cheatStatus->text);
                if (cheatStatus->position < length)
                {
                    strRemoveAt(cheatStatus->text, cheatStatus->position);
                }
            }
            else if (wasKeyPressed(input, WAR_KEY_RIGHT))
            {
                s32 length = strlen(cheatStatus->text);
                if (cheatStatus->position < length)
                {
                    cheatStatus->position++;
                }
            }
            else if (wasKeyPressed(input, WAR_KEY_LEFT))
            {
                if (cheatStatus->position > 0)
                {
                    cheatStatus->position--;
                }
            }
            else if (wasKeyPressed(input, WAR_KEY_HOME))
            {
                cheatStatus->position = 0;
            }
            else if (wasKeyPressed(input, WAR_KEY_END))
            {
                s32 length = strlen(cheatStatus->text);
                cheatStatus->position = length;
            }

            char statusText[STATUS_TEXT_MAX_LENGTH];
            memset(statusText, 0, sizeof(statusText));
            strcpy(statusText, "MSG: ");
            strcpy(statusText + strlen("MSG: "), cheatStatus->text);
            setStatus(context, NO_HIGHLIGHT, 0, 0, 0, statusText);

            NVGfontParams params;
            params.fontSize = statusTextUI->text.fontSize;
            params.fontData = fontsData[statusTextUI->text.fontIndex];

            vec2 prefixSize = nvgMeasureSingleSpriteText("MSG: ", strlen("MSG: "), params);
            vec2 textSize = nvgMeasureSingleSpriteText(cheatStatus->text, cheatStatus->position, params);
            statusCursor->transform.position.x = map->bottomPanel.x + prefixSize.x + textSize.x;

            setUIEntityStatus(statusCursor, true);
            return;
        }
        else
        {
            setUIEntityStatus(statusCursor, false);

            if (wasKeyPressed(input, WAR_KEY_ENTER))
            {
                setCheatsPanelVisible(context, true);
            }
        }
    }

    if (flashStatus->enabled)
    {
        if (flashStatus->startTime + flashStatus->duration >= context->time)
        {
            setStatus(context, NO_HIGHLIGHT, 0, 0, 0, flashStatus->text);
            return;
        }

        // if the time for the flash status is over, just disabled it
        flashStatus->enabled = false;
    }

    char statusText[STATUS_TEXT_MAX_LENGTH] = {0};
    s32 highlightIndex = NO_HIGHLIGHT;
    s32 highlightCount = 0;
    s32 goldCost = 0;
    s32 woodCost = 0;

    if (map->selectedEntities.count > 0)
    {
        for (s32 i = 0; i < map->selectedEntities.count; i++)
        {
            WarEntityId selectedEntityId = map->selectedEntities.items[i];
            WarEntity* selectedEntity = findEntity(context, selectedEntityId);
            assert(selectedEntity);

            if (isBuildingUnit(selectedEntity))
            {
                if (isTraining(selectedEntity) || isGoingToTrain(selectedEntity))
                {
                    WarState* trainState = getTrainState(selectedEntity);
                    WarUnitType unitToBuild = trainState->train.unitToBuild;
                    WarUnitCommandMapping commandMapping = getCommandMappingFromUnitType(unitToBuild);
                    WarUnitCommandBaseData commandData = getCommandBaseData(commandMapping.type);

                    strcpy(statusText, commandData.tooltip2);
                }
                else if (isUpgrading(selectedEntity) || isGoingToUpgrade(selectedEntity))
                {
                    WarState* upgradeState = getUpgradeState(selectedEntity);
                    WarUpgradeType upgradeToBuild = upgradeState->upgrade.upgradeToBuild;
                    WarUnitCommandMapping commandMapping = getCommandMappingFromUpgradeType(upgradeToBuild);
                    WarUnitCommandBaseData commandData = getCommandBaseData(commandMapping.type);

                    strcpy(statusText, commandData.tooltip2);
                }
                else
                {
                    s32 hp = selectedEntity->unit.hp;
                    s32 maxhp = selectedEntity->unit.maxhp;
                    if (hp < maxhp)
                    {
                        // to calculate the amount of wood and gold needed to repair a
                        // building I'm taking the 12% of the damage of the building,
                        // so for the a FARM if it has a damage of 200, the amount of
                        // wood and gold would be 200 * 0.12 = 24.
                        //
                        s32 repairCost = (s32)ceil((maxhp - hp) * 0.12f);
                        sprintf(statusText, "FULL REPAIRS WILL COST %d GOLD & LUMBER", repairCost);
                    }
                }
            }
            else if (isWorkerUnit(selectedEntity))
            {
                if (isCarryingResources(selectedEntity))
                {
                    if (selectedEntity->unit.resourceKind == WAR_RESOURCE_GOLD)
                    {
                        strcpy(statusText, "CARRYING GOLD");
                    }
                    else if (selectedEntity->unit.resourceKind == WAR_RESOURCE_WOOD)
                    {
                        strcpy(statusText, "CARRYING LUMBER");
                    }
                }
            }
        }
    }

    WarEntityList* buttons = getEntitiesOfType(context, WAR_ENTITY_TYPE_BUTTON);
    for(s32 i = 0; i < buttons->count; i++)
    {
        WarEntity* entity = buttons->items[i];
        if (entity)
        {
            WarUIComponent* ui = &entity->ui;
            if (ui->enabled)
            {
                WarButtonComponent* button = &entity->button;
                if (button->enabled && button->interactive && button->hot)
                {
                    strcpy(statusText, button->tooltip);
                    goldCost = button->gold;
                    woodCost = button->wood;
                    highlightIndex = button->highlightIndex;
                    highlightCount = button->highlightCount;
                    break;
                }
            }
        }
    }

    setStatus(context, highlightIndex, highlightCount, goldCost, woodCost, statusText);
}

void updateMapCursor(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    WarEntity* entity = findUIEntity(context, "cursor");
    if (entity)
    {
        entity->transform.position = vec2Subv(input->pos, entity->cursor.hot);

        if (!map->playing)
        {
            changeCursorType(context, entity, WAR_CURSOR_ARROW);
            return;
        }

        if (input->isDragging)
        {
            changeCursorType(context, entity, WAR_CURSOR_GREEN_CROSSHAIR);
            return;
        }

        if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
        {
            WarUnitCommand* command = &map->command;
            switch (command->type)
            {
                case WAR_COMMAND_ATTACK:
                case WAR_COMMAND_SPELL_RAIN_OF_FIRE:
                case WAR_COMMAND_SPELL_POISON_CLOUD:
                {
                    changeCursorType(context, entity, WAR_CURSOR_RED_CROSSHAIR);
                    break;
                }

                case WAR_COMMAND_MOVE:
                case WAR_COMMAND_STOP:
                case WAR_COMMAND_HARVEST:
                case WAR_COMMAND_DELIVER:
                case WAR_COMMAND_REPAIR:
                case WAR_COMMAND_SPELL_HEALING:
                case WAR_COMMAND_SPELL_FAR_SIGHT:
                case WAR_COMMAND_SPELL_INVISIBILITY:
                case WAR_COMMAND_SPELL_RAISE_DEAD:
                case WAR_COMMAND_SPELL_DARK_VISION:
                case WAR_COMMAND_SPELL_UNHOLY_ARMOR:
                {
                    changeCursorType(context, entity, WAR_CURSOR_YELLOW_CROSSHAIR);
                    break;
                }

                case WAR_COMMAND_BUILD_FARM_HUMANS:
                case WAR_COMMAND_BUILD_FARM_ORCS:
                case WAR_COMMAND_BUILD_BARRACKS_HUMANS:
                case WAR_COMMAND_BUILD_BARRACKS_ORCS:
                case WAR_COMMAND_BUILD_CHURCH:
                case WAR_COMMAND_BUILD_TEMPLE:
                case WAR_COMMAND_BUILD_TOWER_HUMANS:
                case WAR_COMMAND_BUILD_TOWER_ORCS:
                case WAR_COMMAND_BUILD_TOWNHALL_HUMANS:
                case WAR_COMMAND_BUILD_TOWNHALL_ORCS:
                case WAR_COMMAND_BUILD_LUMBERMILL_HUMANS:
                case WAR_COMMAND_BUILD_LUMBERMILL_ORCS:
                case WAR_COMMAND_BUILD_STABLE:
                case WAR_COMMAND_BUILD_KENNEL:
                case WAR_COMMAND_BUILD_BLACKSMITH_HUMANS:
                case WAR_COMMAND_BUILD_BLACKSMITH_ORCS:
                case WAR_COMMAND_BUILD_ROAD:
                case WAR_COMMAND_BUILD_WALL:
                {
                    changeCursorType(context, entity, WAR_CURSOR_ARROW);
                    break;
                }

                default:
                {
                    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

                    WarEntity* entityUnderCursor = findEntityUnderCursor(context, true, true);
                    if (!entityUnderCursor)
                    {
                        changeCursorType(context, entity, WAR_CURSOR_ARROW);
                        break;
                    }

                    WarEntityIdList* selectedEntities = &map->selectedEntities;
                    if (selectedEntities->count > 0)
                    {
                        WarEntity* selectedEntity = findEntity(context, selectedEntities->items[0]);
                        if (selectedEntity &&
                            isFriendlyUnit(context, selectedEntity) &&
                            isDudeUnit(selectedEntity))
                        {
                            if (isUnitOfType(entityUnderCursor, WAR_UNIT_GOLDMINE) &&
                                !isUnitUnknown(map, entityUnderCursor) &&
                                isWorkerUnit(selectedEntity))
                            {
                                changeCursorType(context, entity, WAR_CURSOR_YELLOW_CROSSHAIR);
                            }
                            else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_FOREST) &&
                                     !isTileUnkown(map, (s32)targetTile.x, (s32)targetTile.y) &&
                                     isWorkerUnit(selectedEntity))
                            {
                                changeCursorType(context, entity, WAR_CURSOR_YELLOW_CROSSHAIR);
                            }
                            else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_WALL) &&
                                     !isTileUnkown(map, (s32)targetTile.x, (s32)targetTile.y) &&
                                     isWarriorUnit(selectedEntity) &&
                                     canAttack(context, selectedEntity, entityUnderCursor))
                            {
                                changeCursorType(context, entity, WAR_CURSOR_RED_CROSSHAIR);
                            }
                            else if (!isFriendlyUnit(context, entityUnderCursor) &&
                                     isWarriorUnit(selectedEntity) &&
                                     canAttack(context, selectedEntity, entityUnderCursor))
                            {
                                changeCursorType(context, entity, WAR_CURSOR_RED_CROSSHAIR);
                            }
                            else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_FOREST) ||
                                     isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_WALL))
                            {
                                changeCursorType(context, entity, WAR_CURSOR_ARROW);
                            }
                            else
                            {
                                changeCursorType(context, entity, WAR_CURSOR_MAGNIFYING_GLASS);
                            }
                        }
                        else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_FOREST) ||
                                 isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_WALL))
                        {
                            changeCursorType(context, entity, WAR_CURSOR_ARROW);
                        }
                        else
                        {
                            changeCursorType(context, entity, WAR_CURSOR_MAGNIFYING_GLASS);
                        }
                    }
                    else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_FOREST) ||
                             isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_WALL))
                    {
                        changeCursorType(context, entity, WAR_CURSOR_ARROW);
                    }
                    else
                    {
                        changeCursorType(context, entity, WAR_CURSOR_MAGNIFYING_GLASS);
                    }

                    break;
                }
            }
        }
        else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
        {
            WarUnitCommand* command = &map->command;
            switch (command->type)
            {
                case WAR_COMMAND_ATTACK:
                case WAR_COMMAND_SPELL_RAIN_OF_FIRE:
                case WAR_COMMAND_SPELL_POISON_CLOUD:
                {
                    changeCursorType(context, entity, WAR_CURSOR_RED_CROSSHAIR);
                    break;
                }

                case WAR_COMMAND_MOVE:
                case WAR_COMMAND_SPELL_FAR_SIGHT:
                case WAR_COMMAND_SPELL_DARK_VISION:
                {
                    changeCursorType(context, entity, WAR_CURSOR_YELLOW_CROSSHAIR);
                    break;
                }

                default:
                {
                    changeCursorType(context, entity, WAR_CURSOR_ARROW);
                    break;
                }
            }
        }
        else
        {
            vec2 dir = getDirFromMousePos(context, input);
            if (dir.x < 0 && dir.y < 0)         // -1, -1
                changeCursorType(context, entity, WAR_CURSOR_ARROW_UP_LEFT);
            else if (dir.x < 0 && dir.y > 0)    // -1,  1
                changeCursorType(context, entity, WAR_CURSOR_ARROW_BOTTOM_LEFT);
            else if (dir.x > 0 && dir.y < 0)    //  1, -1
                changeCursorType(context, entity, WAR_CURSOR_ARROW_UP_RIGHT);
            else if (dir.x > 0 && dir.y > 0)    //  1,  1
                changeCursorType(context, entity, WAR_CURSOR_ARROW_BOTTOM_RIGHT);
            else if (dir.x < 0)                 // -1,  0
                changeCursorType(context, entity, WAR_CURSOR_ARROW_LEFT);
            else if (dir.x > 0)                 //  1,  0
                changeCursorType(context, entity, WAR_CURSOR_ARROW_RIGHT);
            else if (dir.y < 0)                 //  0, -1
                changeCursorType(context, entity, WAR_CURSOR_ARROW_UP);
            else if (dir.y > 0)                 //  0,  1
                changeCursorType(context, entity, WAR_CURSOR_ARROW_BOTTOM);
            else                                //  0,  0
                changeCursorType(context, entity, WAR_CURSOR_ARROW);
        }
    }
}

void updateStateMachines(WarContext* context)
{
    WarEntityList* entities = getEntities(context);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity* entity = entities->items[i];
        if (entity)
        {
            updateStateMachine(context, entity);
        }
    }
}

void updateActions(WarContext* context)
{
    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            updateAction(context, entity);
        }
    }
}

void updateProjectiles(WarContext* context)
{
    WarEntityList* projectiles = getEntitiesOfType(context, WAR_ENTITY_TYPE_PROJECTILE);
    for (s32 i = 0; i < projectiles->count; i++)
    {
        WarEntity* entity = projectiles->items[i];
        if (entity)
        {
            updateProjectile(context, entity);
        }
    }
}

void updateMagic(WarContext* context)
{
    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity && isMagicUnit(entity))
        {
            if (isDead(entity) || isGoingToDie(entity))
                continue;

            WarUnitComponent* unit = &entity->unit;

            if (unit->manaTime <= 0)
            {
                if (isSummonUnit(entity))
                {
                    unit->mana = max(unit->mana - 1, 0);

                    // when the mana runs out the summoned units will die
                    if (unit->mana == 0)
                    {
                        vec2 position = getUnitCenterPosition(entity, false);

                        WarState* deathState = createDeathState(context, entity);
                        changeNextState(context, entity, deathState, true, true);

                        if (entity->unit.type == WAR_UNIT_SCORPION ||
                            entity->unit.type == WAR_UNIT_SPIDER)
                        {
                            createAudioWithPosition(context, WAR_DEAD_SPIDER_SCORPION, position, false);
                        }
                    }
                }
                else
                {
                    // the magic units have a mana regeneration rate of roughly 1 point/sec
                    // so a magic unit will spend almost 4 minutes to fill its mana when its rans out
                    unit->mana = min(unit->mana + 1, unit->maxMana);
                }

                unit->manaTime = 1.0f;
            }
            else
            {
                unit->manaTime -= getMapScaledSpeed(context, context->deltaTime);
            }
        }
    }
}

bool updatePoisonCloud(WarContext* context, WarEntity* entity)
{
    WarPoisonCloudComponent* poisonCloud = &entity->poisonCloud;

    poisonCloud->time -= getMapScaledSpeed(context, context->deltaTime);
    poisonCloud->damageTime -= getMapScaledSpeed(context, context->deltaTime);

    if (poisonCloud->damageTime <= 0)
    {
        WarEntityList* nearUnits = getNearUnits(context, poisonCloud->position, 2);
        for (s32 i = 0; i < nearUnits->count; i++)
        {
            WarEntity* targetEntity = nearUnits->items[i];
            if (targetEntity &&
                !isDead(targetEntity) && !isGoingToDie(targetEntity) &&
                !isCollapsing(targetEntity) && !isGoingToCollapse(targetEntity))
            {
                takeDamage(context, targetEntity, 0, POISON_CLOUD_DAMAGE);
            }
        }
        WarEntityListFree(nearUnits);

        poisonCloud->damageTime = 1.0f;
    }

    return poisonCloud->time <= 0;
}

bool updateSight(WarContext* context, WarEntity* entity)
{
    WarSightComponent* sight = &entity->sight;
    sight->time -= getMapScaledSpeed(context, context->deltaTime);
    return sight->time <= 0;
}

void updateSpells(WarContext* context)
{
    WarEntityIdList spellsToRemove;
    WarEntityIdListInit(&spellsToRemove, WarEntityIdListDefaultOptions);

    WarEntityList* poisonCloudSpells = getEntitiesOfType(context, WAR_ENTITY_TYPE_POISON_CLOUD);
    for (s32 i = 0; i < poisonCloudSpells->count; i++)
    {
        WarEntity* entity = poisonCloudSpells->items[i];
        if (entity)
        {
            if (updatePoisonCloud(context, entity))
            {
                WarEntityIdListAdd(&spellsToRemove, entity->id);
            }
        }
    }

    WarEntityList* sightSpells = getEntitiesOfType(context, WAR_ENTITY_TYPE_SIGHT);
    for (s32 i = 0; i < sightSpells->count; i++)
    {
        WarEntity* entity = sightSpells->items[i];
        if (entity)
        {
            if (updateSight(context, entity))
            {
                WarEntityIdListAdd(&spellsToRemove, entity->id);
            }
        }
    }

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            WarUnitComponent* unit = &entity->unit;

            if (unit->invisible)
            {
                unit->invisibilityTime -= getMapScaledSpeed(context, context->deltaTime);
                if (unit->invisibilityTime <= 0)
                {
                    unit->invisible = false;
                    unit->invisibilityTime = 0;
                }
            }

            if (unit->invulnerable)
            {
                unit->invulnerabilityTime -= getMapScaledSpeed(context, context->deltaTime);
                if (unit->invulnerabilityTime <= 0)
                {
                    unit->invulnerable = false;
                    unit->invulnerabilityTime = 0;
                }
            }
        }
    }

    for (s32 i = 0; i < spellsToRemove.count; i++)
    {
        removeEntityById(context, spellsToRemove.items[i]);
    }

    WarEntityIdListFree(&spellsToRemove);
}

void updateFoW(WarContext* context)
{
    WarMap* map = context->map;

    for (s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        WarMapTile* tile = &map->tiles[i];

        tile->type = WAR_FOG_PIECE_NONE;
        tile->boundary = WAR_FOG_BOUNDARY_NONE;
        if (tile->state == MAP_TILE_STATE_VISIBLE)
            tile->state = MAP_TILE_STATE_FOG;
    }

    // the Holy Sight and Dark Vision spells are the first entities that change FoW
    WarEntityList* sightSpells = getEntitiesOfType(context, WAR_ENTITY_TYPE_SIGHT);
    for (s32 i = 0; i < sightSpells->count; i++)
    {
        WarEntity* entity = sightSpells->items[i];
        if (entity)
        {
            WarSightComponent* sight = &entity->sight;

            rect r = rectExpand(rectv(sight->position, VEC2_ONE), 3, 3);
            setMapTileState(map, r.x, r.y, r.width, r.height, MAP_TILE_STATE_VISIBLE);
        }
    }

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);

    // do the update of the FoW for friendly units first
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            if (isFriendlyUnit(context, entity))
            {
                WarUnitComponent* unit = &entity->unit;
                vec2 position = getUnitCenterPosition(entity, true);
                s32 sightRange = getUnitSightRange(entity);

                if (isBuildingUnit(entity))
                {
                    // the friendly buildings are always seen by the player
                    unit->hasBeenSeen = true;
                }

                // mark the tiles of the unit as visible
                setUnitMapTileState(map, entity, MAP_TILE_STATE_VISIBLE);

                // reveal the attack target of the unit
                WarEntity* targetEntity = getAttackTarget(context, entity);
                if (targetEntity)
                {
                    WarUnitStats stats = getUnitStats(unit->type);

                    if (isUnit(targetEntity))
                    {
                        if (unitInRange(entity, targetEntity, stats.range))
                        {
                            setUnitMapTileState(map, targetEntity, MAP_TILE_STATE_VISIBLE);
                        }
                    }
                    else if (isWall(targetEntity))
                    {
                        WarState* attackState = getAttackState(entity);
                        vec2 targetTile = attackState->attack.targetTile;

                        if (tileInRange(entity, targetTile, stats.range))
                        {
                            WarWallPiece* piece = getWallPieceAtPosition(targetEntity, targetTile.x, targetTile.y);
                            if (piece)
                            {
                                setMapTileState(map, targetTile.x, targetTile.y, 1, 1, MAP_TILE_STATE_VISIBLE);
                            }
                        }
                    }
                }

                // reveal the attacker
                WarEntity* attacker = getAttacker(context, entity);
                if (attacker)
                {
                    // if the attacker is the same the unit is attacking to
                    // don't change tile state because already happened above
                    if (!targetEntity || attacker->id != targetEntity->id)
                    {
                        setUnitMapTileState(map, attacker, MAP_TILE_STATE_VISIBLE);
                    }
                }

                // check near non-friendly building units to mark it as seen
                WarEntityList* nearUnits = getNearUnits(context, position, sightRange);
                for (s32 k = 0; k < nearUnits->count; k++)
                {
                    WarEntity* targetEntity = nearUnits->items[k];
                    if (targetEntity && !isFriendlyUnit(context, targetEntity) && isBuildingUnit(targetEntity))
                    {
                        targetEntity->unit.hasBeenSeen = true;
                    }
                }
                WarEntityListFree(nearUnits);
            }
        }
    }

    // and then do the update of the FoW for enemies and neutrals units
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            if (!isFriendlyUnit(context, entity))
            {
                if (!isUnitPartiallyVisible(map, entity))
                {
                    // remove from selection enemy or neutral units that goes into fog
                    removeEntityFromSelection(context, entity->id);
                }
            }
        }
    }
}

void determineFoWTypes(WarContext* context)
{
    WarMap* map = context->map;

    if (!map->fowEnabled)
        return;

    const s32 dirC = 8;
    const s32 dirX[] = { -1,  0,  1, 1, 1, 0, -1, -1 };
    const s32 dirY[] = { -1, -1, -1, 0, 1, 1,  1,  0 };

    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            WarMapTile* tile = getMapTileState(map, x, y);
            if (tile->state == MAP_TILE_STATE_VISIBLE)
            {
                s32 index = 0;
                s32 unkownCount = 0;
                s32 fogCount = 0;

                for (s32 d = 0; d < dirC; d++)
                {
                    s32 xx = x + dirX[d];
                    s32 yy = y + dirY[d];

                    if (inRange(xx, 0, MAP_TILES_WIDTH) &&
                        inRange(yy, 0, MAP_TILES_HEIGHT))
                    {
                        WarMapTile* neighborTile = getMapTileState(map, xx, yy);
                        if (neighborTile->state == MAP_TILE_STATE_VISIBLE)
                            index = index | (1 << d);
                        else if (neighborTile->state == MAP_TILE_STATE_FOG)
                            fogCount++;
                        else
                            unkownCount++;
                    }
                }

                if (index != 0xFF)
                {
                    tile->type = fogTileTypeMap[index];
                }

                if (fogCount > 0)
                    tile->boundary = WAR_FOG_BOUNDARY_FOG;
                else if (unkownCount > 0)
                    tile->boundary = WAR_FOG_BOUNDARY_UNKOWN;
            }
            else if (tile->state == MAP_TILE_STATE_FOG)
            {
                s32 index = 0;
                s32 unkownCount = 0;

                for (s32 d = 0; d < dirC; d++)
                {
                    s32 xx = x + dirX[d];
                    s32 yy = y + dirY[d];

                    if (inRange(xx, 0, MAP_TILES_WIDTH) &&
                        inRange(yy, 0, MAP_TILES_HEIGHT))
                    {
                        WarMapTile* neighborTile = getMapTileState(map, xx, yy);
                        if (neighborTile->state == MAP_TILE_STATE_VISIBLE ||
                            neighborTile->state == MAP_TILE_STATE_FOG)
                        {
                            index = index | (1 << d);
                        }
                        else
                        {
                            unkownCount++;
                        }
                    }
                }

                if (index != 0xFF)
                {
                    tile->type = fogTileTypeMap[index];
                }

                if (unkownCount > 0)
                {
                    tile->boundary = WAR_FOG_BOUNDARY_UNKOWN;
                }
            }
            else
            {
                s32 index = 0;

                for (s32 d = 0; d < dirC; d++)
                {
                    s32 xx = x + dirX[d];
                    s32 yy = y + dirY[d];

                    if (inRange(xx, 0, MAP_TILES_WIDTH) &&
                        inRange(yy, 0, MAP_TILES_HEIGHT))
                    {
                        WarMapTile* neighborTile = getMapTileState(map, xx, yy);
                        if (neighborTile->state == MAP_TILE_STATE_VISIBLE ||
                            neighborTile->state == MAP_TILE_STATE_FOG)
                        {
                            index = index | (1 << d);
                        }
                    }
                }

                if (index == 0xFF)
                {
                    tile->type = fogTileTypeMap[index];
                }
            }
        }
    }
}

WarCampaignMapType getCampaignMapTypeByLevelInfoIndex(s32 levelInfoIndex)
{
    return levelInfoIndex >= WAR_CAMPAIGN_HUMANS_01 && levelInfoIndex <= WAR_CAMPAIGN_ORCS_12
        ? (WarCampaignMapType)levelInfoIndex
        : WAR_CAMPAIGN_CUSTOM;
}

WarLevelResult checkObjectives(WarContext* context)
{
    WarMap* map = context->map;

    map->objectivesTime -= context->deltaTime;

    if (map->objectivesTime <= 0)
    {
        WarCampaignMapData data = getCampaignData(getCampaignMapTypeByLevelInfoIndex(map->levelInfoIndex));
        if (data.checkObjectivesFunc)
        {
            return data.checkObjectivesFunc(context);
        }

        map->objectivesTime = 1;
    }

    return WAR_LEVEL_RESULT_NONE;
}

void updateObjectives(WarContext* context)
{
    WarMap* map = context->map;

    if (map->result == WAR_LEVEL_RESULT_NONE)
    {
        map->result = checkObjectives(context);
        return;
    }

    map->playing = false;

    bool isLastLevel = map->levelInfoIndex == WAR_CAMPAIGN_HUMANS_02 ||
                        map->levelInfoIndex == WAR_CAMPAIGN_ORCS_02;

    if (map->result == WAR_LEVEL_RESULT_WIN && isLastLevel)
    {
        showDemoEndMenu(context, true);
    }
    else
    {
        showOrHideGameOverMenu(context, true);
    }
}

void updateMap(WarContext* context)
{
    WarMap* map = context->map;

    updateViewport(context);
    updateDragRect(context);

    if (!map->playing)
    {
        updateUIButtons(context, true);
        updateMapCursor(context);
        return;
    }

    if (!executeCommand(context))
    {
        // only update the selection if the current command doesn't get
        // executed or there is no command at all.
        // the reason is because some commands are executed by the left click
        // as well as the selection, and if a command get executed the current
        // selection shouldn't be lost
        updateSelection(context);
    }

    updateAI(context);

    updateStateMachines(context);
    updateActions(context);
    updateAnimations(context);
    updateProjectiles(context);
    updateMagic(context);
    updateSpells(context);

    updateFoW(context);
    determineFoWTypes(context);

    updateGoldText(context);
    updateWoodText(context);
    updateSelectedUnitsInfo(context);
    updateCommandButtons(context);

    updateUIButtons(context, !cheatsEnabledAndVisible(map));

    updateCommandFromRightClick(context);
    updateStatus(context);

    updateMapCursor(context);

    updateTreesEdit(context);
    updateRoadsEdit(context);
    updateWallsEdit(context);
    updateRuinsEdit(context);
    updateRainOfFireEdit(context);
    updateAddUnit(context);

    updateObjectives(context);
}

void renderTerrain(WarContext* context)
{
    WarMap *map = context->map;

    NVGcontext* gfx = context->gfx;

    WarResource* levelInfo = getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource* levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
    assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

    NVGimageBatch* batch = nvgBeginImageBatch(gfx, map->sprite.image, MAP_TILES_WIDTH * MAP_TILES_HEIGHT);

    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            WarMapTile* tile = getMapTileState(map, x, y);
            if (!map->fowEnabled ||
                tile->state == MAP_TILE_STATE_VISIBLE ||
                tile->state == MAP_TILE_STATE_FOG)
            {
                // index of the tile in the tilesheet
                u16 tileIndex = levelVisual->levelVisual.data[y * MAP_TILES_WIDTH + x];

                // coordinates in pixels of the terrain tile
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                nvgSave(gfx);
                nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

                nvgRestore(gfx);
            }
        }
    }

    nvgEndImageBatch(gfx, batch);
}

void renderFoW(WarContext* context)
{
    WarMap* map = context->map;

    if (!map->fowEnabled)
        return;

    NVGcontext* gfx = context->gfx;

    NVGimageBatch* unkownBatch = nvgBeginImageBatch(gfx, map->blackSprite.image, MAP_TILES_WIDTH * MAP_TILES_HEIGHT);
    NVGimageBatch* fogBatch = nvgBeginImageBatch(gfx, map->blackSprite.image, MAP_TILES_WIDTH * MAP_TILES_HEIGHT);
    NVGimageBatch* unkownBoundaryBatch = nvgBeginImageBatch(gfx, map->sprite.image, MAP_TILES_WIDTH * MAP_TILES_HEIGHT);
    NVGimageBatch* fogBoundaryBatch = nvgBeginImageBatch(gfx, map->sprite.image, MAP_TILES_WIDTH * MAP_TILES_HEIGHT);

    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            WarMapTile* tile = getMapTileState(map, x, y);
            if (tile->type != WAR_FOG_PIECE_NONE)
            {
                s32 tileIndex = (s32)tile->type;

                // coordinates in pixels of the terrain tile
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = (tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT;

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);

                if (tile->state == MAP_TILE_STATE_VISIBLE && tile->boundary == WAR_FOG_BOUNDARY_FOG)
                {
                    nvgRenderBatchImage(gfx, fogBoundaryBatch, rs, rd, VEC2_ONE);
                }
                else if (tile->boundary == WAR_FOG_BOUNDARY_UNKOWN)
                {
                    nvgRenderBatchImage(gfx, unkownBoundaryBatch, rs, rd, VEC2_ONE);
                }
            }

            if (tile->state == MAP_TILE_STATE_UNKOWN)
            {
                rect rs = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRenderBatchImage(gfx, unkownBatch, rs, rd, VEC2_ONE);
            }
            else if (tile->state == MAP_TILE_STATE_FOG)
            {
                rect rs = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRenderBatchImage(gfx, fogBatch, rs, rd, VEC2_ONE);
            }
        }
    }

    nvgEndImageBatch(gfx, unkownBoundaryBatch);
    nvgEndImageBatch(gfx, unkownBatch);

    nvgSave(gfx);
    nvgGlobalAlpha(gfx, 0.5f);
    nvgEndImageBatch(gfx, fogBoundaryBatch);
    nvgEndImageBatch(gfx, fogBatch);
    nvgRestore(gfx);
}

void renderUnitPaths(WarContext* context)
{
    NVGcontext* gfx = context->gfx;

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity *entity = units->items[i];
        if (entity)
        {
            WarState* moveState = getDirectState(entity, WAR_STATE_MOVE);
            if (moveState)
            {
                vec2List positions = moveState->move.positions;
                for(s32 k = moveState->move.positionIndex; k < positions.count; k++)
                {
                    vec2 pos = vec2TileToMapCoordinates(positions.items[k], true);
                    pos = vec2Subv(pos, vec2i(2, 2));
                    nvgFillRect(gfx, rectv(pos, vec2i(4, 4)), getColorFromList(entity->id));
                }

                s32 index = moveState->move.pathNodeIndex;
                WarMapPath path = moveState->move.path;

                if (index >= 0)
                {
                    vec2 prevPos;
                    for(s32 k = 0; k < path.nodes.count; k++)
                    {
                        vec2 pos = vec2TileToMapCoordinates(path.nodes.items[k], true);

                        if (k > 0)
                            nvgStrokeLine(gfx, prevPos, pos, getColorFromList(entity->id), 0.5f);

                        nvgFillRect(gfx, rectv(pos, VEC2_ONE), k == index ? nvgRGB(255, 0, 255) : nvgRGB(255, 255, 0));

                        prevPos = pos;
                    }
                }
            }
        }
    }
}

void renderPassableInfo(WarContext* context)
{
    WarMap *map = context->map;

    NVGcontext* gfx = context->gfx;

    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            if (isStatic(map->finder, x, y))
            {
                vec2 pos = vec2i(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);
                vec2 size = vec2i(MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgFillRect(gfx, rectv(pos, size), nvgRGBA(255, 0, 0, 100));
            }
            else if(isDynamic(map->finder, x, y))
            {
                vec2 pos = vec2i(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);
                vec2 size = vec2i(MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgFillRect(gfx, rectv(pos, size), nvgRGBA(255, 150, 100, 100));
            }
        }
    }
}

void renderMapGrid(WarContext* context)
{
    NVGcontext* gfx = context->gfx;

    for(s32 x = 1; x < MAP_TILES_WIDTH; x++)
    {
        vec2 p1 = vec2i(x * MEGA_TILE_WIDTH, 0);
        vec2 p2 = vec2i(x * MEGA_TILE_WIDTH, MAP_TILES_HEIGHT * MEGA_TILE_HEIGHT);
        nvgStrokeLine(gfx, p1, p2, NVG_WHITE, 0.25f);
    }

    for(s32 y = 1; y < MAP_TILES_HEIGHT; y++)
    {
        vec2 p1 = vec2i(0, y * MEGA_TILE_HEIGHT);
        vec2 p2 = vec2i(MAP_TILES_WIDTH * MAP_TILES_WIDTH, y * MEGA_TILE_HEIGHT);
        nvgStrokeLine(gfx, p1, p2, NVG_WHITE, 0.25f);
    }
}

void renderMapPanel(WarContext *context)
{
    WarMap *map = context->map;

    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    nvgTranslate(gfx, map->mapPanel.x, map->mapPanel.y);
    nvgTranslate(gfx, -map->viewport.x, -map->viewport.y);

    renderTerrain(context);
    renderEntitiesOfType(context, WAR_ENTITY_TYPE_RUIN);
    renderEntitiesOfType(context, WAR_ENTITY_TYPE_ROAD);
    renderEntitiesOfType(context, WAR_ENTITY_TYPE_WALL);
    renderEntitiesOfType(context, WAR_ENTITY_TYPE_FOREST);

#ifdef DEBUG_RENDER_UNIT_PATHS
    renderUnitPaths(context);
#endif

#ifdef DEBUG_RENDER_PASSABLE_INFO
    renderPassableInfo(context);
#endif

#ifdef DEBUG_RENDER_MAP_GRID
    renderMapGrid(context);
#endif

    renderEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    renderUnitSelection(context);
    renderEntitiesOfType(context, WAR_ENTITY_TYPE_PROJECTILE);
    renderEntitiesOfType(context, WAR_ENTITY_TYPE_POISON_CLOUD);
    renderEntitiesOfType(context, WAR_ENTITY_TYPE_ANIMATION);
    renderFoW(context);

    nvgRestore(gfx);
}

void renderMap(WarContext *context)
{
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);
    nvgScale(gfx, context->globalScale, context->globalScale);

    // render map
    renderMapPanel(context);

    // render ui
    renderMapUI(context);

    nvgRestore(gfx);
}