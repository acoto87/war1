#pragma once

#include "war_types.h"

WarAI* wai_createAI(WarContext* context);
WarAICommand* wai_createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type);

WarAICommand* wai_createUnitRequest(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count);
WarAICommand* wai_createWaitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count);
WarAICommand* wai_createSleepForTime(WarContext* context, WarPlayerInfo* aiPlayer, f32 time);

void wai_initAIPlayers(WarContext* context);
void wai_updateAIPlayers(WarContext* context);
