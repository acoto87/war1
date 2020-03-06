bool initGame(WarContext* context)
{
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    context->globalScale = 3;
    context->globalSpeed = 1;
    context->originalWindowWidth = 320;
    context->originalWindowHeight = 200;
    context->windowWidth = (s32)(context->originalWindowWidth * context->globalScale);
    context->windowHeight = (s32)(context->originalWindowHeight * context->globalScale);
    strcpy(context->windowTitle, "War 1");
    context->window = glfwCreateWindow(context->windowWidth, context->windowHeight, context->windowTitle, NULL, NULL);
    context->transitionDelay = 0.0f;
    context->cheatsEnabled = true;

    pthread_mutex_init(&context->__mutex, NULL);

    if (!context->window)
    {
        logError("GLFW window could not be created!\n");
        glfwTerminate();
        return false;
    }

    glfwGetWindowSize(context->window, &context->windowWidth, &context->windowHeight);
    glfwGetFramebufferSize(context->window, &context->framebufferWidth, &context->framebufferHeight);
    context->devicePixelRatio = (f32)context->framebufferWidth / (f32)context->windowWidth;

    glfwMakeContextCurrent(context->window);

    glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);

    glCheckOpenGLVersion();

    // init graphics
    context->gfx = nvgCreateGLES2(NVG_STENCIL_STROKES | NVG_DEBUG);
    if (!context->gfx)
    {
        logError("Could not init nanovg.\n");
        glfwDestroyWindow(context->window);
        glfwTerminate();
		return false;
	}

    // context->fb = nvgluCreateFramebuffer(context->gfx,
    //                                      context->framebufferWidth,
    //                                      context->framebufferHeight,
    //                                      NVG_IMAGE_NEAREST);

    // if (!context->fb)
    // {
    //     logError("Could not create FBO.\n");
    //     glfwDestroyWindow(context->window);
    //     glfwTerminate();
    //     return false;
    // }

    // init audio
    if (!initAudio(context))
    {
        logError("Could not initialize audio.\n");
        return false;
    }

    // load fonts
    context->fontSprites[0] = loadFontSprite(context, "./war1_font_1.png");
    context->fontSprites[1] = loadFontSprite(context, "./war1_font_2.png");

    glViewport(0, 0, context->framebufferWidth, context->framebufferHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // check if the DATA.WAR file exists
    bool dataFileExists = access(DATAWAR_FILE_PATH, F_OK) == 0;
    if (dataFileExists)
    {
        // load DATA.WAR file
        if (!loadDataFile(context))
        {
            logError("Could not load DATA.WAR file\n");
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

    context->time = (f32)glfwGetTime();
    return true;
}

bool loadDataFile(WarContext* context)
{
    context->warFile = loadWarFile(context, DATAWAR_FILE_PATH);
    if (!context->warFile)
    {
        return false;
    }

    for (int i = 0; i < arrayLength(assets); ++i)
    {
        DatabaseEntry entry = assets[i];
        loadResource(context, &entry);
    }

    return true;
}

void setWindowSize(WarContext* context, s32 width, s32 height)
{
    context->windowWidth = width;
    context->windowHeight = height;
    glfwSetWindowSize(context->window, context->windowWidth, context->windowHeight);
    glfwGetFramebufferSize(context->window, &context->framebufferWidth, &context->framebufferHeight);
    context->devicePixelRatio = (f32)context->framebufferWidth / (f32)context->windowWidth;

    // nvgluDeleteFramebuffer(context->fb);
    // context->fb = nvgluCreateFramebuffer(context->gfx,
    //                                      context->framebufferWidth,
    //                                      context->framebufferHeight,
    //                                      NVG_IMAGE_NEAREST);
}

void setGlobalScale(WarContext* context, f32 scale)
{
    context->globalScale = max(scale, 1.0f);
    logDebug("set global scale to: %.2f\n", context->globalScale);

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
    logDebug("set global speed to: %.2f\n", context->globalSpeed);
}

void changeGlobalSpeed(WarContext* context, f32 deltaSpeed)
{
    setGlobalSpeed(context, context->globalSpeed + deltaSpeed);
}

void setMusicVolume(WarContext* context, f32 volume)
{
    context->musicVolume = clamp(volume, 0.0f, 1.0f);
    logDebug("set music volume to: %.2f\n", context->musicVolume);
}

void changeMusicVolume(WarContext* context, f32 deltaVolume)
{
    setMusicVolume(context, context->musicVolume + deltaVolume);
}

void setSoundVolume(WarContext* context, f32 volume)
{
    context->soundVolume = clamp(volume, 0.0f, 1.0f);
    logDebug("set sound volume to: %.2f\n", context->soundVolume);
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

void inputGame(WarContext *context)
{
    // mouse position
    f64 xpos, ypos;
    glfwGetCursorPos(context->window, &xpos, &ypos);

    vec2 pos = vec2f((f32)floor(xpos), (f32)floor(ypos));
    pos = vec2Scalef(pos, 1/context->globalScale);
    context->input.pos = pos;

    // mouse buttons
    setInputButton(context, WAR_MOUSE_LEFT, glfwGetMouseButton(context->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    setInputButton(context, WAR_MOUSE_RIGHT, glfwGetMouseButton(context->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    // keyboard keys
    setInputKey(context, WAR_KEY_SPACE, glfwGetKey(context->window, GLFW_KEY_SPACE) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_APOSTROPHE, glfwGetKey(context->window, GLFW_KEY_APOSTROPHE) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_COMMA, glfwGetKey(context->window, GLFW_KEY_COMMA) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_MINUS, glfwGetKey(context->window, GLFW_KEY_MINUS) == GLFW_PRESS ||
                                        glfwGetKey(context->window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_PERIOD, glfwGetKey(context->window, GLFW_KEY_PERIOD) == GLFW_PRESS ||
                                         glfwGetKey(context->window, GLFW_KEY_KP_DECIMAL) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_SLASH, glfwGetKey(context->window, GLFW_KEY_SLASH) == GLFW_PRESS ||
                                        glfwGetKey(context->window, GLFW_KEY_KP_DIVIDE) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_0, glfwGetKey(context->window, GLFW_KEY_0) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_0) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_1, glfwGetKey(context->window, GLFW_KEY_1) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_1) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_2, glfwGetKey(context->window, GLFW_KEY_2) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_2) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_3, glfwGetKey(context->window, GLFW_KEY_3) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_3) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_4, glfwGetKey(context->window, GLFW_KEY_4) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_4) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_5, glfwGetKey(context->window, GLFW_KEY_5) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_5) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_6, glfwGetKey(context->window, GLFW_KEY_6) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_6) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_7, glfwGetKey(context->window, GLFW_KEY_7) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_7) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_8, glfwGetKey(context->window, GLFW_KEY_8) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_8) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_9, glfwGetKey(context->window, GLFW_KEY_9) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_9) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_SEMICOLON, glfwGetKey(context->window, GLFW_KEY_SEMICOLON) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_EQUAL, glfwGetKey(context->window, GLFW_KEY_EQUAL) == GLFW_PRESS ||
                                        glfwGetKey(context->window, GLFW_KEY_KP_EQUAL) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_A, glfwGetKey(context->window, GLFW_KEY_A) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_B, glfwGetKey(context->window, GLFW_KEY_B) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_C, glfwGetKey(context->window, GLFW_KEY_C) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_D, glfwGetKey(context->window, GLFW_KEY_D) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_E, glfwGetKey(context->window, GLFW_KEY_E) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F, glfwGetKey(context->window, GLFW_KEY_F) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_G, glfwGetKey(context->window, GLFW_KEY_G) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_H, glfwGetKey(context->window, GLFW_KEY_H) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_I, glfwGetKey(context->window, GLFW_KEY_I) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_J, glfwGetKey(context->window, GLFW_KEY_J) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_K, glfwGetKey(context->window, GLFW_KEY_K) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_L, glfwGetKey(context->window, GLFW_KEY_L) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_M, glfwGetKey(context->window, GLFW_KEY_M) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_N, glfwGetKey(context->window, GLFW_KEY_N) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_O, glfwGetKey(context->window, GLFW_KEY_O) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_P, glfwGetKey(context->window, GLFW_KEY_P) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_Q, glfwGetKey(context->window, GLFW_KEY_Q) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_R, glfwGetKey(context->window, GLFW_KEY_R) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_S, glfwGetKey(context->window, GLFW_KEY_S) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_T, glfwGetKey(context->window, GLFW_KEY_T) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_U, glfwGetKey(context->window, GLFW_KEY_U) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_V, glfwGetKey(context->window, GLFW_KEY_V) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_W, glfwGetKey(context->window, GLFW_KEY_W) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_X, glfwGetKey(context->window, GLFW_KEY_X) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_Y, glfwGetKey(context->window, GLFW_KEY_Y) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_Z, glfwGetKey(context->window, GLFW_KEY_Z) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_LEFT_BRACKET, glfwGetKey(context->window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_BACKSLASH, glfwGetKey(context->window, GLFW_KEY_BACKSLASH) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_RIGHT_BRACKET, glfwGetKey(context->window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_GRAVE_ACCENT, glfwGetKey(context->window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_ESC, glfwGetKey(context->window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_ENTER, glfwGetKey(context->window, GLFW_KEY_ENTER) == GLFW_PRESS ||
                                        glfwGetKey(context->window, GLFW_KEY_KP_ENTER) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_TAB, glfwGetKey(context->window, GLFW_KEY_TAB) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_BACKSPACE, glfwGetKey(context->window, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_INSERT, glfwGetKey(context->window, GLFW_KEY_INSERT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_DELETE, glfwGetKey(context->window, GLFW_KEY_DELETE) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_RIGHT, glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_LEFT, glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_DOWN, glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_UP, glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_PAGE_UP, glfwGetKey(context->window, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_PAGE_DOWN, glfwGetKey(context->window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_HOME, glfwGetKey(context->window, GLFW_KEY_HOME) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_END, glfwGetKey(context->window, GLFW_KEY_END) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_F1, glfwGetKey(context->window, GLFW_KEY_F1) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F2, glfwGetKey(context->window, GLFW_KEY_F2) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F3, glfwGetKey(context->window, GLFW_KEY_F3) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F4, glfwGetKey(context->window, GLFW_KEY_F4) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F5, glfwGetKey(context->window, GLFW_KEY_F5) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F6, glfwGetKey(context->window, GLFW_KEY_F6) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F7, glfwGetKey(context->window, GLFW_KEY_F7) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F8, glfwGetKey(context->window, GLFW_KEY_F8) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F9, glfwGetKey(context->window, GLFW_KEY_F9) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F10, glfwGetKey(context->window, GLFW_KEY_F10) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F11, glfwGetKey(context->window, GLFW_KEY_F11) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_F12, glfwGetKey(context->window, GLFW_KEY_F12) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_ASTERISK, glfwGetKey(context->window, GLFW_KEY_KP_MULTIPLY) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_PLUS, glfwGetKey(context->window, GLFW_KEY_KP_ADD) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_SHIFT, glfwGetKey(context->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                                        glfwGetKey(context->window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_CTRL, glfwGetKey(context->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                                       glfwGetKey(context->window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_ALT, glfwGetKey(context->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
                                      glfwGetKey(context->window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);
}

void inputCharCallback(GLFWwindow* window, u32 codepoint)
{
    WarContext* context = glfwGetWindowUserPointer(window);
    assert(context);

    WarScene* scene = context->scene;
    WarMap* map = context->map;
    assert(scene || map);

    WarCheatStatus* cheatStatus = scene
        ? &scene->cheatStatus : &map->cheatStatus;

    if (cheatStatus->enabled && cheatStatus->visible)
    {
        s32 length = strlen(cheatStatus->text);
        if (length + 1 < CHEAT_TEXT_MAX_LENGTH)
        {
            strInsertAt(cheatStatus->text, cheatStatus->position, (char)codepoint);
            cheatStatus->position++;
        }
    }
}

void updateGame(WarContext* context)
{
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
            leaveMap(context);

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
            leaveMap(context);

        context->map = context->nextMap;
        context->nextMap = NULL;

        enterMap(context);

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
        updateMap(context);
    }
    else
    {
        logError("There is no map or scene active.\n");
    }
}

void renderGame(WarContext *context)
{
    NVGcontext* gfx = context->gfx;

    s32 framebufferWidth = context->framebufferWidth;
    s32 framebufferHeight = context->framebufferHeight;

    s32 windowWidth = context->windowWidth;
    s32 windowHeight = context->windowHeight;

    f32 devicePixelRatio = context->devicePixelRatio;

    // render the whole scene to the FBO
    //
    // NOTE: This framebuffer stuff is to be able to make post-processing
    // (generating gifs, or other kind of techniques like old TV effects).
    // I'm commenting it out the framebuffer stuff because there is performance
    // reasons with OpenGL ES `glReadPixels`, since is the way to read back
    // pixels from the framebuffer (there is PBO on OpenGL ES 2.0).
    //
    // NVGLUframebuffer* fb = context->fb;
    // nvgluBindFramebuffer(fb);

    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // don't render anything if it's transitioning
    if (context->transitionDelay > 0)
    {
        return;
    }

    nvgBeginFrame(gfx, windowWidth, windowHeight, devicePixelRatio);

    if (context->scene)
    {
        renderScene(context);
    }
    else if (context->map)
    {
        renderMap(context);
    }

    nvgEndFrame(gfx);

    // nvgluBindFramebuffer(NULL);

    // then render a quad with the texture geneate with the FBO
    // glViewport(0, 0, framebufferWidth, framebufferHeight);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // nvgBeginFrame(gfx, windowWidth, windowHeight, devicePixelRatio);
    // nvgSave(gfx);

    // NVGpaint img = nvgImagePattern(gfx, 0, 0, windowWidth, windowHeight, 0, fb->image, 1.0f);
    // nvgBeginPath(gfx);
    // nvgRect(gfx, 0, 0, windowWidth, windowHeight);
    // nvgFillPaint(gfx, img);
    // nvgFill(gfx);

    // nvgRestore(gfx);
    // nvgEndFrame(gfx);
}

void presentGame(WarContext *context)
{
    glfwSwapBuffers(context->window);
    glfwPollEvents();

    f32 currentTime = glfwGetTime();
    context->deltaTime = (currentTime - context->time);

    // This code is good in theory, but the sleep resolution on different OSes
    // varies, so Sleep in Windows may take a longer time than specified.
    //
    // On Linux I should do a do {..} while (); using the nanosleep function
    // and check for interrutions that wake up the thread before.
    //
    // msleep((s32)((SECONDS_PER_FRAME - context->deltaTime) * 1000));
    // currentTime = (f32)glfwGetTime();
    // context->deltaTime = (currentTime - context->time);

    // This was the previous code that wait until the end of the frame
    // but this burn too much CPU, so it's better the alternative of
    // sleep the process and save CPU usage and battery.
    //
    // Going back to this code for now, until we get a consistent game loop with sleep.
    while (context->deltaTime <= SECONDS_PER_FRAME)
    {
        currentTime = (f32)glfwGetTime();
        context->deltaTime = (currentTime - context->time);
    }

    context->time = currentTime;
    context->fps = (u32)(1.0f / context->deltaTime);
}