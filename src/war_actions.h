#pragma once

#include "war_types.h"

s32 getActionDuration(WarEntity* entity, WarUnitActionType type);
void setAction(WarContext* context, WarEntity* entity, WarUnitActionType type, bool reset, f32 scale);
void updateAction(WarContext* context, WarEntity* entity);
