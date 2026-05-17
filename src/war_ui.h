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

void wui_setUITextHighlight(WarContext* context, WarEntity* uiButton, s32 highlightIndex, s32 highlightCount);

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
    vec2 position;
    vec2 rotation;
    vec2 scale;
    String text;
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
    WarTextWrapping  wrapping;
    WarTextTrimming  trimming;
    bool multiline;
} CreateUITextArgs;

typedef struct
{
    vec2 position;
    vec2 rotation;
    vec2 scale;
    vec2 size;
    WarColor color;
} CreateUIRectArgs;

typedef struct
{
    WarSpriteResourceRef spriteRef;
    vec2 position;
    vec2 rotation;
    vec2 scale;
} CreateUIImageArgs;

typedef struct
{
    WarCursorType type;
    vec2 position;
    vec2 rotation;
    vec2 scale;
    s32 frameIndex;
} CreateUICursorArgs;

typedef struct
{
    vec2 position;
    vec2 rotation;
    vec2 scale;

    s32 fontIndex;
    f32 fontSize;
    String text;
    f32 lineHeight;
    WarColor fontColor;
    WarColor highlightColor;
    s32 highlightIndex;
    s32 highlightCount;
    WarTextAlignment horizontalAlign;
    WarTextAlignment verticalAlign;
    WarTextAlignment lineAlign;
    WarTextWrapping wrapping;
    WarTextTrimming trimming;

    WarSpriteResourceRef backgroundNormalRef;
    WarSpriteResourceRef backgroundPressedRef;
    WarSpriteResourceRef foregroundRef;

    bool interactive;
    WarKeys hotKey;
    String tooltip;
    s32 tooltipHighlightIndex;
    s32 tooltipHighlightCount;
    s32 gold;
    s32 wood;
    WarClickHandler clickHandler;
} CreateUITextButtonArgs;

typedef struct
{
    WarSpriteResourceRef backgroundNormalRef;
    WarSpriteResourceRef backgroundPressedRef;
    WarSpriteResourceRef foregroundRef;
    vec2 position;
    vec2 rotation;
    vec2 scale;

    bool interactive;
    WarKeys hotKey;
    String tooltip;
    s32 tooltipHighlightIndex;
    s32 tooltipHighlightCount;
    s32 gold;
    s32 wood;
    s32 foregroundFrameIndex;
    WarClickHandler clickHandler;
} CreateUIImageButtonArgs;

#define CREATE_UI_TEXT_ARGS_INIT_CONST(...) { \
    .scale          = {1, 1}, \
    .fontSize       = 10, \
    .fontColor      = FONT_NORMAL_COLOR_INIT, \
    .highlightColor = FONT_HIGHLIGHT_COLOR_INIT, \
    .highlightIndex = NO_HIGHLIGHT, \
    .verticalAlign  = WAR_TEXT_ALIGN_TOP, \
    .lineAlign      = WAR_TEXT_ALIGN_LEFT, \
    .wrapping       = WAR_TEXT_WRAP_NONE, \
    __VA_ARGS__ \
}
#define CREATE_UI_TEXT_ARGS_INIT(...) (&(CreateUITextArgs)CREATE_UI_TEXT_ARGS_INIT_CONST(__VA_ARGS__))

#define CREATE_UI_RECT_ARGS_INIT_CONST(...) { \
    .scale = {1, 1}, \
    __VA_ARGS__ \
}
#define CREATE_UI_RECT_ARGS_INIT(...) (&(CreateUIRectArgs)CREATE_UI_RECT_ARGS_INIT_CONST(__VA_ARGS__))

#define CREATE_UI_IMAGE_ARGS_INIT_CONST(...) { \
    .scale = {1, 1}, \
    __VA_ARGS__ \
}
#define CREATE_UI_IMAGE_ARGS_INIT(...) (&(CreateUIImageArgs)CREATE_UI_IMAGE_ARGS_INIT_CONST(__VA_ARGS__))

#define CREATE_UI_CURSOR_ARGS_INIT_CONST(...) { \
    .scale = {1, 1}, \
    __VA_ARGS__ \
}
#define CREATE_UI_CURSOR_ARGS_INIT(...) (&(CreateUICursorArgs)CREATE_UI_CURSOR_ARGS_INIT_CONST(__VA_ARGS__))

#define CREATE_UI_TEXT_BUTTON_ARGS_INIT_CONST(...) { \
    .scale                 = {1, 1}, \
    .fontSize              = 10, \
    .fontColor             = FONT_NORMAL_COLOR_INIT, \
    .highlightColor        = FONT_HIGHLIGHT_COLOR_INIT, \
    .highlightIndex        = NO_HIGHLIGHT, \
    .foregroundRef         = { -1, 0, {0} }, \
    .interactive           = true, \
    .hotKey                = WAR_KEY_NONE, \
    .tooltipHighlightIndex = NO_HIGHLIGHT, \
    .horizontalAlign       = WAR_TEXT_ALIGN_CENTER, \
    .verticalAlign         = WAR_TEXT_ALIGN_MIDDLE, \
    __VA_ARGS__ \
}
#define CREATE_UI_TEXT_BUTTON_ARGS_INIT(...) (&(CreateUITextButtonArgs)CREATE_UI_TEXT_BUTTON_ARGS_INIT_CONST(__VA_ARGS__))

#define CREATE_UI_IMAGE_BUTTON_ARGS_INIT_CONST(...) { \
    .scale                 = {1, 1}, \
    .foregroundRef         = { -1, 0, {0} }, \
    .interactive           = true, \
    .hotKey                = WAR_KEY_NONE, \
    .tooltipHighlightIndex = NO_HIGHLIGHT, \
    .foregroundFrameIndex  = 0, \
    __VA_ARGS__ \
}
#define CREATE_UI_IMAGE_BUTTON_ARGS_INIT(...) (&(CreateUIImageButtonArgs)CREATE_UI_IMAGE_BUTTON_ARGS_INIT_CONST(__VA_ARGS__))

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
