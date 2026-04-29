#pragma once

#include "war_types.h"

WarSpriteAnimation* wanim_createAnimation(WarContext* context, String name, WarSprite sprite, f32 frameDelay, bool loop);
WarSpriteAnimation* wanim_createAnimationFromResourceIndex(WarContext* context, String name, WarSpriteResourceRef spriteResourceRef, f32 frameDelay, bool loop);
void wanim_addAnimation(WarEntity* entity, WarSpriteAnimation* animation);
void wanim_addAnimationFrame(WarSpriteAnimation* animation, s32 frameIndex);
void wanim_addAnimationFrames(WarSpriteAnimation* animation, s32 count, s32 frameIndices[]);
void wanim_addAnimationFramesRange(WarSpriteAnimation* animation, s32 from, s32 to);
f32 wanim_getAnimationDuration(WarSpriteAnimation* animation);
void wanim_freeAnimation(WarSpriteAnimation* animation);
void wanim_removeAnimation(WarContext* context, WarEntity* entity, StringView name);
void wanim_updateAnimations(WarContext* context);
WarSpriteAnimation* wanim_findAnimation(WarContext* context, WarEntity* entity, StringView name);
bool wanim_containsAnimation(WarContext* context, WarEntity* entity, StringView name);
WarSpriteAnimation* wanim_createDamageAnimation(WarContext* context, WarEntity* entity, String name, int damageLevel);
WarSpriteAnimation* wanim_createCollapseAnimation(WarContext* context, WarEntity* entity, String name);
WarSpriteAnimation* wanim_createExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position);
WarSpriteAnimation* wanim_createRainOfFireExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position);
WarSpriteAnimation* wanim_createSpellAnimation(WarContext* context, WarEntity* entity, vec2 position);
WarSpriteAnimation* wanim_createPoisonCloudAnimation(WarContext* context, WarEntity* entity, vec2 position);
