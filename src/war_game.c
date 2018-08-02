internal bool initOpenGL(WarContext *context)
{
    context->vertexShader = loadShaderFromFile(".\\src\\basic.vert", GL_VERTEX_SHADER);
    if (!context->vertexShader)
    {
        printf("Unable to load the vertex shader!\n");
        return false;
    }

    context->fragmentShader = loadShaderFromFile(".\\src\\basic.frag", GL_FRAGMENT_SHADER);
    if (!context->fragmentShader)
    {
        printf("Unable to load the fragment shader!\n");
        return false;
    }

    context->shaderProgram = glCreateProgram();

    // Since a fragment shader is allowed to write to multiple buffers, you need to explicitly specify which output is written to which buffer. 
    // This needs to happen before linking the program. However, since this is 0 by default and there's only one output right now, 
    // the following line of code is not really necessary
    glBindFragDataLocation(context->shaderProgram, 0, "fragColor");

    glAttachShader(context->shaderProgram, context->vertexShader);
    glAttachShader(context->shaderProgram, context->fragmentShader);
    glLinkProgram(context->shaderProgram);
    if (!checkProgramLinkStatus(context->shaderProgram))
    {
        printProgramLog(context->shaderProgram);
        printf("Error linking program %d!\n", context->shaderProgram);
        return false;   
    }

    context->positionLocation = glGetAttribLocation(context->shaderProgram, "position");
    if (context->positionLocation == -1)
    {
        printf("position is not a valid glsl program variable!\n");
    }

    context->texCoordLocation = glGetAttribLocation(context->shaderProgram, "texCoord");
    if (context->texCoordLocation == -1)
    {
        printf("texCoord is not a valid glsl program variable!\n");
    }

    context->modelLocation = glGetUniformLocation(context->shaderProgram, "model");
    if (context->modelLocation == -1)
    {
        printf("model is not a valid glsl program variable!\n");
    }

    context->viewLocation = glGetUniformLocation(context->shaderProgram, "view");
    if (context->viewLocation == -1)
    {
        printf("view is not a valid glsl program variable!\n");
    }

    context->projLocation = glGetUniformLocation(context->shaderProgram, "proj");
    if (context->projLocation == -1)
    {
        printf("proj is not a valid glsl program variable!\n");
    }

    context->texLocation = glGetUniformLocation(context->shaderProgram, "tex");
    if (context->texLocation == -1)
    {
        printf("tex is not a valid glsl program variable!\n");
    }

    glClearColor(0.5f, 0.0f, 0.0f, 1.0);
    glClearDepth(1.0f);

    return true;
}

void createMap(WarContext *context, s32 levelInfoIndex)
{
    assert(levelInfoIndex >= 0 && levelInfoIndex < MAX_RESOURCES_COUNT);

    WarResource *resource = context->resources[levelInfoIndex];
    assert(resource && resource->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarMap *map = (WarMap*)xcalloc(1, sizeof(WarMap));
    map->levelInfoIndex = levelInfoIndex;
    map->tilesetType = MAP_TILESET_FOREST;
    map->scrollSpeed = 200;
    
    WarResource *levelInfo = context->resources[map->levelInfoIndex];
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    // create the map sprite
    {
        WarResource *levelVisual = context->resources[levelInfo->levelInfo.visualIndex];
        assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);
        
        WarResource *tileset = context->resources[levelInfo->levelInfo.tilesetIndex];
        assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

        WarVertex vertices[MAP_WIDTH * MAP_HEIGHT * 4];
        GLuint indices[MAP_WIDTH * MAP_HEIGHT * 6];

        for(s32 y = 0; y < MAP_HEIGHT; y++)
        {
            for(s32 x = 0; x < MAP_WIDTH; x++)
            {
                s32 index = y * MAP_WIDTH + x;
                u16 tileIndex = levelVisual->levelVisual.data[index];

                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                // vertex 00
                vertices[index * 4 + 0].position[0] = (f32)(x + 0) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 0].position[1] = (f32)(MAP_HEIGHT - 1 - y + 0) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 0].texCoords[0] = (f32)(tilePixelX) / TILESET_WIDTH_PX;
                vertices[index * 4 + 0].texCoords[1] = (f32)(tilePixelY + MEGA_TILE_HEIGHT) / TILESET_HEIGHT_PX;

                // vertex 10
                vertices[index * 4 + 1].position[0] = (f32)(x + 1) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 1].position[1] = (f32)(MAP_HEIGHT - 1 - y + 0) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 1].texCoords[0] = (f32)(tilePixelX + MEGA_TILE_WIDTH) / TILESET_WIDTH_PX;
                vertices[index * 4 + 1].texCoords[1] = (f32)(tilePixelY + MEGA_TILE_HEIGHT) / TILESET_HEIGHT_PX;

                // vertex 11
                vertices[index * 4 + 2].position[0] = (f32)(x + 1) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 2].position[1] = (f32)(MAP_HEIGHT - 1 - y + 1) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 2].texCoords[0] = (f32)(tilePixelX + MEGA_TILE_WIDTH) / TILESET_WIDTH_PX;
                vertices[index * 4 + 2].texCoords[1] = (f32)(tilePixelY) / TILESET_HEIGHT_PX;

                // vertex 01
                vertices[index * 4 + 3].position[0] = (f32)(x + 0) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 3].position[1] = (f32)(MAP_HEIGHT - 1 - y + 1) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 3].texCoords[0] = (f32)(tilePixelX) / TILESET_WIDTH_PX;
                vertices[index * 4 + 3].texCoords[1] = (f32)(tilePixelY) / TILESET_HEIGHT_PX;

                // indices for 1st triangle
                indices[index * 6 + 0] = index * 4 + 0;
                indices[index * 6 + 1] = index * 4 + 1;
                indices[index * 6 + 2] = index * 4 + 2;

                // indices for 2nd triangle
                indices[index * 6 + 3] = index * 4 + 0;
                indices[index * 6 + 4] = index * 4 + 2;
                indices[index * 6 + 5] = index * 4 + 3;
            }
        }

        map->sprite = createSprite(context, 
            MAP_WIDTH * MAP_HEIGHT, 
            TILESET_WIDTH_PX, 
            TILESET_HEIGHT_PX, 
            vertices, 
            MAP_WIDTH * MAP_HEIGHT * 4, 
            indices, 
            MAP_WIDTH * MAP_HEIGHT * 6);
    }

    // create the starting entities
    {
        for(s32 i = 0; i < levelInfo->levelInfo.startEntitiesCount; i++)
        {
            WarLevelUnit unit = levelInfo->levelInfo.startEntities[i];

            WarEntity *entity = createEntity(context, WAR_ENTITY_TYPE_UNIT);
            addTransformComponent(context, entity, (vec2){unit.x * MEGA_TILE_WIDTH, (MAP_HEIGHT - unit.y) * MEGA_TILE_HEIGHT});
            addUnitComponent(context, entity, unit.type, unit.x, unit.y, unit.player, unit.value);

            map->entities[i] = entity;
        }
    }

    // set the initial state for the tiles
    {
        for(s32 i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
        {
            map->tileStates[i] = MAP_TILE_STATE_UNKOWN;
        }
    }

    context->map = map;
}

bool initGame(WarContext *context)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    context->windowWidth = 320;
    context->windowHeight = 200;
    context->windowTitle = "War 1";
    context->window = glfwCreateWindow(context->windowWidth, context->windowHeight, context->windowTitle, null, null);
    if (!context->window)
    {
        printf("GLFW window could not be created!");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(context->window);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
        glfwDestroyWindow(context->window);
        glfwTerminate();
        return false;
    }

    if (!initOpenGL(context))
    {
        printf("Unable to initialize OpenGL!\n");
        glfwDestroyWindow(context->window);
        glfwTerminate();
        return false;
    }

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
        map->dir[1] = 1;
    }
    else if (glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        map->dir[1] = -1;
    }
    else
    {
        map->dir[1] = 0;
    }

    map->pos[0] += map->scrollSpeed * map->dir[0] * context->deltaTime;
    map->pos[1] += map->scrollSpeed * map->dir[1] * context->deltaTime;

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

void renderMap(WarContext *context)
{
    WarMap *map = context->map;
    if (!map) return;

    WarResource *levelInfo = context->resources[map->levelInfoIndex];
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, map->pos);

    // render terrain
    {
        WarResource *tileset = context->resources[levelInfo->levelInfo.tilesetIndex];
        assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

        renderSprite(context, &map->sprite, tileset->tilesetData.data, model);
    }

    // render entities
    {
        for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
        {
            if (map->entities[i])
            {
                renderEntity(context, map->entities[i], model);
            }

            break;
        }
    }
}

void renderGame(WarContext *context)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(context->shaderProgram);

    mat4 view;
    glm_mat4_identity(view);
    glUniformMatrix4fv(context->viewLocation, 1, GL_FALSE, (f32*)view);

    mat4 proj;
    glm_mat4_identity(proj);

    // this is a projection matrix to allow specify image dimensions in screen coordinates
    //
    // { 2/w,   0,  0, -1 }
    // {   0, 2/h,  0, -1 }
    // {   0,   0,  0,  0 }
    // {   0,   0,  0,  1 }
    //
    glm_ortho(0.0f, (f32)context->windowWidth, 0.0f, (f32)context->windowHeight, -1.0f, 1.0f, proj);
    glUniformMatrix4fv(context->projLocation, 1, GL_FALSE, (f32*)proj);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderScene(context);
    renderMap(context);

    glUseProgram(0);
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