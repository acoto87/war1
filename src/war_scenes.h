#pragma once

#include "war_campaigns.h"
#include "war_cheats.h"
#include "war_entities.h"

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

struct _WarSceneDescriptor
{
    WarSceneType type;
    WarSceneFunc enterSceneFunc;
    WarSceneFunc leaveSceneFunc;
    WarSceneFunc updateSceneFunc;
};

WarScene* wsc_createScene(WarContext* context, WarSceneType type);
void wsc_freeScene(WarScene* scene);

void wsc_enterScene(WarContext* context);
void wsc_updateScene(WarContext* context);
void wsc_renderScene(WarContext* context);
void wsc_leaveScene(WarContext* context);
