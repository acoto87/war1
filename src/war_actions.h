#pragma once

#include "shl/list.h"

#include "war_fwd.h"
#include "war_math.h"

struct _WarUnitActionStep
{
    WarUnitActionStepType type;
    s32 param;
};

#define WarUnitActionStepEmpty (WarUnitActionStep){WAR_ACTION_STEP_NONE}

bool wact_equalsActionStep(const WarUnitActionStep step1, const WarUnitActionStep step2);

shlDeclareList(WarUnitActionStepList, WarUnitActionStep)

#define WarUnitActionStepListDefaultOptions (WarUnitActionStepListOptions){WarUnitActionStepEmpty, wact_equalsActionStep, NULL}

struct _WarUnitActionDef
{
    WarUnitActionType type;
    bool directional;
    bool loop;
    WarUnitActionStepList steps;
};

struct _WarUnitAction
{
    WarUnitActionStatus status;
    f32 scale;
    f32 waitCount;
    s32 stepIndex;
    WarUnitActionStepType lastActionStep;
    WarUnitActionStepType lastSoundStep;
};

void wact_initUnitActionDefs(void);

void wact_addUnitActions(WarEntity* entity);
s32 wact_getActionDuration(WarEntity* entity, WarUnitActionType type);

void wact_setAction(WarContext* context, WarEntity* entity, WarUnitActionType type, bool reset, f32 scale);
void wact_updateAction(WarContext* context, WarEntity* entity);
