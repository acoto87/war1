#pragma once

#include "war_types.h"

WarSpriteAnimation* wani_createAnimation(WarContext* context, String name, WarSprite sprite, f32 frameDelay, bool loop);
WarSpriteAnimation* wani_createAnimationFromResourceIndex(WarContext* context, String name, WarSpriteResourceRef spriteResourceRef, f32 frameDelay, bool loop);
void wani_addAnimation(WarEntity* entity, WarSpriteAnimation* animation);
void wani_addAnimationFrame(WarSpriteAnimation* animation, s32 frameIndex);
void wani_addAnimationFrames(WarSpriteAnimation* animation, s32 count, s32 frameIndices[]);
void wani_addAnimationFramesRange(WarSpriteAnimation* animation, s32 from, s32 to);
f32 wani_getAnimationDuration(WarSpriteAnimation* animation);
void wani_freeAnimation(WarSpriteAnimation* animation);
void wani_removeAnimation(WarContext* context, WarEntity* entity, StringView name);
void wani_updateAnimations(WarContext* context);
WarSpriteAnimation* wani_findAnimation(WarContext* context, WarEntity* entity, StringView name);
bool wani_containsAnimation(WarContext* context, WarEntity* entity, StringView name);
WarSpriteAnimation* wani_createDamageAnimation(WarContext* context, WarEntity* entity, String name, int damageLevel);
WarSpriteAnimation* wani_createCollapseAnimation(WarContext* context, WarEntity* entity, String name);
WarSpriteAnimation* wani_createExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position);
WarSpriteAnimation* wani_createRainOfFireExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position);
WarSpriteAnimation* wani_createSpellAnimation(WarContext* context, WarEntity* entity, vec2 position);
WarSpriteAnimation* wani_createPoisonCloudAnimation(WarContext* context, WarEntity* entity, vec2 position);
