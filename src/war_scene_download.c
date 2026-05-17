#include "war_scene_download.h"

#include "SDL3/SDL.h"

#include "shl/wstr.h"

#include "war.h"
#include "war_game.h"
#include "war_imui.h"
#include "war_net.h"

static const char downloadWelcomeText[] =
    "Hi there! Thanks for downloading the game! :)\n"
    "\n"
    "War 1 needs the file with all the assets of the game.\n"
    "That file is the DATA.WAR that comes with original\n"
    "game, you can copy and paste that file (if you have it)\n"
    "into the War 1 folder or you can press Enter to\n"
    "download the DEMO DATA.WAR file from the internet.\n"
    "\n"
    "Enjoy the game!";

static const char downloadConfirmText[] =
    "The DEMO DATA.WAR doesn't have all assets.\n"
    "\n"
    "You may not get all the features, but it will allow you to start and play the game.\n"
    "\n"
    "To get the full experience, you need to get an original\n"
    "copy of the game and copy the file from there.\n"
    "\n"
    "Press Enter to confirm you want to download the\n"
    "DEMO DATA.WAR file";

void wsc_enterSceneDownload(WarContext* context)
{
    WarScene* scene = context->scene;
    scene->download.status = WAR_SCENE_DOWNLOAD_DOWNLOAD;
}

void wsc_updateSceneDownload(WarContext* context)
{
    WarInput* input = &context->input;
    WarScene* scene = context->scene;

    switch (scene->download.status)
    {
        case WAR_SCENE_DOWNLOAD_DOWNLOAD:
        {
            if (isKeyJustReleased(input, WAR_KEY_ENTER))
            {
                scene->download.status = WAR_SCENE_DOWNLOAD_CONFIRM;
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_CONFIRM:
        {
            if (isKeyJustReleased(input, WAR_KEY_ENTER))
            {
                scene->download.status = WAR_SCENE_DOWNLOAD_DOWNLOADING;
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_DOWNLOADING:
        {
            bool success = wnet_downloadFileFromUrl(context, wsv_fromCString(ONLINE_DEMO_DATAWAR_FILE_URL), wsv_fromCString(DATAWAR_FILE_PATH));
            if (success)
            {
                scene->download.status = WAR_SCENE_DOWNLOAD_DOWNLOADED;
            }
            else
            {
                scene->download.status = WAR_SCENE_DOWNLOAD_FAILED;
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_DOWNLOADED:
        {
            if (isKeyJustReleased(input, WAR_KEY_ENTER))
            {
                if (wg_loadDataFile(context))
                {
                    scene->download.status = WAR_SCENE_DOWNLOAD_FILE_LOADED;
                }
                else
                {
                    logError("Could not load file: %s", DATAWAR_FILE_PATH);
                }
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_FILE_LOADED:
        {
            WarScene* nextScene = wsc_createScene(context, WAR_SCENE_BLIZZARD);
            wg_setNextScene(context, nextScene, 0.0f);

            break;
        }
        case WAR_SCENE_DOWNLOAD_FAILED:
        {
            if (isKeyJustReleased(input, WAR_KEY_ENTER))
            {
                SDL_PushEvent(&(SDL_Event){ .type = SDL_EVENT_QUIT });
            }

            break;
        }
    }
}

void wsc_renderSceneDownload(WarContext* context)
{
    WarScene* scene = context->scene;

    f32 w = (f32)(context->originalWindowWidth  - 20);
    f32 h = (f32)(context->originalWindowHeight - 20);

    // Main body text — shown in DOWNLOAD and CONFIRM states.
    switch (scene->download.status)
    {
        case WAR_SCENE_DOWNLOAD_DOWNLOAD:
        {
            imui_text(context, "txtDownload",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position   = vec2i(10, 10),
                    .fontIndex  = 1,
                    .fontColor  = WAR_COLOR_RGB(255, 215, 138),
                    .multiline  = true,
                    .boundings  = vec2f(w, h),
                    .wrapping   = WAR_TEXT_WRAP_CHAR,
                    .lineHeight = 120,
                    .text = wsv_fromCString(downloadWelcomeText)
                ));
            break;
        }
        case WAR_SCENE_DOWNLOAD_CONFIRM:
        {
            imui_text(context, "txtDownload",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position   = vec2i(10, 10),
                    .fontIndex  = 1,
                    .fontColor  = WAR_COLOR_RGB(255, 215, 138),
                    .multiline  = true,
                    .boundings  = vec2f(w, h),
                    .wrapping   = WAR_TEXT_WRAP_CHAR,
                    .lineHeight = 120,
                    .text = wsv_fromCString(downloadConfirmText)
                ));
            break;
        }
        case WAR_SCENE_DOWNLOAD_DOWNLOADING:
        {
            imui_text(context, "txtDownloading",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position      = vec2i(10, 10),
                    .fontIndex     = 1,
                    .fontColor     = WAR_COLOR_RGB(255, 215, 138),
                    .boundings     = vec2f(w, h),
                    .verticalAlign = WAR_TEXT_ALIGN_BOTTOM,
                    .text = wsv_fromCString("Downloading...")
                ));
            break;
        }
        case WAR_SCENE_DOWNLOAD_DOWNLOADED:
        case WAR_SCENE_DOWNLOAD_FILE_LOADED:
        {
            imui_text(context, "txtDownloading",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position      = vec2i(10, 10),
                    .fontIndex     = 1,
                    .fontColor     = WAR_COLOR_RGB(255, 215, 138),
                    .boundings     = vec2f(w, h),
                    .verticalAlign = WAR_TEXT_ALIGN_BOTTOM,
                    .text = wsv_fromCString("Downloading... Done. Press Enter to start the game.")
                ));
            break;
        }
        case WAR_SCENE_DOWNLOAD_FAILED:
        {
            imui_text(context, "txtDownloading",
                CREATE_UI_TEXT_ARGS_INIT(
                    .position      = vec2i(10, 10),
                    .fontIndex     = 1,
                    .fontColor     = WAR_COLOR_RGB(255, 215, 138),
                    .boundings     = vec2f(w, h),
                    .verticalAlign = WAR_TEXT_ALIGN_BOTTOM,
                    .text = wsv_fromCString("Downloading... Failed. Press Enter to quit the game.")
                ));
            break;
        }
    }
}
