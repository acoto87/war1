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
            { 256, 288, 40, 48 }, // sFONT_HORIZONTAL_GAP_PX
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

#define FONT_NORMAL_COLOR u8RgbaColor(239, 255, 219, 255)
#define FONT_HIGHLIGHT_COLOR u8RgbaColor(255, 227, 73, 255)
#define NO_HIGHLIGHT -1
#define ALL_HIGHLIGHT -2
#define MAX_LINES 48
#define TAB_WIDTH 2

#define u8ColorToNVGcolor(color) nvgRGBA((color).r, (color).g, (color).b, (color).a)

enum NVGalign textAlignToNVGalign(WarTextAlignment align)
{
    switch (align)
    {
        case WAR_TEXT_ALIGN_LEFT:   return NVG_ALIGN_LEFT;
        case WAR_TEXT_ALIGN_CENTER: return NVG_ALIGN_CENTER;
        case WAR_TEXT_ALIGN_RIGHT:  return NVG_ALIGN_RIGHT;
        case WAR_TEXT_ALIGN_TOP:    return NVG_ALIGN_TOP;
        case WAR_TEXT_ALIGN_MIDDLE: return NVG_ALIGN_MIDDLE;
        case WAR_TEXT_ALIGN_BOTTOM: return NVG_ALIGN_BOTTOM;
        default:
        {
            logError("Invalid alignment value: %d. Defaulting to %d\n", align, NVG_ALIGN_LEFT);
            return NVG_ALIGN_LEFT;
        }
    }
}

enum NVGwrap textWrappingToNVGwrap(WarTextWrapping wrap)
{
    switch (wrap)
    {
        case WAR_TEXT_WRAP_NONE: return NVG_WRAP_NONE;
        case WAR_TEXT_WRAP_CHAR: return NVG_WRAP_WORD;
        default:
        {
            logError("Invalid wrapping value: %d. Defaulting to %d\n", wrap, NVG_WRAP_NONE);
            return NVG_WRAP_NONE;
        }
    }
}

enum NVGtrim textTrimmingToNVGtrim(WarTextTrimming trim)
{
    switch (trim)
    {
        case WAR_TEXT_TRIM_NONE: return NVG_TRIM_NONE;
        case WAR_TEXT_TRIM_SPACES: return NVG_TRIM_SPACES;
        case WAR_TEXT_TRIM_ALL: return NVG_TRIM_ALL;
        default:
        {
            logError("Invalid trimming value: %d. Defaulting to %d\n", trim, NVG_TRIM_NONE);
            return NVG_TRIM_NONE;
        }
    }
}

#define getCharIndex(c) ((s32)((c) > 0 ? (c) - 32 : 0))

WarSprite loadFontSprite(WarContext* context, const char* fontPath)
{
    WarSprite sprite = {0};

    s32 width, height, bitsPerPixel;
    u8* data = stbi_load(fontPath, &width, &height, &bitsPerPixel, 0);
    if (data)
    {
        sprite = createSprite(context, width, height, data);
        stbi_image_free(data);
    }

    return sprite;
}

vec2 getAlignmentOffset(enum NVGalign horizontalAlign, enum NVGalign verticalAlign, vec2 boundings, vec2 textSize)
{
    vec2 offset = VEC2_ZERO;

    switch (horizontalAlign)
    {
        case NVG_ALIGN_LEFT:
        {
            // nothing here
            break;
        }
        case NVG_ALIGN_CENTER:
        {
            offset.x = ceilf(halff(boundings.x - textSize.x));
            break;
        }
        case NVG_ALIGN_RIGHT:
        {
            offset.x = ceilf(boundings.x - textSize.x);
            break;
        }
        default:
        {
            logError("Invalid horizontal alignment value: %d\n", horizontalAlign);
            break;
        }
    }

    switch (verticalAlign)
    {
        case NVG_ALIGN_TOP:
        {
            // nothing here
            break;
        }
        case NVG_ALIGN_MIDDLE:
        {
            offset.y = ceilf(halff(boundings.y) - halff(textSize.y));
            break;
        }
        case NVG_ALIGN_BOTTOM:
        {
            offset.y = ceilf(boundings.y - textSize.y);
            break;
        }
        default:
        {
            logError("Invalid vertical alignment value: %d\n", verticalAlign);
            break;
        }
    }

    return offset;
}

f32 getLineAlignmentOffset(enum NVGalign lineAlign, f32 width, f32 lineWidth)
{
    f32 offset = 0;

    switch (lineAlign)
    {
        case NVG_ALIGN_LEFT:
        {
            // nothing here
            break;
        }
        case NVG_ALIGN_CENTER:
        {
            offset = ceilf(halff(width - lineWidth));
            break;
        }
        case NVG_ALIGN_RIGHT:
        {
            offset = ceilf(width - lineWidth);
            break;
        }
        default:
        {
            logError("Invalid horizontal alignment value: %d\n", lineAlign);
            break;
        }
    }

    return offset;
}

typedef struct
{
    const char* start;
    const char* end;
    f32 width;
} NVGTextSpan;

s32 nvgSplitTextIntoLines(const char* text, s32 maxLines, NVGTextSpan lines[], f32 width, NVGfontParams params)
{
    f32 scale = params.fontSize / params.fontData.lineHeight;
    bool wrap = params.wrapping == NVG_WRAP_WORD;
    f32 x = 0;
    s32 k = 0;

    const char* s = text;
    const char* e = text;
    while (*e && k < maxLines)
    {
        // a new line is generated for each \n
        if (*e == '\n')
        {
            lines[k].start = s;
            lines[k].end = e;
            lines[k].width = x;
            k++;

            s = e + 1;
            x = 0;
        }
        else if (*e == '\t')
        {
            rect rs = params.fontData.data[getCharIndex(' ')];
            f32 dx = (TAB_WIDTH * rs.width + params.fontData.advance) * scale;

            // if the current character doesn't fit in the line generate a new line
            if (x + dx > width && wrap)
            {
                lines[k].start = s;
                lines[k].end = e;
                lines[k].width = x;
                k++;

                s = e;
                x = 0;
            }

            x += dx;
        }
        else
        {
            rect rs = params.fontData.data[getCharIndex(*e)];
            f32 dx = (rs.width + params.fontData.advance) * scale;

            // if the current character doesn't fit in the line generate a new line
            if (x + dx > width && wrap)
            {
                lines[k].start = s;
                lines[k].end = e;
                lines[k].width = x;
                k++;

                s = e;
                x = 0;
            }

            x += dx;
        }

        e++;
    }

    if (s < e && k < maxLines)
    {
        lines[k].start = s;
        lines[k].end = e;
        lines[k].width = x;
        k++;
    }

    // trim start and end spaces on lines
    if (params.trimming != NVG_TRIM_NONE)
    {
        rect whiteSpaceData = params.fontData.data[getCharIndex(' ')];
        f32 whiteSpaceWidth = (whiteSpaceData.width + params.fontData.advance) * scale;

        for (s32 i = 0; i < k; i++)
        {
            while (true)
            {
                if (lines[i].start[0] == ' ' && params.trimming >= NVG_TRIM_SPACES)
                {
                    lines[i].width -= whiteSpaceWidth;
                    lines[i].start++;
                }
                else if (lines[i].start[i] == '\t' && params.trimming >= NVG_TRIM_ALL)
                {
                    lines[i].width -= TAB_WIDTH * whiteSpaceWidth;
                    lines[i].start++;
                }
                else
                    break;
            }

            while (true)
            {
                if (lines[i].end[-1] == ' ' && params.trimming >= NVG_TRIM_SPACES)
                {
                    lines[i].width -= whiteSpaceWidth;
                    lines[i].end--;
                }
                else if (lines[i].end[-1] == '\t' && params.trimming >= NVG_TRIM_ALL)
                {
                    lines[i].width -= TAB_WIDTH * whiteSpaceWidth;
                    lines[i].end--;
                }
                else
                    break;
            }
        }
    }

    return k;
}

vec2 nvgMeasureSingleSpriteText(const char* text, s32 length, NVGfontParams params)
{
    f32 scale = params.fontSize / params.fontData.lineHeight;

    vec2 size = VEC2_ZERO;

    const char* e = text;
    while (length != 0 && *e)
    {
        if (*e == '\n')
        {
            size.x += params.fontData.advance * scale;
        }
        else if (*e == '\t')
        {
            rect rs = params.fontData.data[getCharIndex(' ')];

            size.x += (TAB_WIDTH * rs.width + params.fontData.advance) * scale;
        }
        else
        {
            rect rs = params.fontData.data[getCharIndex(*e)];

            size.x += (rs.width + params.fontData.advance) * scale;
            size.y = max(size.y, rs.height * scale);
        }

        length--;
        e++;
    }

    return size;
}

vec2 nvgMeasureMultiSpriteText(const char* text, f32 width, NVGfontParams params)
{
    f32 scale = params.fontSize / params.fontData.lineHeight;
    f32 lineHeight = params.lineHeight > 0 ? params.lineHeight : params.fontData.lineHeight;

    NVGTextSpan lines[MAX_LINES];
    s32 linesCount = nvgSplitTextIntoLines(text, MAX_LINES, lines, params.boundings.x, params);

    vec2 size = VEC2_ZERO;

    for (s32 i = 0; i < linesCount; i++)
    {
        size.x = max(size.x, lines[i].width);
        size.y += lineHeight * scale;
    }

    return size;
}

f32 nvgSingleSpriteTextSpan(NVGcontext* gfx, const char* text,
                            s32 index, s32 count,
                            f32 x, f32 y,
                            NVGcolor fontColor,
                            WarSprite fontSprite,
                            WarFontData fontData,
                            vec2 boundings,
                            f32 scale)
{
    if (count > 0)
    {
        nvgSave(gfx);
        nvgFillColor(gfx, fontColor);

        NVGimageBatch* batch = nvgBeginImageBatch(gfx, fontSprite.image, count);

        for (s32 i = 0; i < count; i++)
        {
            if (boundings.x > 0 && x * scale > boundings.x)
                break;

            const char c = text[index + i];

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
                nvgFillRect(gfx, rd, NVG_GREEN_SELECTION);
#endif

                if (c != ' ')
                {
                    nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);
                }

                x += rs.width + fontData.advance;
            }
        }

        nvgEndImageBatch(gfx, batch);
        nvgRestore(gfx);
    }

    return x;
}

void nvgSingleSpriteText(NVGcontext* gfx, const char* text, f32 x, f32 y, NVGfontParams params)
{
    f32 scale = params.fontSize / params.fontData.lineHeight;
    vec2 textSize = nvgMeasureSingleSpriteText(text, -1, params);
    s32 length = strlen(text);

    nvgSave(gfx);
    nvgTranslate(gfx, x, y);

    if (!vec2IsZero(params.boundings))
    {
        vec2 textOffset = getAlignmentOffset(params.horizontalAlign, params.verticalAlign, params.boundings, textSize);
        nvgTranslate(gfx, textOffset.x, textOffset.y);
    }

    nvgScale(gfx, scale, scale);

#ifdef DEBUG_RENDER_FONT
    rect outline = rectf(0, 0, textSize.x / scale, 1.5f);
    nvgStrokeRect(gfx, outline, NVG_GREEN_SELECTION, 3);
#endif

    if (params.highlightIndex >= 0)
    {
        x = nvgSingleSpriteTextSpan(gfx, text,
                                    0, params.highlightIndex,
                                    0, 0,
                                    params.fontColor,
                                    params.fontSprite,
                                    params.fontData,
                                    params.boundings,
                                    scale);

        x = nvgSingleSpriteTextSpan(gfx, text,
                                    params.highlightIndex, params.highlightCount,
                                    x, 0,
                                    params.highlightColor,
                                    params.fontSprite,
                                    params.fontData,
                                    params.boundings,
                                    scale);

        x = nvgSingleSpriteTextSpan(gfx, text,
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
        NVGcolor fontColor = params.highlightIndex == ALL_HIGHLIGHT
            ? params.highlightColor : params.fontColor;

        nvgSingleSpriteTextSpan(gfx, text,
                                0, length,
                                0, 0,
                                fontColor,
                                params.fontSprite,
                                params.fontData,
                                params.boundings,
                                scale);
    }

    nvgRestore(gfx);
}

void nvgMultiSpriteText(NVGcontext* gfx, const char* text, f32 x, f32 y, NVGfontParams params)
{
    f32 scale = params.fontSize / params.fontData.lineHeight;
    f32 lineHeight = params.lineHeight > 0 ? params.lineHeight : params.fontData.lineHeight;

    NVGTextSpan lines[MAX_LINES];
    s32 linesCount = nvgSplitTextIntoLines(text, MAX_LINES, lines, params.boundings.x, params);

    nvgSave(gfx);
    nvgTranslate(gfx, x, y);

    vec2 textSize = nvgMeasureMultiSpriteText(text, params.boundings.x, params);

    vec2 textOffset = getAlignmentOffset(params.horizontalAlign, params.verticalAlign, params.boundings, textSize);
    nvgTranslate(gfx, textOffset.x, textOffset.y);

#ifdef DEBUG_RENDER_FONT
    rect outline = rectf(0, 0, textSize.x, textSize.y);
    nvgStrokeRect(gfx, outline, NVG_RED_SELECTION, 1);
#endif

    s32 lineStartIndex = 0;

    for (s32 i = 0; i < linesCount; i++)
    {
        s32 lineLength = (s32)(lines[i].end - lines[i].start);
        f32 lineAlignOffset = getLineAlignmentOffset(params.lineAlign, textSize.x, lines[i].width);
        vec2 lineOffset = vec2f(lineAlignOffset, i * lineHeight);

        if (lineOffset.y * scale > params.boundings.y)
            break;

        nvgSave(gfx);
        nvgTranslate(gfx, lineOffset.x, 0);
        nvgScale(gfx, scale, scale);
        nvgTranslate(gfx, 0, lineOffset.y);

#ifdef DEBUG_RENDER_FONT
        rect outline = rectf(0, 0, lines[i].width / scale, lineHeight);
        nvgStrokeRect(gfx, outline, NVG_GREEN_SELECTION, 1);
#endif

        if (params.highlightIndex >= lineStartIndex && params.highlightIndex < lineStartIndex + lineLength)
        {
            s32 highlightIndex = params.highlightIndex - lineStartIndex;
            s32 highlightCount = min(params.highlightCount, lineLength - highlightIndex);

            x = nvgSingleSpriteTextSpan(gfx, lines[i].start,
                                        0, highlightIndex,
                                        0, 0,
                                        params.fontColor,
                                        params.fontSprite,
                                        params.fontData,
                                        params.boundings,
                                        scale);

            x = nvgSingleSpriteTextSpan(gfx, lines[i].start,
                                        highlightIndex, highlightCount,
                                        x, 0,
                                        params.highlightColor,
                                        params.fontSprite,
                                        params.fontData,
                                        params.boundings,
                                        scale);

            x = nvgSingleSpriteTextSpan(gfx, lines[i].start,
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

            x = nvgSingleSpriteTextSpan(gfx, lines[i].start,
                                        0, highlightCount,
                                        0, 0,
                                        params.highlightColor,
                                        params.fontSprite,
                                        params.fontData,
                                        params.boundings,
                                        scale);

            x = nvgSingleSpriteTextSpan(gfx, lines[i].start,
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
            NVGcolor fontColor = params.highlightIndex == ALL_HIGHLIGHT
                ? params.highlightColor : params.fontColor;

            nvgSingleSpriteTextSpan(gfx, lines[i].start,
                                    0, lineLength,
                                    0, 0,
                                    fontColor,
                                    params.fontSprite,
                                    params.fontData,
                                    params.boundings,
                                    scale);
        }

        nvgRestore(gfx);

        lineStartIndex += lineLength;
    }

    nvgRestore(gfx);
}
