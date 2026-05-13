#include "war_ui.h"

#include <assert.h>
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

void wui_clearUIText(WarContext* context, WarEntity* uiText)
{
    WarTextComponent* text = we_getTextComponent(context, uiText);
    assert(text);

    wstr_free(text->text);
    text->text = wstr_make();

    we_disableComponent(context, uiText, COMP_TEXT);
}

void wui_setUIText(WarContext* context, WarEntity* uiText, String text)
{
    wui_clearUIText(context, uiText);

    if (text.data)
    {
        WarTextComponent* textComp = we_getTextComponent(context, uiText);
        assert(textComp);

        textComp->text = text;

        we_enableComponent(context, uiText, COMP_TEXT);
    }
}

void wui_setUIImage(WarContext* context, WarEntity* uiImage, s32 frameIndex)
{
    WarSpriteComponent* sprite = we_getSpriteComponent(context, uiImage);
    assert(sprite);

    sprite->frameIndex = frameIndex;
    we_setComponentEnabled(context, uiImage, COMP_SPRITE, frameIndex >= 0);
}

void wui_setUIRectWidth(WarContext* context, WarEntity* uiRect, s32 width)
{
    WarRectComponent* rect = we_getRectComponent(context, uiRect);
    assert(rect);

    rect->size.x = (f32)width;
    we_setComponentEnabled(context, uiRect, COMP_RECT, width > 0);
}

void wui_clearUITooltip(WarContext* context, WarEntity* uiButton)
{
    WarButtonComponent* button = we_getButtonComponent(context, uiButton);
    assert(button);

    wstr_free(button->tooltip);
    button->tooltip = wstr_make();
}

void wui_setUITooltip(WarContext* context, WarEntity* uiButton, s32 highlightIndex, s32 highlightCount, String text)
{
    wui_clearUITooltip(context, uiButton);

    if (text.data)
    {
        WarButtonComponent* button = we_getButtonComponent(context, uiButton);
        assert(button);

        button->highlightIndex = highlightIndex;
        button->highlightCount = highlightCount;
        button->tooltip = text;
    }
}

void wui_setUITextHighlight(WarContext* context, WarEntity* uiButton, s32 highlightIndex, s32 highlightCount)
{
    WarTextComponent* textComp = we_getTextComponent(context, uiButton);
    assert(textComp);

    textComp->highlightIndex = highlightIndex;
    textComp->highlightCount = highlightCount;
}

void setUIEntityStatus(WarContext* context, WarEntity* uiEntity, bool value)
{
    we_setComponentEnabled(context, uiEntity, COMP_UI, value);
}

void setUIButtonStatus(WarContext* context, WarEntity* uiEntity, bool value)
{
    we_setComponentEnabled(context, uiEntity, COMP_BUTTON, value);
}

void setUIButtonInteractive(WarContext* context, WarEntity* uiEntity, bool value)
{
    WarButtonComponent* button = we_getButtonComponent(context, uiEntity);
    assert(button);

    button->interactive = value;
}

void setUIButtonHotKey(WarContext* context, WarEntity* uiEntity, WarKeys key)
{
    WarButtonComponent* button = we_getButtonComponent(context, uiEntity);
    assert(button);

    button->hotKey = key;
}

void setUIButtonClickHandler(WarContext* context, WarEntity* uiEntity, void (*handler)(WarContext*, WarEntity*))
{
    WarButtonComponent* button = we_getButtonComponent(context, uiEntity);
    assert(button);

    button->clickHandler = handler;
}

void wui_setUIButtonStatusByName(WarContext* context, StringView name, bool enabled)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        setUIButtonStatus(context, entity, enabled);
    }
}

void wui_setUIButtonInteractiveByName(WarContext* context, StringView name, bool interactive)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        setUIButtonInteractive(context, entity, interactive);
    }
}

void wui_setUIButtonHotKeyByName(WarContext* context, StringView name, WarKeys key)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        setUIButtonHotKey(context, entity, key);
    }
}

void wui_setUIEntityStatusByName(WarContext* context, StringView name, bool enabled)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        setUIEntityStatus(context, entity, enabled);
    }
}

WarEntity* wui_createUIText(WarContext* context, String name, const CreateUITextArgs* args)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_TEXT, true);
    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = args->position,
        .rotation = args->rotation,
        .scale    = args->scale,
    ));
    we_addUIComponent(context, entity, name);
    we_addTextComponent(context, entity, WAR_TEXT_COMPONENT_INIT(
        .fontIndex       = args->fontIndex,
        .fontSize        = args->fontSize,
        .lineHeight      = args->lineHeight,
        .fontColor       = args->fontColor,
        .highlightColor  = args->highlightColor,
        .highlightIndex  = args->highlightIndex,
        .highlightCount  = args->highlightCount,
        .boundings       = args->boundings,
        .horizontalAlign = args->horizontalAlign,
        .verticalAlign   = args->verticalAlign,
        .lineAlign       = args->lineAlign,
        .wrapping        = args->wrapping,
        .trimming        = args->trimming,
        .multiline       = args->multiline,
        .text            = args->text,
    ));
    return entity;
}

WarEntity* wui_createUIRect(WarContext* context, String name, const CreateUIRectArgs* args)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_RECT, true);
    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = args->position,
        .rotation = args->rotation,
        .scale    = args->scale,
    ));
    we_addUIComponent(context, entity, name);
    we_addRectComponent(context, entity, WAR_RECT_COMPONENT_INIT(
        .size  = args->size,
        .color = args->color,
    ));

    return entity;
}

WarEntity* wui_createUIImage(WarContext* context, String name, const CreateUIImageArgs* args)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_IMAGE, true);
    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = args->position,
        .rotation = args->rotation,
        .scale    = args->scale,
    ));
    we_addUIComponent(context, entity, name);
    we_addSpriteComponentFromResource(context, entity, args->spriteRef);
    return entity;
}

WarEntity* wui_createUICursor(WarContext* context, String name, const CreateUICursorArgs* args)
{
    WarResource* resource = wres_getOrCreateResource(context, args->type);
    assert(resource->type == WAR_RESOURCE_TYPE_CURSOR);

    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_CURSOR, true);
    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = args->position,
        .rotation = args->rotation,
        .scale    = args->scale,
    ));
    we_addUIComponent(context, entity, name);
    WarSpriteComponent* sprite = we_addSpriteComponentFromResource(context, entity, imageResourceRef(args->type));
    assert(sprite);
    sprite->frameIndex = args->frameIndex;

    we_addCursorComponent(context, entity, WAR_CURSOR_COMPONENT_INIT(
        .type = args->type,
        .hot  = vec2i(resource->cursor.hotx, resource->cursor.hoty),
    ));

    return entity;
}

WarEntity* wui_createUITextButton(WarContext* context, String name, const CreateUITextButtonArgs* args)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = args->position,
        .rotation = args->rotation,
        .scale    = args->scale,
    ));
    we_addUIComponent(context, entity, name);
    we_addSpriteComponentFromResource(context, entity, args->foregroundRef);

    WarButtonComponent* button = we_addButtonComponentFromResource(context, entity, args->backgroundNormalRef, args->backgroundPressedRef);
    assert(button);

    button->interactive  = args->interactive;
    button->hotKey       = args->hotKey;
    button->gold         = args->gold;
    button->wood         = args->wood;
    button->clickHandler = args->clickHandler;
    button->highlightIndex = args->tooltipHighlightIndex;
    button->highlightCount = args->tooltipHighlightCount;
    button->tooltip = args->tooltip;

    vec2 backgroundSize = vec2i(button->normalSprite.frameWidth, button->normalSprite.frameHeight);
    vec2 boundings = backgroundSize;

    we_addTextComponent(context, entity, WAR_TEXT_COMPONENT_INIT(
        .fontIndex       = args->fontIndex,
        .fontSize        = args->fontSize,
        .lineHeight      = args->lineHeight,
        .fontColor       = args->fontColor,
        .highlightColor  = args->highlightColor,
        .highlightIndex  = args->highlightIndex,
        .highlightCount  = args->highlightCount,
        .boundings       = boundings,
        .horizontalAlign = args->horizontalAlign,
        .verticalAlign   = args->verticalAlign,
        .lineAlign       = args->lineAlign,
        .wrapping        = args->wrapping,
        .trimming        = args->trimming,
        .multiline       = false,
        .text            = args->text,
    ));

    return entity;
}

WarEntity* wui_createUIImageButton(WarContext* context, String name, const CreateUIImageButtonArgs* args)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = args->position,
        .rotation = args->rotation,
        .scale    = args->scale,
    ));
    we_addUIComponent(context, entity, name);
    we_addSpriteComponentFromResource(context, entity, args->foregroundRef);

    WarButtonComponent* button = we_addButtonComponentFromResource(context, entity, args->backgroundNormalRef, args->backgroundPressedRef);
    assert(button);

    button->interactive  = args->interactive;
    button->hotKey       = args->hotKey;
    button->gold         = args->gold;
    button->wood         = args->wood;
    button->clickHandler = args->clickHandler;
    button->highlightIndex = args->tooltipHighlightIndex;
    button->highlightCount = args->tooltipHighlightCount;
    button->tooltip = args->tooltip;

    return entity;
}

void wui_changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type)
{
    assert(entity->type == WAR_ENTITY_TYPE_CURSOR);

    WarCursorComponent* cursor = we_getCursorComponent(context, entity);
    assert(cursor);

    if (cursor->type != type)
    {
        WarResource* resource = wres_getOrCreateResource(context, type);
        assert(resource->type == WAR_RESOURCE_TYPE_CURSOR);

        we_removeCursorComponent(context, entity);
        we_addCursorComponent(context, entity, WAR_CURSOR_COMPONENT_INIT(
            .type = type,
            .hot  = vec2i(resource->cursor.hotx, resource->cursor.hoty),
        ));

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
        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        WarCursorComponent* cursor = we_getCursorComponent(context, entity);
        assert(cursor);

        transform->position = vec2_subv(input->pos, cursor->hot);
        wui_changeCursorType(context, entity, WAR_CURSOR_ARROW);
    }
}

void wui_updateUIButtons(WarContext* context, bool hotKeysEnabled)
{
    TracyCZoneN(ctx, "UpdateUIButtons", 1);

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
            WarButtonComponent* button = we_getButtonComponent(context, entity);
            assert(button);

            if (we_isComponentEnabled(context, entity, COMP_UI) && we_isComponentEnabled(context, entity, COMP_BUTTON) && button->interactive)
            {
                WarEntityIdSetAdd(&buttonsToUpdate, entity->id);

                WarTransformComponent* transform = we_getTransformComponent(context, entity);
                assert(transform);

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
            WarButtonComponent* button = we_getButtonComponent(context, entity);
            assert(button);

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
                    wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_UI_CLICK, .loop=false));
                }

                button->active = false;
                input->capturedUIButtonId = 0;
            }
        }
    }

    WarEntityIdSetFree(&buttonsToUpdate);

    TracyCZoneEnd(ctx);
}

void wui_renderUIEntities(WarContext* context)
{
    TracyCZoneN(ctx, "RenderUIEntities", 1);

    WarEntityList* entities = we_getUIEntities(context);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity *entity = entities->items[i];
        if (entity)
        {
            we_renderEntity(context, entity);
        }
    }

    TracyCZoneEnd(ctx);
}
