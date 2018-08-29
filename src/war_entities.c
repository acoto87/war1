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
    entity->transform.position.x = position.x;
    entity->transform.position.y = position.y;
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
    WarResource *resource = getOrCreateResource(context, resourceIndex);
    if (resource->type != WAR_RESOURCE_TYPE_IMAGE && 
        resource->type != WAR_RESOURCE_TYPE_SPRITE)
    {
        fprintf(stderr, "Resource type not supported in sprite components: %d\n", resource->type);
        return;
    }

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

        default:
        {
            fprintf(stderr, "Unkown resource type: %d\n", resource->type);
            return;
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

    addTransformComponent(context, entity, vec2i(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT));

    s32 spriteIndex = unitsData[type * 4 + 1];
    if (spriteIndex == 0)
    {
        fprintf(stderr, "Sprite for unit of type %d is not configure properly. Default to footman sprite.", type);
        spriteIndex = 279;
    }
    addSpriteComponentFromResource(context, entity, spriteIndex);
}

void addAnimationComponent(WarContext *context, WarEntity *entity, f32 duration, bool loop)
{
    entity->anim = (WarAnimationComponent){0};
    entity->anim.enabled = true;
    entity->anim.loop = loop;
    entity->anim.duration = duration;
    entity->anim.offset = 0;
}

void addRoadComponent(WarContext *context, WarEntity *entity, WarRoadPieceList pieces)
{
    entity->road = (WarRoadComponent){0};
    entity->road.enabled = true;
    entity->road.pieces = pieces;

    addSpriteComponent(context, entity, context->map->sprite);
}

rect getUnitRect(WarContext *context, WarEntity* entity)
{
    WarTransformComponent transform = entity->transform;
    WarUnitComponent unit = entity->unit;

    f32 scale = context->globalScale;

    rect unitRect = rectf(
        transform.position.x,
        transform.position.y,
        unit.sizex * MEGA_TILE_WIDTH,
        unit.sizey * MEGA_TILE_HEIGHT);

    unitRect = rectScalef(unitRect, scale);
    return unitRect;
}

void renderImage(WarContext *context, WarEntity *entity)
{
    NVGcontext *gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarSpriteComponent sprite = entity->sprite;

    if (sprite.enabled)
    {
        u8 *pixels = NULL;
        s32 dx = 0, dy = 0;

        if (sprite.resourceIndex)
        {
            WarResource *resource = getOrCreateResource(context, sprite.resourceIndex);
            if (resource->type == WAR_RESOURCE_TYPE_SPRITE)
            {
                pixels = resource->spriteData.frames[sprite.frameIndex].data;
                dx = resource->spriteData.frames[sprite.frameIndex].dx;
                dy = resource->spriteData.frames[sprite.frameIndex].dy;
            }
        }

        if (transform.enabled)
        {
            nvgTranslate(gfx, -dx, -dy);
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

            for(s32 i = 0; i < road.pieces.count; i++)
            {
                s32 tileIndex = 0;

                s32 x = pieces->items[i].tilex;
                s32 y = pieces->items[i].tiley;

                switch (context->map->tilesetType)
                {
                    case MAP_TILESET_FOREST:
                    {
                        tileIndex = roadsData[pieces->items[i].type * 3 + 1];
                        break;
                    }
                
                    case MAP_TILESET_SWAMP:
                    {
                        tileIndex = roadsData[pieces->items[i].type * 3 + 2];
                        break;
                    }
        
                    default:
                    {
                        fprintf(stderr, "Unkown tileset for a road: %d\n", context->map->tilesetType);
                        return;
                    }
                }

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
        u8 *pixels = NULL;
        s32 dx = 0, dy = 0;
        s32 w, h;

        if (sprite.resourceIndex)
        {
            WarResource *resource = getOrCreateResource(context, sprite.resourceIndex);
            if (resource->type == WAR_RESOURCE_TYPE_SPRITE)
            {
                pixels = resource->spriteData.frames[sprite.frameIndex].data;
                dx = resource->spriteData.frames[sprite.frameIndex].dx;
                dy = resource->spriteData.frames[sprite.frameIndex].dy;
                w = resource->spriteData.frames[sprite.frameIndex].w;
                h = resource->spriteData.frames[sprite.frameIndex].h;
            }
        }

        if (transform.enabled)
        {
            nvgTranslate(gfx, -dx, -dy);
            nvgTranslate(gfx, transform.position.x, transform.position.y);
        }

        if (selected)
        {
            nvgBeginPath(gfx);
            nvgRect(gfx, dx, dy, w, h);
            nvgStrokeColor(gfx, NVG_WHITE);
            nvgStroke(gfx);
        }

        updateSprite(context, &sprite.sprite, pixels);
        renderSprite(context, &sprite.sprite, 0, 0);
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