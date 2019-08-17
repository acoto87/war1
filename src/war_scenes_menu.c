void createSceneMenu(WarContext* context)
{
    WarScene* scene = context->scene;

    WarSpriteResourceRef invalidRef = invalidResourceRef();
    WarSpriteResourceRef mediumNormalRef = imageResourceRef(239);
    WarSpriteResourceRef mediumPressedRef = imageResourceRef(240);
    WarSpriteResourceRef smallNormalRef = imageResourceRef(241);
    WarSpriteResourceRef smallPressedRef = imageResourceRef(242);

    WarEntity* uiEntity;

    uiEntity = createUIImage(context, "imgMenuBackground", imageResourceRef(261), VEC2_ZERO);

    uiEntity = createUITextButton(
        context, "btnMenuSinglePlayer",
        1, 10, "Start a new game",
        mediumNormalRef,
        mediumPressedRef,
        invalidRef,
        vec2i(104, 85));
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
    setUIButtonHotKey(uiEntity, WAR_KEY_Q);
    setUITextHighlight(uiEntity, 0, 1);

    createUICursor(context, "cursor", WAR_CURSOR_ARROW, VEC2_ZERO);

    return scene;
}