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

    WarResource *resource = context->resources[levelInfoIndex];
    assert(resource && resource->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarMap *map = (WarMap*)xcalloc(1, sizeof(WarMap));
    map->levelInfoIndex = levelInfoIndex;
    map->tilesetType = MAP_TILESET_FOREST;
    map->scrollSpeed = 200;
    
    map->leftTopPanel = (Rect){0, 128, 72, 72};
    map->leftBottomPanel = (Rect){0, 0, 72, 128};
    map->rightPanel = (Rect){312, 0, 8, 200};
    map->topPanel = (Rect){72, 188, 240, 12};
    map->bottomPanel = (Rect){72, 0, 240, 12};
    map->mapPanel = (Rect){72, 12, 240, 176};
    map->minimapPanel = (Rect){0, 0, 128, 128};

    context->map = map;

    WarResource *levelInfo = context->resources[map->levelInfoIndex];
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    // create the map sprite
    {
        WarResource *levelVisual = context->resources[levelInfo->levelInfo.visualIndex];
        assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);
        
        WarVertex vertices[MAP_WIDTH * MAP_HEIGHT * 4];
        GLuint indices[MAP_WIDTH * MAP_HEIGHT * 6];

        for(s32 y = 0; y < MAP_HEIGHT; y++)
        {
            for(s32 x = 0; x < MAP_WIDTH; x++)
            {
                s32 index = y * MAP_WIDTH + x;
                u16 tileIndex = levelVisual->levelVisual.data[index];

                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                // vertex 00
                vertices[index * 4 + 0].position[0] = (f32)(x + 0) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 0].position[1] = (f32)(MAP_HEIGHT - 1 - y + 0) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 0].texCoords[0] = (f32)(tilePixelX) / TILESET_WIDTH_PX;
                vertices[index * 4 + 0].texCoords[1] = (f32)(tilePixelY + MEGA_TILE_HEIGHT) / TILESET_HEIGHT_PX;

                // vertex 10
                vertices[index * 4 + 1].position[0] = (f32)(x + 1) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 1].position[1] = (f32)(MAP_HEIGHT - 1 - y + 0) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 1].texCoords[0] = (f32)(tilePixelX + MEGA_TILE_WIDTH) / TILESET_WIDTH_PX;
                vertices[index * 4 + 1].texCoords[1] = (f32)(tilePixelY + MEGA_TILE_HEIGHT) / TILESET_HEIGHT_PX;

                // vertex 11
                vertices[index * 4 + 2].position[0] = (f32)(x + 1) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 2].position[1] = (f32)(MAP_HEIGHT - 1 - y + 1) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 2].texCoords[0] = (f32)(tilePixelX + MEGA_TILE_WIDTH) / TILESET_WIDTH_PX;
                vertices[index * 4 + 2].texCoords[1] = (f32)(tilePixelY) / TILESET_HEIGHT_PX;

                // vertex 01
                vertices[index * 4 + 3].position[0] = (f32)(x + 0) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 3].position[1] = (f32)(MAP_HEIGHT - 1 - y + 1) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 3].texCoords[0] = (f32)(tilePixelX) / TILESET_WIDTH_PX;
                vertices[index * 4 + 3].texCoords[1] = (f32)(tilePixelY) / TILESET_HEIGHT_PX;

                // indices for 1st triangle
                indices[index * 6 + 0] = index * 4 + 0;
                indices[index * 6 + 1] = index * 4 + 1;
                indices[index * 6 + 2] = index * 4 + 2;

                // indices for 2nd triangle
                indices[index * 6 + 3] = index * 4 + 0;
                indices[index * 6 + 4] = index * 4 + 2;
                indices[index * 6 + 5] = index * 4 + 3;
            }
        }

        // map->sprite = createSprite(context, 
        //     MAP_WIDTH * MAP_HEIGHT, 
        //     TILESET_WIDTH_PX, 
        //     TILESET_HEIGHT_PX, 
        //     vertices, 
        //     MAP_WIDTH * MAP_HEIGHT * 4, 
        //     indices, 
        //     MAP_WIDTH * MAP_HEIGHT * 6,
        //     -1);
    }

    s32 entitiesCount = 0;

    // create the starting roads
    {
        WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_ROAD);

        for(s32 i = 0; i < levelInfo->levelInfo.startRoadsCount; i++)
        {
            WarLevelConstruct *construct = &levelInfo->levelInfo.startRoads[i];
            if (construct->type == WAR_CONSTRUCT_ROAD)
            {
                createRoads(&entity->road.pieces, construct);
            }
        }

        determineRoadTypes(&entity->road.pieces);
        addRoadComponent(context, entity);

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
        
        // left bottom panel
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->leftBottomPanel.x, map->leftBottomPanel.y});
        addSpriteComponentFromResource(context, entity, 226);
        map->entities[entitiesCount++] = entity;

        // left top panel (minimap)
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->leftTopPanel.x, map->leftTopPanel.y});
        addSpriteComponentFromResource(context, entity, 224);
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
        for(s32 i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
        {
            map->tileStates[i] = MAP_TILE_STATE_UNKOWN;
        }
    }
}

void renderMap(WarContext *context)
{
    WarMap *map = context->map;
    if (!map) return;

    WarResource *levelInfo = context->resources[map->levelInfoIndex];
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    mat4 baseTransform;
    glm_mat4_identity(baseTransform);
    glm_scale(baseTransform, (vec3){context->globalScale, context->globalScale, 1.0f});

    // render map
    {
        mat4 mapTransform;
        glm_mat4_identity(mapTransform);
        glm_mat4_mul(baseTransform, mapTransform, mapTransform);
        glm_translate(mapTransform, (vec4){map->mapPanel.x, map->mapPanel.y, 0.0f, 0.0f});
        glm_translate(mapTransform, (vec4){map->pos[0], map->pos[1], 0.0f, 0.0f});
        
        // render terrain
        {
            WarResource *tileset = context->resources[levelInfo->levelInfo.tilesetIndex];
            assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

            // renderSprite(context, &map->sprite, tileset->tilesetData.data, mapTransform);
        }

        // render roads
        {
            for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
            {
                WarEntity *entity = map->entities[i];
                if (entity && entity->type == WAR_ENTITY_TYPE_ROAD)
                {
                    renderEntity(context, entity, mapTransform);
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
                    renderEntity(context, entity, mapTransform);
                }
            }
        }
    }

    // render ui
    {
        mat4 uiTransform;
        glm_mat4_identity(uiTransform);
        glm_mat4_mul(baseTransform, uiTransform, uiTransform);

        for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
        {
            WarEntity *entity = map->entities[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_IMAGE)
            {
                renderEntity(context, entity, uiTransform);
            }
        }
    }
}