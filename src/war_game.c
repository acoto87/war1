bool initGame(WarContext *context)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    initLog(LOG_SEVERITY_DEBUG);

    context->globalScale = 3;
    context->originalWindowWidth = 320;
    context->originalWindowHeight = 200;
    context->windowWidth = (u32)(context->originalWindowWidth * context->globalScale);
    context->windowHeight = (u32)(context->originalWindowHeight * context->globalScale);
    context->windowTitle = "War 1";
    context->window = glfwCreateWindow(context->windowWidth, context->windowHeight, context->windowTitle, NULL, NULL);

    if (!context->window)
    {
        logError("GLFW window could not be created!");
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
        logError("Error initializing GLEW! %s", glewGetErrorString(glewError));
        glfwDestroyWindow(context->window);
        glfwTerminate();
        return false;
    }

    // GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();

    context->gfx = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
    if (!context->gfx) 
    {
        logError("Could not init nanovg.");
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
                logInfo("DB entries of type %d aren't handled yet", entry.type);
                break;
            }
        }
    }

    //createEmptyScene(context);
    createMap(context, 117);
    
    context->time = (f32)glfwGetTime();
    return true;
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
}

void updateViewport(WarContext *context)
{
    WarMap *map = context->map;
    WarInput *input = &context->input;

    vec2 dir = VEC2_ZERO;

    if (isKeyPressed(input, WAR_KEY_LEFT))
        dir.x = -1;
    else if (isKeyPressed(input, WAR_KEY_RIGHT))
        dir.x = 1;

    if (isKeyPressed(input, WAR_KEY_DOWN))
        dir.y = 1;
    else if (isKeyPressed(input, WAR_KEY_UP))
        dir.y = -1;

    dir = vec2Normalize(dir);

    map->viewport.x += map->scrollSpeed * dir.x * context->deltaTime;
    map->viewport.x = clamp(map->viewport.x, 0.0f, MAP_WIDTH - map->viewport.width);

    map->viewport.y += map->scrollSpeed * dir.y * context->deltaTime;
    map->viewport.y = clamp(map->viewport.y, 0.0f, MAP_HEIGHT - map->viewport.height);
}

void updateGame(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    updateViewport(context);

    // process all state machines
    // for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    // {
    //     WarEntity* entity = map->entities[i];
    //     if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
    //     {
    //         updateStateMachine(context, entity);
    //     }
    // }

    // process all animations
    // for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    // {
    //     WarEntity* entity = map->entities[i];
    //     if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
    //     {
    //         updateAnimation(context, entity);
    //     }
    // }

    // process all actions
    for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    {
        WarEntity* entity = map->entities[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
        {
            updateAction(context, entity);
        }
    }

    for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
    {
        WarEntity* entity = map->entities[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
        {
            updateAnimations(context, entity);
        }
    }

    if (isButtonPressed(input, WAR_MOUSE_LEFT))
    {
        rect minimapPanel = map->minimapPanel;
        rect mapPanel = map->mapPanel;

        // check if the click is inside the minimap panel        
        if (rectContainsf(minimapPanel, input->pos.x, input->pos.y))
        {
            vec2 minimapSize = vec2i(MINIMAP_WIDTH, MINIMAP_HEIGHT);
            vec2 offset = vec2ScreenToMinimapCoordinates(context, input->pos);

            map->viewport.x = offset.x * MAP_WIDTH / minimapSize.x;
            map->viewport.y = offset.y * MAP_HEIGHT / minimapSize.y;
        }
        // check if the click is inside the map panel
        else if(rectContainsf(mapPanel, input->pos.x, input->pos.y))
        {
            if (!input->dragging)
            {
                input->dragPos = input->pos;
                input->dragging = true;
            }
        }
    }
    else if(wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        rect minimapPanel = map->minimapPanel;
        rect mapPanel = map->mapPanel;

        // check if the click is inside the map panel
        if(input->dragging || rectContainsf(mapPanel, input->pos.x, input->pos.y))
        {
            rect pointerRect = rectpf(input->dragPos.x, input->dragPos.y, input->pos.x, input->pos.y);
            pointerRect = rectScreenToMapCoordinates(context, pointerRect);

            for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
            {
                WarEntity *entity = map->entities[i];
                if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
                {
                    WarTransformComponent transform = entity->transform;
                    WarUnitComponent unit = entity->unit;
                    if (transform.enabled && unit.enabled)
                    {
                        rect unitRect = rectf(
                            transform.position.x,
                            transform.position.y,
                            unit.sizex * MEGA_TILE_WIDTH,
                            unit.sizey * MEGA_TILE_HEIGHT);

                        if (rectIntersects(pointerRect, unitRect))
                        {
                            map->selectedEntities[i] = true;
                        }
                        else if (!input->keys[WAR_KEY_CTRL].pressed)
                        {
                            map->selectedEntities[i] = false;
                        }
                    }
                }
            }
        }

        input->dragging = false;
    }

    if (wasButtonPressed(input, WAR_MOUSE_LEFT))
    {
        s32 selIndex = -1;
        WarEntity* selEntity = NULL;
        
        for(s32 i = 0; i < MAX_ENTITIES_COUNT; i++)
        {
            WarEntity* entity = map->entities[i];
            if (entity && entity->type == WAR_ENTITY_TYPE_UNIT && map->selectedEntities[i])
            {
                selIndex = i;
                selEntity = entity;
                break;
            }
        }

        if (selEntity)
        {
            WarUnitComponent* unit = &selEntity->unit;
            if (isDudeUnit(unit->type))
            {
                // movement stuff
            }

            if (isBuildingUnit(unit->type))
            {
                unit->hp -= unit->maxhp * 0.1f;
                if (unit->hp < 0) unit->hp = 0;

                f32 p = (f32)unit->hp / unit->maxhp;
                if (p <= 0)
                {
                    if (!containsAnimation(context, selEntity, "collapse"))
                    {
                        removeAnimation(context, selEntity, "hugeDamage");

                        selEntity->sprite.enabled = false;

                        vec2 frameSize = getUnitFrameSize(selEntity);
                        vec2 unitSize = getUnitSpriteSize(selEntity);

                        WarSprite sprite = createSpriteFromResourceIndex(context, BUILDING_COLLAPSE_RESOURCE);
                        WarSpriteAnimation* anim = createAnimation("collapse", sprite, 0.1f, false);

                        vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

                        // this is the scale of the explosion animation sprites with respect to the size of the building
                        f32 animScale = unitSize.x / animFrameSize.x;

                        // if the offset is based on the size of the frame, and it's scaled, then the offset must take into
                        // account the scale to make the calculations
                        f32 offsetx = halff(frameSize.x - unitSize.x);
                        f32 offsety = halff(frameSize.y - unitSize.y) - (animFrameSize.y * animScale - unitSize.y);

                        anim->scale = vec2f(animScale, animScale);
                        anim->offset = vec2f(offsetx, offsety);

                        for(s32 i = 0; i < 17; i++)
                            addAnimationFrame(anim, i);
                        
                        addAnimation(selEntity, anim);

                        // deselect the entity and remove it!
                        map->selectedEntities[selIndex] = false;
                        // removeEntity(context, selEntity);
                    }
                }
                else if (p < 0.3f)
                {
                    if (!containsAnimation(context, selEntity, "hugeDamage"))
                    {
                        removeAnimation(context, selEntity, "littleDamage");

                        WarSprite sprite = createSpriteFromResourceIndex(context, BUILDING_DAMAGE_2_RESOURCE);
                        WarSpriteAnimation* anim = createAnimation("hugeDamage", sprite, 0.2f, true);
                        anim->offset = vec2Subv(getUnitCenterPoint(selEntity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));
                        
                        for(s32 i = 0; i < 4; i++)
                            addAnimationFrame(anim, i);
                        
                        addAnimation(selEntity, anim);
                    }
                }
                else if(p < 0.7f)
                {
                    if (!containsAnimation(context, selEntity, "littleDamage"))
                    {
                        WarSprite sprite = createSpriteFromResourceIndex(context, BUILDING_DAMAGE_1_RESOURCE);
                        WarSpriteAnimation* anim = createAnimation("littleDamage", sprite, 0.2f, true);
                        anim->offset = vec2Subv(getUnitCenterPoint(selEntity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));
                        
                        for(s32 i = 0; i < 4; i++)
                            addAnimationFrame(anim, i);
                        
                        addAnimation(selEntity, anim);
                    }
                }
                
            }
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