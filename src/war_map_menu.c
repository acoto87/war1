void createMenu(WarContext* context)
{
    WarMap* map = context->map;

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

    uiEntity = createUIImage(context, "imgMenuBackground", imageResourceRef(233), menuPanel);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtMenuHeader", 
        1, 10, "Warcraft", 
        vec2Addv(menuPanel, vec2i(0, 10)),
        vec2f(map->menuPanel.width, 12),
        WAR_TEXT_ALIGN_CENTER,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextButton(
        context, "btnMenuSave",
        1, 10, "Save Game",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextButton(
        context, "btnMenuLoad",
        1, 10, "Load Game",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 45)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextButton(
        context, "btnMenuOptions",
        1, 10, "Options",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 65)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonClickHandler(uiEntity, handleOptions);
    setUIButtonHotKey(uiEntity, WAR_KEY_O);

    uiEntity = createUITextButton(
        context, "btnMenuRestart",
        1, 10, "Restart scenario",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2Addv(menuPanel, vec2i(20, 85)));
    setUIEntityStatus(uiEntity, false);

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

    uiEntity = createUITextSized(
        context, "txtOptionsHeader", 
        1, 10, "Options", 
        vec2Addv(menuPanel, vec2i(0, 10)),
        vec2f(map->menuPanel.width, 12),
        WAR_TEXT_ALIGN_CENTER,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsGameSpeedLabel", 
        1, 10, "Game Speed", 
        vec2Addv(menuPanel, vec2i(0, 25)),
        vec2f(75, 12),
        WAR_TEXT_ALIGN_RIGHT,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsMusicVolLabel", 
        1, 10, "Music Vol", 
        vec2Addv(menuPanel, vec2i(0, 42)),
        vec2f(75, 12),
        WAR_TEXT_ALIGN_RIGHT,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsSFXVolLabel", 
        1, 10, "SFX Vol", 
        vec2Addv(menuPanel, vec2i(0, 59)),
        vec2f(75, 12),
        WAR_TEXT_ALIGN_RIGHT,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsMouseScrollLabel", 
        1, 10, "Mouse Scroll", 
        vec2Addv(menuPanel, vec2i(0, 76)),
        vec2f(75, 12),
        WAR_TEXT_ALIGN_RIGHT,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsKeyScrollLabel", 
        1, 10, "Key Scroll", 
        vec2Addv(menuPanel, vec2i(0, 93)),
        vec2f(75, 12),
        WAR_TEXT_ALIGN_RIGHT,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsGameSpeedValue", 
        1, 10, "Fastest", 
        vec2Addv(menuPanel, vec2i(92, 25)),
        vec2f(42, 12),
        WAR_TEXT_ALIGN_CENTER,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsMusicVolValue", 
        1, 10, "100", 
        vec2Addv(menuPanel, vec2i(92, 42)),
        vec2f(42, 12),
        WAR_TEXT_ALIGN_CENTER,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsSFXVolValue", 
        1, 10, "82", 
        vec2Addv(menuPanel, vec2i(92, 59)),
        vec2f(42, 12),
        WAR_TEXT_ALIGN_CENTER,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsMouseScrollValue", 
        1, 10, "Slowest", 
        vec2Addv(menuPanel, vec2i(92, 76)),
        vec2f(42, 12),
        WAR_TEXT_ALIGN_CENTER,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtOptionsKeyScrollValue", 
        1, 10, "Normal", 
        vec2Addv(menuPanel, vec2i(92, 93)),
        vec2f(42, 12),
        WAR_TEXT_ALIGN_CENTER,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsGameSpeedDec", 
        leftArrowNormalRef, 
        leftArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(76, 22)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsGameSpeedInc", 
        rightArrowNormalRef, 
        rightArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(133, 22)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsMusicVolDec", 
        leftArrowNormalRef, 
        leftArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(76, 39)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsMusicVolInc", 
        rightArrowNormalRef, 
        rightArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(133, 39)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsSFXVolDec", 
        leftArrowNormalRef, 
        leftArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(76, 56)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsSFXVolInc", 
        rightArrowNormalRef, 
        rightArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(133, 56)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsMouseScrollDec", 
        leftArrowNormalRef, 
        leftArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(76, 73)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsMouseScrollInc", 
        rightArrowNormalRef, 
        rightArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(133, 73)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsKeyScrollDec", 
        leftArrowNormalRef, 
        leftArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(76, 90)));
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUIImageButton(
        context, "btnOptionsKeyScrollInc", 
        rightArrowNormalRef, 
        rightArrowPressedRef, 
        invalidRef, 
        vec2Addv(menuPanel, vec2i(133, 90)));
    setUIEntityStatus(uiEntity, false);
    
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
}

void createGameOverMenu(WarContext* context)
{
    WarMap* map = context->map;
    
    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUIImage(context, "imgGameOverBackground", imageResourceRef(235), messagePanel);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextSized(
        context, "txtGameOverText", 1, 10,
        "You are victorious!", 
        vec2Addv(messagePanel, vec2i(0, 10)),
        vec2f(map->messagePanel.width, 12),
        WAR_TEXT_ALIGN_CENTER,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

    uiEntity = createUITextButton(
        context, "btnGameOverSave",
        1, 10, "Save",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(20, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonHotKey(uiEntity, WAR_KEY_S);

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
}

void createQuitMenu(WarContext* context)
{
    WarMap* map = context->map;
    
    vec2 messagePanel = rectTopLeft(map->messagePanel);

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUITextSized(
        context, "txtQuitText", 1, 10,
        "Are you sure you want to quit?", 
        vec2Addv(messagePanel, vec2i(0, 10)),
        vec2f(map->messagePanel.width, 12),
        WAR_TEXT_ALIGN_CENTER,
        WAR_TEXT_ALIGN_MIDDLE);
    setUIEntityStatus(uiEntity, false);

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

    uiEntity = createUITextButton(
        context, "btnQuitMenu",
        1, 10, "Menu",
        smallNormalRef,
        smallPressedRef,
        invalidRef,
        vec2Addv(messagePanel, vec2i(115, 25)));
    setUIEntityStatus(uiEntity, false);
    setUIButtonHotKey(uiEntity, WAR_KEY_M);

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
    setUIEntityStatusByName(context, "btnMenuRestart", status);
    setUIEntityStatusByName(context, "btnMenuContinue", status);
    setUIEntityStatusByName(context, "btnMenuQuit", status);
}

void showOrHideOptionsMenu(WarContext* context, bool status)
{
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
}

void showOrHideGameOverMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, "rectMenuBackdrop", status);
    setUIEntityStatusByName(context, "imgGameOverBackground", status);
    setUIEntityStatusByName(context, "txtGameOverText", status);
    setUIEntityStatusByName(context, "btnGameOverSave", status);
    setUIEntityStatusByName(context, "btnGameOverContinue", status);
}

void showOrHideQuitMenu(WarContext* context, bool status)
{
    setUIEntityStatusByName(context, "rectMenuBackdrop", status);
    setUIEntityStatusByName(context, "imgGameOverBackground", status);
    setUIEntityStatusByName(context, "txtQuitText", status);
    setUIEntityStatusByName(context, "btnQuitQuit", status);
    setUIEntityStatusByName(context, "btnQuitMenu", status);
    setUIEntityStatusByName(context, "btnQuitCancel", status);
}

// menu button handlers
void handleMenu(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    enableOrDisableCommandButtons(context, false);
    showOrHideMenu(context, true);

    map->status = MAP_MENU;
}

void handleContinue(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    enableOrDisableCommandButtons(context, true);
    showOrHideMenu(context, false);

    map->status = MAP_PLAYING;
}

void handleQuit(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideQuitMenu(context, true);

    map->status = MAP_QUIT;
}

void handleOptions(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideMenu(context, false);
    showOrHideOptionsMenu(context, true);
    
    map->status = MAP_OPTIONS;
}

void handleOptionsOk(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    // persist changes to options here

    showOrHideOptionsMenu(context, false);
    showOrHideMenu(context, true);

    map->status = MAP_MENU;
}

void handleOptionsCancel(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideOptionsMenu(context, false);
    showOrHideMenu(context, true);

    map->status = MAP_MENU;
}

void handleGameOverContinue(WarContext* context, WarEntity* entity)
{
    s32 levelInfoIndex = context->map->levelInfoIndex;

    freeMap(context);
    
    createMap(context, levelInfoIndex + 2);
}

void handleQuitQuit(WarContext* context, WarEntity* entity)
{
    glfwSetWindowShouldClose(context->window, true);
}

void handleQuitCancel(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    showOrHideQuitMenu(context, false);
    showOrHideMenu(context, true);

    map->status = MAP_MENU;
}