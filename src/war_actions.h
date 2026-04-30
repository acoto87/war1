#pragma once

#include "war_fwd.h"
#include "war_math.h"

enum _WarUnitActionStepType
{
    WAR_ACTION_STEP_NONE,
    WAR_ACTION_STEP_UNBREAKABLE,
    WAR_ACTION_STEP_FRAME,
    WAR_ACTION_STEP_WAIT,
    WAR_ACTION_STEP_MOVE,
    WAR_ACTION_STEP_ATTACK,
    WAR_ACTION_STEP_SOUND_SWORD,
    WAR_ACTION_STEP_SOUND_FIST,
    WAR_ACTION_STEP_SOUND_FIREBALL,
    WAR_ACTION_STEP_SOUND_CHOPPING,
    WAR_ACTION_STEP_SOUND_CATAPULT,
    WAR_ACTION_STEP_SOUND_ARROW,
    WAR_ACTION_STEP_SOUND_LIGHTNING,
};

enum _WarUnbreakableParam
{
    WAR_UNBREAKABLE_BEGIN,
    WAR_UNBREAKABLE_END,
};

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

enum _WarUnitActionType
{
    WAR_ACTION_TYPE_NONE = -1,

    WAR_ACTION_TYPE_IDLE,
    WAR_ACTION_TYPE_WALK,
    WAR_ACTION_TYPE_ATTACK,
    WAR_ACTION_TYPE_DEATH,
    WAR_ACTION_TYPE_HARVEST,
    WAR_ACTION_TYPE_REPAIR,
    WAR_ACTION_TYPE_BUILD,

    WAR_ACTION_TYPE_COUNT
};

enum _WarUnitActionStatus
{
    WAR_ACTION_NOT_STARTED,
    WAR_ACTION_RUNNING,
    WAR_ACTION_FINISHED,
};

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
