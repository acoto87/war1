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
    { "level", levelInitAI, levelGetAICommand, levelExecutedAICommand }
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
WarAICommand* createRequestAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool checkExisting, bool waitForIdleWorker);
WarAICommand* createResourceAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resourceKind, s32 count, bool waitForIdleWorker);
WarAICommand* createWaitAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandId commandId, WarResourceKind resource, s32 amount);
WarAICommand* createSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, f32 time);

#define createWaitForCommand(context, aiPlayer, commandId) createWaitAICommand(context, aiPlayer, commandId, WAR_RESOURCE_NONE, 0)
#define createWaitForResource(context, aiPlayer, resource, amount) createWaitAICommand(context, aiPlayer, 0, resource, amount)

WarAICommandResult* createAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status);
WarAICommandResult* createRequestAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status, WarEntityId buildingId, WarEntityId workerId);
WarAICommandResult* createResourceAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status, s32 count, WarEntityId workerIds[]);
WarAICommandResult* createSleepAICommandResult(WarContext* context, WarAICommand* command, WarAICommandStatus status);

void initAIPlayers(WarContext* context);
void updateAIPlayers(WarContext* context);
