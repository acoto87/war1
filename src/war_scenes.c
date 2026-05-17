#include "war_scenes.h"

#include "war_animations.h"
#include "war_cheats.h"
#include "war_scene_blizzard.h"
#include "war_scene_briefing.h"
#include "war_scene_download.h"
#include "war_scene_menu.h"

WarSceneDescriptor sceneDescriptors[WAR_SCENE_COUNT] =
{
    { WAR_SCENE_DOWNLOAD,   wsc_enterSceneDownload, NULL, wsc_updateSceneDownload, wsc_renderSceneDownload, NULL                    },
    { WAR_SCENE_BLIZZARD,   wsc_enterSceneBlizzard, NULL, wsc_updateSceneBlizzard, wsc_renderSceneBlizzard, NULL                    },
    { WAR_SCENE_MAIN_MENU,  wsc_enterSceneMainMenu, NULL, NULL,                    wsc_renderSceneMainMenu, NULL                   },
    { WAR_SCENE_BRIEFING,   wsc_enterSceneBriefing, NULL, wsc_updateSceneBriefing, wsc_renderSceneBriefing, wsc_renderOverlayBriefing }
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
    if (scene->type == WAR_SCENE_BRIEFING)
    {
        wstr_free(scene->briefing.briefingText);
    }

    WarEntityManager* manager = &scene->entityManager;
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
    TracyCZoneN(ctx, "UpdateScene", 1);

    WarScene* scene = context->scene;

    if (!inRange(scene->type, WAR_SCENE_DOWNLOAD, WAR_SCENE_COUNT))
    {
        logError("Unkown scene type: %d", scene->type);
        TracyCZoneEnd(ctx);
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
        wanim_updateAnimations(context);
    }

    TracyCZoneEnd(ctx);
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
    TracyCZoneN(ctx, "RenderScene", 1);

    WarScene* scene = context->scene;
    WarSceneFunc renderSceneFunc = sceneDescriptors[scene->type].renderSceneFunc;
    if (renderSceneFunc)
    {
        renderSceneFunc(context);
    }

    WarEntityList* uiEntities = we_getUIEntities(context);
    assert(uiEntities);

    for (s32 i = 0; i < uiEntities->count; i++)
    {
        WarEntity* entity = uiEntities->items[i];
        if (entity && entity->id != 0)
        {
            we_renderEntity(context, entity);
        }
    }

    WarEntityList* animations = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_ANIMATION);
    assert(animations);

    for(s32 i = 0; i < animations->count; i++)
    {
        WarEntity* entity = animations->items[i];
        if (entity && entity->id != 0)
        {
            we_renderEntity(context, entity);
        }
    }

    // Overlay: rendered on top of both retained UI entities and animations.
    WarSceneFunc renderOverlayFunc = sceneDescriptors[scene->type].renderOverlayFunc;
    if (renderOverlayFunc)
    {
        renderOverlayFunc(context);
    }

    // Cheat panel (imui) — only renders if cheatStatus.enabled is true for this scene.
    wcheatp_renderCheatsPanel(context);

    TracyCZoneEnd(ctx);
}
