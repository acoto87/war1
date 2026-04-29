#include "war_cheats.h"

#include <assert.h>
#include <stdarg.h>

#include "SDL3/SDL.h"
#include "shl/wstr.h"

#include "war_font.h"
#include "war_ui.h"

void wcp_setCheatsPanelVisible(WarContext* context, bool visible)
{
    WarScene* scene = context->scene;
    WarMap* map = context->map;
    assert(scene || map);

    WarCheatStatus* cheatStatus = scene
        ? &scene->cheatStatus : &map->cheatStatus;

    wstr_clear(&cheatStatus->text);
    cheatStatus->position = 0;
    cheatStatus->visible = visible;

    if (visible)
    {
        SDL_StartTextInput(context->window);
    }
    else
    {
        SDL_StopTextInput(context->window);
    }
}

void wcp_setCheatsFeedback(WarContext* context, String feedbackText)
{
    WarScene* scene = context->scene;
    WarMap* map = context->map;
    assert(scene || map);

    WarCheatStatus* cheatStatus = scene
        ? &scene->cheatStatus : &map->cheatStatus;

    if (cheatStatus->feedbackText.data)
    {
        wstr_free(cheatStatus->feedbackText);
        cheatStatus->feedbackText = wstr_make();
    }

    if (feedbackText.data)
    {
        cheatStatus->feedback = true;
        cheatStatus->feedbackTime = 3.0f;
        cheatStatus->feedbackText = feedbackText;
    }
    else
    {
        cheatStatus->feedback = false;
        cheatStatus->feedbackTime = 0.0f;
    }
}

void wcp_createCheatsPanel(WarContext* context)
{
    WarScene* scene = context->scene;
    assert(scene);

    WarCheatStatus* cheatStatus = &scene->cheatStatus;
    cheatStatus->enabled = true;
    cheatStatus->visible = false;
    cheatStatus->position = 0;
    wstr_clear(&cheatStatus->text);

    WarEntity* uiEntity;

    vec2 cheatSize = vec2f((f32)context->originalWindowWidth, 12.0f);
    WarColor cheatBackgroundColor = WAR_COLOR_RGBA(100, 100, 100, 160);
    uiEntity = createUIRect(context, wstr_fromCString("panelCheat"), VEC2_ZERO, cheatSize, cheatBackgroundColor);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIText(context, wstr_fromCString("txtCheat"), 0, 6, wstr_make(), vec2i(2, 4));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIRect(context, wstr_fromCString("cursorCheat"), vec2i(2, 3), vec2i(1, 7), WAR_COLOR_WHITE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIText(context, wstr_fromCString("txtCheatFeedbackText"), 1, 8, wstr_make(), vec2i(10, 20));
    setUITextColor(uiEntity, WAR_COLOR_YELLOW);
    setUIEntityStatus(uiEntity, false);
}

void wcp_setCheatText(WarContext* context, String text)
{
    WarEntity* txtCheat = findUIEntity(context, wsv_fromCString("txtCheat"));
    assert(txtCheat);

    setUIText(txtCheat, text);
}

void wcp_updateCheatsPanel(WarContext* context)
{
    WarScene* scene = context->scene;
    assert(scene);

    WarInput* input = &context->input;
    WarCheatStatus* cheatStatus = &scene->cheatStatus;

    if (!cheatStatus->enabled)
        return;

    WarEntity* cheatPanel = findUIEntity(context, wsv_fromCString("panelCheat"));

    WarEntity* cheatCursor = findUIEntity(context, wsv_fromCString("cursorCheat"));
    assert(cheatCursor);

    WarEntity* cheatText = findUIEntity(context, wsv_fromCString("txtCheat"));
    assert(cheatText);

    WarEntity* cheatFeedbackText = findUIEntity(context, wsv_fromCString("txtCheatFeedbackText"));
    assert(cheatFeedbackText);

    if (cheatStatus->feedback)
    {
        setUIEntityStatus(cheatFeedbackText, true);
        setUIText(cheatFeedbackText, cheatStatus->feedbackText);

        cheatStatus->feedbackTime -= context->deltaTime;
        if (cheatStatus->feedbackTime <= 0)
        {
            cheatStatus->feedbackTime = 0;
            cheatStatus->feedback = false;
        }
    }
    else
    {
        setUIEntityStatus(cheatFeedbackText, false);
    }

    if (cheatStatus->visible)
    {
        if (wasKeyPressed(input, WAR_KEY_ESC) ||
            wasKeyPressed(input, WAR_KEY_ENTER))
        {
            if (wasKeyPressed(input, WAR_KEY_ENTER))
            {
                wcheat_applyCheat(context, wstr_view(&cheatStatus->text));
            }

            wcp_setCheatsPanelVisible(context, false);
            return;
        }

        if (wasKeyPressed(input, WAR_KEY_TAB))
        {
            s32 length = (s32)cheatStatus->text.length;
            if (TAB_WIDTH <= STATUS_TEXT_MAX_LENGTH - length)
            {
                wstr_insert(&cheatStatus->text, cheatStatus->position, wsv_fromCString("\t"));
                cheatStatus->position++;
            }
        }
        else if (wasKeyPressed(input, WAR_KEY_BACKSPACE))
        {
            if (cheatStatus->position > 0)
            {
                wstr_removeRange(&cheatStatus->text, cheatStatus->position - 1, 1);
                cheatStatus->position--;
            }
        }
        else if (wasKeyPressed(input, WAR_KEY_DELETE))
        {
            s32 length = (s32)cheatStatus->text.length;
            if (cheatStatus->position < length)
            {
                wstr_removeRange(&cheatStatus->text, cheatStatus->position, 1);
            }
        }
        else if (wasKeyPressed(input, WAR_KEY_RIGHT))
        {
            s32 length = (s32)cheatStatus->text.length;
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
            s32 length = (s32)cheatStatus->text.length;
            cheatStatus->position = length;
        }

        StringView prefix = wsv_fromCString("MSG: ");
        StringView cheatStatusText = wstr_view(&cheatStatus->text);

        String statusText = wstr_concat(prefix, cheatStatusText);
        wcp_setCheatText(context, statusText);

        WarFontParams params = {0};
        params.fontSize = cheatText->text.fontSize;
        params.fontData = getFontData(cheatText->text.fontIndex);

        vec2 prefixSize = wfont_measureSingleSpriteText(prefix, (s32)wsv_length(prefix), params);
        vec2 textSize = wfont_measureSingleSpriteText(cheatStatusText, cheatStatus->position, params);
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
            wcp_setCheatsPanelVisible(context, true);
        }
    }
}
