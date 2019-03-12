WarSpriteAnimation* createAnimation(char* name, WarSprite sprite, f32 frameDelay, bool loop)
{
    WarSpriteAnimation* anim = (WarSpriteAnimation*)xmalloc(sizeof(WarSpriteAnimation));
    anim->name = name;
    anim->loop = loop;
    anim->offset = VEC2_ZERO;
    anim->scale = VEC2_ONE;
    anim->frameDelay = frameDelay;

    anim->sprite = sprite;

    s32ListInit(&anim->frames, s32ListDefaultOptions);

    anim->animTime = 0;
    anim->status = WAR_ANIM_STATUS_NOT_STARTED;
    return anim;
}

void addAnimation(WarEntity* entity, WarSpriteAnimation* animation)
{
    WarAnimationsComponent* animations = &entity->animations;
    WarSpriteAnimationListAdd(&animations->animations, animation);
}

void addAnimationFrame(WarSpriteAnimation* anim, s32 frameIndex)
{
    s32ListAdd(&anim->frames, frameIndex);
}

void addAnimationFrames(WarSpriteAnimation* anim, s32 count, s32 frameIndices[])
{
    for(s32 i = 0; i < count; i++)
    {
        addAnimationFrame(anim, frameIndices[i]);
    }
}

void resetAnimation(WarSpriteAnimation* anim)
{
    anim->animTime = 0;
    anim->status = WAR_ANIM_STATUS_NOT_STARTED;
}

void updateAnimation(WarContext* context, WarSpriteAnimation* anim)
{
    if (anim->status == WAR_ANIM_STATUS_FINISHED)
    {
        return;
    }

    anim->status = WAR_ANIM_STATUS_RUNNING;
    anim->animTime += getScaledSpeed(context, context->deltaTime / (anim->frameDelay * anim->frames.count));

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

void updateAnimations(WarContext* context, WarEntity* entity)
{
    WarAnimationsComponent* animations = &entity->animations;
    if (animations->enabled)
    {
        for(s32 i = 0; i < animations->animations.count; i++)
        {
            WarSpriteAnimation* anim = animations->animations.items[i];
            updateAnimation(context, anim);
        }
    }
}

s32 findAnimationIndex(WarContext* context, WarEntity* entity, const char* name)
{
    s32 index = -1;

    WarAnimationsComponent* animations = &entity->animations;
    for(s32 i = 0; i < animations->animations.count; i++)
    {
        WarSpriteAnimation* anim = animations->animations.items[i];
        if (strcmp(anim->name, name) == 0)
        {
            index = i;
            break;
        }
    }

    return index;
}

WarSpriteAnimation* findAnimation(WarContext* context, WarEntity* entity, const char* name)
{
    WarAnimationsComponent* animations = &entity->animations;
    for(s32 i = 0; i < animations->animations.count; i++)
    {
        WarSpriteAnimation* anim = animations->animations.items[i];
        if (strcmp(anim->name, name) == 0)
        {
            return anim;
        }
    }

    return NULL;
}

bool containsAnimation(WarContext* context, WarEntity* entity, const char* name)
{
    return findAnimationIndex(context, entity, name) >= 0;
}

void freeAnimation(WarSpriteAnimation* animation)
{
    // here it crash, this doesn't need to be freed if is assigned like animation->name = "anim"?
    // free(animation->name);

    s32ListFree(&animation->frames);
    
    WarSprite* sprite = &animation->sprite;
    for(s32 i = 0; i < sprite->framesCount; i++)
        free(sprite->frames[i].data);

    free(animation);
}

void removeAnimation(WarContext* context, WarEntity* entity, const char* name)
{
    s32 index = findAnimationIndex(context, entity, name);
    if (index >= 0)
    {
        WarAnimationsComponent* animations = &entity->animations;
        WarSpriteAnimationListRemoveAt(&animations->animations, index);
    }
}

WarSpriteAnimation* createDamageAnimation(WarContext* context, WarEntity* entity, char* name, int damageLevel)
{
    s32 resourceIndex = damageLevel == 1 ? BUILDING_DAMAGE_1_RESOURCE : BUILDING_DAMAGE_2_RESOURCE;
    WarSpriteResourceRef spriteResourceRef = createSpriteResourceRef(resourceIndex, 0, NULL);
    WarSprite sprite = createSpriteFromResourceIndex(context, spriteResourceRef);
    WarSpriteAnimation* anim = createAnimation(name, sprite, 0.2f, true);
    anim->offset = vec2Subv(getUnitSpriteCenter(entity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));
    
    for(s32 i = 0; i < 4; i++)
        addAnimationFrame(anim, i);
    
    addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* createCollapseAnimation(WarContext* context, WarEntity* entity, char* name)
{
    vec2 unitFrameSize = getUnitFrameSize(entity);
    vec2 unitSpriteSize = getUnitSpriteSize(entity);

    WarSpriteResourceRef spriteResourceRef = createSpriteResourceRef(BUILDING_COLLAPSE_RESOURCE, 0, NULL);
    WarSprite sprite = createSpriteFromResourceIndex(context, spriteResourceRef);
    WarSpriteAnimation* anim = createAnimation(name, sprite, 0.1f, false);

    vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

    // this is the scale of the explosion animation sprites with respect to the size of the building
    f32 animScale = unitSpriteSize.x / animFrameSize.x;

    // if the offset is based on the size of the frame, and it's scaled, then the offset must take into
    // account the scale to make the calculations
    f32 offsetx = halff(unitFrameSize.x - unitSpriteSize.x);
    f32 offsety = halff(unitFrameSize.y - unitSpriteSize.y) - (animFrameSize.y * animScale - unitSpriteSize.y);

    anim->scale = vec2f(animScale, animScale);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 17; i++)
        addAnimationFrame(anim, i);
    
    addAnimation(entity, anim);

    return anim;
}