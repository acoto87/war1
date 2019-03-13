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

    WarEntityListInit(&map->entities, WarEntityListDefaultOptions);
    WarEntityIdListInit(&map->selectedEntities, WarEntityIdListDefaultOptions);
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

        // DEBUG: create debug forest
        WarTreeList trees;
        WarTreeListInit(&trees, WarTreeListDefaultOptions);
        WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_FOREST, true);
        addSpriteComponent(context, entity, map->sprite);
        addForestComponent(context, entity, trees);
        context->debugForest = entity;
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

        context->debugRoad = road;
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
            piece->hp = WALL_MAX_HP;
            piece->maxhp = WALL_MAX_HP;
        }

        addStateMachineComponent(context, wall);

        WarState* idleState = createIdleState(context, wall, false);
        changeNextState(context, wall, idleState, true, true);

        context->debugWall = wall;
    }

    // create players info
    {
        for (s32 i = 0; i < MAX_PLAYERS_COUNT; i++)
        {
            map->players[i].index = 0;
            map->players[i].race = levelInfo->levelInfo.races[i];
            map->players[i].gold = levelInfo->levelInfo.gold[i];
            map->players[i].wood = levelInfo->levelInfo.lumber[i];
            map->players[i].units = 0;
        }
    }

    // create the starting entities
    {
        for(s32 i = 0; i < levelInfo->levelInfo.startEntitiesCount; i++)
        {
            WarLevelUnit startUnit = levelInfo->levelInfo.startEntities[i];

            WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_UNIT, true);
            addUnitComponent(context, entity, startUnit.type, startUnit.x, startUnit.y, startUnit.player, startUnit.resourceKind, startUnit.amount);
            addTransformComponent(context, entity, vec2i(startUnit.x * MEGA_TILE_WIDTH, startUnit.y * MEGA_TILE_HEIGHT));

            WarUnitsData unitData = getUnitsData(startUnit.type);

            s32 spriteIndex = unitData.resourceIndex;
            if (spriteIndex == 0)
            {
                logError("Sprite for unit of type %d is not configure properly. Default to footman sprite.\n", startUnit.type);
                spriteIndex = 279;
            }
            addSpriteComponentFromResource(context, entity, imageResourceRef(spriteIndex));

            addUnitActions(entity);
            addAnimationsComponent(context, entity);
            addStateMachineComponent(context, entity);

            if (isDudeUnit(entity))
            {
                WarUnitStats unitStats = getUnitStats(startUnit.type);

                entity->unit.maxhp = unitStats.hp;
                entity->unit.hp = unitStats.hp;
                entity->unit.maxMagic = unitStats.magic;
                entity->unit.magic = 100;
                entity->unit.armour = unitStats.armour;
                entity->unit.range = unitStats.range;
                entity->unit.minDamage = unitStats.minDamage;
                entity->unit.rndDamage = unitStats.rndDamage;
                entity->unit.decay = unitStats.decay;
            }
            else if(isBuildingUnit(entity))
            {
                WarBuildingStats buildingStats = getBuildingStats(startUnit.type);

                entity->unit.maxhp = buildingStats.hp;
                entity->unit.hp = buildingStats.hp;
                entity->unit.armour = buildingStats.armour;
            }

            map->players[entity->unit.player].units++;

            WarState* idleState = createIdleState(context, entity, isDudeUnit(entity));
            changeNextState(context, entity, idleState, true, true);
        }
    }

    // add ui entities
    {
        vec2 leftTopPanel = rectTopLeft(map->leftTopPanel);
        vec2 leftBottomPanel = rectTopLeft(map->leftBottomPanel);
        vec2 topPanel = rectTopLeft(map->topPanel);
        vec2 rightPanel = rectTopLeft(map->rightPanel);
        vec2 bottomPanel =rectTopLeft(map->bottomPanel);

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
        createUIText(context, "txtGold", vec2Addv(topPanel, vec2i(150, 1)));
        createUIText(context, "txtWood", vec2Addv(topPanel, vec2i(50, 1)));

        // status text
        createUIText(context, "txtStatus", vec2Addv(rectTopLeft(map->bottomPanel), vec2i(0, 4)));

        // selected unit(s) info
        createUIImage(context, "imgUnitInfo", imageResourceRef(360), vec2Addv(leftBottomPanel, vec2i(2, 0)));
        createUIImage(context, "imgUnitPortrait0", imageResourceRef(361), vec2Addv(leftBottomPanel, vec2i(6, 4)));
        createUIImage(context, "imgUnitPortrait1", imageResourceRef(361), vec2Addv(leftBottomPanel, vec2i(4, 1)));
        createUIImage(context, "imgUnitPortrait2", imageResourceRef(361), vec2Addv(leftBottomPanel, vec2i(38, 1)));
        createUIImage(context, "imgUnitPortrait3", imageResourceRef(361), vec2Addv(leftBottomPanel, vec2i(4, 23)));
        createUIImage(context, "imgUnitPortrait4", imageResourceRef(361), vec2Addv(leftBottomPanel, vec2i(38, 23)));
        createUIImage(context, "imgUnitInfoLife", imageResourceRef(360), vec2Addv(leftBottomPanel, vec2i(3, 16)));
        createUIText(context, "txtUnitName", vec2Addv(leftBottomPanel, vec2i(6, 25)));
        createUIRect(context, "rectLifeBar0", vec2Addv(leftBottomPanel, vec2i(37, 20)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectLifeBar1", vec2Addv(leftBottomPanel, vec2i(4, 17)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectLifeBar2", vec2Addv(leftBottomPanel, vec2i(38, 17)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectLifeBar3", vec2Addv(leftBottomPanel, vec2i(4, 39)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectLifeBar4", vec2Addv(leftBottomPanel, vec2i(38, 39)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectMagicBar", vec2Addv(leftBottomPanel, vec2i(37, 9)), vec2i(27, 3), U8COLOR_GREEN);
        createUIRect(context, "rectPercentBar", vec2Addv(leftBottomPanel, vec2i(4, 37)), vec2i(62, 5), U8COLOR_GREEN);
        createUIImage(context, "rectPercentText", imageResourceRef(410), vec2Addv(leftBottomPanel, vec2i(15, 37)));

        WarSpriteResourceRef invalidRef = invalidResourceRef();

        WarEntity* commandButtons[6] = 
        {
            createUIImageButton(context, "command0", NULL, invalidRef, invalidRef, invalidRef, vec2Addv(leftBottomPanel, vec2i(2, 46)), NULL),
            createUIImageButton(context, "command1", NULL, invalidRef, invalidRef, invalidRef,vec2Addv(leftBottomPanel, vec2i(36, 46)), NULL),
            createUIImageButton(context, "command2", NULL, invalidRef, invalidRef, invalidRef,vec2Addv(leftBottomPanel, vec2i(2, 69)), NULL),
            createUIImageButton(context, "command3", NULL, invalidRef, invalidRef, invalidRef,vec2Addv(leftBottomPanel, vec2i(36, 69)), NULL),
            createUIImageButton(context, "command4", NULL, invalidRef, invalidRef, invalidRef,vec2Addv(leftBottomPanel, vec2i(2, 92)), NULL),
            createUIImageButton(context, "command5", NULL, invalidRef, invalidRef, invalidRef,vec2Addv(leftBottomPanel, vec2i(36, 92)), NULL)
        };

        for (s32 i = 0; i < 6; i++)
            commandButtons[i]->button.enabled = false;

        createUIImageButton(context,
                           "menu",
                           "MENU (F10)",
                           imageResourceRef(362),
                           imageResourceRef(363),
                           invalidRef,
                           vec2Addv(leftBottomPanel, vec2i(3, 116)),
                           NULL);
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

            context->debugRuin = ruins;
        }
        
        // test animations
        {
            WarSpriteResourceRef spriteResourceRef = createSpriteResourceRef(359, 0, NULL);
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
            WarSpriteResourceRef spriteResourceRef = createSpriteResourceRef(360, 0, NULL);
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
            WarSpriteResourceRef spriteResourceRef = createSpriteResourceRef(361, 0, NULL);
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
    }
}

internal void updateViewport(WarContext *context)
{
    WarMap *map = context->map;
    WarInput *input = &context->input;

    map->wasScrolling = false;

    vec2 dir = VEC2_ZERO;
    bool wasScrolling = map->isScrolling;

    // if there was a click in the minimap, then update the position of the viewport
    if (isButtonPressed(input, WAR_MOUSE_LEFT))
    {
        rect minimapPanel = map->minimapPanel;
        rect mapPanel = map->mapPanel;

        // check if the click is inside the minimap panel        
        if (rectContainsf(minimapPanel, input->pos.x, input->pos.y))
        {
            vec2 minimapSize = vec2i(MINIMAP_WIDTH, MINIMAP_HEIGHT);
            vec2 offset = vec2ScreenToMinimapCoordinates(context, input->pos);

            map->viewport.x = offset.x * MAP_WIDTH / minimapSize.x;
            map->viewport.y = offset.y * MAP_HEIGHT / minimapSize.y;
        }
        // check if it was at the edge of the map to scroll also and update the position of the viewport
        else if(!input->isDragging && rectContainsf(mapPanel, input->pos.x, input->pos.y))
        {
            dir = getDirFromMousePos(context, input);
        }
    }
    // check for the arrows keys and update the position of the viewport
    else
    {
        dir = getDirFromArrowKeys(context, input);
    }

    map->viewport.x += map->scrollSpeed * dir.x * context->deltaTime;
    map->viewport.x = clamp(map->viewport.x, 0.0f, MAP_WIDTH - map->viewport.width);

    map->viewport.y += map->scrollSpeed * dir.y * context->deltaTime;
    map->viewport.y = clamp(map->viewport.y, 0.0f, MAP_HEIGHT - map->viewport.height);

    map->isScrolling = !vec2IsZero(dir);

    if (!map->isScrolling)
        map->wasScrolling = wasScrolling;
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
                rect pointerRect = rectScreenToMapCoordinates(context, input->dragRect);

                // select the entities inside the dragging rect
                for(s32 i = 0; i < map->entities.count; i++)
                {
                    WarEntity* entity = map->entities.items[i];
                    if (entity && isUnit(entity) && entity->unit.enabled)
                    {
                        rect unitRect = rectv(entity->transform.position, getUnitSpriteSize(entity));

                        if (rectIntersects(pointerRect, unitRect))
                        {
                            addEntityToSelection(context, entity->id);
                        }
                        else if (!input->keys[WAR_KEY_CTRL].pressed)
                        {
                            removeEntityFromSelection(context, entity->id);
                        }
                    }
                }

                
            }
        }
    }
}

void updateTreesEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (wasKeyPressed(input, WAR_KEY_T))
    {
        context->editingTrees = !context->editingTrees;
    }

    if(wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (context->editingTrees)
        {
            vec2 pointerPos = vec2ScreenToMapCoordinates(context, input->pos);
            pointerPos =  vec2MapToTileCoordinates(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntityId entityId = getTileEntityId(map->finder, x, y);
            WarEntity* entity = findEntity(context, entityId);
            if (!entity)
            {
                entity = context->debugForest;

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
    WarInput* input = &context->input;

    if (wasKeyPressed(input, WAR_KEY_R))
    {
        context->editingRoads = !context->editingRoads;
    }

    if(wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (context->editingRoads)
        {
            vec2 pointerPos = vec2ScreenToMapCoordinates(context, input->pos);
            pointerPos =  vec2MapToTileCoordinates(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntity* road = context->debugRoad;

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
    WarInput* input = &context->input;

    if (wasKeyPressed(input, WAR_KEY_W))
    {
        context->editingWalls = !context->editingWalls;
    }

    if(wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (context->editingWalls)
        {
            vec2 pointerPos = vec2ScreenToMapCoordinates(context, input->pos);
            pointerPos =  vec2MapToTileCoordinates(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntity* wall = context->debugWall;

            WarWallPiece* piece = getWallPieceAtPosition(wall, x, y);
            if (!piece)
            {
                addWallPiece(wall, x, y, 0);

                for(s32 i = 0; i < wall->wall.pieces.count; i++)
                {
                    WarWallPiece* piece = &wall->wall.pieces.items[i];
                    piece->hp = 60;
                    piece->maxhp = 60;
                }

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
    WarInput* input = &context->input;

    if (wasKeyPressed(input, WAR_KEY_U))
    {
        context->editingRuins = !context->editingRuins;
    }

    if(wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (context->editingRuins)
        {
            vec2 pointerPos = vec2ScreenToMapCoordinates(context, input->pos);
            pointerPos =  vec2MapToTileCoordinates(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntity* ruin = context->debugRuin;

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

void updateButtonState(WarContext* context, WarEntity* entity)
{
    assert(entity->type == WAR_ENTITY_TYPE_BUTTON);

    WarMap* map = context->map;
    WarInput* input = &context->input;

    WarTransformComponent* transform = &entity->transform;
    WarButtonComponent* button = &entity->button;

    rect buttonRect = rectv(transform->position, button->backgroundSize);
    bool pointerInside = rectContainsf(buttonRect, input->pos.x, input->pos.y);
    if (wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        button->hover = pointerInside;

        if (button->hover)
        {
            if (button->onClick)
                button->onClick(context, entity);
        }

        button->pressed = false;
    }
    else if (isButtonPressed(input, WAR_MOUSE_LEFT))
    {
        if (button->hover)
        {
            button->pressed = button->hover;
        }
    }
    else if (pointerInside)
    {
        for(s32 i = 0; i < map->entities.count; i++)
        {
            WarEntity* button = map->entities.items[i];
            if (button && button->type == WAR_ENTITY_TYPE_BUTTON)
            {
                button->button.hover = false;
                button->button.pressed = false;
            }
        }

        button->hover = true;

        setStatusText(context, button->tooltip);
    }
    else if (button->hover)
    {
        setStatusText(context, NULL);

        button->hover = false;
    }
}

void updateButtons(WarContext* context)
{
    WarMap* map = context->map;

    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_BUTTON)
        {
            updateButtonState(context, entity);
        }
    }
}

void updateMap(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    WarInput* input = &context->input;

    updateGlobalSpeed(context);
    updateGlobalScale(context);
    updateViewport(context);
    updateDragRect(context);
    updateSelection(context);

    updateGoldText(context);
    updateWoodText(context);
    updateSelectedUnitsInfo(context);
    updateButtons(context);

    updateTreesEdit(context);
    updateRoadsEdit(context);
    updateWallsEdit(context);
    updateRuinsEdit(context);

    // update all state machines
    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity)
        {
            updateStateMachine(context, entity);
        }
    }

    // update all actions
    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity && isUnit(entity))
        {
            updateAction(context, entity);
        }
    }

    // update all animations of entities
    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity)
        {
            updateAnimations(context, entity);
        }
    }

    // update all animations of the map
    for(s32 i = 0; i < map->animations.count; i++)
    {
        WarSpriteAnimation* anim = map->animations.items[i];
        updateAnimation(context, anim);
    }

    //
    // TODO: Refactor this code to an order system that is more robust
    //
    if (wasButtonPressed(input, WAR_MOUSE_RIGHT))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        if (selEntitiesCount > 0)
        {
            vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
            vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

            WarEntityId targetEntityId = getTileEntityId(map->finder, targetTile.x, targetTile.y);
            if (targetEntityId > 0)
            {
                WarEntity* targetEntity = findEntity(context, targetEntityId);
                if (targetEntity)
                {
                    for(s32 i = 0; i < selEntitiesCount; i++)
                    {
                        WarEntityId entityId = map->selectedEntities.items[i];
                        WarEntity* entity = findEntity(context, entityId);
                        assert(entity);

                        // TODO: check here if the attacker can attack the target entity
                        // if it can, the attacker go to attack state
                        // if it can't, the attacker go to follow state

                        // TODO: if right click on a friendly unit, default to follow
                        // to attack a friendly unit the player need to explicitly click on the attack button and the click on the unit,

                        // TODO: if the selected unit is a worker, and target entity is a gold or wood, go to the gathering state

                        if (entity->id != targetEntity->id)
                        {
                            if (isUnitOfType(targetEntity, WAR_UNIT_GOLDMINE))
                            {
                                if (isUnitOfType(entity, WAR_UNIT_PEASANT) ||
                                    isUnitOfType(entity, WAR_UNIT_PEON))
                                {
                                    if (isCarryingResources(entity))
                                    {
                                        // find the closest town hall to deliver the gold
                                        WarRace race = getUnitRace(entity);
                                        WarUnitType townHallType = getTownHallOfRace(race);
                                        WarEntity* townHall = findClosestUnitOfType(context, entity, townHallType);

                                        // if the town hall doesn't exists (it could be under attack and get destroyed), go idle
                                        if (!townHall)
                                        {
                                            WarState* idleState = createIdleState(context, entity, true);
                                            changeNextState(context, entity, idleState, true, true);
                                        }
                                        else
                                        {
                                            WarState* deliverState = createDeliverState(context, entity, townHall->id);
                                            deliverState->nextState = createGatherGoldState(context, entity, targetEntity->id);
                                            changeNextState(context, entity, deliverState, true, true);
                                        }
                                    }
                                    else
                                    {
                                        WarState* gatherGoldState = createGatherGoldState(context, entity, targetEntity->id);
                                        changeNextState(context, entity, gatherGoldState, true, true);        
                                    }
                                }
                                else
                                {
                                    WarState* followState = createFollowState(context, entity, targetEntityId, 1);
                                    changeNextState(context, entity, followState, true, true);
                                }
                            }
                            else if(targetEntity->type == WAR_ENTITY_TYPE_FOREST)
                            {
                                if (isUnitOfType(entity, WAR_UNIT_PEASANT) ||
                                    isUnitOfType(entity, WAR_UNIT_PEON))
                                {
                                    if (isCarryingResources(entity))
                                    {
                                        // find the closest town hall to deliver the gold
                                        WarRace race = getUnitRace(entity);
                                        WarUnitType townHallType = getTownHallOfRace(race);
                                        WarEntity* townHall = findClosestUnitOfType(context, entity, townHallType);

                                        // if the town hall doesn't exists (it could be under attack and get destroyed), go idle
                                        if (!townHall)
                                        {
                                            WarState* idleState = createIdleState(context, entity, true);
                                            changeNextState(context, entity, idleState, true, true);
                                        }
                                        else
                                        {
                                            WarState* deliverState = createDeliverState(context, entity, townHall->id);
                                            deliverState->nextState = createGatherWoodState(context, entity, targetEntityId, targetTile);
                                            changeNextState(context, entity, deliverState, true, true);
                                        }
                                    }
                                    else
                                    {
                                        WarState* gatherWoodState = createGatherWoodState(context, entity, targetEntityId, targetTile);
                                        changeNextState(context, entity, gatherWoodState, true, true);        
                                    }
                                }
                                else
                                {
                                    WarState* followState = createFollowState(context, entity, targetEntityId, 1);
                                    changeNextState(context, entity, followState, true, true);
                                }
                            }
                            else if (isUnit(targetEntity) && (isUnitOfType(targetEntity, WAR_UNIT_TOWNHALL_HUMANS) || isUnitOfType(targetEntity, WAR_UNIT_TOWNHALL_ORCS)))
                            {
                                if (isUnitOfType(entity, WAR_UNIT_PEASANT) || isUnitOfType(entity, WAR_UNIT_PEON))
                                {
                                    if (isCarryingResources(entity))
                                    {
                                        // find the closest town hall to deliver the gold
                                        WarRace race = getUnitRace(entity);
                                        WarUnitType townHallType = getTownHallOfRace(race);
                                        WarEntity* townHall = findClosestUnitOfType(context, entity, townHallType);

                                        // if the town hall doesn't exists (it could be under attack and get destroyed), go idle
                                        if (!townHall)
                                        {
                                            WarState* idleState = createIdleState(context, entity, true);
                                            changeNextState(context, entity, idleState, true, true);
                                        }
                                        else
                                        {
                                            WarState* deliverState = createDeliverState(context, entity, townHall->id);
                                            changeNextState(context, entity, deliverState, true, true);
                                        }
                                    }
                                }
                                else
                                {
                                    // WarState* followState = createFollowState(context, entity, targetEntityId, 1);
                                    // changeNextState(context, entity, followState, true, true);

                                    WarState* attackState = createAttackState(context, entity, targetEntityId, targetTile);
                                    changeNextState(context, entity, attackState, true, true);
                                }
                            }
                            else
                            {
                                // WarState* followState = createFollowState(context, entity, targetEntityId, 1);
                                // changeNextState(context, entity, followState, true, true);

                                WarState* attackState = createAttackState(context, entity, targetEntityId, targetTile);
                                changeNextState(context, entity, attackState, true, true);
                            }
                        }
                    }                    
                }
            }
            else
            {
                rect* rs = (rect*)xcalloc(selEntitiesCount, sizeof(rect));

                for(s32 i = 0; i < selEntitiesCount; i++)
                {
                    WarEntityId entityId = map->selectedEntities.items[i];
                    WarEntity* entity = findEntity(context, entityId);
                    assert(entity);

                    rs[i] = rectv(entity->transform.position, getUnitSpriteSize(entity));
                }

                rect bbox = rs[0];

                for(s32 i = 1; i < selEntitiesCount; i++)
                {
                    if (rs[i].x < bbox.x)
                        bbox.x = rs[i].x;
                    if (rs[i].y < bbox.y)
                        bbox.y = rs[i].y;
                    if (rs[i].x + rs[i].width > bbox.x + bbox.width)
                        bbox.width = (rs[i].x + rs[i].width) - bbox.x;
                    if (rs[i].y + rs[i].height > bbox.y + bbox.height)
                        bbox.height = (rs[i].y + rs[i].height) - bbox.y;
                }

                rect targetbbox = rectf(
                    targetPoint.x - halff(bbox.width),
                    targetPoint.y - halff(bbox.height),
                    bbox.width,
                    bbox.height);

                for(s32 i = 0; i < selEntitiesCount; i++)
                {
                    WarEntityId entityId = map->selectedEntities.items[i];
                    WarEntity* entity = findEntity(context, entityId);
                    assert(entity);

                    vec2 position = vec2f(
                        rs[i].x + halff(rs[i].width), 
                        rs[i].y + halff(rs[i].height));

                    position = vec2MapToTileCoordinates(position);

                    rect targetRect = rectf(
                        targetbbox.x + (rs[i].x - bbox.x),
                        targetbbox.y + (rs[i].y - bbox.y),
                        rs[i].width, 
                        rs[i].height);

                    vec2 target = vec2f(
                        targetRect.x + halff(targetRect.width), 
                        targetRect.y + halff(targetRect.height));

                    target = vec2MapToTileCoordinates(target);

                    if (isKeyPressed(input, WAR_KEY_SHIFT))
                    {
                        if (isPatrolling(entity))
                        {
                            if(isMoving(entity))
                            {
                                WarState* moveState = getMoveState(entity);
                                vec2ListAdd(&moveState->move.positions, target);
                            }
                            
                            WarState* patrolState = getPatrolState(entity);
                            vec2ListAdd(&patrolState->patrol.positions, target);
                        }
                        else if(isMoving(entity) && !isAttacking(entity))
                        {
                            WarState* moveState = getMoveState(entity);
                            vec2ListAdd(&moveState->move.positions, target);
                        }
                        else
                        {
                            WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, target));
                            changeNextState(context, entity, moveState, true, true);
                        }
                    }
                    else
                    {
                        WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, target));
                        changeNextState(context, entity, moveState, true, true);

                        // WarState* patrolState = createPatrolState(context, entity, 2, arrayArg(vec2, position, target));
                        // changeNextState(context, entity, patrolState, true, true);
                    }
                }

                free(rs);
            }
        }
    }
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
        for(s32 i = 0; i < map->entities.count; i++)
        {
            WarEntity *entity = map->entities.items[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_ROAD)
            {
                renderEntity(context, entity, false);
            }
        }
    }

    // render walls
    {
        for(s32 i = 0; i < map->entities.count; i++)
        {
            WarEntity *entity = map->entities.items[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_WALL)
            {
                renderEntity(context, entity, false);
            }
        }
    }

    // render ruins
    {
        for(s32 i = 0; i < map->entities.count; i++)
        {
            WarEntity* entity = map->entities.items[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_RUIN)
            {
                renderEntity(context, entity, false);
            }
        }
    }

    // render wood
    {
        for(s32 i = 0; i < map->entities.count; i++)
        {
            WarEntity* entity = map->entities.items[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_FOREST)
            {
                renderEntity(context, entity, false);
            }
        }
    }

#ifdef DEBUG_RENDER_UNIT_PATHS
    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity *entity = map->entities.items[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
        {
            WarStateMachineComponent* stateMachine = &entity->stateMachine;
            WarState* currentState = stateMachine->currentState;
            if (currentState && currentState->type == WAR_STATE_MOVE)
            {
                vec2List positions = currentState->move.positions;
                for(s32 k = currentState->move.positionIndex; k < positions.count; k++)
                {
                    vec2 pos = vec2TileToMapCoordinates(positions.items[k], true);
                    pos = vec2Subv(pos, vec2i(2, 2));
                    nvgFillRect(gfx, rectv(pos, vec2i(4, 4)), getColorFromList(entity->id));
                }
                
                s32 index = currentState->move.pathNodeIndex;
                WarMapPath path = currentState->move.path;
                
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

    // render units
    {
        for(s32 i = 0; i < map->entities.count; i++)
        {
            WarEntity *entity = map->entities.items[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
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
    assert(map);
    if (!map) return;

    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);
    nvgScale(gfx, context->globalScale, context->globalScale);

    // render map
    renderMapPanel(context);

    // render ui
    renderMapUI(context);

    nvgRestore(gfx);
}