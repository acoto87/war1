#include "war_scene_download.h"

#include "SDL3/SDL.h"

#include "shl/wstr.h"

#include "war.h"
#include "war_game.h"
#include "war_net.h"
#include "war_ui.h"

void wsc_enterSceneDownload(WarContext* context)
{
    WarScene* scene = context->scene;

    scene->download.status = WAR_SCENE_DOWNLOAD_DOWNLOAD;

    static const char text[] = "Hi there! Thanks for downloading the game! :)\n"
                               "\n"
                               "War 1 needs the file with all the assets of the game.\n"
                               "That file is the DATA.WAR that comes with original\n"
                               "game, you can copy and paste that file (if you have it)\n"
                               "into the War 1 folder or you can press Enter to\n"
                               "download the DEMO DATA.WAR file from the internet.\n"
                               "\n"
                               "Enjoy the game!";

    WarEntity* downloadText = wui_createUIText(context, wstr_fromCString("txtDownload"), 1, 10, wstr_fromCString(text), vec2i(10, 10));
    setUITextColor(downloadText, WAR_COLOR_RGB(255, 215, 138));
    setUITextMultiline(downloadText, true);
    setUITextBoundings(downloadText, vec2f((f32)(context->originalWindowWidth - 20), (f32)(context->originalWindowHeight - 20)));
    setUITextHorizontalAlign(downloadText, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(downloadText, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(downloadText, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(downloadText, WAR_TEXT_WRAP_CHAR);
    setUITextLineHeight(downloadText, 120);

    WarEntity* downloadingText = wui_createUIText(context, wstr_fromCString("txtDownloading"), 1, 10, wstr_fromCString("Downloading..."), vec2i(10, 10));
    setUITextColor(downloadingText, WAR_COLOR_RGB(255, 215, 138));
    setUITextMultiline(downloadingText, false);
    setUITextBoundings(downloadingText, vec2f((f32)(context->originalWindowWidth - 20), (f32)(context->originalWindowHeight - 20)));
    setUITextHorizontalAlign(downloadingText, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(downloadingText, WAR_TEXT_ALIGN_BOTTOM);
    setUITextLineAlign(downloadingText, WAR_TEXT_ALIGN_LEFT);
    setUIEntityStatus(downloadingText, false);
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
                static const char confirm[] = "The DEMO DATA.WAR doesn't have all assets.\n"
                                              "\n"
                                              "You may not get all the features, but it will allow you to start and play the game.\n"
                                              "\n"
                                              "To get the full experience, you need to get an original\n"
                                              "copy of the game and copy the file from there.\n"
                                              "\n"
                                              "Press Enter to confirm you want to download the\n"
                                              "DEMO DATA.WAR file";

                WarEntity* downloadText = we_findUIEntity(context, wsv_fromCString("txtDownload"));
                wui_setUIText(downloadText, wstr_fromCString(confirm));

                scene->download.status = WAR_SCENE_DOWNLOAD_CONFIRM;
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_CONFIRM:
        {
            if (isKeyJustReleased(input, WAR_KEY_ENTER))
            {
                WarEntity* downloadingText = we_findUIEntity(context, wsv_fromCString("txtDownloading"));
                setUIEntityStatus(downloadingText, true);

                scene->download.status = WAR_SCENE_DOWNLOAD_DOWNLOADING;
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_DOWNLOADING:
        {
            bool success = wnet_downloadFileFromUrl(context, wsv_fromCString(ONLINE_DEMO_DATAWAR_FILE_URL), wsv_fromCString(DATAWAR_FILE_PATH));
            if (success)
            {
                WarEntity* downloadingText = we_findUIEntity(context, wsv_fromCString("txtDownloading"));
                wui_setUIText(downloadingText, wstr_fromCString("Downloading... Done. Press Enter to start the game."));

                scene->download.status = WAR_SCENE_DOWNLOAD_DOWNLOADED;
            }
            else
            {
                WarEntity* downloadingText = we_findUIEntity(context, wsv_fromCString("txtDownloading"));
                wui_setUIText(downloadingText, wstr_fromCString("Downloading... Failed. Press Enter to quit the game."));

                scene->download.status = WAR_SCENE_DOWNLOAD_FAILED;
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_DOWNLOADED:
        {
            if (isKeyJustReleased(input, WAR_KEY_ENTER))
            {
                // load DATA.WAR file
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
