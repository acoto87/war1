#pragma once

#include <stdarg.h>

#include "shl/wstr.h"

#include "war_types.h"

bool isUIEntity(WarEntity* entity);

void clearUIText(WarEntity* uiText);
void setUIText(WarEntity* uiText, String text);

void setUIImage(WarEntity* uiImage, s32 frameIndex);
void setUIRectWidth(WarEntity* uiRect, s32 width);

void clearUITooltip(WarEntity* uiButton);
void setUITooltip(WarEntity* uiButton, s32 highlightIndex, s32 highlightCount, String text);

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

void setUIButtonStatusByName(WarContext* context, StringView name, bool enabled);
void setUIButtonInteractiveByName(WarContext* context, StringView name, bool interactive);
void setUIButtonHotKeyByName(WarContext* context, StringView name, WarKeys key);
void setUIEntityStatusByName(WarContext* context, StringView name, bool enabled);

WarEntity* createUIText(WarContext* context, String name, s32 fontIndex, f32 fontSize, String text, vec2 position);
WarEntity* createUIRect(WarContext* context, String name, vec2 position, vec2 size, WarColor color);
WarEntity* createUIImage(WarContext* context, String name, WarSpriteResourceRef spriteResourceRef, vec2 position);
WarEntity* createUICursor(WarContext* context, String name, WarCursorType type, vec2 position);
WarEntity* createUITextButton(WarContext* context,
                              String name,
                              s32 fontIndex,
                              f32 fontSize,
                              String text,
                              WarSpriteResourceRef backgroundNormalRef,
                              WarSpriteResourceRef backgroundPressedRef,
                              WarSpriteResourceRef foregroundRef,
                              vec2 position);
WarEntity* createUIImageButton(WarContext* context,
                               String name,
                               WarSpriteResourceRef backgroundNormalRef,
                               WarSpriteResourceRef backgroundPressedRef,
                               WarSpriteResourceRef foregroundRef,
                               vec2 position);

void wmap_changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type);
void updateUICursor(WarContext* context);
void updateUIButtons(WarContext* context, bool hotKeysEnabled);

void renderUIEntities(WarContext* context);
