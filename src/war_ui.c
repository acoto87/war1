bool isUIEntity(WarEntity* entity)
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

void clearUIText(WarEntity* uiText)
{
    if (uiText->text.text)
    {
        free(uiText->text.text);
        uiText->text.text = NULL;
        uiText->text.enabled = false;
    }
}

void setUIText(WarEntity* uiText, const char* text)
{
    clearUIText(uiText);

    if (text)
    {
        uiText->text.text = (char*)xmalloc(strlen(text) + 1);
        strcpy(uiText->text.text, text);
        uiText->text.enabled = true;
    }
}

void setUITextFormatv(WarEntity* uiText, const char* format, va_list args)
{
    if (!format)
    {
        setUIText(uiText, NULL);
        return;
    }

	char buffer[256];
    vsprintf(buffer, format, args);
    setUIText(uiText, buffer);
}

void setUITextFormat(WarEntity* uiText, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    setUITextFormatv(uiText, format, args);
    va_end(args);
}

void setUIImage(WarEntity* uiImage, s32 frameIndex)
{
    uiImage->sprite.frameIndex = frameIndex;
    uiImage->sprite.enabled = frameIndex >= 0;
}

void setUIRectWidth(WarEntity* uiRect, s32 width)
{
    uiRect->rect.size.x = width;
    uiRect->rect.enabled = width > 0;
}

void clearUITooltip(WarEntity* uiButton)
{
    memset(uiButton->button.tooltip, 0, sizeof(uiButton->button.tooltip));
}

void setUITooltip(WarEntity* uiButton, s32 highlightIndex, s32 highlightCount, char* text)
{
    clearUITooltip(uiButton);

    if (text)
    {
        uiButton->button.highlightIndex = highlightIndex;
        uiButton->button.highlightCount = highlightCount;
        strcpy(uiButton->button.tooltip, text);
    }
}

void setUIButtonStatusByName(WarContext* context, const char* name, bool enabled)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        setUIButtonStatus(entity, enabled);
    }
}

void setUIButtonInteractiveByName(WarContext* context, const char* name, bool interactive)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        setUIButtonInteractive(entity, interactive);
    }
}

void setUIButtonHotKeyByName(WarContext* context, const char* name, WarKeys key)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        setUIButtonHotKey(entity, key);
    }
}

void setUIEntityStatusByName(WarContext* context, const char* name, bool enabled)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        setUIEntityStatus(entity, enabled);
    }
}

WarEntity* createUIText(WarContext* context, char* name, s32 fontIndex, f32 fontSize, const char* text, vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_TEXT, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addTextComponent(context, entity, fontIndex, fontSize, text);

    return entity;
}

WarEntity* createUIRect(WarContext* context, char* name, vec2 position, vec2 size, u8Color color)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_RECT, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addRectComponent(context, entity, size, color);

    return entity;
}

WarEntity* createUIImage(WarContext* context, char* name, WarSpriteResourceRef spriteResourceRef, vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addSpriteComponentFromResource(context, entity, spriteResourceRef);

    return entity;
}

WarEntity* createUICursor(WarContext* context, char* name, WarCursorType type, vec2 position)
{
    WarResource* resource = getOrCreateResource(context, type);
    assert(resource->type == WAR_RESOURCE_TYPE_CURSOR);

    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_CURSOR, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addSpriteComponentFromResource(context, entity, imageResourceRef(type));
    addCursorComponent(context, entity, type, vec2i(resource->cursor.hotx, resource->cursor.hoty));

    return entity;
}

WarEntity* createUITextButton(WarContext* context,
                              char* name,
                              s32 fontIndex,
                              f32 fontSize,
                              const char* text,
                              WarSpriteResourceRef backgroundNormalRef,
                              WarSpriteResourceRef backgroundPressedRef,
                              WarSpriteResourceRef foregroundRef,
                              vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addTextComponent(context, entity, fontIndex, fontSize, text);
    addSpriteComponentFromResource(context, entity, foregroundRef);
    addButtonComponentFromResource(context, entity, backgroundNormalRef, backgroundPressedRef);

    WarSprite* normalSprite = &entity->button.normalSprite;
    vec2 backgroundSize = vec2i(normalSprite->frameWidth, normalSprite->frameHeight);

    setUITextBoundings(entity, backgroundSize);
    setUITextHorizontalAlign(entity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(entity, WAR_TEXT_ALIGN_MIDDLE);

    return entity;
}

WarEntity* createUIImageButton(WarContext* context,
                               char* name,
                               WarSpriteResourceRef backgroundNormalRef,
                               WarSpriteResourceRef backgroundPressedRef,
                               WarSpriteResourceRef foregroundRef,
                               vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addSpriteComponentFromResource(context, entity, foregroundRef);
    addButtonComponentFromResource(context, entity, backgroundNormalRef, backgroundPressedRef);

    return entity;
}

void changeCursorType(WarContext* context, WarEntity* entity, WarCursorType type)
{
    assert(entity->type == WAR_ENTITY_TYPE_CURSOR);

    if (entity->cursor.type != type)
    {
        WarResource* resource = getOrCreateResource(context, type);
        assert(resource->type == WAR_RESOURCE_TYPE_CURSOR);

        removeCursorComponent(context, entity);
        addCursorComponent(context, entity, type, vec2i(resource->cursor.hotx, resource->cursor.hoty));

        removeSpriteComponent(context, entity);
        addSpriteComponentFromResource(context, entity, imageResourceRef(type));
    }
}

void updateUICursor(WarContext* context)
{
    WarInput* input = &context->input;

    WarEntity* entity = findUIEntity(context, "cursor");
    if (entity)
    {
        entity->transform.position = vec2Subv(input->pos, entity->cursor.hot);
        changeCursorType(context, entity, WAR_CURSOR_ARROW);
    }
}

void updateUIButtons(WarContext* context, bool hotKeysEnabled)
{
    WarInput* input = &context->input;

    WarEntityList* buttons = getEntitiesOfType(context, WAR_ENTITY_TYPE_BUTTON);

    // store the buttons to update in this frame first
    // because the action of some buttons is to show other buttons
    // in their same location, and if the newly shown button is
    // after in the list, then it will update in this same frame
    // which it shouldn't happen
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
            }
            else
            {
                button->hot = false;
                button->active = false;
            }
        }
    }

    for(s32 i = 0; i < buttons->count; i++)
    {
        WarEntity* entity = buttons->items[i];
        if (entity && WarEntityIdSetContains(&buttonsToUpdate, entity->id))
        {
            WarTransformComponent* transform = &entity->transform;
            WarButtonComponent* button = &entity->button;

            if (hotKeysEnabled && wasKeyPressed(input, button->hotKey))
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

            vec2 backgroundSize = vec2i(button->normalSprite.frameWidth, button->normalSprite.frameHeight);
            rect buttonRect = rectv(transform->position, backgroundSize);
            bool pointerInside = rectContainsf(buttonRect, input->pos.x, input->pos.y);

            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if (button->active)
                {
                    if (pointerInside && button->clickHandler)
                    {
                        button->clickHandler(context, entity);
                        createAudio(context, WAR_UI_CLICK, false);
                    }

                    button->active = false;
                }

            }
            else if (isButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if (button->hot)
                    button->active = true;
            }
            else if (pointerInside)
            {
                for(s32 j = 0; j < buttons->count; j++)
                {
                    WarEntity* otherButton = buttons->items[i];
                    if (otherButton)
                    {
                        otherButton->button.hot = false;
                        otherButton->button.active = false;
                    }
                }

                button->hot = true;
            }
            else
            {
                button->hot = false;
                button->active = false;
            }
        }
    }

    WarEntityIdSetFree(&buttonsToUpdate);
}

void renderUIEntities(WarContext* context)
{
    WarEntityList* entities = getUIEntities(context);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity *entity = entities->items[i];
        if (entity)
        {
            renderEntity(context, entity);
        }
    }
}
