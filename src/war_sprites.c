WarSprite createSprite(WarContext *context, u32 width, u32 height, u8 data[])
{
    WarSprite sprite = (WarSprite){0};
    sprite.frameWidth = width;
    sprite.frameHeight = height;
    sprite.framesCount = 1;
    
    sprite.image = nvgCreateImageRGBA(context->gfx, width, height, NVG_IMAGE_NEAREST, data);

    sprite.frames[0].dx = 0;
    sprite.frames[0].dy = 0;
    sprite.frames[0].w = width;
    sprite.frames[0].h = height;
    sprite.frames[0].off = 0;
    sprite.frames[0].data = (u8*)malloc(width * height * 4);

    if (data)
        memcpy(sprite.frames[0].data, data, width * height * 4);
    
    return sprite;
}

WarSprite createSpriteFrames(WarContext *context, u32 frameWidth, u32 frameHeight, u32 frameCount, WarSpriteFrame frames[])
{
    WarSprite sprite = (WarSprite){0};
    sprite.frameWidth = frameWidth;
    sprite.frameHeight = frameHeight;
    sprite.framesCount = frameCount;

    sprite.image = nvgCreateImageRGBA(context->gfx, frameWidth, frameHeight, NVG_IMAGE_NEAREST, NULL);

    for(s32 i = 0; i < frameCount; i++)
    {
        sprite.frames[i].dx = frames[i].dx;
        sprite.frames[i].dy = frames[i].dy;
        sprite.frames[i].w = frames[i].w;
        sprite.frames[i].h = frames[i].h;
        sprite.frames[i].off = 0;
        sprite.frames[i].data = (u8*)malloc(frameWidth * frameHeight * 4);

        if (frames[i].data)
            memcpy(sprite.frames[i].data, frames[i].data, frameWidth * frameHeight * 4);
    }
    
    return sprite;
}

void updateSpriteImage(WarContext *context, WarSprite *sprite, u8 data[])
{
    nvgUpdateImage(context->gfx, sprite->image, data);
}

void renderSubSprite(WarContext *context, WarSprite *sprite, rect rs, rect rd, vec2 scale)
{
    nvgRenderSubImage(context->gfx, sprite->image, rs, rd, scale);
}

void renderSprite(WarContext *context, WarSprite *sprite, vec2 pos, vec2 scale)
{
    rect rs = rectf(0, 0, sprite->frameWidth, sprite->frameHeight);
    rect rd = rectf(pos.x, pos.y, sprite->frameWidth, sprite->frameHeight);
    nvgRenderSubImage(context->gfx, sprite->image, rs, rd, scale);
}

WarSpriteAnimation* getCurrentAnimation(WarSpriteComponent* sprite)
{
    if (!inRange(sprite->currentAnimIndex, 0, sprite->animations.count))
    {
        return NULL;
    }
    
    return sprite->animations.items[sprite->currentAnimIndex];
}

WarSpriteFrame* getSpriteFrame(WarContext* context, WarSpriteComponent* sprite)
{
    s32 spriteFrameIndex = 0;

    if (sprite->animationsEnabled)
    {
        WarSpriteAnimation* anim = getCurrentAnimation(sprite);
        if (anim)
        {
            s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
            animFrameIndex = clamp(animFrameIndex, 0, anim->frames.count - 1);
            spriteFrameIndex = anim->frames.items[animFrameIndex];
        }
    }
    
    return &sprite->sprite.frames[spriteFrameIndex];
}

void resetAnimation(WarSpriteAnimation* anim)
{
    anim->animTime = 0;
    anim->status = WAR_ANIM_STATUS_NOT_STARTED;
}

WarSpriteAnimation* addSpriteAnimation(WarEntity* entity, char* name, f32 frameDelay, bool loop)
{
    WarSpriteAnimation* anim = (WarSpriteAnimation*)xmalloc(sizeof(WarSpriteAnimation));
    anim->name = name;
    anim->status = WAR_ANIM_STATUS_NOT_STARTED;
    anim->loop = loop;
    anim->flipX = false;
    anim->flipY = false;
    anim->frameDelay = frameDelay;
    anim->animTime = 0;
    anim->status = WAR_ANIM_STATUS_NOT_STARTED;
    WarS32ListInit(&anim->frames);

    WarSpriteAnimationListAdd(&entity->sprite.animations, anim);

    return anim;
}

void addAnimationFrame(WarSpriteAnimation* anim, s32 frameIndex)
{
    WarS32ListAdd(&anim->frames, frameIndex);
}

void addAnimationFrames(WarSpriteAnimation* anim, s32 count, s32 frameIndices[])
{
    for(s32 i = 0; i < count; i++)
    {
        addAnimationFrame(anim, frameIndices[i]);
    }
}

void updateAnimation(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    WarTransformComponent* transform = &entity->transform;
    if (sprite->enabled && sprite->animationsEnabled)
    {
        WarSpriteAnimation* anim = getCurrentAnimation(sprite);
        if (!anim || anim->status == WAR_ANIM_STATUS_FINISHED)
        {
            return;
        }

        anim->status = WAR_ANIM_STATUS_RUNNING;
        anim->animTime += (context->deltaTime / (anim->frameDelay * anim->frames.count));

        if (anim->animTime >= 1)
        {
            anim->animTime = 1;
            anim->status = WAR_ANIM_STATUS_FINISHED;

            if(anim->loop)
            {
                resetAnimation(anim);
            }
        }

        transform->scale.x = anim->flipX ? -1 : 1;
        transform->scale.y = anim->flipY ? -1 : 1;
    }
}

s32 findAnimation(WarSpriteComponent* sprite, const char* name, WarUnitDirection direction)
{
    s32 index = -1;

    for(s32 i = 0; i < sprite->animations.count; i++)
    {
        WarSpriteAnimation* anim = sprite->animations.items[i];
        if (strcmp(anim->name, name) == 0 && anim->direction == direction)
        {
            index = i;
            break;
        }
    }

    return index;
}

void setDirectionalAnimation(WarContext* context, WarEntity* entity, const char* name, WarUnitDirection direction, bool reset)
{
    WarSpriteComponent* sprite = &entity->sprite;
    
    s32 animIndex = findAnimation(sprite, name, direction);
    if (animIndex >= 0)
    {
        WarSpriteAnimation* anim = sprite->animations.items[animIndex];
        assert(anim);
        
        if (reset)
        {
            resetAnimation(anim);
        }

        sprite->currentAnimIndex = animIndex;
    }
}

void setAnimation(WarContext* context, WarEntity* entity, const char* name, bool reset)
{
    setDirectionalAnimation(context, entity, name, WAR_DIRECTION_NORTH, reset);
}

inline void enableAnimations(WarEntity* entity)
{
    entity->sprite.animationsEnabled = true;
}

inline void disableAnimations(WarEntity* entity)
{
    entity->sprite.animationsEnabled = true;
}
