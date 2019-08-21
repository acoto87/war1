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

    switch (scene->type)
    {
        case WAR_SCENE_MAIN_MENU:
        {
            enterSceneMainMenu(context);
            break;
        }

        default:
        {
            logWarning("Can't enter scene of type: %d\n", scene->type);
            break;
        }
    }
}

void updateScene(WarContext* context)
{
    updateUIButtons(context);
    updateUICursor(context);
}

void renderScene(WarContext* context)
{
    NVGcontext* gfx = context->gfx;

    nvgSave(gfx);
    nvgScale(gfx, context->globalScale, context->globalScale);

    renderUIEntities(context);

    nvgRestore(gfx);
}

void leaveScene(WarContext* context)
{
    if (context->scene)
    {
        freeScene(context->scene);
        context->scene = NULL;
    }
}
