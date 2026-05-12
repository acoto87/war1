#include "war_map_ui.h"

#include <assert.h>

#include "shl/wstr.h"

#include "war_entities.h"
#include "war_map.h"
#include "war_ui.h"
#include "war_units.h"

void wmui_createMapUI(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    vec2 leftTopPanel = RECT_TOP_LEFT(map->leftTopPanel);
    vec2 leftBottomPanel = RECT_TOP_LEFT(map->leftBottomPanel);
    vec2 topPanel = RECT_TOP_LEFT(map->topPanel);
    vec2 rightPanel = RECT_TOP_LEFT(map->rightPanel);
    vec2 bottomPanel = RECT_TOP_LEFT(map->bottomPanel);
    vec2 minimapPanel = RECT_TOP_LEFT(map->minimapPanel);

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
    wui_createUIImage(context, wstr_fromCString("panelLeftTop"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRefFromPlayer(player, 224, 225),
        .position  = leftTopPanel,
    });
    wui_createUIImage(context, wstr_fromCString("panelLeftBottom"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRefFromPlayer(player, 226, 227),
        .position  = leftBottomPanel,
    });
    wui_createUIImage(context, wstr_fromCString("panelTop"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRefFromPlayer(player, 218, 219),
        .position  = topPanel,
    });
    wui_createUIImage(context, wstr_fromCString("panelRight"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRefFromPlayer(player, 220, 221),
        .position  = rightPanel,
    });
    wui_createUIImage(context, wstr_fromCString("panelBottom"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRefFromPlayer(player, 222, 223),
        .position  = bottomPanel,
    });

    // minimap
    wmui_createUIMinimap(context, wstr_fromCString("minimap"), minimapPanel);

    // top panel images
    wui_createUIImage(context, wstr_fromCString("imgGold"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRef(406),
        .position  = vec2_addv(topPanel, vec2i(201, 1)),
    });
    wui_createUIImage(context, wstr_fromCString("imgLumber"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRef(407),
        .position  = vec2_addv(topPanel, vec2i(102, 0)),
    });

    // top panel texts
    uiEntity = wui_createUIText(context, wstr_fromCString("txtGold"), &(CreateUITextArgs){
        .position = vec2_addv(topPanel, vec2i(135, 2)),
        .fontSize = 6,
    });
    uiEntity = wui_createUIText(context, wstr_fromCString("txtWood"), &(CreateUITextArgs){
        .position = vec2_addv(topPanel, vec2i(24, 2)),
        .fontSize = 6,
    });

    // status text
    uiEntity = wui_createUIText(context, wstr_fromCString("txtStatus"), &(CreateUITextArgs){
        .position = vec2_addv(bottomPanel, vec2i(2, 5)),
        .fontSize = 6,
    });
    wui_createUIImage(context, wstr_fromCString("imgStatusWood"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRef(407),
        .position  = vec2_addv(bottomPanel, vec2i(163, 3)),
    });
    wui_createUIImage(context, wstr_fromCString("imgStatusGold"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRef(406),
        .position  = vec2_addv(bottomPanel, vec2i(200, 5)),
    });
    uiEntity = wui_createUIText(context, wstr_fromCString("txtStatusWood"), &(CreateUITextArgs){
        .position = vec2_addv(bottomPanel, vec2i(179, 5)),
        .fontSize = 6,
    });
    uiEntity = wui_createUIText(context, wstr_fromCString("txtStatusGold"), &(CreateUITextArgs){
        .position = vec2_addv(bottomPanel, vec2i(218, 5)),
        .fontSize = 6,
    });
    wui_createUIRect(context, wstr_fromCString("txtStatusCursor"), &(CreateUIRectArgs){
        .position = vec2_addv(bottomPanel, vec2i(2, 4)),
        .size     = vec2i(1, 7),
        .color    = WAR_COLOR_WHITE,
    });

    uiEntity = wui_createUIText(context, wstr_fromCString("txtCheatFeedbackText"), &(CreateUITextArgs){
        .position  = vec2_addv(bottomPanel, vec2i(15, -20)),
        .fontIndex = 1,
        .fontSize  = 8,
        .fontColor = WAR_COLOR_YELLOW,
    });
    setUIEntityStatus(context, uiEntity, false);

    // selected unit(s) info
    wui_createUIImage(context, wstr_fromCString("imgUnitInfo"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRefFromPlayer(player, 360, 359),
        .position  = vec2_addv(leftBottomPanel, vec2i(2, 0)),
    });
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait0"), &(CreateUIImageArgs){
        .spriteRef = portraitsRef,
        .position  = vec2_addv(leftBottomPanel, vec2i(6, 4)),
    });
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait1"), &(CreateUIImageArgs){
        .spriteRef = portraitsRef,
        .position  = vec2_addv(leftBottomPanel, vec2i(4, 1)),
    });
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait2"), &(CreateUIImageArgs){
        .spriteRef = portraitsRef,
        .position  = vec2_addv(leftBottomPanel, vec2i(38, 1)),
    });
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait3"), &(CreateUIImageArgs){
        .spriteRef = portraitsRef,
        .position  = vec2_addv(leftBottomPanel, vec2i(4, 23)),
    });
    wui_createUIImage(context, wstr_fromCString("imgUnitPortrait4"), &(CreateUIImageArgs){
        .spriteRef = portraitsRef,
        .position  = vec2_addv(leftBottomPanel, vec2i(38, 23)),
    });
    wui_createUIImage(context, wstr_fromCString("imgUnitInfoLife"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRefFromPlayer(player, 360, 359),
        .position  = vec2_addv(leftBottomPanel, vec2i(3, 16)),
    });
    uiEntity = wui_createUIText(context, wstr_fromCString("txtUnitName"), &(CreateUITextArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(6, 26)),
        .fontSize = 6,
    });
    wui_createUIRect(context, wstr_fromCString("rectLifeBar0"), &(CreateUIRectArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(37, 20)),
        .size     = vec2i(27, 3),
        .color    = WAR_COLOR_GREEN,
    });
    wui_createUIRect(context, wstr_fromCString("rectLifeBar1"), &(CreateUIRectArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(4, 17)),
        .size     = vec2i(27, 3),
        .color    = WAR_COLOR_GREEN,
    });
    wui_createUIRect(context, wstr_fromCString("rectLifeBar2"), &(CreateUIRectArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(38, 17)),
        .size     = vec2i(27, 3),
        .color    = WAR_COLOR_GREEN,
    });
    wui_createUIRect(context, wstr_fromCString("rectLifeBar3"), &(CreateUIRectArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(4, 39)),
        .size     = vec2i(27, 3),
        .color    = WAR_COLOR_GREEN,
    });
    wui_createUIRect(context, wstr_fromCString("rectLifeBar4"), &(CreateUIRectArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(38, 39)),
        .size     = vec2i(27, 3),
        .color    = WAR_COLOR_GREEN,
    });
    wui_createUIRect(context, wstr_fromCString("rectMagicBar"), &(CreateUIRectArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(37, 9)),
        .size     = vec2i(27, 3),
        .color    = WAR_COLOR_GREEN,
    });
    wui_createUIRect(context, wstr_fromCString("rectPercentBar"), &(CreateUIRectArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(4, 37)),
        .size     = vec2i(62, 5),
        .color    = WAR_COLOR_GREEN,
    });
    wui_createUIImage(context, wstr_fromCString("rectPercentText"), &(CreateUIImageArgs){
        .spriteRef = imageResourceRef(410),
        .position  = vec2_addv(leftBottomPanel, vec2i(15, 37)),
    });

    // texts in the command area
    uiEntity = wui_createUIText(context, wstr_fromCString("txtCommand0"), &(CreateUITextArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(3, 46)),
        .fontSize = 6,
    });
    uiEntity = wui_createUIText(context, wstr_fromCString("txtCommand1"), &(CreateUITextArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(3, 56)),
        .fontSize = 6,
    });
    uiEntity = wui_createUIText(context, wstr_fromCString("txtCommand2"), &(CreateUITextArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(7, 64)),
        .fontSize = 6,
    });
    uiEntity = wui_createUIText(context, wstr_fromCString("txtCommand3"), &(CreateUITextArgs){
        .position = vec2_addv(leftBottomPanel, vec2i(11, 54)),
        .fontSize = 6,
    });

    // command buttons
    wui_createUIImageButton(context, wstr_fromCString("btnCommand0"), &(CreateUIImageButtonArgs){
        .backgroundNormalRef  = normalRef,
        .backgroundPressedRef = pressedRef,
        .foregroundRef        = portraitsRef,
        .position             = vec2_addv(leftBottomPanel, vec2i(2, 44)),
    });

    wui_createUIImageButton(context, wstr_fromCString("btnCommand1"), &(CreateUIImageButtonArgs){
        .backgroundNormalRef  = normalRef,
        .backgroundPressedRef = pressedRef,
        .foregroundRef        = portraitsRef,
        .position             = vec2_addv(leftBottomPanel, vec2i(36, 44)),
    });

    wui_createUIImageButton(context, wstr_fromCString("btnCommand2"), &(CreateUIImageButtonArgs){
        .backgroundNormalRef  = normalRef,
        .backgroundPressedRef = pressedRef,
        .foregroundRef        = portraitsRef,
        .position             = vec2_addv(leftBottomPanel, vec2i(2, 67)),
    });

    wui_createUIImageButton(context, wstr_fromCString("btnCommand3"), &(CreateUIImageButtonArgs){
        .backgroundNormalRef  = normalRef,
        .backgroundPressedRef = pressedRef,
        .foregroundRef        = portraitsRef,
        .position             = vec2_addv(leftBottomPanel, vec2i(36, 67)),
    });

    wui_createUIImageButton(context, wstr_fromCString("btnCommand4"), &(CreateUIImageButtonArgs){
        .backgroundNormalRef  = normalRef,
        .backgroundPressedRef = pressedRef,
        .foregroundRef        = portraitsRef,
        .position             = vec2_addv(leftBottomPanel, vec2i(2, 90)),
    });

    wui_createUIImageButton(context, wstr_fromCString("btnCommand5"), &(CreateUIImageButtonArgs){
        .backgroundNormalRef  = normalRef,
        .backgroundPressedRef = pressedRef,
        .foregroundRef        = portraitsRef,
        .position             = vec2_addv(leftBottomPanel, vec2i(36, 90)),
    });

    uiEntity = wui_createUIImageButton(context, wstr_fromCString("btnMenu"), &(CreateUIImageButtonArgs){
        .backgroundNormalRef  = imageResourceRef(362),
        .backgroundPressedRef = imageResourceRef(363),
        .foregroundRef        = invalidRef,
        .position             = vec2_addv(leftBottomPanel, vec2i(3, 116)),
    });
    wui_setUITooltip(context, uiEntity, 6, 3, wstr_fromCString("MENU (F10)"));
    setUIButtonClickHandler(context, uiEntity, wmm_handleMenu);
    setUIButtonHotKey(context, uiEntity, WAR_KEY_F10);
}

WarEntity* wmui_createUIMinimap(WarContext* context, String name, vec2 position)
{
    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_MINIMAP, true);
    we_addTransformComponent(context, entity, position);
    we_addUIComponent(context, entity, name);

    return entity;
}

void wmui_updateGoldText(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateGoldText", 1);

    WarMap* map = context->map;

    WarEntity* txtGold = we_findUIEntity(context, wsv_fromCString("txtGold"));
    assert(txtGold);

    s32 gold = map->players[0].gold;
    wui_setUIText(context, txtGold, wstr_fromCStringFormat("GOLD:%*d", 6, gold));
    setUITextHighlight(context, txtGold, NO_HIGHLIGHT, 0);

    TracyCZoneEnd(ctx);
}

void wmui_updateWoodText(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateWoodText", 1);

    WarMap* map = context->map;

    WarEntity* txtWood = we_findUIEntity(context, wsv_fromCString("txtWood"));
    assert(txtWood);

    s32 wood = map->players[0].wood;
    wui_setUIText(context, txtWood, wstr_fromCStringFormat("LUMBER:%*d", 6, wood));
    setUITextHighlight(context, txtWood, NO_HIGHLIGHT, 0);

    TracyCZoneEnd(ctx);
}

void wmui_updateSelectedUnitsInfo(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateSelectedUnitsInfo", 1);

    WarMap* map = context->map;

    // retrieve entities of sprites of unit info/portraits
    WarEntity* imgUnitInfo = we_findUIEntity(context, wsv_fromCString("imgUnitInfo"));
    assert(imgUnitInfo);

    WarEntity* imgUnitInfoLife = we_findUIEntity(context, wsv_fromCString("imgUnitInfoLife"));
    assert(imgUnitInfoLife);

    String uiEntityName;
    WarEntity* imgUnitPortraits[5];
    WarEntity* rectLifeBars[5];
    for (s32 i = 0; i < 5; i++)
    {
        uiEntityName = wstr_fromCStringFormat("imgUnitPortrait%d", i);
        imgUnitPortraits[i] = we_findUIEntity(context, wstr_view(&uiEntityName));
        assert(imgUnitPortraits[i]);

        uiEntityName = wstr_fromCStringFormat("rectLifeBar%d", i);
        rectLifeBars[i] = we_findUIEntity(context, wstr_view(&uiEntityName));
        assert(rectLifeBars[i]);
    }

    WarEntity* rectMagicBar = we_findUIEntity(context, wsv_fromCString("rectMagicBar"));
    assert(rectMagicBar);

    WarEntity* rectPercentBar = we_findUIEntity(context, wsv_fromCString("rectPercentBar"));
    assert(rectPercentBar);

    WarEntity* rectPercentText = we_findUIEntity(context, wsv_fromCString("rectPercentText"));
    assert(rectPercentText);

    WarEntity* txtUnitName = we_findUIEntity(context, wsv_fromCString("txtUnitName"));
    assert(txtUnitName);

    // reset frame index of the sprites of unit info/portraits
    wui_setUIImage(context, imgUnitInfo, -1);
    wui_setUIImage(context, imgUnitInfoLife, -1);

    for (s32 i = 0; i < 5; i++)
    {
        wui_setUIImage(context, imgUnitPortraits[i], -1);
        wui_setUIRectWidth(context, rectLifeBars[i], 0);
    }

    wui_setUIRectWidth(context, rectMagicBar, 0);
    wui_setUIRectWidth(context, rectPercentBar, 0);
    wui_setUIImage(context, rectPercentText, -1);
    wui_setUIText(context, txtUnitName, wstr_make());
    setUITextHighlight(context, txtUnitName, NO_HIGHLIGHT, 0);

    // update the frame index of unit info/portraits
    // based on the number of entities selected
    //
    // TODO: the max number of selected entities shouldn't greater than 4 but
    // that's not implemented right now, so put a min call to guard for that.
    s32 selectedEntitiesCount = MIN(map->selectedEntities.count, 4);
    if (selectedEntitiesCount > 1)
    {
        // for 4 units selected -> frame indices 5, 8
        // for 3 units selected -> frame indices 4, 7
        // for 2 units selected -> frame indices 3, 6
        wui_setUIImage(context, imgUnitInfo, selectedEntitiesCount + 1);
        wui_setUIImage(context, imgUnitInfoLife, selectedEntitiesCount + 4);

        for (s32 i = 1; i <= selectedEntitiesCount; i++)
        {
            WarEntityId selectedEntityId = map->selectedEntities.items[i - 1];
            WarEntity* selectedEntity = we_findEntity(context, selectedEntityId);
            if (selectedEntity && wu_isUnit(selectedEntity))
            {
                WarUnitComponent* unit = we_getUnitComponent(context, selectedEntity);
                assert(unit);

                WarUnitData unitData = wu_getUnitData(unit->type);
                wui_setUIImage(context, imgUnitPortraits[i], unitData.portraitFrameIndex);
                wmui_setLifeBar(context, rectLifeBars[i], unit);
            }
        }
    }
    else if (selectedEntitiesCount == 1)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[0];
        WarEntity* selectedEntity = we_findEntity(context, selectedEntityId);
        if (selectedEntity && wu_isUnit(selectedEntity))
        {
            WarUnitComponent* unit = we_getUnitComponent(context, selectedEntity);
            assert(unit);

            if (wu_isDudeUnit(context, selectedEntity))
            {
                if (wu_isMagicUnit(context, selectedEntity))
                {
                    wui_setUIImage(context, imgUnitInfo, 1);
                    wmui_setManaBar(context, rectMagicBar, unit);
                }
                else
                {
                    wui_setUIImage(context, imgUnitInfo, 0);
                }
            }
            else if (wu_isBuildingUnit(context, selectedEntity))
            {
                if (unit->building)
                {
                    wui_setUIImage(context, imgUnitInfo, 2);
                    wmui_setPercentBar(context, rectPercentBar, rectPercentText, unit);
                }
                else
                {
                    wui_setUIImage(context, imgUnitInfo, 0);
                }
            }

            WarUnitData unitData = wu_getUnitData(unit->type);
            wui_setUIImage(context, imgUnitPortraits[0], unitData.portraitFrameIndex);
            wui_setUIText(context, txtUnitName, wsv_toString(unitData.name));
            setUITextHighlight(context, txtUnitName, NO_HIGHLIGHT, 0);
            wmui_setLifeBar(context, rectLifeBars[0], unit);
        }
    }

    TracyCZoneEnd(ctx);
}

void wmui_setStatus(WarContext* context, s32 highlightIndex, s32 highlightCount, s32 gold, s32 wood, String text)
{
    WarEntity* txtStatus = we_findUIEntity(context, wsv_fromCString("txtStatus"));
    assert(txtStatus);

    WarEntity* imgStatusWood = we_findUIEntity(context, wsv_fromCString("imgStatusWood"));
    assert(imgStatusWood);

    WarEntity* imgStatusGold = we_findUIEntity(context, wsv_fromCString("imgStatusGold"));
    assert(imgStatusGold);

    WarEntity* txtStatusWood = we_findUIEntity(context, wsv_fromCString("txtStatusWood"));
    assert(txtStatusWood);

    WarEntity* txtStatusGold = we_findUIEntity(context, wsv_fromCString("txtStatusGold"));
    assert(txtStatusGold);

    wui_setUIText(context, txtStatus, text);

    setUITextHighlight(context, txtStatus, highlightIndex, highlightCount);

    if (gold == 0 && wood == 0)
    {
        WarSpriteComponent* spriteWood = we_getSpriteComponent(context, imgStatusWood);
        assert(spriteWood);

        WarSpriteComponent* spriteGold = we_getSpriteComponent(context, imgStatusGold);
        assert(spriteGold);

        we_disableComponent(context, imgStatusWood, COMP_SPRITE);
        we_disableComponent(context, imgStatusGold, COMP_SPRITE);
        wui_clearUIText(context, txtStatusWood);
        wui_clearUIText(context, txtStatusGold);
    }
    else
    {
        WarSpriteComponent* spriteWood = we_getSpriteComponent(context, imgStatusWood);
        assert(spriteWood);

        WarSpriteComponent* spriteGold = we_getSpriteComponent(context, imgStatusGold);
        assert(spriteGold);

        we_enableComponent(context, imgStatusWood, COMP_SPRITE);
        we_enableComponent(context, imgStatusGold, COMP_SPRITE);
        wui_setUIText(context, txtStatusWood, wstr_fromCStringFormat("%d", wood));
        setUITextHighlight(context, txtStatusWood, NO_HIGHLIGHT, 0);
        wui_setUIText(context, txtStatusGold, wstr_fromCStringFormat("%d", gold));
        setUITextHighlight(context, txtStatusGold, NO_HIGHLIGHT, 0);
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

void wmui_setLifeBar(WarContext* context, WarEntity* rectLifeBar, WarUnitComponent* unit)
{
#define LIFE_BAR_RED_THRESHOLD 0.35f
#define LIFE_BAR_YELLOW_THRESHOLD 0.70f
#define LIFE_BAR_WIDTH_PX 27

    f32 hpPercent = PERCENTF01(unit->hp, unit->maxhp);

    WarRectComponent* lifeBarRect = we_getRectComponent(context, rectLifeBar);
    assert(lifeBarRect);

    if (hpPercent <= LIFE_BAR_RED_THRESHOLD)
        lifeBarRect->color = WAR_COLOR_RED;
    else if (hpPercent <= LIFE_BAR_YELLOW_THRESHOLD)
        lifeBarRect->color = WAR_COLOR_YELLOW;
    else
        lifeBarRect->color = WAR_COLOR_GREEN;

    wui_setUIRectWidth(context, rectLifeBar, (s32)(hpPercent * LIFE_BAR_WIDTH_PX));
}

void wmui_setManaBar(WarContext* context, WarEntity* rectMagicBar, WarUnitComponent* unit)
{
#define MAGIC_BAR_WIDTH_PX 27

    f32 magicPercent = PERCENTF01(unit->mana, unit->maxMana);
    wui_setUIRectWidth(context, rectMagicBar, (s32)(magicPercent * MAGIC_BAR_WIDTH_PX));
}

void wmui_setPercentBar(WarContext* context, WarEntity* rectPercentBar, WarEntity* rectPercentText, WarUnitComponent* unit)
{
#define PERCENT_BAR_WIDTH_PX 64

    f32 percent = unit->buildPercent;

    wui_setUIRectWidth(context, rectPercentBar, (s32)(percent * PERCENT_BAR_WIDTH_PX));
    wui_setUIImage(context, rectPercentText, 0);
}

void wmui_renderSelectionRect(WarContext* context)
{
    TracyCZoneN(ctx, "RenderSelectionRect", 1);

    wr_save(context);

    WarInput* input = &context->input;
    if (isMapDragging(input))
    {
        rect pointerRect = rectpf(input->mapDragStartPos.x, input->mapDragStartPos.y, input->pos.x, input->pos.y);
        wr_strokeRect(context, pointerRect, WAR_COLOR_GREEN_SELECTION, 1.0f);
    }

    wr_restore(context);

    TracyCZoneEnd(ctx);
}

void wmui_renderCommand(WarContext* context)
{
    TracyCZoneN(ctx, "RenderCommand", 1);

    WarMap* map = context->map;
    WarUnitCommand* command = &map->command;

    WarInput* input = &context->input;

    wr_save(context);

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
            vec2 position = wmap_screenToMapCoordinatesV(context, input->pos);
            position = wmap_mapToTileCoordinatesV(position);

            WarUnitType buildingToBuild = command->build.buildingToBuild;
            WarUnitData data = wu_getUnitData(buildingToBuild);

            WarColor fillColor = we_checkRectToBuild(context, (s32)position.x, (s32)position.y, data.sizex, data.sizey)
                ? WAR_COLOR_GRAY_TRANSPARENT : WAR_COLOR_RED_TRANSPARENT;

            position = wmap_tileToMapCoordinatesV(position, false);
            position = wmap_mapToScreenCoordinatesV(context, position);
            vec2 size = vec2i(data.sizex * MEGA_TILE_WIDTH, data.sizey * MEGA_TILE_HEIGHT);
            rect buildingRect = rectv(position, size);
            wr_fillRect(context, buildingRect, fillColor);

            break;
        }

        case WAR_COMMAND_BUILD_WALL:
        case WAR_COMMAND_BUILD_ROAD:
        {
            vec2 position = wmap_screenToMapCoordinatesV(context, input->pos);
            position = wmap_mapToTileCoordinatesV(position);

            WarColor fillColor = we_checkRectToBuild(context, (s32)position.x, (s32)position.y, 1, 1)
                ? WAR_COLOR_GRAY_TRANSPARENT : WAR_COLOR_RED_TRANSPARENT;

            position = wmap_tileToMapCoordinatesV(position, false);
            position = wmap_mapToScreenCoordinatesV(context, position);
            vec2 size = vec2i(MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect buildingRect = rectv(position, size);
            wr_fillRect(context, buildingRect, fillColor);

            break;
        }

        default:
        {
            // don't render the rest of the commands
            break;
        }
    }

    wr_restore(context);

    TracyCZoneEnd(ctx);
}

void wmui_renderMapUI(WarContext* context)
{
    TracyCZoneN(ctx, "RenderMapUI", 1);

    wr_save(context);

    wmui_renderSelectionRect(context);
    wmui_renderCommand(context);
    wui_renderUIEntities(context);

    wr_restore(context);

    TracyCZoneEnd(ctx);
}
