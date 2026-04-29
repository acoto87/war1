#include "war_sprites.h"

#include <assert.h>

#include "SDL3/SDL.h"

#include "war_render.h"
#include "war_resources.h"

WarSprite wspr_createSprite(WarContext *context, u32 width, u32 height, u8 data[])
{
    WarSprite sprite = (WarSprite){0};
    assert(width <= UINT16_MAX);
    assert(height <= UINT16_MAX);
    sprite.frameWidth = width;
    sprite.frameHeight = height;
    sprite.framesCount = 1;

    sprite.texture = SDL_CreateTexture(context->renderer,
                                       SDL_PIXELFORMAT_RGBA32,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       width, height);
    if (sprite.texture)
    {
        SDL_SetTextureScaleMode(sprite.texture, SDL_SCALEMODE_NEAREST);
        if (data)
        {
            SDL_UpdateTexture(sprite.texture, NULL, data, width * 4);
        }
    }

    sprite.frames[0].dx = 0;
    sprite.frames[0].dy = 0;
    sprite.frames[0].w = (u16)width;
    sprite.frames[0].h = (u16)height;
    sprite.frames[0].off = 0;
    sprite.frames[0].data = (u8*)war_malloc(width * height * 4);

    if (data)
        memcpy(sprite.frames[0].data, data, width * height * 4);

    return sprite;
}

WarSprite wspr_createSpriteFromFrames(WarContext *context, u32 frameWidth, u32 frameHeight, u32 frameCount, WarSpriteFrame frames[])
{
    WarSprite sprite = (WarSprite){0};
    sprite.frameWidth = frameWidth;
    sprite.frameHeight = frameHeight;
    sprite.framesCount = frameCount;

    sprite.texture = SDL_CreateTexture(context->renderer,
                                       SDL_PIXELFORMAT_RGBA32,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       frameWidth, frameHeight);
    if (sprite.texture)
    {
        SDL_SetTextureScaleMode(sprite.texture, SDL_SCALEMODE_NEAREST);
    }

    for(u32 i = 0; i < frameCount; i++)
    {
        sprite.frames[i].dx = frames[i].dx;
        sprite.frames[i].dy = frames[i].dy;
        sprite.frames[i].w = frames[i].w;
        sprite.frames[i].h = frames[i].h;
        sprite.frames[i].off = 0;
        sprite.frames[i].data = (u8*)war_malloc(frameWidth * frameHeight * 4);

        if (frames[i].data)
            memcpy(sprite.frames[i].data, frames[i].data, frameWidth * frameHeight * 4);
    }

    return sprite;
}

WarSprite wspr_createSpriteFromResource(WarContext* context, WarResource* resource, s32 frameIndicesCount, s32 frameIndices[])
{
    assert(resource);

    WarSprite sprite = (WarSprite){0};

    switch(resource->type)
    {
        case WAR_RESOURCE_TYPE_IMAGE:
        {
            u32 width = resource->imageData.width;
            u32 height = resource->imageData.height;
            u8* data = resource->imageData.pixels;
            sprite = wspr_createSprite(context, width, height, data);
            break;
        }

        case WAR_RESOURCE_TYPE_SPRITE:
        {
            u32 frameWidth = resource->spriteData.frameWidth;
            u32 frameHeight = resource->spriteData.frameHeight;
            u32 framesCount = resource->spriteData.framesCount;
            WarSpriteFrame* frames = resource->spriteData.frames;

            if (frameIndicesCount > 0)
            {
                WarSpriteFrame* allFrames = frames;

                frames = (WarSpriteFrame*)war_malloc_frame(frameIndicesCount * sizeof(WarSpriteFrame));
                for (s32 i = 0; i < frameIndicesCount; i++)
                {
                    s32 frameIndex = frameIndices[i];
                    assert(frameIndex >= 0 && frameIndex < (s32)framesCount);

                    frames[i] = allFrames[frameIndex];
                }

                framesCount = frameIndicesCount;
            }

            sprite = wspr_createSpriteFromFrames(context, frameWidth, frameHeight, framesCount, frames);

            if (frameIndicesCount > 0)
            {
                // frames lives in frameZone — no explicit free needed
            }

            break;
        }

        case WAR_RESOURCE_TYPE_CURSOR:
        {
            u32 width = resource->cursor.width;
            u32 height = resource->cursor.height;
            u8* data = resource->cursor.pixels;
            sprite = wspr_createSprite(context, width, height, data);
            break;
        }

        default:
        {
            logWarning("Trying to load the resource of type %d as a sprite.", resource->type);
            assert(false);
            break;
        }
    }

    return sprite;
}

WarSprite wspr_createSpriteFromResourceIndex(WarContext* context, WarSpriteResourceRef spriteResourceRef)
{
    WarResource* resource = wres_getOrCreateResource(context, spriteResourceRef.resourceIndex);
    return wspr_createSpriteFromResource(context, resource, spriteResourceRef.frameIndicesCount, spriteResourceRef.frameIndices);
}

void wspr_updateSpriteImage(WarContext *context, WarSprite sprite, u8 data[])
{
    NOT_USED(context);

    if (!sprite.texture)
    {
        logWarning("Trying to update a sprite with no texture");
        return;
    }

    SDL_UpdateTexture(sprite.texture, NULL, data, sprite.frameWidth * 4);
}

void wspr_renderSubSprite(WarContext *context, WarSprite sprite, rect rs, rect rd, vec2 scale)
{
    if (!sprite.texture)
    {
        logWarning("Trying to render a sprite with no texture");
        return;
    }

    renderSubImage(context, sprite.texture, rs, rd, scale);
}

void wspr_renderSprite(WarContext *context, WarSprite sprite, vec2 pos, vec2 scale)
{
    if (!sprite.texture)
    {
        logWarning("Trying to render a sprite with no texture");
        return;
    }

    vec2 frameSize = vec2i(sprite.frameWidth, sprite.frameHeight);
    rect rs = rectv(VEC2_ZERO, frameSize);
    rect rd = rectv(pos, frameSize);
    renderSubImage(context, sprite.texture, rs, rd, scale);
}

WarSpriteFrame wspr_getSpriteFrame(WarContext* context, WarSprite sprite, s32 frameIndex)
{
    NOT_USED(context);

    assert(sprite.texture);
    assert(frameIndex >= 0 && frameIndex < (s32)sprite.framesCount);
    return sprite.frames[frameIndex];
}

void wspr_freeSprite(WarContext* context, WarSprite sprite)
{
    if (!sprite.texture)
    {
        logWarning("Trying to free a sprite with no texture");
        return;
    }

    for (s32 i = 0; i < sprite.framesCount; i++)
    {
        if (sprite.frames[i].data)
            war_free(sprite.frames[i].data);
    }

    SDL_DestroyTexture(sprite.texture);
}
