WarEntity* createEntity(WarContext *context, WarEntityType type)
{
    WarEntity *entity = (WarEntity*)xcalloc(1, sizeof(WarEntity));
    entity->id = ++context->staticEntityId;
    entity->type = type;
    entity->enabled = true;
    
    // transform component
    entity->transform = (WarTransformComponent){0};
    entity->transform.enabled = true;

    // clear all other components
    entity->sprite = (WarSpriteComponent){0};
    entity->unit = (WarUnitComponent){0};
    entity->road = (WarRoadComponent){0};
    entity->stateMachine = (WarStateMachineComponent){0};

    return entity;
}

void addTransformComponent(WarContext* context, WarEntity* entity, vec2 position)
{
    entity->transform = (WarTransformComponent){0};
    entity->transform.enabled = true;
    entity->transform.position = position;
    entity->transform.rotation = VEC2_ZERO;
    entity->transform.scale = VEC2_ONE;
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
    assert(resourceIndex);

    WarResource *resource = getOrCreateResource(context, resourceIndex);
    assert(resource->type == WAR_RESOURCE_TYPE_IMAGE || resource->type == WAR_RESOURCE_TYPE_SPRITE);

    WarSprite sprite;

    switch(resource->type)
    {
        case WAR_RESOURCE_TYPE_IMAGE:
        {
            u32 width = resource->imageData.width;
            u32 height = resource->imageData.height;
            u8* data = resource->imageData.pixels;
            sprite = createSprite(context, width, height, data);
            break;
        }

        case WAR_RESOURCE_TYPE_SPRITE:
        {
            u32 frameWidth = resource->spriteData.frameWidth;
            u32 frameHeight = resource->spriteData.frameHeight;
            u32 frameCount = resource->spriteData.framesCount;
            WarSpriteFrame* frames = resource->spriteData.frames;
            sprite = createSpriteFrames(context, frameWidth, frameHeight, frameCount, frames);
            break;
        }
    }

    addSpriteComponent(context, entity, sprite);
    entity->sprite.resourceIndex = resourceIndex;
}

void addUnitComponent(WarContext* context, WarEntity* entity, WarUnitType type, s32 x, s32 y, u8 player, WarResourceKind resourceKind, u32 amount)
{
    entity->unit = (WarUnitComponent){0};
    entity->unit.enabled = true;
    entity->unit.type = type;
    entity->unit.direction = rand() % WAR_DIRECTION_COUNT;
    entity->unit.tilex = x;
    entity->unit.tiley = y;
    entity->unit.sizex = unitsData[type * 4 + 2];
    entity->unit.sizey = unitsData[type * 4 + 3];
    entity->unit.player = player;
    entity->unit.resourceKind = resourceKind;
    entity->unit.amount = amount;
    entity->unit.currentActionIndex = 0;
    WarUnitActionListInit(&entity->unit.actions);
}

void addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces)
{
    entity->road = (WarRoadComponent){0};
    entity->road.enabled = true;
    entity->road.pieces = pieces;
}

void addStateMachineComponent(WarContext* context, WarEntity* entity)
{
    entity->stateMachine = (WarStateMachineComponent){0};
    entity->stateMachine.enabled = true;
}

void removeSpriteComponent(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    for(s32 i = 0; i < sprite->sprite.framesCount; i++)
    {
        u8* data = sprite->sprite.frames[i].data;
        if (data) free(data);
    }

    nvgDeleteImage(context->gfx, sprite->sprite.image);

    *sprite = (WarSpriteComponent){0};
}

#define isNeutral(player) (player == 4)
#define isEnemy(player) (player != 0 && !isNeutral(player))
#define isHuman(player) (player == 0)

inline bool isDudeUnit(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        case WAR_UNIT_PEASANT:
        case WAR_UNIT_PEON:
        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        case WAR_UNIT_CONJURER:
        case WAR_UNIT_WARLOCK:
        case WAR_UNIT_CLERIC:
        case WAR_UNIT_NECROLYTE:
        case WAR_UNIT_MEDIVH:
            return true;
    
        default:
            return false;
    }
}

inline bool isBuildingUnit(WarUnitType type)
{
    switch (type)
    {
        case WAR_UNIT_FARM_HUMANS:
        case WAR_UNIT_FARM_ORCS:
        case WAR_UNIT_BARRACKS_HUMANS:
        case WAR_UNIT_BARRACKS_ORCS:
        case WAR_UNIT_CHURCH:
        case WAR_UNIT_TEMPLE:
        case WAR_UNIT_TOWER_HUMANS:
        case WAR_UNIT_TOWER_ORCS:
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_TOWNHALL_ORCS:
        case WAR_UNIT_LUMBERMILL_HUMANS:
        case WAR_UNIT_LUMBERMILL_ORCS:
        case WAR_UNIT_STABLES:
        case WAR_UNIT_KENNEL:
        case WAR_UNIT_BLACKSMITH_HUMANS:
        case WAR_UNIT_BLACKSMITH_ORCS:
        case WAR_UNIT_STORMWIND:
        case WAR_UNIT_BLACKROCK:
            return true;
    
        default: 
            return false;
    }
}

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
            WarRoadPieceList *pieces = &road->pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, road->pieces.count);

            if (context->map->tilesetType != MAP_TILESET_FOREST &&
                context->map->tilesetType != MAP_TILESET_SWAMP)
            {
                logError("Unkown tileset for a road: %d\n", context->map->tilesetType);
                return;
            }

            // this is the column index in the array roadsData
            // later is used and determine the tileIndex for each piece of the road
            s32 roadsDataIndex = (context->map->tilesetType + 1);

            for(s32 i = 0; i < road->pieces.count; i++)
            {
                // get the index of the tile in the spritesheet of the map, 
                // corresponding to the current tileset type (forest, swamp)
                s32 tileIndex = roadsData[pieces->items[i].type * 3 + roadsDataIndex];

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

    if (sprite->enabled)
    {
        // size of the original sprite
        vec2 frameSize = vec2i(sprite->sprite.frameWidth, sprite->sprite.frameHeight);

        // size of the unit
        vec2 unitSize = vec2i(unit->sizex * MEGA_TILE_WIDTH, unit->sizey * MEGA_TILE_HEIGHT);

        // position of the unit in the map 
        vec2 position = transform->position;

        // scale of the unit: this is modified by animations when the animation indicates that it 
        // should flip horizontally or vertically or both
        vec2 scale = transform->scale;

        // DEBUG
        // nvgFillRect(gfx, rectv(position, unitSize), NVG_GRAY_TRANSPARENT);

        nvgTranslate(gfx, -halff(frameSize.x), -halff(frameSize.y));
        nvgTranslate(gfx, position.x + halff(unitSize.x), position.y + halff(unitSize.y));

        WarSpriteFrame* frame = getSpriteFrame(context, sprite);
        updateSpriteImage(context, &sprite->sprite, frame->data);
        renderSprite(context, &sprite->sprite, VEC2_ZERO, scale);

        if (selected)
        {
            rect selr = rectf(halff(frameSize.x - unitSize.x), halff(frameSize.y - unitSize.y), unitSize.x, unitSize.y);
            nvgStrokeRect(gfx, selr, NVG_GREEN_SELECTION);
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

            default:
            {
                logError("Entity of type %d can't be rendered.\n", entity->type);
                break;
            }
        }

        nvgRestore(gfx);
    }
}