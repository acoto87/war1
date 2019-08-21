void enterSceneBlizzard(WarContext* context)
{
    WarScene* scene = context->scene;
    scene->blizzard.time = 3.0f;

    createUIImage(context, "imgBackground", imageResourceRef(216), VEC2_ZERO);
    createAudio(context, WAR_LOGO, true);
}

void updateSceneBlizzard(WarContext* context)
{
    WarScene* scene = context->scene;

    scene->blizzard.time -= context->deltaTime;

    if (scene->blizzard.time <= 0)
    {
        WarScene* scene = createScene(context, WAR_SCENE_MAIN_MENU);
        setNextScene(context, scene, 0.3f);
    }
}