#pragma once

#include "war_campaigns.h"
#include "war_cheats.h"
#include "war_entities.h"

enum _WarSceneType
{
    WAR_SCENE_DOWNLOAD,
    WAR_SCENE_BLIZZARD,
    WAR_SCENE_MAIN_MENU,
    WAR_SCENE_BRIEFING,

    WAR_SCENE_COUNT
};

enum _WarSceneDownloadState
{
    WAR_SCENE_DOWNLOAD_DOWNLOAD,
    WAR_SCENE_DOWNLOAD_CONFIRM,
    WAR_SCENE_DOWNLOAD_DOWNLOADING,
    WAR_SCENE_DOWNLOAD_DOWNLOADED,
    WAR_SCENE_DOWNLOAD_FILE_LOADED,
    WAR_SCENE_DOWNLOAD_FAILED,
};

struct _WarScene
{
    WarSceneType type;
    WarEntityManager entityManager;

    WarCheatStatus cheatStatus;

    union
    {
        struct
        {
            WarSceneDownloadState status;
        } download;

        struct
        {
            f32 time;
        } blizzard;

        struct
        {
            WarRace yourRace;
            WarRace enemyRace;
            s32 customMap;
        } menu;

        struct
        {
            f32 time;
            WarRace race;
            WarCampaignMapType mapType;
        } briefing;
    };
};

typedef void (*WarSceneFunc)(WarContext* context);

typedef struct
{
    WarSceneType type;
    WarSceneFunc enterSceneFunc;
    WarSceneFunc leaveSceneFunc;
    WarSceneFunc updateSceneFunc;
} WarSceneDescriptor;

WarScene* wsc_createScene(WarContext* context, WarSceneType type);
void wsc_freeScene(WarScene* scene);

void wsc_enterScene(WarContext* context);
void wsc_updateScene(WarContext* context);
void wsc_renderScene(WarContext* context);
void wsc_leaveScene(WarContext* context);
