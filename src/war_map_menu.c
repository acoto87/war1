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
        wui_setUIText(entity, getSpeedStr(value));
        setUITextHighlight(entity, NO_HIGHLIGHT, 0);
    }
}

static void setUITextS32ValueByName(WarContext* context, StringView name, s32 value)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        wui_setUIText(entity, wstr_fromCStringFormat("%d", value));
        setUITextHighlight(entity, NO_HIGHLIGHT, 0);
    }
}

void wmm_createMenu(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    vec2 menuPanel = rectTopLeft(map->menuPanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = wui_createUIRect(
        context, wstr_fromCString("rectMenuBackdrop"),
        VEC2_ZERO, vec2i(context->windowWidth, context->windowHeight),
        WAR_COLOR_RGBA(0, 0, 0, 150));
    setUIEntityStatus(uiEntity, false);

    uiEntity = wui_createUIImage(context, wstr_fromCString("imgMenuBackground"), imageResourceRefFromPlayer(player, 233, 234), menuPanel);
    setUIEntityStatus(uiEntity, false);

    uiEntity = wui_createUIText(
        context, wstr_fromCString("txtMenuHeader"), 1, 10, wstr_fromCString("Warcraft"),
        vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnMenuSave"),
        1, 10, wstr_fromCString("Save"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonHotKey(uiEntity, WAR_KEY_S);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnMenuLoad"),
        1, 10, wstr_fromCString("Load"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(78, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonHotKey(uiEntity, WAR_KEY_L);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnMenuOptions"),
        1, 10, wstr_fromCString("Options"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 45)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleOptions);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnMenuObjectives"),
        1, 10, wstr_fromCString("Objectives"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 65)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleObjectives);
    setUIButtonHotKey(uiEntity, WAR_KEY_J);
    setUITextHighlight(uiEntity, 2, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnMenuRestart"),
        1, 10, wstr_fromCString("Restart scenario"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 85)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleRestart);
    setUIButtonHotKey(uiEntity, WAR_KEY_R);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnMenuContinue"),
        1, 10, wstr_fromCString("Continue"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleContinue);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnMenuQuit"),
        1, 10, wstr_fromCString("Quit"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(78, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleQuit);
    setUIButtonHotKey(uiEntity, WAR_KEY_Q);
    setUITextHighlight(uiEntity, 0, 1);
}

void wmm_createOptionsMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 menuPanel = rectTopLeft(map->menuPanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);
    WarSpriteResourceRef leftArrowNormalRef = imageResourceRef(244);
    WarSpriteResourceRef leftArrowPressedRef = imageResourceRef(245);
    WarSpriteResourceRef rightArrowNormalRef = imageResourceRef(246);
    WarSpriteResourceRef rightArrowPressedRef = imageResourceRef(247);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsHeader"),
        1, 10, wstr_fromCString("Options"), vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsGameSpeedLabel"),
        1, 10, wstr_fromCString("Game Speed"), vec2Addv(menuPanel, vec2i(0, 25)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsMusicVolLabel"),
        1, 10, wstr_fromCString("Music Vol"), vec2Addv(menuPanel, vec2i(0, 42)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsSFXVolLabel"),
        1, 10, wstr_fromCString("SFX Vol"), vec2Addv(menuPanel, vec2i(0, 59)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsMouseScrollLabel"),
        1, 10, wstr_fromCString("Mouse Scroll"), vec2Addv(menuPanel, vec2i(0, 76)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsKeyScrollLabel"),
        1, 10, wstr_fromCString("Key Scroll"), vec2Addv(menuPanel, vec2i(0, 93)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsGameSpeedValue"),
        1, 10, wstr_fromCString("Fastest"), vec2Addv(menuPanel, vec2i(92, 25)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsMusicVolValue"),
        1, 10, wstr_fromCString("100"), vec2Addv(menuPanel, vec2i(92, 42)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsSFXVolValue"),
        1, 10, wstr_fromCString("82"), vec2Addv(menuPanel, vec2i(92, 59)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsMouseScrollValue"),
        1, 10, wstr_fromCString("Slowest"), vec2Addv(menuPanel, vec2i(92, 76)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtOptionsKeyScrollValue"),
        1, 10, wstr_fromCString("Normal"), vec2Addv(menuPanel, vec2i(92, 93)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsGameSpeedDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 22)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleGameSpeedDec);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsGameSpeedInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 22)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleGameSpeedInc);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsMusicVolDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 39)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleMusicVolDec);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsMusicVolInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 39)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleMusicVolInc);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsSFXVolDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 56)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleSfxVolDec);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsSFXVolInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 56)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleSfxVolInc);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsMouseScrollDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 73)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleMouseScrollSpeedDec);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsMouseScrollInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 73)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleMouseScrollSpeedInc);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsKeyScrollDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 90)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleKeyScrollSpeedDec);

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnOptionsKeyScrollInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 90)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleKeyScrollSpeedInc);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnOptionsOk"),
        1, 10, wstr_fromCString("Ok"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 115)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleOptionsOk);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnOptionsCancel"),
        1, 10, wstr_fromCString("Cancel"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(78, 115)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleOptionsCancel);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void createObjectivesMenu(WarContext* context)
{
    WarMap* map = context->map;

    WarResource* levelInfo = wres_getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarCampaignMapData campaignData = wcamp_getCampaignData(wmap_getCampaignMapTypeByLevelInfoIndex(map->levelInfoIndex));

    vec2 menuPanel = rectTopLeft(map->menuPanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(context, wstr_fromCString("txtObjectivesHeader"),
        1, 10, wstr_fromCString("Objectives"), vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtObjectivesText"),
        1, 10, campaignData.objectives, vec2Addv(menuPanel, vec2i(10, 26)));
    setUIEntityStatus(uiEntity, false);
    setUITextMultiline(uiEntity, true);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width - 20, 75));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(uiEntity, WAR_TEXT_WRAP_CHAR);
    setUITextTrimming(uiEntity, WAR_TEXT_TRIM_SPACES);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnObjectivesMenu"),
        1, 10, wstr_fromCString("Menu"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleObjectivesMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_M);
    setUITextHighlight(uiEntity, 0, 1);
}

void createRestartMenu(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = wui_createUIImage(context, wstr_fromCString("imgMessageMenuBackground"), imageResourceRefFromPlayer(player, 235, 236), messagePanel);
    setUIEntityStatus(uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtRestartText"),
        1, 10, wstr_fromCString("Are you sure you want to restart?"),
        vec2Addv(messagePanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->messagePanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnRestartRestart"),
        1, 10, wstr_fromCString("Restart"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleRestartRestart);
    setUIButtonHotKey(uiEntity, WAR_KEY_R);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnRestartCancel"),
        1, 10, wstr_fromCString("Cancel"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(210, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleRestartCancel);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void wmm_createGameOverMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(context, wstr_fromCString("txtGameOverText"),
        1, 10, wstr_fromCString("You are victorious!"),
        vec2Addv(messagePanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->messagePanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnGameOverSave"),
        1, 10, wstr_fromCString("Save"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleGameOverSave);
    setUIButtonHotKey(uiEntity, WAR_KEY_S);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnGameOverContinue"),
        1, 10, wstr_fromCString("Continue"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(210, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleGameOverContinue);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnGameOverOk"),
        1, 10, wstr_fromCString("Ok"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(116, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleGameOverContinue);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);
}

void wmm_createQuitMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(context, wstr_fromCString("txtQuitText"),
        1, 10, wstr_fromCString("Are you sure you want to quit?"),
        vec2Addv(messagePanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->messagePanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnQuitQuit"),
        1, 10, wstr_fromCString("Quit"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleQuitQuit);
    setUIButtonHotKey(uiEntity, WAR_KEY_Q);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnQuitMenu"),
        1, 10, wstr_fromCString("Menu"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(115, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleQuitMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_M);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnQuitCancel"),
        1, 10, wstr_fromCString("Cancel"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(210, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleQuitCancel);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void createDemoEndMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 menuPanel = rectTopLeft(map->menuPanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);

    WarEntity* uiEntity;

    uiEntity = wui_createUIText(
        context, wstr_fromCString("txtDemoEndHeader"),
        1, 10,
        wstr_fromCString("Thanks for playing!"),
        vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = wui_createUIText(
        context, wstr_fromCString("txtDemoEndText"),
        1, 7,
        wstr_fromCString("This is a non-profit project with\n"
        "the personal goal of learning to\n"
        "do RTS engines\n"
        "\n"
        "This is not an official Blizzard\n"
        "product. Warcraft and all assets\n"
        "you see are registered trademarks\n"
        "of Blizzard Entertainment.\n"
        "\n"
        "Made by @acoto87"),
        vec2Addv(menuPanel, vec2i(8, 26)));
    setUIEntityStatus(uiEntity, false);
    setUITextMultiline(uiEntity, true);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width - 16, 75));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(uiEntity, WAR_TEXT_WRAP_CHAR);

    uiEntity = wui_createUITextButton(
        context, wstr_fromCString("btnDemoEndMenu"),
        1, 10, wstr_fromCString("Menu"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, wmm_handleDemoEndMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_M);
    setUITextHighlight(uiEntity, 0, 1);
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
            wui_setUIText(gameOverText, wstr_fromCString("You are victorious!"));

            WarEntity* saveBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverSave"));
            setUIEntityStatus(saveBtn, true);

            WarEntity* continueBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverContinue"));
            setUIEntityStatus(continueBtn, true);

            WarEntity* okBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverOk"));
            setUIEntityStatus(okBtn, false);
        }
        else if (map->result == WAR_LEVEL_RESULT_LOSE)
        {
            WarEntity* gameOverText = we_findUIEntity(context, wsv_fromCString("txtGameOverText"));
            wui_setUIText(gameOverText, wstr_fromCString("You failed to archieve victory..."));

            WarEntity* saveBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverSave"));
            setUIEntityStatus(saveBtn, false);

            WarEntity* continueBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverContinue"));
            setUIEntityStatus(continueBtn, false);

            WarEntity* okBtn = we_findUIEntity(context, wsv_fromCString("btnGameOverOk"));
            setUIEntityStatus(okBtn, true);
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

    map->settings2.musicVol = clamp(map->settings2.musicVol - 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsMusicVolValue"), map->settings2.musicVol);
}

void wmm_handleMusicVolInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.musicVol = clamp(map->settings2.musicVol + 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsMusicVolValue"), map->settings2.musicVol);
}

void wmm_handleSfxVolDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.sfxVol = clamp(map->settings2.sfxVol - 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsSFXVolValue"), map->settings2.sfxVol);
}

void wmm_handleSfxVolInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.sfxVol = clamp(map->settings2.sfxVol + 5, 0, 100);
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
