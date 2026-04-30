#pragma once

#include "war_fwd.h"
#include "war_math.h"

struct _WarUnitActionStep
{
    WarUnitActionStepType type;
    s32 param;
};

#define WarUnitActionStepEmpty (WarUnitActionStep){WAR_ACTION_STEP_NONE}

bool equalsActionStep(const WarUnitActionStep step1, const WarUnitActionStep step2)
{
    return step1.type == step2.type && step1.param == step2.param;
}

shlDeclareList(WarUnitActionStepList, WarUnitActionStep)
shlDefineList(WarUnitActionStepList, WarUnitActionStep)

#define WarUnitActionStepListDefaultOptions (WarUnitActionStepListOptions){WarUnitActionStepEmpty, equalsActionStep, NULL}

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
    bool unbreakable;
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
