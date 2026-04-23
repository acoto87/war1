#include "war_scene_menu.h"

#include "shl/wstr.h"

#include "war_audio.h"
#include "war_game.h"
#include "war_map.h"
#include "war_ui.h"

static String getCustomGameRaceStr(WarRace value)
{
    const char* str = "";
    switch (value)
    {
        case WAR_RACE_NEUTRAL: str = "Random"; break;
        case WAR_RACE_HUMANS:  str = "Human"; break;
        case WAR_RACE_ORCS:    str = "Orc"; break;
        default: str = ""; break;
    }
    return wstr_fromCString(str);
}

static String getCustomMapStr(s32 value)
{
    const char* str = "";
    switch (value)
    {
        case 147: str = "Forest 1"; break;
        case 148: str = "Forest 2"; break;
        case 149: str = "Forest 3"; break;
        case 150: str = "Forest 4"; break;
        case 151: str = "Forest 5"; break;
        case 152: str = "Forest 6"; break;
        case 153: str = "Forest 7"; break;
        case 154: str = "Swamp 6"; break;
        case 155: str = "Swamp 7"; break;
        case 156: str = "Swamp 1"; break;
        case 157: str = "Swamp 2"; break;
        case 158: str = "Swamp 3"; break;
        case 159: str = "Swamp 4"; break;
        case 160: str = "Swamp 5"; break;
        case 161: str = "Dungeon 1"; break;
        case 162: str = "Dungeon 2"; break;
        case 163: str = "Dungeon 3"; break;
        case 164: str = "Dungeon 4"; break;
        case 165: str = "Dungeon 5"; break;
        case 166: str = "Dungeon 6"; break;
        case 167: str = "Dungeon 7"; break;
        case 168: str = "Forest 1.1"; break;
        case 169: str = "Forest 2.1"; break;
        case 170: str = "Forest 3.1"; break;
        case 171: str = "Forest 4.1"; break;
        case 172: str = "Forest 5.1"; break;
        case 173: str = "Forest 6.1"; break;
        case 174: str = "Forest 7.1"; break;
        case 175: str = "Swamp 6.1"; break;
        case 176: str = "Swamp 7.1"; break;
        case 177: str = "Swamp 1.1"; break;
        case 178: str = "Swamp 2.1"; break;
        case 179: str = "Swamp 3.1"; break;
        case 180: str = "Swamp 4.1"; break;
        case 181: str = "Swamp 5.1"; break;
        case 182: str = "Dungeon 1.1"; break;
        case 183: str = "Dungeon 2.1"; break;
        case 184: str = "Dungeon 3.1"; break;
        case 185: str = "Dungeon 4.1"; break;
        case 186: str = "Dungeon 5.1"; break;
        case 187: str = "Dungeon 6.1"; break;
        case 188: str = "Dungeon 7.1"; break;
        default: str = ""; break;
    }
    return wstr_fromCString(str);
}

static void setUIRaceValueByName(WarContext* context, StringView name, WarRace value)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        setUIText(entity, getCustomGameRaceStr(value));
        setUITextHighlight(entity, NO_HIGHLIGHT, 0);
    }
}

static void setCustomMapValueByName(WarContext* context, StringView name, s32 value)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        setUIText(entity, getCustomMapStr(value));
        setUITextHighlight(entity, NO_HIGHLIGHT, 0);
    }
}

void enterSceneMainMenu(WarContext* context)
{
    createMainMenu(context);
    createSinglePlayerMenu(context);
    createLoadMenu(context);
    createCustomGameMenu(context);
    createCheatsPanel(context);

    createUICursor(context, wstr_fromCString("cursor"), WAR_CURSOR_ARROW, VEC2_ZERO);

    if (!isDemo(context))
        createAudio(context, WAR_MUSIC_00, true);
}

void createMainMenu(WarContext* context)
{
    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);

    WarEntity* uiEntity;

    uiEntity = createUIImage(context, wstr_fromCString("imgMenuBackground"), imageResourceRef(261), VEC2_ZERO);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuSinglePlayer"),
        1, 10, wstr_fromCString("Start a new game"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 85));
    setUIButtonClickHandler(uiEntity, handleMenuSinglePlayer);
    setUIButtonHotKey(uiEntity, WAR_KEY_S);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuLoad"),
        1, 10, wstr_fromCString("Load existing game"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 105));
    setUIButtonHotKey(uiEntity, WAR_KEY_L);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuReplayIntro"),
        1, 10, wstr_fromCString("Replay introduction"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 125));
    setUIButtonHotKey(uiEntity, WAR_KEY_R);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnMenuQuit"),
        1, 10, wstr_fromCString("Quit"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 165));
    setUIButtonClickHandler(uiEntity, handleMenuQuit);
    setUIButtonHotKey(uiEntity, WAR_KEY_Q);
    setUITextHighlight(uiEntity, 0, 1);
}

void createSinglePlayerMenu(WarContext* context)
{
    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnSinglePlayerOrc"),
        1, 10, wstr_fromCString("Orc campaign"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 85));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSinglePlayerOrc);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnSinglePlayerHuman"),
        1, 10, wstr_fromCString("Human campaign"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 105));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSinglePlayerHuman);
    setUIButtonHotKey(uiEntity, WAR_KEY_H);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnCustomGame"),
        1, 10, wstr_fromCString("Custom game"),
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 125));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleCustomGame);
    setUIButtonHotKey(uiEntity, WAR_KEY_U);
    setUITextHighlight(uiEntity, 1, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnSinglePlayerCancel"),
        1, 10, wstr_fromCString("Cancel"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2i(133, 165));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSinglePlayerCancel);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void createLoadMenu(WarContext* context)
{
    NOT_USED(context);
}

void createCustomGameMenu(WarContext* context)
{
    WarEntity* uiEntity;

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);
    WarSpriteResourceRef leftArrowNormalRef = imageResourceRef(244);
    WarSpriteResourceRef leftArrowPressedRef = imageResourceRef(245);
    WarSpriteResourceRef rightArrowNormalRef = imageResourceRef(246);
    WarSpriteResourceRef rightArrowPressedRef = imageResourceRef(247);

    uiEntity = createUIText(
        context, wstr_fromCString("txtYourRaceLabel"),
        1, 10, wstr_fromCString("Your race:"),
        vec2i(40, 105));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(100, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
        context, wstr_fromCString("txtEnemyRaceLabel"),
        1, 10, wstr_fromCString("Enemy race:"),
        vec2i(40, 125));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(100, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
        context, wstr_fromCString("txtMapLabel"),
        1, 10, wstr_fromCString("Map:"),
        vec2i(40, 145));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(100, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
        context, wstr_fromCString("txtYourRace"),
        1, 10, wstr_fromCString("Human"),
        vec2i(180, 105));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(50, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
        context, wstr_fromCString("txtEnemyRace"),
        1, 10, wstr_fromCString("Orc"),
        vec2i(180, 125));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(50, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
        context, wstr_fromCString("txtMap"),
        1, 10, wstr_fromCString("147"),
        vec2i(180, 145));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(50, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnYourRaceLeft"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2i(160, 103));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleYourRaceLeft);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnYourRaceRight"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2i(235, 103));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleYourRaceRight);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnEnemyRaceLeft"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2i(160, 123));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleEnemyRaceLeft);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnEnemyRaceRight"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2i(235, 123));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleEnemyRaceRight);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnMapLeft"),
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2i(160, 143));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMapLeft);

    uiEntity = createUIImageButton(
        context, wstr_fromCString("btnMapRight"),
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2i(235, 143));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMapRight);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnCustomGameOk"),
        1, 10, wstr_fromCString("Ok"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2i(100, 165));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleCustomGameOk);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, wstr_fromCString("btnCustomGameCancel"),
        1, 10, wstr_fromCString("Cancel"),
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2i(180, 165));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMenuSinglePlayer);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void showOrHideMainMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuSinglePlayer"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuLoad"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuReplayIntro"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMenuQuit"), status);
}

void showOrHideSinglePlayer(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, wsv_fromCString("btnSinglePlayerOrc"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnSinglePlayerHuman"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnCustomGame"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnSinglePlayerCancel"), status);
}

void showOrHideCustomGame(WarContext* context, bool status)
{
    WarScene* scene = context->scene;

    setUIEntityStatusByName(context, wsv_fromCString("txtYourRaceLabel"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtEnemyRaceLabel"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtMapLabel"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtYourRace"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtEnemyRace"), status);
    setUIEntityStatusByName(context, wsv_fromCString("txtMap"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnYourRaceLeft"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnYourRaceRight"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnEnemyRaceLeft"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnEnemyRaceRight"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMapLeft"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnMapRight"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnCustomGameOk"), status);
    setUIEntityStatusByName(context, wsv_fromCString("btnCustomGameCancel"), status);

    if (status)
    {
        scene->menu.yourRace = WAR_RACE_HUMANS;
        scene->menu.enemyRace = WAR_RACE_ORCS;
        scene->menu.customMap = 147;

        setUIRaceValueByName(context, wsv_fromCString("txtYourRace"), scene->menu.yourRace);
        setUIRaceValueByName(context, wsv_fromCString("txtEnemyRace"), scene->menu.enemyRace);
        setCustomMapValueByName(context, wsv_fromCString("txtMap"), scene->menu.customMap);
    }
}

// menu button handlers
void handleMenuSinglePlayer(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    showOrHideMainMenu(context, false);
    showOrHideSinglePlayer(context, true);
    showOrHideCustomGame(context, false);
}

void handleMenuQuit(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);
    NOT_USED(entity);

    SDL_PushEvent(&(SDL_Event){ .type = SDL_EVENT_QUIT });
}

void handleSinglePlayerOrc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
    scene->briefing.race = WAR_RACE_ORCS;
    scene->briefing.mapType = WAR_CAMPAIGN_ORCS_01;
    setNextScene(context, scene, 1.0f);
}

void handleSinglePlayerHuman(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
    scene->briefing.race = WAR_RACE_HUMANS;
    scene->briefing.mapType = WAR_CAMPAIGN_HUMANS_01;
    setNextScene(context, scene, 1.0f);
}

void handleCustomGame(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    showOrHideMainMenu(context, false);
    showOrHideSinglePlayer(context, false);
    showOrHideCustomGame(context, true);
}

void handleSinglePlayerCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    showOrHideMainMenu(context, true);
    showOrHideSinglePlayer(context, false);
    showOrHideCustomGame(context, false);
}

void handleYourRaceLeft(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.yourRace > WAR_RACE_NEUTRAL)
    {
        scene->menu.yourRace--;
        setUIRaceValueByName(context, wsv_fromCString("txtYourRace"), scene->menu.yourRace);
    }
}

void handleYourRaceRight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.yourRace < WAR_RACE_ORCS)
    {
        scene->menu.yourRace++;
        setUIRaceValueByName(context, wsv_fromCString("txtYourRace"), scene->menu.yourRace);
    }
}

void handleEnemyRaceLeft(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.enemyRace > WAR_RACE_NEUTRAL)
    {
        scene->menu.enemyRace--;
        setUIRaceValueByName(context, wsv_fromCString("txtEnemyRace"), scene->menu.enemyRace);
    }
}

void handleEnemyRaceRight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.enemyRace < WAR_RACE_ORCS)
    {
        scene->menu.enemyRace++;
        setUIRaceValueByName(context, wsv_fromCString("txtEnemyRace"), scene->menu.enemyRace);
    }
}

void handleMapLeft(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.customMap > 147)
    {
        scene->menu.customMap--;
        setCustomMapValueByName(context, wsv_fromCString("txtMap"), scene->menu.customMap);
    }
}

void handleMapRight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.customMap < 188)
    {
        scene->menu.customMap++;
        setCustomMapValueByName(context, wsv_fromCString("txtMap"), scene->menu.customMap);
    }
}

void handleCustomGameOk(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    WarMap* map = createCustomMap(context, scene->menu.customMap, scene->menu.yourRace, scene->menu.enemyRace);
    setNextMap(context, map, 1.0f);
}
