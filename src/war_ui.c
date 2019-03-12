void clearUIText(WarEntity* entity)
{
    if (entity->text.text)
    {
        free(entity->text.text);
        entity->text.text = NULL;
        entity->text.enabled = false;
    }
}

void setUIText(WarEntity* entity, char* text)
{
    clearUIText(entity);

    if (text)
    {
        entity->text.text = (char*)xmalloc(strlen(text) + 1);
        strcpy(entity->text.text, text);
        entity->text.enabled = true;
    }
}

void setUIImage(WarEntity* uiImage, s32 frameIndex)
{
    uiImage->sprite.frameIndex = frameIndex;
    uiImage->sprite.enabled = frameIndex >= 0;
}

void setUIRectWidth(WarEntity* uiRect, s32 width)
{
    uiRect->rect.size.x = width;
    uiRect->rect.enabled = width > 0;
}

WarEntity* createUIText(WarContext* context, char* name, vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_TEXT, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addTextComponent(context, entity, NULL);
    entity->text.shadowBlur = 1.0f;
    entity->text.shadowOffset = VEC2_ONE;

    return entity;
}

WarEntity* createUIRect(WarContext* context, char* name, vec2 position, vec2 size, u8Color color)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_RECT, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addRectComponent(context, entity, size, color);

    return entity;
}

WarEntity* createUIImage(WarContext* context, char* name, WarSpriteResourceRef spriteResourceRef, vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_IMAGE, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addSpriteComponentFromResource(context, entity, spriteResourceRef);

    return entity;
}

WarEntity* createUITextButton(WarContext* context, 
                              char* name,
                              char* tooltip,
                              char* text,
                              WarSpriteResourceRef backgroundNormalRef,
                              WarSpriteResourceRef backgroundPressedRef,
                              vec2 position,
                              WarClickHandler clickHandler)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addTextButtonComponentFromResource(context, 
                                       entity, 
                                       backgroundNormalRef, 
                                       backgroundPressedRef, 
                                       text, 
                                       tooltip,
                                       clickHandler);

    return entity;
}

WarEntity* createUIImageButton(WarContext* context, 
                               char* name,
                               char* tooltip,
                               WarSpriteResourceRef backgroundNormalRef,
                               WarSpriteResourceRef backgroundPressedRef,
                               WarSpriteResourceRef foregroundRef,
                               vec2 position,
                               WarClickHandler clickHandler)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addImageButtonComponentFromResource(context, 
                                        entity, 
                                        backgroundNormalRef, 
                                        backgroundPressedRef, 
                                        foregroundRef, 
                                        tooltip,
                                        clickHandler);

    return entity;
}

bool isUIEntity(WarEntity* entity)
{
    switch (entity->type)
    {
        case WAR_ENTITY_TYPE_IMAGE:
        case WAR_ENTITY_TYPE_TEXT:
        case WAR_ENTITY_TYPE_RECT:
        case WAR_ENTITY_TYPE_BUTTON:
            return true;

        default:
            return false;
    }
}

void updateGoldText(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* txtGold = findUIEntity(context, "txtGold");
    assert(txtGold);

    char buffer[20];
    sprintf(buffer, "GOLD: %d", map->players[0].gold);
    setUIText(txtGold, buffer);
}

void updateWoodText(WarContext* context)
{
    WarMap* map = context->map;
    assert(map);

    WarEntity* txtWood = findUIEntity(context, "txtWood");
    assert(txtWood);

    char buffer[20];
    sprintf(buffer, "LUMBER: %d", map->players[0].wood);
    setUIText(txtWood, buffer);
}

void setStatusText(WarContext* context, char* text)
{
    WarEntity* txtStatus = findUIEntity(context, "txtStatus");
    assert(txtStatus);

    setUIText(txtStatus, text);
}

void setLifeBar(WarEntity* rectLifeBar, WarUnitComponent* unit)
{
#define LIFE_BAR_RED_THRESHOLD 0.35f
#define LIFE_BAR_YELLOW_THRESHOLD 0.70f
#define LIFE_BAR_WIDTH_PX 27

    f32 hpPercent = percentabf01(unit->hp, unit->maxhp);

    if (hpPercent <= LIFE_BAR_RED_THRESHOLD)
        rectLifeBar->rect.color = U8COLOR_RED;
    else if (hpPercent <= LIFE_BAR_YELLOW_THRESHOLD)
        rectLifeBar->rect.color = U8COLOR_YELLOW;
    else
        rectLifeBar->rect.color = U8COLOR_GREEN;

    setUIRectWidth(rectLifeBar, (s32)(hpPercent * LIFE_BAR_WIDTH_PX));
}

void setMagicBar(WarEntity* rectMagicBar, WarUnitComponent* unit)
{
#define MAGIC_BAR_WIDTH_PX 27

    f32 magicPercent = percentabf01(unit->magic, unit->maxMagic);
    setUIRectWidth(rectMagicBar, (s32)(magicPercent * MAGIC_BAR_WIDTH_PX));
}

void setPercentBar(WarEntity* rectPercentBar, WarEntity* rectPercentText, WarUnitComponent* unit)
{
#define PERCENT_BAR_WIDTH_PX 64

    f32 percent = unit->buildPercent;
    
    setUIRectWidth(rectPercentBar, (s32)(percent * PERCENT_BAR_WIDTH_PX));
    setUIImage(rectPercentText, 0);
}

void updateSelectedUnitsInfo(WarContext* context)
{
    WarMap* map = context->map;

    // retrieve entities of sprites of unit info/portraits
    WarEntity* imgUnitInfo = findUIEntity(context, "imgUnitInfo");
    assert(imgUnitInfo);

    WarEntity* imgUnitInfoLife = findUIEntity(context, "imgUnitInfoLife");
    assert(imgUnitInfoLife);

    char uiEntityName[20];
    WarEntity* imgUnitPortraits[5];
    WarEntity* rectLifeBars[5];
    for (s32 i = 0; i < 5; i++)
    {
        sprintf(uiEntityName, "imgUnitPortrait%d", i);
        imgUnitPortraits[i] = findUIEntity(context, uiEntityName);
        assert(imgUnitPortraits[i]);

        sprintf(uiEntityName, "rectLifeBar%d", i);
        rectLifeBars[i] = findUIEntity(context, uiEntityName);
        assert(rectLifeBars[i]);
    }

    WarEntity* rectMagicBar = findUIEntity(context, "rectMagicBar");
    assert(rectMagicBar);

    WarEntity* rectPercentBar = findUIEntity(context, "rectPercentBar");
    assert(rectPercentBar);

    WarEntity* rectPercentText = findUIEntity(context, "rectPercentText");
    assert(rectPercentText);

    WarEntity* txtUnitName = findUIEntity(context, "txtUnitName");
    assert(txtUnitName);

    // reset frame index of the sprites of unit info/portraits
    setUIImage(imgUnitInfo, -1);
    setUIImage(imgUnitInfoLife, -1);

    for (s32 i = 0; i < 5; i++)
    {
        setUIImage(imgUnitPortraits[i], -1);
        setUIRectWidth(rectLifeBars[i], 0);
    }

    setUIRectWidth(rectMagicBar, 0);
    setUIRectWidth(rectPercentBar, 0);
    setUIImage(rectPercentText, -1);
    setUIText(txtUnitName, NULL);

    // update the frame index of unit info/portraits 
    // based on the number of entities selected
    //
    // TODO: the max number of selected entities shouldn't greater than 4 but
    // that's not implemented right now, so put a min call to guard for that.
    s32 selectedEntitiesCount = mini(map->selectedEntities.count, 4);
    if (selectedEntitiesCount >= 2)
    {
        // for 4 units selected -> frame indices 5, 8
        // for 3 units selected -> frame indices 4, 7
        // for 2 units selected -> frame indices 3, 6
        setUIImage(imgUnitInfo, selectedEntitiesCount + 1);
        setUIImage(imgUnitInfoLife, selectedEntitiesCount + 4);

        for (s32 i = 1; i <= selectedEntitiesCount; i++)
        {
            WarEntityId selectedEntityId = map->selectedEntities.items[i - 1];
            WarEntity* selectedEntity = findEntity(context, selectedEntityId);
            if (selectedEntity && isUnit(selectedEntity))
            {
                WarUnitComponent* unit = &selectedEntity->unit;
                WarUnitsData unitsData = getUnitsData(unit->type);
                setUIImage(imgUnitPortraits[i], unitsData.portraitFrameIndex);
                setLifeBar(rectLifeBars[i], unit);
            }
        }
    }
    else if (selectedEntitiesCount >= 1)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[0];
        WarEntity* selectedEntity = findEntity(context, selectedEntityId);
        if (selectedEntity && isUnit(selectedEntity))
        {
            WarUnitComponent* unit = &selectedEntity->unit;

            if (isDudeUnit(selectedEntity))
            {
                if (isMagicUnit(selectedEntity))
                {
                    setUIImage(imgUnitInfo, 1);
                    setMagicBar(rectMagicBar, unit);
                }
                else
                {
                    setUIImage(imgUnitInfo, 0);
                }
            }
            else if (isBuildingUnit(selectedEntity))
            {
                if (!unit->building)
                {
                    setUIImage(imgUnitInfo, 2);
                    setPercentBar(rectPercentBar, rectPercentText, unit);
                }
                else
                {
                    setUIImage(imgUnitInfo, 0);
                }
            }

            WarUnitsData unitsData = getUnitsData(unit->type);
            setUIImage(imgUnitPortraits[0], unitsData.portraitFrameIndex);
            setUIText(txtUnitName, unitsData.name);
            setLifeBar(rectLifeBars[0], unit);
        }
    }
}

void renderSelectionRect(WarContext* context)
{
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    WarInput* input = &context->input;
    if (input->isDragging)
    {
        rect pointerRect = rectpf(input->dragPos.x, input->dragPos.y, input->pos.x, input->pos.y);
        nvgStrokeRect(gfx, pointerRect, NVG_GREEN_SELECTION, 1.0f);
    }

    nvgRestore(gfx);
}

void renderPanels(WarContext* context)
{
    WarMap* map = context->map;
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    for(s32 i = 0; i < map->entities.count; i++)
    {
        WarEntity *entity = map->entities.items[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_IMAGE)
        {
            renderEntity(context, entity, false);
        }
    }

    nvgRestore(gfx);
}

void renderMinimap(WarContext* context)
{
    WarMap* map = context->map;
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    // copy the minimap base to the first frame which is the one that will be rendered
    memcpy(map->minimapSprite.frames[0].data, map->minimapSprite.frames[1].data, MINIMAP_WIDTH * MINIMAP_HEIGHT * 4);

    for(s32 i = 0; i < map->entities.count; i++)
    {
        u8 r = 211, g = 211, b = 211;

        WarEntity* entity = map->entities.items[i];
        if (entity && entity->type == WAR_ENTITY_TYPE_UNIT)
        {
            WarUnitComponent unit = entity->unit;

            if (unit.type == WAR_UNIT_TOWNHALL_HUMANS || 
                unit.type == WAR_UNIT_TOWNHALL_ORCS)
            {
                r = 255; g = 255; b = 0;
            }
            else if(unit.player == 0)
            {
                r = 0; g = 199; b = 0;
            }
            else if(unit.player < 4)
            {
                r = 199; g = 0; b = 0;
            }

            WarTransformComponent transform = entity->transform;
            s32 tileX = (s32)(transform.position.x/MEGA_TILE_WIDTH);
            s32 tileY = (s32)(transform.position.y/MEGA_TILE_HEIGHT);

            for(s32 y = 0; y < unit.sizey; y++)
            {
                for(s32 x = 0; x < unit.sizex; x++)
                {
                    s32 pixel = (tileY + y) * MINIMAP_WIDTH + (tileX + x);
                    map->minimapSprite.frames[0].data[pixel * 4 + 0] = r;
                    map->minimapSprite.frames[0].data[pixel * 4 + 1] = g;
                    map->minimapSprite.frames[0].data[pixel * 4 + 2] = b;
                }
            }
        }
    }

    nvgTranslate(gfx, map->minimapPanel.x, map->minimapPanel.y);

    updateSpriteImage(context, &map->minimapSprite, map->minimapSprite.frames[0].data);
    renderSprite(context, &map->minimapSprite, VEC2_ZERO, VEC2_ONE);

    nvgRestore(gfx);
}

void renderMinimapViewport(WarContext* context)
{
    WarMap* map = context->map;
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    nvgTranslate(gfx, map->minimapPanel.x, map->minimapPanel.y);
    nvgTranslate(gfx, map->viewport.x * MINIMAP_MAP_WIDTH_RATIO, map->viewport.y * MINIMAP_MAP_HEIGHT_RATIO);

    nvgStrokeRect(gfx, recti(0, 0, MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT), NVG_WHITE, 1.0f/context->globalScale);

    nvgRestore(gfx);
}

void renderMapUI(WarContext* context)
{
    WarMap* map = context->map;
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    // render selection rect
    renderSelectionRect(context);

    // render ui elements
    {
        for(s32 i = 0; i < map->entities.count; i++)
        {
            WarEntity *entity = map->entities.items[i];
            if (entity && isUIEntity(entity))
            {
                renderEntity(context, entity, false);
            }
        }
    }

    // render minimap
    renderMinimap(context);
    renderMinimapViewport(context);

    // DEBUG:
    // Render debug info
    {
        char debugText[256];

        sprintf(debugText, 
            "Debug info:\n"
            "speed: %.2f\n"
            "scale: %.2f\n"
            "editing trees = %d\n"
            "editing roads = %d\n"
            "editing walls = %d\n"
            "editing ruins = %d\n",
            context->globalSpeed,
            context->globalScale,
            context->editingTrees,
            context->editingRoads,
            context->editingWalls,
            context->editingRuins);

        rect r = recti(map->mapPanel.x + 2, map->mapPanel.y + 2, 50, 50);
        nvgFillRect(gfx, r, nvgRGBA(50, 50, 50, 200));

        NVGfontParams params = nvgCreateFontParams("defaultFont", 5.0f, nvgRGBA(200, 200, 200, 255));
        nvgMultilineText(gfx, debugText, r.x, r.y, r.width, r.height, params);
    }

    nvgRestore(gfx);
}