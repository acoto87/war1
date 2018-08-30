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
    entity->anim = (WarAnimationComponent){0};
    entity->unit = (WarUnitComponent){0};
    entity->road = (WarRoadComponent){0};

    return entity;
}

void addTransformComponent(WarContext *context, WarEntity *entity, vec2 position)
{
    entity->transform = (WarTransformComponent){0};
    entity->transform.enabled = true;
    entity->transform.position = position;
    entity->transform.rotation = VEC2_ZERO;
    entity->transform.scale = VEC2_ONE;
}

void addSpriteComponent(WarContext *context, WarEntity *entity, WarSprite sprite)
{
    entity->sprite = (WarSpriteComponent){0};
    entity->sprite.enabled = true;
    entity->sprite.resourceIndex = 0;
    entity->sprite.frameIndex = 0;
    entity->sprite.sprite = sprite;
}

void addSpriteComponentFromResource(WarContext *context, WarEntity *entity, s32 resourceIndex)
{
    assert(resourceIndex);

    WarResource *resource = getOrCreateResource(context, resourceIndex);
    assert(resource->type == WAR_RESOURCE_TYPE_IMAGE || resource->type == WAR_RESOURCE_TYPE_SPRITE);

    u8 *pixels = NULL;
    u32 w, h;

    switch(resource->type)
    {
        case WAR_RESOURCE_TYPE_IMAGE:
        {
            pixels = resource->imageData.pixels;
            w = resource->imageData.width;
            h = resource->imageData.height;
            break;
        }

        case WAR_RESOURCE_TYPE_SPRITE:
        {
            pixels = resource->spriteData.frames[0].data;
            w = resource->spriteData.frameWidth;
            h = resource->spriteData.frameHeight;
            break;
        }
    }

    WarSprite sprite = createSprite(context, w, h, pixels);
    addSpriteComponent(context, entity, sprite);
    entity->sprite.resourceIndex = resourceIndex;
}

void addUnitComponent(WarContext *context, WarEntity *entity, WarUnitType type, s32 x, s32 y, u8 player, u16 value)
{
    entity->unit = (WarUnitComponent){0};
    entity->unit.enabled = true;
    entity->unit.type = type;
    entity->unit.direction = WAR_DIRECTION_NORTH;
    entity->unit.tilex = x;
    entity->unit.tiley = y;
    entity->unit.sizex = unitsData[type * 4 + 2];
    entity->unit.sizey = unitsData[type * 4 + 3];
    entity->unit.player = player;
    entity->unit.value = value;
}

void addMovementComponent(WarContext* context, WarEntity* entity)
{
    entity->movement = (WarMovementComponent){0};
    entity->movement.enabled = true;
}

void addRoadComponent(WarContext *context, WarEntity *entity, WarRoadPieceList pieces)
{
    entity->road = (WarRoadComponent){0};
    entity->road.enabled = true;
    entity->road.pieces = pieces;
}

void addAnimationComponent(WarContext *context, WarEntity *entity, f32 duration, bool loop)
{
    entity->anim = (WarAnimationComponent){0};
    entity->anim.enabled = true;
    entity->anim.loop = loop;
    entity->anim.duration = duration;
    entity->anim.offset = 0;
}

rect getUnitRect(WarContext *context, WarEntity* entity)
{
    WarTransformComponent transform = entity->transform;
    WarUnitComponent unit = entity->unit;

    rect unitRect = rectf(
        transform.position.x,
        transform.position.y,
        unit.sizex * MEGA_TILE_WIDTH,
        unit.sizey * MEGA_TILE_HEIGHT);

    return unitRect;
}

WarSpriteFrame getSpriteFrame(WarContext* context, WarSpriteComponent sprite)
{
    assert(sprite.resourceIndex);

    WarResource* resource = getOrCreateResource(context, sprite.resourceIndex);
    assert(resource->type == WAR_RESOURCE_TYPE_IMAGE || resource->type == WAR_RESOURCE_TYPE_SPRITE);

    WarSpriteFrame frame = (WarSpriteFrame){0};
    
    switch (resource->type)
    {
        case WAR_RESOURCE_TYPE_IMAGE:
            frame.data = resource->imageData.pixels;
            frame.w = resource->imageData.width;
            frame.h = resource->imageData.height;
            break;

        case WAR_RESOURCE_TYPE_SPRITE:
        {
            frame = resource->spriteData.frames[sprite.frameIndex];
            break;
        }
    }

    return frame;
}

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

void renderImage(WarContext *context, WarEntity *entity)
{
    NVGcontext *gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent sprite = entity->sprite;

    if (sprite.enabled)
    {
        WarSpriteFrame frame = getSpriteFrame(context, sprite);

        if (transform.enabled)
        {
            nvgTranslate(gfx, -frame.dx, -frame.dy);
            nvgTranslate(gfx, transform.position.x, transform.position.y);
        }

        renderSprite(context, &sprite.sprite, 0, 0);
    }
}

void renderRoad(WarContext *context, WarEntity *entity)
{
    NVGcontext *gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent sprite = entity->sprite;
    WarRoadComponent road = entity->road;

    if (sprite.enabled)
    {
        if (transform.enabled)
        {
            nvgTranslate(gfx, transform.position.x, transform.position.y);
        }

        if (road.enabled)
        {
            WarRoadPieceList *pieces = &road.pieces;

            NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite.sprite.image, road.pieces.count);

            if (context->map->tilesetType != MAP_TILESET_FOREST &&
                context->map->tilesetType != MAP_TILESET_SWAMP)
            {
                fprintf(stderr, "Unkown tileset for a road: %d\n", context->map->tilesetType);
                return;
            }

            // this is the column index in the array roadsData
            // later is used and determine the tileIndex for each piece of the road
            s32 roadsDataIndex = (context->map->tilesetType + 1);

            for(s32 i = 0; i < road.pieces.count; i++)
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
                nvgRenderBatchImage(gfx, batch, 
                    tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT, 
                    0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                nvgRestore(gfx);
            }

            nvgEndImageBatch(gfx, batch);
        }
    }
}

void renderUnit(WarContext *context, WarEntity *entity, bool selected)
{
    NVGcontext *gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent sprite = entity->sprite;
    WarUnitComponent unit = entity->unit;

    if (sprite.enabled)
    {
        WarSpriteFrame frame = getSpriteFrame(context, sprite);

        if (transform.enabled)
        {
            nvgTranslate(gfx, -frame.dx, -frame.dy);
            nvgTranslate(gfx, transform.position.x, transform.position.y);
        }

        updateSprite(context, &sprite.sprite, frame.data);
        renderSprite(context, &sprite.sprite, 0, 0);

        if (selected)
        {
            nvgBeginPath(gfx);
            nvgRect(gfx, frame.dx, frame.dy, frame.w, frame.h);
            nvgStrokeColor(gfx, NVG_GREEN_SELECTION);
            nvgStroke(gfx);
        }
    }
}

void renderEntity(WarContext *context, WarEntity *entity, bool selected)
{
    NVGcontext *gfx = context->gfx;

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
                fprintf(stderr, "Entity of type %d can't be redered.\n", entity->type);
                break;
            }
        }

        nvgRestore(gfx);
    }
}