#define ANIM_NAME_MAX_LENGTH 50

WarSpriteAnimation* createAnimation(const char* name, WarSprite sprite, f32 frameDelay, bool loop)
{
    WarSpriteAnimation* anim = (WarSpriteAnimation*)xmalloc(sizeof(WarSpriteAnimation));

    anim->name = (char*)xcalloc(strlen(name) + 1, sizeof(char));
    strcpy(anim->name, name);

    anim->loop = loop;
    anim->offset = VEC2_ZERO;
    anim->scale = VEC2_ONE;
    anim->frameDelay = frameDelay;
    anim->sprite = sprite;
    anim->animTime = 0;
    anim->loopTime = 0;
    anim->status = WAR_ANIM_STATUS_NOT_STARTED;

    s32ListInit(&anim->frames, s32ListDefaultOptions);

    return anim;
}

WarSpriteAnimation* createAnimationFromResourceIndex(WarContext* context,
                                                     const char* name,
                                                     WarSpriteResourceRef spriteResourceRef,
                                                     f32 frameDelay,
                                                     bool loop)
{
    WarSprite sprite = createSpriteFromResourceIndex(context, spriteResourceRef);
    return createAnimation(name, sprite, frameDelay, loop);
}

void addAnimation(WarEntity* entity, WarSpriteAnimation* animation)
{
    WarAnimationsComponent* animations = &entity->animations;
    WarSpriteAnimationListAdd(&animations->animations, animation);
}

void addAnimationFrame(WarSpriteAnimation* animation, s32 frameIndex)
{
    s32ListAdd(&animation->frames, frameIndex);
}

void addAnimationFrames(WarSpriteAnimation* animation, s32 count, s32 frameIndices[])
{
    if (frameIndices)
    {
        for(s32 i = 0; i < count; i++)
            addAnimationFrame(animation, frameIndices[i]);
    }
    else
    {
        for(s32 i = 0; i < count; i++)
            addAnimationFrame(animation, i);
    }
}

void addAnimationFramesRange(WarSpriteAnimation* animation, s32 from, s32 to)
{
    if (from < to)
    {
        for (s32 i = from; i <= to; i++)
            addAnimationFrame(animation, i);
    }
    else
    {
        for (s32 i = from; i >= to; i--)
            addAnimationFrame(animation, i);
    }
}

f32 getAnimationDuration(WarSpriteAnimation* animation)
{
    return animation->frameDelay * animation->frames.count;
}

void freeAnimation(WarSpriteAnimation* animation)
{
    logInfo("Freeing animation: %s\n", animation->name);

    free(animation->name);

    s32ListFree(&animation->frames);

    WarSprite* sprite = &animation->sprite;
    for(s32 i = 0; i < sprite->framesCount; i++)
        free(sprite->frames[i].data);

    free(animation);
}

s32 findAnimationIndex(WarContext* context, WarEntity* entity, const char* name)
{
    s32 index = -1;

    WarAnimationsComponent* animations = &entity->animations;
    for(s32 i = 0; i < animations->animations.count; i++)
    {
        WarSpriteAnimation* anim = animations->animations.items[i];
        if (strEquals(anim->name, name))
        {
            index = i;
            break;
        }
    }

    return index;
}

void removeAnimation(WarContext* context, WarEntity* entity, const char* name)
{
    logInfo("Trying to remove animation: %s\n", name);

    s32 index = findAnimationIndex(context, entity, name);
    if (index >= 0)
    {
        WarAnimationsComponent* animations = &entity->animations;
        WarSpriteAnimationListRemoveAt(&animations->animations, index);
    }
}

void resetAnimation(WarSpriteAnimation* animation)
{
    animation->animTime = 0;
    animation->loopTime = 0;
    animation->status = WAR_ANIM_STATUS_NOT_STARTED;
}

void updateAnimation(WarContext* context, WarEntity* entity, WarSpriteAnimation* animation)
{
    if (animation->status == WAR_ANIM_STATUS_FINISHED)
    {
        removeAnimation(context, entity, animation->name);
        return;
    }

    animation->status = WAR_ANIM_STATUS_RUNNING;

    if (animation->loopTime > 0)
    {
        if (context->scene)
            animation->loopTime -= getScaledSpeed(context, context->deltaTime);
        else if (context->map)
            animation->loopTime -= getMapScaledSpeed(context, context->deltaTime);

        return;
    }

    f32 dt = context->deltaTime / getAnimationDuration(animation);

    if (context->scene)
        dt = getScaledSpeed(context, dt);
    else if (context->map)
        dt = getMapScaledSpeed(context, dt);

    animation->animTime += dt;

    if (animation->animTime >= 1)
    {
        animation->animTime = 1;
        animation->status = WAR_ANIM_STATUS_FINISHED;

        if(animation->loop)
        {
            resetAnimation(animation);
            animation->loopTime = animation->loopDelay;
            animation->status = WAR_ANIM_STATUS_RUNNING;
        }
    }
}

void updateAnimations(WarContext* context)
{
    WarEntityList* entities = getEntities(context);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity* entity = entities->items[i];
        if (entity)
        {
            WarAnimationsComponent* animations = &entity->animations;
            if (animations->enabled)
            {
                for(s32 i = 0; i < animations->animations.count; i++)
                {
                    WarSpriteAnimation* anim = animations->animations.items[i];
                    updateAnimation(context, entity, anim);
                }
            }
        }
    }
}

WarSpriteAnimation* findAnimation(WarContext* context, WarEntity* entity, const char* name)
{
    WarAnimationsComponent* animations = &entity->animations;
    for(s32 i = 0; i < animations->animations.count; i++)
    {
        WarSpriteAnimation* anim = animations->animations.items[i];
        if (strEquals(anim->name, name))
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

WarSpriteAnimation* createDamageAnimation(WarContext* context, WarEntity* entity, char* name, int damageLevel)
{
    s32 resourceIndex = damageLevel == 1 ? WAR_BUILDING_DAMAGE_1_RESOURCE : WAR_BUILDING_DAMAGE_2_RESOURCE;
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(resourceIndex);
    WarSprite sprite = createSpriteFromResourceIndex(context, spriteResourceRef);
    WarSpriteAnimation* anim = createAnimation(name, sprite, 0.2f, true);
    anim->offset = vec2Subv(getUnitSpriteCenter(entity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));

    for(s32 i = 0; i < 4; i++)
        addAnimationFrame(anim, i);

    addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* createCollapseAnimation(WarContext* context, WarEntity* entity, const char* name)
{
    vec2 unitFrameSize = getUnitFrameSize(entity);
    vec2 unitSpriteSize = getUnitSpriteSize(entity);

    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_BUILDING_COLLAPSE_RESOURCE);
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

WarSpriteAnimation* createExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_EXPLOSION_RESOURCE);
    WarSprite sprite = createSpriteFromResourceIndex(context, spriteResourceRef);

    char name[ANIM_NAME_MAX_LENGTH];
    sprintf(name, "explosion_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = createAnimation(name, sprite, 0.1f, false);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 6; i++)
        addAnimationFrame(anim, i);

    addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* createRainOfFireExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_RAIN_OF_FIRE_EXPLOSION_RESOURCE);
    WarSprite sprite = createSpriteFromResourceIndex(context, spriteResourceRef);

    char name[ANIM_NAME_MAX_LENGTH];
    sprintf(name, "explosion_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = createAnimation(name, sprite, 0.1f, false);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 3; i < 6; i++)
        addAnimationFrame(anim, i);

    addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* createSpellAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_SPELL_RESOURCE);
    WarSprite sprite = createSpriteFromResourceIndex(context, spriteResourceRef);

    char name[ANIM_NAME_MAX_LENGTH];
    sprintf(name, "spell_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = createAnimation(name, sprite, 0.4f, false);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 6; i++)
        addAnimationFrame(anim, i);

    addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* createPoisonCloudAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_POISON_CLOUD_RESOURCE);
    WarSprite sprite = createSpriteFromResourceIndex(context, spriteResourceRef);

    char name[ANIM_NAME_MAX_LENGTH];
    sprintf(name, "poison_cloud_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = createAnimation(name, sprite, 0.5f, true);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 4; i++)
        addAnimationFrame(anim, i);

    addAnimation(entity, anim);

    return anim;
}
