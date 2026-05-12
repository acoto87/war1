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

void setUIEntityStatus(WarContext* ctx, WarEntity* uiEntity, bool value);

void setUIButtonStatus(WarContext* ctx, WarEntity* uiEntity, bool value);
void setUIButtonInteractive(WarContext* ctx, WarEntity* uiEntity, bool value);
void setUIButtonHotKey(WarContext* ctx, WarEntity* uiEntity, WarKeys key);
void setUIButtonClickHandler(WarContext* ctx, WarEntity* uiEntity, void (*handler)(WarContext*, WarEntity*));

void wui_setUIButtonStatusByName(WarContext* context, StringView name, bool enabled);
void wui_setUIButtonInteractiveByName(WarContext* context, StringView name, bool interactive);
void wui_setUIButtonHotKeyByName(WarContext* context, StringView name, WarKeys key);
void wui_setUIEntityStatusByName(WarContext* context, StringView name, bool enabled);

typedef struct
{
    vec2     position;
    String   text;
    s32      fontIndex;
    f32      fontSize;
    f32      lineHeight;
    WarColor fontColor;
    vec2     boundings;
    WarTextAlignment horizontalAlign;
    WarTextAlignment verticalAlign;
    WarTextAlignment lineAlign;
    WarTextWrapping  wrapping;
    WarTextTrimming  trimming;
    bool     multiline;
} CreateUITextArgs;

typedef struct
{
    vec2     position;
    vec2     size;
    WarColor color;
} CreateUIRectArgs;

typedef struct
{
    WarSpriteResourceRef spriteRef;
    vec2                 position;
} CreateUIImageArgs;

typedef struct
{
    WarCursorType type;
    vec2          position;
} CreateUICursorArgs;

typedef struct
{
    s32                  fontIndex;
    f32                  fontSize;
    String               text;
    WarSpriteResourceRef backgroundNormalRef;
    WarSpriteResourceRef backgroundPressedRef;
    WarSpriteResourceRef foregroundRef;
    vec2                 position;
} CreateUITextButtonArgs;

typedef struct
{
    WarSpriteResourceRef backgroundNormalRef;
    WarSpriteResourceRef backgroundPressedRef;
    WarSpriteResourceRef foregroundRef;
    vec2                 position;
} CreateUIImageButtonArgs;

WarEntity* wui_createUIText(WarContext* context, String name, const CreateUITextArgs* args);

WarEntity* wui_createUIRect(WarContext* context, String name, const CreateUIRectArgs* args);
WarEntity* wui_createUIImage(WarContext* context, String name, const CreateUIImageArgs* args);
WarEntity* wui_createUICursor(WarContext* context, String name, const CreateUICursorArgs* args);
WarEntity* wui_createUITextButton(WarContext* context, String name, const CreateUITextButtonArgs* args);
WarEntity* wui_createUIImageButton(WarContext* context, String name, const CreateUIImageButtonArgs* args);

void wui_changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type);
void wui_updateUICursor(WarContext* context);
void wui_updateUIButtons(WarContext* context, bool hotKeysEnabled);

void wui_renderUIEntities(WarContext* context);
