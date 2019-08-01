void clearUIText(WarEntity* uiText)
{
    if (uiText->text.text)
    {
        free(uiText->text.text);
        uiText->text.text = NULL;
        uiText->text.enabled = false;
    }
}

void setUIText(WarEntity* uiText, s32 highlightIndex, const char* text)
{
    clearUIText(uiText);

    if (text)
    {
        uiText->text.text = (char*)xmalloc(strlen(text) + 1);
        uiText->text.highlightIndex = highlightIndex;
        strcpy(uiText->text.text, text);
        uiText->text.enabled = true;
    }
}

void setUITextFormatv(WarEntity* uiText, s32 highlightIndex, const char* format, va_list args)
{
    if (!format)
    {
        setUIText(uiText, highlightIndex, NULL);
        return;
    }

	char buffer[256];
    vsprintf(buffer, format, args);
    setUIText(uiText, highlightIndex, buffer);
}

void setUITextFormat(WarEntity* uiText, s32 highlightIndex, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    setUITextFormatv(uiText, highlightIndex, format, args);
    va_end(args);
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

void clearUITooltip(WarEntity* uiButton)
{
    memset(uiButton->button.tooltip, 0, sizeof(uiButton->button.tooltip));
}

void setUITooltip(WarEntity* uiButton, s32 highlightIndex, char* text)
{
    clearUITooltip(uiButton);

    if (text)
    {
        uiButton->button.highlightIndex = highlightIndex;
        strcpy(uiButton->button.tooltip, text);
    }
}

WarEntity* createUIText(WarContext* context, char* name, s32 fontIndex, f32 fontSize, const char* text, vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_TEXT, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addTextComponent(context, entity, fontIndex, fontSize, text);

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

WarEntity* createUICursor(WarContext* context, char* name, WarCursorType type, vec2 position)
{
    WarResource* resource = getOrCreateResource(context, type);
    assert(resource->type == WAR_RESOURCE_TYPE_CURSOR);

    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_CURSOR, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addSpriteComponentFromResource(context, entity, imageResourceRef(type));
    addCursorComponent(context, entity, type, vec2i(resource->cursor.hotx, resource->cursor.hoty));

    return entity;
}

WarEntity* createUITextButton(WarContext* context, 
                              char* name,
                              s32 fontIndex,
                              f32 fontSize,
                              const char* text,
                              WarSpriteResourceRef backgroundNormalRef,
                              WarSpriteResourceRef backgroundPressedRef,
                              WarSpriteResourceRef foregroundRef,
                              vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addTextComponent(context, entity, fontIndex, fontSize, text);
    addSpriteComponentFromResource(context, entity, foregroundRef);
    addButtonComponentFromResource(context, entity, backgroundNormalRef, backgroundPressedRef);

    return entity;
}

WarEntity* createUIImageButton(WarContext* context, 
                               char* name,
                               WarSpriteResourceRef backgroundNormalRef,
                               WarSpriteResourceRef backgroundPressedRef,
                               WarSpriteResourceRef foregroundRef,
                               vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_BUTTON, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);
    addSpriteComponentFromResource(context, entity, foregroundRef);
    addButtonComponentFromResource(context, entity, backgroundNormalRef, backgroundPressedRef);

    return entity;
}

WarEntity* createUIMinimap(WarContext* context, char* name, vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_MINIMAP, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);

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
        case WAR_ENTITY_TYPE_CURSOR:
        case WAR_ENTITY_TYPE_MINIMAP:
            return true;

        default:
            return false;
    }
}

void updateGoldText(WarContext* context)
{
    WarMap* map = context->map;

    WarEntity* txtGold = findUIEntity(context, "txtGold");
    assert(txtGold);

    s32 gold = map->players[0].gold;
    setUITextFormat(txtGold, NO_HIGHLIGHT, "GOLD:%*d", 6, gold);
}

void updateWoodText(WarContext* context)
{
    WarMap* map = context->map;

    WarEntity* txtWood = findUIEntity(context, "txtWood");
    assert(txtWood);

    s32 wood = map->players[0].wood;
    setUITextFormat(txtWood, NO_HIGHLIGHT, "LUMBER:%*d", 6, wood);
}

void setStatus(WarContext* context, s32 highlightIndex, s32 gold, s32 wood, char* text, ...)
{
    WarEntity* txtStatus = findUIEntity(context, "txtStatus");
    assert(txtStatus);

    WarEntity* imgStatusWood = findUIEntity(context, "imgStatusWood");
    assert(imgStatusWood);

    WarEntity* imgStatusGold = findUIEntity(context, "imgStatusGold");
    assert(imgStatusGold);

    WarEntity* txtStatusWood = findUIEntity(context, "txtStatusWood");
    assert(txtStatusWood);

    WarEntity* txtStatusGold = findUIEntity(context, "txtStatusGold");
    assert(txtStatusGold);

    va_list args;
    va_start (args, text);
    setUITextFormatv(txtStatus, highlightIndex, text, args);
    va_end (args);

    if (gold == 0 && wood == 0)
    {
        imgStatusWood->sprite.enabled = false;
        imgStatusGold->sprite.enabled = false;
        clearUIText(txtStatusWood);
        clearUIText(txtStatusGold);
    }
    else
    {
        imgStatusWood->sprite.enabled = true;
        imgStatusGold->sprite.enabled = true;
        setUITextFormat(txtStatusWood, NO_HIGHLIGHT, "%d", wood);
        setUITextFormat(txtStatusGold, NO_HIGHLIGHT, "%d", gold);
    }
}

void setFlashStatus(WarContext* context, f32 duration, char* text)
{
    WarMap* map = context->map;
    WarFlashStatus* flashStatus = &map->flashStatus;

    assert(duration >= 0);
    assert(text);

    flashStatus->enabled = true;
    flashStatus->startTime = context->time;
    flashStatus->duration = duration;
    strcpy(flashStatus->text, text);
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

void setManaBar(WarEntity* rectMagicBar, WarUnitComponent* unit)
{
#define MAGIC_BAR_WIDTH_PX 27

    f32 magicPercent = percentabf01(unit->mana, unit->maxMana);
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
    setUIText(txtUnitName, NO_HIGHLIGHT, NULL);

    // update the frame index of unit info/portraits 
    // based on the number of entities selected
    //
    // TODO: the max number of selected entities shouldn't greater than 4 but
    // that's not implemented right now, so put a min call to guard for that.
    s32 selectedEntitiesCount = min(map->selectedEntities.count, 4);
    if (selectedEntitiesCount > 1)
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
                WarUnitData unitsData = getUnitData(unit->type);
                setUIImage(imgUnitPortraits[i], unitsData.portraitFrameIndex);
                setLifeBar(rectLifeBars[i], unit);
            }
        }
    }
    else if (selectedEntitiesCount == 1)
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
                    setManaBar(rectMagicBar, unit);
                }
                else
                {
                    setUIImage(imgUnitInfo, 0);
                }
            }
            else if (isBuildingUnit(selectedEntity))
            {
                if (unit->building)
                {
                    setUIImage(imgUnitInfo, 2);
                    setPercentBar(rectPercentBar, rectPercentText, unit);
                }
                else
                {
                    setUIImage(imgUnitInfo, 0);
                }
            }

            WarUnitData unitsData = getUnitData(unit->type);
            setUIImage(imgUnitPortraits[0], unitsData.portraitFrameIndex);
            setUIText(txtUnitName, NO_HIGHLIGHT, unitsData.name);
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

void renderCommand(WarContext* context)
{
    WarMap* map = context->map;
    WarUnitCommand* command = &map->command;

    WarInput* input = &context->input;

    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    switch (command->type)
    {
        case WAR_COMMAND_BUILD_FARM_HUMANS:
        case WAR_COMMAND_BUILD_FARM_ORCS:
        case WAR_COMMAND_BUILD_BARRACKS_HUMANS:
        case WAR_COMMAND_BUILD_BARRACKS_ORCS:
        case WAR_COMMAND_BUILD_CHURCH:
        case WAR_COMMAND_BUILD_TEMPLE:
        case WAR_COMMAND_BUILD_TOWER_HUMANS:
        case WAR_COMMAND_BUILD_TOWER_ORCS:
        case WAR_COMMAND_BUILD_TOWNHALL_HUMANS:
        case WAR_COMMAND_BUILD_TOWNHALL_ORCS:
        case WAR_COMMAND_BUILD_LUMBERMILL_HUMANS:
        case WAR_COMMAND_BUILD_LUMBERMILL_ORCS:
        case WAR_COMMAND_BUILD_STABLE:
        case WAR_COMMAND_BUILD_KENNEL:
        case WAR_COMMAND_BUILD_BLACKSMITH_HUMANS:
        case WAR_COMMAND_BUILD_BLACKSMITH_ORCS:
        {
            vec2 position = vec2ScreenToMapCoordinates(context, input->pos);
            position = vec2MapToTileCoordinates(position);

            WarUnitType buildingToBuild = command->build.buildingToBuild;
            WarUnitData data = getUnitData(buildingToBuild);

            NVGcolor fillColor = checkRectToBuild(context, position.x, position.y, data.sizex, data.sizey) 
                ? NVG_GRAY_TRANSPARENT : NVG_RED_TRANSPARENT;

            position = vec2TileToMapCoordinates(position, false);
            position = vec2MapToScreenCoordinates(context, position);
            vec2 size = vec2i(data.sizex * MEGA_TILE_WIDTH, data.sizey * MEGA_TILE_HEIGHT);
            rect buildingRect = rectv(position, size);
            nvgFillRect(gfx, buildingRect, fillColor);

            break;
        }

        case WAR_COMMAND_BUILD_WALL:
        case WAR_COMMAND_BUILD_ROAD:
        {
            vec2 position = vec2ScreenToMapCoordinates(context, input->pos);
            position = vec2MapToTileCoordinates(position);

            NVGcolor fillColor = checkRectToBuild(context, position.x, position.y, 1, 1)
                ? NVG_GRAY_TRANSPARENT : NVG_RED_TRANSPARENT;

            position = vec2TileToMapCoordinates(position, false);
            position = vec2MapToScreenCoordinates(context, position);
            vec2 size = vec2i(MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect buildingRect = rectv(position, size);
            nvgFillRect(gfx, buildingRect, fillColor);

            break;
        }

        default:
        {
            // don't render the rest of the commands
            break;
        }
    }

    nvgRestore(gfx);
}

void renderUIEntities(WarContext* context)
{
    WarMap* map = context->map;

    WarEntityList* entities = getUIEntities(map);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity *entity = entities->items[i];
        if (entity)
        {
            renderEntity(context, entity);
        }
    }
}

void renderMapUI(WarContext* context)
{
    // WarMap* map = context->map;
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    renderSelectionRect(context);
    renderCommand(context);
    renderUIEntities(context);

    // DEBUG:
    // Render debug info
    {
        // char debugText[256];

        // sprintf(debugText, 
        //     "Debug info:\n"
        //     "speed: %.2f\n"
        //     "scale: %.2f\n"
        //     "editing trees = %d\n"
        //     "editing roads = %d\n"
        //     "editing walls = %d\n"
        //     "editing ruins = %d\n",
        //     context->globalSpeed,
        //     context->globalScale,
        //     map->editingTrees,
        //     map->editingRoads,
        //     map->editingWalls,
        //     map->editingRuins);

        // rect r = recti(map->mapPanel.x + 2, map->mapPanel.y + 2, 50, 50);
        // nvgFillRect(gfx, r, nvgRGBA(50, 50, 50, 200));

        // NVGfontParams params;
        // params.fontFace = "defaultFont";
        // params.fontSize = 5.0f;
        // params.fontColor = nvgRGBA(200, 200, 200, 255);

        // nvgMultilineText(gfx, debugText, r.x, r.y, r.width, r.height, params);
    }

    nvgRestore(gfx);
}