#include "war_game.h"

#include <assert.h>
#include <math.h>
#include <string.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#include <io.h>
#ifndef F_OK
#define F_OK 0
#endif
#define access _access
#else
#include <unistd.h>
#endif

#include "shl/memzone.h"
#include "shl/wstr.h"
#include "TracyC.h"

#include "war_alloc.h"
#include "war_actions.h"
#include "war_audio.h"
#include "war_file.h"
#include "war_font.h"
#include "war.h"
#include "war_imui.h"
#include "war_map.h"
#include "war_resources.h"
#include "war_scenes.h"

static WarKeys getWarKeyFromSDLKey(SDL_Keycode key)
{
    switch (key)
    {
        case SDLK_SPACE: return WAR_KEY_SPACE;
        case SDLK_APOSTROPHE: return WAR_KEY_APOSTROPHE;
        case SDLK_COMMA: return WAR_KEY_COMMA;
        case SDLK_MINUS: return WAR_KEY_MINUS;
        case SDLK_PERIOD: return WAR_KEY_PERIOD;
        case SDLK_SLASH: return WAR_KEY_SLASH;
        case SDLK_0: return WAR_KEY_0;
        case SDLK_1: return WAR_KEY_1;
        case SDLK_2: return WAR_KEY_2;
        case SDLK_3: return WAR_KEY_3;
        case SDLK_4: return WAR_KEY_4;
        case SDLK_5: return WAR_KEY_5;
        case SDLK_6: return WAR_KEY_6;
        case SDLK_7: return WAR_KEY_7;
        case SDLK_8: return WAR_KEY_8;
        case SDLK_9: return WAR_KEY_9;
        case SDLK_SEMICOLON: return WAR_KEY_SEMICOLON;
        case SDLK_EQUALS: return WAR_KEY_EQUAL;
        case SDLK_A: return WAR_KEY_A;
        case SDLK_B: return WAR_KEY_B;
        case SDLK_C: return WAR_KEY_C;
        case SDLK_D: return WAR_KEY_D;
        case SDLK_E: return WAR_KEY_E;
        case SDLK_F: return WAR_KEY_F;
        case SDLK_G: return WAR_KEY_G;
        case SDLK_H: return WAR_KEY_H;
        case SDLK_I: return WAR_KEY_I;
        case SDLK_J: return WAR_KEY_J;
        case SDLK_K: return WAR_KEY_K;
        case SDLK_L: return WAR_KEY_L;
        case SDLK_M: return WAR_KEY_M;
        case SDLK_N: return WAR_KEY_N;
        case SDLK_O: return WAR_KEY_O;
        case SDLK_P: return WAR_KEY_P;
        case SDLK_Q: return WAR_KEY_Q;
        case SDLK_R: return WAR_KEY_R;
        case SDLK_S: return WAR_KEY_S;
        case SDLK_T: return WAR_KEY_T;
        case SDLK_U: return WAR_KEY_U;
        case SDLK_V: return WAR_KEY_V;
        case SDLK_W: return WAR_KEY_W;
        case SDLK_X: return WAR_KEY_X;
        case SDLK_Y: return WAR_KEY_Y;
        case SDLK_Z: return WAR_KEY_Z;
        case SDLK_LEFTBRACKET: return WAR_KEY_LEFT_BRACKET;
        case SDLK_BACKSLASH: return WAR_KEY_BACKSLASH;
        case SDLK_RIGHTBRACKET: return WAR_KEY_RIGHT_BRACKET;
        case SDLK_GRAVE: return WAR_KEY_GRAVE_ACCENT;
        case SDLK_ESCAPE: return WAR_KEY_ESC;
        case SDLK_RETURN: return WAR_KEY_ENTER;
        case SDLK_KP_ENTER: return WAR_KEY_ENTER;
        case SDLK_TAB: return WAR_KEY_TAB;
        case SDLK_BACKSPACE: return WAR_KEY_BACKSPACE;
        case SDLK_INSERT: return WAR_KEY_INSERT;
        case SDLK_DELETE: return WAR_KEY_DELETE;
        case SDLK_RIGHT: return WAR_KEY_RIGHT;
        case SDLK_LEFT: return WAR_KEY_LEFT;
        case SDLK_DOWN: return WAR_KEY_DOWN;
        case SDLK_UP: return WAR_KEY_UP;
        case SDLK_PAGEUP: return WAR_KEY_PAGE_UP;
        case SDLK_PAGEDOWN: return WAR_KEY_PAGE_DOWN;
        case SDLK_HOME: return WAR_KEY_HOME;
        case SDLK_END: return WAR_KEY_END;
        case SDLK_F1: return WAR_KEY_F1;
        case SDLK_F2: return WAR_KEY_F2;
        case SDLK_F3: return WAR_KEY_F3;
        case SDLK_F4: return WAR_KEY_F4;
        case SDLK_F5: return WAR_KEY_F5;
        case SDLK_F6: return WAR_KEY_F6;
        case SDLK_F7: return WAR_KEY_F7;
        case SDLK_F8: return WAR_KEY_F8;
        case SDLK_F9: return WAR_KEY_F9;
        case SDLK_F10: return WAR_KEY_F10;
        case SDLK_F11: return WAR_KEY_F11;
        case SDLK_F12: return WAR_KEY_F12;
        case SDLK_KP_MULTIPLY: return WAR_KEY_ASTERISK;
        case SDLK_KP_PLUS: return WAR_KEY_PLUS;
        case SDLK_KP_0: return WAR_KEY_0;
        case SDLK_KP_1: return WAR_KEY_1;
        case SDLK_KP_2: return WAR_KEY_2;
        case SDLK_KP_3: return WAR_KEY_3;
        case SDLK_KP_4: return WAR_KEY_4;
        case SDLK_KP_5: return WAR_KEY_5;
        case SDLK_KP_6: return WAR_KEY_6;
        case SDLK_KP_7: return WAR_KEY_7;
        case SDLK_KP_8: return WAR_KEY_8;
        case SDLK_KP_9: return WAR_KEY_9;
        case SDLK_KP_MINUS: return WAR_KEY_MINUS;
        case SDLK_KP_PERIOD: return WAR_KEY_PERIOD;
        case SDLK_KP_DIVIDE: return WAR_KEY_SLASH;
        case SDLK_KP_EQUALS: return WAR_KEY_EQUAL;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT: return WAR_KEY_SHIFT;
        case SDLK_LCTRL:
        case SDLK_RCTRL: return WAR_KEY_CTRL;
        case SDLK_LALT:
        case SDLK_RALT: return WAR_KEY_ALT;
        default: return WAR_KEY_NONE;
    }
}

static bool parseLaunchRace(StringView value, WarRace* race)
{
    if (wsv_equalsIgnoreCase(value, WSV_LITERAL("human")))
    {
        *race = WAR_RACE_HUMANS;
        return true;
    }
    if (wsv_equalsIgnoreCase(value, WSV_LITERAL("orc")))
    {
        *race = WAR_RACE_ORCS;
        return true;
    }
    if (wsv_equalsIgnoreCase(value, WSV_LITERAL("random")))
    {
        *race = WAR_RACE_NEUTRAL;
        return true;
    }
    return false;
}

static bool parseDecimalU64(StringView value, u64* result)
{
    if (value.length == 0)
    {
        return false;
    }

    u64 parsed = 0;
    for (size_t i = 0; i < value.length; i++)
    {
        char c = value.data[i];
        if (c < '0' || c > '9')
        {
            return false;
        }

        u64 digit = (u64)(c - '0');
        if (parsed > (UINT64_MAX - digit) / 10)
        {
            return false;
        }
        parsed = parsed * 10 + digit;
    }

    *result = parsed;
    return true;
}

static bool parseDecimalS32(StringView value, s32* result)
{
    u64 parsed = 0;
    if (!parseDecimalU64(value, &parsed) || parsed > INT32_MAX)
    {
        return false;
    }
    *result = (s32)parsed;
    return true;
}

bool wg_parseCommandLine(WarContext* context, int argc, char** argv)
{
    assert(context);

    memset(&context->launch, 0, sizeof(context->launch));
    context->launch.campaignRace = WAR_RACE_NEUTRAL;
    context->launch.customGame.playerRace = WAR_RACE_NEUTRAL;
    context->launch.customGame.enemyRace = WAR_RACE_NEUTRAL;
    context->launch.customGame.startConfigurationIndex = -1;

    bool missionSet = false;
    bool customMission = false;
    bool mapSet = false;
    bool raceSet = false;
    bool enemyRaceSet = false;
    bool goldSet = false;
    bool woodSet = false;
    bool seedSet = false;

    for (s32 i = 1; i < argc; i++)
    {
        StringView arg = wsv_fromCString(argv[i]);
        if (wsv_equals(arg, WSV_LITERAL("--skip-intro")))
        {
            context->launch.skipIntro = true;
        }
        else if (wsv_equals(arg, WSV_LITERAL("--race")))
        {
            if (++i >= argc || !parseLaunchRace(wsv_fromCString(argv[i]), &context->launch.campaignRace))
            {
                logError("--race requires human, orc, or random.");
                return false;
            }
            context->launch.customGame.playerRace = context->launch.campaignRace;
            raceSet = true;
        }
        else if (wsv_equals(arg, WSV_LITERAL("--enemy-race")))
        {
            if (++i >= argc || !parseLaunchRace(wsv_fromCString(argv[i]), &context->launch.customGame.enemyRace))
            {
                logError("--enemy-race requires human, orc, or random.");
                return false;
            }
            enemyRaceSet = true;
        }
        else if (wsv_equals(arg, WSV_LITERAL("--mission")))
        {
            if (++i >= argc)
            {
                logError("Missing value after --mission.");
                return false;
            }

            StringView value = wsv_fromCString(argv[i]);
            customMission = false;
            if (wsv_equalsIgnoreCase(value, WSV_LITERAL("custom")))
            {
                customMission = true;
            }
            else if (!parseDecimalS32(value, &context->launch.campaignMission) ||
                     context->launch.campaignMission < 1 || context->launch.campaignMission > 12)
            {
                logError("--mission requires custom or a number from 1 to 12.");
                return false;
            }
            missionSet = true;
        }
        else if (wsv_equals(arg, WSV_LITERAL("--map")) || wsv_equals(arg, WSV_LITERAL("-m")))
        {
            if (++i >= argc)
            {
                logError("Missing value after %.*s.", (s32)arg.length, arg.data);
                return false;
            }
            if (SDL_strlen(argv[i]) >= sizeof(context->launch.mapValue))
            {
                logError("Map value is too long.");
                return false;
            }
            SDL_strlcpy(context->launch.mapValue, argv[i], sizeof(context->launch.mapValue));
            mapSet = true;
        }
        else if (wsv_equals(arg, WSV_LITERAL("--gold")) || wsv_equals(arg, WSV_LITERAL("--wood")))
        {
            s32 value = 0;
            if (++i >= argc || !parseDecimalS32(wsv_fromCString(argv[i]), &value))
            {
                logError("%.*s requires a non-negative integer.", (s32)arg.length, arg.data);
                return false;
            }

            if (wsv_equals(arg, WSV_LITERAL("--gold")))
            {
                context->launch.customGame.hasGold = true;
                context->launch.customGame.gold = value;
                goldSet = true;
            }
            else
            {
                context->launch.customGame.hasWood = true;
                context->launch.customGame.wood = value;
                woodSet = true;
            }
        }
        else if (wsv_equals(arg, WSV_LITERAL("--seed")))
        {
            u64 value = 0;
            if (++i >= argc || !parseDecimalU64(wsv_fromCString(argv[i]), &value))
            {
                logError("--seed requires a non-negative integer.");
                return false;
            }
            context->launch.customGame.hasSeed = true;
            context->launch.customGame.seed = value;
            seedSet = true;
        }
        else
        {
            logError("Unknown command-line option: %.*s", (s32)arg.length, arg.data);
            return false;
        }
    }

    if (missionSet && customMission)
    {
        if (!mapSet)
        {
            logError("--mission custom requires --map <predefined-name>.");
            return false;
        }
        if (!wres_tryGetPredefinedCustomMapIndex(
                wsv_fromCString(context->launch.mapValue), &context->launch.customMapIndex))
        {
            logError("Unknown predefined custom map: %s", context->launch.mapValue);
            return false;
        }

        context->launch.mode = WAR_LAUNCH_PREDEFINED_CUSTOM;
        context->launch.customGame.enabled = true;
        return true;
    }

    if (missionSet)
    {
        if (mapSet || enemyRaceSet || goldSet || woodSet || seedSet)
        {
            logError("Campaign missions do not accept --map, --enemy-race, --gold, --wood, or --seed.");
            return false;
        }
        context->launch.mode = WAR_LAUNCH_CAMPAIGN;
        return true;
    }

    if (mapSet)
    {
        if (raceSet || enemyRaceSet || goldSet || woodSet || seedSet)
        {
            logError("External map files do not accept race, resource, or seed overrides.");
            return false;
        }
        context->launch.mode = WAR_LAUNCH_MAP_FILE;
        return true;
    }

    if (raceSet || enemyRaceSet || goldSet || woodSet || seedSet)
    {
        logError("Race, resource, and seed options require --mission custom or a campaign mission.");
        return false;
    }

    return true;
}

bool wg_setStartupDestination(WarContext* context)
{
    WarLaunchConfig* launch = &context->launch;
    switch (launch->mode)
    {
        case WAR_LAUNCH_MAP_FILE:
        {
            WarMap* map = wmap_loadCustomMap(context, wsv_fromCString(launch->mapValue));
            if (!map)
            {
                logError("Could not load custom map file: %s", launch->mapValue);
                return false;
            }
            wg_setNextMap(context, map, 0.0f);
            return true;
        }

        case WAR_LAUNCH_CAMPAIGN:
        {
            WarRace race = launch->campaignRace;
            if (race == WAR_RACE_NEUTRAL)
            {
                race = SDL_rand(2) == 0 ? WAR_RACE_HUMANS : WAR_RACE_ORCS;
            }

            WarCampaignMapType mapType;
            if (!wcamp_tryGetMapType(race, launch->campaignMission, &mapType))
            {
                logError("Invalid campaign selection: race=%d mission=%d", race, launch->campaignMission);
                return false;
            }

            WarResource* levelInfo = wres_getOrCreateResource(context, mapType);
            if (levelInfo->type != WAR_RESOURCE_TYPE_LEVEL_INFO)
            {
                logError("Campaign mission is unavailable in this DATA.WAR: race=%s mission=%d",
                         race == WAR_RACE_HUMANS ? "human" : "orc", launch->campaignMission);
                return false;
            }

            launch->campaignRace = race;
            WarMap* map = wmap_createMap(context, mapType);
            wg_setNextMap(context, map, 0.0f);
            return true;
        }

        case WAR_LAUNCH_PREDEFINED_CUSTOM:
        {
            WarResource* levelInfo = wres_getOrCreateResource(context, launch->customMapIndex);
            if (levelInfo->type != WAR_RESOURCE_TYPE_LEVEL_INFO || !levelInfo->levelInfo.customMap)
            {
                logError("Predefined custom map is unavailable in this DATA.WAR: %s", launch->mapValue);
                return false;
            }

            WarMap* map = wmap_createCustomMap(context, launch->customMapIndex, &launch->customGame);
            launch->customGame = map->customGame;
            wg_setNextMap(context, map, 0.0f);
            return true;
        }

        case WAR_LAUNCH_DEFAULT:
        {
            WarSceneType sceneType = launch->skipIntro ? WAR_SCENE_MAIN_MENU : WAR_SCENE_BLIZZARD;
            WarScene* scene = wsc_createScene(context, sceneType);
            wg_setNextScene(context, scene, 0.0f);
            return true;
        }
    }

    logError("Unknown launch mode: %d", launch->mode);
    return false;
}

static void appendCheatTextInput(WarContext* context, StringView text)
{
    WarScene* scene = context->scene;
    WarMap* map = context->map;
    assert(scene || map);

    WarCheatStatus* cheatStatus = scene
        ? &scene->cheatStatus : &map->status.cheatStatus;

    if (!cheatStatus->enabled || !cheatStatus->visible)
    {
        return;
    }

    const char* cursor = wsv_data(text);
    size_t remaining = text.length;
    while (remaining > 0)
    {
        Uint32 codepoint = SDL_StepUTF8(&cursor, &remaining);
        if (codepoint >= 32 && codepoint <= 126)
        {
            s32 length = (s32)cheatStatus->text.length;
            if (length + 1 < CHEAT_TEXT_MAX_LENGTH)
            {
                char c = (char)codepoint;
                wstr_insert(&cheatStatus->text, cheatStatus->position, wsv_fromParts(&c, 1));
                cheatStatus->position++;
            }
        }
    }
}

static void setWindowIcon(SDL_Window* window) {
    // Load the PNG file. We force 4 channels (RGBA) because SDL prefers it.
    int width, height, channels;
    u8* pixels = stbi_load("war1.png", &width, &height, &channels, 4);
    if (!pixels) {
        logWarning("Failed to load icon with stb_image: %s", stbi_failure_reason());
        return;
    }

    // Calculate the pitch (bytes per row): width * 4 bytes (RGBA)
    int pitch = width * 4;

    // Create a surface from the raw pixel data
    // In SDL3, we use SDL_PIXELFORMAT_RGBA32 for standard 32-bit pixel arrays
    SDL_Surface* iconSurface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, pixels, pitch);
    if (iconSurface) {
        SDL_SetWindowIcon(window, iconSurface);
        SDL_DestroySurface(iconSurface);
    } else {
        logWarning("Failed to create SDL surface for icon: %s", SDL_GetError());
    }

    stbi_image_free(pixels);
}

static void updateMetricS32(WarMetricS32* metric, s32 newSample, s32 index)
{
    metric->sampleSum -= metric->sample[index];
    metric->sample[index] = newSample;
    metric->sampleSum += metric->sample[index];
    if (metric->sampleCount < arrayLength(metric->sample))
    {
        metric->sampleCount++;
    }
    metric->avg = metric->sampleCount > 0 ? metric->sampleSum / metric->sampleCount : 0;
    metric->last = newSample;
}

static void updateMetricU64(WarMetricU64* metric, u64 newSample, s32 index)
{
    metric->sampleSum -= metric->sample[index];
    metric->sample[index] = newSample;
    metric->sampleSum += metric->sample[index];
    if (metric->sampleCount < arrayLength(metric->sample))
    {
        metric->sampleCount++;
    }
    metric->avg = metric->sampleCount > 0 ? metric->sampleSum / (u64)metric->sampleCount : 0;
    metric->last = newSample;
}

static void cpuRelax(void)
{
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    _mm_pause();
#elif defined(__i386__) || defined(__x86_64__)
    __builtin_ia32_pause();
#else
    SDL_Delay(0);
#endif
}

static u64 waitUntilFrameDeadlineNs(u64 targetNs)
{
    u64 now = SDL_GetTicksNS();
    if (now >= targetNs)
    {
        return now;
    }

    u64 remainingNs = targetNs - now;
    const u64 spinThresholdNs = 2 * 1000 * 1000;

    if (remainingNs > spinThresholdNs)
    {
        u64 sleepNs = remainingNs - spinThresholdNs;
        SDL_Delay((u32)(sleepNs / 1000000));
    }

    do
    {
        cpuRelax();
        now = SDL_GetTicksNS();
    } while (now < targetNs);

    return now;
}

void wg_beginFrame(WarContext* context)
{
    u64 currentFrameStartNs = SDL_GetTicksNS();

    if (context->lastFrameStartNs == 0)
    {
        context->lastFrameStartNs = currentFrameStartNs;
    }

    context->frameStartNs = currentFrameStartNs;

    u64 frameDeltaNs = currentFrameStartNs - context->lastFrameStartNs;
    if (frameDeltaNs == 0)
    {
        frameDeltaNs = 1;
    }

    context->realDeltaTime = (f32)ns2s(frameDeltaNs);
    context->realTime += context->realDeltaTime;

    f32 gameSpeedScale = context->globalSpeed;

    if (context->map)
    {
        WarMap* map = context->map;

        f32 speedScale = 1.0f;

        if (map->settings.gameSpeed < WAR_SPEED_NORMAL)
        {
            speedScale = 1.0f - (WAR_SPEED_NORMAL - map->settings.gameSpeed) * 0.25f;
        }
        else if (map->settings.gameSpeed > WAR_SPEED_NORMAL)
        {
            speedScale = 1.0f + (map->settings.gameSpeed - WAR_SPEED_NORMAL) * 0.5f;
        }

        gameSpeedScale *= speedScale;
    }

    if (context->paused)
    {
        context->gameDeltaTime = 0.0f;
    }
    else
    {
        context->gameDeltaTime = context->realDeltaTime * gameSpeedScale;
        context->gameTime += context->gameDeltaTime;
    }

    context->lastFrameStartNs = currentFrameStartNs;
}

bool wg_initGame(WarContext* context)
{
    context->globalScale = 3;
    context->globalSpeed = 1;
    context->originalWindowWidth = 320;
    context->originalWindowHeight = 200;
    context->windowWidth = (s32)(context->originalWindowWidth * context->globalScale);
    context->windowHeight = (s32)(context->originalWindowHeight * context->globalScale);
    wstr_assignCString(&context->windowTitle, "War 1");
    context->window = SDL_CreateWindow(wstr_cstr(&context->windowTitle), context->windowWidth, context->windowHeight, 0);
    if (!context->window)
    {
        logError("Error creating SDL window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    setWindowIcon(context->window);

    context->renderer = SDL_CreateRenderer(context->window, NULL);
    if (!context->renderer)
    {
        logError("Error creating SDL renderer: %s", SDL_GetError());
        SDL_DestroyWindow(context->window);
        SDL_Quit();
        return false;
    }

    // Set logical presentation so all rendering is in 320x200 space.
    // SDL handles upscaling to the actual window size.
    if (!SDL_SetRenderLogicalPresentation(context->renderer, context->originalWindowWidth, context->originalWindowHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE))
    {
        logError("Error setting logical presentation: %s", SDL_GetError());
        SDL_DestroyRenderer(context->renderer);
        SDL_DestroyWindow(context->window);
        SDL_Quit();
        return false;
    }

    // Initialize render state stack
    wr_init(context);

    context->transitionEndRealTime = 0.0;
    context->cheatsEnabled = true;

    context->__mutex = SDL_CreateMutex();

    SDL_HideCursor();

    // init audio
    if (!wa_initAudio(context))
    {
        logError("Could not initialize audio.");
        return false;
    }

    // load fonts
    context->fontSprites[0] = wfont_loadFontSprite(context, wsv_fromCString("./war1_font_1.png"));
    context->fontSprites[1] = wfont_loadFontSprite(context, wsv_fromCString("./war1_font_2.png"));

    // check if the DATA.WAR file exists
    bool dataFileExists = access(DATAWAR_FILE_PATH, F_OK) == 0;
    if (dataFileExists)
    {
        // load DATA.WAR file
        if (!wg_loadDataFile(context))
        {
            logError("Could not load file: %s", DATAWAR_FILE_PATH);
            return false;
        }

        if (!wg_setStartupDestination(context))
        {
            return false;
        }
    }
    else
    {
        WarScene* scene = wsc_createScene(context, WAR_SCENE_DOWNLOAD);
        wg_setNextScene(context, scene, 0.0f);
    }

    wact_initUnitActionDefs();

    context->frameStartNs = SDL_GetTicksNS();
    context->lastFrameStartNs = context->frameStartNs;
    context->frameWorkEndNs = context->frameStartNs;
    context->frameEndNs = context->frameStartNs;
    context->realTime = 0.0;
    context->realDeltaTime = 0.0f;
    context->gameTime = 0.0;
    context->gameDeltaTime = 0.0f;
    return true;
}

void wg_quitGame(WarContext* context)
{
    // Destroy audio stream (this also closes the audio device)
    if (context->audioStream)
    {
        SDL_DestroyAudioStream(context->audioStream);
        context->audioStream = NULL;
    }

    if (context->soundFont)
    {
        tsf_close(context->soundFont);
        context->soundFont = NULL;
    }

    if (context->audioRemoveMutex)
    {
        SDL_DestroyMutex(context->audioRemoveMutex);
        context->audioRemoveMutex = NULL;
    }

    if (context->audioMixBuffer)
    {
        wm_free(context->audioMixBuffer);
        context->audioMixBuffer = NULL;
        context->audioMixBufferCapacity = 0;
    }

    if (context->__mutex)
    {
        SDL_DestroyMutex(context->__mutex);
        context->__mutex = NULL;
    }

    if (context->renderer)
    {
        SDL_DestroyRenderer(context->renderer);
        context->renderer = NULL;
    }

    if (context->window)
    {
        SDL_DestroyWindow(context->window);
        context->window = NULL;
    }

    wstr_free(context->windowTitle);

    SDL_Quit();
}

bool wg_loadDataFile(WarContext* context)
{
    TracyCZoneN(ctx, "wg_loadDataFile", true);

    bool skipDecompress[MAX_RESOURCES_COUNT];
    memset(skipDecompress, 0, sizeof(skipDecompress));
    for (int i = 0; i < arrayLength(assets); ++i)
    {
        if (assets[i].type == DB_ENTRY_TYPE_UNKNOWN)
            skipDecompress[assets[i].index] = true;
    }

    context->warFile = wfile_loadWarFile(context, wsv_fromCString(DATAWAR_FILE_PATH), skipDecompress);
    if (!context->warFile)
    {
        TracyCZoneEnd(ctx);
        return false;
    }

    context->resources = (WarResource*)wm_alloc(sizeof(WarResource) * MAX_RESOURCES_COUNT);
    assert(context->resources);

    for (int i = 0; i < arrayLength(assets); ++i)
    {
        DatabaseEntry entry = assets[i];
        wres_loadResource(context, &entry);
    }

    TracyCZoneEnd(ctx);
    return true;
}

void wg_setWindowSize(WarContext* context, s32 width, s32 height)
{
    context->windowWidth = width;
    context->windowHeight = height;
    SDL_SetWindowSize(context->window, context->windowWidth, context->windowHeight);
}

void wg_setGlobalScale(WarContext* context, f32 scale)
{
    context->globalScale = MAX(scale, 1.0f);
    logDebug("set global scale to: %.2f", context->globalScale);

    s32 newWidth = (s32)(context->originalWindowWidth * context->globalScale);
    s32 newHeight = (s32)(context->originalWindowHeight * context->globalScale);
    wg_setWindowSize(context, newWidth, newHeight);
}

void wg_changeGlobalScale(WarContext* context, f32 deltaScale)
{
    wg_setGlobalScale(context, context->globalScale + deltaScale);
}

void wg_setGlobalSpeed(WarContext* context, f32 speed)
{
    context->globalSpeed = MAX(speed, 1.0f);
    logDebug("set global speed to: %.2f", context->globalSpeed);
}

void wg_changeGlobalSpeed(WarContext* context, f32 deltaSpeed)
{
    wg_setGlobalSpeed(context, context->globalSpeed + deltaSpeed);
}

void wg_setMusicVolume(WarContext* context, f32 volume)
{
    context->musicVolume = CLAMP(volume, 0.0f, 1.0f);
    logDebug("set music volume to: %.2f", context->musicVolume);
}

void wg_changeMusicVolume(WarContext* context, f32 deltaVolume)
{
    wg_setMusicVolume(context, context->musicVolume + deltaVolume);
}

void wg_setSoundVolume(WarContext* context, f32 volume)
{
    context->soundVolume = CLAMP(volume, 0.0f, 1.0f);
    logDebug("set sound volume to: %.2f", context->soundVolume);
}

void wg_changeSoundVolume(WarContext* context, f32 deltaVolume)
{
    wg_setSoundVolume(context, context->soundVolume + deltaVolume);
}

void wg_setNextScene(WarContext* context, WarScene* scene, f32 transitionDelay)
{
    context->nextScene = scene;
    context->transitionEndRealTime = context->realTime + transitionDelay;
}

void wg_setNextMap(WarContext* context, WarMap* map, f32 transitionDelay)
{
    context->nextMap = map;
    context->transitionEndRealTime = context->realTime + transitionDelay;
}

void wg_setInputButton(WarContext* context, s32 button, bool pressed)
{
    WarInput* input = &context->input;
    WarInputState* state = &input->buttons[button];

    if (pressed && !state->held)
    {
        state->justPressed = true;
    }
    else if (!pressed && state->held)
    {
        state->justReleased = true;
    }

    state->held = pressed;
}

void wg_setInputKey(WarContext* context, s32 key, bool pressed)
{
    WarInput* input = &context->input;
    WarInputState* state = &input->keys[key];

    if (pressed && !state->held)
    {
        state->justPressed = true;
    }
    else if (!pressed && state->held)
    {
        state->justReleased = true;
    }

    state->held = pressed;
}

void wg_beginInputFrame(WarContext* context)
{
    WarInput* input = &context->input;

    for (s32 i = 0; i < WAR_MOUSE_COUNT; ++i)
    {
        input->buttons[i].justPressed = false;
        input->buttons[i].justReleased = false;
    }

    for (s32 i = 0; i < WAR_KEY_COUNT; ++i)
    {
        input->keys[i].justPressed = false;
        input->keys[i].justReleased = false;
    }
}

void wg_processGameEvent(WarContext* context, SDL_Event* event)
{
    WarInput* input = &context->input;

    // NOTE: Convert event coordinates from window space to logical render space (320x200).
    // SDL_SetRenderLogicalPresentation does NOT do this automatically in SDL3.
    SDL_ConvertEventToRenderCoordinates(context->renderer, event);

    switch (event->type)
    {
        case SDL_EVENT_MOUSE_MOTION:
        {
            input->pos = vec2f((f32)floorf(event->motion.x), (f32)floorf(event->motion.y));
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            input->pos = vec2f((f32)floorf(event->button.x), (f32)floorf(event->button.y));

            if (event->button.button == SDL_BUTTON_LEFT)
            {
                wg_setInputButton(context, WAR_MOUSE_LEFT, event->button.down);
            }
            else if (event->button.button == SDL_BUTTON_RIGHT)
            {
                wg_setInputButton(context, WAR_MOUSE_RIGHT, event->button.down);
            }
            break;
        }

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        {
            WarKeys key = getWarKeyFromSDLKey(event->key.key);
            if (key != WAR_KEY_NONE)
            {
                bool pressed = event->type == SDL_EVENT_KEY_DOWN;

                if (key == WAR_KEY_SHIFT)
                {
                    wg_setInputKey(context, key, (event->key.mod & SDL_KMOD_SHIFT) != 0);
                }
                else if (key == WAR_KEY_CTRL)
                {
                    wg_setInputKey(context, key, (event->key.mod & SDL_KMOD_CTRL) != 0);
                }
                else if (key == WAR_KEY_ALT)
                {
                    wg_setInputKey(context, key, (event->key.mod & SDL_KMOD_ALT) != 0);
                }
                else
                {
                    wg_setInputKey(context, key, pressed);
                }
            }
            break;
        }

        case SDL_EVENT_TEXT_INPUT:
            appendCheatTextInput(context, wsv_fromCString(event->text.text));
            break;

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            if (!SDL_SetWindowMouseGrab(context->window, true))
            {
                logError("Error setting window mouse grab: %s", SDL_GetError());
            }
            break;

        case SDL_EVENT_WINDOW_FOCUS_LOST:
        case SDL_EVENT_WINDOW_MINIMIZED:
        case SDL_EVENT_WINDOW_HIDDEN:
        {
            for (s32 i = 0; i < WAR_MOUSE_COUNT; ++i)
            {
                input->buttons[i].held = false;
                input->buttons[i].justPressed = false;
                input->buttons[i].justReleased = false;
            }

            for (s32 i = 0; i < WAR_KEY_COUNT; ++i)
            {
                input->keys[i].held = false;
                input->keys[i].justPressed = false;
                input->keys[i].justReleased = false;
            }

            input->capturedUIButtonId = 0;
            input->mapDragState.status = WAR_DRAG_NONE;
            input->mapDragState.startPos = VEC2_ZERO;
            input->mapDragState.rect = RECT_EMPTY;

            if (!SDL_SetWindowMouseGrab(context->window, false))
            {
                logError("Error setting window mouse grab: %s", SDL_GetError());
            }
            break;
        }

        default:
            break;
    }
}

void wg_updateGame(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateGame", 1);

    mz_reset(frameZone);

    // Drain entity removals that the audio callback thread queued while we were
    // in the previous tick. We do this on the main thread (before any scene or
    // map update) so that we_removeEntityById never runs concurrently with audio.
    if (context->audioRemoveMutex)
    {
        SDL_LockMutex(context->audioRemoveMutex);
        s32 drainCount = context->audioRemovePendingCount;
        WarEntityId drainIds[AUDIO_REMOVE_PENDING_MAX];
        for (s32 i = 0; i < drainCount; i++)
            drainIds[i] = context->audioRemovePending[i];
        context->audioRemovePendingCount = 0;
        SDL_UnlockMutex(context->audioRemoveMutex);

        for (s32 i = 0; i < drainCount; i++)
            we_removeEntityById(context, drainIds[i]);
    }

    WarInput* input = &context->input;

    if (isKeyHeld(input, WAR_KEY_CTRL) &&
        !isKeyHeld(input, WAR_KEY_SHIFT) &&
        isKeyJustReleased(input, WAR_KEY_P))
    {
        context->paused = !context->paused;
    }

    if (context->paused)
    {
        // Allow scrolling and cheat panel input even while the simulation is frozen.
        if (context->map)
            wmap_updateMapPaused(context);
        TracyCZoneEnd(ctx);
        return;
    }

    if (context->nextScene)
    {
        context->audioEnabled = false;

        if (context->scene)
            wsc_leaveScene(context);
        else if (context->map)
            wmap_leaveMap(context);

        context->scene = context->nextScene;
        context->nextScene = NULL;

        wsc_enterScene(context);

        context->audioEnabled = true;
    }
    else if (context->nextMap)
    {
        context->audioEnabled = false;

        if (context->scene)
            wsc_leaveScene(context);
        else if (context->map)
            wmap_leaveMap(context);

        context->map = context->nextMap;
        context->nextMap = NULL;

        wmap_enterMap(context);

        context->audioEnabled = true;
    }

    if (context->realTime < context->transitionEndRealTime)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    if (context->scene)
    {
        wsc_updateScene(context);
    }
    else if (context->map)
    {
        wmap_updateMap(context);
    }
    else
    {
        logError("There is no map or scene active.");
    }

    TracyCZoneEnd(ctx);
}

void wg_renderGame(WarContext *context)
{
    TracyCZoneN(ctx, "RenderGame", 1);

    // Clear the screen (black)
    SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);
    SDL_RenderClear(context->renderer);

    // don't render anything if it's transitioning
    if (context->realTime < context->transitionEndRealTime)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    // Reset render state for this frame
    wr_init(context);

    imui_begin(context);

    if (context->scene)
    {
        wsc_renderScene(context);
    }
    else if (context->map)
    {
        wmap_renderMap(context);
    }

    imui_end(context);

    TracyCZoneEnd(ctx);
}

void wg_presentGame(WarContext *context)
{
    u64 targetFrameEndNs = context->frameStartNs + (u64)s2ns(SECONDS_PER_FRAME);

    SDL_RenderPresent(context->renderer);

    context->frameWorkEndNs = SDL_GetTicksNS();

    TracyCZoneN(waitCtx, "FrameWait", 1);
    context->frameEndNs = waitUntilFrameDeadlineNs(targetFrameEndNs);
    TracyCZoneEnd(waitCtx);

    s32 index = context->frameCount % METRIC_SAMPLE_COUNT;
    u64 workTimeNs = context->frameWorkEndNs - context->frameStartNs;
    u64 waitTimeNs = context->frameEndNs - context->frameWorkEndNs;
    u64 frameTimeNs = context->frameEndNs - context->frameStartNs;

    updateMetricU64(&context->workTimeMetric, workTimeNs, index);
    updateMetricU64(&context->waitTimeMetric, waitTimeNs, index);
    updateMetricU64(&context->frameTimeMetric, frameTimeNs, index);

    s32 fps = 0;
    if (context->frameTimeMetric.sampleSum > 0)
    {
        fps = (s32)((f64)context->frameTimeMetric.sampleCount * 1000000000.0 / (f64)context->frameTimeMetric.sampleSum);
    }
    updateMetricS32(&context->fpsMetric, fps, index);

    TracyCPlotI("FPS", (s64)context->fpsMetric.avg);
    TracyCPlotF("DT", (f32)ns2ms(context->frameTimeMetric.avg));
}
