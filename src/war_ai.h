#pragma once

#include "shl/queue.h"

#include "war_units.h"

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

bool wai_equalsAICommand(const WarAICommand* command1, const WarAICommand* command2);
void wai_freeAICommand(WarAICommand* command);

shlDeclareQueue(WarAICommandQueue, WarAICommand*)
shlDeclareList(WarAICommandList, WarAICommand*)

#define WarAICommandListDefaultOptions ((WarAICommandListOptions){NULL, wai_equalsAICommand, wai_freeAICommand})
#define WarAICommandQueueDefaultOptions ((WarAICommandQueueOptions){NULL, wai_equalsAICommand, wai_freeAICommand})

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
