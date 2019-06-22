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

void updateMinimapTile(WarSpriteFrame* minimapFrame, WarResource* levelVisual, WarResource* tileset, s32 x, s32 y)
{
    s32 index = y * MAP_TILES_WIDTH + x;
    u8Color color = getMapTileAverage(levelVisual, tileset, x, y);
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

    updateMinimapTile(&map->minimapSprite.frames[1], levelVisual, tileset, x, y);
}

void createMap(WarContext *context, s32 levelInfoIndex)
{
    WarResource* levelInfo = getOrCreateResource(context, levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource* levelPassable = getOrCreateResource(context, levelInfo->levelInfo.passableIndex);
    assert(levelPassable && levelPassable->type == WAR_RESOURCE_TYPE_LEVEL_PASSABLE);

    WarMap *map = (WarMap*)xcalloc(1, sizeof(WarMap));
    map->levelInfoIndex = levelInfoIndex;
    map->tilesetType = MAP_TILESET_FOREST;
    map->scrollSpeed = 200;

    map->leftTopPanel = recti(0, 0, 72, 72);
    map->leftBottomPanel = recti(0, 72, 72, 128);
    map->rightPanel = recti(312, 0, 8, 200);
    map->topPanel = recti(72, 0, 240, 12);
    map->bottomPanel = recti(72, 188, 240, 12);
    map->mapPanel = recti(72, 12, MAP_VIEWPORT_WIDTH, MAP_VIEWPORT_HEIGHT);
    map->minimapPanel = recti(3, 6, MINIMAP_WIDTH, MINIMAP_HEIGHT);

    s32 startX = levelInfo->levelInfo.startX * MEGA_TILE_WIDTH;
    s32 startY = levelInfo->levelInfo.startY * MEGA_TILE_HEIGHT;
    map->viewport = recti(startX, startY, MAP_VIEWPORT_WIDTH, MAP_VIEWPORT_HEIGHT);

    // initialize entities list
    WarEntityListInit(&map->entities, WarEntityListDefaultOptions);

    // initialize entity by type map
    WarEntityMapOptions entitiesByTypeOptions = (WarEntityMapOptions){0};
    entitiesByTypeOptions.defaultValue = NULL;
    entitiesByTypeOptions.hashFn = hashEntityType;
    entitiesByTypeOptions.equalsFn = equalsEntityType;
    entitiesByTypeOptions.freeFn = freeEntityList;
    WarEntityMapInit(&map->entitiesByType, entitiesByTypeOptions);
    for (WarEntityType type = WAR_ENTITY_TYPE_IMAGE; type < WAR_ENTITY_TYPE_COUNT; type++)
    {
        WarEntityList* list = (WarEntityList*)xmalloc(sizeof(WarEntityList));
        WarEntityListInit(list, WarEntityListNonFreeOptions);
        WarEntityMapSet(&map->entitiesByType, type, list);
    }

    // initialize unit by type map
    WarUnitMapOptions unitsByTypeOptions = (WarUnitMapOptions){0};
    unitsByTypeOptions.defaultValue = NULL;
    unitsByTypeOptions.hashFn = hashUnitType;
    unitsByTypeOptions.equalsFn = equalsUnitType;
    unitsByTypeOptions.freeFn = freeEntityList;
    WarUnitMapInit(&map->unitsByType, unitsByTypeOptions);
    for (WarUnitType type = WAR_UNIT_FOOTMAN; type < WAR_UNIT_COUNT; type++)
    {
        WarEntityList* list = (WarEntityList*)xmalloc(sizeof(WarEntityList));
        WarEntityListInit(list, WarEntityListNonFreeOptions);
        WarUnitMapSet(&map->unitsByType, type, list);
    }

    // initialize the entities by id map
    WarEntityIdMapOptions entitiesByIdOptions = (WarEntityIdMapOptions){0};
    entitiesByIdOptions.defaultValue = NULL;
    entitiesByIdOptions.hashFn = hashEntityId;
    entitiesByIdOptions.equalsFn = equalsEntityId;
    WarEntityIdMapInit(&map->entitiesById, entitiesByIdOptions);

    // initialize ui entities list
    WarEntityListInit(&map->uiEntities, WarEntityListNonFreeOptions);

    // initialize selected entities list
    WarEntityIdListInit(&map->selectedEntities, WarEntityIdListDefaultOptions);

    // initialize map animations lists
    WarSpriteAnimationListInit(&map->animations, WarSpriteAnimationListDefaultOptions);

    map->finder = initPathFinder(PATH_FINDING_ASTAR, MAP_TILES_WIDTH, MAP_TILES_HEIGHT, levelPassable->levelPassable.data);

    context->map = map;

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
        }
        
        for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
        {
            for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
            {
                s32 index = y * MAP_TILES_WIDTH + x;
                u8Color color = getMapTileAverage(levelVisual, tileset, x, y);
                for(s32 i = 0; i < 2; i++)
                {
                    minimapFrames[i].data[index * 4 + 0] = color.r;
                    minimapFrames[i].data[index * 4 + 1] = color.g;
                    minimapFrames[i].data[index * 4 + 2] = color.b;
                    minimapFrames[i].data[index * 4 + 3] = color.a;
                }
            }
        }

        map->minimapSprite = createSpriteFromFrames(context, MINIMAP_WIDTH, MINIMAP_HEIGHT, 2, minimapFrames);
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
                
                WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_FOREST, true);
                addSpriteComponent(context, entity, map->sprite);
                addForestComponent(context, entity, trees);

                for (s32 i = 0; i < trees.count; i++)
                {
                    WarTree* tree = &trees.items[i];
                    setStaticEntity(map->finder, tree->tilex, tree->tiley, 1, 1, entity->id);
                }

                determineTreeTiles(context, entity);
            }
        }

        WarTreeList trees;
        WarTreeListInit(&trees, WarTreeListDefaultOptions);
        WarEntity *forest = createEntity(context, WAR_ENTITY_TYPE_FOREST, true);
        addSpriteComponent(context, forest, map->sprite);
        addForestComponent(context, forest, trees);
        map->forest = forest;
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

    // create players info
    {
        for (s32 i = 0; i < MAX_PLAYERS_COUNT; i++)
        {
            map->players[i].index = 0;
            map->players[i].race = levelInfo->levelInfo.races[i];
            map->players[i].gold = levelInfo->levelInfo.gold[i];
            map->players[i].wood = levelInfo->levelInfo.lumber[i];

            for (s32 j = 0; j < MAX_FEATURES_COUNT; j++)
                map->players[i].features[j] = true; //levelInfo->levelInfo.allowedFeatures[j];

            for (s32 j = 0; j < MAX_UPGRADES_COUNT; j++)
            {
                map->players[i].upgrades[j].allowed = 2; //levelInfo->levelInfo.allowedUpgrades[j][i];
                map->players[i].upgrades[j].level = 0;
            }
        }
    }

    // create the starting entities
    {
        for(s32 i = 0; i < levelInfo->levelInfo.startEntitiesCount; i++)
        {
            WarLevelUnit startUnit = levelInfo->levelInfo.startEntities[i];
            createUnit(context, startUnit.type, startUnit.x, startUnit.y, startUnit.player, 
                       startUnit.resourceKind, startUnit.amount, true);
        }

        createBuilding(context, WAR_UNIT_BARRACKS_HUMANS, 37, 18, 0, false);
        createBuilding(context, WAR_UNIT_LUMBERMILL_HUMANS, 36, 22, 0, false);
        createBuilding(context, WAR_UNIT_BLACKSMITH_HUMANS, 40, 16, 0, false);
        createBuilding(context, WAR_UNIT_CHURCH, 45, 22, 0, false);
        createBuilding(context, WAR_UNIT_STABLE, 45, 18, 0, false);
        createBuilding(context, WAR_UNIT_TOWER_HUMANS, 34, 16, 0, false);
    }

    // add ui entities
    {
        vec2 leftTopPanel = rectTopLeft(map->leftTopPanel);
        vec2 leftBottomPanel = rectTopLeft(map->leftBottomPanel);
        vec2 topPanel = rectTopLeft(map->topPanel);
        vec2 rightPanel = rectTopLeft(map->rightPanel);
        vec2 bottomPanel = rectTopLeft(map->bottomPanel);

        WarSpriteResourceRef invalidRef = invalidResourceRef();
        WarSpriteResourceRef normalRef = imageResourceRef(364);
        WarSpriteResourceRef pressedRef = imageResourceRef(365);
        WarSpriteResourceRef portraitsRef = imageResourceRef(361);

        // panels
        createUIImage(context, "panelLeftTop", imageResourceRef(224), leftTopPanel);
        createUIImage(context, "panelLeftBottom", imageResourceRef(226), leftBottomPanel);
        createUIImage(context, "panelTop", imageResourceRef(218), topPanel);
        createUIImage(context, "panelRight", imageResourceRef(220), rightPanel);
        createUIImage(context, "panelBottom", imageResourceRef(222), bottomPanel);

        // top panel images
        createUIImage(context, "imgGold", imageResourceRef(406), vec2Addv(topPanel, vec2i(201, 1)));
        createUIImage(context, "imgLumber", imageResourceRef(407), vec2Addv(topPanel, vec2i(102, 0)));

        // top panel texts
        createUIText(context, "txtGold", 0, vec2Addv(topPanel, vec2i(135, 2)));
        createUIText(context, "txtWood", 0, vec2Addv(topPanel, vec2i(24, 2)));

        // status text
        createUIText(context, "txtStatus", 0, vec2Addv(bottomPanel, vec2i(2, 5)));
        createUIImage(context, "imgStatusWood", imageResourceRef(407), vec2Addv(bottomPanel, vec2i(163, 3)));
        createUIImage(context, "imgStatusGold", imageResourceRef(406), vec2Addv(bottomPanel, vec2i(200, 5)));
        createUIText(context, "txtStatusWood", 0, vec2Addv(bottomPanel, vec2i(179, 5)));
        createUIText(context, "txtStatusGold", 0, vec2Addv(bottomPanel, vec2i(218, 5)));

        // selected unit(s) info
        createUIImage(context, "imgUnitInfo", imageResourceRef(360), vec2Addv(leftBottomPanel, vec2i(2, 0)));
        createUIImage(context, "imgUnitPortrait0", portraitsRef, vec2Addv(leftBottomPanel, vec2i(6, 4)));
        createUIImage(context, "imgUnitPortrait1", portraitsRef, vec2Addv(leftBottomPanel, vec2i(4, 1)));
        createUIImage(context, "imgUnitPortrait2", portraitsRef, vec2Addv(leftBottomPanel, vec2i(38, 1)));
        createUIImage(context, "imgUnitPortrait3", portraitsRef, vec2Addv(leftBottomPanel, vec2i(4, 23)));
        createUIImage(context, "imgUnitPortrait4", portraitsRef, vec2Addv(leftBottomPanel, vec2i(38, 23)));
        createUIImage(context, "imgUnitInfoLife", imageResourceRef(360), vec2Addv(leftBottomPanel, vec2i(3, 16)));
        createUIText(context, "txtUnitName", 0, vec2Addv(leftBottomPanel, vec2i(6, 26)));
        createUIRect(context, "rectLifeBar0", vec2Addv(leftBottomPanel, vec2i(37, 20)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectLifeBar1", vec2Addv(leftBottomPanel, vec2i(4, 17)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectLifeBar2", vec2Addv(leftBottomPanel, vec2i(38, 17)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectLifeBar3", vec2Addv(leftBottomPanel, vec2i(4, 39)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectLifeBar4", vec2Addv(leftBottomPanel, vec2i(38, 39)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectMagicBar", vec2Addv(leftBottomPanel, vec2i(37, 9)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectPercentBar", vec2Addv(leftBottomPanel, vec2i(4, 37)), vec2i(62, 5), U8COLOR_GREEN);
        createUIImage(context, "rectPercentText", imageResourceRef(410), vec2Addv(leftBottomPanel, vec2i(15, 37)));

        // texts in the command area
        createUIText(context, "txtCommand0", 0, vec2Addv(leftBottomPanel, vec2i(3, 46)));
        createUIText(context, "txtCommand1", 0, vec2Addv(leftBottomPanel, vec2i(3, 56)));
        createUIText(context, "txtCommand2", 0, vec2Addv(leftBottomPanel, vec2i(7, 64)));
        createUIText(context, "txtCommand3", 0, vec2Addv(leftBottomPanel, vec2i(11, 54)));

        // command buttons
        createUIImageButton(
            context, "btnCommand0", 
            normalRef, pressedRef, portraitsRef, 
            vec2Addv(leftBottomPanel, vec2i(2, 44)));

        createUIImageButton(
            context, "btnCommand1", 
            normalRef, pressedRef, portraitsRef, 
            vec2Addv(leftBottomPanel, vec2i(36, 44)));

        createUIImageButton(
            context, "btnCommand2", 
            normalRef, pressedRef, portraitsRef, 
            vec2Addv(leftBottomPanel, vec2i(2, 67)));

        createUIImageButton(
            context, "btnCommand3", 
            normalRef, pressedRef, portraitsRef, 
            vec2Addv(leftBottomPanel, vec2i(36, 67)));

        createUIImageButton(
            context, "btnCommand4", 
            normalRef, pressedRef, portraitsRef, 
            vec2Addv(leftBottomPanel, vec2i(2, 90)));

        createUIImageButton(
            context, "btnCommand5", 
            normalRef, pressedRef, portraitsRef, 
            vec2Addv(leftBottomPanel, vec2i(36, 90)));

        WarEntity* menuButton = createUIImageButton(
            context, "btnMenu", 
            imageResourceRef(362), imageResourceRef(363), invalidRef, 
            vec2Addv(leftBottomPanel, vec2i(3, 116)));

        setUITooltip(menuButton, NO_HIGHLIGHT, "MENU (F10)");

        createUICursor(context, "cursor", WAR_CURSOR_ARROW, VEC2_ZERO);
    }

    // set the initial state for the tiles
    {
        for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
        {
            map->tileStates[i] = MAP_TILE_STATE_UNKOWN;
        }
    }

    // DEBUG
    // add animations
    {
        // test ruins
        {
            WarEntity* ruins = createRuins(context);
            addRuinsPieces(context, ruins, 11, 6, 3);
            addRuinsPieces(context, ruins, 13, 5, 2);
            addRuinsPieces(context, ruins, 9, 5, 3);
            addRuinsPieces(context, ruins, 8, 8, 4);
            determineRuinTypes(context, ruins);

            map->ruin = ruins;
        }
        
        // test animations
        {
            WarSpriteResourceRef spriteResourceRef = imageResourceRef(359);
            WarSprite sprite2 = createSpriteFromResourceIndex(context, spriteResourceRef);
            WarSpriteAnimation* anim2 = createAnimation("horsie2", sprite2, 0.5f, true);
            anim2->offset = vec2i(100, 100);

            const s32 baseFrameIndices2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
            const s32 indexOff2 = 0;

            for(s32 i = 0; i < arrayLength(baseFrameIndices2); i++)
            {
                addAnimationFrame(anim2, baseFrameIndices2[i] + indexOff2);
            }

            WarSpriteAnimationListAdd(&map->animations, anim2);
        }

        {
            WarSpriteResourceRef spriteResourceRef = imageResourceRef(360);
            WarSprite sprite3 = createSpriteFromResourceIndex(context, spriteResourceRef);
            WarSpriteAnimation* anim3 = createAnimation("horsie3", sprite3, 0.5f, true);
            anim3->offset = vec2i(200, 100);

            const s32 baseFrameIndices3[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
            const s32 indexOff3 = 0;

            for(s32 i = 0; i < arrayLength(baseFrameIndices3); i++)
            {
                addAnimationFrame(anim3, baseFrameIndices3[i] + indexOff3);
            }

            WarSpriteAnimationListAdd(&map->animations, anim3);
        }

        {
            WarSpriteResourceRef spriteResourceRef = imageResourceRef(361);
            WarSprite sprite4 = createSpriteFromResourceIndex(context, spriteResourceRef);
            WarSpriteAnimation* anim4 = createAnimation("horsie3", sprite4, 0.5f, true);
            anim4->offset = vec2i(300, 100);

            s32 baseFrameindices4[93];
            for (s32 i = 0; i < 93; i++)
                baseFrameindices4[i] = i;

            const s32 indexOff4 = 0;

            for(s32 i = 0; i < arrayLength(baseFrameindices4); i++)
            {
                addAnimationFrame(anim4, baseFrameindices4[i] + indexOff4);
            }

            WarSpriteAnimationListAdd(&map->animations, anim4);
        }

        createAudio(context, WAR_MUSIC_00, true);
    }

    // WarEntity* txtDebug = createUIText(context, "txtDebug", 1, vec2Addv(rectTopLeft(map->mapPanel), vec2i(30, 60)));
    // txtDebug->text.fontSize = 8;
    // setUIText(txtDebug, NO_HIGHLIGHT, "Warcraft: Orcs & Humans");

    // txtDebug = createUIText(context, "txtDebug2", 1, vec2Addv(rectTopLeft(map->mapPanel), vec2i(30, 80)));
    // txtDebug->text.fontSize = 6;
    // setUIText(txtDebug, NO_HIGHLIGHT, "Yes, my lord.");

    // txtDebug = createUIText(context, "txtDebug3", 1, vec2Addv(rectTopLeft(map->mapPanel), vec2i(30, 100)));
    // txtDebug->text.fontSize = 6;
    // setUIText(txtDebug, NO_HIGHLIGHT, "What do you want?");

    // txtDebug = createUIText(context, "txtDebug4", 1, vec2Addv(rectTopLeft(map->mapPanel), vec2i(30, 120)));
    // txtDebug->text.fontSize = 6;
    // setUIText(txtDebug, NO_HIGHLIGHT, "Stop poking me!");
}

void updateGlobalSpeed(WarContext* context)
{
    WarInput* input = &context->input;

    if (isKeyPressed(input, WAR_KEY_CTRL) && !isKeyPressed(input, WAR_KEY_SHIFT))
    {
        if (wasKeyPressed(input, WAR_KEY_1))
            setGlobalSpeed(context, 1.0f);
        else if (wasKeyPressed(input, WAR_KEY_2))
            setGlobalSpeed(context, 2.0f);
        else if (wasKeyPressed(input, WAR_KEY_3))
            setGlobalSpeed(context, 3.0f);
        else if (wasKeyPressed(input, WAR_KEY_4))
            setGlobalSpeed(context, 4.0f);
    }
}

void updateGlobalScale(WarContext* context)
{
    WarInput* input = &context->input;

    if (isKeyPressed(input, WAR_KEY_CTRL) && isKeyPressed(input, WAR_KEY_SHIFT))
    {
        if (wasKeyPressed(input, WAR_KEY_1))
            setGlobalScale(context, 1.0f);
        else if (wasKeyPressed(input, WAR_KEY_2))
            setGlobalScale(context, 2.0f);
        else if (wasKeyPressed(input, WAR_KEY_3))
            setGlobalScale(context, 3.0f);
        else if (wasKeyPressed(input, WAR_KEY_4))
            setGlobalScale(context, 4.0f);
    }
}

void updateVolume(WarContext* context)
{
    WarInput* input = &context->input;

    if (isKeyPressed(input, WAR_KEY_CTRL))
    {
        if (isKeyPressed(input, WAR_KEY_SHIFT))
        {
            if (wasKeyPressed(input, WAR_KEY_UP))
                changeMusicVolume(context, 0.1f);
            else if (wasKeyPressed(input, WAR_KEY_DOWN))
                changeMusicVolume(context, -0.1f);
        }
        else
        {
            if (wasKeyPressed(input, WAR_KEY_UP))
                changeSoundVolume(context, 0.1f);
            else if (wasKeyPressed(input, WAR_KEY_DOWN))
                changeSoundVolume(context, -0.1f);
        }
    }
}

void updateViewport(WarContext *context)
{
    WarMap *map = context->map;
    WarInput *input = &context->input;

    map->wasScrolling = false;

    vec2 dir = VEC2_ZERO;
    bool wasScrolling = map->isScrolling;

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
            dir = getDirFromMousePos(context, input);
        }
    }
    // check for the arrows keys and update the position of the viewport
    else
    {
        if (!isKeyPressed(input, WAR_KEY_CTRL) && 
            !isKeyPressed(input, WAR_KEY_SHIFT))
        {
            dir = getDirFromArrowKeys(context, input);
        }
    }

    map->viewport.x += map->scrollSpeed * dir.x * context->deltaTime;
    map->viewport.x = clamp(map->viewport.x, 0.0f, MAP_WIDTH - map->viewport.width);

    map->viewport.y += map->scrollSpeed * dir.y * context->deltaTime;
    map->viewport.y = clamp(map->viewport.y, 0.0f, MAP_HEIGHT - map->viewport.height);

    map->isScrolling = !vec2IsZero(dir);

    if (!map->isScrolling)
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
                WarEntityList* units = getEntitiesOfType(map, WAR_ENTITY_TYPE_UNIT);
                for(s32 i = 0; i < units->count; i++)
                {
                    WarEntity* entity = units->items[i];
                    if (entity && entity->unit.enabled)
                    {
                        // don't select dead entities or corpses
                        if (isDead(entity) || isGoingToDie(entity) || isCorpseUnit(entity))
                        {
                            continue;
                        }

                        rect unitRect = rectv(entity->transform.position, getUnitSpriteSize(entity));

                        if (rectIntersects(pointerRect, unitRect))
                        {
                            WarEntityListAdd(&newSelectedEntities, entity);
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

                // calculate the number of dudes and building entities in the selection
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
                            if (map->selectedEntities.count == 1)
                            {
                                WarEntityId oldSelectedEntityId = map->selectedEntities.items[0];
                                if (oldSelectedEntityId == newSelectedEntity->id)
                                {
                                    if (getUnitRace(newSelectedEntity) == WAR_RACE_HUMANS)
                                    {
                                        createAudioRandom(context, WAR_HUMAN_ANNOYED_1, WAR_HUMAN_ANNOYED_3, false);
                                    }
                                    else
                                    {
                                        createAudioRandom(context, WAR_ORC_ANNOYED_1, WAR_ORC_ANNOYED_3, false);
                                    }
                                }
                                else
                                {
                                    if (getUnitRace(newSelectedEntity) == WAR_RACE_HUMANS)
                                    {
                                        createAudioRandom(context, WAR_HUMAN_SELECTED_1, WAR_HUMAN_SELECTED_5, false);
                                    }
                                    else
                                    {
                                        createAudioRandom(context, WAR_ORC_SELECTED_1, WAR_ORC_SELECTED_5, false);
                                    }
                                }
                            }
                            else
                            {
                                if (getUnitRace(newSelectedEntity) == WAR_RACE_HUMANS)
                                {
                                    createAudioRandom(context, WAR_HUMAN_SELECTED_1, WAR_HUMAN_SELECTED_5, false);
                                }
                                else
                                {
                                    createAudioRandom(context, WAR_ORC_SELECTED_1, WAR_ORC_SELECTED_5, false);
                                }
                            }
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
                        if (isBuilding(newSelectedEntity) || isGoingToBuild(newSelectedEntity))
                        {
                            createAudio(context, WAR_BUILDING, false);
                        }
                        else
                        {
                            switch (newSelectedEntity->unit.type)
                            {
                                case WAR_UNIT_CHURCH:
                                {
                                    createAudio(context, WAR_HUMAN_CHURCH, false);
                                    break;
                                }
                                case WAR_UNIT_TEMPLE:
                                {
                                    createAudio(context, WAR_ORC_TEMPLE, false);
                                    break;
                                }
                                case WAR_UNIT_STABLE:
                                {
                                    createAudio(context, WAR_HUMAN_STABLE, false);
                                    break;
                                }
                                case WAR_UNIT_KENNEL:
                                {
                                    createAudio(context, WAR_ORC_KENNEL, false);
                                    break;
                                }
                                case WAR_UNIT_BLACKSMITH_HUMANS:
                                case WAR_UNIT_BLACKSMITH_ORCS:
                                {
                                    createAudio(context, WAR_BLACKSMITH, false);
                                    break;
                                }
                                default:
                                {
                                    createAudio(context, WAR_UI_CLICK, false);
                                    break;
                                }
                            }
                        }
                    }
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

    if (isKeyPressed(input, WAR_KEY_CTRL) && 
        wasKeyPressed(input, WAR_KEY_T))
    {
        map->editingTrees = !map->editingTrees;
    }

    if (map->editingTrees)
    {
        if(wasButtonPressed(input, WAR_MOUSE_LEFT))
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

    if (isKeyPressed(input, WAR_KEY_CTRL) && 
        wasKeyPressed(input, WAR_KEY_R))
    {
        map->editingRoads = !map->editingRoads;
    }

    if (map->editingRoads)
    {
        if(wasButtonPressed(input, WAR_MOUSE_LEFT))
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

    if (isKeyPressed(input, WAR_KEY_CTRL) && 
        wasKeyPressed(input, WAR_KEY_W))
    {
        map->editingWalls = !map->editingWalls;
    }

    if (map->editingWalls)
    {
        if(wasButtonPressed(input, WAR_MOUSE_LEFT))
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

    if (isKeyPressed(input, WAR_KEY_CTRL) && 
        wasKeyPressed(input, WAR_KEY_U))
    {
        map->editingRuins = !map->editingRuins;
    }

    if (map->editingRuins)
    {
        if(wasButtonPressed(input, WAR_MOUSE_LEFT))
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

void updateCommands(WarContext* context)
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
            s32 farmsCount = getPlayerUnitCount(context, 0, entity->unit.type);
            s32 dudesCount = getPlayerDudesCount(context, 0);

            setUIText(commandTexts[0], NO_HIGHLIGHT, "FOOD USAGE:");
            setUITextFormat(commandTexts[1], NO_HIGHLIGHT, "GROWN %d", farmsCount * 4 + 1);
            setUITextFormat(commandTexts[2], NO_HIGHLIGHT, " USED %d", dudesCount);

            return;
        }
    }
    
    // if the selected unit is a goldmine,
    // just add the text with the remaining gold
    if (entity->unit.type == WAR_UNIT_GOLDMINE)
    {
        s32 gold = entity->unit.amount;

        setUIText(commandTexts[0], NO_HIGHLIGHT, "GOLD LEFT");
        setUITextFormat(commandTexts[3], NO_HIGHLIGHT, "%d", gold);
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
            setUITooltip(commandButtons[i], commandData.highlightIndex, commandData.tooltip);
            commandButtons[i]->button.gold = commandData.gold;
            commandButtons[i]->button.wood = commandData.wood;
            commandButtons[i]->button.clickHandler = commandData.clickHandler;
            commandButtons[i]->button.enabled = true;
            commandButtons[i]->button.hotKey = commandData.hotKey;
        }
    }
}

void updateCommandFromButtons(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    WarEntityList* buttons = getEntitiesOfType(map, WAR_ENTITY_TYPE_BUTTON);
    for(s32 i = 0; i < buttons->count; i++)
    {
        WarEntity* entity = buttons->items[i];
        if (entity)
        {
            WarTransformComponent* transform = &entity->transform;
            WarButtonComponent* button = &entity->button;
            
            if (!button->enabled)
            {
                button->hot = false;
                button->active = false;
                continue;
            }

            if (wasKeyPressed(input, button->hotKey))
            {
                if (button->clickHandler)
                {
                    button->hot = false;
                    button->active = false;

                    button->clickHandler(context, entity);

                    // in this case break to not allow pressing multiple keys
                    // and executing all of the command for those keys
                    break;
                }
            }

            vec2 backgroundSize = vec2i(button->normalSprite.frameWidth, button->normalSprite.frameHeight);
            rect buttonRect = rectv(transform->position, backgroundSize);
            bool pointerInside = rectContainsf(buttonRect, input->pos.x, input->pos.y);

            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                button->hot = pointerInside;

                if (button->hot)
                {
                    if (button->clickHandler)
                    {
                        button->clickHandler(context, entity);

                        createAudio(context, WAR_UI_CLICK, false);
                    }
                }

                button->active = false;
            }
            else if (isButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if (button->hot)
                {
                    button->active = true;
                }
            }
            else if (pointerInside)
            {
                for(s32 j = 0; j < buttons->count; j++)
                {
                    WarEntity* otherButton = buttons->items[i];
                    if (otherButton)
                    {
                        otherButton->button.hot = false;
                        otherButton->button.active = false;
                    }
                }

                button->hot = true;
            }
            else
            {
                button->hot = false;
            }
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
                        if (isUnitOfType(targetEntity, WAR_UNIT_GOLDMINE) ||
                            isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST))
                        {
                            executeHarvestCommand(context, targetEntity, targetTile);
                        }
                        else if (isUnitOfType(targetEntity, WAR_UNIT_TOWNHALL_HUMANS) || 
                                isUnitOfType(targetEntity, WAR_UNIT_TOWNHALL_ORCS))
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
    WarFlashStatus* flashStatus = &map->flashStatus;

    if (flashStatus->enabled)
    {
        if (flashStatus->startTime + flashStatus->duration >= context->time)
        {
            setStatus(context, NO_HIGHLIGHT, 0, 0, flashStatus->text);
            return;
        }
        
        // if the time for the flash status is over, just disabled it
        flashStatus->enabled = false;
    }

    char* statusText = NULL;
    s32 highlightIndex = NO_HIGHLIGHT;
    s32 goldCost = 0;
    s32 woodCost = 0;

    bool freeStatusText = false;

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

                    statusText = commandData.tooltip2;
                    highlightIndex = NO_HIGHLIGHT;
                    goldCost = 0;
                    woodCost = 0;
                }
                else if (isUpgrading(selectedEntity) || isGoingToUpgrade(selectedEntity))
                {
                    WarState* upgradeState = getUpgradeState(selectedEntity);
                    WarUpgradeType upgradeToBuild = upgradeState->upgrade.upgradeToBuild;
                    WarUnitCommandMapping commandMapping = getCommandMappingFromUpgradeType(upgradeToBuild);
                    WarUnitCommandBaseData commandData = getCommandBaseData(commandMapping.type);
                    
                    statusText = commandData.tooltip2;
                    highlightIndex = NO_HIGHLIGHT;
                    goldCost = 0;
                    woodCost = 0;
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

                        statusText = (char*)xmalloc(50);
                        sprintf(statusText, "FULL REPAIRS WILL COST %d GOLD & LUMBER", repairCost);
                        freeStatusText = true;

                        highlightIndex = NO_HIGHLIGHT;
                        goldCost = 0;
                        woodCost = 0;
                    }
                }
            }
            else if (isWorkerUnit(selectedEntity))
            {
                if (isCarryingResources(selectedEntity))
                {
                    if (selectedEntity->unit.resourceKind == WAR_RESOURCE_GOLD)
                    {
                        statusText = "CARRYING GOLD";
                    }
                    else if (selectedEntity->unit.resourceKind == WAR_RESOURCE_WOOD)
                    {
                        statusText = "CARRYING LUMBER";
                    }

                    highlightIndex = NO_HIGHLIGHT;
                    goldCost = 0;
                    woodCost = 0;
                }
            }
        }
    }

    WarEntityList* buttons = getEntitiesOfType(map, WAR_ENTITY_TYPE_BUTTON);
    for(s32 i = 0; i < buttons->count; i++)
    {
        WarEntity* entity = buttons->items[i];
        if (entity)
        {
            WarButtonComponent* button = &entity->button;
            if (button->hot)
            {
                statusText = button->tooltip;
                goldCost = button->gold;
                woodCost = button->wood;
                highlightIndex = button->highlightIndex;
                break;
            }
        }
    }

    setStatus(context, highlightIndex, goldCost, woodCost, statusText);

    if (freeStatusText)
    {
        free(statusText);
    }
}

void changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type)
{
    assert(entity->type == WAR_ENTITY_TYPE_CURSOR);

    if (entity->cursor.type == type)
    {
        return;
    }

    WarResource* resource = getOrCreateResource(context, type);
    assert(resource->type == WAR_RESOURCE_TYPE_CURSOR);

    removeCursorComponent(context, entity);
    addCursorComponent(context, entity, type, vec2i(resource->cursor.hotx, resource->cursor.hoty));

    removeSpriteComponent(context, entity);
    addSpriteComponentFromResource(context, entity, imageResourceRef(type));
}

void updateCursor(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;
    
    WarEntity* entity = findUIEntity(context, "cursor");
    if (entity)
    {
        if (input->isDragging)
        {
            changeCursorType(context, entity, WAR_CURSOR_GREEN_CROSSHAIR);
        }
        else if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
        {
            WarUnitCommand* command = &map->command;
            switch (command->type)
            {
                case WAR_COMMAND_ATTACK:
                {
                    changeCursorType(context, entity, WAR_CURSOR_RED_CROSSHAIR);
                    break;
                }

                case WAR_COMMAND_MOVE:
                case WAR_COMMAND_STOP:
                case WAR_COMMAND_HARVEST:
                case WAR_COMMAND_DELIVER:
                case WAR_COMMAND_REPAIR:
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
                    bool entityUnderCursor = false;

                    vec2 pointerRect = vec2ScreenToMapCoordinates(context, input->pos);

                    WarEntityList* units = getEntitiesOfType(map, WAR_ENTITY_TYPE_UNIT);
                    for(s32 i = 0; i < units->count; i++)
                    {
                        WarEntity* entity = units->items[i];
                        if (entity && entity->unit.enabled)
                        {
                            // don't change the cursor for dead entities or corpses
                            if (isDead(entity) || isGoingToDie(entity) || isCorpseUnit(entity))
                            {
                                continue;
                            }
                            
                            rect unitRect = rectv(entity->transform.position, getUnitSpriteSize(entity));
                            if (rectContainsf(unitRect, pointerRect.x, pointerRect.y))
                            {
                                entityUnderCursor = true;
                                break;
                            }
                        }
                    }

                    if (entityUnderCursor)
                    {
                        changeCursorType(context, entity, WAR_CURSOR_MAGNIFYING_GLASS);                        
                    }
                    else
                    {
                        changeCursorType(context, entity, WAR_CURSOR_ARROW);
                    }
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

        vec2 hot = entity->cursor.hot;
        entity->transform.position = vec2Subv(input->pos, hot);
    }
}

void updateStateMachines(WarContext* context)
{
    WarMap* map = context->map;

    WarEntityList* entities = getEntities(map);
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
    WarMap* map = context->map;

    WarEntityList* units = getEntitiesOfType(map, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            updateAction(context, entity);
        }
    }
}

void updateAnimations(WarContext* context)
{
    WarMap* map = context->map;

    // update all animations of entities
    WarEntityList* entities = getEntities(map);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity* entity = entities->items[i];
        if (entity)
        {
            updateEntityAnimations(context, entity);
        }
    }

    // update all animations of the map
    for(s32 i = 0; i < map->animations.count; i++)
    {
        WarSpriteAnimation* anim = map->animations.items[i];
        updateAnimation(context, anim);
    }
}

void updateProjectile(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarProjectileComponent* projectile = &entity->projectile;

    if (sprite->enabled && projectile->enabled)
    {
        switch (projectile->type)
        {
            case WAR_PROJECTILE_ARROW:
            {
                vec2 position = transform->position;
                
                vec2 direction = vec2Subv(projectile->target, projectile->origin);
                f32 directionLength = vec2Length(direction);
                direction = vec2Normalize(direction);

                f32 speed = getScaledSpeed(context, projectile->speed);
                vec2 step = vec2Mulf(direction, speed * context->deltaTime);
                f32 stepLength = vec2Length(step);

                if (directionLength < stepLength)
                {
                    step = direction;
                }

                position = vec2Addv(position, step);

                s32 frameIndex = 2;
                vec2 scale = VEC2_ONE;

                f32 angle = vec2Angle(VEC2_RIGHT, direction);
                if (angle >= 22.5f && angle < 67.5f)
                    frameIndex = 1;
                else if (angle >= 67.5f && angle < 112.5f)
                    frameIndex = 0;
                else if (angle >= 112.5f && angle < 157.5f)
                    frameIndex = 1;
                    scale.x *= -1;
                else if (angle >= 157.5f && angle < 202.5f)
                    frameIndex = 2;
                    scale.x *= -1;
                else if (angle >= 202.5f && angle < 247.5f)
                    frameIndex = 3;
                    scale.x *= -1;
                else if (angle >= 247.5f && angle < 292.5f)
                    frameIndex = 4;
                else if (angle >= 292.5f && angle < 337.5f)
                    frameIndex = 3;
                else
                    frameIndex = 2;

                transform->position = position;
                transform->scale = scale;
                sprite->frameIndex = frameIndex;

                break;
            }
            case WAR_PROJECTILE_CATAPULT:
            {
                break;
            }
            case WAR_PROJECTILE_FIREBALL:
            {
                break;
            }
            case WAR_PROJECTILE_FIREBALL_2:
            {
                break;
            }
            case WAR_PROJECTILE_WATER_ELEMENTAL:
            {
                break;
            }
            case WAR_PROJECTILE_RAIN_OF_FIR:
            {
                break;
            }
        }

        WarProjectileType type;
        WarEntityId owner;
        vec2 fromTile;
        vec2 toTile;
        f32 speed;
    }
}

void updateProjectiles(WarContext* context)
{
    WarMap* map = context->map;

    WarEntityList* projectiles = getEntitiesOfType(map, WAR_ENTITY_TYPE_PROJECTILE);
    for (s32 i = 0; i < projectiles->count; i++)
    {
        WarEntity* entity = projectiles->items[i];
        if (entity)
        {
            updateProjectile(context, entity);
        }
    }
}

void updateMap(WarContext* context)
{
    updateGlobalSpeed(context);
    updateGlobalScale(context);
    updateVolume(context);

    updateViewport(context);
    updateDragRect(context);

    if (!executeCommand(context))
    {
        // only update the selection if the current command doesn't get
        // executed or there is no command at all.
        // the reason is because some commands are executed by the left click
        // as well as the selection, and if a command get executed the current 
        // selection shouldn't be lost
        updateSelection(context);
    }

    updateStateMachines(context);
    updateActions(context);
    updateAnimations(context);
    updateProjectiles(context);

    updateGoldText(context);
    updateWoodText(context);
    updateSelectedUnitsInfo(context);
    updateCommands(context);
    updateCommandFromButtons(context);
    updateCommandFromRightClick(context);
    updateStatus(context);
    updateCursor(context);

    updateTreesEdit(context);
    updateRoadsEdit(context);
    updateWallsEdit(context);
    updateRuinsEdit(context);
}

void renderMapPanel(WarContext *context)
{
    WarMap *map = context->map;

    NVGcontext* gfx = context->gfx;

    WarResource* levelInfo = getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    nvgSave(gfx);

    nvgTranslate(gfx, map->mapPanel.x, map->mapPanel.y);
    nvgTranslate(gfx, -map->viewport.x, -map->viewport.y);
    
    // render terrain
    {
        WarResource* levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
        assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

        NVGimageBatch* batch = nvgBeginImageBatch(gfx, map->sprite.image, MAP_TILES_WIDTH * MAP_TILES_HEIGHT);

        for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
        {
            for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
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

        nvgEndImageBatch(gfx, batch);
    }

    // render roads
    {
        WarEntityList* roads = getEntitiesOfType(map, WAR_ENTITY_TYPE_ROAD);
        for(s32 i = 0; i < roads->count; i++)
        {
            WarEntity *entity = roads->items[i];
            if (entity)
            {
                renderEntity(context, entity, false);
            }
        }
    }

    // render walls
    {
        WarEntityList* walls = getEntitiesOfType(map, WAR_ENTITY_TYPE_WALL);
        for(s32 i = 0; i < walls->count; i++)
        {
            WarEntity *entity = walls->items[i];
            if (entity)
            {
                renderEntity(context, entity, false);
            }
        }
    }

    // render ruins
    {
        WarEntityList* ruins = getEntitiesOfType(map, WAR_ENTITY_TYPE_RUIN);
        for(s32 i = 0; i < ruins->count; i++)
        {
            WarEntity* entity = ruins->items[i];
            if (entity)
            {
                renderEntity(context, entity, false);
            }
        }
    }

    // render wood
    {
        WarEntityList* forests = getEntitiesOfType(map, WAR_ENTITY_TYPE_FOREST);
        for(s32 i = 0; i < forests->count; i++)
        {
            WarEntity* entity = forests->items[i];
            if (entity)
            {
                renderEntity(context, entity, false);
            }
        }
    }

    {
#ifdef DEBUG_RENDER_UNIT_PATHS
        WarEntityList* units = getEntitiesOfType(map, WAR_ENTITY_TYPE_UNIT);
        for(s32 i = 0; i < units->count; i++)
        {
            WarEntity *entity = units->items[i];
            if (entity)
            {
                WarState* moveState = getDirectState(entity, WAR_STATE_MOVE)
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
#endif
    }

    {
#ifdef DEBUG_RENDER_PASSABLE_INFO
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
#endif
    }

    {
#ifdef DEBUG_RENDER_MAP_GRID
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
#endif
    }

    // render units
    {
        WarEntityList* units = getEntitiesOfType(map, WAR_ENTITY_TYPE_UNIT);
        for(s32 i = 0; i < units->count; i++)
        {
            WarEntity *entity = units->items[i];
            if (entity)
            {
                bool selected = WarEntityIdListContains(&map->selectedEntities, entity->id);
                renderEntity(context, entity, selected);
            }
        }
    }

    // render animations
    {
        for(s32 i = 0; i < map->animations.count; i++)
        {
            WarSpriteAnimation* anim = map->animations.items[i];
            if (anim->status == WAR_ANIM_STATUS_RUNNING)
            {
                s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
                animFrameIndex = clamp(animFrameIndex, 0, anim->frames.count - 1);

                s32 spriteFrameIndex = anim->frames.items[animFrameIndex];
                assert(spriteFrameIndex >= 0 && spriteFrameIndex < anim->sprite.framesCount);

                nvgSave(gfx);

                nvgTranslate(gfx, anim->offset.x, anim->offset.y);
                nvgScale(gfx, anim->scale.x, anim->scale.y);

#ifdef DEBUG_RENDER_MAP_ANIMATIONS
                // size of the original sprite
                vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

                nvgFillRect(gfx, rectv(VEC2_ZERO, animFrameSize), NVG_GRAY_TRANSPARENT);
#endif

                WarSpriteFrame frame = anim->sprite.frames[spriteFrameIndex];
                updateSpriteImage(context, anim->sprite, frame.data);
                renderSprite(context, anim->sprite, VEC2_ZERO, VEC2_ONE);

                nvgRestore(gfx);
            }
        }
    }

    nvgRestore(gfx);
}

void renderMap(WarContext *context)
{
    WarMap *map = context->map;

    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);
    nvgScale(gfx, context->globalScale, context->globalScale);

    // render map
    renderMapPanel(context);

    // render ui
    renderMapUI(context);

    nvgRestore(gfx);
}