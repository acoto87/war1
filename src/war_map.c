WarRoadPiece* createRoads(WarRoadPiece *pieces, WarLevelConstruct *construct)
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

        sb_push(pieces, piece);

        x += dx;
    }

    piece = (WarRoadPiece){0};
    piece.player = player;
    piece.tilex = x;
    piece.tiley = y;

    sb_push(pieces, piece);

    while (y != y2)
    {
        piece = (WarRoadPiece){0};
        piece.player = player;
        piece.tilex = x;
        piece.tiley = y;

        sb_push(pieces, piece);

        y += dy;
    }

    piece = (WarRoadPiece){0};
    piece.player = player;
    piece.tilex = x;
    piece.tiley = y;

    sb_push(pieces, piece);

    return pieces;
}

void determineRoadTypes(WarRoadPiece *pieces)
{
    s32 count = sb_count(pieces);
    for(s32 i = 0; i < count; i++)
    {
        bool left = false, 
             top = false, 
             right = false, 
             bottom = false;

        for(s32 j = 0; j < count; j++)
        {
            if (i == j) continue;

            if (pieces[j].tilex == pieces[i].tilex - 1 && pieces[j].tiley == pieces[i].tiley)
                left = true;
            else if(pieces[j].tilex == pieces[i].tilex + 1 && pieces[j].tiley == pieces[i].tiley)
                right = true;
            else if (pieces[j].tilex == pieces[i].tilex && pieces[j].tiley == pieces[i].tiley - 1)
                top = true;
            else if (pieces[j].tilex == pieces[i].tilex && pieces[j].tiley == pieces[i].tiley + 1)
                bottom = true;
        }

        // Endpieces
        if (top && !bottom && !left && !right)
            pieces[i].type = WAR_ROAD_PIECE_BOTTOM;
        if (!top && bottom && !left && !right)
            pieces[i].type = WAR_ROAD_PIECE_TOP;
        if (!top && !bottom && !left && right)
            pieces[i].type = WAR_ROAD_PIECE_LEFT;
        if (!top && !bottom && left && !right)
            pieces[i].type = WAR_ROAD_PIECE_RIGHT;

        // Corner pieces
        if (top && !bottom && left && !right)
            pieces[i].type = WAR_ROAD_PIECE_TOP_LEFT;
        if (!top && bottom && left && !right)
            pieces[i].type = WAR_ROAD_PIECE_BOTTOM_LEFT;
        if (top && !bottom && !left && right)
            pieces[i].type = WAR_ROAD_PIECE_TOP_RIGHT;
        if (!top && bottom && !left && right)
            pieces[i].type = WAR_ROAD_PIECE_BOTTOM_RIGHT;

        // Middle pieces
        if (!top && !bottom && left && right)
            pieces[i].type = WAR_ROAD_PIECE_HORIZONTAL;
        if (top && bottom && !left && !right)
            pieces[i].type = WAR_ROAD_PIECE_VERTICAL;

        // Quad piece
        if (top && bottom && left && right)
            pieces[i].type = WAR_ROAD_PIECE_CROSS;

        // T-Corners
        if (top && bottom && left && !right)
            pieces[i].type = WAR_ROAD_PIECE_T_RIGHT;
        if (top && bottom && !left && right)
            pieces[i].type = WAR_ROAD_PIECE_T_LEFT;
        if (!top && bottom && left && right)
            pieces[i].type = WAR_ROAD_PIECE_T_TOP;
        if (top && !bottom && left && right)
            pieces[i].type = WAR_ROAD_PIECE_T_BOTTOM;
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

        map->sprite = createSprite(context, 
            MAP_WIDTH * MAP_HEIGHT, 
            TILESET_WIDTH_PX, 
            TILESET_HEIGHT_PX, 
            vertices, 
            MAP_WIDTH * MAP_HEIGHT * 4, 
            indices, 
            MAP_WIDTH * MAP_HEIGHT * 6,
            -1);
    }

    s32 entitiesCount = 0;

    // create the starting roads
    {
        WarRoadPiece *pieces = null;

        for(s32 i = 0; i < levelInfo->levelInfo.startRoadsCount; i++)
        {
            WarLevelConstruct *construct = &levelInfo->levelInfo.startRoads[i];
            if (construct->type == WAR_CONSTRUCT_ROAD)
            {
                pieces = createRoads(pieces, construct);
            }
        }

        determineRoadTypes(pieces);

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

    // set the initial state for the tiles
    {
        for(s32 i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
        {
            map->tileStates[i] = MAP_TILE_STATE_UNKOWN;
        }
    }

}
