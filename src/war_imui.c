#include "war_imui.h"

#include <assert.h>
#include <string.h>

#include "war_audio.h"
#include "war_font.h"
#include "war_render.h"
#include "war_resources.h"
#include "war_sprites.h"

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Return the cached WarSprite for the given (resourceIndex, frameIndex) pair,
// creating it on first use.  Each unique pair has its own SDL_Texture
// pre-loaded with the correct frame's pixel data so multiple frames of the
// same resource can be rendered in a single pass without SDL_UpdateTexture
// conflicts.  A ref with resourceIndex < 0 (invalidRef) returns a
// zero-initialised sprite.
static WarSprite imui_getOrCreateSprite(WarContext* context, WarSpriteResourceRef ref, s32 frameIndex)
{
    if (ref.resourceIndex < 0)
    {
        return (WarSprite){0};
    }

    WarImuiState* imui = &context->imui;

    // Search for an existing cache entry.
    // Track the least recently used entry for eviction if needed.
    s32 lruIndex = 0;

    for (s32 i = 0; i < imui->spriteCacheCount; i++)
    {
        WarImuiSpriteEntry* entry = &imui->spriteCache[i];
        if (entry->resourceIndex == ref.resourceIndex &&
            entry->frameIndex    == frameIndex)
        {
            return entry->sprite;
        }

        if (entry->lastUsedFrame < imui->spriteCache[lruIndex].lastUsedFrame)
        {
            lruIndex = i;
        }
    }

    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, ref);
    if (frameIndex >= 0 && frameIndex < sprite.framesCount)
    {
        WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite, frameIndex);
        wspr_updateSpriteImage(context, sprite, frame.data);
    }

    if (imui->spriteCacheCount < IMUI_SPRITE_CACHE_SIZE)
    {
        imui->spriteCache[imui->spriteCacheCount].resourceIndex = ref.resourceIndex;
        imui->spriteCache[imui->spriteCacheCount].frameIndex    = frameIndex;
        imui->spriteCache[imui->spriteCacheCount].sprite        = sprite;
        imui->spriteCacheCount++;
        return sprite;
    }

    logDebug("IMUI sprite cache full, evicting resource %d frame %d last used at frame %u",
             imui->spriteCache[lruIndex].resourceIndex,
             imui->spriteCache[lruIndex].frameIndex,
             imui->spriteCache[lruIndex].lastUsedFrame);

    // Evict the least recently used sprite.
    WarImuiSpriteEntry* lruEntry = &imui->spriteCache[lruIndex];
    wspr_freeSprite(context, lruEntry->sprite);
    lruEntry->resourceIndex = ref.resourceIndex;
    lruEntry->frameIndex    = frameIndex;
    lruEntry->lastUsedFrame = context->frameCount;
    lruEntry->sprite = sprite;

    return sprite;
}

// Store tooltip deferral data from a hovered button.
void imui_deferTooltip(WarContext* context, StringView tooltip, s32 highlightIndex, s32 highlightCount, s32 gold, s32 wood)
{
    WarImuiState* imui = &context->imui;

    imui->show_tooltip            = true;
    imui->tooltip_highlight_index = highlightIndex;
    imui->tooltip_highlight_count = highlightCount;
    imui->tooltip_gold             = gold;
    imui->tooltip_wood             = wood;

    if (tooltip.data && tooltip.length > 0)
    {
        size_t copyLen = MIN(tooltip.length, IMUI_TOOLTIP_TEXT_MAX - 1);
        memcpy(imui->tooltip_text, tooltip.data, copyLen);
        imui->tooltip_text[copyLen] = '\0';
    }
    else
    {
        imui->tooltip_text[0] = '\0';
    }
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void imui_begin(WarContext* context)
{
    WarImuiState* imui = &context->imui;

    imui->hot_item             = 0;
    imui->is_mouse_over_ui     = false;
    imui->show_tooltip         = false;
    imui->tooltip_text[0]      = '\0';
    imui->tooltip_highlight_index = NO_HIGHLIGHT;
    imui->tooltip_highlight_count = 0;
    imui->tooltip_gold            = 0;
    imui->tooltip_wood            = 0;
    imui->cursor_type          = WAR_CURSOR_ARROW;
    imui->hotkeys_enabled      = true;
    // NOTE: active_item and spriteCache are intentionally NOT reset here.
    // active_item persists until the mouse button is released (cleared in imui_end).
    // spriteCache persists for the lifetime of the context.
}

void imui_end(WarContext* context)
{
    WarImuiState* imui  = &context->imui;
    WarInput*     input = &context->input;
    WarMap*       map   = context->map;

    // Clear the active item once the left mouse button is fully released.
    if (!isButtonHeld(input, WAR_MOUSE_LEFT))
    {
        imui->active_item = 0;
    }

    // Draw deferred tooltip text on top of all other rendered content.
    // The tooltip is rendered at the map's status-bar position (bottom panel,
    // offset vec2i(2, 5) relative to recti(72, 188, 240, 12)).
    if (imui->show_tooltip && imui->tooltip_text[0] != '\0' && context->map)
    {
        WarFontParams params;
        params.fontIndex      = 0;
        params.fontSize       = 6.0f;
        params.lineHeight     = 0.0f;
        params.fontColor      = FONT_NORMAL_COLOR;
        params.highlightColor = FONT_HIGHLIGHT_COLOR;
        params.highlightIndex = imui->tooltip_highlight_index;
        params.highlightCount = imui->tooltip_highlight_count;
        params.boundings      = vec2f(0.0f, 0.0f);
        params.horizontalAlign = WAR_TEXT_ALIGN_LEFT;
        params.verticalAlign   = WAR_TEXT_ALIGN_TOP;
        params.lineAlign       = WAR_TEXT_ALIGN_LEFT;
        params.wrapping        = WAR_TEXT_WRAP_NONE;
        params.trimming        = WAR_TEXT_TRIM_NONE;
        params.fontSprite      = context->fontSprites[0];
        params.fontData        = getFontData(0);

        StringView tooltipView = wsv_fromCString(imui->tooltip_text);

        // Render directly at the status-bar text position to match the retained UI.
        wr_save(context);
        wfont_renderSingleSpriteText(context, tooltipView, 74.0f, 193.0f, params);
        wr_restore(context);

        // If the tooltip includes gold or wood costs, render those icons + amounts to the right of the text.
        if (imui->tooltip_gold > 0 || imui->tooltip_wood > 0)
        {
            vec2 bottomPanel = RECT_TOP_LEFT(map->bottomPanel);

            if (imui->tooltip_wood > 0)
            {
                WarSprite woodSprite = imui_getOrCreateSprite(context, imageResourceRef(407), 0);
                if (woodSprite.texture)
                {
                    vec2 iconPos = vec2_addv(bottomPanel, vec2i(163, 3));
                    vec2 textPos = vec2_addv(bottomPanel, vec2i(179, 5));

                    wr_save(context);
                    wr_translate(context, iconPos.x, iconPos.y);
                    wspr_renderSprite(context, woodSprite, VEC2_ZERO, VEC2_ONE);
                    wr_restore(context);

                    char woodText[16];
                    StringView woodTextStr = wsv_fromCStringFormat(woodText, arrayLength(woodText), "%d", imui->tooltip_wood);
                    wfont_renderSingleSpriteText(context, woodTextStr, textPos.x, textPos.y, params);
                }
            }

            if (imui->tooltip_gold > 0)
            {
                WarSprite goldSprite = imui_getOrCreateSprite(context, imageResourceRef(406), 0);
                if (goldSprite.texture)
                {
                    vec2 iconPos = vec2_addv(bottomPanel, vec2i(200, 5));
                    vec2 textPos = vec2_addv(bottomPanel, vec2i(218, 5));

                    wr_save(context);
                    wr_translate(context, iconPos.x, iconPos.y);
                    wspr_renderSprite(context, goldSprite, VEC2_ZERO, VEC2_ONE);
                    wr_restore(context);

                    char goldText[16];
                    StringView goldTextStr = wsv_fromCStringFormat(goldText, arrayLength(goldText), "%d", imui->tooltip_gold);
                    wfont_renderSingleSpriteText(context, goldTextStr, textPos.x, textPos.y, params);
                }
            }
        }
    }

    // Draw the cursor sprite on top of everything else.
    // The hotspot offset is read from the cursor resource so the sprite is
    // positioned so that the logical pointer tip sits at the mouse coordinates.
    {
        WarSpriteResourceRef cursorRef = imageResourceRef(imui->cursor_type);
        WarSprite cursorSprite = imui_getOrCreateSprite(context, cursorRef, 0);
        if (cursorSprite.texture)
        {
            WarResource* resource = wres_getOrCreateResource(context, imui->cursor_type);
            assert(resource->type == WAR_RESOURCE_TYPE_CURSOR);

            vec2 hotspot = vec2i(resource->cursor.hotx, resource->cursor.hoty);
            vec2 pos     = vec2_subv(input->pos, hotspot);

            wr_save(context);
            wr_translate(context, pos.x, pos.y);
            wspr_renderSprite(context, cursorSprite, VEC2_ZERO, VEC2_ONE);
            wr_restore(context);
        }
    }
}

// ---------------------------------------------------------------------------
// Primitives
// ---------------------------------------------------------------------------

void imui_text(WarContext* context, const char* id, const CreateUITextArgs* args)
{
    NOT_USED(id);
    assert(args);

    WarFontParams params;
    params.fontIndex      = args->fontIndex;
    params.fontSize       = args->fontSize;
    params.lineHeight     = args->lineHeight;
    params.fontColor      = args->fontColor;
    params.highlightColor = args->highlightColor;
    params.highlightIndex = args->highlightIndex;
    params.highlightCount = args->highlightCount;
    params.boundings      = args->boundings;
    params.horizontalAlign = args->horizontalAlign;
    params.verticalAlign   = args->verticalAlign;
    params.lineAlign       = args->lineAlign;
    params.wrapping        = args->wrapping;
    params.trimming        = args->trimming;
    params.fontSprite      = context->fontSprites[args->fontIndex];
    params.fontData        = getFontData(args->fontIndex);

    wr_save(context);
    wr_translate(context, args->position.x, args->position.y);
    wr_scale(context, args->scale.x, args->scale.y);

    if (args->multiline)
    {
        wfont_renderMultiSpriteText(context, args->text, 0.0f, 0.0f, params);
    }
    else
    {
        wfont_renderSingleSpriteText(context, args->text, 0.0f, 0.0f, params);
    }

    wr_restore(context);
}

void imui_rect(WarContext* context, const char* id, const CreateUIRectArgs* args)
{
    NOT_USED(id);
    assert(args);

    wr_save(context);
    wr_translate(context, args->position.x, args->position.y);
    wr_scale(context, args->scale.x, args->scale.y);

    rect r = rectv(VEC2_ZERO, args->size);
    wr_fillRect(context, r, args->color);

    wr_restore(context);
}

void imui_image(WarContext* context, const char* id, const CreateUIImageArgs* args)
{
    NOT_USED(id);
    assert(args);

    WarSprite sprite = imui_getOrCreateSprite(context, args->spriteRef, 0);
    if (!sprite.texture)
    {
        return;
    }

    wr_save(context);
    wr_translate(context, args->position.x, args->position.y);
    wr_scale(context, args->scale.x, args->scale.y);

    wspr_renderSprite(context, sprite, VEC2_ZERO, VEC2_ONE);

    wr_restore(context);
}

// Draw a specific frame of a sprite resource.  Unlike imui_image (which always
// renders frame 0), this function keys the cache on (resourceIndex, frameIndex)
// so multiple frames of the same resource can be drawn in a single pass.
void imui_image_frame(WarContext* context, const char* id, const CreateUIImageArgs* args, s32 frameIndex)
{
    NOT_USED(id);
    assert(args);

    WarSprite sprite = imui_getOrCreateSprite(context, args->spriteRef, frameIndex);
    if (!sprite.texture)
    {
        return;
    }

    WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite, frameIndex);

    wr_save(context);
    wr_translate(context, -(f32)frame.dx, -(f32)frame.dy);
    wr_translate(context, args->position.x, args->position.y);
    wr_scale(context, args->scale.x, args->scale.y);

    wspr_renderSprite(context, sprite, VEC2_ZERO, VEC2_ONE);

    wr_restore(context);
}

// ---------------------------------------------------------------------------
// Internal: shared button click/hover logic
// ---------------------------------------------------------------------------

// Evaluate hover + active state for a button rect.
// Sets hot_item / active_item, updates is_mouse_over_ui.
// Returns true the frame the button is considered clicked.
static bool imui_evalButton(WarContext* context, u32 itemId, rect buttonRect, WarKeys hotKey)
{
    WarInput*     input = &context->input;
    WarImuiState* imui  = &context->imui;

    bool isHovered = rect_containsf(buttonRect, input->pos.x, input->pos.y);

    if (isHovered)
    {
        imui->hot_item        = itemId;
        imui->is_mouse_over_ui = true;

        if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
        {
            imui->active_item = itemId;
        }
    }

    // Hot-key: fires immediately on key release regardless of mouse position.
    // Suppressed when hotkeys_enabled is false (e.g. cheat panel is open).
    if (imui->hotkeys_enabled && hotKey != WAR_KEY_NONE && isKeyJustReleased(input, hotKey))
    {
        return true;
    }

    // Mouse click: fires when released over the same item that was pressed.
    if (isButtonJustReleased(input, WAR_MOUSE_LEFT) && imui->active_item == itemId)
    {
        if (isHovered)
        {
            wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId = WAR_UI_CLICK, .loop = false));
            return true;
        }
    }

    return false;
}

// ---------------------------------------------------------------------------
// Interactive elements
// ---------------------------------------------------------------------------

bool imui_image_button(WarContext* context, const char* id, const CreateUIImageButtonArgs* args)
{
    assert(id);
    assert(args);

    if (!args->interactive)
    {
        return false;
    }

    u32 itemId = imui_hashCString(id);

    WarSprite normalSprite  = imui_getOrCreateSprite(context, args->backgroundNormalRef, 0);
    WarSprite pressedSprite = imui_getOrCreateSprite(context, args->backgroundPressedRef, 0);
    WarSprite fgSprite      = imui_getOrCreateSprite(context, args->foregroundRef, args->foregroundFrameIndex);

    f32 btnW = normalSprite.frameWidth  > 0 ? (f32)normalSprite.frameWidth  : 16.0f;
    f32 btnH = normalSprite.frameHeight > 0 ? (f32)normalSprite.frameHeight : 16.0f;

    rect buttonRect = rectf(
        args->position.x * args->scale.x,
        args->position.y * args->scale.y,
        btnW * args->scale.x,
        btnH * args->scale.y
    );

    bool clicked = imui_evalButton(context, itemId, buttonRect, args->hotKey);

    bool isActive = (context->imui.active_item == itemId) &&
                    isButtonHeld(&context->input, WAR_MOUSE_LEFT) &&
                    (context->imui.hot_item == itemId);

    // Tooltip deferral
    if (context->imui.hot_item == itemId && args->tooltip.length > 0)
    {
        imui_deferTooltip(context, args->tooltip, args->tooltipHighlightIndex, args->tooltipHighlightCount, args->gold, args->wood);
    }

    // Render
    wr_save(context);
    wr_translate(context, args->position.x, args->position.y);
    wr_scale(context, args->scale.x, args->scale.y);

    // Background
    {
        WarSprite bg = isActive ? pressedSprite : normalSprite;
        if (bg.texture)
        {
            wspr_renderSprite(context, bg, VEC2_ZERO, VEC2_ONE);
        }
    }

    // Foreground icon, centred inside the background
    if (fgSprite.texture && fgSprite.framesCount > 0)
    {
        vec2 bgSize = vec2i(normalSprite.frameWidth,  normalSprite.frameHeight);
        vec2 fgSize = vec2i(fgSprite.frameWidth, fgSprite.frameHeight);
        vec2 offset = vec2_half(vec2_subv(bgSize, fgSize));

        if (isActive)
        {
            offset = vec2_addv(offset, vec2i(0, 1));
        }

        wr_translate(context, offset.x, offset.y);
        wspr_renderSprite(context, fgSprite, VEC2_ZERO, VEC2_ONE);
    }

    wr_restore(context);

    return clicked;
}

bool imui_text_button(WarContext* context, const char* id, const CreateUITextButtonArgs* args)
{
    assert(id);
    assert(args);

    if (!args->interactive)
    {
        return false;
    }

    u32 itemId = imui_hashCString(id);

    WarSprite normalSprite  = imui_getOrCreateSprite(context, args->backgroundNormalRef, 0);
    WarSprite pressedSprite = imui_getOrCreateSprite(context, args->backgroundPressedRef, 0);

    f32 btnW = normalSprite.frameWidth  > 0 ? (f32)normalSprite.frameWidth  : 64.0f;
    f32 btnH = normalSprite.frameHeight > 0 ? (f32)normalSprite.frameHeight : 16.0f;

    rect buttonRect = rectf(
        args->position.x * args->scale.x,
        args->position.y * args->scale.y,
        btnW * args->scale.x,
        btnH * args->scale.y
    );

    bool clicked = imui_evalButton(context, itemId, buttonRect, args->hotKey);

    bool isActive = (context->imui.active_item == itemId) &&
                    isButtonHeld(&context->input, WAR_MOUSE_LEFT) &&
                    (context->imui.hot_item == itemId);
    bool isHot    = (context->imui.hot_item == itemId);

    // Tooltip deferral
    if (isHot && args->tooltip.length > 0)
    {
        imui_deferTooltip(context, args->tooltip, args->tooltipHighlightIndex, args->tooltipHighlightCount, args->gold, args->wood);
    }

    // Render
    wr_save(context);
    wr_translate(context, args->position.x, args->position.y);
    wr_scale(context, args->scale.x, args->scale.y);

    // Background
    {
        WarSprite bg = isActive ? pressedSprite : normalSprite;
        if (bg.texture)
        {
            wspr_renderSprite(context, bg, VEC2_ZERO, VEC2_ONE);
        }
    }

    // Text label, centred inside background bounds
    if (args->text.data && args->text.length > 0)
    {
        WarFontParams params;
        params.fontIndex      = args->fontIndex;
        params.fontSize       = args->fontSize;
        params.lineHeight     = args->lineHeight;
        params.fontColor      = args->fontColor;
        params.highlightColor = args->highlightColor;
        // Highlight the full label when hovered (matches retained-mode button behaviour)
        params.highlightIndex = isHot ? ALL_HIGHLIGHT : args->highlightIndex;
        params.highlightCount = args->highlightCount;
        params.boundings      = vec2f(btnW, btnH);
        params.horizontalAlign = args->horizontalAlign;
        params.verticalAlign   = args->verticalAlign;
        params.lineAlign       = args->lineAlign;
        params.wrapping        = args->wrapping;
        params.trimming        = args->trimming;
        params.fontSprite      = context->fontSprites[args->fontIndex];
        params.fontData        = getFontData(args->fontIndex);

        wfont_renderSingleSpriteText(context, args->text, 0.0f, 0.0f, params);
    }

    wr_restore(context);

    return clicked;
}
