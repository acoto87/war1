#include "war_ui.h"

#include <stdlib.h>

#include "shl/wstr.h"

#include "war_audio.h"
#include "war_entities.h"
#include "war_resources.h"

bool wui_isUIEntity(WarEntity* entity)
{
    switch (entity->type)
    {
        case WAR_ENTITY_TYPE_IMAGE:
        case WAR_ENTITY_TYPE_TEXT:
        case WAR_ENTITY_TYPE_RECT:
        case WAR_ENTITY_TYPE_BUTTON:
        case WAR_ENTITY_TYPE_CURSOR:
        case WAR_ENTITY_TYPE_MINIMAP:
            return true;

        default:
            return false;
    }
}

void wui_clearUIText(WarEntity* uiText)
{
    wstr_free(uiText->text.text);
    uiText->text.text = wstr_make();
    uiText->text.enabled = false;
}

void wui_setUIText(WarEntity* uiText, String text)
{
    wui_clearUIText(uiText);

    if (text.data)
    {
        uiText->text.text = text;
        uiText->text.enabled = true;
    }
}

void wui_setUIImage(WarEntity* uiImage, s32 frameIndex)
{
    uiImage->sprite.frameIndex = frameIndex;
    uiImage->sprite.enabled = frameIndex >= 0;
}

void wui_setUIRectWidth(WarEntity* uiRect, s32 width)
{
    uiRect->rect.size.x = (f32)width;
    uiRect->rect.enabled = width > 0;
}

void wui_clearUITooltip(WarEntity* uiButton)
{
    wstr_free(uiButton->button.tooltip);
    uiButton->button.tooltip = wstr_make();
}

void wui_setUITooltip(WarEntity* uiButton, s32 highlightIndex, s32 highlightCount, String text)
{
    wui_clearUITooltip(uiButton);

    if (text.data)
    {
        uiButton->button.highlightIndex = highlightIndex;
        uiButton->button.highlightCount = highlightCount;
        uiButton->button.tooltip = text;
    }
}

void wui_setUIButtonStatusByName(WarContext* context, StringView name, bool enabled)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        setUIButtonStatus(entity, enabled);
    }
}

void wui_setUIButtonInteractiveByName(WarContext* context, StringView name, bool interactive)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        setUIButtonInteractive(entity, interactive);
    }
}

void wui_setUIButtonHotKeyByName(WarContext* context, StringView name, WarKeys key)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        setUIButtonHotKey(entity, key);
    }
}

void wui_setUIEntityStatusByName(WarContext* context, StringView name, bool enabled)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        setUIEntityStatus(entity, enabled);
    }
}

WarEntity* wui_createUIText(WarContext* context, String name, s32 fontIndex, f32 fontSize, String text, vec2 position)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_TEXT, true);
    we_addTransformComponent(context, entity, position);
    we_addUIComponent(context, entity, name);
    we_addTextComponent(context, entity, fontIndex, fontSize, text);

    return entity;
}

WarEntity* wui_createUIRect(WarContext* context, String name, vec2 position, vec2 size, WarColor color)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_RECT, true);
    we_addTransformComponent(context, entity, position);
    we_addUIComponent(context, entity, name);
    we_addRectComponent(context, entity, size, color);

    return entity;
}

WarEntity* wui_createUIImage(WarContext* context, String name, WarSpriteResourceRef spriteResourceRef, vec2 position)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_IMAGE, true);
    we_addTransformComponent(context, entity, position);
    we_addUIComponent(context, entity, name);
    we_addSpriteComponentFromResource(context, entity, spriteResourceRef);

    return entity;
}

WarEntity* wui_createUICursor(WarContext* context, String name, WarCursorType type, vec2 position)
{
    WarResource* resource = wres_getOrCreateResource(context, type);
    assert(resource->type == WAR_RESOURCE_TYPE_CURSOR);

    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_CURSOR, true);
    we_addTransformComponent(context, entity, position);
    we_addUIComponent(context, entity, name);
    we_addSpriteComponentFromResource(context, entity, imageResourceRef(type));
    we_addCursorComponent(context, entity, type, vec2i(resource->cursor.hotx, resource->cursor.hoty));

    return entity;
}

WarEntity* wui_createUITextButton(WarContext* context,
                              String name,
                              s32 fontIndex,
                              f32 fontSize,
                              String text,
                              WarSpriteResourceRef backgroundNormalRef,
                              WarSpriteResourceRef backgroundPressedRef,
                              WarSpriteResourceRef foregroundRef,
                              vec2 position)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    we_addTransformComponent(context, entity, position);
    we_addUIComponent(context, entity, name);
    we_addTextComponent(context, entity, fontIndex, fontSize, text);
    we_addSpriteComponentFromResource(context, entity, foregroundRef);
    we_addButtonComponentFromResource(context, entity, backgroundNormalRef, backgroundPressedRef);

    WarSprite* normalSprite = &entity->button.normalSprite;
    vec2 backgroundSize = vec2i(normalSprite->frameWidth, normalSprite->frameHeight);

    setUITextBoundings(entity, backgroundSize);
    setUITextHorizontalAlign(entity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(entity, WAR_TEXT_ALIGN_MIDDLE);

    return entity;
}

WarEntity* wui_createUIImageButton(WarContext* context,
                               String name,
                               WarSpriteResourceRef backgroundNormalRef,
                               WarSpriteResourceRef backgroundPressedRef,
                               WarSpriteResourceRef foregroundRef,
                               vec2 position)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    we_addTransformComponent(context, entity, position);
    we_addUIComponent(context, entity, name);
    we_addSpriteComponentFromResource(context, entity, foregroundRef);
    we_addButtonComponentFromResource(context, entity, backgroundNormalRef, backgroundPressedRef);

    return entity;
}

void wui_changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type)
{
    assert(entity->type == WAR_ENTITY_TYPE_CURSOR);

    if (entity->cursor.type != type)
    {
        WarResource* resource = wres_getOrCreateResource(context, type);
        assert(resource->type == WAR_RESOURCE_TYPE_CURSOR);

        we_removeCursorComponent(context, entity);
        we_addCursorComponent(context, entity, type, vec2i(resource->cursor.hotx, resource->cursor.hoty));

        we_removeSpriteComponent(context, entity);
        we_addSpriteComponentFromResource(context, entity, imageResourceRef(type));
    }
}

void wui_updateUICursor(WarContext* context)
{
    WarInput* input = &context->input;

    WarEntity* entity = we_findUIEntity(context, wsv_fromCString("cursor"));
    if (entity)
    {
        entity->transform.position = vec2_subv(input->pos, entity->cursor.hot);
        wui_changeCursorType(context, entity, WAR_CURSOR_ARROW);
    }
}

void wui_updateUIButtons(WarContext* context, bool hotKeysEnabled)
{
    WarInput* input = &context->input;
    WarEntityList* buttons = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_BUTTON);
    WarEntity* hoveredButton = NULL;
    WarEntity* capturedButton = NULL;

    // NOTE: Store the buttons to update in this frame first
    // because the action of some buttons is to show other buttons
    // in their same location, and if the newly shown button is
    // after in the list, then it will update in this same frame
    // which shouldn't happen
    WarEntityIdSet buttonsToUpdate;
    WarEntityIdSetInit(&buttonsToUpdate, WarEntityIdSetDefaultOptions);

    for(s32 i = 0; i < buttons->count; i++)
    {
        WarEntity* entity = buttons->items[i];
        if (entity)
        {
            WarUIComponent* ui = &entity->ui;
            WarButtonComponent* button = &entity->button;

            if (ui->enabled && button->enabled && button->interactive)
            {
                WarEntityIdSetAdd(&buttonsToUpdate, entity->id);

                WarTransformComponent* transform = &entity->transform;
                vec2 backgroundSize = vec2i(button->normalSprite.frameWidth, button->normalSprite.frameHeight);
                rect buttonRect = rectv(transform->position, backgroundSize);
                if (rect_containsf(buttonRect, input->pos.x, input->pos.y))
                {
                    hoveredButton = entity;
                }
            }
            else
            {
                button->hot = false;
                button->active = false;

                if (input->capturedUIButtonId == entity->id)
                {
                    input->capturedUIButtonId = 0;
                }
            }
        }
    }

    if (input->capturedUIButtonId)
    {
        capturedButton = we_findEntity(context, input->capturedUIButtonId);
        if (!capturedButton || !WarEntityIdSetContains(&buttonsToUpdate, capturedButton->id))
        {
            input->capturedUIButtonId = 0;
            capturedButton = NULL;
        }
    }

    if (isButtonJustPressed(input, WAR_MOUSE_LEFT) && hoveredButton)
    {
        input->capturedUIButtonId = hoveredButton->id;
        capturedButton = hoveredButton;
    }

    for(s32 i = 0; i < buttons->count; i++)
    {
        WarEntity* entity = buttons->items[i];
        if (entity && WarEntityIdSetContains(&buttonsToUpdate, entity->id))
        {
            WarButtonComponent* button = &entity->button;
            bool isHovered = entity == hoveredButton;
            bool isCaptured = entity == capturedButton;

            button->hot = isHovered;
            button->active = isHovered && isCaptured && isButtonHeld(input, WAR_MOUSE_LEFT);

            if (hotKeysEnabled && isKeyJustReleased(input, button->hotKey))
            {
                if (button->clickHandler)
                {
                    button->hot = false;
                    button->active = false;

                    button->clickHandler(context, entity);

                    // in this case break to not allow pressing multiple keys
                    // and executing all of the command for those keys
                    break;
                }
            }

            if (isButtonJustReleased(input, WAR_MOUSE_LEFT) && isCaptured)
            {
                if (isHovered && button->clickHandler)
                {
                    button->clickHandler(context, entity);
                    wa_createAudio(context, WAR_UI_CLICK, false);
                }

                button->active = false;
                input->capturedUIButtonId = 0;
            }
        }
    }

    WarEntityIdSetFree(&buttonsToUpdate);
}

void wui_renderUIEntities(WarContext* context)
{
    WarEntityList* entities = we_getUIEntities(context);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity *entity = entities->items[i];
        if (entity)
        {
            we_renderEntity(context, entity);
        }
    }
}
