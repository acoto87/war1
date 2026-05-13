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

static void wsm_setUIRaceValueByName(WarContext* context, StringView name, WarRace value)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        wui_setUIText(context, entity, getCustomGameRaceStr(value));
        setUITextHighlight(context, entity, NO_HIGHLIGHT, 0);
    }
}

static void wsm_setCustomMapValueByName(WarContext* context, StringView name, s32 value)
{
    WarEntity* entity = we_findUIEntity(context, name);
    if (entity)
    {
        wui_setUIText(context, entity, getCustomMapStr(value));
        setUITextHighlight(context, entity, NO_HIGHLIGHT, 0);
    }
}

void wsc_enterSceneMainMenu(WarContext* context)
{
    wsc_createMainMenu(context);
    wsc_createSinglePlayerMenu(context);
    wsc_createLoadMenu(context);
    wsc_createCustomGameMenu(context);
    wcheatp_createCheatsPanel(context);

    wui_createUICursor(context, wstr_fromCString("cursor"), CREATE_UI_CURSOR_ARGS_INIT(
        .type     = WAR_CURSOR_ARROW,
        .position = VEC2_ZERO,
    ));

    if (!isDemo(context))
        wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_MUSIC_00, .loop=true));
}

void wsc_createMainMenu(WarContext* context)
{
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);

    wui_createUIImage(context, wstr_fromCString("imgMenuBackground"), CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRef(261),
        .position  = VEC2_ZERO,
    ));

    wui_createUITextButton(context, wstr_fromCString("btnMenuSinglePlayer"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Start a new game"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2i(104, 85),
        .hotKey               = WAR_KEY_S,
        .clickHandler         = wsc_handleMenuSinglePlayer,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));

    wui_createUITextButton(context, wstr_fromCString("btnMenuLoad"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Load existing game"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2i(104, 105),
        .hotKey               = WAR_KEY_L,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));

    wui_createUITextButton(context, wstr_fromCString("btnMenuReplayIntro"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Replay introduction"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2i(104, 125),
        .hotKey               = WAR_KEY_R,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));

    wui_createUITextButton(context, wstr_fromCString("btnMenuQuit"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Quit"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2i(104, 165),
        .hotKey               = WAR_KEY_Q,
        .clickHandler         = wsc_handleMenuQuit,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
}

void wsc_createSinglePlayerMenu(WarContext* context)
{
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnSinglePlayerOrc"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Orc campaign"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2i(104, 85),
        .hotKey               = WAR_KEY_O,
        .clickHandler         = wsc_handleSinglePlayerOrc,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnSinglePlayerHuman"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Human campaign"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2i(104, 105),
        .hotKey               = WAR_KEY_H,
        .clickHandler         = wsc_handleSinglePlayerHuman,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnCustomGame"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Custom game"),
        .backgroundNormalRef  = mediumNormalRef,
        .backgroundPressedRef = mediumPressedRef,
        .position             = vec2i(104, 125),
        .hotKey               = WAR_KEY_U,
        .clickHandler         = wsc_handleCustomGame,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnSinglePlayerCancel"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Cancel"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2i(133, 165),
        .hotKey               = WAR_KEY_C,
        .clickHandler         = wsc_handleSinglePlayerCancel,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);
}

void wsc_createLoadMenu(WarContext* context)
{
    NOT_USED(context);
}

void wsc_createCustomGameMenu(WarContext* context)
{
    WarEntity* uiEntity;
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);
    WarSpriteResourceRef leftArrowNormalRef = imageResourceRef(244);
    WarSpriteResourceRef leftArrowPressedRef = imageResourceRef(245);
    WarSpriteResourceRef rightArrowNormalRef = imageResourceRef(246);
    WarSpriteResourceRef rightArrowPressedRef = imageResourceRef(247);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtYourRaceLabel"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2i(40, 105),
        .fontIndex       = 1,
        .boundings       = vec2f(100, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Your race:"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtEnemyRaceLabel"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2i(40, 125),
        .fontIndex       = 1,
        .boundings       = vec2f(100, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Enemy race:"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtMapLabel"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2i(40, 145),
        .fontIndex       = 1,
        .boundings       = vec2f(100, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Map:"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtYourRace"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2i(180, 105),
        .fontIndex       = 1,
        .boundings       = vec2f(50, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Human"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtEnemyRace"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2i(180, 125),
        .fontIndex       = 1,
        .boundings       = vec2f(50, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("Orc"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtMap"), CREATE_UI_TEXT_ARGS_INIT(
        .position        = vec2i(180, 145),
        .fontIndex       = 1,
        .boundings       = vec2f(50, 12),
        .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
        .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
        .text            = wstr_fromCString("147"),
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnYourRaceLeft"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = leftArrowNormalRef,
        .backgroundPressedRef = leftArrowPressedRef,
        .position             = vec2i(160, 103),
        .clickHandler         = wsc_handleYourRaceLeft
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnYourRaceRight"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = rightArrowNormalRef,
        .backgroundPressedRef = rightArrowPressedRef,
        .position             = vec2i(235, 103),
        .clickHandler         = wsc_handleYourRaceRight
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnEnemyRaceLeft"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = leftArrowNormalRef,
        .backgroundPressedRef = leftArrowPressedRef,
        .position             = vec2i(160, 123),
        .clickHandler         = wsc_handleEnemyRaceLeft,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnEnemyRaceRight"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = rightArrowNormalRef,
        .backgroundPressedRef = rightArrowPressedRef,
        .position             = vec2i(235, 123),
        .clickHandler         = wsc_handleEnemyRaceRight,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnMapLeft"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = leftArrowNormalRef,
        .backgroundPressedRef = leftArrowPressedRef,
        .position             = vec2i(160, 143),
        .clickHandler         = wsc_handleMapLeft,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnMapRight"), CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
        .backgroundNormalRef  = rightArrowNormalRef,
        .backgroundPressedRef = rightArrowPressedRef,
        .position             = vec2i(235, 143),
        .clickHandler         = wsc_handleMapRight,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnCustomGameOk"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Ok"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2i(100, 165),
        .hotKey               = WAR_KEY_O,
        .clickHandler         = wsc_handleCustomGameOk,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);

    uiEntity = wui_createUITextButton(context, wstr_fromCString("btnCustomGameCancel"), CREATE_UI_TEXT_BUTTON_ARGS_INIT(
        .fontIndex            = 1,
        .text                 = wstr_fromCString("Cancel"),
        .backgroundNormalRef  = smallNormalRef,
        .backgroundPressedRef = smallPressedRef,
        .position             = vec2i(180, 165),
        .hotKey               = WAR_KEY_C,
        .clickHandler         = wsc_handleMenuSinglePlayer,
        .highlightIndex       = 0,
        .highlightCount       = 1,
    ));
    setUIEntityStatus(context, uiEntity, false);
}

void wsc_showOrHideMainMenu(WarContext* context, bool status)
{
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuSinglePlayer"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuLoad"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuReplayIntro"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMenuQuit"), status);
}

void wsc_showOrHideSinglePlayer(WarContext* context, bool status)
{
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnSinglePlayerOrc"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnSinglePlayerHuman"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnCustomGame"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnSinglePlayerCancel"), status);
}

void wsc_showOrHideCustomGame(WarContext* context, bool status)
{
    WarScene* scene = context->scene;

    wui_setUIEntityStatusByName(context, wsv_fromCString("txtYourRaceLabel"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtEnemyRaceLabel"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtMapLabel"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtYourRace"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtEnemyRace"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("txtMap"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnYourRaceLeft"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnYourRaceRight"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnEnemyRaceLeft"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnEnemyRaceRight"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMapLeft"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnMapRight"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnCustomGameOk"), status);
    wui_setUIEntityStatusByName(context, wsv_fromCString("btnCustomGameCancel"), status);

    if (status)
    {
        scene->menu.yourRace = WAR_RACE_HUMANS;
        scene->menu.enemyRace = WAR_RACE_ORCS;
        scene->menu.customMap = 147;

        wsm_setUIRaceValueByName(context, wsv_fromCString("txtYourRace"), scene->menu.yourRace);
        wsm_setUIRaceValueByName(context, wsv_fromCString("txtEnemyRace"), scene->menu.enemyRace);
        wsm_setCustomMapValueByName(context, wsv_fromCString("txtMap"), scene->menu.customMap);
    }
}

// menu button handlers
void wsc_handleMenuSinglePlayer(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    wsc_showOrHideMainMenu(context, false);
    wsc_showOrHideSinglePlayer(context, true);
    wsc_showOrHideCustomGame(context, false);
}

void wsc_handleMenuQuit(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);
    NOT_USED(entity);

    SDL_PushEvent(&(SDL_Event){ .type = SDL_EVENT_QUIT });
}

void wsc_handleSinglePlayerOrc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = wsc_createScene(context, WAR_SCENE_BRIEFING);
    scene->briefing.race = WAR_RACE_ORCS;
    scene->briefing.mapType = WAR_CAMPAIGN_ORCS_01;
    wg_setNextScene(context, scene, 1.0f);
}

void wsc_handleSinglePlayerHuman(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = wsc_createScene(context, WAR_SCENE_BRIEFING);
    scene->briefing.race = WAR_RACE_HUMANS;
    scene->briefing.mapType = WAR_CAMPAIGN_HUMANS_01;
    wg_setNextScene(context, scene, 1.0f);
}

void wsc_handleCustomGame(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    wsc_showOrHideMainMenu(context, false);
    wsc_showOrHideSinglePlayer(context, false);
    wsc_showOrHideCustomGame(context, true);
}

void wsc_handleSinglePlayerCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    wsc_showOrHideMainMenu(context, true);
    wsc_showOrHideSinglePlayer(context, false);
    wsc_showOrHideCustomGame(context, false);
}

void wsc_handleYourRaceLeft(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.yourRace > WAR_RACE_NEUTRAL)
    {
        scene->menu.yourRace--;
        wsm_setUIRaceValueByName(context, wsv_fromCString("txtYourRace"), scene->menu.yourRace);
    }
}

void wsc_handleYourRaceRight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.yourRace < WAR_RACE_ORCS)
    {
        scene->menu.yourRace++;
        wsm_setUIRaceValueByName(context, wsv_fromCString("txtYourRace"), scene->menu.yourRace);
    }
}

void wsc_handleEnemyRaceLeft(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.enemyRace > WAR_RACE_NEUTRAL)
    {
        scene->menu.enemyRace--;
        wsm_setUIRaceValueByName(context, wsv_fromCString("txtEnemyRace"), scene->menu.enemyRace);
    }
}

void wsc_handleEnemyRaceRight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.enemyRace < WAR_RACE_ORCS)
    {
        scene->menu.enemyRace++;
        wsm_setUIRaceValueByName(context, wsv_fromCString("txtEnemyRace"), scene->menu.enemyRace);
    }
}

void wsc_handleMapLeft(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.customMap > 147)
    {
        scene->menu.customMap--;
        wsm_setCustomMapValueByName(context, wsv_fromCString("txtMap"), scene->menu.customMap);
    }
}

void wsc_handleMapRight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.customMap < 188)
    {
        scene->menu.customMap++;
        wsm_setCustomMapValueByName(context, wsv_fromCString("txtMap"), scene->menu.customMap);
    }
}

void wsc_handleCustomGameOk(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    WarMap* map = createCustomMap(context, scene->menu.customMap, scene->menu.yourRace, scene->menu.enemyRace);
    wg_setNextMap(context, map, 1.0f);
}
