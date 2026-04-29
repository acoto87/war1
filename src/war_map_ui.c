#include "war_map_ui.h"

#include "shl/wstr.h"

#include "war_entities.h"
#include "war_map.h"
#include "war_ui.h"
#include "war_units.h"

void wmui_createMapUI(WarContext* context)
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
    wstr_clear(&cheatStatus->text);

    WarEntity* uiEntity;

    // panels
    wui_createUIImage(context, wstr_fromCString("panelLeftTop"), imageResourceRefFromPlayer(player, 224, 225), leftTopPanel);
    wui_createUIImage(context, wstr_fromCString("panelLeftBottom"), imageResourceRefFromPlayer(player, 226, 227), leftBottomPanel);
    wui_createUIImage(context, wstr_fromCString("panelTop"), imageResourceRefFromPlayer(player, 218, 219), topPanel);
    wui_createUIImage(context, wstr_fromCString("panelRight"), imageResourceRefFromPlayer(player, 220, 221), rightPanel);
    wui_createUIImage(context, wstr_fromCString("panelBottom"), imageResourceRefFromPlayer(player, 222, 223), bottomPanel);

    // minimap
    wmui_createUIMinimap(context, wstr_fromCString("minimap"), minimapPanel);

    // top panel images
    wui_createUIImage(context, wstr_fromCString("imgGold"), imageResourceRef(406), vec2Addv(topPanel, vec2i(201, 1)));
    wui_createUIImage(context, wstr_fromCString("imgLumber"), imageResourceRef(407), vec2Addv(topPanel, vec2i(102, 0)));

    // top panel texts
    wui_createUIText(context, wstr_fromCString("txtGold"), 0, 6, wstr_make(), vec2Addv(topPanel, vec2i(135, 2)));
    wui_createUIText(context, wstr_fromCString("txtWood"), 0, 6, wstr_make(), vec2Addv(topPanel, vec2i(24, 2)));

    // status text
    wui_createUIText(context, wstr_fromCString("txtStatus"), 0, 6, wstr_make(), vec2Addv(bottomPanel, vec2i(2, 5)));
    wui_createUIImage(context, wstr_fromCString("imgStatusWood"), imageResourceRef(407), vec2Addv(bottomPanel, vec2i(163, 3)));
    wui_createUIImage(context, wstr_fromCString("imgStatusGold"), imageResourceRef(406), vec2Addv(bottomPanel, vec2i(200, 5)));
    wui_createUIText(context, wstr_fromCString("txtStatusWood"), 0, 6, wstr_make(), vec2Addv(bottomPanel, vec2i(179, 5)));
    wui_createUIText(context, wstr_fromCString("txtStatusGold"), 0, 6, wstr_make(), vec2Addv(bottomPanel, vec2i(218, 5)));
    wui_createUIRect(context, wstr_fromCString("txtStatusCursor"), vec2Addv(bottomPanel, vec2i(2, 4)), vec2i(1, 7), WAR_COLOR_WHITE);

    uiEntity = wui_createUIText(context, wstr_fromCString("txtCheatFeedbackText"), 1, 8, wstr_make(), vec2Addv(bottomPanel, vec2i(15, -20)));
    setUITextColor(uiEntity, WAR_COLOR_YELLOW);
    setUIEntityStatus(uiEntity, false);

    // selected unit(s) info
    wui_createUIImage(context, wstr_fromCString("imgUnitInfo"), imageResourceRefFromPlayer(player, 360, 359), vec2Addv(leftBottomPanel, vec2i(2, 0)));
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait0"), portraitsRef, vec2Addv(leftBottomPanel, vec2i(6, 4)));
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait1"), portraitsRef, vec2Addv(leftBottomPanel, vec2i(4, 1)));
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait2"), portraitsRef, vec2Addv(leftBottomPanel, vec2i(38, 1)));
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait3"), portraitsRef, vec2Addv(leftBottomPanel, vec2i(4, 23)));
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait4"), portraitsRef, vec2Addv(leftBottomPanel, vec2i(38, 23)));
    wui_createUIImage(context, wstr_fromCString("imgUnitInfoLife"), imageResourceRefFromPlayer(player, 360, 359), vec2Addv(leftBottomPanel, vec2i(3, 16)));
    wui_createUIText(context, wstr_fromCString("txtUnitName"), 0, 6, wstr_make(), vec2Addv(leftBottomPanel, vec2i(6, 26)));
    wui_createUIRect(context, wstr_fromCString("rectLifeBar0"), vec2Addv(leftBottomPanel, vec2i(37, 20)), vec2i(27, 3), WAR_COLOR_GREEN);
    wui_createUIRect(context, wstr_fromCString("rectLifeBar1"), vec2Addv(leftBottomPanel, vec2i(4, 17)), vec2i(27, 3), WAR_COLOR_GREEN);
    wui_createUIRect(context, wstr_fromCString("rectLifeBar2"), vec2Addv(leftBottomPanel, vec2i(38, 17)), vec2i(27, 3), WAR_COLOR_GREEN);
    wui_createUIRect(context, wstr_fromCString("rectLifeBar3"), vec2Addv(leftBottomPanel, vec2i(4, 39)), vec2i(27, 3), WAR_COLOR_GREEN);
    wui_createUIRect(context, wstr_fromCString("rectLifeBar4"), vec2Addv(leftBottomPanel, vec2i(38, 39)), vec2i(27, 3), WAR_COLOR_GREEN);
    wui_createUIRect(context, wstr_fromCString("rectMagicBar"), vec2Addv(leftBottomPanel, vec2i(37, 9)), vec2i(27, 3), WAR_COLOR_GREEN);
    wui_createUIRect(context, wstr_fromCString("rectPercentBar"), vec2Addv(leftBottomPanel, vec2i(4, 37)), vec2i(62, 5), WAR_COLOR_GREEN);
    wui_createUIImage(context, wstr_fromCString("rectPercentText"), imageResourceRef(410), vec2Addv(leftBottomPanel, vec2i(15, 37)));

    // texts in the command area
    wui_createUIText(context, wstr_fromCString("txtCommand0"), 0, 6, wstr_make(), vec2Addv(leftBottomPanel, vec2i(3, 46)));
    wui_createUIText(context, wstr_fromCString("txtCommand1"), 0, 6, wstr_make(), vec2Addv(leftBottomPanel, vec2i(3, 56)));
    wui_createUIText(context, wstr_fromCString("txtCommand2"), 0, 6, wstr_make(), vec2Addv(leftBottomPanel, vec2i(7, 64)));
    wui_createUIText(context, wstr_fromCString("txtCommand3"), 0, 6, wstr_make(), vec2Addv(leftBottomPanel, vec2i(11, 54)));

    // command buttons
    wui_createUIImageButton(
        context, wstr_fromCString("btnCommand0"),
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(2, 44)));

    wui_createUIImageButton(
        context, wstr_fromCString("btnCommand1"),
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(36, 44)));

    wui_createUIImageButton(
        context, wstr_fromCString("btnCommand2"),
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(2, 67)));

    wui_createUIImageButton(
        context, wstr_fromCString("btnCommand3"),
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(36, 67)));

    wui_createUIImageButton(
        context, wstr_fromCString("btnCommand4"),
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(2, 90)));

    wui_createUIImageButton(
        context, wstr_fromCString("btnCommand5"),
        normalRef, pressedRef, portraitsRef,
        vec2Addv(leftBottomPanel, vec2i(36, 90)));

    uiEntity = wui_createUIImageButton(
        context, wstr_fromCString("btnMenu"),
        imageResourceRef(362),
        imageResourceRef(363),
        invalidRef,
        vec2Addv(leftBottomPanel, vec2i(3, 116)));
    wui_setUITooltip(uiEntity, 6, 3, wstr_fromCString("MENU (F10)"));
    setUIButtonClickHandler(uiEntity, wmm_handleMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_F10);
}

WarEntity* wmui_createUIMinimap(WarContext* context, String name, vec2 position)
{
    WarEntity* entity = went_createEntity(context, WAR_ENTITY_TYPE_MINIMAP, true);
    went_addTransformComponent(context, entity, position);
    went_addUIComponent(context, entity, name);

    return entity;
}

void wmui_updateGoldText(WarContext* context)
{
    WarMap* map = context->map;

    WarEntity* txtGold = went_findUIEntity(context, wsv_fromCString("txtGold"));
    assert(txtGold);

    s32 gold = map->players[0].gold;
    wui_setUIText(txtGold, wstr_fromCStringFormat("GOLD:%*d", 6, gold));
    setUITextHighlight(txtGold, NO_HIGHLIGHT, 0);
}

void wmui_updateWoodText(WarContext* context)
{
    WarMap* map = context->map;

    WarEntity* txtWood = went_findUIEntity(context, wsv_fromCString("txtWood"));
    assert(txtWood);

    s32 wood = map->players[0].wood;
    wui_setUIText(txtWood, wstr_fromCStringFormat("LUMBER:%*d", 6, wood));
    setUITextHighlight(txtWood, NO_HIGHLIGHT, 0);
}

void wmui_updateSelectedUnitsInfo(WarContext* context)
{
    WarMap* map = context->map;

    // retrieve entities of sprites of unit info/portraits
    WarEntity* imgUnitInfo = went_findUIEntity(context, wsv_fromCString("imgUnitInfo"));
    assert(imgUnitInfo);

    WarEntity* imgUnitInfoLife = went_findUIEntity(context, wsv_fromCString("imgUnitInfoLife"));
    assert(imgUnitInfoLife);

    String uiEntityName;
    WarEntity* imgUnitPortraits[5];
    WarEntity* rectLifeBars[5];
    for (s32 i = 0; i < 5; i++)
    {
        uiEntityName = wstr_fromCStringFormat("imgUnitPortrait%d", i);
        imgUnitPortraits[i] = went_findUIEntity(context, wstr_view(&uiEntityName));
        assert(imgUnitPortraits[i]);

        uiEntityName = wstr_fromCStringFormat("rectLifeBar%d", i);
        rectLifeBars[i] = went_findUIEntity(context, wstr_view(&uiEntityName));
        assert(rectLifeBars[i]);
    }

    WarEntity* rectMagicBar = went_findUIEntity(context, wsv_fromCString("rectMagicBar"));
    assert(rectMagicBar);

    WarEntity* rectPercentBar = went_findUIEntity(context, wsv_fromCString("rectPercentBar"));
    assert(rectPercentBar);

    WarEntity* rectPercentText = went_findUIEntity(context, wsv_fromCString("rectPercentText"));
    assert(rectPercentText);

    WarEntity* txtUnitName = went_findUIEntity(context, wsv_fromCString("txtUnitName"));
    assert(txtUnitName);

    // reset frame index of the sprites of unit info/portraits
    wui_setUIImage(imgUnitInfo, -1);
    wui_setUIImage(imgUnitInfoLife, -1);

    for (s32 i = 0; i < 5; i++)
    {
        wui_setUIImage(imgUnitPortraits[i], -1);
        wui_setUIRectWidth(rectLifeBars[i], 0);
    }

    wui_setUIRectWidth(rectMagicBar, 0);
    wui_setUIRectWidth(rectPercentBar, 0);
    wui_setUIImage(rectPercentText, -1);
    wui_setUIText(txtUnitName, wstr_make());
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
        wui_setUIImage(imgUnitInfo, selectedEntitiesCount + 1);
        wui_setUIImage(imgUnitInfoLife, selectedEntitiesCount + 4);

        for (s32 i = 1; i <= selectedEntitiesCount; i++)
        {
            WarEntityId selectedEntityId = map->selectedEntities.items[i - 1];
            WarEntity* selectedEntity = went_findEntity(context, selectedEntityId);
            if (selectedEntity && isUnit(selectedEntity))
            {
                WarUnitComponent* unit = &selectedEntity->unit;
                WarUnitData unitData = wun_getUnitData(unit->type);
                wui_setUIImage(imgUnitPortraits[i], unitData.portraitFrameIndex);
                wmui_setLifeBar(rectLifeBars[i], unit);
            }
        }
    }
    else if (selectedEntitiesCount == 1)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[0];
        WarEntity* selectedEntity = went_findEntity(context, selectedEntityId);
        if (selectedEntity && isUnit(selectedEntity))
        {
            WarUnitComponent* unit = &selectedEntity->unit;

            if (wun_isDudeUnit(selectedEntity))
            {
                if (wun_isMagicUnit(selectedEntity))
                {
                    wui_setUIImage(imgUnitInfo, 1);
                    wmui_setManaBar(rectMagicBar, unit);
                }
                else
                {
                    wui_setUIImage(imgUnitInfo, 0);
                }
            }
            else if (wun_isBuildingUnit(selectedEntity))
            {
                if (unit->building)
                {
                    wui_setUIImage(imgUnitInfo, 2);
                    wmui_setPercentBar(rectPercentBar, rectPercentText, unit);
                }
                else
                {
                    wui_setUIImage(imgUnitInfo, 0);
                }
            }

            WarUnitData unitData = wun_getUnitData(unit->type);
            wui_setUIImage(imgUnitPortraits[0], unitData.portraitFrameIndex);
            wui_setUIText(txtUnitName, wsv_toString(unitData.name));
            setUITextHighlight(txtUnitName, NO_HIGHLIGHT, 0);
            wmui_setLifeBar(rectLifeBars[0], unit);
        }
    }
}

void wmui_setStatus(WarContext* context, s32 highlightIndex, s32 highlightCount, s32 gold, s32 wood, String text)
{
    WarEntity* txtStatus = went_findUIEntity(context, wsv_fromCString("txtStatus"));
    assert(txtStatus);

    WarEntity* imgStatusWood = went_findUIEntity(context, wsv_fromCString("imgStatusWood"));
    assert(imgStatusWood);

    WarEntity* imgStatusGold = went_findUIEntity(context, wsv_fromCString("imgStatusGold"));
    assert(imgStatusGold);

    WarEntity* txtStatusWood = went_findUIEntity(context, wsv_fromCString("txtStatusWood"));
    assert(txtStatusWood);

    WarEntity* txtStatusGold = went_findUIEntity(context, wsv_fromCString("txtStatusGold"));
    assert(txtStatusGold);

    wui_setUIText(txtStatus, text);

    setUITextHighlight(txtStatus, highlightIndex, highlightCount);

    if (gold == 0 && wood == 0)
    {
        imgStatusWood->sprite.enabled = false;
        imgStatusGold->sprite.enabled = false;
        wui_clearUIText(txtStatusWood);
        wui_clearUIText(txtStatusGold);
    }
    else
    {
        imgStatusWood->sprite.enabled = true;
        imgStatusGold->sprite.enabled = true;
        wui_setUIText(txtStatusWood, wstr_fromCStringFormat("%d", wood));
        setUITextHighlight(txtStatusWood, NO_HIGHLIGHT, 0);
        wui_setUIText(txtStatusGold, wstr_fromCStringFormat("%d", gold));
        setUITextHighlight(txtStatusGold, NO_HIGHLIGHT, 0);
    }
}

void wmui_setFlashStatus(WarContext* context, f32 duration, String text)
{
    WarMap* map = context->map;
    WarFlashStatus* flashStatus = &map->flashStatus;

    assert(duration >= 0);
    assert(text.data);

    flashStatus->enabled = true;
    flashStatus->startTime = context->time;
    flashStatus->duration = duration;
    wstr_free(flashStatus->text);
    flashStatus->text = text;
}

void wmui_setLifeBar(WarEntity* rectLifeBar, WarUnitComponent* unit)
{
#define LIFE_BAR_RED_THRESHOLD 0.35f
#define LIFE_BAR_YELLOW_THRESHOLD 0.70f
#define LIFE_BAR_WIDTH_PX 27

    f32 hpPercent = percentabf01(unit->hp, unit->maxhp);

    if (hpPercent <= LIFE_BAR_RED_THRESHOLD)
        rectLifeBar->rect.color = WAR_COLOR_RED;
    else if (hpPercent <= LIFE_BAR_YELLOW_THRESHOLD)
        rectLifeBar->rect.color = WAR_COLOR_YELLOW;
    else
        rectLifeBar->rect.color = WAR_COLOR_GREEN;

    wui_setUIRectWidth(rectLifeBar, (s32)(hpPercent * LIFE_BAR_WIDTH_PX));
}

void wmui_setManaBar(WarEntity* rectMagicBar, WarUnitComponent* unit)
{
#define MAGIC_BAR_WIDTH_PX 27

    f32 magicPercent = percentabf01(unit->mana, unit->maxMana);
    wui_setUIRectWidth(rectMagicBar, (s32)(magicPercent * MAGIC_BAR_WIDTH_PX));
}

void wmui_setPercentBar(WarEntity* rectPercentBar, WarEntity* rectPercentText, WarUnitComponent* unit)
{
#define PERCENT_BAR_WIDTH_PX 64

    f32 percent = unit->buildPercent;

    wui_setUIRectWidth(rectPercentBar, (s32)(percent * PERCENT_BAR_WIDTH_PX));
    wui_setUIImage(rectPercentText, 0);
}

void wmui_renderSelectionRect(WarContext* context)
{
    wrend_renderSave(context);

    WarInput* input = &context->input;
    if (input->isDragging)
    {
        rect pointerRect = rectpf(input->dragPos.x, input->dragPos.y, input->pos.x, input->pos.y);
        wrend_renderStrokeRect(context, pointerRect, WAR_COLOR_GREEN_SELECTION, 1.0f);
    }

    wrend_renderRestore(context);
}

void wmui_renderCommand(WarContext* context)
{
    WarMap* map = context->map;
    WarUnitCommand* command = &map->command;

    WarInput* input = &context->input;

    wrend_renderSave(context);

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
            vec2 position = wmap_vec2ScreenToMapCoordinates(context, input->pos);
            position = wmap_vec2MapToTileCoordinates(position);

            WarUnitType buildingToBuild = command->build.buildingToBuild;
            WarUnitData data = wun_getUnitData(buildingToBuild);

            WarColor fillColor = went_checkRectToBuild(context, (s32)position.x, (s32)position.y, data.sizex, data.sizey)
                ? WAR_COLOR_GRAY_TRANSPARENT : WAR_COLOR_RED_TRANSPARENT;

            position = wmap_vec2TileToMapCoordinates(position, false);
            position = wmap_vec2MapToScreenCoordinates(context, position);
            vec2 size = vec2i(data.sizex * MEGA_TILE_WIDTH, data.sizey * MEGA_TILE_HEIGHT);
            rect buildingRect = rectv(position, size);
            wrend_renderFillRect(context, buildingRect, fillColor);

            break;
        }

        case WAR_COMMAND_BUILD_WALL:
        case WAR_COMMAND_BUILD_ROAD:
        {
            vec2 position = wmap_vec2ScreenToMapCoordinates(context, input->pos);
            position = wmap_vec2MapToTileCoordinates(position);

            WarColor fillColor = went_checkRectToBuild(context, (s32)position.x, (s32)position.y, 1, 1)
                ? WAR_COLOR_GRAY_TRANSPARENT : WAR_COLOR_RED_TRANSPARENT;

            position = wmap_vec2TileToMapCoordinates(position, false);
            position = wmap_vec2MapToScreenCoordinates(context, position);
            vec2 size = vec2i(MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect buildingRect = rectv(position, size);
            wrend_renderFillRect(context, buildingRect, fillColor);

            break;
        }

        default:
        {
            // don't render the rest of the commands
            break;
        }
    }

    wrend_renderRestore(context);
}

void wmui_renderMapUI(WarContext* context)
{
    wrend_renderSave(context);

    wmui_renderSelectionRect(context);
    wmui_renderCommand(context);
    wui_renderUIEntities(context);

    wrend_renderRestore(context);
}
