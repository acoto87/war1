static void setCustomGameRaceStr(WarRace value, char buffer[])
{
    switch (value)
    {
        case WAR_RACE_NEUTRAL: strcpy(buffer, "Random"); break;
        case WAR_RACE_HUMANS:  strcpy(buffer, "Human");  break;
        case WAR_RACE_ORCS:    strcpy(buffer, "Orc");    break;
    }
}

static void setUIRaceValueByName(WarContext* context, const char* name, WarRace value)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        char raceStr[10];
        setCustomGameRaceStr(value, raceStr);
        setUIText(entity, raceStr);
        setUITextHighlight(entity, NO_HIGHLIGHT, 0);
    }
}

void enterSceneMainMenu(WarContext* context)
{
    createMainMenu(context);
    createSinglePlayerMenu(context);
    createLoadMenu(context);
    createCustomGameMenu(context);
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
        context, "btnCustomGame",
        1, 10, "Custom game",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 125));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleCustomGame);
    setUIButtonHotKey(uiEntity, WAR_KEY_U);
    setUITextHighlight(uiEntity, 1, 1);

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

void createCustomGameMenu(WarContext* context)
{
    WarEntity* uiEntity;

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);
    WarSpriteResourceRef leftArrowNormalRef = imageResourceRef(244);
    WarSpriteResourceRef leftArrowPressedRef = imageResourceRef(245);
    WarSpriteResourceRef rightArrowNormalRef = imageResourceRef(246);
    WarSpriteResourceRef rightArrowPressedRef = imageResourceRef(247);

    uiEntity = createUIText(
        context, "txtYourRaceLabel",
        1, 10, "Your race:",
        vec2i(40, 105));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(100, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
        context, "txtEnemyRaceLabel",
        1, 10, "Enemy race:",
        vec2i(40, 125));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(100, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
        context, "txtYourRace",
        1, 10, "Human",
        vec2i(180, 105));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(50, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
        context, "txtEnemyRace",
        1, 10, "Orc",
        vec2i(180, 125));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(50, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIImageButton(
        context, "btnYourRaceLeft",
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2i(160, 103));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleYourRaceLeft);

    uiEntity = createUIImageButton(
        context, "btnYourRaceRight",
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2i(235, 103));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleYourRaceRight);

    uiEntity = createUIImageButton(
        context, "btnEnemyRaceLeft",
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2i(160, 123));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleEnemyRaceLeft);

    uiEntity = createUIImageButton(
        context, "btnEnemyRaceRight",
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2i(235, 123));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleEnemyRaceRight);

    uiEntity = createUITextButton(
        context, "btnCustomGameOk",
        1, 10, "Ok",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2i(100, 165));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleCustomGameOk);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnCustomGameCancel",
        1, 10, "Cancel",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2i(180, 165));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMenuSinglePlayer);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
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
    setUIEntityStatusByName(context, "btnCustomGame", status);
    setUIEntityStatusByName(context, "btnSinglePlayerCancel", status);
}

void showOrHideCustomGame(WarContext* context, bool status)
{
    WarScene* scene = context->scene;

    setUIEntityStatusByName(context, "txtYourRaceLabel", status);
    setUIEntityStatusByName(context, "txtEnemyRaceLabel", status);
    setUIEntityStatusByName(context, "txtYourRace", status);
    setUIEntityStatusByName(context, "txtEnemyRace", status);
    setUIEntityStatusByName(context, "btnYourRaceLeft", status);
    setUIEntityStatusByName(context, "btnYourRaceRight", status);
    setUIEntityStatusByName(context, "btnEnemyRaceLeft", status);
    setUIEntityStatusByName(context, "btnEnemyRaceRight", status);
    setUIEntityStatusByName(context, "btnCustomGameOk", status);
    setUIEntityStatusByName(context, "btnCustomGameCancel", status);

    if (status)
    {
        scene->menu.yourRace = WAR_RACE_HUMANS;
        scene->menu.enemyRace = WAR_RACE_ORCS;

        setUIRaceValueByName(context, "txtYourRace", scene->menu.yourRace);
        setUIRaceValueByName(context, "txtEnemyRace", scene->menu.enemyRace);
    }
}

// menu button handlers
void handleMenuSinglePlayer(WarContext* context, WarEntity* entity)
{
    showOrHideMainMenu(context, false);
    showOrHideSinglePlayer(context, true);
    showOrHideCustomGame(context, false);
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

void handleCustomGame(WarContext* context, WarEntity* entity)
{
    showOrHideMainMenu(context, false);
    showOrHideSinglePlayer(context, false);
    showOrHideCustomGame(context, true);
}

void handleSinglePlayerCancel(WarContext* context, WarEntity* entity)
{
    showOrHideMainMenu(context, true);
    showOrHideSinglePlayer(context, false);
    showOrHideCustomGame(context, false);
}

void handleYourRaceLeft(WarContext* context, WarEntity* entity)
{
    WarScene* scene = context->scene;

    if (scene->menu.yourRace > WAR_RACE_NEUTRAL)
    {
        scene->menu.yourRace--;
        setUIRaceValueByName(context, "txtYourRace", scene->menu.yourRace);
    }
}

void handleYourRaceRight(WarContext* context, WarEntity* entity)
{
    WarScene* scene = context->scene;

    if (scene->menu.yourRace < WAR_RACE_ORCS)
    {
        scene->menu.yourRace++;
        setUIRaceValueByName(context, "txtYourRace", scene->menu.yourRace);
    }
}

void handleEnemyRaceLeft(WarContext* context, WarEntity* entity)
{
    WarScene* scene = context->scene;

    if (scene->menu.enemyRace > WAR_RACE_NEUTRAL)
    {
        scene->menu.enemyRace--;
        setUIRaceValueByName(context, "txtEnemyRace", scene->menu.enemyRace);
    }
}

void handleEnemyRaceRight(WarContext* context, WarEntity* entity)
{
    WarScene* scene = context->scene;

    if (scene->menu.enemyRace < WAR_RACE_ORCS)
    {
        scene->menu.enemyRace++;
        setUIRaceValueByName(context, "txtEnemyRace", scene->menu.enemyRace);
    }
}

void handleCustomGameOk(WarContext* context, WarEntity* entity)
{
    WarScene* scene = context->scene;

    WarMap* map = createCustomMap(context, 147, scene->menu.yourRace, scene->menu.enemyRace);
    setNextMap(context, map, 1.0f);
}