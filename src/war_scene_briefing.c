void enterSceneBriefing(WarContext* context)
{
    WarScene* scene = context->scene;
    scene->briefing.time = 20.0f;

    WarSpriteResourceRef backgroundResourceRef;

    if (scene->briefing.race == WAR_RACE_HUMANS)
        backgroundResourceRef = imageResourceRef(421);
    else if (scene->briefing.race == WAR_RACE_ORCS)
        backgroundResourceRef = imageResourceRef(422);
    else
        logError("Not allowed briefing scenes for race: %d\n", scene->briefing.race);

    WarSprite s = createSpriteFromResourceIndex(context, imageResourceRef(426));
    WarSpriteAnimation* anim1 = createAnimation("anim1", s, 0.2f, true);
    addAnimationFrames(anim1, 5, arrayArg(s32, 0, 1, 2, 3, 4));
    WarSpriteAnimationListAdd(&scene->animations, anim1);

    createUIImage(context, "imgBackground", backgroundResourceRef, VEC2_ZERO);
    createAudio(context, WAR_CAMPAIGNS_HUMAN_01_INTRO, true);
}

void updateSceneBriefing(WarContext* context)
{
    WarScene* scene = context->scene;

    scene->briefing.time -= context->deltaTime;

    if (scene->briefing.time <= 0)
    {
        WarMap* map = createMap(context, scene->briefing.mapType);
        setNextMap(context, map, 1.0f);
    }

    updateUIAnimations(context);
}
