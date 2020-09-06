#pragma once

typedef enum
{
    WP_RESOURCE_TYPE_GOLD,
    WP_RESOURCE_TYPE_WOOD,
    WP_RESOURCE_TYPE_SUPPLY,
    WP_RESOURCE_TYPE_TOWNHALL,
    WP_RESOURCE_TYPE_BARRACKS,
    WP_RESOURCE_TYPE_CHURCH_TEMPLE,
    WP_RESOURCE_TYPE_TOWER,
    WP_RESOURCE_TYPE_STABLE_KENNEL,
    WP_RESOURCE_TYPE_LUMBERMILL,
    WP_RESOURCE_TYPE_BLACKSMITH,
    WP_RESOURCE_TYPE_PEASANT_PEON,
    WP_RESOURCE_TYPE_FOOTMAN_GRUNT,
    WP_RESOURCE_TYPE_ARCHER_SPEARMAN,
    WP_RESOURCE_TYPE_KNIGHT_RAIDER,
    WP_RESOURCE_TYPE_CATAPULT,
    WP_RESOURCE_TYPE_CONJURER_WARLOCK,
    WP_RESOURCE_TYPE_CLERIC_NECROLYTE,

    WP_RESOURCE_TYPE_COUNT
} WPResourceType;

typedef struct
{
    u32 time;
    u32 resources[WP_RESOURCE_TYPE_COUNT];
} WPGameState;

typedef struct
{
    u32 resources[WP_RESOURCE_TYPE_COUNT];
} WPGoal;

typedef enum
{
    WP_ACTION_TYPE_COLLECT_GOLD,
    WP_ACTION_TYPE_COLLECT_WOOD,
    WP_ACTION_TYPE_BUILD_SUPPLY,
    WP_ACTION_TYPE_BUILD_TOWNHALL,
    WP_ACTION_TYPE_BUILD_BARRACKS,
    WP_ACTION_TYPE_BUILD_CHURCH_TEMPLE,
    WP_ACTION_TYPE_BUILD_TOWER,
    WP_ACTION_TYPE_BUILD_STABLE_KENNEL,
    WP_ACTION_TYPE_BUILD_LUMBERMILL,
    WP_ACTION_TYPE_BUILD_BLACKSMITH,
    WP_ACTION_TYPE_TRAIN_PEASANT_PEON,
    WP_ACTION_TYPE_TRAIN_FOOTMAN_GRUNT,
    WP_ACTION_TYPE_TRAIN_ARCHER_SPEARMAN,
    WP_ACTION_TYPE_TRAIN_KNIGHT_RAIDER,
    WP_ACTION_TYPE_TRAIN_CATAPULT,
    WP_ACTION_TYPE_TRAIN_CONJURER_WARLOCK,
    WP_ACTION_TYPE_TRAIN_CLERIC_NECROLYTE,

    WP_ACTION_TYPE_COUNT
} WPActionType;

typedef struct
{
    s32 id;
    WPActionType type;
    u32 start;
    u32 end;
    u32 duration;
} WPAction;

typedef struct
{
    WPActionType type;
    u32 duration;
    u32 require[WP_RESOURCE_TYPE_COUNT];
    u32 borrow[WP_RESOURCE_TYPE_COUNT];
    u32 consume[WP_RESOURCE_TYPE_COUNT];
    u32 produce[WP_RESOURCE_TYPE_COUNT];
} WPActionMetadata;

#define WP_INVALID_ACTION (WPAction){WP_ACTION_TYPE_COUNT, 0, 0, 0}
#define WP_INVALID_ACTION_METADATA (WPActionMetadata){WP_ACTION_TYPE_COUNT, 0}

bool equalsWPAction(const WPAction a, const WPAction b)
{
    return a.id == b.id;
}

shlDeclareList(WPPlan, WPAction)
shlDefineList(WPPlan, WPAction)

#define WPPlanDefaultOptions (WPPlanOptions){WP_INVALID_ACTION, equalsWPAction, NULL}

#define WP_INVALID_PLAN (WPPlan){0};

WPActionMetadata wpGetActionMetadataThatProduces(WPResourceType type);

WPGameState wpCreateGameState(u32 time, const u32 resources[]);
WPGameState wpCreateGameStateFromArgs(u32 time, s32 resourcesCount, ...);
WPGoal wpCreateGoal(const u32 resources[]);
WPGoal wpCreateGoalFromArgs(s32 resourcesCount, ...);
WPGoal wpCreateGoalFromAction(const WPActionMetadata* action, u32 repeat);
WPAction createAction(WPActionType type, u32 start);
void wpUpdateAction(WPAction* action, u32 start);

u32 wpGetPlanDuration(const WPPlan* plan);

bool wpCanExecuteAction(const WPGameState* gameState, const WPAction* action);
void wpStartAction(WPGameState* gameState, const WPAction* action);
void wpEndAction(WPGameState* gameState, const WPAction* action);
bool wpSatisfyGoal(const WPGameState* gameState, const WPGoal* goal);
void wpExecutePlan(WPGameState* gameState, const WPPlan* plan, s32 index, u32 endTime);
void wpSchedulePlan(WPPlan* plan, WPGameState gameState);
void wpMEA(WPGameState* gameState, const WPGoal* goal, WPPlan* plan);
void wpBestPlan(WPGameState* gameState, const WPGoal* goal, WPPlan* plan);

char* resourceTypeToString(WPResourceType resourceType);
char* actionTypeToString(WPActionType actionType);
void printGameState(const WPGameState* gameState);
void printGoal(const WPGoal* goal);
void printAction(const WPAction* action);
void printPlan(const WPPlan* plan);
