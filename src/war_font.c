#include "war_font.h"

#include "war_sprites.h"
#include "shl/wstr.h"

// the fonts was created in a 64x64 (64x96 for the second font)
// sprite but for the actual game a 512x512 (512x768) was used,
// so the original dimensions of this array are actually multiplied by 8
//
WarFontData fontsData[2] =
{
    // font 0 for in game texts
    {
        512, 512, 48, 8,
        {
            {   0,   0, 24, 48 }, //
            {  40,   0, 16, 48 }, // !
            {  56,   0, 32, 48 }, // "
            {  88,   0, 48, 48 }, // #
            { 136,   0, 40, 48 }, // $
            { 176,   0, 48, 48 }, // %
            { 224,   0, 40, 48 }, // &
            { 264,   0, 24, 48 }, // '
            { 288,   0, 24, 48 }, // (
            { 312,   0, 24, 48 }, // )
            { 336,   0, 32, 48 }, // *
            { 368,   0, 32, 48 }, // +
            { 400,   0, 24, 48 }, // '
            { 424,   0, 32, 48 }, // -
            { 456,   0, 16, 48 }, // .
            {   0,  48, 48, 48 }, // /
            {  48,  48, 40, 48 }, // 0
            {  88,  48, 32, 48 }, // 1
            { 120,  48, 40, 48 }, // 2
            { 160,  48, 40, 48 }, // 3
            { 200,  48, 40, 48 }, // 4
            { 240,  48, 40, 48 }, // 5
            { 280,  48, 40, 48 }, // 6
            { 320,  48, 40, 48 }, // 7
            { 360,  48, 40, 48 }, // 8
            { 400,  48, 40, 48 }, // 9
            { 440,  48, 32, 48 }, // :
            { 472,  48, 16, 48 }, // ;
            {   0,  96, 32, 48 }, // <
            {  32,  96, 32, 48 }, // =
            {  64,  96, 32, 48 }, // >
            {  96,  96, 40, 48 }, // ?
            { 136,  96, 48, 48 }, // @
            { 184,  96, 40, 48 }, // A
            { 224,  96, 40, 48 }, // B
            { 264,  96, 40, 48 }, // C
            { 304,  96, 40, 48 }, // D
            { 344,  96, 40, 48 }, // E
            { 384,  96, 40, 48 }, // F
            { 424,  96, 40, 48 }, // G
            { 464,  96, 40, 48 }, // H
            {   0, 144, 32, 48 }, // I
            {  32, 144, 40, 48 }, // J
            {  72, 144, 40, 48 }, // K
            { 112, 144, 40, 48 }, // L
            { 152, 144, 48, 48 }, // M
            { 200, 144, 48, 48 }, // N
            { 248, 144, 40, 48 }, // O
            { 288, 144, 40, 48 }, // P
            { 328, 144, 40, 48 }, // Q
            { 368, 144, 40, 48 }, // R
            { 408, 144, 40, 48 }, // S
            { 448, 144, 32, 48 }, // T
            {   0, 192, 40, 48 }, // U
            {  40, 192, 48, 48 }, // V
            {  88, 192, 48, 48 }, // W
            { 136, 192, 48, 48 }, // X
            { 184, 192, 48, 48 }, // Y
            { 232, 192, 48, 48 }, // Z
            { 280, 192, 24, 48 }, // [
            { 304, 192, 48, 48 }, // backslash
            { 352, 192, 24, 48 }, // ]
            { 376, 192, 32, 48 }, // ^
            { 408, 192, 40, 48 }, // _
            { 448, 192, 24, 48 }, // `
            {   0, 240, 40, 48 }, // a
            {  40, 240, 40, 48 }, // b
            {  80, 240, 40, 48 }, // c
            { 120, 240, 40, 48 }, // d
            { 160, 240, 40, 48 }, // e
            { 200, 240, 40, 48 }, // f
            { 240, 240, 40, 48 }, // g
            { 280, 240, 40, 48 }, // h
            { 320, 240, 32, 48 }, // i
            { 352, 240, 40, 48 }, // j
            { 392, 240, 40, 48 }, // k
            { 432, 240, 40, 48 }, // l
            {   0, 288, 48, 48 }, // m
            {  48, 288, 48, 48 }, // n
            {  96, 288, 40, 48 }, // o
            { 136, 288, 40, 48 }, // p
            { 176, 288, 40, 48 }, // q
            { 216, 288, 40, 48 }, // r
            { 256, 288, 40, 48 }, // s
            { 296, 288, 32, 48 }, // t
            { 328, 288, 40, 48 }, // u
            { 360, 288, 48, 48 }, // v
            { 408, 288, 48, 48 }, // w
            {   0, 336, 48, 48 }, // x
            {  48, 336, 48, 48 }, // y
            {  96, 336, 48, 48 }, // z
            { 144, 336, 32, 48 }, // {
            { 176, 336, 16, 48 }, // |
            { 192, 336, 32, 48 }, // }
            { 224, 336, 48, 48 }, // ~
        }
    },
    // font 1 for menu texts
    {
        512, 768, 80, 8,
        {
            {   0,   0, 30, 80 }, // space
            {  48,   0, 16, 80 }, // !
            {  64,   0, 32, 80 }, // "
            {  96,   0, 48, 80 }, // #
            { 144,   0, 48, 80 }, // $
            { 192,   0, 64, 80 }, // %
            { 256,   0, 40, 80 }, // &
            { 296,   0, 24, 80 }, // '
            { 320,   0, 32, 80 }, // (
            { 352,   0, 32, 80 }, // )
            { 384,   0, 56, 80 }, // *
            { 440,   0, 48, 80 }, // +
            { 488,   0, 24, 80 }, // '
            {   0,  80, 32, 80 }, // -
            {  32,  80, 24, 80 }, // .
            {  56,  80, 56, 80 }, // /
            { 112,  80, 48, 80 }, // 0
            { 160,  80, 32, 80 }, // 1
            { 192,  80, 48, 80 }, // 2
            { 240,  80, 48, 80 }, // 3
            { 288,  80, 48, 80 }, // 4
            { 336,  80, 48, 80 }, // 5
            { 384,  80, 48, 80 }, // 6
            { 432,  80, 48, 80 }, // 7
            {   0, 160, 48, 80 }, // 8
            {  48, 160, 48, 80 }, // 9
            {  96, 160, 24, 80 }, // :
            { 120, 160, 24, 80 }, // ;
            { 144, 160, 40, 80 }, // <
            { 184, 160, 40, 80 }, // =
            { 224, 160, 40, 80 }, // >
            { 264, 160, 40, 80 }, // ?
            { 304, 160, 64, 80 }, // @
            { 368, 160, 48, 80 }, // A
            { 416, 160, 48, 80 }, // B
            { 464, 160, 48, 80 }, // C
            {   0, 240, 48, 80 }, // D
            {  48, 240, 48, 80 }, // E
            {  96, 240, 48, 80 }, // F
            { 144, 240, 48, 80 }, // G
            { 192, 240, 48, 80 }, // H
            { 240, 240, 32, 80 }, // I
            { 272, 240, 48, 80 }, // J
            { 320, 240, 48, 80 }, // K
            { 368, 240, 48, 80 }, // L
            { 416, 240, 64, 80 }, // M
            {   0, 320, 56, 80 }, // N
            {  56, 320, 48, 80 }, // O
            { 104, 320, 48, 80 }, // P
            { 152, 320, 48, 80 }, // Q
            { 200, 320, 48, 80 }, // R
            { 248, 320, 48, 80 }, // S
            { 296, 320, 48, 80 }, // T
            { 344, 320, 48, 80 }, // U
            { 392, 320, 48, 80 }, // V
            { 440, 320, 64, 80 }, // W
            {   0, 400, 48, 80 }, // X
            {  48, 400, 48, 80 }, // Y
            {  96, 400, 48, 80 }, // Z
            { 144, 400, 32, 80 }, // [
            { 176, 400, 56, 80 }, // backslash
            { 232, 400, 32, 80 }, // ]
            { 264, 400, 48, 80 }, // ^
            { 312, 400, 48, 80 }, // _
            { 360, 400, 32, 80 }, // `
            { 392, 400, 40, 80 }, // a
            { 432, 400, 40, 80 }, // b
            { 472, 400, 40, 80 }, // c
            {   0, 480, 40, 80 }, // d
            {  40, 480, 40, 80 }, // e
            {  80, 480, 32, 80 }, // f
            { 112, 480, 40, 80 }, // g
            { 152, 480, 40, 80 }, // h
            { 192, 480, 16, 80 }, // i
            { 208, 480, 24, 80 }, // j
            { 232, 480, 40, 80 }, // k
            { 272, 480, 16, 80 }, // l
            { 288, 480, 64, 80 }, // m
            { 352, 480, 40, 80 }, // n
            { 392, 480, 40, 80 }, // o
            { 432, 480, 40, 80 }, // p
            {   0, 560, 48, 80 }, // q
            {  48, 560, 40, 80 }, // r
            {  88, 560, 40, 80 }, // s
            { 128, 560, 32, 80 }, // t
            { 160, 560, 40, 80 }, // u
            { 200, 560, 48, 80 }, // v
            { 248, 560, 64, 80 }, // w
            { 312, 560, 48, 80 }, // x
            { 360, 560, 40, 80 }, // y
            { 400, 560, 48, 80 }, // z
            {   0, 640, 32, 80 }, // {
            {  32, 560, 16, 80 }, // |
            {  48, 560, 32, 80 }, // }
            {  80, 640, 56, 80 }, // ~
        }
    }
};

#define getCharIndex(c) ((s32)((c) > 0 ? (c) - 32 : 0))

WarSprite wfont_loadFontSprite(WarContext* context, StringView fontPath)
{
    WarSprite sprite = {0};

    s32 width, height, bitsPerPixel;
    u8* data = stbi_load(wsv_data(fontPath), &width, &height, &bitsPerPixel, 0);
    if (data)
    {
        sprite = wspr_createSprite(context, width, height, data);
        stbi_image_free(data);
    }

    return sprite;
}

WarFontData getFontData(s32 fontIndex)
{
    if (fontIndex < 0 || fontIndex >= arrayLength(fontsData))
    {
        logError("Invalid font index: %d", fontIndex);
        return (WarFontData){0};
    }

    return fontsData[fontIndex];
}

vec2 wfont_getAlignmentOffset(WarTextAlignment horizontalAlign, WarTextAlignment verticalAlign, vec2 boundings, vec2 textSize)
{
    vec2 offset = VEC2_ZERO;

    switch (horizontalAlign)
    {
        case WAR_TEXT_ALIGN_LEFT:
        {
            // nothing here
            break;
        }
        case WAR_TEXT_ALIGN_CENTER:
        {
            offset.x = ceilf(halff(boundings.x - textSize.x));
            break;
        }
        case WAR_TEXT_ALIGN_RIGHT:
        {
            offset.x = ceilf(boundings.x - textSize.x);
            break;
        }
        default:
        {
            logError("Invalid horizontal alignment value: %d", horizontalAlign);
            break;
        }
    }

    switch (verticalAlign)
    {
        case WAR_TEXT_ALIGN_TOP:
        {
            // nothing here
            break;
        }
        case WAR_TEXT_ALIGN_MIDDLE:
        {
            offset.y = ceilf(halff(boundings.y) - halff(textSize.y));
            break;
        }
        case WAR_TEXT_ALIGN_BOTTOM:
        {
            offset.y = ceilf(boundings.y - textSize.y);
            break;
        }
        default:
        {
            logError("Invalid vertical alignment value: %d", verticalAlign);
            break;
        }
    }

    return offset;
}

f32 wfont_getLineAlignmentOffset(WarTextAlignment lineAlign, f32 width, f32 lineWidth)
{
    f32 offset = 0;

    switch (lineAlign)
    {
        case WAR_TEXT_ALIGN_LEFT:
        {
            // nothing here
            break;
        }
        case WAR_TEXT_ALIGN_CENTER:
        {
            offset = ceilf(halff(width - lineWidth));
            break;
        }
        case WAR_TEXT_ALIGN_RIGHT:
        {
            offset = ceilf(width - lineWidth);
            break;
        }
        default:
        {
            logError("Invalid horizontal alignment value: %d", lineAlign);
            break;
        }
    }

    return offset;
}

typedef struct
{
    s32 start;
    s32 length;
    f32 width;
} WarTextSpan;

static s32 wfont_splitTextIntoLines(StringView text, s32 maxLines, WarTextSpan lines[], f32 width, WarFontParams params)
{
    f32 scale = params.fontSize / params.fontData.lineHeight;
    bool wrap = params.wrapping == WAR_TEXT_WRAP_CHAR;
    f32 x = 0;
    s32 k = 0;

    s32 s = 0;
    s32 n = (s32)wsv_length(text);

    for (s32 i = 0; i < n && k < maxLines; i++)
    {
        char c = text.data[i];

        // a new line is generated for each \n
        if (c == '\n')
        {
            lines[k].start  = s;
            lines[k].length = i - s;
            lines[k].width  = x;
            k++;

            s = i + 1;
            x = 0;
        }
        else if (c == '\t')
        {
            rect rs = params.fontData.data[getCharIndex(' ')];
            f32 dx = (TAB_WIDTH * rs.width + params.fontData.advance) * scale;

            // if the current character doesn't fit in the line generate a new line
            if (x + dx > width && wrap)
            {
                lines[k].start  = s;
                lines[k].length = i - s;
                lines[k].width  = x;
                k++;

                s = i;
                x = 0;
            }

            x += dx;
        }
        else
        {
            rect rs = params.fontData.data[getCharIndex(c)];
            f32 dx = (rs.width + params.fontData.advance) * scale;

            // if the current character doesn't fit in the line generate a new line
            if (x + dx > width && wrap)
            {
                lines[k].start  = s;
                lines[k].length = i - s;
                lines[k].width  = x;
                k++;

                s = i;
                x = 0;
            }

            x += dx;
        }
    }

    if (s < n && k < maxLines)
    {
        lines[k].start  = s;
        lines[k].length = n - s;
        lines[k].width  = x;
        k++;
    }

    // trim start and end spaces on lines
    if (params.trimming != WAR_TEXT_TRIM_NONE)
    {
        rect whiteSpaceData = params.fontData.data[getCharIndex(' ')];
        f32 whiteSpaceWidth = (whiteSpaceData.width + params.fontData.advance) * scale;

        for (s32 j = 0; j < k; j++)
        {
            while (lines[j].length > 0)
            {
                char first = text.data[lines[j].start];
                if (first == ' ' && params.trimming >= WAR_TEXT_TRIM_SPACES)
                {
                    lines[j].width -= whiteSpaceWidth;
                    lines[j].start++;
                    lines[j].length--;
                }
                else if (first == '\t' && params.trimming >= WAR_TEXT_TRIM_ALL)
                {
                    lines[j].width -= TAB_WIDTH * whiteSpaceWidth;
                    lines[j].start++;
                    lines[j].length--;
                }
                else
                    break;
            }

            while (lines[j].length > 0)
            {
                char last = text.data[lines[j].start + lines[j].length - 1];
                if (last == ' ' && params.trimming >= WAR_TEXT_TRIM_SPACES)
                {
                    lines[j].width -= whiteSpaceWidth;
                    lines[j].length--;
                }
                else if (last == '\t' && params.trimming >= WAR_TEXT_TRIM_ALL)
                {
                    lines[j].width -= TAB_WIDTH * whiteSpaceWidth;
                    lines[j].length--;
                }
                else
                    break;
            }
        }
    }

    return k;
}

vec2 wfont_measureSingleSpriteText(StringView text, s32 length, WarFontParams params)
{
    f32 scale = params.fontSize / params.fontData.lineHeight;

    vec2 size = VEC2_ZERO;

    StringView sv = wsv_slice(text, 0, (size_t)length);

    for (size_t i = 0; i < sv.length; i++)
    {
        char c = sv.data[i];
        if (c == '\n')
        {
            size.x += params.fontData.advance * scale;
        }
        else if (c == '\t')
        {
            rect rs = params.fontData.data[getCharIndex(' ')];

            size.x += (TAB_WIDTH * rs.width + params.fontData.advance) * scale;
        }
        else
        {
            rect rs = params.fontData.data[getCharIndex(c)];

            size.x += (rs.width + params.fontData.advance) * scale;
            size.y = max(size.y, rs.height * scale);
        }
    }

    return size;
}

vec2 wfont_measureMultiSpriteText(StringView text, f32 width, WarFontParams params)
{
    NOT_USED(width);

    f32 scale = params.fontSize / params.fontData.lineHeight;
    f32 lineHeight = params.lineHeight > 0 ? params.lineHeight : params.fontData.lineHeight;

    WarTextSpan lines[MAX_LINES];
    s32 linesCount = wfont_splitTextIntoLines(text, MAX_LINES, lines, params.boundings.x, params);

    vec2 size = VEC2_ZERO;

    for (s32 i = 0; i < linesCount; i++)
    {
        size.x = max(size.x, lines[i].width);
        size.y += lineHeight * scale;
    }

    return size;
}

// Render a span of characters from the font sprite.
// Uses SDL_SetTextureColorMod for tinting instead of nvgFillColor.
f32 wfont_renderSingleSpriteTextSpan(WarContext* context, StringView text,
                               s32 index, s32 count,
                               f32 x, f32 y,
                               WarColor fontColor,
                               WarSprite fontSprite,
                               WarFontData fontData,
                               vec2 boundings,
                               f32 scale)
{
    if (count > 0)
    {
        renderSave(context);

        // Apply font color tint to the texture
        SDL_SetTextureColorMod(fontSprite.texture, fontColor.r, fontColor.g, fontColor.b);
        SDL_SetTextureAlphaMod(fontSprite.texture, fontColor.a);

        for (s32 i = 0; i < count; i++)
        {
            if (boundings.x > 0 && x * scale > boundings.x)
                break;

            const char c = text.data[index + i];

            if (c == '\n')
            {
                // when we are rendering a single text span, all newline characters
                // are rendered as ' ' since a single text span is considered
                // a one line of text, it won't go to the next line
                rect rs = fontData.data[getCharIndex(' ')];
                x += rs.width + fontData.advance;
            }
            else if (c == '\t')
            {
                rect rs = fontData.data[getCharIndex(' ')];
                x += TAB_WIDTH * rs.width + fontData.advance;
            }
            else
            {
                rect rs = fontData.data[getCharIndex(c)];
                rect rd = rectf(x, y, rs.width, rs.height);

#ifdef DEBUG_RENDER_FONT
                renderFillRect(context, rd, WAR_COLOR_GREEN_SELECTION);
#endif

                if (c != ' ')
                {
                    renderSubImage(context, fontSprite.texture, rs, rd, VEC2_ONE);
                }

                x += rs.width + fontData.advance;
            }
        }

        // Reset texture color mod to white
        SDL_SetTextureColorMod(fontSprite.texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(fontSprite.texture, 255);

        renderRestore(context);
    }

    return x;
}

void wfont_renderSingleSpriteText(WarContext* context, StringView text, f32 x, f32 y, WarFontParams params)
{
    f32 scale = params.fontSize / params.fontData.lineHeight;
    vec2 textSize = wfont_measureSingleSpriteText(text, -1, params);
    s32 length = (s32)wsv_length(text);

    renderSave(context);
    renderTranslate(context, x, y);

    if (!vec2IsZero(params.boundings))
    {
        vec2 textOffset = wfont_getAlignmentOffset(params.horizontalAlign, params.verticalAlign, params.boundings, textSize);
        renderTranslate(context, textOffset.x, textOffset.y);
    }

    renderScale(context, scale, scale);

#ifdef DEBUG_RENDER_FONT
    rect outline = rectf(0, 0, textSize.x / scale, 1.5f);
    renderStrokeRect(context, outline, WAR_COLOR_GREEN_SELECTION, 3);
#endif

    if (params.highlightIndex >= 0)
    {
        x = wfont_renderSingleSpriteTextSpan(context, text,
                                    0, params.highlightIndex,
                                    0, 0,
                                    params.fontColor,
                                    params.fontSprite,
                                    params.fontData,
                                    params.boundings,
                                    scale);

        x = wfont_renderSingleSpriteTextSpan(context, text,
                                    params.highlightIndex, params.highlightCount,
                                    x, 0,
                                    params.highlightColor,
                                    params.fontSprite,
                                    params.fontData,
                                    params.boundings,
                                    scale);

        x = wfont_renderSingleSpriteTextSpan(context, text,
                                    params.highlightIndex + params.highlightCount,
                                    length - params.highlightIndex - params.highlightCount,
                                    x, 0,
                                    params.fontColor,
                                    params.fontSprite,
                                    params.fontData,
                                    params.boundings,
                                    scale);
    }
    else
    {
        // No highlight, highlightIndex = -1
        // All highlight, highlightIndex = -2
        WarColor fontColor = params.highlightIndex == ALL_HIGHLIGHT
            ? params.highlightColor : params.fontColor;

        wfont_renderSingleSpriteTextSpan(context, text,
                                0, length,
                                0, 0,
                                fontColor,
                                params.fontSprite,
                                params.fontData,
                                params.boundings,
                                scale);
    }

    renderRestore(context);
}

void wfont_renderMultiSpriteText(WarContext* context, StringView text, f32 x, f32 y, WarFontParams params)
{
    f32 scale = params.fontSize / params.fontData.lineHeight;
    f32 lineHeight = params.lineHeight > 0 ? params.lineHeight : params.fontData.lineHeight;

    WarTextSpan lines[MAX_LINES];
    s32 linesCount = wfont_splitTextIntoLines(text, MAX_LINES, lines, params.boundings.x, params);

    renderSave(context);
    renderTranslate(context, x, y);

    vec2 textSize = wfont_measureMultiSpriteText(text, params.boundings.x, params);

    vec2 textOffset = wfont_getAlignmentOffset(params.horizontalAlign, params.verticalAlign, params.boundings, textSize);
    renderTranslate(context, textOffset.x, textOffset.y);

#ifdef DEBUG_RENDER_FONT
    rect outline = rectf(0, 0, textSize.x, textSize.y);
    renderStrokeRect(context, outline, WAR_COLOR_RED_SELECTION, 1);
#endif

    s32 lineStartIndex = 0;

    for (s32 i = 0; i < linesCount; i++)
    {
        s32 lineLength = lines[i].length;
        StringView lineView = wsv_slice(text, (size_t)lines[i].start, (size_t)lineLength);
        f32 lineAlignOffset = wfont_getLineAlignmentOffset(params.lineAlign, textSize.x, lines[i].width);
        vec2 lineOffset = vec2f(lineAlignOffset, i * lineHeight);

        if (lineOffset.y * scale > params.boundings.y)
            break;

        renderSave(context);
        renderTranslate(context, lineOffset.x, 0);
        renderScale(context, scale, scale);
        renderTranslate(context, 0, lineOffset.y);

#ifdef DEBUG_RENDER_FONT
        rect outline = rectf(0, 0, lines[i].width / scale, lineHeight);
        renderStrokeRect(context, outline, WAR_COLOR_GREEN_SELECTION, 1);
#endif

        if (params.highlightIndex >= lineStartIndex && params.highlightIndex < lineStartIndex + lineLength)
        {
            s32 highlightIndex = params.highlightIndex - lineStartIndex;
            s32 highlightCount = min(params.highlightCount, lineLength - highlightIndex);

            x = wfont_renderSingleSpriteTextSpan(context, lineView,
                                        0, highlightIndex,
                                        0, 0,
                                        params.fontColor,
                                        params.fontSprite,
                                        params.fontData,
                                        params.boundings,
                                        scale);

            x = wfont_renderSingleSpriteTextSpan(context, lineView,
                                        highlightIndex, highlightCount,
                                        x, 0,
                                        params.highlightColor,
                                        params.fontSprite,
                                        params.fontData,
                                        params.boundings,
                                        scale);

            x = wfont_renderSingleSpriteTextSpan(context, lineView,
                                        highlightIndex + highlightCount,
                                        lineLength - highlightIndex - highlightCount,
                                        x, 0,
                                        params.fontColor,
                                        params.fontSprite,
                                        params.fontData,
                                        params.boundings,
                                        scale);
        }
        else if (params.highlightIndex < lineStartIndex &&
                 params.highlightIndex + params.highlightCount >= lineStartIndex)
        {
            s32 highlightCount = min(params.highlightIndex + params.highlightCount - lineStartIndex, lineLength);

            x = wfont_renderSingleSpriteTextSpan(context, lineView,
                                        0, highlightCount,
                                        0, 0,
                                        params.highlightColor,
                                        params.fontSprite,
                                        params.fontData,
                                        params.boundings,
                                        scale);

            x = wfont_renderSingleSpriteTextSpan(context, lineView,
                                        highlightCount,
                                        lineLength - highlightCount,
                                        x, 0,
                                        params.fontColor,
                                        params.fontSprite,
                                        params.fontData,
                                        params.boundings,
                                        scale);
        }
        else
        {
            // No highlight, highlightIndex = -1
            // All highlight, highlightIndex = -2
            WarColor fontColor = params.highlightIndex == ALL_HIGHLIGHT
                ? params.highlightColor : params.fontColor;

            wfont_renderSingleSpriteTextSpan(context, lineView,
                                    0, lineLength,
                                    0, 0,
                                    fontColor,
                                    params.fontSprite,
                                    params.fontData,
                                    params.boundings,
                                    scale);
        }

        renderRestore(context);

        lineStartIndex += lineLength;
    }

    renderRestore(context);
}
