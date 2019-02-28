void addTransformComponent(WarContext* context, WarEntity* entity, vec2 position)
{
    entity->transform = (WarTransformComponent){0};
    entity->transform.enabled = true;
    entity->transform.position = position;
    entity->transform.rotation = VEC2_ZERO;
    entity->transform.scale = VEC2_ONE;
}

void removeTransformComponent(WarContext* context, WarEntity* entity)
{
    entity->transform = (WarTransformComponent){0};
}

void addSpriteComponent(WarContext* context, WarEntity* entity, WarSprite sprite)
{
    entity->sprite = (WarSpriteComponent){0};
    entity->sprite.enabled = true;
    entity->sprite.frameIndex = 0;
    entity->sprite.resourceIndex = 0;
    entity->sprite.sprite = sprite;
}

void addSpriteComponentFromResource(WarContext* context, WarEntity* entity, s32 resourceIndex)
{
    WarSprite sprite = createSpriteFromResourceIndex(context, resourceIndex);
    addSpriteComponent(context, entity, sprite);
    entity->sprite.resourceIndex = resourceIndex;
}

void removeSpriteComponent(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    for (s32 i = 0; i < sprite->sprite.framesCount; i++)
    {
        u8* data = sprite->sprite.frames[i].data;
        if (data)
            free(data);
    }

    nvgDeleteImage(context->gfx, sprite->sprite.image);

    *sprite = (WarSpriteComponent){0};
}

void addUnitComponent(WarContext* context, WarEntity* entity, WarUnitType type, s32 x, s32 y, u8 player, WarResourceKind resourceKind, u32 amount)
{
    WarUnitsData unitData = getUnitsData(type);

    entity->unit = (WarUnitComponent){0};
    entity->unit.enabled = true;
    entity->unit.type = type;
    entity->unit.direction = rand() % WAR_DIRECTION_COUNT;
    entity->unit.tilex = x;
    entity->unit.tiley = y;
    entity->unit.sizex = unitData.sizex;
    entity->unit.sizey = unitData.sizey;
    entity->unit.player = player;
    entity->unit.resourceKind = resourceKind;
    entity->unit.amount = amount;
    entity->unit.level = 0;
    entity->unit.hp = 0;
    entity->unit.maxhp = 0;
    entity->unit.armour = 0;
    entity->unit.range = 0;
    entity->unit.minDamage = 0;
    entity->unit.rndDamage = 0;
    entity->unit.decay = 0;
    entity->unit.actionIndex = 0;

    WarUnitActionListInit(&entity->unit.actions, WarUnitActionListDefaultOptions);
}

void removeUnitComponent(WarContext* context, WarEntity* entity)
{
    WarUnitActionListFree(&entity->unit.actions);
    entity->unit = (WarUnitComponent){0};
}

void addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces)
{
    entity->road = (WarRoadComponent){0};
    entity->road.enabled = true;
    entity->road.pieces = pieces;
}

void removeRoadComponent(WarContext* context, WarEntity* entity)
{
    WarRoadPieceListFree(&entity->road.pieces);
    entity->road = (WarRoadComponent){0};
}

void addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces)
{
    entity->wall = (WarWallComponent){0};
    entity->wall.enabled = true;
    entity->wall.pieces = pieces;
}

void removeWallComponent(WarContext* context, WarEntity* entity)
{
    WarWallPieceListFree(&entity->wall.pieces);
    entity->wall = (WarWallComponent){0};
}

void addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces)
{
    entity->ruin = (WarRuinComponent){0};
    entity->ruin.enabled = true;
    entity->ruin.pieces = pieces;
}

void removeRuinComponent(WarContext* context, WarEntity* entity)
{
    WarRuinPieceListFree(&entity->ruin.pieces);
    entity->ruin = (WarRuinComponent){0};
}

void addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees)
{
    entity->forest = (WarForestComponent){0};
    entity->forest.enabled = true;
    entity->forest.trees = trees;
}

void removeForestComponent(WarContext* context, WarEntity* entity)
{
    WarTreeListFree(&entity->forest.trees);
    entity->forest = (WarForestComponent){0};
}

void addStateMachineComponent(WarContext* context, WarEntity* entity)
{
    entity->stateMachine = (WarStateMachineComponent){0};
    entity->stateMachine.enabled = true;
    entity->stateMachine.currentState = NULL;
    entity->stateMachine.nextState = NULL;
}

void removeStateMachineComponent(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;

    if (stateMachine->currentState)
        leaveState(context, entity, stateMachine->currentState);

    if (stateMachine->nextState)
        leaveState(context, entity, stateMachine->nextState);

    entity->stateMachine = (WarStateMachineComponent){0};
}

void addAnimationsComponent(WarContext* context, WarEntity* entity)
{
    entity->animations = (WarAnimationsComponent){0};
    entity->animations.enabled = true;
    WarSpriteAnimationListInit(&entity->animations.animations, WarSpriteAnimationListDefaultOptions);
}

void removeAnimationsComponent(WarContext* context, WarEntity* entity)
{
    WarSpriteAnimationListFree(&entity->animations.animations);
    entity->animations = (WarAnimationsComponent){0};
}

// Entities
WarEntity* createEntity(WarContext* context, WarEntityType type)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* entity = (WarEntity *)xcalloc(1, sizeof(WarEntity));
    entity->id = ++context->staticEntityId;
    entity->type = type;
    entity->enabled = true;

    // transform component
    entity->transform = (WarTransformComponent){0};
    entity->sprite = (WarSpriteComponent){0};
    entity->unit = (WarUnitComponent){0};
    entity->road = (WarRoadComponent){0};
    entity->wall = (WarWallComponent){0};
    entity->forest = (WarForestComponent){0};
    entity->stateMachine = (WarStateMachineComponent){0};
    entity->animations = (WarAnimationsComponent){0};

    WarEntityListAdd(&map->entities, entity);

    return entity;
}

s32 findEntityIndex(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    for (s32 i = 0; i < map->entities.count; i++)
    {
        if (map->entities.items[i]->id == id)
            return i;
    }

    return -1;
}

WarEntity* findEntity(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    for (s32 i = 0; i < map->entities.count; i++)
    {
        if (map->entities.items[i]->id == id)
            return map->entities.items[i];
    }

    return NULL;
}

WarEntity* findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* result = NULL;
    f32 minDst = INT32_MAX;

    for (s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity* target = map->entities.items[i];
        if (isUnitOfType(target, type))
        {
            s32 dst = unitDistanceInTiles(entity, target);
            if (dst < minDst)
            {
                result = target;
                minDst = dst;
            }
        }
    }

    return result;
}

void removeEntityById(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    assert(map);

    s32 index = findEntityIndex(context, id);
    assert(index >= 0);

    WarEntity* entity = map->entities.items[index];

    removeTransformComponent(context, entity);
    removeSpriteComponent(context, entity);
    removeRoadComponent(context, entity);
    removeWallComponent(context, entity);
    removeRuinComponent(context, entity);
    removeForestComponent(context, entity);
    removeUnitComponent(context, entity);
    removeStateMachineComponent(context, entity);
    removeAnimationsComponent(context, entity);

    WarEntityListRemoveAt(&map->entities, index);
}

// Render entities
void _renderImage(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;

    if (sprite->enabled)
    {
        WarSpriteFrame* frame = getSpriteFrame(context, sprite);

        nvgTranslate(gfx, -frame->dx, -frame->dy);
        nvgTranslate(gfx, transform.position.x, transform.position.y);
        renderSprite(context, &sprite->sprite, VEC2_ZERO, VEC2_ONE);
    }
}

void _renderRoad(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarSpriteComponent* sprite = &entity->sprite;
    WarRoadComponent* road = &entity->road;

    // the roads are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled)
    {
        if (road->enabled)
        {
            WarRoadPieceList* pieces = &road->pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, road->pieces.count);

            for (s32 i = 0; i < pieces->count; i++)
            {
                // get the index of the tile in the spritesheet of the map,
                // corresponding to the current tileset type (forest, swamp)
                WarRoadsData roadsData = getRoadsData(pieces->items[i].type);

                s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? roadsData.tileIndexForest : roadsData.tileIndexSwamp;
                
                // the position in the world of the road piece tile
                s32 x = pieces->items[i].tilex;
                s32 y = pieces->items[i].tiley;

                // coordinates in pixels of the road piece tile
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                nvgSave(gfx);
                nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

                nvgRestore(gfx);
            }

            nvgEndImageBatch(gfx, batch);
        }
    }
}

void _renderWall(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarSpriteComponent* sprite = &entity->sprite;
    WarWallComponent* wall = &entity->wall;

    // the walls are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled)
    {
        if (wall->enabled)
        {
            WarWallPieceList* pieces = &wall->pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, wall->pieces.count);

            for (s32 i = 0; i < pieces->count; i++)
            {
                WarWallPiece* piece = &pieces->items[i];

                // get the index of the tile in the spritesheet of the map,
                // corresponding to the current tileset type (forest, swamp)
                WarWallsData wallsData = getWallsData(piece->type);

                s32 tileIndex = 0;

                s32 hpPercent = percentabi(piece->hp, piece->maxhp);
                if (hpPercent <= 0)
                {
                    tileIndex = (tilesetType == MAP_TILESET_FOREST)
                        ? wallsData.tileDestroyedForest : wallsData.tileDestroyedSwamp;
                }
                else if(hpPercent < 50)
                {
                    tileIndex = (tilesetType == MAP_TILESET_FOREST)
                        ? wallsData.tileDamagedForest : wallsData.tileDamagedSwamp;
                }
                else
                {
                    tileIndex = (tilesetType == MAP_TILESET_FOREST)
                        ? wallsData.tileForest : wallsData.tileSwamp;
                }

                // the position in the world of the wall piece tile
                s32 x = piece->tilex;
                s32 y = piece->tiley;

                // coordinates in pixels of the wall piece tile
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                nvgSave(gfx);
                nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

                nvgRestore(gfx);
            }

            nvgEndImageBatch(gfx, batch);
        }
    }
}

void _renderRuin(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarSpriteComponent* sprite = &entity->sprite;
    WarRuinComponent* ruin = &entity->ruin;

    // the walls are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled)
    {
        if (ruin->enabled)
        {
            WarRuinPieceList* pieces = &ruin->pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, ruin->pieces.count);

            for (s32 i = 0; i < ruin->pieces.count; i++)
            {
                WarRuinPiece* piece = &pieces->items[i];
                if (piece->type == WAR_RUIN_PIECE_NONE)
                    continue;
                    
                // get the index of the tile in the spritesheet of the map,
                // corresponding to the current tileset type (forest, swamp)
                WarRuinsData ruinsData = getRuinsData(piece->type);
                
                s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? ruinsData.tileIndexForest : ruinsData.tileIndexSwamp;
                
                // the position in the world of the road piece tile
                s32 x = piece->tilex;
                s32 y = piece->tiley;

                // coordinates in pixels of the road piece tile
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                nvgSave(gfx);
                nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

                nvgRestore(gfx);
            }

            nvgEndImageBatch(gfx, batch);
        }
    }
}

void _renderForest(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    WarForestComponent* forest = &entity->forest;

    // the wood are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled)
    {
        if (forest->enabled)
        {
            WarTreeList* trees = &forest->trees;
            for (s32 i = 0; i < trees->count; i++)
            {
                WarTree* tree = &trees->items[i];

                if (tree->type == WAR_TREE_NONE)
                    continue;

                WarTreesData data = getTreesData(tree->type);

                // the position in the world of the wood tile
                s32 x = tree->tilex;
                s32 y = tree->tiley;

                s32 prevTileIndex = getMapTileIndex(context, x, y);
                s32 newTileIndex = (tilesetType == MAP_TILESET_FOREST) ? data.tileIndexForest : data.tileIndexSwamp;

                if (prevTileIndex != newTileIndex)
                    logDebug("different tile index for tree (%d, %d), prev: %d, new: %d\n", x, y, prevTileIndex, newTileIndex);

                setMapTileIndex(context, x, y, newTileIndex);
            }            
        }
    }
}

void _renderUnit(WarContext* context, WarEntity* entity, bool selected)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarAnimationsComponent* animations = &entity->animations;

    // size of the original sprite
    vec2 frameSize = getUnitFrameSize(entity);

    // size of the unit
    vec2 unitSize = getUnitSpriteSize(entity);

    // position of the unit in the map
    vec2 position = transform->position;

    // scale of the unit: this is modified by animations when the animation indicates that it
    // should flip horizontally or vertically or both
    vec2 scale = transform->scale;

    nvgTranslate(gfx, -halff(frameSize.x), -halff(frameSize.y));
    nvgTranslate(gfx, halff(unitSize.x), halff(unitSize.y));
    nvgTranslate(gfx, position.x, position.y);

#ifdef DEBUG_RENDER_UNIT_INFO
    nvgFillRect(gfx, getUnitFrameRect(entity), nvgRGBA(0, 0, 128, 128));
    nvgFillRect(gfx, getUnitSpriteRect(entity), NVG_GRAY_TRANSPARENT);
    nvgFillRect(gfx, rectv(getUnitSpriteCenter(entity), VEC2_ONE), nvgRGB(255, 0, 0));
#endif

#ifdef DEBUG_RENDER_UNIT_STATS
    WarUnitComponent* unit = &entity->unit;

    rect spriteRect = getUnitSpriteRect(entity);

    char debugText[50];

    if (unit->hp == 0)
        sprintf(debugText, "hp: dead");
    else
        sprintf(debugText, "hp: %d", percentabi(unit->hp, unit->maxhp));

    nvgFontSize(gfx, 5.0f);
    nvgFontFace(gfx, "roboto-r");
    nvgFillColor(gfx, nvgRGBA(200, 200, 200, 255));
    nvgTextAlign(gfx, NVG_ALIGN_LEFT);
    nvgText(gfx, spriteRect.x, spriteRect.y, debugText, NULL);
#endif

    if (sprite->enabled)
    {
        nvgSave(gfx);

        WarSpriteFrame* frame = getSpriteFrame(context, sprite);
        updateSpriteImage(context, &sprite->sprite, frame->data);
        renderSprite(context, &sprite->sprite, VEC2_ZERO, scale);

        nvgRestore(gfx);

        if (selected)
        {
            rect selr = rectf(halff(frameSize.x - unitSize.x), halff(frameSize.y - unitSize.y), unitSize.x, unitSize.y);
            nvgStrokeRect(gfx, selr, NVG_GREEN_SELECTION, 1.0f);
        }
    }

    if (animations->enabled)
    {
        for (s32 i = 0; i < animations->animations.count; i++)
        {
            WarSpriteAnimation* anim = animations->animations.items[i];
            if (anim->status == WAR_ANIM_STATUS_RUNNING)
            {
                s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
                animFrameIndex = clamp(animFrameIndex, 0, anim->frames.count - 1);

                s32 spriteFrameIndex = anim->frames.items[animFrameIndex];
                assert(spriteFrameIndex >= 0 && spriteFrameIndex < anim->sprite.framesCount);

                nvgSave(gfx);

                nvgTranslate(gfx, anim->offset.x, anim->offset.y);
                nvgScale(gfx, anim->scale.x, anim->scale.y);

#ifdef DEBUG_RENDER_UNIT_ANIMATIONS
                // size of the original sprite
                vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

                nvgFillRect(gfx, rectv(VEC2_ZERO, animFrameSize), NVG_GRAY_TRANSPARENT);
#endif

                WarSpriteFrame frame = anim->sprite.frames[spriteFrameIndex];
                updateSpriteImage(context, &anim->sprite, frame.data);
                renderSprite(context, &anim->sprite, VEC2_ZERO, VEC2_ONE);

                nvgRestore(gfx);
            }
        }
    }
}

void renderEntity(WarContext* context, WarEntity* entity, bool selected)
{
    NVGcontext* gfx = context->gfx;

    if (entity->id && entity->enabled)
    {
        nvgSave(gfx);

        switch (entity->type)
        {
            case WAR_ENTITY_TYPE_IMAGE:
            {
                _renderImage(context, entity);
                break;
            }

            case WAR_ENTITY_TYPE_UNIT:
            {
                _renderUnit(context, entity, selected);
                break;
            }

            case WAR_ENTITY_TYPE_ROAD:
            {
                _renderRoad(context, entity);
                break;
            }

            case WAR_ENTITY_TYPE_WALL:
            {
                _renderWall(context, entity);
                break;
            }

            case WAR_ENTITY_TYPE_RUIN:
            {
                _renderRuin(context, entity);
                break;
            }

            case WAR_ENTITY_TYPE_FOREST:
            {
                _renderForest(context, entity);
                break;
            }

            default:
            {
                logError("Entity of type %d can't be rendered.\n", entity->type);
                break;
            }
        }

        nvgRestore(gfx);
    }
}

void takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage)
{
    assert(isUnit(entity));

    WarUnitComponent *unit = &entity->unit;

    // Minimal damage + [Random damage - Enemy's Armour]
    s32 damage = getTotalDamage(minDamage, rndDamage, unit->armour);
    unit->hp -= damage;
    unit->hp = maxi(unit->hp, 0);

    if (unit->hp == 0)
    {
        if (isBuildingUnit(entity))
        {
            WarState* collapseState = createCollapseState(context, entity);
            changeNextState(context, entity, collapseState, true, true);
        }
        else
        {
            WarState* deathState = createDeathState(context, entity);
            deathState->delay = __frameCountToSeconds(108);
            changeNextState(context, entity, deathState, true, true);
        }
    }
    else if (isBuildingUnit(entity))
    {
        if (!isDamaged(entity))
        {
            WarState* damagedState = createDamagedState(context, entity);
            changeNextState(context, entity, damagedState, true, true);
        }
    }
}
