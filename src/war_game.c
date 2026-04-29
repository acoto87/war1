#include "war_game.h"

#include <assert.h>
#include <math.h>

#if defined(_MSC_VER) && !defined(__clan_)
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

#include "war_alloc.h"
#include "war_actions.h"
#include "war_audio.h"
#include "war_file.h"
#include "war_font.h"
#include "war.h"
#include "war_map_render.h"
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

static void appendCheatTextInput(WarContext* context, StringView text)
{
    WarScene* scene = context->scene;
    WarMap* map = context->map;
    assert(scene || map);

    WarCheatStatus* cheatStatus = scene
        ? &scene->cheatStatus : &map->cheatStatus;

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

bool initGame(WarContext* context)
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
    if (!SDL_SetRenderLogicalPresentation(context->renderer,
            context->originalWindowWidth, context->originalWindowHeight,
            SDL_LOGICAL_PRESENTATION_INTEGER_SCALE))
    {
        logError("Error setting logical presentation: %s", SDL_GetError());
        SDL_DestroyRenderer(context->renderer);
        SDL_DestroyWindow(context->window);
        SDL_Quit();
        return false;
    }

    // Initialize render state stack
    wrend_renderInit(context);

    context->transitionDelay = 0.0f;
    context->cheatsEnabled = true;

    context->__mutex = SDL_CreateMutex();

    SDL_HideCursor();

    // init audio
    if (!waud_initAudio(context))
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
        if (!loadDataFile(context))
        {
            logError("Could not load file: %s", DATAWAR_FILE_PATH);
            return false;
        }

        WarScene* scene = createScene(context, WAR_SCENE_BLIZZARD);
        setNextScene(context, scene, 0.0f);
    }
    else
    {
        WarScene* scene = createScene(context, WAR_SCENE_DOWNLOAD);
        setNextScene(context, scene, 0.0f);
    }

    initUnitActionDefs();

    context->time = SDL_GetTicks() / 1000.0f;
    return true;
}

void quitGame(WarContext* context)
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
        war_free(context->audioMixBuffer);
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

bool loadDataFile(WarContext* context)
{
    context->warFile = wfile_loadWarFile(context, wsv_fromCString(DATAWAR_FILE_PATH));
    if (!context->warFile)
    {
        return false;
    }

    for (int i = 0; i < arrayLength(assets); ++i)
    {
        DatabaseEntry entry = assets[i];
        wres_loadResource(context, &entry);
    }

    return true;
}

void setWindowSize(WarContext* context, s32 width, s32 height)
{
    context->windowWidth = width;
    context->windowHeight = height;
    SDL_SetWindowSize(context->window, context->windowWidth, context->windowHeight);
}

void setGlobalScale(WarContext* context, f32 scale)
{
    context->globalScale = max(scale, 1.0f);
    logDebug("set global scale to: %.2f", context->globalScale);

    s32 newWidth = (s32)(context->originalWindowWidth * context->globalScale);
    s32 newHeight = (s32)(context->originalWindowHeight * context->globalScale);
    setWindowSize(context, newWidth, newHeight);
}

void changeGlobalScale(WarContext* context, f32 deltaScale)
{
    setGlobalScale(context, context->globalScale + deltaScale);
}

void setGlobalSpeed(WarContext* context, f32 speed)
{
    context->globalSpeed = max(speed, 1.0f);
    logDebug("set global speed to: %.2f", context->globalSpeed);
}

void changeGlobalSpeed(WarContext* context, f32 deltaSpeed)
{
    setGlobalSpeed(context, context->globalSpeed + deltaSpeed);
}

void setMusicVolume(WarContext* context, f32 volume)
{
    context->musicVolume = clamp(volume, 0.0f, 1.0f);
    logDebug("set music volume to: %.2f", context->musicVolume);
}

void changeMusicVolume(WarContext* context, f32 deltaVolume)
{
    setMusicVolume(context, context->musicVolume + deltaVolume);
}

void setSoundVolume(WarContext* context, f32 volume)
{
    context->soundVolume = clamp(volume, 0.0f, 1.0f);
    logDebug("set sound volume to: %.2f", context->soundVolume);
}

void changeSoundVolume(WarContext* context, f32 deltaVolume)
{
    setSoundVolume(context, context->soundVolume + deltaVolume);
}

void setNextScene(WarContext* context, WarScene* scene, f32 transitionDelay)
{
    context->nextScene = scene;
    context->transitionDelay = transitionDelay;
}

void setNextMap(WarContext* context, WarMap* map, f32 transitionDelay)
{
    context->nextMap = map;
    context->transitionDelay = transitionDelay;
}

void setInputButton(WarContext* context, s32 button, bool pressed)
{
    WarInput* input = &context->input;

    input->buttons[button].wasPressed = input->buttons[button].pressed && !pressed;
    input->buttons[button].pressed = pressed;
}

void setInputKey(WarContext* context, s32 key, bool pressed)
{
    WarInput* input = &context->input;

    input->keys[key].wasPressed = input->keys[key].pressed && !pressed;
    input->keys[key].pressed = pressed;
}

void beginInputFrame(WarContext* context)
{
    WarInput* input = &context->input;

    for (s32 i = 0; i < WAR_MOUSE_COUNT; ++i)
    {
        input->buttons[i].wasPressed = false;
    }

    for (s32 i = 0; i < WAR_KEY_COUNT; ++i)
    {
        input->keys[i].wasPressed = false;
    }

    input->wasDragging = false;
}

void processGameEvent(WarContext* context, SDL_Event* event)
{
    // NOTE: Convert event coordinates from window space to logical render space (320x200).
    // SDL_SetRenderLogicalPresentation does NOT do this automatically in SDL3.
    SDL_ConvertEventToRenderCoordinates(context->renderer, event);

    switch (event->type)
    {
        case SDL_EVENT_MOUSE_MOTION:
        {
            vec2 pos = vec2f((f32)floorf(event->motion.x), (f32)floorf(event->motion.y));
            context->input.pos = pos;
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            vec2 pos = vec2f((f32)floorf(event->button.x), (f32)floorf(event->button.y));
            context->input.pos = pos;

            bool pressed = event->button.down;
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                setInputButton(context, WAR_MOUSE_LEFT, pressed);
            }
            else if (event->button.button == SDL_BUTTON_RIGHT)
            {
                setInputButton(context, WAR_MOUSE_RIGHT, pressed);
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
                    setInputKey(context, key, (event->key.mod & SDL_KMOD_SHIFT) != 0);
                }
                else if (key == WAR_KEY_CTRL)
                {
                    setInputKey(context, key, (event->key.mod & SDL_KMOD_CTRL) != 0);
                }
                else if (key == WAR_KEY_ALT)
                {
                    setInputKey(context, key, (event->key.mod & SDL_KMOD_ALT) != 0);
                }
                else
                {
                    setInputKey(context, key, pressed);
                }
            }
            break;
        }

        case SDL_EVENT_TEXT_INPUT:
            appendCheatTextInput(context, wsv_fromCString(event->text.text));
            break;

        case SDL_EVENT_WINDOW_FOCUS_LOST:
        case SDL_EVENT_WINDOW_MINIMIZED:
        case SDL_EVENT_WINDOW_HIDDEN:
        {
            WarInput* input = &context->input;

            for (s32 i = 0; i < WAR_MOUSE_COUNT; ++i)
            {
                input->buttons[i].pressed = false;
                input->buttons[i].wasPressed = false;
            }

            for (s32 i = 0; i < WAR_KEY_COUNT; ++i)
            {
                input->keys[i].pressed = false;
                input->keys[i].wasPressed = false;
            }

            input->isDragging = false;
            input->wasDragging = false;
            input->dragPos = VEC2_ZERO;
            input->dragRect = RECT_EMPTY;
            break;
        }

        default:
            break;
    }
}

void updateGame(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateGame", 1);

    mz_reset(frameZone);

    // Drain entity removals that the audio callback thread queued while we were
    // in the previous tick. We do this on the main thread (before any scene or
    // map update) so that went_removeEntityById never runs concurrently with audio.
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
            went_removeEntityById(context, drainIds[i]);
    }

    WarInput* input = &context->input;

    if (isKeyPressed(input, WAR_KEY_CTRL) &&
        wasKeyPressed(input, WAR_KEY_P))
    {
        context->paused = !context->paused;
    }

    if (context->paused)
    {
        return;
    }

    if (context->nextScene)
    {
        context->audioEnabled = false;

        if (context->scene)
            leaveScene(context);
        else if (context->map)
            wmap_leaveMap(context);

        context->scene = context->nextScene;
        context->nextScene = NULL;

        enterScene(context);

        context->audioEnabled = true;
    }
    else if (context->nextMap)
    {
        context->audioEnabled = false;

        if (context->scene)
            leaveScene(context);
        else if (context->map)
            wmap_leaveMap(context);

        context->map = context->nextMap;
        context->nextMap = NULL;

        wmap_enterMap(context);

        context->audioEnabled = true;
    }

    if (context->transitionDelay > 0)
    {
        context->transitionDelay = max(context->transitionDelay - context->deltaTime, 0.0f);
        return;
    }

    if (context->scene)
    {
        updateScene(context);
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

void renderGame(WarContext *context)
{
    TracyCZoneN(ctx, "RenderGame", 1);

    // Clear the screen (black)
    SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);
    SDL_RenderClear(context->renderer);

    // don't render anything if it's transitioning
    if (context->transitionDelay > 0)
    {
        return;
    }

    // Reset render state for this frame
    wrend_renderInit(context);

    if (context->scene)
    {
        renderScene(context);
    }
    else if (context->map)
    {
        renderMap(context);
    }

    TracyCZoneEnd(ctx);
}

void presentGame(WarContext *context)
{
    SDL_RenderPresent(context->renderer);

    f32 currentTime = SDL_GetTicks() / 1000.0f;
    context->deltaTime = (currentTime - context->time);

    // This code is good in theory, but the sleep resolution on different OSes
    // varies, so Sleep in Windows may take a longer time than specified.
    //
    // On Linux I should do a do {..} while (); using the nanosleep function
    // and check for interrutions that wake up the thread before.
    //
    // SDL_Delay((s32)((SECONDS_PER_FRAME - context->deltaTime) * 1000));
    // currentTime = SDL_GetTicks() / 1000.0f;
    // context->deltaTime = (currentTime - context->time);

    // This was the previous code that wait until the end of the frame
    // but this burn too much CPU, so it's better the alternative of
    // sleep the process and save CPU usage and battery.
    //
    // Going back to this code for now, until we get a consistent game loop with sleep.
    {
        TracyCZoneN(waitCtx, "FrameWait", 1);
        while (context->deltaTime <= SECONDS_PER_FRAME)
        {
            currentTime = SDL_GetTicks() / 1000.0f;
            context->deltaTime = (currentTime - context->time);
        }
        TracyCZoneEnd(waitCtx);
    }

    context->time = currentTime;
    context->fps = (u32)(1.0f / context->deltaTime);

    TracyCPlotI("FPS", (s64)context->fps);
    TracyCPlotF("DeltaTime_ms", context->deltaTime * 1000.0f);
}
