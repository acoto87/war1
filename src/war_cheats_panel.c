void createCheatsPanel(WarContext* context)
{
    WarScene* scene = context->scene;
    assert(scene);

    WarCheatStatus* cheatStatus = &scene->cheatStatus;

    cheatStatus->enabled = false;
    cheatStatus->position = 0;
    memset(cheatStatus->text, 0, sizeof(cheatStatus->text));

    WarEntity* uiEntity;

    vec2 cheatSize = vec2f(context->originalWindowWidth, 12);
    u8Color cheatBackgroundColor = u8RgbaColor(100, 100, 100, 160);
    uiEntity = createUIRect(context, "panelCheat", VEC2_ZERO, cheatSize, cheatBackgroundColor);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIText(context, "txtCheat", 0, 6, NULL, vec2i(2, 4));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIRect(context, "cursorCheat", vec2i(2, 3), vec2i(1, 7), U8COLOR_WHITE);
    setUIEntityStatus(uiEntity, false);
}

void setCheatText(WarContext* context, char* text, ...)
{
    WarEntity* txtCheat = findUIEntity(context, "txtCheat");
    assert(txtCheat);

    va_list args;
    va_start (args, text);
    setUITextFormatv(txtCheat, text, args);
    va_end (args);
}

void updateCheatsPanel(WarContext* context)
{
    WarScene* scene = context->scene;
    assert(scene);

    WarInput* input = &context->input;
    WarCheatStatus* cheatStatus = &scene->cheatStatus;

    WarEntity* cheatPanel = findUIEntity(context, "panelCheat");
    assert(cheatPanel);

    WarEntity* cheatCursor = findUIEntity(context, "cursorCheat");
    assert(cheatCursor);

    WarEntity* cheatText = findUIEntity(context, "txtCheat");
    assert(cheatText);

    if (cheatStatus->enabled)
    {
        if (wasKeyPressed(input, WAR_KEY_ESC) ||
            wasKeyPressed(input, WAR_KEY_ENTER))
        {
            if (wasKeyPressed(input, WAR_KEY_ENTER))
            {
                applyCheat(context, cheatStatus->text);
            }

            memset(cheatStatus->text, 0, sizeof(cheatStatus->text));
            cheatStatus->position = 0;
            cheatStatus->enabled = false;

            return;
        }

        if (wasKeyPressed(input, WAR_KEY_TAB))
        {
            s32 length = strlen(cheatStatus->text);
            if (TAB_WIDTH <= STATUS_TEXT_MAX_LENGTH - length)
            {
                strInsertAt(cheatStatus->text, cheatStatus->position, '\t');
                cheatStatus->position++;
            }
        }
        else if (wasKeyPressed(input, WAR_KEY_BACKSPACE))
        {
            if (cheatStatus->position > 0)
            {
                strRemoveAt(cheatStatus->text, cheatStatus->position - 1);
                cheatStatus->position--;
            }
        }
        else if (wasKeyPressed(input, WAR_KEY_DELETE))
        {
            s32 length = strlen(cheatStatus->text);
            if (cheatStatus->position < length)
            {
                strRemoveAt(cheatStatus->text, cheatStatus->position);
            }
        }
        else if (wasKeyPressed(input, WAR_KEY_RIGHT))
        {
            s32 length = strlen(cheatStatus->text);
            if (cheatStatus->position < length)
            {
                cheatStatus->position++;
            }
        }
        else if (wasKeyPressed(input, WAR_KEY_LEFT))
        {
            if (cheatStatus->position > 0)
            {
                cheatStatus->position--;
            }
        }
        else if (wasKeyPressed(input, WAR_KEY_HOME))
        {
            cheatStatus->position = 0;
        }
        else if (wasKeyPressed(input, WAR_KEY_END))
        {
            s32 length = strlen(cheatStatus->text);
            cheatStatus->position = length;
        }

        char statusText[STATUS_TEXT_MAX_LENGTH];
        memset(statusText, 0, sizeof(statusText));
        strcpy(statusText, "MSG: ");
        strcpy(statusText + strlen("MSG: "), cheatStatus->text);
        setCheatText(context, statusText);

        NVGfontParams params;
        params.fontSize = cheatText->text.fontSize;
        params.fontData = fontsData[cheatText->text.fontIndex];

        vec2 prefixSize = nvgMeasureSingleSpriteText("MSG: ", strlen("MSG: "), params);
        vec2 textSize = nvgMeasureSingleSpriteText(cheatStatus->text, cheatStatus->position, params);
        cheatCursor->transform.position.x = prefixSize.x + textSize.x;

        setUIEntityStatus(cheatPanel, true);
        setUIEntityStatus(cheatCursor, true);
        setUIEntityStatus(cheatText, true);
    }
    else
    {
        setUIEntityStatus(cheatPanel, false);
        setUIEntityStatus(cheatCursor, false);
        setUIEntityStatus(cheatText, false);

        if (wasKeyPressed(input, WAR_KEY_ENTER))
        {
            memset(cheatStatus->text, 0, sizeof(cheatStatus->text));
            cheatStatus->position = 0;
            cheatStatus->enabled = true;
        }
    }
}
