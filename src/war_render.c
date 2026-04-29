#include "war_render.h"

#include <assert.h>

// ---------------------------------------------------------------------------
// Render state stack (replaces nvgSave/nvgRestore/nvgTranslate/nvgScale/nvgGlobalAlpha)
// ---------------------------------------------------------------------------
void renderInit(WarContext* context)
{
    context->renderStateTop = 0;
    context->renderState[0].offsetX = 0;
    context->renderState[0].offsetY = 0;
    context->renderState[0].scaleX = 1;
    context->renderState[0].scaleY = 1;
    context->renderState[0].alpha = 1;
}

static WarRenderState* renderGetState(WarContext* context)
{
    return &context->renderState[context->renderStateTop];
}

void renderSave(WarContext* context)
{
    assert(context->renderStateTop + 1 < MAX_RENDER_STATE_STACK);
    context->renderState[context->renderStateTop + 1] = context->renderState[context->renderStateTop];
    context->renderStateTop++;
}

void renderRestore(WarContext* context)
{
    assert(context->renderStateTop > 0);
    context->renderStateTop--;
}

void renderTranslate(WarContext* context, f32 tx, f32 ty)
{
    WarRenderState* s = renderGetState(context);
    s->offsetX += tx * s->scaleX;
    s->offsetY += ty * s->scaleY;
}

void renderScale(WarContext* context, f32 sx, f32 sy)
{
    WarRenderState* s = renderGetState(context);
    s->scaleX *= sx;
    s->scaleY *= sy;
}

void renderGlobalAlpha(WarContext* context, f32 a)
{
    WarRenderState* s = renderGetState(context);
    s->alpha *= a;
}

// ---------------------------------------------------------------------------
// Transform helpers — map local coordinates to screen coordinates
// ---------------------------------------------------------------------------
static SDL_FRect renderTransformRect(WarContext* context, rect r)
{
    WarRenderState* s = renderGetState(context);
    f32 sx = s->scaleX;
    f32 sy = s->scaleY;

    f32 x = s->offsetX + r.x * sx;
    f32 y = s->offsetY + r.y * sy;
    f32 w = r.width * sx;
    f32 h = r.height * sy;

    // if scale is negative, flip the origin
    if (w < 0) { x += w; w = -w; }
    if (h < 0) { y += h; h = -h; }

    return (SDL_FRect){ x, y, w, h };
}

static void renderTransformPoint(WarContext* context, f32 lx, f32 ly, f32* ox, f32* oy)
{
    WarRenderState* s = renderGetState(context);
    *ox = s->offsetX + lx * s->scaleX;
    *oy = s->offsetY + ly * s->scaleY;
}

// ---------------------------------------------------------------------------
// Primitive rendering (replaces nvgFillRect, nvgStrokeRect, nvgStrokeLine, etc.)
// ---------------------------------------------------------------------------
static void renderSetDrawColor(WarContext* context, WarColor color)
{
    WarRenderState* s = renderGetState(context);
    u8 a = (u8)(color.a * s->alpha);
    SDL_SetRenderDrawColor(context->renderer, color.r, color.g, color.b, a);
}

void renderFillRect(WarContext* context, rect r, WarColor color)
{
    SDL_FRect dr = renderTransformRect(context, r);
    renderSetDrawColor(context, color);
    SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(context->renderer, &dr);
}

void renderFillRects(WarContext* context, s32 count, rect r[], WarColor color)
{
    for (s32 i = 0; i < count; i++)
        renderFillRect(context, r[i], color);
}

void renderStrokeRect(WarContext* context, rect r, WarColor color, f32 width)
{
    NOT_USED(width); // SDL_RenderRect always draws 1px lines; good enough at 320x200
    SDL_FRect dr = renderTransformRect(context, r);
    renderSetDrawColor(context, color);
    SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderRect(context->renderer, &dr);
}

void renderStrokeLine(WarContext* context, vec2 p1, vec2 p2, WarColor color, f32 width)
{
    NOT_USED(width);
    f32 x1, y1, x2, y2;
    renderTransformPoint(context, p1.x, p1.y, &x1, &y1);
    renderTransformPoint(context, p2.x, p2.y, &x2, &y2);
    renderSetDrawColor(context, color);
    SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderLine(context->renderer, x1, y1, x2, y2);
}

// ---------------------------------------------------------------------------
// Image / sub-image rendering (replaces nvgRenderSubImage and batch system)
// ---------------------------------------------------------------------------
void renderSubImage(WarContext* context, SDL_Texture* texture, rect rs, rect rd, vec2 scale)
{
    if (!texture) return;

    WarRenderState* s = renderGetState(context);

    // Combine the per-call scale with the render-state scale
    f32 combinedSX = s->scaleX * scale.x;
    f32 combinedSY = s->scaleY * scale.y;

    // Determine flip flags from sign of combined scale
    SDL_FlipMode flip = SDL_FLIP_NONE;
    if (combinedSX < 0 && combinedSY < 0)
        flip = (SDL_FlipMode)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
    else if (combinedSX < 0)
        flip = SDL_FLIP_HORIZONTAL;
    else if (combinedSY < 0)
        flip = SDL_FLIP_VERTICAL;

    f32 absSX = combinedSX < 0 ? -combinedSX : combinedSX;
    f32 absSY = combinedSY < 0 ? -combinedSY : combinedSY;

    f32 dx = s->offsetX + rd.x * s->scaleX;
    f32 dy = s->offsetY + rd.y * s->scaleY;
    f32 dw = rd.width * absSX;
    f32 dh = rd.height * absSY;

    // if state scale is negative, adjust origin
    if (s->scaleX < 0) dx -= dw;
    if (s->scaleY < 0) dy -= dh;

    SDL_FRect srcRect = { rs.x, rs.y, rs.width, rs.height };
    SDL_FRect dstRect = { dx, dy, dw, dh };

    // Apply alpha from render state
    u8 alpha = (u8)(s->alpha * 255);
    SDL_SetTextureAlphaMod(texture, alpha);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    if (flip != SDL_FLIP_NONE)
        SDL_RenderTextureRotated(context->renderer, texture, &srcRect, &dstRect, 0.0, NULL, flip);
    else
        SDL_RenderTexture(context->renderer, texture, &srcRect, &dstRect);
}

// ---------------------------------------------------------------------------
// Color helper (replaces getColorFromList which returned NVGcolor)
// ---------------------------------------------------------------------------
WarColor getColorFromList(s32 index)
{
    const u32 colors[] =
    {
        0xE52B50, // Amaranth
        0xFFBF00, // Amber
        0x9966CC, // Amethyst
        0xFBCEB1, // Apricot
        0x7FFFD4, // Aquamarine
        0x007FFF, // Azure
        0x89CFF0, // Baby blue
        0xF5F5DC, // Beige
        0x000000, // Black
        0x0000FF, // Blue
        0x0095B6, // Blue-green
        0x8A2BE2, // Blue-violet
        0xDE5D83, // Blush
        0xCD7F32, // Bronze
        0x964B00, // Brown
        0x800020, // Burgundy
        0x702963, // Byzantium
        0x960018, // Carmine
        0xDE3163, // Cerise
        0x007BA7, // Cerulean
        0xF7E7CE, // Champagne
        0x7FFF00, // Chartreuse green
        0x7B3F00, // Chocolate
        0x0047AB, // Cobalt blue
        0x6F4E37, // Coffee
        0xB87333, // Copper
        0xF88379, // Coral
        0xDC143C, // Crimson
        0x00FFFF, // Cyan
        0xEDC9AF, // Desert sand
        0x7DF9FF, // Electric blue
        0x50C878, // Emerald
        0x00FF3F, // Erin
        0xFFD700, // Gold
        0x808080, // Gray
        0x00FF00, // Green
        0x3FFF00, // Harlequin
        0x4B0082, // Indigo
        0xFFFFF0, // Ivory
        0x00A86B, // Jade
        0x29AB87, // Jungle green
        0xB57EDC, // Lavender
        0xFFF700, // Lemon
        0xC8A2C8, // Lilac
        0xBFFF00, // Lime
        0xFF00FF, // Magenta
        0xFF00AF, // Magenta rose
        0x800000, // Maroon
        0xE0B0FF, // Mauve
        0x000080, // Navy blue
        0xCC7722, // Ocher
        0x808000, // Olive
        0xFFA500, // Orange
        0xFF4500, // Orange-red
        0xDA70D6, // Orchid
        0xFFE5B4, // Peach
        0xD1E231, // Pear
        0xCCCCFF, // Periwinkle
        0x1C39BB, // Persian blue
        0xFFC0CB, // Pink
        0x8E4585, // Plum
        0x003153, // Prussian blue
        0xCC8899, // Puce
        0x800080, // Purple
        0xE30B5C, // Raspberry
        0xFF0000, // Red
        0xC71585, // Red-violet
        0xFF007F, // Rose
        0xE0115F, // Ruby
        0xFA8072, // Salmon
        0x92000A, // Sangria
        0x0F52BA, // Sapphire
        0xFF2400, // Scarlet
        0xC0C0C0, // Silver
        0x708090, // Slate gray
        0xA7FC00, // Spring bud
        0x00FF7F, // Spring green
        0xD2B48C, // Tan
        0x483C32, // Taupe
        0x008080, // Teal
        0x40E0D0, // Turquoise
        0xEE82EE, // Violet
        0x40826D, // Viridian
        0xFFFFFF, // White
        0xFFFF00, // Yellow
    };

    u32 rgb = colors[index % arrayLength(colors)];

    WarColor color;
    color.r = (rgb >> 16) & 0xFF;
    color.g = (rgb >>  8) & 0xFF;
    color.b = (rgb >>  0) & 0xFF;
    color.a = 255;
    return color;
}
