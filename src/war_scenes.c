#include "war_scenes.h"

#include "war_animations.h"
#include "war_cheats.h"
#include "war_scene_blizzard.h"
#include "war_scene_briefing.h"
#include "war_scene_download.h"

WarSceneDescriptor sceneDescriptors[WAR_SCENE_COUNT] =
{
    { WAR_SCENE_DOWNLOAD,   wsd_enterSceneDownload, NULL, wsd_updateSceneDownload },
    { WAR_SCENE_BLIZZARD,   wsb_enterSceneBlizzard, NULL, wsb_updateSceneBlizzard },
    { WAR_SCENE_MAIN_MENU,  wsm_enterSceneMainMenu, NULL, NULL },
    { WAR_SCENE_BRIEFING,   wsbr_enterSceneBriefing, NULL, wsbr_updateSceneBriefing }
};

WarScene* wsc_createScene(WarContext* context, WarSceneType type)
{
    WarScene* scene = (WarScene*)war_malloc(sizeof(WarScene));
    scene->type = type;

    went_initEntityManager(context, &scene->entityManager);

    return scene;
}

void wsc_freeScene(WarScene* scene)
{
    WarEntityManager* manager = &scene->entityManager;
    WarEntityListFree(&manager->entities);
    WarEntityMapFree(&manager->entitiesByType);
    WarUnitMapFree(&manager->unitsByType);
    WarEntityIdMapFree(&manager->entitiesById);
    WarEntityListFree(&manager->uiEntities);
}

void wsc_enterScene(WarContext* context)
{
    WarScene* scene = context->scene;

    if (!inRange(scene->type, WAR_SCENE_DOWNLOAD, WAR_SCENE_COUNT))
    {
        logError("Unkown scene type: %d", scene->type);
        return;
    }

    WarSceneFunc enterSceneFunc = sceneDescriptors[scene->type].enterSceneFunc;
    if (enterSceneFunc)
    {
        enterSceneFunc(context);
    }
}

void wsc_updateScene(WarContext* context)
{
    WarScene* scene = context->scene;

    if (!inRange(scene->type, WAR_SCENE_DOWNLOAD, WAR_SCENE_COUNT))
    {
        logError("Unkown scene type: %d", scene->type);
        return;
    }

    WarSceneFunc updateSceneFunc = sceneDescriptors[scene->type].updateSceneFunc;
    if (updateSceneFunc)
    {
        updateSceneFunc(context);
    }
    else
    {
        wcp_updateCheatsPanel(context);
        wui_updateUIButtons(context, !cheatsEnabledAndVisible(scene));
        wui_updateUICursor(context);
        wani_updateAnimations(context);
    }
}

void wsc_leaveScene(WarContext* context)
{
    WarScene* scene = context->scene;
    if (!scene)
        return;

    if (!inRange(scene->type, WAR_SCENE_DOWNLOAD, WAR_SCENE_COUNT))
    {
        logError("Unkown scene type: %d", scene->type);
        return;
    }

    WarSceneFunc leaveSceneFunc = sceneDescriptors[scene->type].leaveSceneFunc;
    if (leaveSceneFunc)
    {
        leaveSceneFunc(context);
    }
    else
    {
        wsc_freeScene(context->scene);
        context->scene = NULL;
    }
}

void wsc_renderScene(WarContext* context)
{
    WarEntityList* uiEntities = went_getEntities(context);
    for (s32 i = 0; i < uiEntities->count; i++)
    {
        WarEntity* entity = uiEntities->items[i];
        if (entity && (wui_isUIEntity(entity) || entity->type == WAR_ENTITY_TYPE_ANIMATION))
        {
            went_renderEntity(context, entity);
        }
    }
}
