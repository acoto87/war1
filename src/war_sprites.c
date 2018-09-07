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

void renderSubSprite(WarContext *context, WarSprite *sprite, rect rs, rect rd, bool flipX, bool flipY)
{
    nvgRenderSubImage(context->gfx, sprite->image, rs, rd, flipX, flipY);
}

void renderSprite(WarContext *context, WarSprite *sprite, vec2 pos, bool flipX, bool flipY)
{
    rect rs = rectf(0, 0, sprite->frameWidth, sprite->frameHeight);
    rect rd = rectf(pos.x, pos.y, sprite->frameWidth, sprite->frameHeight);
    nvgRenderSubImage(context->gfx, sprite->image, rs, rd, flipX, flipY);
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
        s32 animFrameIndex = (s32)(anim->animTime * (anim->frameCount - 1));
        spriteFrameIndex = anim->frames[animFrameIndex];
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
    anim->frameCount = 0;

    resetAnimation(anim);

    WarSpriteAnimationListAdd(&entity->sprite.animations, anim);

    return anim;
}

void addAnimationFrame(WarSpriteAnimation* anim, s32 frameIndex)
{
    if (anim->frameCount < MAX_SPRITE_FRAME_COUNT)
    {
        anim->frames[anim->frameCount] = frameIndex;
        anim->frameCount++;    
    }
}

void updateAnimation(WarContext* context, WarSpriteComponent* sprite)
{
    if (!sprite->animationsEnabled)
        return;

    WarSpriteAnimation* anim = sprite->animations.items[sprite->currentAnimIndex];
    if (anim->status == WAR_ANIM_STATUS_FINISHED)
        return;

    anim->status = WAR_ANIM_STATUS_RUNNING;
    anim->animTime += (context->deltaTime / (anim->frameDelay * anim->frameCount));

    if (anim->animTime >= 1)
    {
        anim->animTime = 1;
        anim->status = WAR_ANIM_STATUS_FINISHED;

        if(anim->loop)
        {
            resetAnimation(anim);
        }
    }
}

void setSpriteAnimation(WarContext* context, WarEntity* entity, const char* name)
{
    WarSpriteComponent* sprite = &entity->sprite;
    
    s32 animIndex = -1;
    WarSpriteAnimation* anim = NULL;
    for(s32 i = 0; i < sprite->animations.count; i++)
    {
        anim = sprite->animations.items[i];
        if (strcmp(anim->name, name) == 0)
        {
            animIndex = i;
            break;
        }
    }
    
    if (anim && animIndex >= 0)
    {
        resetAnimation(anim);
        sprite->currentAnimIndex = animIndex;
    }
}

inline void enableAnimations(WarEntity* entity)
{
    entity->sprite.animationsEnabled = true;
}

inline void disableAnimations(WarEntity* entity)
{
    entity->sprite.animationsEnabled = true;
}
