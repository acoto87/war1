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

internal inline void addEntityToSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    // subtitute this with a set data structure that doesn't allow duplicates
    if (!WarEntityIdListContains(&map->selectedEntities, id))
        WarEntityIdListAdd(&map->selectedEntities, id);
}

internal inline void removeEntityFromSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    WarEntityIdListRemove(&map->selectedEntities, id);
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

    map->finder = initPathFinder(PATH_FINDING_ASTAR, MAP_TILES_WIDTH, MAP_TILES_HEIGHT, levelPassable->levelPassable.data);

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
            minimapFrames[i].data = (u8*)xcalloc(MINIMAP_WIDTH * MINIMAP_HEIGHT * 4, sizeof(u8));
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

        map->minimapSprite = createSpriteFromFrames(context, MINIMAP_WIDTH, MINIMAP_HEIGHT, 2, minimapFrames);
    }

    // create the starting roads
    {
        WarRoadPieceList pieces;
        WarRoadPieceListInit(&pieces, WarRoadPieceListDefaultOptions);

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
    }

    // create the starting entities
    {
        for(s32 i = 0; i < levelInfo->levelInfo.startEntitiesCount; i++)
        {
            WarLevelUnit unit = levelInfo->levelInfo.startEntities[i];

            if (unit.type == WAR_UNIT_FOOTMAN)
            {
                unit.type = WAR_UNIT_RAIDER;
            }
            
            WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_UNIT);
            addUnitComponent(context, entity, unit.type, unit.x, unit.y, unit.player, unit.resourceKind, unit.amount);
            addTransformComponent(context, entity, vec2i(unit.x * MEGA_TILE_WIDTH, unit.y * MEGA_TILE_HEIGHT));

            WarUnitsData unitData = getUnitsData(unit.type);
            s32 spriteIndex = unitData.resourceIndex;
            if (spriteIndex == 0)
            {
                logError("Sprite for unit of type %d is not configure properly. Default to footman sprite.", unit.type);
                spriteIndex = 279;
            }
            addSpriteComponentFromResource(context, entity, spriteIndex);

            buildUnitActions(entity);

            if (isBuildingUnit(unit.type))
            {
                addAnimationsComponent(context, entity);

                entity->unit.maxhp = 100;
                entity->unit.hp = 100;

                setStaticEntity(map->finder, entity->unit.tilex, entity->unit.tiley, entity->unit.sizex, entity->unit.sizey);
            }

            addStateMachineComponent(context, entity);

            WarState* idleState = createIdleState(context, entity, isDudeUnit(unit.type));
            changeNextState(context, entity, idleState, true, true);
        }
    }

    // add ui entities
    {
        WarEntity *entity;
        
        // left top panel (minimap)
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->leftTopPanel.x, map->leftTopPanel.y});
        addSpriteComponentFromResource(context, entity, 224);

        // left bottom panel
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->leftBottomPanel.x, map->leftBottomPanel.y});
        addSpriteComponentFromResource(context, entity, 226);
        
        // top panel
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->topPanel.x, map->topPanel.y});
        addSpriteComponentFromResource(context, entity, 218);

        // right panel
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->rightPanel.x, map->rightPanel.y});
        addSpriteComponentFromResource(context, entity, 220);

        // bottom panel
        entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE);
        addTransformComponent(context, entity, (vec2){map->bottomPanel.x, map->bottomPanel.y});
        addSpriteComponentFromResource(context, entity, 222);
    }

    // set the initial state for the tiles
    {
        for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
        {
            map->tileStates[i] = MAP_TILE_STATE_UNKOWN;
        }
    }
}

internal void updateViewport(WarContext *context)
{
    WarMap *map = context->map;
    WarInput *input = &context->input;

    vec2 dir = VEC2_ZERO;

    if (isKeyPressed(input, WAR_KEY_LEFT))
        dir.x = -1;
    else if (isKeyPressed(input, WAR_KEY_RIGHT))
        dir.x = 1;

    if (isKeyPressed(input, WAR_KEY_DOWN))
        dir.y = 1;
    else if (isKeyPressed(input, WAR_KEY_UP))
        dir.y = -1;

    dir = vec2Normalize(dir);

    map->viewport.x += map->scrollSpeed * dir.x * context->deltaTime;
    map->viewport.x = clamp(map->viewport.x, 0.0f, MAP_WIDTH - map->viewport.width);

    map->viewport.y += map->scrollSpeed * dir.y * context->deltaTime;
    map->viewport.y = clamp(map->viewport.y, 0.0f, MAP_HEIGHT - map->viewport.height);
}

void updateMap(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    WarInput* input = &context->input;

    updateViewport(context);

    // process all state machines
    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
        {
            updateStateMachine(context, entity);
        }
    }

    // update all actions
    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
        {
            updateAction(context, entity);
        }
    }

    // update all animations
    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* entity = map->entities.items[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
        {
            updateAnimations(context, entity);
        }
    }

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
        // check if the click is inside the map panel
        else if(rectContainsf(mapPanel, input->pos.x, input->pos.y))
        {
            if (!input->dragging)
            {
                input->dragPos = input->pos;
                input->dragging = true;
            }
        }
    }
    else if(wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        rect minimapPanel = map->minimapPanel;
        rect mapPanel = map->mapPanel;

        // check if the click is inside the map panel
        if(input->dragging || rectContainsf(mapPanel, input->pos.x, input->pos.y))
        {
            rect pointerRect = rectpf(input->dragPos.x, input->dragPos.y, input->pos.x, input->pos.y);
            pointerRect = rectScreenToMapCoordinates(context, pointerRect);

            for(s32 i = 0; i < map->entities.count; i++)
            {
                WarEntity* entity = map->entities.items[i];
                if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
                {
                    WarTransformComponent transform = entity->transform;
                    WarUnitComponent unit = entity->unit;
                    if (transform.enabled && unit.enabled)
                    {
                        rect unitRect = rectv(transform.position, getUnitSpriteSize(entity));

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

        input->dragging = false;
    }

    if (wasButtonPressed(input, WAR_MOUSE_RIGHT))
    {
        WarEntity* selEntity = NULL;

        if (map->selectedEntities.count > 0)
        {
            WarEntityId selEntityId = map->selectedEntities.items[0];
            s32 selEntityIndex = findEntity(context, selEntityId);
            if (selEntityIndex >= 0)
                selEntity = map->entities.items[selEntityIndex];
        }

        if (selEntity)
        {
            WarUnitComponent* unit = &selEntity->unit;
            if (isDudeUnit(unit->type))
            {
                WarTransformComponent* transform = &selEntity->transform;
                vec2 unitCenterPoint = vec2Mulf(getUnitSpriteSize(selEntity), 0.5f);
                vec2 position = vec2Addv(transform->position, unitCenterPoint);
                vec2 target = vec2ScreenToMapCoordinates(context, input->pos);

                position = vec2MapToTileCoordinates(position);
                target = vec2MapToTileCoordinates(target);
                
                WarMapPath path = findPath(map->finder, (s32)position.x, (s32)position.y, (s32)target.x, (s32)target.y);
                if (path.nodes.count > 0)
                {
                    // WarState* moveState = createMoveState(context, selEntity, path);
                    // changeNextState(context, selEntity, moveState, true, true);

                    WarState* patrolState = createPatrolState(context, selEntity, path);
                    changeNextState(context, selEntity, patrolState, true, true);
                }
                else
                {
                    freePath(path);
                }
            }

            if (isBuildingUnit(unit->type))
            {
                unit->hp -= (s32)(unit->maxhp * 0.1f);
                if (unit->hp < 0) unit->hp = 0;

                f32 p = (f32)unit->hp / unit->maxhp;
                if (p <= 0)
                {
                    if (!containsAnimation(context, selEntity, "collapse"))
                    {
                        removeAnimation(context, selEntity, "hugeDamage");

                        selEntity->sprite.enabled = false;

                        vec2 frameSize = getUnitFrameSize(selEntity);
                        vec2 unitSize = getUnitSpriteSize(selEntity);

                        WarSprite sprite = createSpriteFromResourceIndex(context, BUILDING_COLLAPSE_RESOURCE);
                        WarSpriteAnimation* anim = createAnimation("collapse", sprite, 0.1f, false);

                        vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

                        // this is the scale of the explosion animation sprites with respect to the size of the building
                        f32 animScale = unitSize.x / animFrameSize.x;

                        // if the offset is based on the size of the frame, and it's scaled, then the offset must take into
                        // account the scale to make the calculations
                        f32 offsetx = halff(frameSize.x - unitSize.x);
                        f32 offsety = halff(frameSize.y - unitSize.y) - (animFrameSize.y * animScale - unitSize.y);

                        anim->scale = vec2f(animScale, animScale);
                        anim->offset = vec2f(offsetx, offsety);

                        for(s32 i = 0; i < 17; i++)
                            addAnimationFrame(anim, i);
                        
                        addAnimation(selEntity, anim);

                        // deselect the entity and remove it!
                        removeEntityFromSelection(context, selEntity->id);

                        // if the entity is removed here, then the collapse animation doesn't show
                        // this is because the animation is tied to the entity
                        // for this two solutions:
                        //   1. delay the removing of the entity until the animation is finished
                        //   2. the collapse animation is not tied to the entity
                        // I need to know when the animation finish to spawn a ruins entity,
                        // or the ruins entity spawn at this same moment.
                        //
                        // removeEntityById(context, selEntity->id);
                    }
                }
                else if (p < 0.3f)
                {
                    if (!containsAnimation(context, selEntity, "hugeDamage"))
                    {
                        removeAnimation(context, selEntity, "littleDamage");

                        WarSprite sprite = createSpriteFromResourceIndex(context, BUILDING_DAMAGE_2_RESOURCE);
                        WarSpriteAnimation* anim = createAnimation("hugeDamage", sprite, 0.2f, true);
                        anim->offset = vec2Subv(getUnitSpriteCenter(selEntity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));
                        
                        for(s32 i = 0; i < 4; i++)
                            addAnimationFrame(anim, i);
                        
                        addAnimation(selEntity, anim);
                    }
                }
                else if(p < 0.7f)
                {
                    if (!containsAnimation(context, selEntity, "littleDamage"))
                    {
                        WarSprite sprite = createSpriteFromResourceIndex(context, BUILDING_DAMAGE_1_RESOURCE);
                        WarSpriteAnimation* anim = createAnimation("littleDamage", sprite, 0.2f, true);
                        anim->offset = vec2Subv(getUnitSpriteCenter(selEntity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));
                        
                        for(s32 i = 0; i < 4; i++)
                            addAnimationFrame(anim, i);
                        
                        addAnimation(selEntity, anim);
                    }
                }
                
            }
        }
    }
}

void renderMap(WarContext *context)
{
    WarMap *map = context->map;
    if (!map) return;

    NVGcontext* gfx = context->gfx;

    WarResource* levelInfo = getOrCreateResource(context, map->levelInfoIndex);
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
            WarResource* levelVisual = getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
            assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

            WarResource* tileset = getOrCreateResource(context, levelInfo->levelInfo.tilesetIndex);
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
            for(s32 i = 0; i < map->entities.count; i++)
            {
                WarEntity *entity = map->entities.items[i];
                if (entity && entity->type == WAR_ENTITY_TYPE_ROAD)
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
                if (currentState)
                {
                    s32 index = -1;
                    WarMapPath path;
                    
                    if (currentState->type == WAR_STATE_MOVE)
                    {
                        index = currentState->move.currentIndex;
                        path = stateMachine->currentState->move.path;
                    }
                    else if(currentState->type == WAR_STATE_PATROL)
                    {
                        index = currentState->patrol.currentIndex;
                        path = stateMachine->currentState->patrol.path;
                    }
                    
                    if (index >= 0)
                    {
                        vec2 prevPos;
                        for(s32 i = 0; i < path.nodes.count; i++)
                        {
                            vec2 pos = path.nodes.items[i];
                            pos.x = pos.x * MEGA_TILE_WIDTH + halfi(MEGA_TILE_WIDTH);
                            pos.y = pos.y * MEGA_TILE_HEIGHT + halfi(MEGA_TILE_HEIGHT);

                            if (i > 0)
                                nvgStrokeLine(gfx, prevPos, pos, getColorFromList(entity->id), 0.5f);

                            nvgFillRect(gfx, rectv(pos, VEC2_ONE), i == index ? nvgRGB(255, 0, 255) : nvgRGB(255, 255, 0));

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
                if (map->finder.data[y * MAP_TILES_WIDTH + x])
                {
                    vec2 pos = vec2i(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);
                    vec2 size = vec2i(MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                    nvgFillRect(gfx, rectv(pos, size), nvgRGBA(200, 0, 0, 100));
                }
            }
        }
#endif

#ifdef DEBUG_RENDER_MAP_GRID
        for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
        {
            for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
            {
                nvgSave(gfx);

                nvgBeginPath(gfx);
                nvgRect(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgStrokeWidth(gfx, 0.5f);
                nvgStrokeColor(gfx, NVG_WHITE);
                nvgStroke(gfx);

                nvgRestore(gfx);
            }
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

            for(s32 i = 0; i < map->entities.count; i++)
            {
                WarEntity *entity = map->entities.items[i];
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

            for(s32 i = 0; i < map->entities.count; i++)
            {
                u8 r = 211, g = 211, b = 211;

                WarEntity* entity = map->entities.items[i];
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