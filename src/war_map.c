inline vec2 vec2ScreenToMapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;
    assert(map);

    rect mapPanel = map->mapPanel;
    rect viewport = map->viewport;

    v = vec2Translatef(v, -mapPanel.x, -mapPanel.y);
    v = vec2Translatef(v, viewport.x, viewport.y);
    return v;
}

inline vec2 vec2ScreenToMinimapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;
    assert(map);

    rect minimapPanel = map->minimapPanel;
    vec2 minimapPanelSize = vec2f(minimapPanel.width, minimapPanel.height);

    vec2 minimapViewportSize = vec2f(MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT);

    v = vec2Translatef(v, -minimapPanel.x, -minimapPanel.y);
    v = vec2Translatef(v, -minimapViewportSize.x / 2, -minimapViewportSize.y / 2);
    v = vec2Clampv(v, VEC2_ZERO, vec2Subv(minimapPanelSize, minimapViewportSize));
    return v;
}

inline rect rectScreenToMapCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;
    assert(map);

    rect mapPanel = map->mapPanel;
    rect viewport = map->viewport;

    r = rectTranslatef(r, -mapPanel.x, -mapPanel.y);
    r = rectTranslatef(r, viewport.x, viewport.y);
    return r;
}

inline vec2 vec2MapToScreenCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;
    assert(map);

    v = vec2Translatef(v, -map->viewport.x, -map->viewport.y);
    v = vec2Translatef(v, map->mapPanel.x, map->mapPanel.y);
    return v;
}

inline rect rectMapToScreenCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;
    assert(map);

    r = rectTranslatef(r, -map->viewport.x, -map->viewport.y);
    r = rectTranslatef(r, map->mapPanel.x, map->mapPanel.y);
    return r;
}

void createRoads(WarRoadPieceList *pieces, WarLevelConstruct *construct)
{
    s32 x1 = construct->x1;
    s32 y1 = construct->y1;
    s32 x2 = construct->x2;
    s32 y2 = construct->y2;
    u8 player = construct->player;

    s32 dx = x2 - x1;
    dx = sign(dx);

    s32 dy = y2 - y1;
    dy = sign(dy);
    
    s32 x = x1;
    s32 y = y1;

    WarRoadPiece piece;

    while (x != x2)
    {
        piece = (WarRoadPiece){0};
        piece.player = player;
        piece.tilex = x;
        piece.tiley = y;

        WarRoadPieceListAdd(pieces, piece);

        x += dx;
    }

    piece = (WarRoadPiece){0};
    piece.player = player;
    piece.tilex = x;
    piece.tiley = y;

    WarRoadPieceListAdd(pieces, piece);

    while (y != y2)
    {
        piece = (WarRoadPiece){0};
        piece.player = player;
        piece.tilex = x;
        piece.tiley = y;

        WarRoadPieceListAdd(pieces, piece);

        y += dy;
    }

    piece = (WarRoadPiece){0};
    piece.player = player;
    piece.tilex = x;
    piece.tiley = y;

    WarRoadPieceListAdd(pieces, piece);
}

void determineRoadTypes(WarRoadPieceList *pieces)
{
    s32 count = pieces->count;
    for(s32 i = 0; i < count; i++)
    {
        bool left = false, 
             top = false, 
             right = false, 
             bottom = false;

        for(s32 j = 0; j < count; j++)
        {
            if (i == j) continue;

            if (pieces->items[j].tilex == pieces->items[i].tilex - 1 && pieces->items[j].tiley == pieces->items[i].tiley)
                left = true;
            else if(pieces->items[j].tilex == pieces->items[i].tilex + 1 && pieces->items[j].tiley == pieces->items[i].tiley)
                right = true;
            else if (pieces->items[j].tilex == pieces->items[i].tilex && pieces->items[j].tiley == pieces->items[i].tiley - 1)
                top = true;
            else if (pieces->items[j].tilex == pieces->items[i].tilex && pieces->items[j].tiley == pieces->items[i].tiley + 1)
                bottom = true;
        }

        // Endpieces
        if (top && !bottom && !left && !right)
            pieces->items[i].type = WAR_ROAD_PIECE_BOTTOM;
        if (!top && bottom && !left && !right)
            pieces->items[i].type = WAR_ROAD_PIECE_TOP;
        if (!top && !bottom && !left && right)
            pieces->items[i].type = WAR_ROAD_PIECE_LEFT;
        if (!top && !bottom && left && !right)
            pieces->items[i].type = WAR_ROAD_PIECE_RIGHT;

        // Corner pieces
        if (top && !bottom && left && !right)
            pieces->items[i].type = WAR_ROAD_PIECE_TOP_LEFT;
        if (!top && bottom && left && !right)
            pieces->items[i].type = WAR_ROAD_PIECE_BOTTOM_LEFT;
        if (top && !bottom && !left && right)
            pieces->items[i].type = WAR_ROAD_PIECE_TOP_RIGHT;
        if (!top && bottom && !left && right)
            pieces->items[i].type = WAR_ROAD_PIECE_BOTTOM_RIGHT;

        // Middle pieces
        if (!top && !bottom && left && right)
            pieces->items[i].type = WAR_ROAD_PIECE_HORIZONTAL;
        if (top && bottom && !left && !right)
            pieces->items[i].type = WAR_ROAD_PIECE_VERTICAL;

        // Quad piece
        if (top && bottom && left && right)
            pieces->items[i].type = WAR_ROAD_PIECE_CROSS;

        // T-Corners
        if (top && bottom && left && !right)
            pieces->items[i].type = WAR_ROAD_PIECE_T_RIGHT;
        if (top && bottom && !left && right)
            pieces->items[i].type = WAR_ROAD_PIECE_T_LEFT;
        if (!top && bottom && left && right)
            pieces->items[i].type = WAR_ROAD_PIECE_T_TOP;
        if (top && !bottom && left && right)
            pieces->items[i].type = WAR_ROAD_PIECE_T_BOTTOM;
    }
}

void createMap(WarContext *context, s32 levelInfoIndex)
{
    WarResource *levelInfo = getOrCreateResource(context, levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

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

    context->map = map;

    // create the map sprite
    {
        WarResource *levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
        assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

        WarResource *tileset = getOrCreateResource(context, levelInfo->levelInfo.tilesetIndex);
        assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

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
            minimapFrames[i].data = (u8*)calloc(MINIMAP_WIDTH * MINIMAP_HEIGHT * 4, sizeof(u8));
        }
        
        for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
        {
            for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
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

                for(s32 i = 0; i < 2; i++)
                {
                    minimapFrames[i].data[index * 4 + 0] = (u8)r;
                    minimapFrames[i].data[index * 4 + 1] = (u8)g;
                    minimapFrames[i].data[index * 4 + 2] = (u8)b;
                    minimapFrames[i].data[index * 4 + 3] = 255;   
                }
            }
        }

        map->minimapSprite = createSpriteFrames(context, MINIMAP_WIDTH, MINIMAP_HEIGHT, 2, minimapFrames);
    }

    s32 entitiesCount = 0;

    // create the starting roads
    {
        WarRoadPieceList pieces;
        WarRoadPieceListInit(&pieces);

        for(s32 i = 0; i < levelInfo->levelInfo.startRoadsCount; i++)
        {
            WarLevelConstruct *construct = &levelInfo->levelInfo.startRoads[i];
            if (construct->type == WAR_CONSTRUCT_ROAD)
            {
                createRoads(&pieces, construct);
            }
        }

        determineRoadTypes(&pieces);

        WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_ROAD);
        addRoadComponent(context, entity, pieces);
        addSpriteComponent(context, entity, map->sprite);

        map->entities[entitiesCount++] = entity;
    }

    // create the starting entities
    {
        for(s32 i = 0; i < levelInfo->levelInfo.startEntitiesCount; i++)
        {
            WarLevelUnit unit = levelInfo->levelInfo.startEntities[i];

            WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_UNIT);
            addUnitComponent(context, entity, unit.type, unit.x, unit.y, unit.player, unit.resourceKind, unit.amount);
            addTransformComponent(context, entity, vec2i(unit.x * MEGA_TILE_WIDTH, unit.y * MEGA_TILE_HEIGHT));

            s32 spriteIndex = unitsData[unit.type * 4 + 1];
            if (spriteIndex == 0)
            {
                logError("Sprite for unit of type %d is not configure properly. Default to footman sprite.", unit.type);
                spriteIndex = 279;
            }
            addSpriteComponentFromResource(context, entity, spriteIndex);

            buildUnitActions(entity);
            setAction(context, entity, WAR_ACTION_TYPE_WALK, true);

            // addStateMachineComponent(context, entity);

            // WarState* idleState = createIdleState(true);
            // changeState(&entity->stateMachine, idleState);

            map->entities[entitiesCount++] = entity;
        }
    }

    // add ui entities
    {
        WarEntity *entity;
        
        // left top panel (minimap)
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->leftTopPanel.x, map->leftTopPanel.y});
        addSpriteComponentFromResource(context, entity, 224);
        map->entities[entitiesCount++] = entity;

        // left bottom panel
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->leftBottomPanel.x, map->leftBottomPanel.y});
        addSpriteComponentFromResource(context, entity, 226);
        map->entities[entitiesCount++] = entity;
        
        // top panel
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->topPanel.x, map->topPanel.y});
        addSpriteComponentFromResource(context, entity, 218);
        map->entities[entitiesCount++] = entity;

        // right panel
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->rightPanel.x, map->rightPanel.y});
        addSpriteComponentFromResource(context, entity, 220);
        map->entities[entitiesCount++] = entity;

        // bottom panel
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->bottomPanel.x, map->bottomPanel.y});
        addSpriteComponentFromResource(context, entity, 222);
        map->entities[entitiesCount++] = entity;
    }

    // set the initial state for the tiles
    {
        for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
        {
            map->tileStates[i] = MAP_TILE_STATE_UNKOWN;
        }
    }
}

void renderMap(WarContext *context)
{
    WarMap *map = context->map;
    if (!map) return;

    NVGcontext *gfx = context->gfx;

    WarResource *levelInfo = getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    nvgSave(gfx);

    nvgScale(gfx, context->globalScale, context->globalScale);

    // render map
    {
        nvgSave(gfx);

        nvgTranslate(gfx, map->mapPanel.x, map->mapPanel.y);
        nvgTranslate(gfx, -map->viewport.x, -map->viewport.y);
        
        // render terrain
        {
            WarResource *levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
            assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

            WarResource *tileset = getOrCreateResource(context, levelInfo->levelInfo.tilesetIndex);
            assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

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
            for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
            {
                WarEntity *entity = map->entities[i];
                if (entity && entity->type == WAR_ENTITY_TYPE_ROAD)
                {
                    renderEntity(context, entity, false);
                }
            }
        }

        // render units
        {
            for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
            {
                WarEntity *entity = map->entities[i];
                if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
                {
                    renderEntity(context, entity, map->selectedEntities[i]);
                }
            }
        }

        // DEBUG
        // render grid
        {
            // for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
            // {
            //     for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
            //     {
            //         nvgSave(gfx);

            //         nvgBeginPath(gfx);
            //         nvgRect(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            //         nvgStrokeWidth(gfx, 0.5f);
            //         nvgStrokeColor(gfx, NVG_WHITE);
            //         nvgStroke(gfx);

            //         nvgRestore(gfx);
            //     }
            // }
        }

        nvgRestore(gfx);
    }

    // render ui
    {
        nvgSave(gfx);

        // render selection rect
        {
            nvgSave(gfx);

            WarInput* input = &context->input;
            if (input->dragging)
            {
                rect pointerRect = rectpf(input->dragPos.x, input->dragPos.y, input->pos.x, input->pos.y);

                nvgBeginPath(gfx);
                nvgRect(gfx, pointerRect.x, pointerRect.y, pointerRect.width, pointerRect.height);
                nvgStrokeColor(gfx, NVG_GREEN_SELECTION);
                nvgStroke(gfx);
            }

            nvgRestore(gfx);
        }

        // render images
        {
            nvgSave(gfx);

            for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
            {
                WarEntity *entity = map->entities[i];
                if (entity && entity->type == WAR_ENTITY_TYPE_IMAGE)
                {
                    renderEntity(context, entity, false);
                }
            }

            nvgRestore(gfx);
        }
        
        // render minimap
        {
            nvgSave(gfx);

            // copy the minimap base to the first frame which is the one that will be rendered
            memcpy(map->minimapSprite.frames[0].data, map->minimapSprite.frames[1].data, MINIMAP_WIDTH * MINIMAP_HEIGHT * 4);

            for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
            {
                u8 r = 211, g = 211, b = 211;

                WarEntity* entity = map->entities[i];
                if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
                {
                    WarUnitComponent unit = entity->unit;

                    if (unit.type == WAR_UNIT_TOWNHALL_HUMANS || 
                        unit.type == WAR_UNIT_TOWNHALL_ORCS)
                    {
                        r = 255; g = 255; b = 0;
                    }
                    else if(unit.player == 0)
                    {
                        r = 0; g = 199; b = 0;
                    }
                    else if(unit.player < 4)
                    {
                        r = 199; g = 0; b = 0;
                    }

                    WarTransformComponent transform = entity->transform;
                    s32 tileX = (s32)(transform.position.x/MEGA_TILE_WIDTH);
                    s32 tileY = (s32)(transform.position.y/MEGA_TILE_HEIGHT);

                    for(s32 y = 0; y < unit.sizey; y++)
                    {
                        for(s32 x = 0; x < unit.sizex; x++)
                        {
                            s32 pixel = (tileY + y) * MINIMAP_WIDTH + (tileX + x);
                            map->minimapSprite.frames[0].data[pixel * 4 + 0] = r;
                            map->minimapSprite.frames[0].data[pixel * 4 + 1] = g;
                            map->minimapSprite.frames[0].data[pixel * 4 + 2] = b;
                        }
                    }
                }
            }

            nvgTranslate(gfx, map->minimapPanel.x, map->minimapPanel.y);

            updateSpriteImage(context, &map->minimapSprite, map->minimapSprite.frames[0].data);
            renderSprite(context, &map->minimapSprite, VEC2_ZERO, VEC2_ONE);

            nvgRestore(gfx);
        }

        // render minimap viewport
        {
            nvgSave(gfx);

            nvgTranslate(gfx, map->minimapPanel.x, map->minimapPanel.y);
            nvgTranslate(gfx, map->viewport.x * MINIMAP_MAP_WIDTH_RATIO, map->viewport.y * MINIMAP_MAP_HEIGHT_RATIO);

            nvgBeginPath(gfx);
            nvgRect(gfx, 0, 0, MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT);
            nvgStrokeWidth(gfx, 1.0f/context->globalScale);
            nvgStrokeColor(gfx, NVG_WHITE);
            nvgStroke(gfx);

            nvgRestore(gfx);
        }

        nvgRestore(gfx);
    }

    nvgRestore(gfx);
}