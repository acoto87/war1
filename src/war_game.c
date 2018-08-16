bool initGame(WarContext *context)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    context->globalScale = 4;
    context->originalWindowWidth = 320;
    context->originalWindowHeight = 200;
    context->windowWidth = (u32)(context->originalWindowWidth * context->globalScale);
    context->windowHeight = (u32)(context->originalWindowHeight * context->globalScale);
    context->windowTitle = "War 1";
    context->window = glfwCreateWindow(context->windowWidth, context->windowHeight, context->windowTitle, null, null);
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

void updateGame(WarContext *context)
{
    WarMap *map = context->map;
    if (!map) return;

    if (glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        map->dir[0] = 1;
    }
    else if (glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        map->dir[0] = -1;
    }
    else
    {
        map->dir[0] = 0;
    }

    if (glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        map->dir[1] = -1;
    }
    else if (glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        map->dir[1] = 1;
    }
    else
    {
        map->dir[1] = 0;
    }

    map->pos[0] += map->scrollSpeed * map->dir[0] * context->deltaTime;
    map->pos[0] = clamp(map->pos[0], -(MAP_WIDTH_PX - map->mapPanel.width), 0.0f);

    map->pos[1] += map->scrollSpeed * map->dir[1] * context->deltaTime;
    map->pos[1] = clamp(map->pos[1], -(MAP_HEIGHT_PX - map->mapPanel.height), 0.0f);

    // WarScene *scene = context->currentScene;
    // for (s32 i = 0; i < MAX_ENTITIES_COUNT; ++i)
    // {
    //     if (scene->entities[i].id && scene->entities[i].enabled)
    //     {
    //         // if (scene->entities[i].transform.enabled)
    //         // {
    //         //     scene->entities[i].transform.position[1] += 100*context->deltaTime;
    //         // }

    //         if (scene->entities[i].anim.enabled)
    //         {
    //             scene->entities[i].anim.offset += context->deltaTime * (1 / scene->entities[i].anim.duration);
    //             scene->entities[i].anim.offset = clamp(scene->entities[i].anim.offset, 0, 1);

    //             if (scene->entities[i].anim.offset == 1 && scene->entities[i].anim.loop)
    //             {
    //                 scene->entities[i].anim.offset = 0;
    //             }

    //             WarResource *resource = getOrCreateResource(context, scene->entities[i].sprite.resourceIndex);
    //             scene->entities[i].sprite.frameIndex = (s32)(scene->entities[i].anim.offset * (resource->spriteData.framesCount-1));
    //         }
    //     }
    // }
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