WarSceneDescriptor sceneDescriptors[WAR_SCENE_COUNT] =
{
    { WAR_SCENE_DOWNLOAD,   enterSceneDownload, NULL, updateSceneDownload },
    { WAR_SCENE_BLIZZARD,   enterSceneBlizzard, NULL, updateSceneBlizzard },
    { WAR_SCENE_MAIN_MENU,  enterSceneMainMenu, NULL, NULL },
    { WAR_SCENE_BRIEFING,   enterSceneBriefing, NULL, updateSceneBriefing }
};

WarScene* createScene(WarContext* context, WarSceneType type)
{
    WarScene* scene = (WarScene*)xcalloc(1, sizeof(WarScene));
    scene->type = type;

    initEntityManager(&scene->entityManager);

    return scene;
}

void freeScene(WarScene* scene)
{
    WarEntityManager* manager = &scene->entityManager;
    WarEntityListFree(&manager->entities);
    WarEntityMapFree(&manager->entitiesByType);
    WarUnitMapFree(&manager->unitsByType);
    WarEntityIdMapFree(&manager->entitiesById);
    WarEntityListFree(&manager->uiEntities);
}

void enterScene(WarContext* context)
{
    WarScene* scene = context->scene;

    if (!inRange(scene->type, WAR_SCENE_DOWNLOAD, WAR_SCENE_COUNT))
    {
        logError("Unkown scene type: %d\n", scene->type);
        return;
    }

    WarSceneFunc enterSceneFunc = sceneDescriptors[scene->type].enterSceneFunc;
    if (enterSceneFunc)
    {
        enterSceneFunc(context);
    }
}

void updateScene(WarContext* context)
{
    WarScene* scene = context->scene;

    if (!inRange(scene->type, WAR_SCENE_DOWNLOAD, WAR_SCENE_COUNT))
    {
        logError("Unkown scene type: %d\n", scene->type);
        return;
    }

    WarSceneFunc updateSceneFunc = sceneDescriptors[scene->type].updateSceneFunc;
    if (updateSceneFunc)
    {
        updateSceneFunc(context);
    }
    else
    {
        updateCheatsPanel(context);
        updateUIButtons(context, !cheatsEnabledAndVisible(scene));
        updateUICursor(context);
        updateAnimations(context);
    }
}

void leaveScene(WarContext* context)
{
    WarScene* scene = context->scene;
    if (!scene)
        return;

    if (!inRange(scene->type, WAR_SCENE_DOWNLOAD, WAR_SCENE_COUNT))
    {
        logError("Unkown scene type: %d\n", scene->type);
        return;
    }

    WarSceneFunc leaveSceneFunc = sceneDescriptors[scene->type].leaveSceneFunc;
    if (leaveSceneFunc)
    {
        leaveSceneFunc(context);
    }
    else
    {
        freeScene(context->scene);
        context->scene = NULL;
    }
}

void renderScene(WarContext* context)
{
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);
    nvgScale(gfx, context->globalScale, context->globalScale);

    WarEntityList* uiEntities = getEntities(context);
    for (s32 i = 0; i < uiEntities->count; i++)
    {
        WarEntity* entity = uiEntities->items[i];
        if (entity && (isUIEntity(entity) || entity->type == WAR_ENTITY_TYPE_ANIMATION))
        {
            renderEntity(context, entity);
        }
    }

    nvgRestore(gfx);
}
