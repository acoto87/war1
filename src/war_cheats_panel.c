#include "war_cheats.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "SDL3/SDL.h"
#include "shl/wstr.h"

#include "war_font.h"
#include "war_imui.h"

void wcheatp_setCheatsPanelVisible(WarContext* context, bool visible)
{
    WarScene* scene = context->scene;
    WarMap* map = context->map;
    assert(scene || map);

    WarCheatStatus* cheatStatus = scene
        ? &scene->cheatStatus : &map->status.cheatStatus;

    wstr_clear(&cheatStatus->text);
    cheatStatus->position = 0;
    cheatStatus->cursorX  = -1.0f;
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

void wcheatp_setCheatsFeedback(WarContext* context, String feedbackText)
{
    WarScene* scene = context->scene;
    WarMap* map = context->map;
    assert(scene || map);

    WarCheatStatus* cheatStatus = scene
        ? &scene->cheatStatus : &map->status.cheatStatus;

    if (cheatStatus->feedbackText.data)
    {
        wstr_free(cheatStatus->feedbackText);
        cheatStatus->feedbackText = wstr_make();
    }

    if (feedbackText.data)
    {
        cheatStatus->feedback = true;
        cheatStatus->feedbackEndRealTime = context->realTime + 3.0;
        cheatStatus->feedbackText = feedbackText;
    }
    else
    {
        cheatStatus->feedback = false;
        cheatStatus->feedbackEndRealTime = 0.0;
    }
}

void wcheatp_createCheatsPanel(WarContext* context)
{
    WarScene* scene = context->scene;
    assert(scene);

    WarCheatStatus* cheatStatus = &scene->cheatStatus;
    cheatStatus->enabled = true;
    cheatStatus->visible = false;
    cheatStatus->position = 0;
    cheatStatus->cursorX  = -1.0f;
    wstr_clear(&cheatStatus->text);
    // NOTE: No retained entities are created; the panel is rendered each
    // frame by wcheatp_renderCheatsPanel() called from wsc_renderScene().
}

void wcheatp_setCheatText(WarContext* context, String text)
{
    NOT_USED(context);
    // Text is now derived directly from cheatStatus->text in wcheatp_renderCheatsPanel.
    // Free the String to avoid leaks if callers still pass one.
    wstr_free(text);
}

void wcheatp_updateCheatsPanel(WarContext* context)
{
    WarScene* scene = context->scene;
    assert(scene);

    WarInput* input = &context->input;
    WarCheatStatus* cheatStatus = &scene->cheatStatus;

    if (!cheatStatus->enabled)
        return;

    // Tick feedback timer.
    if (cheatStatus->feedback)
    {
        if (context->realTime >= cheatStatus->feedbackEndRealTime)
        {
            cheatStatus->feedback = false;
        }
    }

    if (cheatStatus->visible)
    {
        if (isKeyJustReleased(input, WAR_KEY_ESC) ||
            isKeyJustReleased(input, WAR_KEY_ENTER))
        {
            if (isKeyJustReleased(input, WAR_KEY_ENTER))
            {
                wcheat_applyCheat(context, wstr_view(&cheatStatus->text));
            }

            wcheatp_setCheatsPanelVisible(context, false);
            return;
        }

        if (isKeyJustReleased(input, WAR_KEY_TAB))
        {
            s32 length = (s32)cheatStatus->text.length;
            if (TAB_WIDTH <= STATUS_TEXT_MAX_LENGTH - length)
            {
                wstr_insert(&cheatStatus->text, cheatStatus->position, wsv_fromCString("\t"));
                cheatStatus->position++;
            }
        }
        else if (isKeyJustReleased(input, WAR_KEY_BACKSPACE))
        {
            if (cheatStatus->position > 0)
            {
                wstr_removeRange(&cheatStatus->text, cheatStatus->position - 1, 1);
                cheatStatus->position--;
            }
        }
        else if (isKeyJustReleased(input, WAR_KEY_DELETE))
        {
            s32 length = (s32)cheatStatus->text.length;
            if (cheatStatus->position < length)
            {
                wstr_removeRange(&cheatStatus->text, cheatStatus->position, 1);
            }
        }
        else if (isKeyJustReleased(input, WAR_KEY_RIGHT))
        {
            s32 length = (s32)cheatStatus->text.length;
            if (cheatStatus->position < length)
            {
                cheatStatus->position++;
            }
        }
        else if (isKeyJustReleased(input, WAR_KEY_LEFT))
        {
            if (cheatStatus->position > 0)
            {
                cheatStatus->position--;
            }
        }
        else if (isKeyJustReleased(input, WAR_KEY_HOME))
        {
            cheatStatus->position = 0;
        }
        else if (isKeyJustReleased(input, WAR_KEY_END))
        {
            s32 length = (s32)cheatStatus->text.length;
            cheatStatus->position = length;
        }

        // Compute cursor X for the render step.
        StringView prefix = wsv_fromCString("MSG: ");
        StringView cheatStatusText = wstr_view(&cheatStatus->text);

        WarFontParams params = {0};
        params.fontSize = 6.0f;
        params.fontData = getFontData(0);

        vec2 prefixSize = wfont_measureSingleSpriteText(prefix, (s32)wsv_length(prefix), params);
        vec2 textSize   = wfont_measureSingleSpriteText(cheatStatusText, cheatStatus->position, params);
        cheatStatus->cursorX = 2.0f + prefixSize.x + textSize.x;
    }
    else
    {
        cheatStatus->cursorX = -1.0f;

        if (isKeyJustReleased(input, WAR_KEY_ENTER))
        {
            wcheatp_setCheatsPanelVisible(context, true);
        }
    }
}

void wcheatp_renderCheatsPanel(WarContext* context)
{
    WarScene* scene = context->scene;
    assert(scene);

    WarCheatStatus* cheatStatus = &scene->cheatStatus;

    if (!cheatStatus->enabled)
        return;

    // --- Cheat panel background + input line ---
    if (cheatStatus->visible)
    {
        // Gray translucent background bar at the top of the screen.
        imui_rect(context, "panelCheat", CREATE_UI_RECT_ARGS_INIT(
            .position = VEC2_ZERO,
            .size     = vec2f((f32)context->originalWindowWidth, 12.0f),
            .color    = WAR_COLOR_RGBA(100, 100, 100, 160),
        ));

        // "MSG: <typed text>" — built into a local buffer to avoid a heap alloc.
        char inputBuf[8 + CHEAT_TEXT_MAX_LENGTH];
        StringView cheatText = wstr_view(&cheatStatus->text);
        snprintf(inputBuf, sizeof(inputBuf), "MSG: %.*s", (int)cheatText.length, cheatText.data ? cheatText.data : "");

        imui_text(context, "txtCheat",
            CREATE_UI_TEXT_ARGS_INIT(
                .position = vec2i(2, 4),
                .fontSize = 6,
                .text = wsv_fromCString(inputBuf)
            ));

        // Blinking cursor rect — position computed in wcheatp_updateCheatsPanel.
        if (cheatStatus->cursorX >= 0.0f)
        {
            imui_rect(context, "cursorCheat", CREATE_UI_RECT_ARGS_INIT(
                .position = vec2f(cheatStatus->cursorX, 3.0f),
                .size     = vec2i(1, 7),
                .color    = WAR_COLOR_WHITE,
            ));
        }
    }

    // --- Cheat feedback text (shown for a few seconds after a cheat is applied) ---
    if (cheatStatus->feedback && cheatStatus->feedbackText.data)
    {
        imui_text(context, "txtCheatFeedback",
            CREATE_UI_TEXT_ARGS_INIT(
                .position  = vec2i(10, 20),
                .fontIndex = 1,
                .fontSize  = 8,
                .fontColor = WAR_COLOR_YELLOW,
                .text = wstr_view(&cheatStatus->feedbackText)
            ));
    }
}
