#pragma once

typedef struct
{
    char name[20];
    WarAIInitFunc initFunc;
    WarAINextCommandFunc getCommandFunc;
} WarAIData;

WarAIData getAIData(const char* name);

WarAI* createAI(WarContext* context, const char* name);

void initAIPlayers(WarContext* context);
void updateAIPlayers(WarContext* context);
