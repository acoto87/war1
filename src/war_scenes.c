#include "war_scenes.h"

#include "war_animations.h"
#include "war_cheats.h"
#include "war_scene_blizzard.h"
#include "war_scene_briefing.h"
#include "war_scene_download.h"

WarSceneDescriptor sceneDescriptors[WAR_SCENE_COUNT] =
{
    { WAR_SCENE_DOWNLOAD,   wsd_enterSceneDownload, NULL, wsd_updateSceneDownload },
    { WAR_SCENE_BLIZZARD,   wsbl_enterSceneBlizzard, NULL, wsbl_updateSceneBlizzard },
    { WAR_SCENE_MAIN_MENU,  wsm_enterSceneMainMenu, NULL, NULL },
    { WAR_SCENE_BRIEFING,   wsbr_enterSceneBriefing, NULL, wsbr_updateSceneBriefing }
};

WarScene* wsc_createScene(WarContext* context, WarSceneType type)
{
    WarScene* scene = (WarScene*)wm_alloc(sizeof(WarScene));
    scene->type = type;

    we_initEntityManager(context, &scene->entityManager);

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
        wcheatp_updateCheatsPanel(context);
        wui_updateUIButtons(context, !cheatsEnabledAndVisible(scene));
        wui_updateUICursor(context);
        wanim_updateAnimations(context);
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
    WarEntityList* uiEntities = we_getEntities(context);
    for (s32 i = 0; i < uiEntities->count; i++)
    {
        WarEntity* entity = uiEntities->items[i];
        if (entity && (wui_isUIEntity(entity) || entity->type == WAR_ENTITY_TYPE_ANIMATION))
        {
            we_renderEntity(context, entity);
        }
    }
}
