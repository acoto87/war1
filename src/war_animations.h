#pragma once

#include "war_types.h"

WarSpriteAnimation* createAnimation(String name, WarSprite sprite, f32 frameDelay, bool loop);
WarSpriteAnimation* createAnimationFromResourceIndex(WarContext* context, String name, WarSpriteResourceRef spriteResourceRef, f32 frameDelay, bool loop);
void addAnimation(WarEntity* entity, WarSpriteAnimation* animation);
void addAnimationFrame(WarSpriteAnimation* animation, s32 frameIndex);
void addAnimationFrames(WarSpriteAnimation* animation, s32 count, s32 frameIndices[]);
void addAnimationFramesRange(WarSpriteAnimation* animation, s32 from, s32 to);
f32 getAnimationDuration(WarSpriteAnimation* animation);
void freeAnimation(WarSpriteAnimation* animation);
void removeAnimation(WarContext* context, WarEntity* entity, StringView name);
void updateAnimations(WarContext* context);
WarSpriteAnimation* findAnimation(WarContext* context, WarEntity* entity, StringView name);
bool containsAnimation(WarContext* context, WarEntity* entity, StringView name);
WarSpriteAnimation* createDamageAnimation(WarContext* context, WarEntity* entity, String name, int damageLevel);
WarSpriteAnimation* createCollapseAnimation(WarContext* context, WarEntity* entity, String name);
WarSpriteAnimation* createExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position);
WarSpriteAnimation* createRainOfFireExplosionAnimation(WarContext* context, WarEntity* entity, vec2 position);
WarSpriteAnimation* createSpellAnimation(WarContext* context, WarEntity* entity, vec2 position);
WarSpriteAnimation* createPoisonCloudAnimation(WarContext* context, WarEntity* entity, vec2 position);
