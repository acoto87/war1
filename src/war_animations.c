#include "war_animations.h"

#include <stdlib.h>

#include "shl/wstr.h"

#include "war_sprites.h"

#define ANIM_NAME_MAX_LENGTH 50

WarSpriteAnimation* wani_createAnimation(WarContext* context, String name, WarSprite sprite, f32 frameDelay, bool loop)
{
    WarSpriteAnimation* anim = (WarSpriteAnimation*)war_malloc(sizeof(WarSpriteAnimation));

    anim->name = name;
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

WarSpriteAnimation* wani_createAnimationFromResourceIndex(WarContext* context,
                                                     String name,
                                                     WarSpriteResourceRef spriteResourceRef,
                                                     f32 frameDelay,
                                                     bool loop)
{
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);
    return wani_createAnimation(context, name, sprite, frameDelay, loop);
}

void wani_addAnimation(WarEntity* entity, WarSpriteAnimation* animation)
{
    WarAnimationsComponent* animations = &entity->animations;
    WarSpriteAnimationListAdd(&animations->animations, animation);
}

void wani_addAnimationFrame(WarSpriteAnimation* animation, s32 frameIndex)
{
    s32ListAdd(&animation->frames, frameIndex);
}

void wani_addAnimationFrames(WarSpriteAnimation* animation, s32 count, s32 frameIndices[])
{
    if (frameIndices)
    {
        for(s32 i = 0; i < count; i++)
            wani_addAnimationFrame(animation, frameIndices[i]);
    }
    else
    {
        for(s32 i = 0; i < count; i++)
            wani_addAnimationFrame(animation, i);
    }
}

void wani_addAnimationFramesRange(WarSpriteAnimation* animation, s32 from, s32 to)
{
    if (from < to)
    {
        for (s32 i = from; i <= to; i++)
            wani_addAnimationFrame(animation, i);
    }
    else
    {
        for (s32 i = from; i >= to; i--)
            wani_addAnimationFrame(animation, i);
    }
}

f32 wani_getAnimationDuration(WarSpriteAnimation* animation)
{
    return animation->frameDelay * animation->frames.count;
}

void wani_freeAnimation(WarSpriteAnimation* animation)
{
    if (animation)
    {
        s32ListFree(&animation->frames);
        war_free(animation);
    }
}

s32 wani_findAnimationIndex(WarContext* context, WarEntity* entity, StringView name)
{
    NOT_USED(context);

    s32 index = -1;

    WarAnimationsComponent* animations = &entity->animations;
    for(s32 i = 0; i < animations->animations.count; i++)
    {
        WarSpriteAnimation* anim = animations->animations.items[i];
        if (wsv_equals(wstr_view(&anim->name), name))
        {
            index = i;
            break;
        }
    }

    return index;
}

void wani_removeAnimation(WarContext* context, WarEntity* entity, StringView name)
{
    logInfo("Trying to remove animation: %s", name);

    s32 index = wani_findAnimationIndex(context, entity, name);
    if (index >= 0)
    {
        WarAnimationsComponent* animations = &entity->animations;
        WarSpriteAnimationListRemoveAt(&animations->animations, index);
    }
}

void wani_resetAnimation(WarSpriteAnimation* animation)
{
    animation->animTime = 0;
    animation->loopTime = 0;
    animation->status = WAR_ANIM_STATUS_NOT_STARTED;
}

void wani_updateAnimation(WarContext* context, WarEntity* entity, WarSpriteAnimation* animation)
{
    if (animation->status == WAR_ANIM_STATUS_FINISHED)
    {
        wani_removeAnimation(context, entity, wstr_view(&animation->name));
        return;
    }

    animation->status = WAR_ANIM_STATUS_RUNNING;

    if (animation->loopTime > 0)
    {
        if (context->scene)
            animation->loopTime -= getScaledSpeed(context, context->deltaTime);
        else if (context->map)
            animation->loopTime -= wmap_getMapScaledSpeed(context, context->deltaTime);

        return;
    }

    f32 dt = context->deltaTime / wani_getAnimationDuration(animation);

    if (context->scene)
        dt = getScaledSpeed(context, dt);
    else if (context->map)
        dt = wmap_getMapScaledSpeed(context, dt);

    animation->animTime += dt;

    if (animation->animTime >= 1)
    {
        animation->animTime = 1;
        animation->status = WAR_ANIM_STATUS_FINISHED;

        if(animation->loop)
        {
            wani_resetAnimation(animation);
            animation->loopTime = animation->loopDelay;
            animation->status = WAR_ANIM_STATUS_RUNNING;
        }
    }
}

void wani_updateAnimations(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateAnimations", 1);
    WarEntityList* entities = getEntities(context);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity* entity = entities->items[i];
        if (entity)
        {
            WarAnimationsComponent* animations = &entity->animations;
            if (animations->enabled)
            {
                for(s32 k = 0; k < animations->animations.count; k++)
                {
                    WarSpriteAnimation* anim = animations->animations.items[k];
                    wani_updateAnimation(context, entity, anim);
                }
            }
        }
    }
    TracyCZoneEnd(ctx);
}

WarSpriteAnimation* wani_findAnimation(WarContext* context, WarEntity* entity, StringView name)
{
    NOT_USED(context);

    WarAnimationsComponent* animations = &entity->animations;
    for(s32 i = 0; i < animations->animations.count; i++)
    {
        WarSpriteAnimation* anim = animations->animations.items[i];
        if (wsv_equals(wstr_view(&anim->name), name))
        {
            return anim;
        }
    }

    return NULL;
}

bool wani_containsAnimation(WarContext* context, WarEntity* entity, StringView name)
{
    return wani_findAnimationIndex(context, entity, name) >= 0;
}

WarSpriteAnimation* wani_createDamageAnimation(WarContext* context, WarEntity* entity, String name, int damageLevel)
{
    s32 resourceIndex = damageLevel == 1 ? WAR_BUILDING_DAMAGE_1_RESOURCE : WAR_BUILDING_DAMAGE_2_RESOURCE;
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(resourceIndex);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);
    WarSpriteAnimation* anim = wani_createAnimation(context, name, sprite, 0.2f, true);
    anim->offset = vec2Subv(getUnitSpriteCenter(entity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));

    for(s32 i = 0; i < 4; i++)
        wani_addAnimationFrame(anim, i);

    wani_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wani_createCollapseAnimation(WarContext* context, WarEntity* entity, String name)
{
    vec2 unitFrameSize = getUnitFrameSize(entity);
    vec2 unitSpriteSize = getUnitSpriteSize(entity);

    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_BUILDING_COLLAPSE_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);
    WarSpriteAnimation* anim = wani_createAnimation(context, name, sprite, 0.1f, false);

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
        wani_addAnimationFrame(anim, i);

    wani_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wani_createExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_EXPLOSION_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);

    String name = wstr_make();
    wstr_appendFormat(&name, "explosion_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = wani_createAnimation(context, name, sprite, 0.1f, false);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 6; i++)
        wani_addAnimationFrame(anim, i);

    wani_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wani_createRainOfFireExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_RAIN_OF_FIRE_EXPLOSION_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);

    String name = wstr_make();
    wstr_appendFormat(&name, "explosion_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = wani_createAnimation(context, name, sprite, 0.1f, false);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 3; i < 6; i++)
        wani_addAnimationFrame(anim, i);

    wani_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wani_createSpellAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_SPELL_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);

    String name = wstr_make();
    wstr_appendFormat(&name, "spell_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = wani_createAnimation(context, name, sprite, 0.4f, false);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 6; i++)
        wani_addAnimationFrame(anim, i);

    wani_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wani_createPoisonCloudAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_POISON_CLOUD_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);

    String name = wstr_make();
    wstr_appendFormat(&name, "poison_cloud_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = wani_createAnimation(context, name, sprite, 0.5f, true);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 4; i++)
        wani_addAnimationFrame(anim, i);

    wani_addAnimation(entity, anim);

    return anim;
}
