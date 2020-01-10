void enterSceneDownload(WarContext* context)
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

    WarEntity* downloadText = createUIText(context, "txtDownload", 1, 10, text, vec2i(10, 10));
    setUITextColor(downloadText, u8RgbColor(255, 215, 138));
    setUITextMultiline(downloadText, true);
    setUITextBoundings(downloadText, vec2f(context->originalWindowWidth-20, context->originalWindowHeight-20));
    setUITextHorizontalAlign(downloadText, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(downloadText, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(downloadText, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(downloadText, WAR_TEXT_WRAP_CHAR);
    setUITextLineHeight(downloadText, 120);

    WarEntity* downloadingText = createUIText(context, "txtDownloading", 1, 10, "Downloading...", vec2i(10, 10));
    setUITextColor(downloadingText, u8RgbColor(255, 215, 138));
    setUITextMultiline(downloadingText, false);
    setUITextBoundings(downloadingText, vec2f(context->originalWindowWidth-20, context->originalWindowHeight-20));
    setUITextHorizontalAlign(downloadingText, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(downloadingText, WAR_TEXT_ALIGN_BOTTOM);
    setUITextLineAlign(downloadingText, WAR_TEXT_ALIGN_LEFT);
    setUIEntityStatus(downloadingText, false);
}

void updateSceneDownload(WarContext* context)
{
    WarInput* input = &context->input;
    WarScene* scene = context->scene;

    switch (scene->download.status)
    {
        case WAR_SCENE_DOWNLOAD_DOWNLOAD:
        {
            if (wasKeyPressed(input, WAR_KEY_ENTER))
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

                WarEntity* downloadText = findUIEntity(context, "txtDownload");
                setUIText(downloadText, confirm);

                scene->download.status = WAR_SCENE_DOWNLOAD_CONFIRM;
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_CONFIRM:
        {
            if (wasKeyPressed(input, WAR_KEY_ENTER))
            {
                WarEntity* downloadingText = findUIEntity(context, "txtDownloading");
                setUIEntityStatus(downloadingText, true);

                scene->download.status = WAR_SCENE_DOWNLOAD_DOWNLOADING;
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_DOWNLOADING:
        {
            bool success = downloadFileFromUrl(ONLINE_DEMO_DATAWAR_FILE_URL, DATAWAR_FILE_PATH);
            if (success)
            {
                WarEntity* downloadingText = findUIEntity(context, "txtDownloading");
                setUIText(downloadingText, "Downloading... Done. Press Enter to start the game.");

                scene->download.status = WAR_SCENE_DOWNLOAD_DOWNLOADED;
            }
            else
            {
                WarEntity* downloadingText = findUIEntity(context, "txtDownloading");
                setUIText(downloadingText, "Downloading... Failed. Press Enter to quit the game.");

                scene->download.status = WAR_SCENE_DOWNLOAD_FAILED;
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_DOWNLOADED:
        {
            if (wasKeyPressed(input, WAR_KEY_ENTER))
            {
                // load DATA.WAR file
                if (loadDataFile(context))
                {
                    scene->download.status = WAR_SCENE_DOWNLOAD_FILE_LOADED;
                }
                else
                {
                    logError("Could not load DATA.WAR file\n");
                }
            }

            break;
        }
        case WAR_SCENE_DOWNLOAD_FILE_LOADED:
        {
            WarScene* scene = createScene(context, WAR_SCENE_BLIZZARD);
            setNextScene(context, scene, 0.0f);

            break;
        }
        case WAR_SCENE_DOWNLOAD_FAILED:
        {
            if (wasKeyPressed(input, WAR_KEY_ENTER))
            {
                glfwSetWindowShouldClose(context->window, true);
            }

            break;
        }
    }
}