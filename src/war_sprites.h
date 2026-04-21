#pragma once

#include "war_types.h"

WarSprite createSprite(WarContext* context, u32 width, u32 height, u8 data[]);
WarSprite createSpriteFromFrames(WarContext* context, u32 frameWidth, u32 frameHeight, u32 frameCount, WarSpriteFrame frames[]);
WarSprite createSpriteFromResource(WarContext* context, WarResource* resource, s32 frameIndicesCount, s32 frameIndices[]);
WarSprite createSpriteFromResourceIndex(WarContext* context, WarSpriteResourceRef spriteResourceRef);
void updateSpriteImage(WarContext* context, WarSprite sprite, u8 data[]);
void renderSubSprite(WarContext* context, WarSprite sprite, rect rs, rect rd, vec2 scale);
void renderSprite(WarContext* context, WarSprite sprite, vec2 pos, vec2 scale);
WarSpriteFrame getSpriteFrame(WarContext* context, WarSprite sprite, s32 frameIndex);
void freeSprite(WarContext* context, WarSprite sprite);
