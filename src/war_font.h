#pragma once

#include "war_types.h"

#define NO_HIGHLIGHT -1
#define ALL_HIGHLIGHT -2

typedef struct
{
    s32 fontIndex;
    f32 fontSize;
    f32 lineHeight;
    WarColor fontColor;
    WarColor highlightColor;
    s32 highlightIndex;
    s32 highlightCount;
    vec2 boundings;
    WarTextAlignment horizontalAlign;
    WarTextAlignment verticalAlign;
    WarTextAlignment lineAlign;
    WarTextWrapping wrapping;
    WarTextTrimming trimming;

    WarSprite fontSprite;
    WarFontData fontData;
} WarFontParams;

WarSprite loadFontSprite(WarContext* context, StringView fontPath);
vec2 measureSingleSpriteText(StringView text, s32 length, WarFontParams params);
vec2 measureMultiSpriteText(StringView text, f32 width, WarFontParams params);
void renderSingleSpriteText(WarContext* context, StringView text, f32 x, f32 y, WarFontParams params);
void renderMultiSpriteText(WarContext* context, StringView text, f32 x, f32 y, WarFontParams params);
