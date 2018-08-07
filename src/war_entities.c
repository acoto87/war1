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

    return entity;
}

void addTransformComponent(WarContext *context, WarEntity *entity, vec2 position)
{
    entity->transform = (WarTransformComponent){0};
    entity->transform.enabled = true;
    entity->transform.position[0] = position[0];
    entity->transform.position[1] = position[1];
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

    u32 w, h;

    switch(resource->type)
    {
        case WAR_RESOURCE_TYPE_IMAGE:
        {
            w = resource->imageData.width;
            h = resource->imageData.height;
            break;
        }

        case WAR_RESOURCE_TYPE_SPRITE:
        {
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

    // create VBO
    WarVertex vertices[] = 
    {
        {{0, 0}, {0.0f, 1.0f}},
        {{w, 0}, {1.0f, 1.0f}},
        {{w, h}, {1.0f, 0.0f}},
        {{0, h}, {0.0f, 0.0f}}
    };

    // create IBO
    GLuint indices[] = { 0, 1, 2, 0, 2, 3 };

    WarSprite sprite = createSprite(context, 1, w, h, vertices, 4, indices, 6, -1);
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

    addTransformComponent(context, entity, (vec2){x * MEGA_TILE_WIDTH, (MAP_HEIGHT - y) * MEGA_TILE_HEIGHT});

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

void addRoadComponent(WarContext *context, WarEntity *entity)
{
    WarRoadPieceList *pieces = &entity->road.pieces;
    s32 count = pieces->count;

    WarVertex *vertices = (WarVertex*)xcalloc(count * 4, sizeof(WarVertex));
    GLuint *indices = (GLuint*)xcalloc(count * 6, sizeof(GLuint));

    for(s32 i = 0; i < count; i++)
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

        // vertex 00
        vertices[i * 4 + 0].position[0] = (f32)(x + 0) * MEGA_TILE_WIDTH;
        vertices[i * 4 + 0].position[1] = (f32)(MAP_HEIGHT - 1 - y + 0) * MEGA_TILE_HEIGHT;
        vertices[i * 4 + 0].texCoords[0] = (f32)(tilePixelX) / TILESET_WIDTH_PX;
        vertices[i * 4 + 0].texCoords[1] = (f32)(tilePixelY + MEGA_TILE_HEIGHT) / TILESET_HEIGHT_PX;

        // vertex 10
        vertices[i * 4 + 1].position[0] = (f32)(x + 1) * MEGA_TILE_WIDTH;
        vertices[i * 4 + 1].position[1] = (f32)(MAP_HEIGHT - 1 - y + 0) * MEGA_TILE_HEIGHT;
        vertices[i * 4 + 1].texCoords[0] = (f32)(tilePixelX + MEGA_TILE_WIDTH) / TILESET_WIDTH_PX;
        vertices[i * 4 + 1].texCoords[1] = (f32)(tilePixelY + MEGA_TILE_HEIGHT) / TILESET_HEIGHT_PX;

        // vertex 11
        vertices[i * 4 + 2].position[0] = (f32)(x + 1) * MEGA_TILE_WIDTH;
        vertices[i * 4 + 2].position[1] = (f32)(MAP_HEIGHT - 1 - y + 1) * MEGA_TILE_HEIGHT;
        vertices[i * 4 + 2].texCoords[0] = (f32)(tilePixelX + MEGA_TILE_WIDTH) / TILESET_WIDTH_PX;
        vertices[i * 4 + 2].texCoords[1] = (f32)(tilePixelY) / TILESET_HEIGHT_PX;

        // vertex 01
        vertices[i * 4 + 3].position[0] = (f32)(x + 0) * MEGA_TILE_WIDTH;
        vertices[i * 4 + 3].position[1] = (f32)(MAP_HEIGHT - 1 - y + 1) * MEGA_TILE_HEIGHT;
        vertices[i * 4 + 3].texCoords[0] = (f32)(tilePixelX) / TILESET_WIDTH_PX;
        vertices[i * 4 + 3].texCoords[1] = (f32)(tilePixelY) / TILESET_HEIGHT_PX;

        // indices for 1st triangle
        indices[i * 6 + 0] = i * 4 + 0;
        indices[i * 6 + 1] = i * 4 + 1;
        indices[i * 6 + 2] = i * 4 + 2;

        // indices for 2nd triangle
        indices[i * 6 + 3] = i * 4 + 0;
        indices[i * 6 + 4] = i * 4 + 2;
        indices[i * 6 + 5] = i * 4 + 3;
    }

    entity->road = (WarRoadComponent){0};
    entity->road.enabled = true;

    s32 textureIndex = context->map->sprite.textureIndex;
    WarSprite sprite = createSprite(context, count, TILESET_WIDTH_PX, TILESET_HEIGHT_PX, vertices, count * 4, indices, count * 6, textureIndex);
    addSpriteComponent(context, entity, sprite);

    free(vertices);
    free(indices);
}

void renderImage(WarContext *context, WarEntity *entity, mat4 baseTransform)
{
    WarTransformComponent transform = entity->transform;
    WarSpriteComponent sprite = entity->sprite;

    if (sprite.enabled)
    {
        mat4 model;
        glm_mat4_identity(model);
        glm_mat4_mul(baseTransform, model, model);

        u8 *pixels = null;
        s32 dx = 0, dy = 0;

        if (sprite.resourceIndex)
        {
            WarResource *resource = getOrCreateResource(context, sprite.resourceIndex);

            switch(resource->type)
            {
                case WAR_RESOURCE_TYPE_IMAGE:
                {
                    pixels = resource->imageData.pixels;
                    break;
                }

                case WAR_RESOURCE_TYPE_SPRITE:
                {
                    pixels = resource->spriteData.frames[sprite.frameIndex].data;
                    dx = resource->spriteData.frames[sprite.frameIndex].dx;
                    dy = resource->spriteData.frames[sprite.frameIndex].dy;
                    break;
                }

                default:
                {
                    fprintf(stderr, "Sprite component is enabled with and unkown resource type: %d\n", resource->type);
                    return;
                }
            }
        }

        if (transform.enabled)
        {
            glm_translate(model, (vec4){-dx, -dy, 0.0f, 0.0f});
            glm_translate(model, (vec4){transform.position[0], transform.position[1], 0.0f, 0.0f});
            glUniformMatrix4fv(context->modelLocation, 1, GL_FALSE, (f32*)model);
        }

        renderSprite(context, &sprite.sprite, pixels, model);
    }
}

void renderRoad(WarContext *context, WarEntity *entity, mat4 baseTransform)
{
    WarTransformComponent transform = entity->transform;
    WarSpriteComponent sprite = entity->sprite;

    if (sprite.enabled)
    {
        mat4 model;
        glm_mat4_identity(model);
        glm_mat4_mul(baseTransform, model, model);

        if (transform.enabled)
        {
            glm_translate(model, (vec4){transform.position[0], transform.position[1], 0.0f, 0.0f});
            glUniformMatrix4fv(context->modelLocation, 1, GL_FALSE, (f32*)model);
        }

        renderSprite(context, &sprite.sprite, null, model);
    }
}

void renderUnit(WarContext *context, WarEntity *entity, mat4 baseTransform)
{
    WarTransformComponent transform = entity->transform;
    WarSpriteComponent sprite = entity->sprite;
    WarUnitComponent unit = entity->unit;

    if (sprite.enabled)
    {
        mat4 model;
        glm_mat4_identity(model);
        glm_mat4_mul(baseTransform, model, model);

        u8 *pixels = null;
        s32 dx = 0, dy = 0;

        if (sprite.resourceIndex)
        {
            WarResource *resource = getOrCreateResource(context, sprite.resourceIndex);

            switch(resource->type)
            {
                case WAR_RESOURCE_TYPE_IMAGE:
                {
                    pixels = resource->imageData.pixels;
                    break;
                }

                case WAR_RESOURCE_TYPE_SPRITE:
                {
                    pixels = resource->spriteData.frames[sprite.frameIndex].data;
                    dx = resource->spriteData.frames[sprite.frameIndex].dx;
                    dy = resource->spriteData.frames[sprite.frameIndex].dy;
                    break;
                }

                default:
                {
                    fprintf(stderr, "Sprite component is enabled with and unkown resource type: %d\n", resource->type);
                    return;
                }
            }
        }

        if (transform.enabled)
        {
            glm_translate(model, (vec4){-dx, -dy, 0.0f, 0.0f});
            glm_translate(model, (vec4){transform.position[0], transform.position[1], 0.0f, 0.0f});

            if (unit.enabled)
            {
                glm_translate(model, (vec4){0, -unit.sizey * MEGA_TILE_HEIGHT, 0.0f, 0.0f});
            }

            glUniformMatrix4fv(context->modelLocation, 1, GL_FALSE, (f32*)model);
        }

        renderSprite(context, &sprite.sprite, pixels, model);
    }
}

void renderEntity(WarContext *context, WarEntity *entity, mat4 baseTransform)
{
    if (entity->id && entity->enabled)
    {
        switch (entity->type)
        {
            case WAR_ENTITY_TYPE_IMAGE:
            {
                renderImage(context, entity, baseTransform);
                break;
            }

            case WAR_ENTITY_TYPE_UNIT:
            {
                renderUnit(context, entity, baseTransform);
                break;
            }

            case WAR_ENTITY_TYPE_ROAD:
            {
                renderRoad(context, entity, baseTransform);
                break;
            }

            default:
            {
                fprintf(stderr, "Entity of type %d can't be redered.\n", entity->type);
                break;
            }
        }
    }
}