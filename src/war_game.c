bool initGame(WarContext *context)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    context->globalScale = 3;
    context->originalWindowWidth = 320;
    context->originalWindowHeight = 200;
    context->windowWidth = (u32)(context->originalWindowWidth * context->globalScale);
    context->windowHeight = (u32)(context->originalWindowHeight * context->globalScale);
    context->windowTitle = "War 1";
    context->window = glfwCreateWindow(context->windowWidth, context->windowHeight, context->windowTitle, NULL, NULL);
    if (!context->window)
    {
        fprintf(stderr, "GLFW window could not be created!");
        glfwTerminate();
        return false;
    }

    glfwGetWindowSize(context->window, &context->windowWidth, &context->windowHeight);
    glfwGetFramebufferSize(context->window, &context->framebufferWidth, &context->framebufferHeight);
    context->devicePixelRatio = (f32)context->framebufferWidth / (f32)context->windowWidth;

    glfwMakeContextCurrent(context->window);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        fprintf(stderr, "Error initializing GLEW! %s\n", glewGetErrorString(glewError));
        glfwDestroyWindow(context->window);
        glfwTerminate();
        return false;
    }

    // GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();

    context->gfx = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
    if (!context->gfx) 
    {
		fprintf(stderr, "Could not init nanovg.\n");
        glfwDestroyWindow(context->window);
        glfwTerminate();
		return -1;
	}

    glViewport(0, 0, context->framebufferWidth, context->framebufferHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    context->warFilePath = ".\\Build\\DATA.WAR";
    context->warFile = loadWarFile(context->warFilePath);

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
                printf("DB entries of type %d aren't handled yet\n", entry.type);
                break;
            }
        }
    }

    //createEmptyScene(context);
    createMap(context, 117);
    
    context->time = (f32)glfwGetTime();
    return true;
}

void inputGame(WarContext *context)
{
    f64 xpos, ypos;
    glfwGetCursorPos(context->window, &xpos, &ypos);

    context->input.x = floorf((f32)xpos);
    context->input.y = floorf((f32)ypos);
    
    // mouse buttons
    context->input.buttons[WAR_MOUSE_LEFT] = glfwGetMouseButton(context->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    context->input.buttons[WAR_MOUSE_RIGHT] = glfwGetMouseButton(context->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    
    // keyboard keys
    context->input.keys[WAR_KEY_LEFT] = glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS;
    context->input.keys[WAR_KEY_RIGHT] = glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    context->input.keys[WAR_KEY_UP] = glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS;
    context->input.keys[WAR_KEY_DOWN] = glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS;
}

void updateGame(WarContext *context)
{
    WarMap *map = context->map;
    if (!map) return;

    WarInput *input = &context->input;

    f32 dirX = 0, dirY = 0;

    if (input->keys[WAR_KEY_LEFT])
        dirX = -1;
    else if (input->keys[WAR_KEY_RIGHT])
        dirX = 1;

    if (input->keys[WAR_KEY_DOWN])
        dirY = 1;
    else if (input->keys[WAR_KEY_UP])
        dirY = -1;

    map->viewport.x += map->scrollSpeed * dirX * context->deltaTime;
    map->viewport.x = clamp(map->viewport.x, 0.0f, MAP_WIDTH - map->viewport.width);

    map->viewport.y += map->scrollSpeed * dirY * context->deltaTime;
    map->viewport.y = clamp(map->viewport.y, 0.0f, MAP_HEIGHT - map->viewport.height);

    if (input->buttons[WAR_MOUSE_LEFT])
    {
        f32 scale = context->globalScale;
        Rect scaledMinimapPanel = scaleRect(map->minimapPanel, scale);
        if (rectContains(scaledMinimapPanel, input->x, input->y))
        {
            f32 offsetx = input->x - scaledMinimapPanel.x;
            offsetx -= MINIMAP_VIEWPORT_WIDTH * scale/2;
            offsetx = clamp(offsetx, 0, scaledMinimapPanel.width - MINIMAP_VIEWPORT_WIDTH * scale);

            f32 offsety = input->y - scaledMinimapPanel.y;
            offsety -= MINIMAP_VIEWPORT_HEIGHT * scale/2;
            offsety = clamp(offsety, 0, scaledMinimapPanel.height - MINIMAP_VIEWPORT_HEIGHT * scale);

            map->viewport.x = offsetx * 1024 / (64 * scale);
            map->viewport.y = offsety * 1024 / (64 * scale);
        }
    }
}

void renderScene(WarContext *context)
{
    // WarScene *scene = context->currentScene;
    // if (!scene) return;
    
    // for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    // {
    //     renderEntity(context, &scene->entities[i]);
    // }
}

void renderGame(WarContext *context)
{
    NVGcontext *gfx = context->gfx;
    
    glViewport(0, 0, context->framebufferWidth, context->framebufferHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(gfx, (f32)context->windowWidth, (f32)context->windowHeight, context->devicePixelRatio);

    renderScene(context);
    renderMap(context);

    nvgEndFrame(gfx);
}

void presentGame(WarContext *context)
{
    glfwSwapBuffers(context->window);
    glfwPollEvents();

    f32 currentTime = glfwGetTime();
    context->deltaTime = (currentTime - context->time);

    // force to 60 fps
    while (context->deltaTime <= (1.0f/60.0f))
    {
        currentTime = (f32)glfwGetTime();
        context->deltaTime = (currentTime - context->time);
    }

    context->time = currentTime;
    context->fps = (u32)(1.0f / context->deltaTime);
}