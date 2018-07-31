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

void createEmptyScene(WarContext *context)
{
    context->currentScene = (WarScene*)xcalloc(1, sizeof(WarScene));

    s32 entityIndex = createEntity(context, WAR_ENTITY_TYPE_IMAGE);    
    addSpriteComponent(context, entityIndex, 458); 

    // s32 entityIndex = createEntity(context, WAR_ENTITY_TYPE_UNIT);
    // addTransformComponent(context, entityIndex, (vec2){100, 100});
    // addSpriteComponent(context, entityIndex, 279);
    // addAnimationComponent(context, entityIndex, 10.0f, true);

    // for(s32 i = 0; i < 68; i++)
    // {
    //     entityIndex = createEntity(context, WAR_ENTITY_TYPE_SPRITE);
    //     addTransformComponent(context, entityIndex, (vec2){(i%10) * 100, ((s32)(i/10))*100});
    //     addSpriteComponent(context, entityIndex, 279 + i);
    //     addAnimationComponent(context, entityIndex, 10.0f, true);
    // }
}

void createMap(WarContext *context, s32 levelInfoIndex)
{
    assert(levelInfoIndex >= 0 && levelInfoIndex < MAX_RESOURCES_COUNT);

    WarResource *resource = context->resources[levelInfoIndex];
    assert(resource && resource->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    context->map = (WarMap*)xcalloc(1, sizeof(WarMap));
    context->map->levelInfoIndex = levelInfoIndex;
    context->map->tilesetType = MAP_TILESET_FOREST;
    
    for(s32 i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
    {
        context->map->tileStates[i] = MAP_TILE_STATE_UNKOWN;
    }
    
}

bool initGame(WarContext *context)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    context->windowWidth = 640;
    context->windowHeight = 400;
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
        }
    }

    //createEmptyScene(context);
    createMap(context, 117);
    
    context->time = glfwGetTime();
    return true;
}

vec2 dir;
vec2 pos;
s32 scrollSpeed = 200;

void updateGame(WarContext *context)
{
    if (glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        dir[0] = 1;
    }
    else if (glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        dir[0] = -1;
    }
    else
    {
        dir[0] = 0;
    }

    if (glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        dir[1] = 1;
    }
    else if (glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        dir[1] = -1;
    }
    else
    {
        dir[1] = 0;
    }

    pos[0] += scrollSpeed * dir[0] * context->deltaTime;
    pos[1] += scrollSpeed * dir[1] * context->deltaTime;

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

void renderEntity(WarContext *context, WarEntity *entity)
{
    if (entity->id && entity->enabled)
    {
        WarTransformComponent transform = entity->transform;
        if (transform.enabled)
        {
            mat4 model;
            glm_mat4_identity(model);
            glm_translate(model, transform.position);
            glUniformMatrix4fv(context->modelLocation, 1, GL_FALSE, (f32*)model);
        }

        WarSpriteComponent sprite = entity->sprite;
        if (sprite.enabled)
        {
            u32 w, h;
            u8 *pixels;

            WarResource *resource = getOrCreateResource(context, sprite.resourceIndex);
            switch(resource->type)
            {
                case WAR_RESOURCE_TYPE_IMAGE:
                {
                    w = resource->imageData.width;
                    h = resource->imageData.height;
                    pixels = resource->imageData.pixels;
                    break;
                }

                case WAR_RESOURCE_TYPE_SPRITE:
                {
                    w = resource->spriteData.frameWidth;
                    h = resource->spriteData.frameHeight;
                    pixels = resource->spriteData.frames[sprite.frameIndex].data;
                    break;
                }

                default:
                {
                    printf("Sprite component is enabled in a non-rendereable entity");
                    break;
                }
            }

            glBindTexture(GL_TEXTURE_2D, context->textures[sprite.textureIndex]);
            setTextureData(context, sprite.textureIndex, w, h, pixels);

            glBindVertexArray(sprite.vao);
            glBindBuffer(GL_ARRAY_BUFFER, sprite.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite.ibo);

            glEnableVertexAttribArray(context->positionLocation);
            glEnableVertexAttribArray(context->texCoordLocation);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, null);

            glDisableVertexAttribArray(context->texCoordLocation);
            glDisableVertexAttribArray(context->positionLocation);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

void renderScene(WarContext *context)
{
    WarScene *scene = context->currentScene;
    if (!scene) return;
    
    for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    {
        renderEntity(context, &scene->entities[i]);
    }
}

typedef struct
{
    vec2 position;
    vec2 textCoords;
} VertexData;

void renderMap(WarContext *context)
{
    WarMap *map = context->map;
    if (!map) return;
    
    WarResource *levelInfo = context->resources[map->levelInfoIndex];
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource *levelVisual = context->resources[63];
    assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);
    
    WarResource *tileset = context->resources[189];
    assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

    if (!context->textures[map->textureIndex])
    {
        map->textureIndex = createTexture(context);

        VertexData vertices[MAP_WIDTH * MAP_HEIGHT * 4];
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
                vertices[index * 4 + 0].position[0] = (x + 0) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 0].position[1] = (MAP_HEIGHT - y + 0) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 0].textCoords[0] = (f32)(tilePixelX) / TILESET_WIDTH_PX;
                vertices[index * 4 + 0].textCoords[1] = (f32)(tilePixelY + MEGA_TILE_HEIGHT) / TILESET_HEIGHT_PX;

                // vertex 10
                vertices[index * 4 + 1].position[0] = (x + 1) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 1].position[1] = (MAP_HEIGHT - y + 0) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 1].textCoords[0] = (f32)(tilePixelX + MEGA_TILE_WIDTH) / TILESET_WIDTH_PX;
                vertices[index * 4 + 1].textCoords[1] = (f32)(tilePixelY + MEGA_TILE_HEIGHT) / TILESET_HEIGHT_PX;

                // vertex 11
                vertices[index * 4 + 2].position[0] = (x + 1) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 2].position[1] = (MAP_HEIGHT - y + 1) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 2].textCoords[0] = (f32)(tilePixelX + MEGA_TILE_WIDTH) / TILESET_WIDTH_PX;
                vertices[index * 4 + 2].textCoords[1] = (f32)(tilePixelY) / TILESET_HEIGHT_PX;

                // vertex 01
                vertices[index * 4 + 3].position[0] = (x + 0) * MEGA_TILE_WIDTH;
                vertices[index * 4 + 3].position[1] = (MAP_HEIGHT - y + 1) * MEGA_TILE_HEIGHT;
                vertices[index * 4 + 3].textCoords[0] = (f32)(tilePixelX) / TILESET_WIDTH_PX;
                vertices[index * 4 + 3].textCoords[1] = (f32)(tilePixelY) / TILESET_HEIGHT_PX;

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

        // if this isn't generated glEnableVertexAttribArray set GL_INVALID_OPERATION error.
        glGenVertexArrays(1, &map->vao);
        glBindVertexArray(map->vao);
        
        glGenBuffers(1, &map->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
        glBufferData(GL_ARRAY_BUFFER, MAP_WIDTH * MAP_HEIGHT * 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &map->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, map->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,  MAP_WIDTH * MAP_HEIGHT * 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(context->positionLocation);
        glVertexAttribPointer(context->positionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glDisableVertexAttribArray(context->positionLocation);

        glEnableVertexAttribArray(context->texCoordLocation);
        glVertexAttribPointer(context->texCoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
        glDisableVertexAttribArray(context->texCoordLocation);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, pos);
    glUniformMatrix4fv(context->modelLocation, 1, GL_FALSE, (f32*)model);

    glBindTexture(GL_TEXTURE_2D, context->textures[map->textureIndex]);
    setTextureData(context, map->textureIndex, TILESET_WIDTH_PX, TILESET_HEIGHT_PX, tileset->tilesetData.data);

    glBindVertexArray(map->vao);
    glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, map->ibo);

    glEnableVertexAttribArray(context->positionLocation);
    glEnableVertexAttribArray(context->texCoordLocation);

    glDrawElements(GL_TRIANGLES, MAP_WIDTH * MAP_HEIGHT * 6, GL_UNSIGNED_INT, null);

    glDisableVertexAttribArray(context->texCoordLocation);
    glDisableVertexAttribArray(context->positionLocation);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    
    // for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    // {
    //     renderEntity(context, &map->entities[i]);
    // }
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
    glm_ortho(0.0f, context->windowWidth, 0.0f, context->windowHeight, -1.0f, 1.0f, proj);
    glUniformMatrix4fv(context->projLocation, 1, GL_FALSE, (f32*)proj);

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
        currentTime = glfwGetTime();
        context->deltaTime = (currentTime - context->time);
    }

    context->time = currentTime;
    context->fps = (u32)(1.0f / context->deltaTime);
}