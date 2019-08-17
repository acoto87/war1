WarScene* createScene(WarContext* context)
{
    WarScene* scene = (WarScene*)xcalloc(1, sizeof(WarScene));

    initEntityManager(&scene->entityManager);

    return scene;
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
