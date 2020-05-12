#pragma once

typedef struct
{
    char name[20];
    WarAIInitFunc initFunc;
    WarAIGetCommandFunc getCommandFunc;
    WarAIExecutedCommandFunc executedCommandFunc;
} WarAIData;

const WarAIData aiData[] =
{
    { "land-attack", landAttackInitAI, landAttackGetAICommand, landAttackExecutedAICommand }
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

WarAICommand* createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type);
WarAICommand* createRequestAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool waitForIdleWorker);
WarAICommand* createResourceAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resourceKind, s32 count, bool waitForIdleWorker);
WarAICommand* createSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, f32 time);

WarAICommandResult* createAICommandResult(WarContext* context, WarAICommand* command, bool completed);
WarAICommandResult* createRequestAICommandResult(WarContext* context, WarAICommand* command, bool completed, WarEntityId buildingId, WarEntityId workerId);
WarAICommandResult* createResourceAICommandResult(WarContext* context, WarAICommand* command, bool completed, s32 count, WarEntityId workerIds[]);

void initAIPlayers(WarContext* context);
void updateAIPlayers(WarContext* context);
