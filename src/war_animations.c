WarSpriteAnimation* createAnimation(char* name, WarSprite sprite, f32 frameDelay, bool loop)
{
    WarSpriteAnimation* anim = (WarSpriteAnimation*)xmalloc(sizeof(WarSpriteAnimation));
    anim->name = name;
    anim->loop = loop;
    anim->offset = VEC2_ZERO;
    anim->scale = VEC2_ONE;
    anim->frameDelay = frameDelay;

    anim->sprite = sprite;
    WarS32ListInit(&anim->frames);

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
    WarS32ListAdd(&anim->frames, frameIndex);
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

s32 findAnimation(WarContext* context, WarEntity* entity, const char* name)
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

void removeAnimation(WarContext* context, WarEntity* entity, const char* name)
{
    s32 index = findAnimation(context, entity, name);
    if (index >= 0)
    {
        WarAnimationsComponent* animations = &entity->animations;
        WarSpriteAnimationListRemoveAt(&animations->animations, index);
    }
}