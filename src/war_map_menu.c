#include "war_map_menu.h"

#include "SDL3/SDL.h"
#include "shl/wstr.h"

#include "war_campaigns.h"
#include "war_game.h"
#include "war_map.h"
#include "war_resources.h"
#include "war_ui.h"

static String getSpeedStr(WarMapSpeed value)
{
    const char* str = "";
    switch (value)
    {
        case WAR_SPEED_SLOWEST: str = "Slowest";  break;
        case WAR_SPEED_SLOW:    str = "Slow";     break;
        case WAR_SPEED_NORMAL:  str = "Normal";   break;
        case WAR_SPEED_FASTER:  str = "Faster";   break;
        case WAR_SPEED_FASTEST: str = "Fastest";  break;
        default: str = ""; break;
    }
    return wstr_fromCString(str);
}

static void setUITextSpeedValueByName(WarContext* context, StringView name, WarMapSpeed value)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        WarTextComponent* text = we_getTextComponent(context, entity);
        assert(text);

        wui_setUIText(context, entity, getSpeedStr(value));

        text->highlightIndex = NO_HIGHLIGHT;
        text->highlightCount = 0;
    }
}

static void setUITextS32ValueByName(WarContext* context, StringView name, s32 value)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        WarTextComponent* text = we_getTextComponent(context, entity);
        assert(text);

        wui_setUIText(context, entity, wstr_fromCStringFormat("%d", value));

        text->highlightIndex = NO_HIGHLIGHT;
        text->highlightCount = 0;
    }
}

void wmm_createMenu(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    vec2 menuPanel = RECT_TOP_LEFT(map->menuPanel);
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = wui_createUIRect(context, wstr_fromCString("rectMenuBackdrop"), CREATE_UI_RECT_ARGS_INIT(
        .position = VEC2_ZERO,
        .size     = vec2i(context->windowWidth, context->windowHeight),
        .color    = WAR_COLOR_RGBA(0, 0, 0, 150),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImage(context, wstr_fromCString("imgMenuBackground"), CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRefFromPlayer(player, 233, 234),
        .position  = menuPanel,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtMenuHeader"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(0, 10)),
        .fontIndex       = 1,
        .boundings       = vec2f(map->menuPanel.width, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Warcraft"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnMenuSave"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Save"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(20, 25)),
        .highlightIndex       = 0,
        .highlightCount       = 1,
        .hotKey               = WAR_KEY_S,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnMenuLoad"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Load"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(78, 25)),
        .hotKey               = WAR_KEY_L,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnMenuOptions"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Options"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(20, 45)),
        .hotKey               = WAR_KEY_O,
        .clickHandler         = wmm_handleOptions,
        .highlightIndex = 0,
        .highlightCount = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnMenuObjectives"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Objectives"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(20, 65)),
        .hotKey               = WAR_KEY_J,
        .clickHandler         = wmm_handleObjectives,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnMenuRestart"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Restart scenario"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(20, 85)),
        .hotKey               = WAR_KEY_R,
        .clickHandler         = wmm_handleRestart,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnMenuContinue"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Continue"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(20, 105)),
        .hotKey               = WAR_KEY_C,
        .clickHandler         = wmm_handleContinue,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnMenuQuit"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Quit"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(78, 105)),
        .hotKey               = WAR_KEY_Q,
        .clickHandler         = wmm_handleQuit,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);
}

void wmm_createOptionsMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 menuPanel = RECT_TOP_LEFT(map->menuPanel);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);
    WarSpriteResourceRef leftArrowNormalRef = imageResourceRef(244);
    WarSpriteResourceRef leftArrowPressedRef = imageResourceRef(245);
    WarSpriteResourceRef rightArrowNormalRef = imageResourceRef(246);
    WarSpriteResourceRef rightArrowPressedRef = imageResourceRef(247);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsHeader"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(0, 10)),
        .fontIndex       = 1,
        .boundings       = vec2f(map->menuPanel.width, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Options"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsGameSpeedLabel"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(0, 25)),
        .fontIndex       = 1,
        .boundings       = vec2f(75, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Game Speed"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsMusicVolLabel"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(0, 42)),
        .fontIndex       = 1,
        .boundings       = vec2f(75, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Music Vol"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsSFXVolLabel"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(0, 59)),
        .fontIndex       = 1,
        .boundings       = vec2f(75, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("SFX Vol"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsMouseScrollLabel"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(0, 76)),
        .fontIndex       = 1,
        .boundings       = vec2f(75, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Mouse Scroll"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsKeyScrollLabel"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(0, 93)),
        .fontIndex       = 1,
        .boundings       = vec2f(75, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Key Scroll"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsGameSpeedValue"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(92, 25)),
        .fontIndex       = 1,
        .boundings       = vec2f(42, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Fastest"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsMusicVolValue"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(92, 42)),
        .fontIndex       = 1,
        .boundings       = vec2f(42, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("100"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsSFXVolValue"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(92, 59)),
        .fontIndex       = 1,
        .boundings       = vec2f(42, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("82"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsMouseScrollValue"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(92, 76)),
        .fontIndex       = 1,
        .boundings       = vec2f(42, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Slowest"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsKeyScrollValue"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(92, 93)),
        .fontIndex       = 1,
        .boundings       = vec2f(42, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Normal"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsGameSpeedDec"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = leftArrowNormalRef,
        .backgroundPressedRef = leftArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(76, 22)),
        .clickHandler         = wmm_handleGameSpeedDec,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsGameSpeedInc"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = rightArrowNormalRef,
        .backgroundPressedRef = rightArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(133, 22)),
        .clickHandler         = wmm_handleGameSpeedInc,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsMusicVolDec"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = leftArrowNormalRef,
        .backgroundPressedRef = leftArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(76, 39)),
        .clickHandler         = wmm_handleMusicVolDec,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsMusicVolInc"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = rightArrowNormalRef,
        .backgroundPressedRef = rightArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(133, 39)),
        .clickHandler         = wmm_handleMusicVolInc,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsSFXVolDec"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = leftArrowNormalRef,
        .backgroundPressedRef = leftArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(76, 56)),
        .clickHandler         = wmm_handleSfxVolDec,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsSFXVolInc"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = rightArrowNormalRef,
        .backgroundPressedRef = rightArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(133, 56)),
        .clickHandler         = wmm_handleSfxVolInc,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsMouseScrollDec"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = leftArrowNormalRef,
        .backgroundPressedRef = leftArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(76, 73)),
        .clickHandler         = wmm_handleMouseScrollSpeedDec,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsMouseScrollInc"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = rightArrowNormalRef,
        .backgroundPressedRef = rightArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(133, 73)),
        .clickHandler         = wmm_handleMouseScrollSpeedInc,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsKeyScrollDec"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = leftArrowNormalRef,
        .backgroundPressedRef = leftArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(76, 90)),
        .clickHandler         = wmm_handleKeyScrollSpeedDec,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnOptionsKeyScrollInc"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = rightArrowNormalRef,
        .backgroundPressedRef = rightArrowPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(133, 90)),
        .clickHandler         = wmm_handleKeyScrollSpeedInc,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnOptionsOk"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Ok"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(20, 115)),
        .hotKey               = WAR_KEY_O,
        .clickHandler         = wmm_handleOptionsOk,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnOptionsCancel"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Cancel"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(78, 115)),
        .hotKey               = WAR_KEY_C,
        .clickHandler         = wmm_handleOptionsCancel,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);
}

void createObjectivesMenu(WarContext* context)
{
    WarMap* map = context->map;

    WarResource* levelInfo = wres_getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarCampaignMapData campaignData = wcamp_getCampaignData(wmap_getCampaignMapTypeByLevelInfoIndex(map->levelInfoIndex));

    vec2 menuPanel = RECT_TOP_LEFT(map->menuPanel);
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(context, wstr_fromCString("txtObjectivesHeader"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(0, 10)),
        .fontIndex       = 1,
        .boundings       = vec2f(map->menuPanel.width, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Objectives"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtObjectivesText"), CREATE_UI_TEXT_ARGS_INIT(
        .position  = vec2_addv(menuPanel, vec2i(10, 26)),
        .fontIndex = 1,
        .multiline = true,
        .boundings = vec2f(map->menuPanel.width - 20, 75),
        .wrapping  = WAR_TEXT_WRAP_CHAR,
        .trimming  = WAR_TEXT_TRIM_SPACES,
        .text      = campaignData.objectives,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnObjectivesMenu"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Menu"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(20, 105)),
        .hotKey               = WAR_KEY_M,
        .clickHandler         = wmm_handleObjectivesMenu,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);
}

void createRestartMenu(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    vec2 messagePanel = RECT_TOP_LEFT(map->messagePanel);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = wui_createUIImage(context, wstr_fromCString("imgMessageMenuBackground"), CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRefFromPlayer(player, 235, 236),
        .position  = messagePanel,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtRestartText"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(messagePanel, vec2i(0, 10)),
        .fontIndex       = 1,
        .boundings       = vec2f(map->messagePanel.width, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Are you sure you want to restart?"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnRestartRestart"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Restart"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(messagePanel, vec2i(20, 25)),
    ));
    setUIEntityStatus(context, uiEntity, false);
    setUIButtonClickHandler(context, uiEntity, wmm_handleRestartRestart);
    setUIButtonHotKey(context, uiEntity, WAR_KEY_R);
    setUITextHighlight(context, uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnRestartCancel"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Cancel"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(messagePanel, vec2i(210, 25)),
        .hotKey               = WAR_KEY_C,
        .clickHandler         = wmm_handleRestartCancel,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);
}

void wmm_createGameOverMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 messagePanel = RECT_TOP_LEFT(map->messagePanel);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(context, wstr_fromCString("txtGameOverText"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(messagePanel, vec2i(0, 10)),
        .fontIndex       = 1,
        .boundings       = vec2f(map->messagePanel.width, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("You are victorious!"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnGameOverSave"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Save"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(messagePanel, vec2i(20, 25)),
        .hotKey               = WAR_KEY_S,
        .clickHandler         = wmm_handleGameOverSave,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnGameOverContinue"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Continue"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(messagePanel, vec2i(210, 25)),
        .hotKey               = WAR_KEY_C,
        .clickHandler         = wmm_handleGameOverContinue,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnGameOverOk"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Ok"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(messagePanel, vec2i(116, 25)),
        .hotKey               = WAR_KEY_O,
        // .clickHandler         = wmm_handleGameOverOk,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);
}

void wmm_createQuitMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 messagePanel = RECT_TOP_LEFT(map->messagePanel);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(context, wstr_fromCString("txtQuitText"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(messagePanel, vec2i(0, 10)),
        .fontIndex       = 1,
        .boundings       = vec2f(map->messagePanel.width, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Are you sure you want to quit?"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnQuitQuit"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Quit"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(messagePanel, vec2i(20, 25)),
        .hotKey               = WAR_KEY_Q,
        .clickHandler         = wmm_handleQuitQuit,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnQuitMenu"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Menu"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(messagePanel, vec2i(115, 25)),
        .hotKey               = WAR_KEY_M,
        .clickHandler         = wmm_handleQuitMenu,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnQuitCancel"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Cancel"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2_addv(messagePanel, vec2i(210, 25)),
        .hotKey               = WAR_KEY_C,
        .clickHandler         = wmm_handleQuitCancel,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);
}

void createDemoEndMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 menuPanel = RECT_TOP_LEFT(map->menuPanel);
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(context, wstr_fromCString("txtDemoEndHeader"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2_addv(menuPanel, vec2i(0, 10)),
        .fontIndex       = 1,
        .boundings       = vec2f(map->menuPanel.width, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Thanks for playing!"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtDemoEndText"), CREATE_UI_TEXT_ARGS_INIT(
        .position  = vec2_addv(menuPanel, vec2i(8, 26)),
        .fontIndex = 1,
        .fontSize  = 7,
        .multiline = true,
        .boundings = vec2f(map->menuPanel.width - 16, 75),
        .wrapping  = WAR_TEXT_WRAP_CHAR,
        .text      = wstr_fromCString(
            "This is a non-profit project with\n"
            "the personal goal of learning to\n"
            "do RTS engines\n"
            "\n"
            "This is not an official Blizzard\n"
            "product. Warcraft and all assets\n"
            "you see are registered trademarks\n"
            "of Blizzard Entertainment.\n"
            "\n"
            "Made by @acoto87"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnDemoEndMenu"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Menu"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2_addv(menuPanel, vec2i(20, 105)),
        .hotKey               = WAR_KEY_M,
        .clickHandler         = wmm_handleDemoEndMenu,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);
}

void wmm_enableOrDisableCommandButtons(WarContext* context, bool interactive)
{
    wui_setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand0"), interactive);
    wui_setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand1"), interactive);
    wui_setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand2"), interactive);
    wui_setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand3"), interactive);
    wui_setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand4"), interactive);
    wui_setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand5"), interactive);

    wui_setUIButtonInteractiveByName(context, wsv_fromCString("btnMenu"), interactive);
}

void wmm_showOrHideMenu(WarContext* context, bool status)
{
    wui_setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("imgMenuBackground"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtMenuHeader"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuSave"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuLoad"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuOptions"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuObjectives"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuRestart"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuContinue"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuQuit"), status);
}

void wmm_showOrHideOptionsMenu(WarContext* context, bool status)
{
    WarMap* map = context->map;

    wui_setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("imgMenuBackground"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsHeader"), status);

    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsGameSpeedLabel"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsGameSpeedValue"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsGameSpeedDec"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsGameSpeedInc"), status);

    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsMusicVolLabel"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsMusicVolValue"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsMusicVolDec"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsMusicVolInc"), status);

    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsSFXVolLabel"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsSFXVolValue"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsSFXVolDec"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsSFXVolInc"), status);

    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsMouseScrollLabel"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsMouseScrollValue"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsMouseScrollDec"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsMouseScrollInc"), status);

    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsKeyScrollLabel"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtOptionsKeyScrollValue"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsKeyScrollDec"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsKeyScrollInc"), status);

    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsOk"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnOptionsCancel"), status);

    if (status)
    {
        map->settings.musicVol = (s32)(context->musicVolume * 100);
        map->settings.sfxVol = (s32)(context->soundVolume * 100);
        memcpy(&map->settings2, &map->settings, sizeof(WarMapSettings));

        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsGameSpeedValue"), map->settings2.gameSpeed);
        setUITextS32ValueByName(context, wsv_fromCString("txtOptionsMusicVolValue"), map->settings2.musicVol);
        setUITextS32ValueByName(context, wsv_fromCString("txtOptionsSFXVolValue"), map->settings2.sfxVol);
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsMouseScrollValue"), map->settings2.mouseScrollSpeed);
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsKeyScrollValue"), map->settings2.keyScrollSpeed);
    }
}

void showOrHideObjectivesMenu(WarContext* context, bool status)
{
    wui_setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("imgMenuBackground"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtObjectivesHeader"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtObjectivesText"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnObjectivesMenu"), status);
}

void showOrHideRestartMenu(WarContext* context, bool status)
{
    wui_setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("imgMessageMenuBackground"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtRestartText"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnRestartRestart"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnRestartCancel"), status);
}

void wmm_showOrHideGameOverMenu(WarContext* context, bool status)
{
    WarMap* map = context->map;

    wmm_enableOrDisableCommandButtons(context, !status);

    wui_setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("imgMessageMenuBackground"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtGameOverText"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnGameOverSave"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnGameOverContinue"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnGameOverOk"), status);

    if (status)
    {
        if (map->result == WAR_LEVEL_RESULT_WIN)
        {
            WarEntity* gameOverText = we_findUIEntity(context, wsv_fromCString("txtGameOverText"));
            wui_setUIText(context, gameOverText, wstr_fromCString("You are victorious!"));

            WarEntity* saveBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverSave"));
            setUIEntityStatus(context, saveBtn, true);

            WarEntity* continueBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverContinue"));
            setUIEntityStatus(context, continueBtn, true);

            WarEntity* okBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverOk"));
            setUIEntityStatus(context, okBtn, false);
        }
        else if (map->result == WAR_LEVEL_RESULT_LOSE)
        {
            WarEntity* gameOverText = we_findUIEntity(context, wsv_fromCString("txtGameOverText"));
            wui_setUIText(context, gameOverText, wstr_fromCString("You failed to archieve victory..."));

            WarEntity* saveBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverSave"));
            setUIEntityStatus(context, saveBtn, false);

            WarEntity* continueBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverContinue"));
            setUIEntityStatus(context, continueBtn, false);

            WarEntity* okBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverOk"));
            setUIEntityStatus(context, okBtn, true);
        }
    }
}

void wmm_showOrHideQuitMenu(WarContext* context, bool status)
{
    wui_setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("imgMessageMenuBackground"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtQuitText"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnQuitQuit"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnQuitMenu"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnQuitCancel"), status);
}

void wmm_showDemoEndMenu(WarContext* context, bool status)
{
    wmm_enableOrDisableCommandButtons(context, !status);

    wui_setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("imgMenuBackground"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtDemoEndHeader"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtDemoEndText"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnDemoEndMenu"), status);
}

// menu button handlers
void wmm_handleMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    wmm_enableOrDisableCommandButtons(context, false);
    wmm_showOrHideMenu(context, true);

    map->playing = false;
}

void wmm_handleOptions(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    wmm_showOrHideMenu(context, false);
    wmm_showOrHideOptionsMenu(context, true);

    map->playing = false;
}

void wmm_handleObjectives(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    wmm_showOrHideMenu(context, false);
    showOrHideObjectivesMenu(context, true);

    map->playing = false;
}

void wmm_handleRestart(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    wmm_showOrHideMenu(context, false);
    showOrHideRestartMenu(context, true);

    map->playing = false;
}

void wmm_handleContinue(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    wmm_enableOrDisableCommandButtons(context, true);
    wmm_showOrHideMenu(context, false);

    map->playing = true;
}

void wmm_handleQuit(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    wmm_showOrHideMenu(context, false);
    wmm_showOrHideQuitMenu(context, true);

    map->playing = false;
}

void wmm_handleGameSpeedDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.gameSpeed > WAR_SPEED_SLOWEST)
    {
        map->settings2.gameSpeed--;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsGameSpeedValue"), map->settings2.gameSpeed);
    }
}

void wmm_handleGameSpeedInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.gameSpeed < WAR_SPEED_FASTEST)
    {
        map->settings2.gameSpeed++;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsGameSpeedValue"), map->settings2.gameSpeed);
    }
}

void wmm_handleMusicVolDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.musicVol = CLAMP(map->settings2.musicVol - 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsMusicVolValue"), map->settings2.musicVol);
}

void wmm_handleMusicVolInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.musicVol = CLAMP(map->settings2.musicVol + 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsMusicVolValue"), map->settings2.musicVol);
}

void wmm_handleSfxVolDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.sfxVol = CLAMP(map->settings2.sfxVol - 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsSFXVolValue"), map->settings2.sfxVol);
}

void wmm_handleSfxVolInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.sfxVol = CLAMP(map->settings2.sfxVol + 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsSFXVolValue"), map->settings2.sfxVol);
}

void wmm_handleMouseScrollSpeedDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.mouseScrollSpeed > WAR_SPEED_SLOWEST)
    {
        map->settings2.mouseScrollSpeed--;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsMouseScrollValue"), map->settings2.mouseScrollSpeed);
    }
}

void wmm_handleMouseScrollSpeedInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.mouseScrollSpeed < WAR_SPEED_FASTEST)
    {
        map->settings2.mouseScrollSpeed++;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsMouseScrollValue"), map->settings2.mouseScrollSpeed);
    }
}

void wmm_handleKeyScrollSpeedDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.keyScrollSpeed > WAR_SPEED_SLOWEST)
    {
        map->settings2.keyScrollSpeed--;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsKeyScrollValue"), map->settings2.keyScrollSpeed);
    }
}

void wmm_handleKeyScrollSpeedInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.keyScrollSpeed < WAR_SPEED_FASTEST)
    {
        map->settings2.keyScrollSpeed++;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsKeyScrollValue"), map->settings2.keyScrollSpeed);
    }
}

void wmm_handleOptionsOk(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    // persist the changes
    memcpy(&map->settings, &map->settings2, sizeof(WarMapSettings));
    context->musicVolume = (f32)map->settings.musicVol / 100;
    context->soundVolume = (f32)map->settings.sfxVol / 100;

    wmm_showOrHideOptionsMenu(context, false);
    wmm_showOrHideMenu(context, true);

    map->playing = false;
}

void wmm_handleOptionsCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    wmm_showOrHideOptionsMenu(context, false);
    wmm_showOrHideMenu(context, true);

    map->playing = false;
}

void wmm_handleObjectivesMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideObjectivesMenu(context, false);
    wmm_showOrHideMenu(context, true);

    map->playing = false;
}

void wmm_handleRestartRestart(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    s32 levelInfoIndex = context->map->levelInfoIndex;

    WarMap* map = wmap_createMap(context, levelInfoIndex);
    wg_setNextMap(context, map, 1.0f);
}

void wmm_handleRestartCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideRestartMenu(context, false);
    wmm_showOrHideMenu(context, true);

    map->playing = false;
}

void wmm_handleGameOverSave(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);
    NOT_USED(entity);

    NOT_IMPLEMENTED();
}

void wmm_handleGameOverContinue(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;
    s32 levelInfoIndex = map->levelInfoIndex;

    if (map->result == WAR_LEVEL_RESULT_WIN)
    {
        WarScene* scene = wsc_createScene(context, WAR_SCENE_BRIEFING);
        scene->briefing.race = map->players[0].race;
        scene->briefing.mapType = levelInfoIndex + 2;
        wg_setNextScene(context, scene, 1.0f);
    }
    else if (map->result == WAR_LEVEL_RESULT_LOSE)
    {
        WarScene* scene = wsc_createScene(context, WAR_SCENE_BRIEFING);
        scene->briefing.race = map->players[0].race;
        scene->briefing.mapType = levelInfoIndex;
        wg_setNextScene(context, scene, 1.0f);
    }
    else
    {
        logError("It shouldn't reach here! Map result: %d", map->result);
        assert(false);
    }
}

void wmm_handleQuitQuit(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);
    NOT_USED(entity);

    SDL_PushEvent(&(SDL_Event){ .type = SDL_EVENT_QUIT });
}

void wmm_handleQuitMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = wsc_createScene(context, WAR_SCENE_MAIN_MENU);
    wg_setNextScene(context, scene, 1.0f);
}

void wmm_handleQuitCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    wmm_showOrHideQuitMenu(context, false);
    wmm_showOrHideMenu(context, true);

    map->playing = false;
}

void wmm_handleDemoEndMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = wsc_createScene(context, WAR_SCENE_MAIN_MENU);
    wg_setNextScene(context, scene, 1.0f);
}
