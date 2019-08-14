void createMapUI(WarContext* context)
{
    WarMap* map = context->map;

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

    // panels
    createUIImage(context, "panelLeftTop", imageResourceRef(224), leftTopPanel);
    createUIImage(context, "panelLeftBottom", imageResourceRef(226), leftBottomPanel);
    createUIImage(context, "panelTop", imageResourceRef(218), topPanel);
    createUIImage(context, "panelRight", imageResourceRef(220), rightPanel);
    createUIImage(context, "panelBottom", imageResourceRef(222), bottomPanel);

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

    // selected unit(s) info
    createUIImage(context, "imgUnitInfo", imageResourceRef(360), vec2Addv(leftBottomPanel, vec2i(2, 0)));
    createUIImage(context, "imgUnitPortrait0", portraitsRef, vec2Addv(leftBottomPanel, vec2i(6, 4)));
    createUIImage(context, "imgUnitPortrait1", portraitsRef, vec2Addv(leftBottomPanel, vec2i(4, 1)));
    createUIImage(context, "imgUnitPortrait2", portraitsRef, vec2Addv(leftBottomPanel, vec2i(38, 1)));
    createUIImage(context, "imgUnitPortrait3", portraitsRef, vec2Addv(leftBottomPanel, vec2i(4, 23)));
    createUIImage(context, "imgUnitPortrait4", portraitsRef, vec2Addv(leftBottomPanel, vec2i(38, 23)));
    createUIImage(context, "imgUnitInfoLife", imageResourceRef(360), vec2Addv(leftBottomPanel, vec2i(3, 16)));
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

    WarEntity* uiEntity;

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