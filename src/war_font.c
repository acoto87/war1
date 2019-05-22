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
            { 224, 336, 48, 48 } // ~
        }
    },
    // font 1 for menu texts
    {
        512, 768, 48, 8,
        {
            {   0,   0, 48, 80 }, // space 
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
            { 208, 480, 16, 80 }, // j
            { 224, 480, 40, 80 }, // k
            { 264, 480, 16, 80 }, // l
            { 280, 480, 64, 80 }, // m
            { 344, 480, 40, 80 }, // n
            { 384, 480, 40, 80 }, // o
            { 424, 480, 40, 80 }, // p
            { 464, 480, 48, 80 }, // q
            {   0, 560, 40, 80 }, // r
            {  40, 560, 40, 80 }, // s
            {  80, 560, 32, 80 }, // t
            { 112, 560, 40, 80 }, // u
            { 152, 560, 48, 80 }, // v
            { 200, 560, 64, 80 }, // w
            { 264, 560, 48, 80 }, // x
            { 312, 560, 40, 80 }, // y
            { 352, 560, 48, 80 }, // z
            { 400, 560, 32, 80 }, // {
            { 432, 560, 16, 80 }, // |
            { 448, 560, 32, 80 }, // }
            {   0, 640, 56, 80 } // ~
        }
    }
};

#define FONT_NORMAL_COLOR u8RgbaColor(239, 255, 219, 255)
#define FONT_HIGHLIGHT_COLOR u8RgbaColor(255, 227, 73, 255)
#define NO_HIGHLIGHT -1

#define u8ColorToNVGcolor(color) nvgRGBA((color).r, (color).g, (color).b, (color).a)

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

s32 nvgSingleSpriteTextSpan(NVGcontext* gfx, const char* text, s32 index, s32 count, f32 x, f32 y, NVGfontParams params)
{
    if (count > 0)
    {
        nvgSave(gfx);
        nvgFillColor(gfx, params.fontColor);

        NVGimageBatch* batch = nvgBeginImageBatch(gfx, params.fontSprite.image, count);

        for (s32 i = 0; i < count; i++)
        {
            rect rs = params.fontData.data[getCharIndex(text[index + i])];
            rect rd = rectf(x, y, rs.width, rs.height);

            if (text[index + i] != ' ')
            {
#ifdef DEBUG_RENDER_FONT
                nvgFillRect(gfx, rd, NVG_GREEN_SELECTION);
#endif
                
                nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);
            }

            x += rs.width + params.fontData.advance;
        }

        nvgEndImageBatch(gfx, batch);
        nvgRestore(gfx);
    }
    
    return x;
}

void nvgSingleSpriteText(NVGcontext* gfx, const char* text, f32 x, f32 y, NVGfontParams params)
{
    s32 len = strlen(text);
    f32 scale = params.fontSize / params.fontData.lineHeight;
    s32 highlightIndex = params.highlightIndex;

    NVGcolor fontColor = params.fontColor;
    NVGcolor highlightColor = u8ColorToNVGcolor(FONT_HIGHLIGHT_COLOR);

    nvgSave(gfx);
    nvgScale(gfx, scale, scale);

    if (highlightIndex >= 0)
    {
        params.fontColor = fontColor;
        x = nvgSingleSpriteTextSpan(gfx, text, 0, highlightIndex, x, y, params);

        params.fontColor = highlightColor;
        x = nvgSingleSpriteTextSpan(gfx, text, highlightIndex, 1, x, y, params);

        params.fontColor = fontColor;
        x = nvgSingleSpriteTextSpan(gfx, text, highlightIndex + 1, len - highlightIndex, x, y, params);
    }
    else
    {
        params.fontColor = fontColor;
        nvgSingleSpriteTextSpan(gfx, text, 0, len, x, y, params);
    }

    nvgRestore(gfx);
}