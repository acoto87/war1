#pragma once

#include "shl/queue.h"

#include "war_units.h"

enum _WarAICommandStatus
{
    WAR_AI_COMMAND_STATUS_CREATED,
    WAR_AI_COMMAND_STATUS_STARTED,
    WAR_AI_COMMAND_STATUS_COMPLETED,
};

enum _WarAICommandType
{
    WAR_AI_COMMAND_REQUEST,
    WAR_AI_COMMAND_WAIT,
    WAR_AI_COMMAND_ATTACK,
    WAR_AI_COMMAND_DEFEND,
    WAR_AI_COMMAND_SLEEP,

    WAR_AI_COMMAND_COUNT
};

struct _WarAICommand
{
    u32 id;
    WarAICommandType type;
    WarAICommandStatus status;

    union
    {
        struct
        {
            WarUnitType unitType;
            s32 count;
        } request;

        struct
        {
            WarUnitType unitType;
            s32 count;
        } wait;

        struct
        {
            f32 time;
        } sleep;
    };
};

shlDeclareQueue(WarAICommandQueue, WarAICommand*)
shlDefineQueue(WarAICommandQueue, WarAICommand*)

shlDeclareList(WarAICommandList, WarAICommand*)
shlDefineList(WarAICommandList, WarAICommand*)

bool aiCommandEquals(const WarAICommand* command1, const WarAICommand* command2)
{
    return command1->id == command2->id;
}

static inline void aiCommandFree(WarAICommand* command)
{
    wm_free((void*)command);
}

#define WarAICommandListDefaultOptions ((WarAICommandListOptions){NULL, aiCommandEquals, aiCommandFree})
#define WarAICommandQueueDefaultOptions ((WarAICommandQueueOptions){NULL, aiCommandEquals, aiCommandFree})

struct _WarAI
{
    u32 staticCommandId;
    WarAICommandList currentCommands;
    WarAICommandQueue nextCommands;
    void* customData;
};

WarAI* wai_createAI(WarContext* context);
WarAICommand* wai_createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type);

WarAICommand* wai_createUnitRequest(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count);
WarAICommand* wai_createWaitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count);
WarAICommand* wai_createSleepForTime(WarContext* context, WarPlayerInfo* aiPlayer, f32 time);

void wai_initAIPlayers(WarContext* context);
void wai_updateAIPlayers(WarContext* context);
