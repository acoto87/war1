#pragma once

typedef enum
{
    WAR_AI_COMMAND_FLAG_NONE = 0,
    WAR_AI_COMMAND_FLAG_WAIT = 1
} WarAICommandFlags;

uint32_t hashAICommandId(const WarAICommandId id)
{
    return id;
}

bool equalsAICommand(const WarAICommandId id1, const WarAICommandId id2)
{
    return id1 == id2;
}

shlDeclareMap(WarAICommandFlagsMap, WarAICommandId, WarAICommandFlags)
shlDefineMap(WarAICommandFlagsMap, WarAICommandId, WarAICommandFlags)

#define WarAICommandFlagsMapDefaultOptions (WarAICommandFlagsMapOptions){WAR_AI_COMMAND_FLAG_NONE, hashAICommandId, equalsAICommand, NULL}

void freeAICommandResult(WarAICommandResult* commandResult)
{
    free((void*)commandResult);
}

shlDeclareMap(WarAICommandResultsMap, WarAICommandId, WarAICommandResult*)
shlDefineMap(WarAICommandResultsMap, WarAICommandId, WarAICommandResult*)

#define WarAICommandResultsMapDefaultOptions (WarAICommandResultsMapOptions){NULL, hashAICommandId, equalsAICommand, freeAICommandResult}

typedef struct
{
    s32 index;
    WarAICommandList commands;
    WarAICommandFlagsMap commandFlags;
    WarAICommandResultsMap commandResults;
} WarAICustomData;

void landAttackInitAI(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* landAttackGetAICommand(WarContext* context, WarPlayerInfo* aiPlayer);
void landAttackExecutedAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command, WarAICommandResult* result);