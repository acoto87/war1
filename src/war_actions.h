#pragma once

#include "war_types.h"

void wact_initUnitActionDefs(void);

void wact_addUnitActions(WarEntity* entity);

s32 wact_getActionDuration(WarEntity* entity, WarUnitActionType type);
void wact_setAction(WarContext* context, WarEntity* entity, WarUnitActionType type, bool reset, f32 scale);
void wact_updateAction(WarContext* context, WarEntity* entity);
