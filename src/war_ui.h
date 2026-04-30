#pragma once

#include <stdarg.h>

#include "shl/wstr.h"

#include "war.h"

bool wui_isUIEntity(WarEntity* entity);

void wui_clearUIText(WarEntity* uiText);
void wui_setUIText(WarEntity* uiText, String text);

void wui_setUIImage(WarEntity* uiImage, s32 frameIndex);
void wui_setUIRectWidth(WarEntity* uiRect, s32 width);

void wui_clearUITooltip(WarEntity* uiButton);
void wui_setUITooltip(WarEntity* uiButton, s32 highlightIndex, s32 highlightCount, String text);

#define setUIEntityStatus(uiEntity, value) ((uiEntity)->ui.enabled = (value))

#define setUITextBoundings(uiEntity, value) ((uiEntity)->text.boundings = (value))
#define setUITextHorizontalAlign(uiEntity, value) ((uiEntity)->text.horizontalAlign = (value))
#define setUITextVerticalAlign(uiEntity, value) ((uiEntity)->text.verticalAlign = (value))
#define setUITextLineAlign(uiEntity, value) ((uiEntity)->text.lineAlign = (value))
#define setUITextWrapping(uiEntity, value) ((uiEntity)->text.wrapping = (value))
#define setUITextColor(uiEntity, value) ((uiEntity)->text.fontColor = (value))
#define setUITextHighlight(uiEntity, index, count) \
    do { ((uiEntity)->text.highlightIndex = (index)); \
       ((uiEntity)->text.highlightCount = (count)); } while (0)
#define setUITextHighlightColor(uiEntity, color) ((uiEntity)->text.highlightColor = (color))
#define setUITextMultiline(uiEntity, value) ((uiEntity)->text.multiline = (value))
#define setUITextLineHeight(uiEntity, value) ((uiEntity)->text.lineHeight = (value))
#define setUITextTrimming(uiEntity, value) ((uiEntity)->text.trimming = (value))

#define setUIButtonStatus(uiEntity, value) ((uiEntity)->button.enabled = (value))
#define setUIButtonInteractive(uiEntity, value) ((uiEntity)->button.interactive = (value))
#define setUIButtonHotKey(uiEntity, key) ((uiEntity)->button.hotKey = (key))
#define setUIButtonClickHandler(uiEntity, handler) ((uiEntity)->button.clickHandler = (handler))

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
