bool initGame(WarContext* context)
{
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef WAR_OPENGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif

    context->globalScale = 3;
    context->globalSpeed = 1;
    context->originalWindowWidth = 320;
    context->originalWindowHeight = 200;
    context->windowWidth = (s32)(context->originalWindowWidth * context->globalScale);
    context->windowHeight = (s32)(context->originalWindowHeight * context->globalScale);
    strcpy(context->windowTitle, "War 1");
    context->window = glfwCreateWindow(context->windowWidth, context->windowHeight, context->windowTitle, NULL, NULL);

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

    gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);

    glCheckOpenGLVersion();

#ifdef WAR_OPENGL
    context->gfx = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
    context->gfx = nvgCreateGLES2(NVG_STENCIL_STROKES | NVG_DEBUG);
#endif

    if (!context->gfx) 
    {
        logError("Could not init nanovg.\n");
        glfwDestroyWindow(context->window);
        glfwTerminate();
		return -1;
	}

    context->fb = nvgluCreateFramebuffer(context->gfx, 
                                         context->framebufferWidth, 
                                         context->framebufferHeight, 
                                         NVG_IMAGE_NEAREST);

    if (!context->fb) 
    {
        logError("Could not create FBO.\n");
        glfwDestroyWindow(context->window);
        glfwTerminate();
        return -1;
    }

    // load fonts
    nvgCreateFont(context->gfx, "defaultFont", "./build/Roboto-Regular.ttf");

    glViewport(0, 0, context->framebufferWidth, context->framebufferHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    context->fontPath = "./build/war1_font.png";
    context->fontSprite = loadFontSprite(context);

    context->warFilePath = "./build/DATA.WAR";
    context->warFile = loadWarFile(context);

    for (int i = 0; i < arrayLength(assets); ++i)
    {
        DatabaseEntry entry = assets[i];
        switch (entry.type)
        {
            case DB_ENTRY_TYPE_PALETTE:
            {
                loadPaletteResource(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_IMAGE:
            {
                loadImageResource(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_SPRITE:
            {
                loadSpriteResource(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_LEVEL_INFO:
            {
                loadLevelInfo(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_LEVEL_VISUAL:
            {
                loadLevelVisual(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_LEVEL_PASSABLE:
            {
                loadLevelPassable(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_TILESET:
            {
                loadTileset(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_TILES:
            {
                loadTiles(context, &entry);
                break;
            }

            case DB_ENTRY_TYPE_TEXT:
            {
                loadText(context, &entry);
                break;
            }

            default:
            {
                logInfo("DB entries of type %d aren't handled yet.\n", entry.type);
                break;
            }
        }
    }

    //createEmptyScene(context);
    createMap(context, 117);
    
    context->time = (f32)glfwGetTime();
    return true;
}

void setWindowSize(WarContext* context, s32 width, s32 height)
{
    context->windowWidth = width;
    context->windowHeight = height;
    glfwSetWindowSize(context->window, context->windowWidth, context->windowHeight);
    glfwGetFramebufferSize(context->window, &context->framebufferWidth, &context->framebufferHeight);
    context->devicePixelRatio = (f32)context->framebufferWidth / (f32)context->windowWidth;

    nvgluDeleteFramebuffer(context->fb);
    context->fb = nvgluCreateFramebuffer(context->gfx,
                                         context->framebufferWidth,
                                         context->framebufferHeight,
                                         NVG_IMAGE_NEAREST);
}

void setGlobalScale(WarContext* context, f32 scale)
{
    assert(scale >= 1);

    context->globalScale = scale;

    s32 newWidth = (s32)(context->originalWindowWidth * context->globalScale);
    s32 newHeight = (s32)(context->originalWindowHeight * context->globalScale);
    setWindowSize(context, newWidth, newHeight);
}

void setGlobalSpeed(WarContext* context, f32 speed)
{
    assert(speed >= 1);

    context->globalSpeed = speed;
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
    setInputKey(context, WAR_KEY_CTRL, glfwGetKey(context->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                                       glfwGetKey(context->window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_SHIFT, glfwGetKey(context->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                                        glfwGetKey(context->window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_LEFT, glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_RIGHT, glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_UP, glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_DOWN, glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS);

    setInputKey(context, WAR_KEY_P, glfwGetKey(context->window, GLFW_KEY_P) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_T, glfwGetKey(context->window, GLFW_KEY_T) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_R, glfwGetKey(context->window, GLFW_KEY_R) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_U, glfwGetKey(context->window, GLFW_KEY_U) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_W, glfwGetKey(context->window, GLFW_KEY_W) == GLFW_PRESS);
    setInputKey(context, WAR_KEY_G, glfwGetKey(context->window, GLFW_KEY_G) == GLFW_PRESS);

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
    setInputKey(context, WAR_KEY_0, glfwGetKey(context->window, GLFW_KEY_0) == GLFW_PRESS ||
                                    glfwGetKey(context->window, GLFW_KEY_KP_0) == GLFW_PRESS);
}

void updateGame(WarContext* context)
{
    WarInput* input = &context->input;
    if (wasKeyPressed(input, WAR_KEY_P))
    {
        context->paused = !context->paused;
    }

    if (!context->paused)
    {
        updateMap(context);
    }
}

void renderGame(WarContext *context)
{
    NVGcontext* gfx = context->gfx;
    NVGLUframebuffer* fb = context->fb;

    s32 framebufferWidth = context->framebufferWidth;
    s32 framebufferHeight = context->framebufferHeight;

    s32 windowWidth = context->windowWidth;
    s32 windowHeight = context->windowHeight;

    f32 devicePixelRatio = context->devicePixelRatio;

    // render the whole scene to the FBO
    nvgluBindFramebuffer(fb);

    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(gfx, windowWidth, windowHeight, devicePixelRatio);
    renderMap(context);
    nvgEndFrame(gfx);

    nvgluBindFramebuffer(NULL);

    // then render a quad with the texture geneate with the FBO
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(gfx, windowWidth, windowHeight, devicePixelRatio);
    nvgSave(gfx);

    NVGpaint img = nvgImagePattern(gfx, 0, 0, windowWidth, windowHeight, 0, fb->image, 1.0f);
    nvgBeginPath(gfx);
    nvgRect(gfx, 0, 0, windowWidth, windowHeight);
    nvgFillPaint(gfx, img);
    nvgFill(gfx);
    
    nvgRestore(gfx);
    nvgEndFrame(gfx);
}

void presentGame(WarContext *context)
{
    glfwSwapBuffers(context->window);
    glfwPollEvents();

    f32 currentTime = glfwGetTime();
    context->deltaTime = (currentTime - context->time);

    // force to 60 fps
    while (context->deltaTime <= SECONDS_PER_FRAME)
    {
        currentTime = (f32)glfwGetTime();
        context->deltaTime = (currentTime - context->time);
    }

    context->time = currentTime;
    context->fps = (u32)(1.0f / context->deltaTime);
}