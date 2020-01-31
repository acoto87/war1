bool isUIEntity(WarEntity* entity);

void clearUIText(WarEntity* uiText);
void setUIText(WarEntity* uiText, const char* text);
void setUITextFormatv(WarEntity* uiText, const char* format, va_list args);
void setUITextFormat(WarEntity* uiText, const char* format, ...);

void setUIImage(WarEntity* uiImage, s32 frameIndex);
void setUIRectWidth(WarEntity* uiRect, s32 width);

void clearUITooltip(WarEntity* uiButton);
void setUITooltip(WarEntity* uiButton, s32 highlightIndex, s32 highlightCount, char* text);

#define setUIEntityStatus(uiEntity, value) ((uiEntity)->ui.enabled = (value))

#define setUITextBoundings(uiEntity, value) ((uiEntity)->text.boundings = (value))
#define setUITextHorizontalAlign(uiEntity, value) ((uiEntity)->text.horizontalAlign = (value))
#define setUITextVerticalAlign(uiEntity, value) ((uiEntity)->text.verticalAlign = (value))
#define setUITextLineAlign(uiEntity, value) ((uiEntity)->text.lineAlign = (value))
#define setUITextWrapping(uiEntity, value) ((uiEntity)->text.wrapping = (value))
#define setUITextColor(uiEntity, value) ((uiEntity)->text.fontColor = (value))
#define setUITextHighlight(uiEntity, index, count) \
    ({ ((uiEntity)->text.highlightIndex = (index)); \
       ((uiEntity)->text.highlightCount = (count)); })
#define setUITextHighlightColor(uiEntity, color) ((uiEntity)->text.highlightColor = (color))
#define setUITextMultiline(uiEntity, value) ((uiEntity)->text.multiline = (value))
#define setUITextLineHeight(uiEntity, value) ((uiEntity)->text.lineHeight = (value))
#define setUITextTrimming(uiEntity, value) ((uiEntity)->text.trimming = (value))

#define setUIButtonStatus(uiEntity, value) ((uiEntity)->button.enabled = (value))
#define setUIButtonInteractive(uiEntity, value) ((uiEntity)->button.interactive = (value))
#define setUIButtonHotKey(uiEntity, key) ((uiEntity)->button.hotKey = (key))
#define setUIButtonClickHandler(uiEntity, handler) ((uiEntity)->button.clickHandler = (handler))

void setUIButtonStatusByName(WarContext* context, const char* name, bool enabled);
void setUIButtonInteractiveByName(WarContext* context, const char* name, bool interactive);
void setUIButtonHotKeyByName(WarContext* context, const char* name, WarKeys key);
void setUIEntityStatusByName(WarContext* context, const char* name, bool enabled);

WarEntity* createUIText(WarContext* context, char* name, s32 fontIndex, f32 fontSize, const char* text, vec2 position);
WarEntity* createUIRect(WarContext* context, char* name, vec2 position, vec2 size, u8Color color);
WarEntity* createUIImage(WarContext* context, char* name, WarSpriteResourceRef spriteResourceRef, vec2 position);
WarEntity* createUICursor(WarContext* context, char* name, WarCursorType type, vec2 position);
WarEntity* createUITextButton(WarContext* context,
                              char* name,
                              s32 fontIndex,
                              f32 fontSize,
                              const char* text,
                              WarSpriteResourceRef backgroundNormalRef,
                              WarSpriteResourceRef backgroundPressedRef,
                              WarSpriteResourceRef foregroundRef,
                              vec2 position);
WarEntity* createUIImageButton(WarContext* context,
                               char* name,
                               WarSpriteResourceRef backgroundNormalRef,
                               WarSpriteResourceRef backgroundPressedRef,
                               WarSpriteResourceRef foregroundRef,
                               vec2 position);

void changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type);
void updateUICursor(WarContext* context);
void updateUIButtons(WarContext* context, bool hotKeysEnabled);

void renderUIEntities(WarContext* context);