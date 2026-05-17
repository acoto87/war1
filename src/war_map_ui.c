#include "war_map_ui.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "shl/wstr.h"

#include "war_campaigns.h"
#include "war_entities.h"
#include "war_imui.h"
#include "war_map.h"
#include "war_map_menu.h"
#include "war_ui.h"
#include "war_units.h"

void wmui_createMapUI(WarContext* context)
{
    WarMap* map = context->map;
    vec2 minimapPanel = RECT_TOP_LEFT(map->minimapPanel);

    WarCheatStatus* cheatStatus = &map->cheatStatus;
    cheatStatus->enabled = true;
    cheatStatus->visible = false;
    cheatStatus->position = 0;
    wstr_clear(&cheatStatus->text);

    wmui_createUIMinimap(context, wstr_fromCString("minimap"), minimapPanel);
}

WarEntity* wmui_createUIMinimap(WarContext* context, String name, vec2 position)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_MINIMAP, true);
    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = position,
    ));
    we_addUIComponent(context, entity, name);

    return entity;
}

void wmui_setStatus(WarContext* context, s32 highlightIndex, s32 highlightCount, s32 gold, s32 wood, StringView text)
{
    WarMap* map = context->map;

    size_t copyLen = text.length < (sizeof(map->hudStatusText) - 1)
        ? text.length : (sizeof(map->hudStatusText) - 1);

    if (text.data && copyLen > 0)
    {
        memcpy(map->hudStatusText, text.data, copyLen);
    }

    map->hudStatusText[copyLen]      = '\0';
    map->hudStatusHighlightIndex     = highlightIndex;
    map->hudStatusHighlightCount     = highlightCount;
    map->hudStatusGold               = gold;
    map->hudStatusWood               = wood;
}

void wmui_setFlashStatus(WarContext* context, f32 duration, String text)
{
    WarMap* map = context->map;
    WarFlashStatus* flashStatus = &map->flashStatus;

    assert(duration >= 0);
    assert(text.data);

    flashStatus->enabled = true;
    flashStatus->startTime = context->time;
    flashStatus->duration = duration;
    wstr_free(flashStatus->text);
    flashStatus->text = text;
}

static const char* getSpeedStr(WarMapSpeed value)
{
    switch (value)
    {
        case WAR_SPEED_SLOWEST: return "Slowest";
        case WAR_SPEED_SLOW:    return "Slow";
        case WAR_SPEED_NORMAL:  return "Normal";
        case WAR_SPEED_FASTER:  return "Faster";
        case WAR_SPEED_FASTEST: return "Fastest";
        default:                return "";
    }
}

static void wmui_renderMenus(WarContext* context)
{
    TracyCZoneN(ctx, "RenderMenus", 1);

    WarMap* map = context->map;

    if (map->menuState == WAR_MENU_STATE_NONE)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    WarPlayerInfo* player = &map->players[0];

    WarSpriteResourceRef mediumNormalRef  = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);
    WarSpriteResourceRef smallNormalRef   = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef  = imageResourceRef(242);
    WarSpriteResourceRef leftArrowNormal  = imageResourceRef(244);
    WarSpriteResourceRef leftArrowPressed = imageResourceRef(245);
    WarSpriteResourceRef rightArrowNormal = imageResourceRef(246);
    WarSpriteResourceRef rightArrowPressed = imageResourceRef(247);

    vec2 mp   = RECT_TOP_LEFT(map->menuPanel);
    vec2 msgp = RECT_TOP_LEFT(map->messagePanel);

    char buf[256];

    // Semi-transparent backdrop over the whole window
    imui_rect(context, "menuBackdrop",
        CREATE_UI_RECT_ARGS_INIT(
            .position = VEC2_ZERO,
            .size     = vec2i(context->windowWidth, context->windowHeight),
            .color    = WAR_COLOR_RGBA(0, 0, 0, 150),
        ));

    switch (map->menuState)
    {
        case WAR_MENU_STATE_MAIN:
        {
            imui_image(context, "imgMenuBackground",
                CREATE_UI_IMAGE_ARGS_INIT(
                    .spriteRef = imageResourceRefFromPlayer(player, 233, 234),
                    .position  = mp,
                ));

            imui_text(context, "txtMenuHeader",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(mp, vec2i(0, 10)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(map->menuPanel.width, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString("Warcraft")
                ));

            // Save / Load not yet implemented — no handler
            imui_text_button(context, "btnMenuSave",
                CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                    .fontIndex            = 1,
                    .text                 = wsv_fromCString("Save"),
                    .backgroundNormalRef  = smallNormalRef,
                    .backgroundPressedRef = smallPressedRef,
                    .position             = vec2_addv(mp, vec2i(20, 25)),
                    .hotKey               = WAR_KEY_S,
                    .highlightIndex       = 0,
                    .highlightCount       = 1,
                ));

            imui_text_button(context, "btnMenuLoad",
                CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                    .fontIndex            = 1,
                    .text                 = wsv_fromCString("Load"),
                    .backgroundNormalRef  = smallNormalRef,
                    .backgroundPressedRef = smallPressedRef,
                    .position             = vec2_addv(mp, vec2i(78, 25)),
                    .hotKey               = WAR_KEY_L,
                    .highlightIndex       = 0,
                    .highlightCount       = 1,
                ));

            if (imui_text_button(context, "btnMenuOptions",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Options"),
                        .backgroundNormalRef  = mediumNormalRef,
                        .backgroundPressedRef = mediumPressedRef,
                        .position             = vec2_addv(mp, vec2i(20, 45)),
                        .hotKey               = WAR_KEY_O,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleOptions(context, NULL);

            if (imui_text_button(context, "btnMenuObjectives",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Objectives"),
                        .backgroundNormalRef  = mediumNormalRef,
                        .backgroundPressedRef = mediumPressedRef,
                        .position             = vec2_addv(mp, vec2i(20, 65)),
                        .hotKey               = WAR_KEY_J,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleObjectives(context, NULL);

            if (imui_text_button(context, "btnMenuRestart",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Restart scenario"),
                        .backgroundNormalRef  = mediumNormalRef,
                        .backgroundPressedRef = mediumPressedRef,
                        .position             = vec2_addv(mp, vec2i(20, 85)),
                        .hotKey               = WAR_KEY_R,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleRestart(context, NULL);

            if (imui_text_button(context, "btnMenuContinue",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Continue"),
                        .backgroundNormalRef  = smallNormalRef,
                        .backgroundPressedRef = smallPressedRef,
                        .position             = vec2_addv(mp, vec2i(20, 105)),
                        .hotKey               = WAR_KEY_C,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleContinue(context, NULL);

            if (imui_text_button(context, "btnMenuQuit",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Quit"),
                        .backgroundNormalRef  = smallNormalRef,
                        .backgroundPressedRef = smallPressedRef,
                        .position             = vec2_addv(mp, vec2i(78, 105)),
                        .hotKey               = WAR_KEY_Q,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleQuit(context, NULL);

            break;
        }

        case WAR_MENU_STATE_OPTIONS:
        {
            imui_image(context, "imgMenuBackground",
                CREATE_UI_IMAGE_ARGS_INIT(
                    .spriteRef = imageResourceRefFromPlayer(player, 233, 234),
                    .position  = mp,
                ));

            imui_text(context, "txtOptionsHeader",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(mp, vec2i(0, 10)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(map->menuPanel.width, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString("Options")
                ));

            // Labels (right-aligned in 75px)
            static const char* labelNames[5] = { "Game Speed", "Music Vol", "SFX Vol", "Mouse Scroll", "Key Scroll" };
            static const s32   labelOffY[5]  = { 25, 42, 59, 76, 93 };

            for (s32 i = 0; i < 5; i++)
            {
                snprintf(buf, sizeof(buf), "txtOptionsLabel%d", i);
                imui_text(context, buf,
                    CREATE_UI_TEXT_ARGS_INIT(
                        .position        = vec2_addv(mp, vec2i(0, labelOffY[i])),
                        .fontIndex       = 1,
                        .boundings       = vec2f(75, 12),
                        .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
                        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                        .text = wsv_fromCString(labelNames[i])
                    ));
            }

            // Value texts (centred in 42px)
            snprintf(buf, sizeof(buf), "%s", getSpeedStr(map->settings2.gameSpeed));
            imui_text(context, "txtOptionsGameSpeedValue",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(mp, vec2i(92, 25)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(42, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString(buf)
                ));

            snprintf(buf, sizeof(buf), "%d", map->settings2.musicVol);
            imui_text(context, "txtOptionsMusicVolValue",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(mp, vec2i(92, 42)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(42, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString(buf)
                ));

            snprintf(buf, sizeof(buf), "%d", map->settings2.sfxVol);
            imui_text(context, "txtOptionsSFXVolValue",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(mp, vec2i(92, 59)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(42, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString(buf)
                ));

            snprintf(buf, sizeof(buf), "%s", getSpeedStr(map->settings2.mouseScrollSpeed));
            imui_text(context, "txtOptionsMouseScrollValue",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(mp, vec2i(92, 76)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(42, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString(buf)
                ));

            snprintf(buf, sizeof(buf), "%s", getSpeedStr(map->settings2.keyScrollSpeed));
            imui_text(context, "txtOptionsKeyScrollValue",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(mp, vec2i(92, 93)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(42, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString(buf)
                ));

            // Arrow buttons — (dec at x+76, inc at x+133), y offsets: 22,39,56,73,90
            if (imui_image_button(context, "btnOptionsGameSpeedDec",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = leftArrowNormal,
                        .backgroundPressedRef = leftArrowPressed,
                        .position             = vec2_addv(mp, vec2i(76, 22)),
                    )))
                wmm_handleGameSpeedDec(context, NULL);

            if (imui_image_button(context, "btnOptionsGameSpeedInc",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = rightArrowNormal,
                        .backgroundPressedRef = rightArrowPressed,
                        .position             = vec2_addv(mp, vec2i(133, 22)),
                    )))
                wmm_handleGameSpeedInc(context, NULL);

            if (imui_image_button(context, "btnOptionsMusicVolDec",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = leftArrowNormal,
                        .backgroundPressedRef = leftArrowPressed,
                        .position             = vec2_addv(mp, vec2i(76, 39)),
                    )))
                wmm_handleMusicVolDec(context, NULL);

            if (imui_image_button(context, "btnOptionsMusicVolInc",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = rightArrowNormal,
                        .backgroundPressedRef = rightArrowPressed,
                        .position             = vec2_addv(mp, vec2i(133, 39)),
                    )))
                wmm_handleMusicVolInc(context, NULL);

            if (imui_image_button(context, "btnOptionsSFXVolDec",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = leftArrowNormal,
                        .backgroundPressedRef = leftArrowPressed,
                        .position             = vec2_addv(mp, vec2i(76, 56)),
                    )))
                wmm_handleSfxVolDec(context, NULL);

            if (imui_image_button(context, "btnOptionsSFXVolInc",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = rightArrowNormal,
                        .backgroundPressedRef = rightArrowPressed,
                        .position             = vec2_addv(mp, vec2i(133, 56)),
                    )))
                wmm_handleSfxVolInc(context, NULL);

            if (imui_image_button(context, "btnOptionsMouseScrollDec",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = leftArrowNormal,
                        .backgroundPressedRef = leftArrowPressed,
                        .position             = vec2_addv(mp, vec2i(76, 73)),
                    )))
                wmm_handleMouseScrollSpeedDec(context, NULL);

            if (imui_image_button(context, "btnOptionsMouseScrollInc",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = rightArrowNormal,
                        .backgroundPressedRef = rightArrowPressed,
                        .position             = vec2_addv(mp, vec2i(133, 73)),
                    )))
                wmm_handleMouseScrollSpeedInc(context, NULL);

            if (imui_image_button(context, "btnOptionsKeyScrollDec",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = leftArrowNormal,
                        .backgroundPressedRef = leftArrowPressed,
                        .position             = vec2_addv(mp, vec2i(76, 90)),
                    )))
                wmm_handleKeyScrollSpeedDec(context, NULL);

            if (imui_image_button(context, "btnOptionsKeyScrollInc",
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = rightArrowNormal,
                        .backgroundPressedRef = rightArrowPressed,
                        .position             = vec2_addv(mp, vec2i(133, 90)),
                    )))
                wmm_handleKeyScrollSpeedInc(context, NULL);

            if (imui_text_button(context, "btnOptionsOk",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Ok"),
                        .backgroundNormalRef  = smallNormalRef,
                        .backgroundPressedRef = smallPressedRef,
                        .position             = vec2_addv(mp, vec2i(20, 115)),
                        .hotKey               = WAR_KEY_O,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleOptionsOk(context, NULL);

            if (imui_text_button(context, "btnOptionsCancel",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Cancel"),
                        .backgroundNormalRef  = smallNormalRef,
                        .backgroundPressedRef = smallPressedRef,
                        .position             = vec2_addv(mp, vec2i(78, 115)),
                        .hotKey               = WAR_KEY_C,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleOptionsCancel(context, NULL);

            break;
        }

        case WAR_MENU_STATE_OBJECTIVES:
        {
            WarCampaignMapData campaignData = wcamp_getCampaignData(
                wmap_getCampaignMapTypeByLevelInfoIndex(map->levelInfoIndex));

            imui_image(context, "imgMenuBackground",
                CREATE_UI_IMAGE_ARGS_INIT(
                    .spriteRef = imageResourceRefFromPlayer(player, 233, 234),
                    .position  = mp,
                ));

            imui_text(context, "txtObjectivesHeader",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(mp, vec2i(0, 10)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(map->menuPanel.width, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString("Objectives")
                ));

            imui_text(context, "txtObjectivesText",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position  = vec2_addv(mp, vec2i(10, 26)),
                    .fontIndex = 1,
                    .multiline = true,
                    .boundings = vec2f(map->menuPanel.width - 20, 75),
                    .wrapping  = WAR_TEXT_WRAP_CHAR,
                    .trimming  = WAR_TEXT_TRIM_SPACES,
                    .text = wstr_view(&campaignData.objectives)
                ));

            if (imui_text_button(context, "btnObjectivesMenu",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Menu"),
                        .backgroundNormalRef  = mediumNormalRef,
                        .backgroundPressedRef = mediumPressedRef,
                        .position             = vec2_addv(mp, vec2i(20, 105)),
                        .hotKey               = WAR_KEY_M,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleObjectivesMenu(context, NULL);

            break;
        }

        case WAR_MENU_STATE_RESTART:
        {
            imui_image(context, "imgMessageMenuBackground",
                CREATE_UI_IMAGE_ARGS_INIT(
                    .spriteRef = imageResourceRefFromPlayer(player, 235, 236),
                    .position  = msgp,
                ));

            imui_text(context, "txtRestartText",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(msgp, vec2i(0, 10)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(map->messagePanel.width, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString("Are you sure you want to restart?")
                ));

            if (imui_text_button(context, "btnRestartRestart",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Restart"),
                        .backgroundNormalRef  = smallNormalRef,
                        .backgroundPressedRef = smallPressedRef,
                        .position             = vec2_addv(msgp, vec2i(20, 25)),
                        .hotKey               = WAR_KEY_R,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleRestartRestart(context, NULL);

            if (imui_text_button(context, "btnRestartCancel",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Cancel"),
                        .backgroundNormalRef  = smallNormalRef,
                        .backgroundPressedRef = smallPressedRef,
                        .position             = vec2_addv(msgp, vec2i(210, 25)),
                        .hotKey               = WAR_KEY_C,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleRestartCancel(context, NULL);

            break;
        }

        case WAR_MENU_STATE_GAME_OVER:
        {
            imui_image(context, "imgMessageMenuBackground",
                CREATE_UI_IMAGE_ARGS_INIT(
                    .spriteRef = imageResourceRefFromPlayer(player, 235, 236),
                    .position  = msgp,
                ));

            const char* gameOverText = (map->result == WAR_LEVEL_RESULT_WIN)
                ? "You are victorious!"
                : "You failed to archieve victory...";

            imui_text(context, "txtGameOverText",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(msgp, vec2i(0, 10)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(map->messagePanel.width, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString(gameOverText)
                ));

            if (map->result == WAR_LEVEL_RESULT_WIN)
            {
                if (imui_text_button(context, "btnGameOverSave",
                        CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                            .fontIndex            = 1,
                            .text                 = wsv_fromCString("Save"),
                            .backgroundNormalRef  = smallNormalRef,
                            .backgroundPressedRef = smallPressedRef,
                            .position             = vec2_addv(msgp, vec2i(20, 25)),
                            .hotKey               = WAR_KEY_S,
                            .highlightIndex       = 0,
                            .highlightCount       = 1,
                        )))
                    wmm_handleGameOverSave(context, NULL);

                if (imui_text_button(context, "btnGameOverContinue",
                        CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                            .fontIndex            = 1,
                            .text                 = wsv_fromCString("Continue"),
                            .backgroundNormalRef  = smallNormalRef,
                            .backgroundPressedRef = smallPressedRef,
                            .position             = vec2_addv(msgp, vec2i(210, 25)),
                            .hotKey               = WAR_KEY_C,
                            .highlightIndex       = 0,
                            .highlightCount       = 1,
                        )))
                    wmm_handleGameOverContinue(context, NULL);
            }
            else
            {
                if (imui_text_button(context, "btnGameOverOk",
                        CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                            .fontIndex            = 1,
                            .text                 = wsv_fromCString("Ok"),
                            .backgroundNormalRef  = smallNormalRef,
                            .backgroundPressedRef = smallPressedRef,
                            .position             = vec2_addv(msgp, vec2i(116, 25)),
                            .hotKey               = WAR_KEY_O,
                            .highlightIndex       = 0,
                            .highlightCount       = 1,
                        )))
                    wmm_handleGameOverContinue(context, NULL);
            }

            break;
        }

        case WAR_MENU_STATE_QUIT:
        {
            imui_image(context, "imgMessageMenuBackground",
                CREATE_UI_IMAGE_ARGS_INIT(
                    .spriteRef = imageResourceRefFromPlayer(player, 235, 236),
                    .position  = msgp,
                ));

            imui_text(context, "txtQuitText",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(msgp, vec2i(0, 10)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(map->messagePanel.width, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text = wsv_fromCString("Are you sure you want to quit?")
                ));

            if (imui_text_button(context, "btnQuitQuit",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Quit"),
                        .backgroundNormalRef  = smallNormalRef,
                        .backgroundPressedRef = smallPressedRef,
                        .position             = vec2_addv(msgp, vec2i(20, 25)),
                        .hotKey               = WAR_KEY_Q,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleQuitQuit(context, NULL);

            if (imui_text_button(context, "btnQuitMenu",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Menu"),
                        .backgroundNormalRef  = smallNormalRef,
                        .backgroundPressedRef = smallPressedRef,
                        .position             = vec2_addv(msgp, vec2i(115, 25)),
                        .hotKey               = WAR_KEY_M,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleQuitMenu(context, NULL);

            if (imui_text_button(context, "btnQuitCancel",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Cancel"),
                        .backgroundNormalRef  = smallNormalRef,
                        .backgroundPressedRef = smallPressedRef,
                        .position             = vec2_addv(msgp, vec2i(210, 25)),
                        .hotKey               = WAR_KEY_C,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleQuitCancel(context, NULL);

            break;
        }

        case WAR_MENU_STATE_DEMO_END:
        {
            imui_image(context, "imgMenuBackground",
                CREATE_UI_IMAGE_ARGS_INIT(
                    .spriteRef = imageResourceRefFromPlayer(player, 233, 234),
                    .position  = mp,
                ));

            imui_text(context, "txtDemoEndHeader",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position        = vec2_addv(mp, vec2i(0, 10)),
                    .fontIndex       = 1,
                    .boundings       = vec2f(map->menuPanel.width, 12),
                    .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                    .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                    .text            = wsv_fromCString("Thanks for playing!")
                ));

            imui_text(context, "txtDemoEndText",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position  = vec2_addv(mp, vec2i(8, 26)),
                    .fontIndex = 1,
                    .fontSize  = 7,
                    .multiline = true,
                    .boundings = vec2f(map->menuPanel.width - 16, 75),
                    .wrapping  = WAR_TEXT_WRAP_CHAR,
                    .text = wsv_fromCString(
                        "This is a non-profit project with\n"
                        "the personal goal of learning to\n"
                        "do RTS engines\n"
                        "\n"
                        "This is not an official Blizzard\n"
                        "product. Warcraft and all assets\n"
                        "you see are registered trademarks\n"
                        "of Blizzard Entertainment.\n"
                        "\n"
                        "Made by @acoro87")
                ));

            if (imui_text_button(context, "btnDemoEndMenu",
                    CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                        .fontIndex            = 1,
                        .text                 = wsv_fromCString("Menu"),
                        .backgroundNormalRef  = mediumNormalRef,
                        .backgroundPressedRef = mediumPressedRef,
                        .position             = vec2_addv(mp, vec2i(20, 105)),
                        .hotKey               = WAR_KEY_M,
                        .highlightIndex       = 0,
                        .highlightCount       = 1,
                    )))
                wmm_handleDemoEndMenu(context, NULL);

            break;
        }

        default:
            break;
    }

    TracyCZoneEnd(ctx);
}

static void wmui_renderHUD(WarContext* context)
{
    TracyCZoneN(ctx, "RenderHUD", 1);

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    WarCheatStatus* cheatStatus = &map->cheatStatus;

    // Suppress imui button hotkeys while the cheat panel is open so that
    // key presses intended as cheat input don't trigger command shortcuts.
    if (cheatStatus->enabled && cheatStatus->visible)
    {
        context->imui.hotkeys_enabled = false;
    }

    vec2 topPanel        = RECT_TOP_LEFT(map->topPanel);
    vec2 bottomPanel     = RECT_TOP_LEFT(map->bottomPanel);
    vec2 leftBottomPanel = RECT_TOP_LEFT(map->leftBottomPanel);

    char buf[32];

    // --- Gold counter (top panel) ---
    snprintf(buf, sizeof(buf), "GOLD:%6d", player->gold);
    imui_text(context, "txtGold",
        CREATE_UI_TEXT_ARGS_INIT(
            .position = vec2_addv(topPanel, vec2i(135, 2)),
            .fontSize = 6,
            .text = wsv_fromCString(buf)
        ));

    // --- Wood counter (top panel) ---
    snprintf(buf, sizeof(buf), "LUMBER:%6d", player->wood);
    imui_text(context, "txtWood",
        CREATE_UI_TEXT_ARGS_INIT(
            .position = vec2_addv(topPanel, vec2i(24, 2)),
            .fontSize = 6,
            .text = wsv_fromCString(buf)
        ));

    // --- Status text (bottom panel) ---
    if (map->hudStatusText[0] != '\0')
    {
        imui_text(context, "txtStatus",
            CREATE_UI_TEXT_ARGS_INIT(
                .position       = vec2_addv(bottomPanel, vec2i(2, 5)),
                .fontSize       = 6,
                .highlightIndex = map->hudStatusHighlightIndex,
                .highlightCount = map->hudStatusHighlightCount,
                .text = wsv_fromCString(map->hudStatusText)
            ));
    }

    // --- Status gold/wood cost icons + amounts (bottom panel) ---
    if (map->hudStatusGold > 0 || map->hudStatusWood > 0)
    {
        imui_image(context, "imgStatusWood", CREATE_UI_IMAGE_ARGS_INIT(
            .spriteRef = imageResourceRef(407),
            .position  = vec2_addv(bottomPanel, vec2i(163, 3)),
        ));

        snprintf(buf, sizeof(buf), "%d", map->hudStatusWood);
        imui_text(context, "txtStatusWood",
            CREATE_UI_TEXT_ARGS_INIT(
                .position = vec2_addv(bottomPanel, vec2i(179, 5)),
                .fontSize = 6,
                .text = wsv_fromCString(buf)
            ));

        imui_image(context, "imgStatusGold", CREATE_UI_IMAGE_ARGS_INIT(
            .spriteRef = imageResourceRef(406),
            .position  = vec2_addv(bottomPanel, vec2i(200, 5)),
        ));

        snprintf(buf, sizeof(buf), "%d", map->hudStatusGold);
        imui_text(context, "txtStatusGold",
            CREATE_UI_TEXT_ARGS_INIT(
                .position = vec2_addv(bottomPanel, vec2i(218, 5)),
                .fontSize = 6,
                .text = wsv_fromCString(buf)
            ));
    }

    // --- Selection info panel (left bottom panel) ---
    //
    // Life bar thresholds and widths match the original retained-mode values.
    // Frame indices for resource 360/359:
    //   0 = single unit non-magic dude / complete building
    //   1 = single unit magic dude (shows mana bar)
    //   2 = single unit building under construction (shows build-percent bar)
    //   3-5 = multi-select background (selectedEntitiesCount+1, for selectedEntitiesCount 2-4)
    //   6-8 = multi-select life-bar section (selectedEntitiesCount+4, for selectedEntitiesCount 2-4)
    {
        const f32 hpRedThresh    = 0.35f;
        const f32 hpYellowThresh = 0.70f;
        const s32 hpBarWidthPx   = 27;
        const s32 mpBarWidthPx   = 27;
        const s32 pctBarWidthPx  = 64;

        s32 selectedEntitiesCount = MIN(map->selectedEntities.count, 4);
        if (selectedEntitiesCount == 1)
        {
            WarEntityId selectedEntityId = map->selectedEntities.items[0];
            WarEntity* selectedEntity = we_findEntity(context, selectedEntityId);
            if (selectedEntity && wu_isUnit(selectedEntity))
            {
                WarUnitComponent* unit = we_getUnitComponent(context, selectedEntity);
                assert(unit);

                // Background panel frame
                s32 infoFrame = 0;
                if (wu_isDudeUnit(context, selectedEntity) && wu_isMagicUnit(context, selectedEntity))
                {
                    infoFrame = 1;
                }
                else if (wu_isBuildingUnit(context, selectedEntity) && unit->building)
                {
                    infoFrame = 2;
                }

                imui_image_frame(context, "imgUnitInfo",
                    CREATE_UI_IMAGE_ARGS_INIT(
                        .spriteRef = imageResourceRefFromPlayer(player, 360, 359),
                        .position  = vec2_addv(leftBottomPanel, vec2i(2, 0)),
                    ),
                    infoFrame);

                // Portrait
                WarUnitData unitData = wu_getUnitData(unit->type);
                imui_image_frame(context, "imgUnitPortrait0",
                    CREATE_UI_IMAGE_ARGS_INIT(
                        .spriteRef = imageResourceRef(361),
                        .position  = vec2_addv(leftBottomPanel, vec2i(6, 4)),
                    ),
                    unitData.portraitFrameIndex);

                // Unit name
                imui_text(context, "txtUnitName",
                    CREATE_UI_TEXT_ARGS_INIT(
                        .position       = vec2_addv(leftBottomPanel, vec2i(6, 26)),
                        .fontSize       = 6,
                        .highlightIndex = NO_HIGHLIGHT,
                        .text = unitData.name
                    ));

                // Life bar
                {
                    f32 hpPct = PERCENTF01(unit->hp, unit->maxhp);

                    WarColor barColor = WAR_COLOR_GREEN;
                    if (hpPct <= hpRedThresh)
                        barColor = WAR_COLOR_RED;
                    else if (hpPct <= hpYellowThresh)
                        barColor = WAR_COLOR_YELLOW;

                    imui_rect(context, "rectLifeBar0",
                        CREATE_UI_RECT_ARGS_INIT(
                            .position = vec2_addv(leftBottomPanel, vec2i(37, 20)),
                            .size     = vec2i((s32)(hpPct * hpBarWidthPx), 3),
                            .color    = barColor,
                        ));
                }

                // Mana bar (magic dudes only)
                if (wu_isDudeUnit(context, selectedEntity) && wu_isMagicUnit(context, selectedEntity))
                {
                    f32 mpPct = PERCENTF01(unit->mana, unit->maxMana);
                    imui_rect(context, "rectMagicBar",
                        CREATE_UI_RECT_ARGS_INIT(
                            .position = vec2_addv(leftBottomPanel, vec2i(37, 9)),
                            .size     = vec2i((s32)(mpPct * mpBarWidthPx), 3),
                            .color    = WAR_COLOR_GREEN,
                        ));
                }

                // Build-percent bar (buildings under construction only)
                if (wu_isBuildingUnit(context, selectedEntity) && unit->building)
                {
                    f32 pct = unit->buildPercent;
                    imui_rect(context, "rectPercentBar",
                        CREATE_UI_RECT_ARGS_INIT(
                            .position = vec2_addv(leftBottomPanel, vec2i(4, 37)),
                            .size     = vec2i((s32)(pct * pctBarWidthPx), 5),
                            .color    = WAR_COLOR_GREEN,
                        ));
                    imui_image_frame(context, "rectPercentText",
                        CREATE_UI_IMAGE_ARGS_INIT(
                            .spriteRef = imageResourceRef(410),
                            .position  = vec2_addv(leftBottomPanel, vec2i(15, 37)),
                        ),
                        0);
                }
            }
        }
        else if (selectedEntitiesCount > 1)
        {
            // Portrait and life-bar slot positions for multi-select (indices 1-4)
            const s32 portraitOffX[5] = { 0, 4, 38,  4, 38 };
            const s32 portraitOffY[5] = { 0, 1,  1, 23, 23 };
            const s32 lifeBarOffX[5]  = { 0, 4, 38,  4, 38 };
            const s32 lifeBarOffY[5]  = { 0, 17, 17, 39, 39 };

            // Background panels (frame 3/4/5 for 2/3/4 units)
            imui_image_frame(context, "imgUnitInfo",
                CREATE_UI_IMAGE_ARGS_INIT(
                    .spriteRef = imageResourceRefFromPlayer(player, 360, 359),
                    .position  = vec2_addv(leftBottomPanel, vec2i(2, 0)),
                ),
                selectedEntitiesCount + 1);

            imui_image_frame(context, "imgUnitInfoLife",
                CREATE_UI_IMAGE_ARGS_INIT(
                    .spriteRef = imageResourceRefFromPlayer(player, 360, 359),
                    .position  = vec2_addv(leftBottomPanel, vec2i(3, 16)),
                ),
                selectedEntitiesCount + 4);

            for (s32 i = 1; i <= selectedEntitiesCount; i++)
            {
                WarEntityId selectedEntityId = map->selectedEntities.items[i - 1];
                WarEntity* selectedEntity = we_findEntity(context, selectedEntityId);
                if (selectedEntity && wu_isUnit(selectedEntity))
                {
                    WarUnitComponent* unit = we_getUnitComponent(context, selectedEntity);
                    assert(unit);

                    WarUnitData unitData = wu_getUnitData(unit->type);

                    // Portrait
                    char portraitId[32];
                    snprintf(portraitId, sizeof(portraitId), "imgUnitPortrait%d", i);
                    imui_image_frame(context, portraitId,
                        CREATE_UI_IMAGE_ARGS_INIT(
                            .spriteRef = imageResourceRef(361),
                            .position  = vec2_addv(leftBottomPanel, vec2i(portraitOffX[i], portraitOffY[i])),
                        ),
                        unitData.portraitFrameIndex);

                    // Life bar
                    f32 hpPct = PERCENTF01(unit->hp, unit->maxhp);

                    WarColor barColor = WAR_COLOR_GREEN;
                    if (hpPct <= hpRedThresh)
                        barColor = WAR_COLOR_RED;
                    else if (hpPct <= hpYellowThresh)
                        barColor = WAR_COLOR_YELLOW;

                    char lifeBarId[32];
                    snprintf(lifeBarId, sizeof(lifeBarId), "rectLifeBar%d", i);
                    imui_rect(context, lifeBarId,
                        CREATE_UI_RECT_ARGS_INIT(
                            .position = vec2_addv(leftBottomPanel, vec2i(lifeBarOffX[i], lifeBarOffY[i])),
                            .size     = vec2i((s32)(hpPct * hpBarWidthPx), 3),
                            .color    = barColor,
                        ));
                }
            }
        }
    }

    // --- Command panel (only when no menu is open) ---
    if (map->menuState == WAR_MENU_STATE_NONE)
    {
        WarSpriteResourceRef normalRef    = imageResourceRef(364);
        WarSpriteResourceRef pressedRef   = imageResourceRef(365);
        WarSpriteResourceRef portraitsRef = imageResourceRef(361);

        static const s32 cmdOffX[6] = {  2, 36,  2, 36,  2, 36 };
        static const s32 cmdOffY[6] = { 44, 44, 67, 67, 90, 90 };

        for (s32 i = 0; i < 6; i++)
        {
            if (!map->commandSlotActive[i])
                continue;

            char btnId[16];
            snprintf(btnId, sizeof(btnId), "btnCommand%d", i);

            if (imui_image_button(context, btnId,
                    CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                        .backgroundNormalRef  = normalRef,
                        .backgroundPressedRef = pressedRef,
                        .foregroundRef        = portraitsRef,
                        .foregroundFrameIndex = map->commandSlots[i].frameIndex,
                        .position             = vec2_addv(leftBottomPanel, vec2i(cmdOffX[i], cmdOffY[i])),
                        .hotKey               = map->commandSlots[i].hotKey,
                    )))
            {
                if (map->commandSlots[i].clickHandler)
                    map->commandSlots[i].clickHandler(context, NULL);
            }
        }

        // Command info text rows (farm / goldmine info)
        static const s32 txtOffX[4] = { 3,  3,  7, 11 };
        static const s32 txtOffY[4] = { 46, 56, 64, 54 };

        for (s32 i = 0; i < 4; i++)
        {
            if (!map->commandTextVisible[i])
                continue;

            char txtId[16];
            snprintf(txtId, sizeof(txtId), "txtCommand%d", i);

            imui_text(context, txtId,
                CREATE_UI_TEXT_ARGS_INIT(
                    .position       = vec2_addv(leftBottomPanel, vec2i(txtOffX[i], txtOffY[i])),
                    .fontSize       = 6,
                    .highlightIndex = map->commandTextHighlightIndex[i],
                    .highlightCount = map->commandTextHighlightCount[i],
                    .text = wsv_fromCString(map->commandTexts[i])
                ));
        }

        // Menu button
        if (imui_image_button(context, "btnMenu",
                CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                    .backgroundNormalRef   = imageResourceRef(362),
                    .backgroundPressedRef  = imageResourceRef(363),
                    .position              = vec2_addv(leftBottomPanel, vec2i(3, 116)),
                    .hotKey                = WAR_KEY_F10,
                    .tooltip               = wsv_fromCString("MENU (F10)"),
                    .tooltipHighlightIndex = 6,
                    .tooltipHighlightCount = 3,
                )))
        {
            wmm_handleMenu(context, NULL);
        }
    }

    // --- Cheat input cursor (visible while cheat panel is open) ---
    // Drawn as a 1×7 white rect at the current text insertion position.
    if (cheatStatus->enabled && cheatStatus->visible && cheatStatus->cursorX >= 0.0f)
    {
        imui_rect(context, "rectStatusCursor", CREATE_UI_RECT_ARGS_INIT(
            .position = vec2f(bottomPanel.x + cheatStatus->cursorX, bottomPanel.y + 4.0f),
            .size     = vec2i(1, 7),
            .color    = WAR_COLOR_WHITE,
        ));
    }

    // --- Cheat feedback text (shown for a few seconds after applying a cheat) ---
    if (cheatStatus->enabled && cheatStatus->feedback && cheatStatus->feedbackText.data)
    {
        imui_text(context, "txtCheatFeedback",
            CREATE_UI_TEXT_ARGS_INIT(
                .position  = vec2_addv(bottomPanel, vec2i(15, -20)),
                .fontIndex = 1,
                .fontSize  = 8,
                .fontColor = WAR_COLOR_YELLOW,
                .text = wstr_view(&cheatStatus->feedbackText)
            ));
    }

    TracyCZoneEnd(ctx);
}

void wmui_renderSelectionRect(WarContext* context)
{
    TracyCZoneN(ctx, "RenderSelectionRect", 1);

    wr_save(context);

    WarInput* input = &context->input;
    if (isMapDragging(input))
    {
        rect pointerRect = rectpf(input->mapDragStartPos.x, input->mapDragStartPos.y, input->pos.x, input->pos.y);
        wr_strokeRect(context, pointerRect, WAR_COLOR_GREEN_SELECTION, 1.0f);
    }

    wr_restore(context);

    TracyCZoneEnd(ctx);
}

void wmui_renderCommand(WarContext* context)
{
    TracyCZoneN(ctx, "RenderCommand", 1);

    WarMap* map = context->map;
    WarUnitCommand* command = &map->command;

    WarInput* input = &context->input;

    wr_save(context);

    switch (command->type)
    {
        case WAR_COMMAND_BUILD_FARM_HUMANS:
        case WAR_COMMAND_BUILD_FARM_ORCS:
        case WAR_COMMAND_BUILD_BARRACKS_HUMANS:
        case WAR_COMMAND_BUILD_BARRACKS_ORCS:
        case WAR_COMMAND_BUILD_CHURCH:
        case WAR_COMMAND_BUILD_TEMPLE:
        case WAR_COMMAND_BUILD_TOWER_HUMANS:
        case WAR_COMMAND_BUILD_TOWER_ORCS:
        case WAR_COMMAND_BUILD_TOWNHALL_HUMANS:
        case WAR_COMMAND_BUILD_TOWNHALL_ORCS:
        case WAR_COMMAND_BUILD_LUMBERMILL_HUMANS:
        case WAR_COMMAND_BUILD_LUMBERMILL_ORCS:
        case WAR_COMMAND_BUILD_STABLE:
        case WAR_COMMAND_BUILD_KENNEL:
        case WAR_COMMAND_BUILD_BLACKSMITH_HUMANS:
        case WAR_COMMAND_BUILD_BLACKSMITH_ORCS:
        {
            vec2 position = wmap_screenToMapCoordinatesV(context, input->pos);
            position = wmap_mapToTileCoordinatesV(position);

            WarUnitType buildingToBuild = command->build.buildingToBuild;
            WarUnitData data = wu_getUnitData(buildingToBuild);

            WarColor fillColor = we_checkRectToBuild(context, (s32)position.x, (s32)position.y, data.sizex, data.sizey)
                ? WAR_COLOR_GRAY_TRANSPARENT : WAR_COLOR_RED_TRANSPARENT;

            position = wmap_tileToMapCoordinatesV(position, false);
            position = wmap_mapToScreenCoordinatesV(context, position);
            vec2 size = vec2i(data.sizex * MEGA_TILE_WIDTH, data.sizey * MEGA_TILE_HEIGHT);
            rect buildingRect = rectv(position, size);
            wr_fillRect(context, buildingRect, fillColor);

            break;
        }

        case WAR_COMMAND_BUILD_WALL:
        case WAR_COMMAND_BUILD_ROAD:
        {
            vec2 position = wmap_screenToMapCoordinatesV(context, input->pos);
            position = wmap_mapToTileCoordinatesV(position);

            WarColor fillColor = we_checkRectToBuild(context, (s32)position.x, (s32)position.y, 1, 1)
                ? WAR_COLOR_GRAY_TRANSPARENT : WAR_COLOR_RED_TRANSPARENT;

            position = wmap_tileToMapCoordinatesV(position, false);
            position = wmap_mapToScreenCoordinatesV(context, position);
            vec2 size = vec2i(MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect buildingRect = rectv(position, size);
            wr_fillRect(context, buildingRect, fillColor);

            break;
        }

        default:
        {
            // don't render the rest of the commands
            break;
        }
    }

    wr_restore(context);

    TracyCZoneEnd(ctx);
}

void wmui_renderMapUI(WarContext* context)
{
    TracyCZoneN(ctx, "RenderMapUI", 1);

    wr_save(context);

    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    vec2 leftTopPanel    = RECT_TOP_LEFT(map->leftTopPanel);
    vec2 leftBottomPanel = RECT_TOP_LEFT(map->leftBottomPanel);
    vec2 topPanel        = RECT_TOP_LEFT(map->topPanel);
    vec2 rightPanel      = RECT_TOP_LEFT(map->rightPanel);
    vec2 bottomPanel     = RECT_TOP_LEFT(map->bottomPanel);

    imui_image(context, "panelLeftTop", CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRefFromPlayer(player, 224, 225),
        .position  = leftTopPanel,
    ));
    imui_image(context, "panelLeftBottom", CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRefFromPlayer(player, 226, 227),
        .position  = leftBottomPanel,
    ));
    imui_image(context, "panelTop", CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRefFromPlayer(player, 218, 219),
        .position  = topPanel,
    ));
    imui_image(context, "panelRight", CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRefFromPlayer(player, 220, 221),
        .position  = rightPanel,
    ));
    imui_image(context, "panelBottom", CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRefFromPlayer(player, 222, 223),
        .position  = bottomPanel,
    ));
    imui_image(context, "imgGold", CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRef(406),
        .position  = vec2_addv(topPanel, vec2i(201, 1)),
    ));
    imui_image(context, "imgLumber", CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRef(407),
        .position  = vec2_addv(topPanel, vec2i(102, 0)),
    ));

    wmui_renderSelectionRect(context);
    wmui_renderCommand(context);
    wui_renderUIEntities(context);
    wmui_renderHUD(context);
    wmui_renderMenus(context);

    wr_restore(context);

    TracyCZoneEnd(ctx);
}
