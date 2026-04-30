#pragma once

#include "SDL3/SDL.h"

#include "common.h"
#include "war.h"
#include "war_math.h"

struct _WarSpriteFrame
{
    u16 dx, dy;
    u16 w, h;
    u32 off;
    u8* data;
};

struct _WarSprite
{
    SDL_Texture* texture;
    s32 frameWidth;
    s32 frameHeight;
    s32 framesCount;
    WarSpriteFrame frames[MAX_SPRITE_FRAME_COUNT];
};

struct _WarSpriteResourceRef
{
    s32 resourceIndex;
    s32 frameIndicesCount;
    s32 frameIndices[MAX_SPRITE_FRAME_COUNT];
};

WarSpriteResourceRef createSpriteResourceRef(s32 resourceIndex, s32 frameIndicesCount, s32 frameIndices[])
{
    WarSpriteResourceRef spriteResourceRef = (WarSpriteResourceRef){0};
    spriteResourceRef.resourceIndex = resourceIndex;
    spriteResourceRef.frameIndicesCount = frameIndicesCount;
    if (frameIndices)
    {
        memcpy(spriteResourceRef.frameIndices, frameIndices, frameIndicesCount * sizeof(s32));
    }
    return spriteResourceRef;
}

#define invalidResourceRef() createSpriteResourceRef(-1, 0, NULL)
#define imageResourceRef(resourceIndex) createSpriteResourceRef((resourceIndex), 0, NULL)
#define spriteResourceRef(resourceIndex, spriteIndex) createSpriteResourceRef((resourceIndex), 1, arrayArg(s32, (spriteIndex)))

WarSprite wspr_createSprite(WarContext* context, u32 width, u32 height, u8 data[]);
WarSprite wspr_createSpriteFromFrames(WarContext* context, u32 frameWidth, u32 frameHeight, u32 frameCount, WarSpriteFrame frames[]);
WarSprite wspr_createSpriteFromResource(WarContext* context, WarResource* resource, s32 frameIndicesCount, s32 frameIndices[]);
WarSprite wspr_createSpriteFromResourceIndex(WarContext* context, WarSpriteResourceRef spriteResourceRef);
void wspr_updateSpriteImage(WarContext* context, WarSprite sprite, u8 data[]);
void wspr_renderSubSprite(WarContext* context, WarSprite sprite, rect rs, rect rd, vec2 scale);
void wspr_renderSprite(WarContext* context, WarSprite sprite, vec2 pos, vec2 scale);
WarSpriteFrame wspr_getSpriteFrame(WarContext* context, WarSprite sprite, s32 frameIndex);
void wspr_freeSprite(WarContext* context, WarSprite sprite);
