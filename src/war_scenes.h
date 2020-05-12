#pragma once

typedef void (*WarSceneFunc)(WarContext* context);

typedef struct
{
    WarSceneType type;
    WarSceneFunc enterSceneFunc;
    WarSceneFunc leaveSceneFunc;
    WarSceneFunc updateSceneFunc;
} WarSceneDescriptor;

WarScene* createScene(WarContext* context, WarSceneType type);
void freeScene(WarScene* scene);

void enterScene(WarContext* context);
void updateScene(WarContext* context);
void renderScene(WarContext* context);
void leaveScene(WarContext* context);
