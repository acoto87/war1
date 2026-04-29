#pragma once

#include "war_types.h"

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
