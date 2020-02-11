void enterSceneBriefingHumans(WarContext* context)
{
    WarScene* scene = context->scene;

    WarCampaignMapData data = getCampaignData(scene->briefing.mapType);

    scene->briefing.time = data.briefingDuration;

    createUIImage(context, "imgBackground", imageResourceRef(421), VEC2_ZERO);

    WarEntity* animEntity = createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
    addAnimationsComponent(context, animEntity);

    WarSpriteAnimation* anim1 = createAnimationFromResourceIndex(context, "anim1", imageResourceRef(428), 0.2f, true);
    anim1->offset = vec2i(83, 37);
    anim1->loopDelay = 2.0f;
    addAnimationFramesRange(anim1, 0, 4);
    addAnimationFramesRange(anim1, 4, 0);
    addAnimation(animEntity, anim1);

    WarSpriteAnimation* anim2 = createAnimationFromResourceIndex(context, "anim2", imageResourceRef(429), 0.2f, true);
    anim2->offset = vec2i(207, 29);
    anim1->loopDelay = 2.0f;
    addAnimationFramesRange(anim2, 0, 20);
    addAnimation(animEntity, anim2);

    WarSpriteAnimation* anim3 = createAnimationFromResourceIndex(context, "anim3", imageResourceRef(430), 0.1f, true);
    anim3->offset = vec2i(21, 17);
    addAnimationFramesRange(anim3, 0, 20);
    addAnimation(animEntity, anim3);

    WarSpriteAnimation* anim4 = createAnimationFromResourceIndex(context, "anim4", imageResourceRef(431), 0.1f, true);
    anim4->offset = vec2i(275, 21);
    addAnimationFramesRange(anim4, 0, 20);
    addAnimation(animEntity, anim4);

    WarEntity* briefingText = createUIText(context, "txtBriefing", 1, 10, data.briefingText, vec2i(20, 160));
    setUITextColor(briefingText, u8RgbColor(255, 215, 138));
    setUITextMultiline(briefingText, true);
    setUITextBoundings(briefingText, vec2f(context->originalWindowWidth - 40, 200));
    setUITextHorizontalAlign(briefingText, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(briefingText, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(briefingText, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(briefingText, WAR_TEXT_WRAP_CHAR);
    setUITextLineHeight(briefingText, 150);

    if (!isDemo(context))
        createAudio(context, data.briefingAudioId, false);
}

void enterSceneBriefingOrcs(WarContext* context)
{
    WarScene* scene = context->scene;

    WarCampaignMapData data = getCampaignData(scene->briefing.mapType);

    scene->briefing.time = data.briefingDuration;

    createUIImage(context, "imgBackground", imageResourceRef(422), VEC2_ZERO);

    WarEntity* animEntity = createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
    addAnimationsComponent(context, animEntity);

    WarSpriteAnimation* anim1 = createAnimationFromResourceIndex(context, "anim1", imageResourceRef(426), 0.2f, true);
    anim1->offset = vec2i(18, 67);
    anim1->loopDelay = 2.0f;
    addAnimationFramesRange(anim1, 0, 4);
    addAnimationFramesRange(anim1, 4, 0);
    addAnimation(animEntity, anim1);

    WarSpriteAnimation* anim2 = createAnimationFromResourceIndex(context, "anim2", imageResourceRef(427), 0.2f, true);
    anim2->offset = vec2i(202, 52);
    anim1->loopDelay = 2.0f;
    addAnimationFramesRange(anim2, 0, 4);
    addAnimationFramesRange(anim2, 4, 0);
    addAnimation(animEntity, anim2);

    if (!isDemo(context))
    {
        WarSpriteAnimation* anim3 = createAnimationFromResourceIndex(context, "anim3", imageResourceRef(425), 0.1f, true);
        anim3->offset = vec2i(140, 66);
        addAnimationFramesRange(anim3, 0, 30);
        addAnimation(animEntity, anim3);
    }

    WarEntity* briefingText = createUIText(context, "txtBriefing", 1, 10, data.briefingText, vec2i(20, 160));
    setUITextColor(briefingText, u8RgbColor(255, 215, 138));
    setUITextMultiline(briefingText, true);
    setUITextBoundings(briefingText, vec2f(context->originalWindowWidth - 40, 200));
    setUITextHorizontalAlign(briefingText, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(briefingText, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(briefingText, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(briefingText, WAR_TEXT_WRAP_CHAR);
    setUITextLineHeight(briefingText, 150);

    if (!isDemo(context))
        createAudio(context, data.briefingAudioId, false);
}

void enterSceneBriefing(WarContext* context)
{
    WarScene* scene = context->scene;

    switch (scene->briefing.race)
    {
        case WAR_RACE_HUMANS:
        {
            enterSceneBriefingHumans(context);
            break;
        }

        case WAR_RACE_ORCS:
        {
            enterSceneBriefingOrcs(context);
            break;
        }

        default:
        {
            logError("Not allowed briefing scenes for race: %d\n", scene->briefing.race);
            break;
        }
    }
}

void updateSceneBriefing(WarContext* context)
{
    WarInput* input = &context->input;
    WarScene* scene = context->scene;

    scene->briefing.time -= context->deltaTime;

    WarEntity* txtBriefing = findUIEntity(context, "txtBriefing");
    if (txtBriefing)
    {
        vec2 position = txtBriefing->transform.position;
        position.y -= 10 * context->deltaTime;
        txtBriefing->transform.position = position;
    }

    updateAnimations(context);

    if (scene->briefing.time <= 0 ||
        wasButtonPressed(input, WAR_MOUSE_LEFT) ||
        wasKeyPressed(input, WAR_KEY_ENTER) ||
        wasKeyPressed(input, WAR_KEY_SPACE))
    {
        WarMap* map = createMap(context, scene->briefing.mapType);
        setNextMap(context, map, 1.0f);
    }
}
