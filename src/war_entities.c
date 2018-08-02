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

void addSpriteComponent(WarContext *context, WarEntity *entity, s32 resourceIndex)
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

    entity->sprite = (WarSpriteComponent){0};
    entity->sprite.enabled = true;
    entity->sprite.resourceIndex = resourceIndex;
    entity->sprite.frameIndex = 0;
    entity->sprite.sprite = createSprite(context, 1, w, h, vertices, 4, indices, 6);
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

    s32 spriteIndex = unitsData[type * 4 + 1];
    if (spriteIndex == 0)
    {
        fprintf(stderr, "Sprite for unit of type %d is not configure properly. Default to footman sprite.", type);
        spriteIndex = 279;
    }
    addSpriteComponent(context, entity, spriteIndex);
}

void addAnimationComponent(WarContext *context, WarEntity *entity, f32 duration, bool loop)
{
    entity->anim = (WarAnimationComponent){0};
    entity->anim.enabled = true;
    entity->anim.loop = loop;
    entity->anim.duration = duration;
    entity->anim.offset = 0;
}

void renderEntity(WarContext *context, WarEntity *entity, mat4 baseTransform)
{
    if (entity->id && entity->enabled)
    {
        mat4 model;
        glm_mat4_identity(model);
        glm_mat4_mul(baseTransform, model, model);

        WarTransformComponent transform = entity->transform;
        if (transform.enabled)
        {
            glm_translate(model, (vec4){transform.position[0], transform.position[1], 0.0f, 0.0f});

            if (entity->type == WAR_ENTITY_TYPE_UNIT)
            {
                WarUnitComponent unit = entity->unit;
                glm_translate(model, (vec4){-unit.sizex * MEGA_TILE_WIDTH * 0.5f, -unit.sizey * MEGA_TILE_HEIGHT * 0.5f, 0.0f, 0.0f});
            }

            glUniformMatrix4fv(context->modelLocation, 1, GL_FALSE, (f32*)model);
        }

        WarSpriteComponent sprite = entity->sprite;
        if (sprite.enabled)
        {
            u8 *pixels;

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
                    break;
                }

                default:
                {
                    fprintf(stderr, "Sprite component is enabled in a non-rendereable entity");
                    return;
                }
            }

            renderSprite(context, &sprite.sprite, pixels, model);
        }
    }
}