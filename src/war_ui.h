#pragma once

#include <stdarg.h>

#include "shl/wstr.h"

#include "war.h"
#include "war_entities.h"

bool wui_isUIEntity(WarEntity* entity);

void wui_clearUIText(WarContext* context, WarEntity* uiText);
void wui_setUIText(WarContext* context, WarEntity* uiText, String text);

void wui_setUIImage(WarContext* context, WarEntity* uiImage, s32 frameIndex);
void wui_setUIRectWidth(WarContext* context, WarEntity* uiRect, s32 width);

void wui_clearUITooltip(WarContext* context, WarEntity* uiButton);
void wui_setUITooltip(WarContext* context, WarEntity* uiButton, s32 highlightIndex, s32 highlightCount, String text);

#define setUIEntityStatus(ctx, uiEntity, value) (we_getUIComponent((ctx), (uiEntity))->enabled = (value))

#define setUITextBoundings(ctx, uiEntity, value) (we_getTextComponent((ctx), (uiEntity))->boundings = (value))
#define setUITextHorizontalAlign(ctx, uiEntity, value) (we_getTextComponent((ctx), (uiEntity))->horizontalAlign = (value))
#define setUITextVerticalAlign(ctx, uiEntity, value) (we_getTextComponent((ctx), (uiEntity))->verticalAlign = (value))
#define setUITextLineAlign(ctx, uiEntity, value) (we_getTextComponent((ctx), (uiEntity))->lineAlign = (value))
#define setUITextWrapping(ctx, uiEntity, value) (we_getTextComponent((ctx), (uiEntity))->wrapping = (value))
#define setUITextColor(ctx, uiEntity, value) (we_getTextComponent((ctx), (uiEntity))->fontColor = (value))
#define setUITextHighlight(ctx, uiEntity, index, count) \
    do { (we_getTextComponent((ctx), (uiEntity))->highlightIndex = (index)); \
       (we_getTextComponent((ctx), (uiEntity))->highlightCount = (count)); } while (0)
#define setUITextHighlightColor(ctx, uiEntity, color) (we_getTextComponent((ctx), (uiEntity))->highlightColor = (color))
#define setUITextMultiline(ctx, uiEntity, value) (we_getTextComponent((ctx), (uiEntity))->multiline = (value))
#define setUITextLineHeight(ctx, uiEntity, value) (we_getTextComponent((ctx), (uiEntity))->lineHeight = (value))
#define setUITextTrimming(ctx, uiEntity, value) (we_getTextComponent((ctx), (uiEntity))->trimming = (value))

#define setUIButtonStatus(ctx, uiEntity, value) (we_getButtonComponent((ctx), (uiEntity))->enabled = (value))
#define setUIButtonInteractive(ctx, uiEntity, value) (we_getButtonComponent((ctx), (uiEntity))->interactive = (value))
#define setUIButtonHotKey(ctx, uiEntity, key) (we_getButtonComponent((ctx), (uiEntity))->hotKey = (key))
#define setUIButtonClickHandler(ctx, uiEntity, handler) (we_getButtonComponent((ctx), (uiEntity))->clickHandler = (handler))

void wui_setUIButtonStatusByName(WarContext* context, StringView name, bool enabled);
void wui_setUIButtonInteractiveByName(WarContext* context, StringView name, bool interactive);
void wui_setUIButtonHotKeyByName(WarContext* context, StringView name, WarKeys key);
void wui_setUIEntityStatusByName(WarContext* context, StringView name, bool enabled);

WarEntity* wui_createUIText(WarContext* context, String name, s32 fontIndex, f32 fontSize, String text, vec2 position);
WarEntity* wui_createUIRect(WarContext* context, String name, vec2 position, vec2 size, WarColor color);
WarEntity* wui_createUIImage(WarContext* context, String name, WarSpriteResourceRef spriteResourceRef, vec2 position);
WarEntity* wui_createUICursor(WarContext* context, String name, WarCursorType type, vec2 position);
WarEntity* wui_createUITextButton(WarContext* context,
                              String name,
                              s32 fontIndex,
                              f32 fontSize,
                              String text,
                              WarSpriteResourceRef backgroundNormalRef,
                              WarSpriteResourceRef backgroundPressedRef,
                              WarSpriteResourceRef foregroundRef,
                              vec2 position);
WarEntity* wui_createUIImageButton(WarContext* context,
                               String name,
                               WarSpriteResourceRef backgroundNormalRef,
                               WarSpriteResourceRef backgroundPressedRef,
                               WarSpriteResourceRef foregroundRef,
                               vec2 position);

void wui_changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type);
void wui_updateUICursor(WarContext* context);
void wui_updateUIButtons(WarContext* context, bool hotKeysEnabled);

void wui_renderUIEntities(WarContext* context);
