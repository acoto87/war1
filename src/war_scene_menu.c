#include "war_scene_menu.h"

#include "SDL3/SDL.h"
#include "shl/wstr.h"

#include "war_audio.h"
#include "war_game.h"
#include "war_imui.h"
#include "war_map.h"
#include "war_ui.h"

static const char* getCustomGameRaceStr(WarRace value)
{
    switch (value)
    {
        case WAR_RACE_NEUTRAL: return "Random";
        case WAR_RACE_HUMANS:  return "Human";
        case WAR_RACE_ORCS:    return "Orc";
        default:               return "";
    }
}

static const char* getCustomMapStr(s32 value)
{
    switch (value)
    {
        case 147: return "Forest 1";
        case 148: return "Forest 2";
        case 149: return "Forest 3";
        case 150: return "Forest 4";
        case 151: return "Forest 5";
        case 152: return "Forest 6";
        case 153: return "Forest 7";
        case 154: return "Swamp 6";
        case 155: return "Swamp 7";
        case 156: return "Swamp 1";
        case 157: return "Swamp 2";
        case 158: return "Swamp 3";
        case 159: return "Swamp 4";
        case 160: return "Swamp 5";
        case 161: return "Dungeon 1";
        case 162: return "Dungeon 2";
        case 163: return "Dungeon 3";
        case 164: return "Dungeon 4";
        case 165: return "Dungeon 5";
        case 166: return "Dungeon 6";
        case 167: return "Dungeon 7";
        case 168: return "Forest 1.1";
        case 169: return "Forest 2.1";
        case 170: return "Forest 3.1";
        case 171: return "Forest 4.1";
        case 172: return "Forest 5.1";
        case 173: return "Forest 6.1";
        case 174: return "Forest 7.1";
        case 175: return "Swamp 6.1";
        case 176: return "Swamp 7.1";
        case 177: return "Swamp 1.1";
        case 178: return "Swamp 2.1";
        case 179: return "Swamp 3.1";
        case 180: return "Swamp 4.1";
        case 181: return "Swamp 5.1";
        case 182: return "Dungeon 1.1";
        case 183: return "Dungeon 2.1";
        case 184: return "Dungeon 3.1";
        case 185: return "Dungeon 4.1";
        case 186: return "Dungeon 5.1";
        case 187: return "Dungeon 6.1";
        case 188: return "Dungeon 7.1";
        default:  return "";
    }
}

void wsc_enterSceneMainMenu(WarContext* context)
{
    WarScene* scene = context->scene;
    scene->menu.menuPanel = WAR_MAIN_MENU_PANEL_MAIN;

    wcheatp_createCheatsPanel(context);

    if (!isDemo(context))
        wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_MUSIC_00, .loop=true));
}

void wsc_renderSceneMainMenu(WarContext* context)
{
    WarScene* scene = context->scene;

    WarSpriteResourceRef mediumNormalRef  = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);
    WarSpriteResourceRef smallNormalRef   = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef  = imageResourceRef(242);

    // Background is always visible
    imui_image(context, "imgMenuBackground", CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRef(261),
        .position  = VEC2_ZERO,
    ));

    switch (scene->menu.menuPanel)
    {
        case WAR_MAIN_MENU_PANEL_MAIN:
        {
            if (imui_text_button(context, "btnMenuSinglePlayer", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Start a new game"),
                .backgroundNormalRef  = mediumNormalRef,
                .backgroundPressedRef = mediumPressedRef,
                .position             = vec2i(104, 85),
                .hotKey               = WAR_KEY_S,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            )))
            {
                wsc_handleMenuSinglePlayer(context, NULL);
            }

            // Load existing game — not yet implemented; button is inert
            imui_text_button(context, "btnMenuLoad", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Load existing game"),
                .backgroundNormalRef  = mediumNormalRef,
                .backgroundPressedRef = mediumPressedRef,
                .position             = vec2i(104, 105),
                .hotKey               = WAR_KEY_L,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            ));

            // Replay introduction — not yet implemented; button is inert
            imui_text_button(context, "btnMenuReplayIntro", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Replay introduction"),
                .backgroundNormalRef  = mediumNormalRef,
                .backgroundPressedRef = mediumPressedRef,
                .position             = vec2i(104, 125),
                .hotKey               = WAR_KEY_R,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            ));

            if (imui_text_button(context, "btnMenuQuit", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Quit"),
                .backgroundNormalRef  = mediumNormalRef,
                .backgroundPressedRef = mediumPressedRef,
                .position             = vec2i(104, 165),
                .hotKey               = WAR_KEY_Q,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            )))
            {
                wsc_handleMenuQuit(context, NULL);
            }

            break;
        }

        case WAR_MAIN_MENU_PANEL_SINGLE_PLAYER:
        {
            if (imui_text_button(context, "btnSinglePlayerOrc", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Orc campaign"),
                .backgroundNormalRef  = mediumNormalRef,
                .backgroundPressedRef = mediumPressedRef,
                .position             = vec2i(104, 85),
                .hotKey               = WAR_KEY_O,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            )))
            {
                wsc_handleSinglePlayerOrc(context, NULL);
            }

            if (imui_text_button(context, "btnSinglePlayerHuman", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Human campaign"),
                .backgroundNormalRef  = mediumNormalRef,
                .backgroundPressedRef = mediumPressedRef,
                .position             = vec2i(104, 105),
                .hotKey               = WAR_KEY_H,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            )))
            {
                wsc_handleSinglePlayerHuman(context, NULL);
            }

            if (imui_text_button(context, "btnCustomGame", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Custom game"),
                .backgroundNormalRef  = mediumNormalRef,
                .backgroundPressedRef = mediumPressedRef,
                .position             = vec2i(104, 125),
                .hotKey               = WAR_KEY_U,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            )))
            {
                wsc_handleCustomGame(context, NULL);
            }

            if (imui_text_button(context, "btnSinglePlayerCancel", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Cancel"),
                .backgroundNormalRef  = smallNormalRef,
                .backgroundPressedRef = smallPressedRef,
                .position             = vec2i(133, 165),
                .hotKey               = WAR_KEY_C,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            )))
            {
                wsc_handleSinglePlayerCancel(context, NULL);
            }

            break;
        }

        case WAR_MAIN_MENU_PANEL_CUSTOM_GAME:
        {
            WarSpriteResourceRef leftArrowNormalRef   = imageResourceRef(244);
            WarSpriteResourceRef leftArrowPressedRef  = imageResourceRef(245);
            WarSpriteResourceRef rightArrowNormalRef  = imageResourceRef(246);
            WarSpriteResourceRef rightArrowPressedRef = imageResourceRef(247);

            // Static labels
            imui_text(context, "txtYourRaceLabel", CREATE_UI_TEXT_ARGS_INIT(
                .position        = vec2i(40, 105),
                .fontIndex       = 1,
                .boundings       = vec2f(100, 12),
                .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
                .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                .text            = wstr_fromCString("Your race:"),
            ));

            imui_text(context, "txtEnemyRaceLabel", CREATE_UI_TEXT_ARGS_INIT(
                .position        = vec2i(40, 125),
                .fontIndex       = 1,
                .boundings       = vec2f(100, 12),
                .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
                .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                .text            = wstr_fromCString("Enemy race:"),
            ));

            imui_text(context, "txtMapLabel", CREATE_UI_TEXT_ARGS_INIT(
                .position        = vec2i(40, 145),
                .fontIndex       = 1,
                .boundings       = vec2f(100, 12),
                .horizontalAlign = WAR_TEXT_ALIGN_RIGHT,
                .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                .text            = wstr_fromCString("Map:"),
            ));

            // Dynamic values
            imui_text(context, "txtYourRace", CREATE_UI_TEXT_ARGS_INIT(
                .position        = vec2i(180, 105),
                .fontIndex       = 1,
                .boundings       = vec2f(50, 12),
                .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                .text            = wstr_fromCString(getCustomGameRaceStr(scene->menu.yourRace)),
            ));

            imui_text(context, "txtEnemyRace", CREATE_UI_TEXT_ARGS_INIT(
                .position        = vec2i(180, 125),
                .fontIndex       = 1,
                .boundings       = vec2f(50, 12),
                .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                .text            = wstr_fromCString(getCustomGameRaceStr(scene->menu.enemyRace)),
            ));

            imui_text(context, "txtMap", CREATE_UI_TEXT_ARGS_INIT(
                .position        = vec2i(180, 145),
                .fontIndex       = 1,
                .boundings       = vec2f(50, 12),
                .horizontalAlign = WAR_TEXT_ALIGN_CENTER,
                .verticalAlign   = WAR_TEXT_ALIGN_MIDDLE,
                .text            = wstr_fromCString(getCustomMapStr(scene->menu.customMap)),
            ));

            // Your race arrows
            if (imui_image_button(context, "btnYourRaceLeft", CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                .backgroundNormalRef  = leftArrowNormalRef,
                .backgroundPressedRef = leftArrowPressedRef,
                .position             = vec2i(160, 103),
            )))
            {
                wsc_handleYourRaceLeft(context, NULL);
            }

            if (imui_image_button(context, "btnYourRaceRight", CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                .backgroundNormalRef  = rightArrowNormalRef,
                .backgroundPressedRef = rightArrowPressedRef,
                .position             = vec2i(235, 103),
            )))
            {
                wsc_handleYourRaceRight(context, NULL);
            }

            // Enemy race arrows
            if (imui_image_button(context, "btnEnemyRaceLeft", CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                .backgroundNormalRef  = leftArrowNormalRef,
                .backgroundPressedRef = leftArrowPressedRef,
                .position             = vec2i(160, 123),
            )))
            {
                wsc_handleEnemyRaceLeft(context, NULL);
            }

            if (imui_image_button(context, "btnEnemyRaceRight", CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                .backgroundNormalRef  = rightArrowNormalRef,
                .backgroundPressedRef = rightArrowPressedRef,
                .position             = vec2i(235, 123),
            )))
            {
                wsc_handleEnemyRaceRight(context, NULL);
            }

            // Map arrows
            if (imui_image_button(context, "btnMapLeft", CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                .backgroundNormalRef  = leftArrowNormalRef,
                .backgroundPressedRef = leftArrowPressedRef,
                .position             = vec2i(160, 143),
            )))
            {
                wsc_handleMapLeft(context, NULL);
            }

            if (imui_image_button(context, "btnMapRight", CREATE_UI_IMAGE_BUTTON_ARGS_INIT(
                .backgroundNormalRef  = rightArrowNormalRef,
                .backgroundPressedRef = rightArrowPressedRef,
                .position             = vec2i(235, 143),
            )))
            {
                wsc_handleMapRight(context, NULL);
            }

            // Ok / Cancel
            if (imui_text_button(context, "btnCustomGameOk", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Ok"),
                .backgroundNormalRef  = smallNormalRef,
                .backgroundPressedRef = smallPressedRef,
                .position             = vec2i(100, 165),
                .hotKey               = WAR_KEY_O,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            )))
            {
                wsc_handleCustomGameOk(context, NULL);
            }

            if (imui_text_button(context, "btnCustomGameCancel", CREATE_UI_TEXT_BUTTON_ARGS_INIT(
                .fontIndex            = 1,
                .text                 = wstr_fromCString("Cancel"),
                .backgroundNormalRef  = smallNormalRef,
                .backgroundPressedRef = smallPressedRef,
                .position             = vec2i(180, 165),
                .hotKey               = WAR_KEY_C,
                .highlightIndex       = 0,
                .highlightCount       = 1,
            )))
            {
                // Cancel from custom game goes back to single player panel
                wsc_handleMenuSinglePlayer(context, NULL);
            }

            break;
        }

        default:
            break;
    }
}

// menu button handlers
void wsc_handleMenuSinglePlayer(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    context->scene->menu.menuPanel = WAR_MAIN_MENU_PANEL_SINGLE_PLAYER;
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

    WarScene* scene = context->scene;
    scene->menu.menuPanel  = WAR_MAIN_MENU_PANEL_CUSTOM_GAME;
    scene->menu.yourRace   = WAR_RACE_HUMANS;
    scene->menu.enemyRace  = WAR_RACE_ORCS;
    scene->menu.customMap  = 147;
}

void wsc_handleSinglePlayerCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    context->scene->menu.menuPanel = WAR_MAIN_MENU_PANEL_MAIN;
}

void wsc_handleYourRaceLeft(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.yourRace > WAR_RACE_NEUTRAL)
        scene->menu.yourRace--;
}

void wsc_handleYourRaceRight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.yourRace < WAR_RACE_ORCS)
        scene->menu.yourRace++;
}

void wsc_handleEnemyRaceLeft(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.enemyRace > WAR_RACE_NEUTRAL)
        scene->menu.enemyRace--;
}

void wsc_handleEnemyRaceRight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.enemyRace < WAR_RACE_ORCS)
        scene->menu.enemyRace++;
}

void wsc_handleMapLeft(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.customMap > 147)
        scene->menu.customMap--;
}

void wsc_handleMapRight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    if (scene->menu.customMap < 188)
        scene->menu.customMap++;
}

void wsc_handleCustomGameOk(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = context->scene;

    WarMap* map = createCustomMap(context, scene->menu.customMap, scene->menu.yourRace, scene->menu.enemyRace);
    wg_setNextMap(context, map, 1.0f);
}
