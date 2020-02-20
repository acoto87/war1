void enterSceneMainMenu(WarContext* context)
{
    createMainMenu(context);
    createSinglePlayerMenu(context);
    createLoadMenu(context);
    createCheatsPanel(context);

    createUICursor(context, "cursor", WAR_CURSOR_ARROW, VEC2_ZERO);

    if (!isDemo(context))
        createAudio(context, WAR_MUSIC_00, true);
}

void createMainMenu(WarContext* context)
{
    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);

    WarEntity* uiEntity;

    uiEntity = createUIImage(context, "imgMenuBackground", imageResourceRef(261), VEC2_ZERO);

    uiEntity = createUITextButton(
        context, "btnMenuSinglePlayer",
        1, 10, "Start a new game",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 85));
    setUIButtonClickHandler(uiEntity, handleMenuSinglePlayer);
    setUIButtonHotKey(uiEntity, WAR_KEY_S);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnMenuLoad",
        1, 10, "Load existing game",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 105));
    setUIButtonHotKey(uiEntity, WAR_KEY_L);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnMenuReplayIntro",
        1, 10, "Replay introduction",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 125));
    setUIButtonHotKey(uiEntity, WAR_KEY_R);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnMenuQuit",
        1, 10, "Quit",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 165));
    setUIButtonClickHandler(uiEntity, handleMenuQuit);
    setUIButtonHotKey(uiEntity, WAR_KEY_Q);
    setUITextHighlight(uiEntity, 0, 1);
}

void createSinglePlayerMenu(WarContext* context)
{
    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUITextButton(
        context, "btnSinglePlayerOrc",
        1, 10, "Orc campaign",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 85));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSinglePlayerOrc);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnSinglePlayerHuman",
        1, 10, "Human campaign",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 105));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSinglePlayerHuman);
    setUIButtonHotKey(uiEntity, WAR_KEY_H);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnSinglePlayerCancel",
        1, 10, "Cancel",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2i(133, 165));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSinglePlayerCancel);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void createLoadMenu(WarContext* context)
{
}

void showOrHideMainMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, "btnMenuSinglePlayer", status);
    setUIEntityStatusByName(context, "btnMenuLoad", status);
    setUIEntityStatusByName(context, "btnMenuReplayIntro", status);
    setUIEntityStatusByName(context, "btnMenuQuit", status);
}

void showOrHideSinglePlayer(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, "btnSinglePlayerOrc", status);
    setUIEntityStatusByName(context, "btnSinglePlayerHuman", status);
    setUIEntityStatusByName(context, "btnSinglePlayerCancel", status);
}

// menu button handlers
void handleMenuSinglePlayer(WarContext* context, WarEntity* entity)
{
    showOrHideMainMenu(context, false);
    showOrHideSinglePlayer(context, true);
}

void handleMenuQuit(WarContext* context, WarEntity* entity)
{
    glfwSetWindowShouldClose(context->window, true);
}

void handleSinglePlayerOrc(WarContext* context, WarEntity* entity)
{
    WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
    scene->briefing.race = WAR_RACE_ORCS;
    scene->briefing.mapType = WAR_CAMPAIGN_ORCS_01;
    setNextScene(context, scene, 1.0f);
}

void handleSinglePlayerHuman(WarContext* context, WarEntity* entity)
{
    WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
    scene->briefing.race = WAR_RACE_HUMANS;
    scene->briefing.mapType = WAR_CAMPAIGN_HUMANS_01;
    setNextScene(context, scene, 1.0f);
}

void handleSinglePlayerCancel(WarContext* context, WarEntity* entity)
{
    showOrHideSinglePlayer(context, false);
    showOrHideMainMenu(context, true);
}