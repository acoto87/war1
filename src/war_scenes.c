WarScene* createScene(WarContext* context)
{
    WarScene* scene = (WarScene*)xcalloc(1, sizeof(WarScene));

    initEntityManager(&scene->entityManager);

    return scene;
}

void freeScene(WarContext* context)
{
    WarScene* scene = context->scene;

    context->audioEnabled = false;

    WarEntityManager* manager = &scene->entityManager;
    WarEntityListFree(&manager->entities);
    WarEntityMapFree(&manager->entitiesByType);
    WarUnitMapFree(&manager->unitsByType);
    WarEntityIdMapFree(&manager->entitiesById);
    WarEntityListFree(&manager->uiEntities);

    context->scene = NULL;
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
