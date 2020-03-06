void createMapUI(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    vec2 leftTopPanel = rectTopLeft(map->leftTopPanel);
    vec2 leftBottomPanel = rectTopLeft(map->leftBottomPanel);
    vec2 topPanel = rectTopLeft(map->topPanel);
    vec2 rightPanel = rectTopLeft(map->rightPanel);
    vec2 bottomPanel = rectTopLeft(map->bottomPanel);
    vec2 minimapPanel = rectTopLeft(map->minimapPanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef normalRef = imageResourceRef(364);
    WarSpriteResourceRef pressedRef = imageResourceRef(365);
    WarSpriteResourceRef portraitsRef = imageResourceRef(361);

    WarCheatStatus* cheatStatus = &map->cheatStatus;
    cheatStatus->enabled = true;
    cheatStatus->visible = false;
    cheatStatus->position = 0;
    memset(cheatStatus->text, 0, sizeof(cheatStatus->text));

    WarEntity* uiEntity;

    // panels
    createUIImage(context, "panelLeftTop", imageResourceRefFromPlayer(player, 224, 225), leftTopPanel);
    createUIImage(context, "panelLeftBottom", imageResourceRefFromPlayer(player, 226, 227), leftBottomPanel);
    createUIImage(context, "panelTop", imageResourceRefFromPlayer(player, 218, 219), topPanel);
    createUIImage(context, "panelRight", imageResourceRefFromPlayer(player, 220, 221), rightPanel);
    createUIImage(context, "panelBottom", imageResourceRefFromPlayer(player, 222, 223), bottomPanel);

    // minimap
    createUIMinimap(context, "minimap", minimapPanel);

    // top panel images
    createUIImage(context, "imgGold", imageResourceRef(406), vec2Addv(topPanel, vec2i(201, 1)));
    createUIImage(context, "imgLumber", imageResourceRef(407), vec2Addv(topPanel, vec2i(102, 0)));

    // top panel texts
    createUIText(context, "txtGold", 0, 6, NULL, vec2Addv(topPanel, vec2i(135, 2)));
    createUIText(context, "txtWood", 0, 6, NULL, vec2Addv(topPanel, vec2i(24, 2)));

    // status text
    createUIText(context, "txtStatus", 0, 6, NULL, vec2Addv(bottomPanel, vec2i(2, 5)));
    createUIImage(context, "imgStatusWood", imageResourceRef(407), vec2Addv(bottomPanel, vec2i(163, 3)));
    createUIImage(context, "imgStatusGold", imageResourceRef(406), vec2Addv(bottomPanel, vec2i(200, 5)));
    createUIText(context, "txtStatusWood", 0, 6, NULL, vec2Addv(bottomPanel, vec2i(179, 5)));
    createUIText(context, "txtStatusGold", 0, 6, NULL, vec2Addv(bottomPanel, vec2i(218, 5)));
    createUIRect(context, "txtStatusCursor", vec2Addv(bottomPanel, vec2i(2, 4)), vec2i(1, 7), U8COLOR_WHITE);

    uiEntity = createUIText(context, "txtCheatFeedbackText", 1, 8, NULL, vec2Addv(bottomPanel, vec2i(15, -20)));
    setUITextColor(uiEntity, U8COLOR_YELLOW);
    setUIEntityStatus(uiEntity, false);

    // selected unit(s) info
    createUIImage(context, "imgUnitInfo", imageResourceRefFromPlayer(player, 360, 359), vec2Addv(leftBottomPanel, vec2i(2, 0)));
    createUIImage(context, "imgUnitPortrait0", portraitsRef, vec2Addv(leftBottomPanel, vec2i(6, 4)));
    createUIImage(context, "imgUnitPortrait1", portraitsRef, vec2Addv(leftBottomPanel, vec2i(4, 1)));
    createUIImage(context, "imgUnitPortrait2", portraitsRef, vec2Addv(leftBottomPanel, vec2i(38, 1)));
    createUIImage(context, "imgUnitPortrait3", portraitsRef, vec2Addv(leftBottomPanel, vec2i(4, 23)));
    createUIImage(context, "imgUnitPortrait4", portraitsRef, vec2Addv(leftBottomPanel, vec2i(38, 23)));
    createUIImage(context, "imgUnitInfoLife", imageResourceRefFromPlayer(player, 360, 359), vec2Addv(leftBottomPanel, vec2i(3, 16)));
    createUIText(context, "txtUnitName", 0, 6, NULL, vec2Addv(leftBottomPanel, vec2i(6, 26)));
    createUIRect(context, "rectLifeBar0", vec2Addv(leftBottomPanel, vec2i(37, 20)), vec2i(27, 3), U8COLOR_GREEN);
    createUIRect(context, "rectLifeBar1", vec2Addv(leftBottomPanel, vec2i(4, 17)), vec2i(27, 3), U8COLOR_GREEN);
    createUIRect(context, "rectLifeBar2", vec2Addv(leftBottomPanel, vec2i(38, 17)), vec2i(27, 3), U8COLOR_GREEN);
    createUIRect(context, "rectLifeBar3", vec2Addv(leftBottomPanel, vec2i(4, 39)), vec2i(27, 3), U8COLOR_GREEN);
    createUIRect(context, "rectLifeBar4", vec2Addv(leftBottomPanel, vec2i(38, 39)), vec2i(27, 3), U8COLOR_GREEN);
    createUIRect(context, "rectMagicBar", vec2Addv(leftBottomPanel, vec2i(37, 9)), vec2i(27, 3), U8COLOR_GREEN);
    createUIRect(context, "rectPercentBar", vec2Addv(leftBottomPanel, vec2i(4, 37)), vec2i(62, 5), U8COLOR_GREEN);
    createUIImage(context, "rectPercentText", imageResourceRef(410), vec2Addv(leftBottomPanel, vec2i(15, 37)));

    // texts in the command area
    createUIText(context, "txtCommand0", 0, 6, NULL, vec2Addv(leftBottomPanel, vec2i(3, 46)));
    createUIText(context, "txtCommand1", 0, 6, NULL, vec2Addv(leftBottomPanel, vec2i(3, 56)));
    createUIText(context, "txtCommand2", 0, 6, NULL, vec2Addv(leftBottomPanel, vec2i(7, 64)));
    createUIText(context, "txtCommand3", 0, 6, NULL, vec2Addv(leftBottomPanel, vec2i(11, 54)));

    // command buttons
    createUIImageButton(
        context, "btnCommand0",
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(2, 44)));

    createUIImageButton(
        context, "btnCommand1",
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(36, 44)));

    createUIImageButton(
        context, "btnCommand2",
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(2, 67)));

    createUIImageButton(
        context, "btnCommand3",
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(36, 67)));

    createUIImageButton(
        context, "btnCommand4",
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(2, 90)));

    createUIImageButton(
        context, "btnCommand5",
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(36, 90)));

    uiEntity = createUIImageButton(
        context, "btnMenu",
        imageResourceRef(362),
        imageResourceRef(363),
        invalidRef,
        vec2Addv(leftBottomPanel, vec2i(3, 116)));
    setUITooltip(uiEntity, 6, 3, "MENU (F10)");
    setUIButtonClickHandler(uiEntity, handleMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_F10);
}

WarEntity* createUIMinimap(WarContext* context, char* name, vec2 position)
{
    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_MINIMAP, true);
    addTransformComponent(context, entity, position);
    addUIComponent(context, entity, name);

    return entity;
}

void updateGoldText(WarContext* context)
{
    WarMap* map = context->map;

    WarEntity* txtGold = findUIEntity(context, "txtGold");
    assert(txtGold);

    s32 gold = map->players[0].gold;
    setUITextFormat(txtGold, "GOLD:%*d", 6, gold);
    setUITextHighlight(txtGold, NO_HIGHLIGHT, 0);
}

void updateWoodText(WarContext* context)
{
    WarMap* map = context->map;

    WarEntity* txtWood = findUIEntity(context, "txtWood");
    assert(txtWood);

    s32 wood = map->players[0].wood;
    setUITextFormat(txtWood, "LUMBER:%*d", 6, wood);
    setUITextHighlight(txtWood, NO_HIGHLIGHT, 0);
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
    setUITextHighlight(txtUnitName, NO_HIGHLIGHT, 0);

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
            setUIText(txtUnitName, unitsData.name);
            setUITextHighlight(txtUnitName, NO_HIGHLIGHT, 0);
            setLifeBar(rectLifeBars[0], unit);
        }
    }
}

void setStatus(WarContext* context, s32 highlightIndex, s32 highlightCount, s32 gold, s32 wood, char* text, ...)
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
    setUITextFormatv(txtStatus, text, args);
    va_end (args);

    setUITextHighlight(txtStatus, highlightIndex, highlightCount);

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
        setUITextFormat(txtStatusWood, "%d", wood);
        setUITextHighlight(txtStatusWood, NO_HIGHLIGHT, 0);
        setUITextFormat(txtStatusGold, "%d", gold);
        setUITextHighlight(txtStatusGold, NO_HIGHLIGHT, 0);
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

void renderMapUI(WarContext* context)
{
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);

    renderSelectionRect(context);
    renderCommand(context);
    renderUIEntities(context);

    nvgRestore(gfx);
}