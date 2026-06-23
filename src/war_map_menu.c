#include "war_map_menu.h"

#include "SDL3/SDL.h"
#include "shl/wstr.h"

#include "war_campaigns.h"
#include "war_game.h"
#include "war_map.h"
#include "war_resources.h"
#include "war_ui.h"

void wmm_showOrHideGameOverMenu(WarContext* context, bool status)
{
    WarMap* map = context->map;

    map->menuState = status ? WAR_MENU_STATE_GAME_OVER : WAR_MENU_STATE_NONE;
    map->playing = !status;
}

void wmm_showDemoEndMenu(WarContext* context, bool status)
{
    WarMap* map = context->map;

    map->menuState = status ? WAR_MENU_STATE_DEMO_END : WAR_MENU_STATE_NONE;
    map->playing = !status;
}

// menu button handlers
void wmm_handleMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->menuState = WAR_MENU_STATE_MAIN;
    map->playing = false;
}

void wmm_handleOptions(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    // copy live volume values into settings, then snapshot into settings2
    map->settings.musicVol = (s32)(context->musicVolume * 100);
    map->settings.sfxVol = (s32)(context->soundVolume * 100);
    memcpy(&map->settings2, &map->settings, sizeof(WarMapSettings));

    map->menuState = WAR_MENU_STATE_OPTIONS;
    map->playing = false;
}

void wmm_handleObjectives(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->menuState = WAR_MENU_STATE_OBJECTIVES;
    map->playing = false;
}

void wmm_handleRestart(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->menuState = WAR_MENU_STATE_RESTART;
    map->playing = false;
}

void wmm_handleContinue(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->menuState = WAR_MENU_STATE_NONE;
    map->playing = true;
}

void wmm_handleQuit(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->menuState = WAR_MENU_STATE_QUIT;
    map->playing = false;
}

void wmm_handleGameSpeedDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.gameSpeed > WAR_SPEED_SLOWEST)
        map->settings2.gameSpeed--;
}

void wmm_handleGameSpeedInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.gameSpeed < WAR_SPEED_FASTEST)
        map->settings2.gameSpeed++;
}

void wmm_handleMusicVolDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.musicVol = CLAMP(map->settings2.musicVol - 5, 0, 100);
}

void wmm_handleMusicVolInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.musicVol = CLAMP(map->settings2.musicVol + 5, 0, 100);
}

void wmm_handleSfxVolDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.sfxVol = CLAMP(map->settings2.sfxVol - 5, 0, 100);
}

void wmm_handleSfxVolInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->settings2.sfxVol = CLAMP(map->settings2.sfxVol + 5, 0, 100);
}

void wmm_handleMouseScrollSpeedDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.mouseScrollSpeed > WAR_SPEED_SLOWEST)
        map->settings2.mouseScrollSpeed--;
}

void wmm_handleMouseScrollSpeedInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.mouseScrollSpeed < WAR_SPEED_FASTEST)
        map->settings2.mouseScrollSpeed++;
}

void wmm_handleKeyScrollSpeedDec(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.keyScrollSpeed > WAR_SPEED_SLOWEST)
        map->settings2.keyScrollSpeed--;
}

void wmm_handleKeyScrollSpeedInc(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    if (map->settings2.keyScrollSpeed < WAR_SPEED_FASTEST)
        map->settings2.keyScrollSpeed++;
}

void wmm_handleOptionsOk(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    // persist the changes
    memcpy(&map->settings, &map->settings2, sizeof(WarMapSettings));
    context->musicVolume = (f32)map->settings.musicVol / 100;
    context->soundVolume = (f32)map->settings.sfxVol / 100;

    map->menuState = WAR_MENU_STATE_MAIN;
    map->playing = false;
}

void wmm_handleOptionsCancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->menuState = WAR_MENU_STATE_MAIN;
    map->playing = false;
}

void wmm_handleObjectivesMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->menuState = WAR_MENU_STATE_MAIN;
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

    map->menuState = WAR_MENU_STATE_MAIN;
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

    if (map->custom)
    {
        WarScene* scene = wsc_createScene(context, WAR_SCENE_MAIN_MENU);
        wg_setNextScene(context, scene, 1.0f);
        return;
    }

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

    map->menuState = WAR_MENU_STATE_MAIN;
    map->playing = false;
}

void wmm_handleDemoEndMenu(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarScene* scene = wsc_createScene(context, WAR_SCENE_MAIN_MENU);
    wg_setNextScene(context, scene, 1.0f);
}
