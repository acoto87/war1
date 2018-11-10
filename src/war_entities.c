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

#define isNeutral(player) (player == 4)
#define isEnemy(player) (player != 0 && !isNeutral(player))
#define isHuman(player) (player == 0)

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
    entity->stateMachine = (WarStateMachineComponent){0};
    entity->animations = (WarAnimationsComponent){0};

    WarEntityListAdd(&map->entities, entity);

    return entity;
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
        WarRoadPieceListAdd(pieces, createRoadPiece(0, x, y, player));
        x += dx;
    }

    WarRoadPieceListAdd(pieces, createRoadPiece(0, x, y, player));

    while (y != y2)
    {
        WarRoadPieceListAdd(pieces, createRoadPiece(0, x, y, player));
        y += dy;
    }

    WarRoadPieceListAdd(pieces, createRoadPiece(0, x, y, player));
}

void determineRoadTypes(WarRoadPieceList *pieces)
{
    s32 count = pieces->count;
    for(s32 i = 0; i < count; i++)
    {
        WarRoadPiece* pi = &pieces->items[i];

        bool left = false, 
             top = false, 
             right = false, 
             bottom = false;

        for(s32 j = 0; j < count; j++)
        {
            if (i == j) continue;

            WarRoadPiece* pj = &pieces->items[j];

            if (pj->tilex == pi->tilex - 1 && pj->tiley == pi->tiley)
                left = true;
            else if(pj->tilex == pi->tilex + 1 && pj->tiley == pi->tiley)
                right = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley - 1)
                top = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley + 1)
                bottom = true;
        }

        // Endpieces
        if (top && !bottom && !left && !right)
            pi->type = WAR_ROAD_PIECE_BOTTOM;
        if (!top && bottom && !left && !right)
            pi->type = WAR_ROAD_PIECE_TOP;
        if (!top && !bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_LEFT;
        if (!top && !bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_RIGHT;

        // Corner pieces
        if (top && !bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_TOP_LEFT;
        if (!top && bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_BOTTOM_LEFT;
        if (top && !bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_TOP_RIGHT;
        if (!top && bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_BOTTOM_RIGHT;

        // Middle pieces
        if (!top && !bottom && left && right)
            pi->type = WAR_ROAD_PIECE_HORIZONTAL;
        if (top && bottom && !left && !right)
            pi->type = WAR_ROAD_PIECE_VERTICAL;

        // Quad piece
        if (top && bottom && left && right)
            pi->type = WAR_ROAD_PIECE_CROSS;

        // T-Corners
        if (top && bottom && left && !right)
            pi->type = WAR_ROAD_PIECE_T_RIGHT;
        if (top && bottom && !left && right)
            pi->type = WAR_ROAD_PIECE_T_LEFT;
        if (!top && bottom && left && right)
            pi->type = WAR_ROAD_PIECE_T_TOP;
        if (top && !bottom && left && right)
            pi->type = WAR_ROAD_PIECE_T_BOTTOM;
    }
}

void determineRuinTypes(WarEntity* entity)
{
    WarRuinComponent* ruin = &entity->ruin;
    WarRuinPieceList* pieces = &ruin->pieces;
    s32 count = pieces->count;
    for(s32 i = 0; i < count; i++)
    {
        WarRuinPiece* pi = &pieces->items[i];

        bool topLeft = false,
             top = false,
             topRight = false,
             left = false,
             right = false,
             bottomLeft = false,
             bottom = false,
             bottomRight = false;

        for(s32 j = 0; j < count; j++)
        {
            if (i == j) continue;

            WarRuinPiece* pj = &pieces->items[j];

            if (pj->tilex == pi->tilex - 1 && pj->tiley == pi->tiley - 1)
                topLeft = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley - 1)
                top = true;
            else if (pj->tilex == pi->tilex + 1 && pj->tiley == pi->tiley - 1)
                topRight = true;
            else if (pj->tilex == pi->tilex - 1 && pj->tiley == pi->tiley)
                left = true;
            else if(pj->tilex == pi->tilex + 1 && pj->tiley == pi->tiley)
                right = true;
            else if (pj->tilex == pi->tilex - 1 && pj->tiley == pi->tiley + 1)
                bottomLeft = true;
            else if (pj->tilex == pi->tilex && pj->tiley == pi->tiley + 1)
                bottom = true;
            else if (pj->tilex == pi->tilex + 1 && pj->tiley == pi->tiley + 1)
                bottomRight = true;
        }

        // Corners
        if (!topLeft && !top && !left && right && bottom && bottomRight)
            pi->type = WAR_RUIN_PIECE_TOP_LEFT;
        if (!topRight && !top && !right && left && bottom && bottomLeft)
            pi->type = WAR_RUIN_PIECE_TOP_RIGHT;
        if (!bottomLeft && !bottom && !left && right && top && topRight)
            pi->type = WAR_RUIN_PIECE_BOTTOM_LEFT;
        if (!bottomRight && !bottom && !right && left && top && topLeft)
            pi->type = WAR_RUIN_PIECE_BOTTOM_RIGHT;

        // Edges
        if (!top && left && right && bottom)
            pi->type = WAR_RUIN_PIECE_TOP;
        if (!left && top && bottom && right)
            pi->type = WAR_RUIN_PIECE_LEFT;
        if (!right && top && bottom && left)
            pi->type = WAR_RUIN_PIECE_RIGHT;
        if (!bottom && left && right && top)
            pi->type = WAR_RUIN_PIECE_BOTTOM;

        // Insides
        if (!bottomRight && top && left && right && bottom && topLeft)
            pi->type = WAR_RUIN_PIECE_TOP_LEFT_INSIDE;
        if (!bottomLeft && top && right && left && bottom && topRight)
            pi->type = WAR_RUIN_PIECE_TOP_RIGHT_INSIDE;
        if (!topRight && top && left && right && bottom && bottomLeft)
            pi->type = WAR_RUIN_PIECE_BOTTOM_LEFT_INSIDE;
        if (!topLeft && top && left && right && bottom && bottomRight)
            pi->type = WAR_RUIN_PIECE_BOTTOM_RIGHT_INSIDE;

        // Diagonals
        if (!topRight && !bottomLeft && topLeft && bottomRight)
            pi->type = WAR_RUIN_PIECE_DIAG_1;
        if (!topLeft && !bottomRight && topRight && bottomLeft)
            pi->type = WAR_RUIN_PIECE_DIAG_2;

        // Center
        if (topLeft && top && topRight && left && right && bottomLeft && bottom && bottomRight)
            pi->type = WAR_RUIN_PIECE_CENTER;
        if (!topLeft && !top && !topRight && !left && !right && !bottomLeft && !bottom && !bottomRight)
            pi->type = WAR_RUIN_PIECE_CENTER;
    }
}

WarEntity* createRuins(WarContext* context, s32 x, s32 y, s32 dim)
{
    WarMap* map = context->map;

    WarRuinPieceList pieces;
    WarRuinPieceListInit(&pieces, WarRuinPieceListDefaultOptions);

    for(s32 yy = 0; yy < dim; yy++)
    {
        for(s32 xx = 0; xx < dim; xx++)
            WarRuinPieceListAdd(&pieces, createRuinPiece(x + xx, y + yy));
    }

    WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_RUIN);
    addRuinComponent(context, entity, pieces);
    addSpriteComponent(context, entity, map->sprite);

    determineRuinTypes(entity);

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
    removeRuinComponent(context, entity);
    removeUnitComponent(context, entity);
    removeStateMachineComponent(context, entity);
    removeAnimationsComponent(context, entity);

    WarEntityListRemoveAt(&map->entities, index);
}

// Render entities
void renderImage(WarContext* context, WarEntity* entity)
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

void renderRoad(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarRoadComponent* road = &entity->road;

    if (sprite->enabled)
    {
        nvgTranslate(gfx, transform.position.x, transform.position.y);

        if (road->enabled)
        {
            WarRoadPieceList* pieces = &road->pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, road->pieces.count);

            WarMapTilesetType tilesetType = context->map->tilesetType;

            for (s32 i = 0; i < road->pieces.count; i++)
            {
                // get the index of the tile in the spritesheet of the map,
                // corresponding to the current tileset type (forest, swamp)
                WarRoadsData roadsData = getRoadsData(pieces->items[i].type);

                s32 tileIndex = 0;
                
                switch (tilesetType)
                {
                    case MAP_TILESET_FOREST:
                    {
                        tileIndex = roadsData.tileIndexForest;
                        break;
                    }

                    case MAP_TILESET_SWAMP:
                    {
                        tileIndex = roadsData.tileIndexSwamp;
                        break;
                    }

                    case MAP_TILESET_DUNGEON:
                    {
                        tileIndex = roadsData.tileIndexDungeon;
                        break;
                    }
                }

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

void renderRuin(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarRuinComponent* ruin = &entity->ruin;

    if (sprite->enabled)
    {
        nvgTranslate(gfx, transform.position.x, transform.position.y);

        if (ruin->enabled)
        {
            WarRuinPieceList* pieces = &ruin->pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, ruin->pieces.count);

            WarMapTilesetType tilesetType = context->map->tilesetType;

            for (s32 i = 0; i < ruin->pieces.count; i++)
            {
                // get the index of the tile in the spritesheet of the map,
                // corresponding to the current tileset type (forest, swamp)
                WarRuinsData ruinsData = getRuinsData(pieces->items[i].type);
                
                s32 tileIndex = 0;
                
                switch (tilesetType)
                {
                    case MAP_TILESET_FOREST:
                    {
                        tileIndex = ruinsData.tileIndexForest;
                        break;
                    }

                    case MAP_TILESET_SWAMP:
                    {
                        tileIndex = ruinsData.tileIndexSwamp;
                        break;
                    }

                    case MAP_TILESET_DUNGEON:
                    {
                        tileIndex = ruinsData.tileIndexDungeon;
                        break;
                    }
                }

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

void renderUnit(WarContext* context, WarEntity* entity, bool selected)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarUnitComponent* unit = &entity->unit;
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

                // size of the original sprite
                vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

                nvgSave(gfx);

                nvgTranslate(gfx, anim->offset.x, anim->offset.y);
                nvgScale(gfx, anim->scale.x, anim->scale.y);

#ifdef DEBUG_RENDER_UNIT_ANIMATIONS
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
            renderImage(context, entity);
            break;
        }

        case WAR_ENTITY_TYPE_UNIT:
        {
            renderUnit(context, entity, selected);
            break;
        }

        case WAR_ENTITY_TYPE_ROAD:
        {
            renderRoad(context, entity);
            break;
        }

        case WAR_ENTITY_TYPE_RUIN:
        {
            renderRuin(context, entity);
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
    s32 damage = minDamage + maxi(rndDamage - unit->armour, 0);
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