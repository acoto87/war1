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
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        setUIText(entity, getSpeedStr(value));
        setUITextHighlight(entity, NO_HIGHLIGHT, 0);
    }
}

static void setUITextS32ValueByName(WarContext* context, StringView name, s32 value)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        setUIText(entity, wstr_fromCStringFormat("%d", value));
        setUITextHighlight(entity, NO_HIGHLIGHT, 0);
    }
}

void createMenu(WarContext* context)
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

    uiEntity = createUIRect(
        context, wstr_fromCString("rectMenuBackdrop"),
        VEC2_ZERO, vec2i(context->windowWidth, context->windowHeight),
        WAR_COLOR_RGBA(0, 0, 0, 150));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImage(context, wstr_fromCString("imgMenuBackground"), imageResourceRefFromPlayer(player, 233, 234), menuPanel);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIText(
        context, wstr_fromCString("txtMenuHeader"), 1, 10, wstr_fromCString("Warcraft"),
        vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuSave"),
        1, 10, wstr_fromCString("Save"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonHotKey(uiEntity, WAR_KEY_S);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuLoad"),
        1, 10, wstr_fromCString("Load"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(78, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonHotKey(uiEntity, WAR_KEY_L);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuOptions"),
        1, 10, wstr_fromCString("Options"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 45)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleOptions);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuObjectives"),
        1, 10, wstr_fromCString("Objectives"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 65)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleObjectives);
    setUIButtonHotKey(uiEntity, WAR_KEY_J);
    setUITextHighlight(uiEntity, 2, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuRestart"),
        1, 10, wstr_fromCString("Restart scenario"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 85)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleRestart);
    setUIButtonHotKey(uiEntity, WAR_KEY_R);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuContinue"),
        1, 10, wstr_fromCString("Continue"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleContinue);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuQuit"),
        1, 10, wstr_fromCString("Quit"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(78, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleQuit);
    setUIButtonHotKey(uiEntity, WAR_KEY_Q);
    setUITextHighlight(uiEntity, 0, 1);
}

void createOptionsMenu(WarContext* context)
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

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsHeader"),
        1, 10, wstr_fromCString("Options"), vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsGameSpeedLabel"),
        1, 10, wstr_fromCString("Game Speed"), vec2Addv(menuPanel, vec2i(0, 25)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsMusicVolLabel"),
        1, 10, wstr_fromCString("Music Vol"), vec2Addv(menuPanel, vec2i(0, 42)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsSFXVolLabel"),
        1, 10, wstr_fromCString("SFX Vol"), vec2Addv(menuPanel, vec2i(0, 59)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsMouseScrollLabel"),
        1, 10, wstr_fromCString("Mouse Scroll"), vec2Addv(menuPanel, vec2i(0, 76)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsKeyScrollLabel"),
        1, 10, wstr_fromCString("Key Scroll"), vec2Addv(menuPanel, vec2i(0, 93)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsGameSpeedValue"),
        1, 10, wstr_fromCString("Fastest"), vec2Addv(menuPanel, vec2i(92, 25)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsMusicVolValue"),
        1, 10, wstr_fromCString("100"), vec2Addv(menuPanel, vec2i(92, 42)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsSFXVolValue"),
        1, 10, wstr_fromCString("82"), vec2Addv(menuPanel, vec2i(92, 59)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsMouseScrollValue"),
        1, 10, wstr_fromCString("Slowest"), vec2Addv(menuPanel, vec2i(92, 76)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtOptionsKeyScrollValue"),
        1, 10, wstr_fromCString("Normal"), vec2Addv(menuPanel, vec2i(92, 93)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsGameSpeedDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 22)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameSpeedDec);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsGameSpeedInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 22)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameSpeedInc);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsMusicVolDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 39)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMusicVolDec);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsMusicVolInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 39)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMusicVolInc);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsSFXVolDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 56)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSfxVolDec);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsSFXVolInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 56)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSfxVolInc);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsMouseScrollDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 73)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMouseScrollSpeedDec);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsMouseScrollInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 73)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMouseScrollSpeedInc);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsKeyScrollDec"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 90)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleKeyScrollSpeedDec);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnOptionsKeyScrollInc"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 90)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleKeyScrollSpeedInc);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnOptionsOk"),
        1, 10, wstr_fromCString("Ok"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 115)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleOptionsOk);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnOptionsCancel"),
        1, 10, wstr_fromCString("Cancel"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(78, 115)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleOptionsCancel);
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

    uiEntity = createUIText(context, wstr_fromCString("txtObjectivesHeader"),
        1, 10, wstr_fromCString("Objectives"), vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, wstr_fromCString("txtObjectivesText"),
        1, 10, campaignData.objectives, vec2Addv(menuPanel, vec2i(10, 26)));
    setUIEntityStatus(uiEntity, false);
    setUITextMultiline(uiEntity, true);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width - 20, 75));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(uiEntity, WAR_TEXT_WRAP_CHAR);
    setUITextTrimming(uiEntity, WAR_TEXT_TRIM_SPACES);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnObjectivesMenu"),
        1, 10, wstr_fromCString("Menu"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleObjectivesMenu);
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

    uiEntity = createUIImage(context, wstr_fromCString("imgMessageMenuBackground"), imageResourceRefFromPlayer(player, 235, 236), messagePanel);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIText(context, wstr_fromCString("txtRestartText"),
        1, 10, wstr_fromCString("Are you sure you want to restart?"),
        vec2Addv(messagePanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->messagePanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnRestartRestart"),
        1, 10, wstr_fromCString("Restart"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleRestartRestart);
    setUIButtonHotKey(uiEntity, WAR_KEY_R);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnRestartCancel"),
        1, 10, wstr_fromCString("Cancel"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(210, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleRestartCancel);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void createGameOverMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUIText(context, wstr_fromCString("txtGameOverText"),
        1, 10, wstr_fromCString("You are victorious!"),
        vec2Addv(messagePanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->messagePanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnGameOverSave"),
        1, 10, wstr_fromCString("Save"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameOverSave);
    setUIButtonHotKey(uiEntity, WAR_KEY_S);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnGameOverContinue"),
        1, 10, wstr_fromCString("Continue"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(210, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameOverContinue);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnGameOverOk"),
        1, 10, wstr_fromCString("Ok"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(116, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameOverContinue);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);
}

void createQuitMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUIText(context, wstr_fromCString("txtQuitText"),
        1, 10, wstr_fromCString("Are you sure you want to quit?"),
        vec2Addv(messagePanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->messagePanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnQuitQuit"),
        1, 10, wstr_fromCString("Quit"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleQuitQuit);
    setUIButtonHotKey(uiEntity, WAR_KEY_Q);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnQuitMenu"),
        1, 10, wstr_fromCString("Menu"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(115, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleQuitMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_M);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnQuitCancel"),
        1, 10, wstr_fromCString("Cancel"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(210, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleQuitCancel);
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

    uiEntity = createUIText(
        context, wstr_fromCString("txtDemoEndHeader"),
        1, 10,
        wstr_fromCString("Thanks for playing!"),
        vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
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

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnDemoEndMenu"),
        1, 10, wstr_fromCString("Menu"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleDemoEndMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_M);
    setUITextHighlight(uiEntity, 0, 1);
}

void enableOrDisableCommandButtons(WarContext* context, bool interactive)
{
    setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand0"), interactive);
    setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand1"), interactive);
    setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand2"), interactive);
    setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand3"), interactive);
    setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand4"), interactive);
    setUIButtonInteractiveByName(context, wsv_fromCString("btnCommand5"), interactive);

    setUIButtonInteractiveByName(context, wsv_fromCString("btnMenu"), interactive);
}

void showOrHideMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    setUIEntityStatusByName(context, wsv_fromCString("imgMenuBackground"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtMenuHeader"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuSave"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuLoad"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuOptions"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuObjectives"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuRestart"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuContinue"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuQuit"), status);
}

void showOrHideOptionsMenu(WarContext* context, bool status)
{
    WarMap* map = context->map;

    setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    setUIEntityStatusByName(context, wsv_fromCString("imgMenuBackground"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsHeader"), status);

    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsGameSpeedLabel"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsGameSpeedValue"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsGameSpeedDec"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsGameSpeedInc"), status);

    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsMusicVolLabel"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsMusicVolValue"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsMusicVolDec"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsMusicVolInc"), status);

    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsSFXVolLabel"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsSFXVolValue"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsSFXVolDec"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsSFXVolInc"), status);

    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsMouseScrollLabel"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsMouseScrollValue"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsMouseScrollDec"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsMouseScrollInc"), status);

    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsKeyScrollLabel"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtOptionsKeyScrollValue"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsKeyScrollDec"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsKeyScrollInc"), status);

    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsOk"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnOptionsCancel"), status);

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
    setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    setUIEntityStatusByName(context, wsv_fromCString("imgMenuBackground"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtObjectivesHeader"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtObjectivesText"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnObjectivesMenu"), status);
}

void showOrHideRestartMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    setUIEntityStatusByName(context, wsv_fromCString("imgMessageMenuBackground"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtRestartText"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnRestartRestart"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnRestartCancel"), status);
}

void showOrHideGameOverMenu(WarContext* context, bool status)
{
    WarMap* map = context->map;

    enableOrDisableCommandButtons(context, !status);

    setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    setUIEntityStatusByName(context, wsv_fromCString("imgMessageMenuBackground"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtGameOverText"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnGameOverSave"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnGameOverContinue"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnGameOverOk"), status);

    if (status)
    {
        if (map->result == WAR_LEVEL_RESULT_WIN)
        {
            WarEntity* gameOverText = findUIEntity(context, wsv_fromCString("txtGameOverText"));
            setUIText(gameOverText, wstr_fromCString("You are victorious!"));

            WarEntity* saveBtn = findUIEntity(context, wsv_fromCString("btnGameOverSave"));
            setUIEntityStatus(saveBtn, true);

            WarEntity* continueBtn = findUIEntity(context, wsv_fromCString("btnGameOverContinue"));
            setUIEntityStatus(continueBtn, true);

            WarEntity* okBtn = findUIEntity(context, wsv_fromCString("btnGameOverOk"));
            setUIEntityStatus(okBtn, false);
        }
        else if (map->result == WAR_LEVEL_RESULT_LOSE)
        {
            WarEntity* gameOverText = findUIEntity(context, wsv_fromCString("txtGameOverText"));
            setUIText(gameOverText, wstr_fromCString("You failed to archieve victory..."));

            WarEntity* saveBtn = findUIEntity(context, wsv_fromCString("btnGameOverSave"));
            setUIEntityStatus(saveBtn, false);

            WarEntity* continueBtn = findUIEntity(context, wsv_fromCString("btnGameOverContinue"));
            setUIEntityStatus(continueBtn, false);

            WarEntity* okBtn = findUIEntity(context, wsv_fromCString("btnGameOverOk"));
            setUIEntityStatus(okBtn, true);
        }
    }
}

void showOrHideQuitMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    setUIEntityStatusByName(context, wsv_fromCString("imgMessageMenuBackground"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtQuitText"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnQuitQuit"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnQuitMenu"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnQuitCancel"), status);
}

void showDemoEndMenu(WarContext* context, bool status)
{
    enableOrDisableCommandButtons(context, !status);

    setUIEntityStatusByName(context, wsv_fromCString("rectMenuBackdrop"), status);
    setUIEntityStatusByName(context, wsv_fromCString("imgMenuBackground"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtDemoEndHeader"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtDemoEndText"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnDemoEndMenu"), status);
}

// menu button handlers
void handleMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    enableOrDisableCommandButtons(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleOptions(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideOptionsMenu(context, true);

    map->playing = false;
}

void handleObjectives(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideObjectivesMenu(context, true);

    map->playing = false;
}

void handleRestart(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideRestartMenu(context, true);

    map->playing = false;
}

void handleContinue(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    enableOrDisableCommandButtons(context, true);
    showOrHideMenu(context, false);

    map->playing = true;
}

void handleQuit(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideQuitMenu(context, true);

    map->playing = false;
}

void handleGameSpeedDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.gameSpeed > WAR_SPEED_SLOWEST)
    {
        map->settings2.gameSpeed--;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsGameSpeedValue"), map->settings2.gameSpeed);
    }
}

void handleGameSpeedInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.gameSpeed < WAR_SPEED_FASTEST)
    {
        map->settings2.gameSpeed++;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsGameSpeedValue"), map->settings2.gameSpeed);
    }
}

void handleMusicVolDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.musicVol = clamp(map->settings2.musicVol - 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsMusicVolValue"), map->settings2.musicVol);
}

void handleMusicVolInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.musicVol = clamp(map->settings2.musicVol + 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsMusicVolValue"), map->settings2.musicVol);
}

void handleSfxVolDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.sfxVol = clamp(map->settings2.sfxVol - 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsSFXVolValue"), map->settings2.sfxVol);
}

void handleSfxVolInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.sfxVol = clamp(map->settings2.sfxVol + 5, 0, 100);
    setUITextS32ValueByName(context, wsv_fromCString("txtOptionsSFXVolValue"), map->settings2.sfxVol);
}

void handleMouseScrollSpeedDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.mouseScrollSpeed > WAR_SPEED_SLOWEST)
    {
        map->settings2.mouseScrollSpeed--;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsMouseScrollValue"), map->settings2.mouseScrollSpeed);
    }
}

void handleMouseScrollSpeedInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.mouseScrollSpeed < WAR_SPEED_FASTEST)
    {
        map->settings2.mouseScrollSpeed++;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsMouseScrollValue"), map->settings2.mouseScrollSpeed);
    }
}

void handleKeyScrollSpeedDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.keyScrollSpeed > WAR_SPEED_SLOWEST)
    {
        map->settings2.keyScrollSpeed--;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsKeyScrollValue"), map->settings2.keyScrollSpeed);
    }
}

void handleKeyScrollSpeedInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.keyScrollSpeed < WAR_SPEED_FASTEST)
    {
        map->settings2.keyScrollSpeed++;
        setUITextSpeedValueByName(context, wsv_fromCString("txtOptionsKeyScrollValue"), map->settings2.keyScrollSpeed);
    }
}

void handleOptionsOk(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    // persist the changes
    memcpy(&map->settings, &map->settings2, sizeof(WarMapSettings));
    context->musicVolume = (f32)map->settings.musicVol / 100;
    context->soundVolume = (f32)map->settings.sfxVol / 100;

    showOrHideOptionsMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleOptionsCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideOptionsMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleObjectivesMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideObjectivesMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleRestartRestart(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    s32 levelInfoIndex = context->map->levelInfoIndex;

    WarMap* map = wmap_createMap(context, levelInfoIndex);
    setNextMap(context, map, 1.0f);
}

void handleRestartCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideRestartMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleGameOverSave(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);
    NOT_USED(entity);

    NOT_IMPLEMENTED();
}

void handleGameOverContinue(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;
    s32 levelInfoIndex = map->levelInfoIndex;

    if (map->result == WAR_LEVEL_RESULT_WIN)
    {
        WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
        scene->briefing.race = map->players[0].race;
        scene->briefing.mapType = levelInfoIndex + 2;
        setNextScene(context, scene, 1.0f);
    }
    else if (map->result == WAR_LEVEL_RESULT_LOSE)
    {
        WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
        scene->briefing.race = map->players[0].race;
        scene->briefing.mapType = levelInfoIndex;
        setNextScene(context, scene, 1.0f);
    }
    else
    {
        logError("It shouldn't reach here! Map result: %d", map->result);
        assert(false);
    }
}

void handleQuitQuit(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);
    NOT_USED(entity);

    SDL_PushEvent(&(SDL_Event){ .type = SDL_EVENT_QUIT });
}

void handleQuitMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = createScene(context, WAR_SCENE_MAIN_MENU);
    setNextScene(context, scene, 1.0f);
}

void handleQuitCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    showOrHideQuitMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleDemoEndMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = createScene(context, WAR_SCENE_MAIN_MENU);
    setNextScene(context, scene, 1.0f);
}
