#include "shl/wstr.h"

#include "war_animations.h"
#include "war_sprites.h"

#define ANIM_NAME_MAX_LENGTH 50

bool wanim_equalsSpriteAnimation(const WarSpriteAnimation* anim1, const WarSpriteAnimation* anim2)
{
    return wsv_equals(wstr_view(&anim1->name), wstr_view(&anim2->name));
}

shlDefineList(WarSpriteAnimationList, WarSpriteAnimation*)

WarSpriteAnimation* wanim_createAnimation(WarContext* context, String name, WarSprite sprite, f32 frameDelay, bool loop)
{
    NOT_USED(context);

    WarSpriteAnimation* anim = (WarSpriteAnimation*)wm_alloc(sizeof(WarSpriteAnimation));

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

WarSpriteAnimation* wanim_createAnimationFromResourceIndex(WarContext* context,
                                                     String name,
                                                     WarSpriteResourceRef spriteResourceRef,
                                                     f32 frameDelay,
                                                     bool loop)
{
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);
    return wanim_createAnimation(context, name, sprite, frameDelay, loop);
}

void wanim_addAnimation(WarEntity* entity, WarSpriteAnimation* animation)
{
    WarAnimationsComponent* animations = &entity->animations;
    WarSpriteAnimationListAdd(&animations->animations, animation);
}

void wanim_addAnimationFrame(WarSpriteAnimation* animation, s32 frameIndex)
{
    s32ListAdd(&animation->frames, frameIndex);
}

void wanim_addAnimationFrames(WarSpriteAnimation* animation, s32 count, s32 frameIndices[])
{
    if (frameIndices)
    {
        for(s32 i = 0; i < count; i++)
            wanim_addAnimationFrame(animation, frameIndices[i]);
    }
    else
    {
        for(s32 i = 0; i < count; i++)
            wanim_addAnimationFrame(animation, i);
    }
}

void wanim_addAnimationFramesRange(WarSpriteAnimation* animation, s32 from, s32 to)
{
    if (from < to)
    {
        for (s32 i = from; i <= to; i++)
            wanim_addAnimationFrame(animation, i);
    }
    else
    {
        for (s32 i = from; i >= to; i--)
            wanim_addAnimationFrame(animation, i);
    }
}

f32 wanim_getAnimationDuration(WarSpriteAnimation* animation)
{
    return animation->frameDelay * animation->frames.count;
}

void wanim_freeAnimation(WarSpriteAnimation* animation)
{
    if (animation)
    {
        s32ListFree(&animation->frames);
        wm_free(animation);
    }
}

s32 wanim_findAnimationIndex(WarContext* context, WarEntity* entity, StringView name)
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

void wanim_removeAnimation(WarContext* context, WarEntity* entity, StringView name)
{
    logInfo("Trying to remove animation: %s", name);

    s32 index = wanim_findAnimationIndex(context, entity, name);
    if (index >= 0)
    {
        WarAnimationsComponent* animations = &entity->animations;
        WarSpriteAnimationListRemoveAt(&animations->animations, index);
    }
}

void wanim_resetAnimation(WarSpriteAnimation* animation)
{
    animation->animTime = 0;
    animation->loopTime = 0;
    animation->status = WAR_ANIM_STATUS_NOT_STARTED;
}

void wanim_updateAnimation(WarContext* context, WarEntity* entity, WarSpriteAnimation* animation)
{
    if (animation->status == WAR_ANIM_STATUS_FINISHED)
    {
        wanim_removeAnimation(context, entity, wstr_view(&animation->name));
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

    f32 dt = context->deltaTime / wanim_getAnimationDuration(animation);

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
            wanim_resetAnimation(animation);
            animation->loopTime = animation->loopDelay;
            animation->status = WAR_ANIM_STATUS_RUNNING;
        }
    }
}

void wanim_updateAnimations(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateAnimations", 1);
    WarEntityList* entities = we_getEntities(context);
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
                    wanim_updateAnimation(context, entity, anim);
                }
            }
        }
    }
    TracyCZoneEnd(ctx);
}

WarSpriteAnimation* wanim_findAnimation(WarContext* context, WarEntity* entity, StringView name)
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

bool wanim_containsAnimation(WarContext* context, WarEntity* entity, StringView name)
{
    return wanim_findAnimationIndex(context, entity, name) >= 0;
}

WarSpriteAnimation* wanim_createDamageAnimation(WarContext* context, WarEntity* entity, String name, int damageLevel)
{
    s32 resourceIndex = damageLevel == 1 ? WAR_BUILDING_DAMAGE_1_RESOURCE : WAR_BUILDING_DAMAGE_2_RESOURCE;
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(resourceIndex);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);
    WarSpriteAnimation* anim = wanim_createAnimation(context, name, sprite, 0.2f, true);
    anim->offset = vec2Subv(wu_getUnitSpriteCenter(entity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));

    for(s32 i = 0; i < 4; i++)
        wanim_addAnimationFrame(anim, i);

    wanim_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wanim_createCollapseAnimation(WarContext* context, WarEntity* entity, String name)
{
    vec2 unitFrameSize = wu_getUnitFrameSize(entity);
    vec2 unitSpriteSize = wu_getUnitSpriteSize(entity);

    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_BUILDING_COLLAPSE_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);
    WarSpriteAnimation* anim = wanim_createAnimation(context, name, sprite, 0.1f, false);

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
        wanim_addAnimationFrame(anim, i);

    wanim_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wanim_createExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_EXPLOSION_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);

    String name = wstr_make();
    wstr_appendFormat(&name, "explosion_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = wanim_createAnimation(context, name, sprite, 0.1f, false);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 6; i++)
        wanim_addAnimationFrame(anim, i);

    wanim_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wanim_createRainOfFireExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_RAIN_OF_FIRE_EXPLOSION_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);

    String name = wstr_make();
    wstr_appendFormat(&name, "explosion_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = wanim_createAnimation(context, name, sprite, 0.1f, false);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 3; i < 6; i++)
        wanim_addAnimationFrame(anim, i);

    wanim_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wanim_createSpellAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_SPELL_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);

    String name = wstr_make();
    wstr_appendFormat(&name, "spell_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = wanim_createAnimation(context, name, sprite, 0.4f, false);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 6; i++)
        wanim_addAnimationFrame(anim, i);

    wanim_addAnimation(entity, anim);

    return anim;
}

WarSpriteAnimation* wanim_createPoisonCloudAnimation(WarContext* context, WarEntity* entity, vec2 position)
{
    WarSpriteResourceRef spriteResourceRef = imageResourceRef(WAR_POISON_CLOUD_RESOURCE);
    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);

    String name = wstr_make();
    wstr_appendFormat(&name, "poison_cloud_%.2f_%.2f", position.x, position.y);
    WarSpriteAnimation* anim = wanim_createAnimation(context, name, sprite, 0.5f, true);

    f32 offsetx = position.x - halff(sprite.frameWidth);
    f32 offsety = position.y - halff(sprite.frameHeight);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 4; i++)
        wanim_addAnimationFrame(anim, i);

    wanim_addAnimation(entity, anim);

    return anim;
}
