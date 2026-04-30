#pragma once

#include "war_math.h"
#include "war_color.h"
#include "war_sprites.h"

#define FONT_NORMAL_COLOR WAR_COLOR_RGBA(239, 255, 219, 255)
#define FONT_HIGHLIGHT_COLOR WAR_COLOR_RGBA(255, 227, 73, 255)
#define NO_HIGHLIGHT -1
#define ALL_HIGHLIGHT -2
#define TAB_WIDTH 2
#define MAX_LINES 48

struct _WarFontData
{
    s32 spriteWidth;
    s32 spriteHeight;
    s32 lineHeight;
    s32 advance;
    rect data[95];
};

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

WarSprite wfont_loadFontSprite(WarContext* context, StringView fontPath);
WarFontData getFontData(s32 fontIndex);
vec2 wfont_measureSingleSpriteText(StringView text, s32 length, WarFontParams params);
vec2 wfont_measureMultiSpriteText(StringView text, f32 width, WarFontParams params);
void wfont_renderSingleSpriteText(WarContext* context, StringView text, f32 x, f32 y, WarFontParams params);
void wfont_renderMultiSpriteText(WarContext* context, StringView text, f32 x, f32 y, WarFontParams params);
