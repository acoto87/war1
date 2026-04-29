#pragma once

#include "war_types.h"

WarSprite wspr_createSprite(WarContext* context, u32 width, u32 height, u8 data[]);
WarSprite wspr_createSpriteFromFrames(WarContext* context, u32 frameWidth, u32 frameHeight, u32 frameCount, WarSpriteFrame frames[]);
WarSprite wspr_createSpriteFromResource(WarContext* context, WarResource* resource, s32 frameIndicesCount, s32 frameIndices[]);
WarSprite wspr_createSpriteFromResourceIndex(WarContext* context, WarSpriteResourceRef spriteResourceRef);
void wspr_updateSpriteImage(WarContext* context, WarSprite sprite, u8 data[]);
void wspr_renderSubSprite(WarContext* context, WarSprite sprite, rect rs, rect rd, vec2 scale);
void wspr_renderSprite(WarContext* context, WarSprite sprite, vec2 pos, vec2 scale);
WarSpriteFrame wspr_getSpriteFrame(WarContext* context, WarSprite sprite, s32 frameIndex);
void wspr_freeSprite(WarContext* context, WarSprite sprite);
