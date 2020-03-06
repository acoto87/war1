void setSpeedStr(WarMapSpeed value, char buffer[])
{
    switch (value)
    {
        case WAR_SPEED_SLOWEST: strcpy(buffer, "Slowest");  break;
        case WAR_SPEED_SLOW:    strcpy(buffer, "Slow");     break;
        case WAR_SPEED_NORMAL:  strcpy(buffer, "Normal");   break;
        case WAR_SPEED_FASTER:  strcpy(buffer, "Faster");   break;
        case WAR_SPEED_FASTEST: strcpy(buffer, "Fastest");  break;
    }
}

void createMenu(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    vec2 menuPanel = rectTopLeft(map->menuPanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUIRect(
        context, "rectMenuBackdrop",
        VEC2_ZERO, vec2i(context->windowWidth, context->windowHeight),
        u8RgbaColor(0, 0, 0, 150));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImage(context, "imgMenuBackground", imageResourceRefFromPlayer(player, 233, 234), menuPanel);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIText(
        context, "txtMenuHeader", 1, 10, "Warcraft",
        vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUITextButton(
        context, "btnMenuSave",
        1, 10, "Save",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonHotKey(uiEntity, WAR_KEY_S);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnMenuLoad",
        1, 10, "Load",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(78, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonHotKey(uiEntity, WAR_KEY_L);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnMenuOptions",
        1, 10, "Options",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 45)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleOptions);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnMenuObjectives",
        1, 10, "Objectives",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 65)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleObjectives);
    setUIButtonHotKey(uiEntity, WAR_KEY_J);
    setUITextHighlight(uiEntity, 2, 1);

    uiEntity = createUITextButton(
        context, "btnMenuRestart",
        1, 10, "Restart scenario",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 85)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleRestart);
    setUIButtonHotKey(uiEntity, WAR_KEY_R);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnMenuContinue",
        1, 10, "Continue",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleContinue);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnMenuQuit",
        1, 10, "Quit",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(78, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleQuit);
    setUIButtonHotKey(uiEntity, WAR_KEY_Q);
    setUITextHighlight(uiEntity, 0, 1);
}

void createOptionsMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 menuPanel = rectTopLeft(map->menuPanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);
    WarSpriteResourceRef leftArrowNormalRef = imageResourceRef(244);
    WarSpriteResourceRef leftArrowPressedRef = imageResourceRef(245);
    WarSpriteResourceRef rightArrowNormalRef = imageResourceRef(246);
    WarSpriteResourceRef rightArrowPressedRef = imageResourceRef(247);

    WarEntity* uiEntity;

    uiEntity = createUIText(context, "txtOptionsHeader",
        1, 10, "Options", vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsGameSpeedLabel",
        1, 10, "Game Speed", vec2Addv(menuPanel, vec2i(0, 25)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsMusicVolLabel",
        1, 10, "Music Vol", vec2Addv(menuPanel, vec2i(0, 42)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsSFXVolLabel",
        1, 10, "SFX Vol", vec2Addv(menuPanel, vec2i(0, 59)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsMouseScrollLabel",
        1, 10, "Mouse Scroll", vec2Addv(menuPanel, vec2i(0, 76)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsKeyScrollLabel",
        1, 10, "Key Scroll", vec2Addv(menuPanel, vec2i(0, 93)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(75, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_RIGHT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsGameSpeedValue",
        1, 10, "Fastest", vec2Addv(menuPanel, vec2i(92, 25)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsMusicVolValue",
        1, 10, "100", vec2Addv(menuPanel, vec2i(92, 42)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsSFXVolValue",
        1, 10, "82", vec2Addv(menuPanel, vec2i(92, 59)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsMouseScrollValue",
        1, 10, "Slowest", vec2Addv(menuPanel, vec2i(92, 76)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtOptionsKeyScrollValue",
        1, 10, "Normal", vec2Addv(menuPanel, vec2i(92, 93)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(42, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIImageButton(
        context, "btnOptionsGameSpeedDec",
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 22)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameSpeedDec);

    uiEntity = createUIImageButton(
        context, "btnOptionsGameSpeedInc",
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 22)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameSpeedInc);

    uiEntity = createUIImageButton(
        context, "btnOptionsMusicVolDec",
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 39)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMusicVolDec);

    uiEntity = createUIImageButton(
        context, "btnOptionsMusicVolInc",
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 39)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMusicVolInc);

    uiEntity = createUIImageButton(
        context, "btnOptionsSFXVolDec",
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 56)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSfxVolDec);

    uiEntity = createUIImageButton(
        context, "btnOptionsSFXVolInc",
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 56)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleSfxVolInc);

    uiEntity = createUIImageButton(
        context, "btnOptionsMouseScrollDec",
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 73)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMouseScrollSpeedDec);

    uiEntity = createUIImageButton(
        context, "btnOptionsMouseScrollInc",
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 73)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleMouseScrollSpeedInc);

    uiEntity = createUIImageButton(
        context, "btnOptionsKeyScrollDec",
        leftArrowNormalRef,
        leftArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(76, 90)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleKeyScrollSpeedDec);

    uiEntity = createUIImageButton(
        context, "btnOptionsKeyScrollInc",
        rightArrowNormalRef,
        rightArrowPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(133, 90)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleKeyScrollSpeedInc);

    uiEntity = createUITextButton(
        context, "btnOptionsOk",
        1, 10, "Ok",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 115)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleOptionsOk);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnOptionsCancel",
        1, 10, "Cancel",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(78, 115)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleOptionsCancel);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void createObjectivesMenu(WarContext* context)
{
    WarMap* map = context->map;

    WarResource* levelInfo = getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarCampaignMapData campaignData = getCampaignData((WarCampaignMapType)map->levelInfoIndex);

    vec2 menuPanel = rectTopLeft(map->menuPanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);

    WarEntity* uiEntity;

    uiEntity = createUIText(context, "txtObjectivesHeader",
        1, 10, "Objectives", vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(context, "txtObjectivesText",
        1, 10, campaignData.objectives, vec2Addv(menuPanel, vec2i(10, 26)));
    setUIEntityStatus(uiEntity, false);
    setUITextMultiline(uiEntity, true);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width - 20, 75));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(uiEntity, WAR_TEXT_WRAP_CHAR);
    setUITextTrimming(uiEntity, WAR_TEXT_TRIM_SPACES);

    uiEntity = createUITextButton(
        context, "btnObjectivesMenu",
        1, 10, "Menu",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleObjectivesMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_M);
    setUITextHighlight(uiEntity, 0, 1);
}

void createRestartMenu(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUIImage(context, "imgMessageMenuBackground", imageResourceRefFromPlayer(player, 235, 236), messagePanel);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIText(context, "txtRestartText",
        1, 10, "Are you sure you want to restart?",
        vec2Addv(messagePanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->messagePanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUITextButton(
        context, "btnRestartRestart",
        1, 10, "Restart",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleRestartRestart);
    setUIButtonHotKey(uiEntity, WAR_KEY_R);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnRestartCancel",
        1, 10, "Cancel",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(210, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleRestartCancel);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void createGameOverMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUIText(context, "txtGameOverText",
        1, 10, "You are victorious!",
        vec2Addv(messagePanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->messagePanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUITextButton(
        context, "btnGameOverSave",
        1, 10, "Save",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameOverSave);
    setUIButtonHotKey(uiEntity, WAR_KEY_S);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnGameOverContinue",
        1, 10, "Continue",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(210, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameOverContinue);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnGameOverOk",
        1, 10, "Ok",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(116, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleGameOverContinue);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);
    setUITextHighlight(uiEntity, 0, 1);
}

void createQuitMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUIText(context, "txtQuitText",
        1, 10, "Are you sure you want to quit?",
        vec2Addv(messagePanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->messagePanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUITextButton(
        context, "btnQuitQuit",
        1, 10, "Quit",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleQuitQuit);
    setUIButtonHotKey(uiEntity, WAR_KEY_Q);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnQuitMenu",
        1, 10, "Menu",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(115, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleQuitMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_M);
    setUITextHighlight(uiEntity, 0, 1);

    uiEntity = createUITextButton(
        context, "btnQuitCancel",
        1, 10, "Cancel",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(210, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleQuitCancel);
    setUIButtonHotKey(uiEntity, WAR_KEY_C);
    setUITextHighlight(uiEntity, 0, 1);
}

void createDemoEndMenu(WarContext* context)
{
    WarMap* map = context->map;

    vec2 menuPanel = rectTopLeft(map->menuPanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);

    WarEntity* uiEntity;

    uiEntity = createUIText(
        context, "txtDemoEndHeader",
        1, 10,
        "Thanks for playing!",
        vec2Addv(menuPanel, vec2i(0, 10)));
    setUIEntityStatus(uiEntity, false);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width, 12));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_CENTER);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_MIDDLE);

    uiEntity = createUIText(
        context, "txtDemoEndText",
        1, 7,
        "This is a non-profit project with\n"
        "the personal goal of learning to\n"
        "do RTS engines\n"
        "\n"
        "This is not an official Blizzard\n"
        "product. Warcraft and all assets\n"
        "you see are registered trademarks\n"
        "of Blizzard Entertainment.\n"
        "\n"
        "Made by @acoto87",
        vec2Addv(menuPanel, vec2i(8, 26)));
    setUIEntityStatus(uiEntity, false);
    setUITextMultiline(uiEntity, true);
    setUITextBoundings(uiEntity, vec2f(map->menuPanel.width - 16, 75));
    setUITextHorizontalAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(uiEntity, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(uiEntity, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(uiEntity, WAR_TEXT_WRAP_CHAR);

    uiEntity = createUITextButton(
        context, "btnDemoEndMenu",
        1, 10, "Menu",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 105)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleDemoEndMenu);
    setUIButtonHotKey(uiEntity, WAR_KEY_M);
    setUITextHighlight(uiEntity, 0, 1);
}

void setUITextSpeedValueByName(WarContext* context, const char* name, WarMapSpeed value)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        char speedStr[10];
        setSpeedStr(value, speedStr);
        setUIText(entity, speedStr);
        setUITextHighlight(entity, NO_HIGHLIGHT, 0);
    }
}

void setUITextS32ValueByName(WarContext* context, const char* name, s32 value)
{
    WarEntity* entity = findUIEntity(context, name);
    if (entity)
    {
        setUITextFormat(entity, "%d", value);
        setUITextHighlight(entity, NO_HIGHLIGHT, 0);
    }
}

void enableOrDisableCommandButtons(WarContext* context, bool interactive)
{
    setUIButtonInteractiveByName(context, "btnCommand0", interactive);
    setUIButtonInteractiveByName(context, "btnCommand1", interactive);
    setUIButtonInteractiveByName(context, "btnCommand2", interactive);
    setUIButtonInteractiveByName(context, "btnCommand3", interactive);
    setUIButtonInteractiveByName(context, "btnCommand4", interactive);
    setUIButtonInteractiveByName(context, "btnCommand5", interactive);

    setUIButtonInteractiveByName(context, "btnMenu", interactive);
}

void showOrHideMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, "rectMenuBackdrop", status);
    setUIEntityStatusByName(context, "imgMenuBackground", status);
    setUIEntityStatusByName(context, "txtMenuHeader", status);
    setUIEntityStatusByName(context, "btnMenuSave", status);
    setUIEntityStatusByName(context, "btnMenuLoad", status);
    setUIEntityStatusByName(context, "btnMenuOptions", status);
    setUIEntityStatusByName(context, "btnMenuObjectives", status);
    setUIEntityStatusByName(context, "btnMenuRestart", status);
    setUIEntityStatusByName(context, "btnMenuContinue", status);
    setUIEntityStatusByName(context, "btnMenuQuit", status);
}

void showOrHideOptionsMenu(WarContext* context, bool status)
{
    WarMap* map = context->map;

    setUIEntityStatusByName(context, "rectMenuBackdrop", status);
    setUIEntityStatusByName(context, "imgMenuBackground", status);
    setUIEntityStatusByName(context, "txtOptionsHeader", status);

    setUIEntityStatusByName(context, "txtOptionsGameSpeedLabel", status);
    setUIEntityStatusByName(context, "txtOptionsGameSpeedValue", status);
    setUIEntityStatusByName(context, "btnOptionsGameSpeedDec", status);
    setUIEntityStatusByName(context, "btnOptionsGameSpeedInc", status);

    setUIEntityStatusByName(context, "txtOptionsMusicVolLabel", status);
    setUIEntityStatusByName(context, "txtOptionsMusicVolValue", status);
    setUIEntityStatusByName(context, "btnOptionsMusicVolDec", status);
    setUIEntityStatusByName(context, "btnOptionsMusicVolInc", status);

    setUIEntityStatusByName(context, "txtOptionsSFXVolLabel", status);
    setUIEntityStatusByName(context, "txtOptionsSFXVolValue", status);
    setUIEntityStatusByName(context, "btnOptionsSFXVolDec", status);
    setUIEntityStatusByName(context, "btnOptionsSFXVolInc", status);

    setUIEntityStatusByName(context, "txtOptionsMouseScrollLabel", status);
    setUIEntityStatusByName(context, "txtOptionsMouseScrollValue", status);
    setUIEntityStatusByName(context, "btnOptionsMouseScrollDec", status);
    setUIEntityStatusByName(context, "btnOptionsMouseScrollInc", status);

    setUIEntityStatusByName(context, "txtOptionsKeyScrollLabel", status);
    setUIEntityStatusByName(context, "txtOptionsKeyScrollValue", status);
    setUIEntityStatusByName(context, "btnOptionsKeyScrollDec", status);
    setUIEntityStatusByName(context, "btnOptionsKeyScrollInc", status);

    setUIEntityStatusByName(context, "btnOptionsOk", status);
    setUIEntityStatusByName(context, "btnOptionsCancel", status);

    if (status)
    {
        map->settings.musicVol = (s32)(context->musicVolume * 100);
        map->settings.sfxVol = (s32)(context->soundVolume * 100);
        memcpy(&map->settings2, &map->settings, sizeof(WarMapSettings));

        setUITextSpeedValueByName(context, "txtOptionsGameSpeedValue", map->settings2.gameSpeed);
        setUITextS32ValueByName(context, "txtOptionsMusicVolValue", map->settings2.musicVol);
        setUITextS32ValueByName(context, "txtOptionsSFXVolValue", map->settings2.sfxVol);
        setUITextSpeedValueByName(context, "txtOptionsMouseScrollValue", map->settings2.mouseScrollSpeed);
        setUITextSpeedValueByName(context, "txtOptionsKeyScrollValue", map->settings2.keyScrollSpeed);
    }
}

void showOrHideObjectivesMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, "rectMenuBackdrop", status);
    setUIEntityStatusByName(context, "imgMenuBackground", status);
    setUIEntityStatusByName(context, "txtObjectivesHeader", status);
    setUIEntityStatusByName(context, "txtObjectivesText", status);
    setUIEntityStatusByName(context, "btnObjectivesMenu", status);
}

void showOrHideRestartMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, "rectMenuBackdrop", status);
    setUIEntityStatusByName(context, "imgMessageMenuBackground", status);
    setUIEntityStatusByName(context, "txtRestartText", status);
    setUIEntityStatusByName(context, "btnRestartRestart", status);
    setUIEntityStatusByName(context, "btnRestartCancel", status);
}

void showOrHideGameOverMenu(WarContext* context, bool status)
{
    WarMap* map = context->map;

    enableOrDisableCommandButtons(context, !status);

    setUIEntityStatusByName(context, "rectMenuBackdrop", status);
    setUIEntityStatusByName(context, "imgMessageMenuBackground", status);
    setUIEntityStatusByName(context, "txtGameOverText", status);
    setUIEntityStatusByName(context, "btnGameOverSave", status);
    setUIEntityStatusByName(context, "btnGameOverContinue", status);
    setUIEntityStatusByName(context, "btnGameOverOk", status);

    if (status)
    {
        if (map->result == WAR_LEVEL_RESULT_WIN)
        {
            WarEntity* gameOverText = findUIEntity(context, "txtGameOverText");
            setUIText(gameOverText, "You are victorious!");

            WarEntity* saveBtn = findUIEntity(context, "btnGameOverSave");
            setUIEntityStatus(saveBtn, true);

            WarEntity* continueBtn = findUIEntity(context, "btnGameOverContinue");
            setUIEntityStatus(continueBtn, true);

            WarEntity* okBtn = findUIEntity(context, "btnGameOverOk");
            setUIEntityStatus(okBtn, false);
        }
        else if (map->result == WAR_LEVEL_RESULT_LOSE)
        {
            WarEntity* gameOverText = findUIEntity(context, "txtGameOverText");
            setUIText(gameOverText, "You failed to archieve victory...");

            WarEntity* saveBtn = findUIEntity(context, "btnGameOverSave");
            setUIEntityStatus(saveBtn, false);

            WarEntity* continueBtn = findUIEntity(context, "btnGameOverContinue");
            setUIEntityStatus(continueBtn, false);

            WarEntity* okBtn = findUIEntity(context, "btnGameOverOk");
            setUIEntityStatus(okBtn, true);
        }
    }
}

void showOrHideQuitMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, "rectMenuBackdrop", status);
    setUIEntityStatusByName(context, "imgMessageMenuBackground", status);
    setUIEntityStatusByName(context, "txtQuitText", status);
    setUIEntityStatusByName(context, "btnQuitQuit", status);
    setUIEntityStatusByName(context, "btnQuitMenu", status);
    setUIEntityStatusByName(context, "btnQuitCancel", status);
}

void showDemoEndMenu(WarContext* context, bool status)
{
    enableOrDisableCommandButtons(context, !status);

    setUIEntityStatusByName(context, "rectMenuBackdrop", status);
    setUIEntityStatusByName(context, "imgMenuBackground", status);
    setUIEntityStatusByName(context, "txtDemoEndHeader", status);
    setUIEntityStatusByName(context, "txtDemoEndText", status);
    setUIEntityStatusByName(context, "btnDemoEndMenu", status);
}

// menu button handlers
void handleMenu(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    enableOrDisableCommandButtons(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleOptions(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideOptionsMenu(context, true);

    map->playing = false;
}

void handleObjectives(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideObjectivesMenu(context, true);

    map->playing = false;
}

void handleRestart(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideRestartMenu(context, true);

    map->playing = false;
}

void handleContinue(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    enableOrDisableCommandButtons(context, true);
    showOrHideMenu(context, false);

    map->playing = true;
}

void handleQuit(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideQuitMenu(context, true);

    map->playing = false;
}

void handleGameSpeedDec(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    if (map->settings2.gameSpeed > WAR_SPEED_SLOWEST)
    {
        map->settings2.gameSpeed--;
        setUITextSpeedValueByName(context, "txtOptionsGameSpeedValue", map->settings2.gameSpeed);
    }
}

void handleGameSpeedInc(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    if (map->settings2.gameSpeed < WAR_SPEED_FASTEST)
    {
        map->settings2.gameSpeed++;
        setUITextSpeedValueByName(context, "txtOptionsGameSpeedValue", map->settings2.gameSpeed);
    }
}

void handleMusicVolDec(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->settings2.musicVol = clamp(map->settings2.musicVol - 5, 0, 100);
    setUITextS32ValueByName(context, "txtOptionsMusicVolValue", map->settings2.musicVol);
}

void handleMusicVolInc(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->settings2.musicVol = clamp(map->settings2.musicVol + 5, 0, 100);
    setUITextS32ValueByName(context, "txtOptionsMusicVolValue", map->settings2.musicVol);
}

void handleSfxVolDec(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->settings2.sfxVol = clamp(map->settings2.sfxVol - 5, 0, 100);
    setUITextS32ValueByName(context, "txtOptionsSFXVolValue", map->settings2.sfxVol);
}

void handleSfxVolInc(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    map->settings2.sfxVol = clamp(map->settings2.sfxVol + 5, 0, 100);
    setUITextS32ValueByName(context, "txtOptionsSFXVolValue", map->settings2.sfxVol);
}

void handleMouseScrollSpeedDec(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    if (map->settings2.mouseScrollSpeed > WAR_SPEED_SLOWEST)
    {
        map->settings2.mouseScrollSpeed--;
        setUITextSpeedValueByName(context, "txtOptionsMouseScrollValue", map->settings2.mouseScrollSpeed);
    }
}

void handleMouseScrollSpeedInc(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    if (map->settings2.mouseScrollSpeed < WAR_SPEED_FASTEST)
    {
        map->settings2.mouseScrollSpeed++;
        setUITextSpeedValueByName(context, "txtOptionsMouseScrollValue", map->settings2.mouseScrollSpeed);
    }
}

void handleKeyScrollSpeedDec(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    if (map->settings2.keyScrollSpeed > WAR_SPEED_SLOWEST)
    {
        map->settings2.keyScrollSpeed--;
        setUITextSpeedValueByName(context, "txtOptionsKeyScrollValue", map->settings2.keyScrollSpeed);
    }
}

void handleKeyScrollSpeedInc(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    if (map->settings2.keyScrollSpeed < WAR_SPEED_FASTEST)
    {
        map->settings2.keyScrollSpeed++;
        setUITextSpeedValueByName(context, "txtOptionsKeyScrollValue", map->settings2.keyScrollSpeed);
    }
}

void handleOptionsOk(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    // persist the changes
    memcpy(&map->settings, &map->settings2, sizeof(WarMapSettings));
    context->musicVolume = (f32)map->settings.musicVol / 100;
    context->soundVolume = (f32)map->settings.sfxVol / 100;

    showOrHideOptionsMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleOptionsCancel(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideOptionsMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleObjectivesMenu(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideObjectivesMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleRestartRestart(WarContext* context, WarEntity* entity)
{
    s32 levelInfoIndex = context->map->levelInfoIndex;

    WarMap* map = createMap(context, levelInfoIndex);
    setNextMap(context, map, 1.0f);
}

void handleRestartCancel(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideRestartMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleGameOverSave(WarContext* context, WarEntity* entity)
{
    NOT_IMPLEMENTED();
}

void handleGameOverContinue(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    s32 levelInfoIndex = map->levelInfoIndex;

    if (map->result == WAR_LEVEL_RESULT_WIN)
    {
        WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
        scene->briefing.race = map->players[0].race;
        scene->briefing.mapType = levelInfoIndex + 2;
        setNextScene(context, scene, 1.0f);
    }
    else if (map->result == WAR_LEVEL_RESULT_LOSE)
    {
        WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
        scene->briefing.race = map->players[0].race;
        scene->briefing.mapType = levelInfoIndex;
        setNextScene(context, scene, 1.0f);
    }
    else
    {
        logError("It shouldn't reach here!");
        assert(false);
    }
}

void handleQuitQuit(WarContext* context, WarEntity* entity)
{
    glfwSetWindowShouldClose(context->window, true);
}

void handleQuitMenu(WarContext* context, WarEntity* entity)
{
    WarScene* scene = createScene(context, WAR_SCENE_MAIN_MENU);
    setNextScene(context, scene, 1.0f);
}

void handleQuitCancel(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideQuitMenu(context, false);
    showOrHideMenu(context, true);

    map->playing = false;
}

void handleDemoEndMenu(WarContext* context, WarEntity* entity)
{
    WarScene* scene = createScene(context, WAR_SCENE_MAIN_MENU);
    setNextScene(context, scene, 1.0f);
}