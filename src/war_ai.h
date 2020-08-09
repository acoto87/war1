#pragma once

typedef struct
{
    char name[20];
    WarAIInitFunc initFunc;
    WarAINextCommandFunc getCommandFunc;
} WarAIData;

const WarAIData aiData[] =
{
    { "level", levelInitAI, levelGetAICommand }
};

WarAIData getAIData(const char* name)
{
    s32 index = 0;
    s32 length = arrayLength(aiData);
    while (index < length && !strEquals(aiData[index].name, name))
        index++;

    assert(index < length);
    return aiData[index];
}

WarAI* createAI(WarContext* context, const char* name);

void initAIPlayers(WarContext* context);
void updateAIPlayers(WarContext* context);
