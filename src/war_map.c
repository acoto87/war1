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
    assert(levelInfoIndex >= 0 && levelInfoIndex < MAX_RESOURCES_COUNT);

    WarResource *resource = getOrCreateResource(context, levelInfoIndex);
    assert(resource && resource->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

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

    map->viewport = recti(0, 0, MAP_VIEWPORT_WIDTH, MAP_VIEWPORT_HEIGHT);

    context->map = map;

    WarResource *levelInfo = getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    // create the map sprite
    {
        WarResource *tileset = getOrCreateResource(context, levelInfo->levelInfo.tilesetIndex);
        assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

        map->sprite = createSprite(context, TILESET_WIDTH, TILESET_HEIGHT, tileset->tilesetData.data);
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

        map->entities[entitiesCount++] = entity;
    }

    // create the starting entities
    {
        for(s32 i = 0; i < levelInfo->levelInfo.startEntitiesCount; i++)
        {
            WarLevelUnit unit = levelInfo->levelInfo.startEntities[i];

            WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_UNIT);
            addUnitComponent(context, entity, unit.type, unit.x, unit.y, unit.player, unit.value);

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
                    s32 index = y * MAP_TILES_WIDTH + x;
                    u16 tileIndex = levelVisual->levelVisual.data[index];

                    s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                    s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                    nvgSave(gfx);
                    nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

                    nvgRenderBatchImage(gfx, batch, 
                        tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT,
                        0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);

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

        for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
        {
            WarEntity *entity = map->entities[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_IMAGE)
            {
                renderEntity(context, entity, false);
            }
        }

        // DEBUG
        // render minimap position
        {
            // nvgSave(gfx);

            // nvgTranslate(gfx, map->minimapPanel.x, map->minimapPanel.y);

            // nvgBeginPath(gfx);
            // nvgRect(gfx, 0, 0, MINIMAP_WIDTH, MINIMAP_HEIGHT);
            // nvgFillColor(gfx, NVG_GRAY_TRANSPARENT);
            // nvgFill(gfx);

            // nvgRestore(gfx);
        }

        // render minimap
        {
            // nvgSave(gfx);

            // WarResource *levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
            // assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

            // WarResource *tileset = getOrCreateResource(context, levelInfo->levelInfo.tilesetIndex);
            // assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

            // u8 *minimapData = (u8*)calloc(MINIMAP_WIDTH * MINIMAP_HEIGHT * 4, sizeof(u8));
            // for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
            // {
            //     for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
            //     {
            //         s32 index = y * MAP_TILES_WIDTH + x;
            //         u16 tileIndex = levelVisual->levelVisual.data[index];

            //         s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            //         s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            //         nvgSave(gfx);
            //         nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);
            //         renderSubSprite(context, &map->sprite, NULL, tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            //         nvgRestore(gfx);
            //     }
            // }

            // renderSprite(context, &map->minimapSprite, minimapData);

            // nvgRestore(gfx);
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

inline vec2 vec2ScreenToMapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;
    assert(map);

    f32 scale = context->globalScale;

    rect mapPanel = rectScalef(map->mapPanel, scale);
    rect viewport = rectScalef(map->viewport, scale);

    v = vec2Translatef(v, -mapPanel.x, -mapPanel.y);
    v = vec2Translatef(v, viewport.x, viewport.y);
    return v;
}

inline vec2 vec2ScreenToMinimapCoordinates(WarContext* context, vec2 v)
{
    WarMap* map = context->map;
    assert(map);

    f32 scale = context->globalScale;

    rect minimapPanel = rectScalef(map->minimapPanel, scale);
    vec2 minimapPanelSize = vec2i(minimapPanel.width, minimapPanel.height);

    vec2 minimapViewportSize = vec2i(MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT);
    minimapViewportSize = vec2Scalef(minimapViewportSize, scale);

    v = vec2Translatef(v, -minimapPanel.x, -minimapPanel.y);
    v = vec2Translatef(v, -minimapViewportSize.x / 2, -minimapViewportSize.y / 2);
    v = vec2Clampv(v, VEC2_ZERO, vec2Subv(minimapPanelSize, minimapViewportSize));
    return v;
}

inline rect rectScreenToMapCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;
    assert(map);

    f32 scale = context->globalScale;
    
    rect mapPanel = rectScalef(map->mapPanel, scale);
    rect viewport = rectScalef(map->viewport, scale);

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
    v = vec2Scalef(v, context->globalScale);
    return v;
}

inline rect rectMapToScreenCoordinates(WarContext* context, rect r)
{
    WarMap* map = context->map;
    assert(map);

    r = rectTranslatef(r, -map->viewport.x, -map->viewport.y);
    r = rectTranslatef(r, map->mapPanel.x, map->mapPanel.y);
    r = rectScalef(r, context->globalScale);
    return r;
}