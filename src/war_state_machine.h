#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "war_math.h"
#include "war_units.h"
#include "war_rvo.h"
#include "war_pathfinder.h"
#include "war_collections.h"

#define INVALID_STATE_TYPE  ((WarStateType)(-1))
#define INVALID_STATE_IDX   (-1)

struct _WarStateRef
{
    WarStateType type;
    s32          idx;
};

#define WAR_STATE_REF_INVALID  ((WarStateRef){ INVALID_STATE_TYPE, INVALID_STATE_IDX })
#define WAR_STATE_REF_IS_VALID(r) ((r).type != INVALID_STATE_TYPE)

struct _WarStateBase
{
    WarStateType type;
    WarEntityId  entityId;
    f64          nextUpdateGameTime;
    f32          delay;
    WarStateRef  nextRef;
};

struct _WarStateIdle
{
    WarStateBase base; bool lookAround;
};

struct _WarStateMove
{
    WarStateBase base;
    s32  waypointsIndex;
    s32  waypointsCount;
    vec2 waypoints[64];

    bool checkForAttacks;

    vec2 rvoVelocity;       // last frame's RVO output, pixels/sec
    f32  settleTimer;        // seconds without progress toward final goal
    f32  closestGoalDistSq;  // best squared distance to final goal achieved

    vec2 rvoPreferredVelocity;
    vec2 rvoAdjustedVelocity;
    vec2 rvoPosition;
    f32  rvoRadius;
    s32  rvoNumCandidates;
    s32  rvoBestIndex;
    vec2 rvoCandidates[RVO_MAX_CANDIDATES];
    bool rvoCandidateHadCollision[RVO_MAX_CANDIDATES];
};

struct _WarStatePatrol
{
    WarStateBase base;
    s32      waypointsIndex;
    Vec2List positions;
    s32      dir;
};

struct _WarStateFollow
{
    WarStateBase base;
    // the follow state can follow an entity or a point
    s32  targetEntityId;
    vec2 targetTile;
    // the range distance (in tiles) in which the follower stops
    s32  distance;
};

struct _WarStateAttack
{
    WarStateBase base;
    s32  targetEntityId;
    vec2 targetTile;
};

struct _WarStateGold
{
    WarStateBase base;
    s32 goldmineId;
};

struct _WarStateMining
{
    WarStateBase base;
    s32 goldmineId; f32 miningTime;
};

struct _WarStateWood
{
    WarStateBase base;
    s32 forestId; vec2 position;
};

struct _WarStateChopping
{
    WarStateBase base;
    s32 forestId; vec2 position;
};

struct _WarStateDeliver
{
    WarStateBase base;
    s32 townHallId; bool insideBuilding;
};

struct _WarStateDeath
{
    WarStateBase base;
};

struct _WarStateCollapse
{
    WarStateBase base;
};

struct _WarStateTrain
{
    WarStateBase base;
    WarUnitType unitToBuild;
    f32 buildTime;
    f32 totalBuildTime;
    bool cancelled;
};

struct _WarStateUpgrade
{
    WarStateBase base;
    WarUpgradeType upgradeToBuild;
    f32 buildTime;
    f32 totalBuildTime;
    bool cancelled;
};

struct _WarStateBuild
{
    WarStateBase base;
    WarEntityId workerId;
    f32 buildTime;
    f32 totalBuildTime;
    bool cancelled;
};

struct _WarStateRepair
{
    WarStateBase base;
    WarEntityId buildingId;
};

struct _WarStateRepairing
{
    WarStateBase base;
    WarEntityId buildingId;
    bool insideBuilding;
};

struct _WarStateCast
{
    WarStateBase base;
    WarSpellType spellType;
    WarEntityId  targetEntityId;
    vec2         targetTile;
};

struct _WarStateWait
{
    WarStateBase base;
    f64 waitEndGameTime;
};

static_assert(offsetof(WarStateIdle,      base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateMove,      base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStatePatrol,    base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateFollow,    base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateAttack,    base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateGold,      base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateMining,    base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateWood,      base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateChopping,  base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateDeliver,   base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateDeath,     base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateCollapse,  base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateTrain,     base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateUpgrade,   base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateBuild,     base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateRepair,    base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateRepairing, base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateCast,      base) == 0, "WarStateBase must be the first member");
static_assert(offsetof(WarStateWait,      base) == 0, "WarStateBase must be the first member");

typedef struct
{
    WarStateType type;
    void (*enterFunc)(WarContext* context, WarEntity* entity, WarStateBase* state);
    void (*leaveFunc)(WarContext* context, WarEntity* entity, WarStateBase* state);
    void (*updateFunc)(WarContext* context, WarEntity* entity, WarStateBase* state);
    void (*freeFunc)(WarContext* context, WarStateBase* state);
} WarStateDescriptor;

WarStateRef    wst_allocState(WarContext* context, WarStateType type, WarEntityId entityId);
void           wst_freeStateRef(WarContext* context, WarStateRef ref);
WarStateBase*  wst_deref(WarContext* context, WarStateRef ref);
WarStateRef    wst_refOf(WarContext* context, const WarStateBase* state);
void           wst_chainNext(WarContext* context, WarStateBase* from, WarStateBase* to);

WarStateIdle*      wst_createIdleState(WarContext* context, WarEntity* entity, bool lookAround);
WarStateMove*      wst_createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[]);
WarStatePatrol*    wst_createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[]);
WarStateFollow*    wst_createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile, s32 distance);
WarStateAttack*    wst_createAttackState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile);
WarStateDeath*     wst_createDeathState(WarContext* context, WarEntity* entity);
WarStateCollapse*  wst_createCollapseState(WarContext* context, WarEntity* entity);
WarStateWait*      wst_createWaitState(WarContext* context, WarEntity* entity, f32 waitTime);
WarStateGold*      wst_createGatherGoldState(WarContext* context, WarEntity* entity, WarEntityId goldmineId);
WarStateMining*    wst_createMiningState(WarContext* context, WarEntity* entity, WarEntityId goldmineId);
WarStateWood*      wst_createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 position);
WarStateChopping*  wst_createChoppingState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position);
WarStateDeliver*   wst_createDeliverState(WarContext* context, WarEntity* entity, WarEntityId townHallId);
WarStateTrain*     wst_createTrainState(WarContext* context, WarEntity* entity, WarUnitType unitToBuild, f32 buildTime);
WarStateUpgrade*   wst_createUpgradeState(WarContext* context, WarEntity* entity, WarUpgradeType upgradeToBuild, f32 buildTime);
WarStateBuild*     wst_createBuildState(WarContext* context, WarEntity* entity, f32 buildTime);
WarStateRepair*    wst_createRepairState(WarContext* context, WarEntity* entity, WarEntityId buildingId);
WarStateRepairing* wst_createRepairingState(WarContext* context, WarEntity* entity, WarEntityId buildingId);
WarStateCast*      wst_createCastState(WarContext* context, WarEntity* entity, WarSpellType spellType, WarEntityId targetEntityId, vec2 targetTile);

void wst_changeNextState(WarContext* context, WarEntity* entity, WarStateBase* state, bool leaveState, bool enterState);
bool wst_changeStateNextState(WarContext* context, WarEntity* entity, WarStateBase* state);

WarStateBase*   wst_getState(WarContext* context, WarEntity* entity, WarStateType type);
WarStateBase*   wst_getDirectState(WarContext* context, WarEntity* entity, WarStateType type);
WarStateBase*   wst_getNextState(WarContext* context, WarEntity* entity, WarStateType type);

WarStateIdle*      wst_getIdleState(WarContext* context, WarEntity* entity);
WarStateMove*      wst_getMoveState(WarContext* context, WarEntity* entity);
WarStatePatrol*    wst_getPatrolState(WarContext* context, WarEntity* entity);
WarStateFollow*    wst_getFollowState(WarContext* context, WarEntity* entity);
WarStateAttack*    wst_getAttackState(WarContext* context, WarEntity* entity);
WarStateDeath*     wst_getDeathState(WarContext* context, WarEntity* entity);
WarStateCollapse*  wst_getCollapseState(WarContext* context, WarEntity* entity);
WarStateGold*      wst_getGatherGoldState(WarContext* context, WarEntity* entity);
WarStateMining*    wst_getMiningState(WarContext* context, WarEntity* entity);
WarStateWood*      wst_getGatherWoodState(WarContext* context, WarEntity* entity);
WarStateChopping*  wst_getChoppingState(WarContext* context, WarEntity* entity);
WarStateDeliver*   wst_getDeliverState(WarContext* context, WarEntity* entity);
WarStateTrain*     wst_getTrainState(WarContext* context, WarEntity* entity);
WarStateUpgrade*   wst_getUpgradeState(WarContext* context, WarEntity* entity);
WarStateBuild*     wst_getBuildState(WarContext* context, WarEntity* entity);
WarStateRepair*    wst_getRepairState(WarContext* context, WarEntity* entity);
WarStateRepairing* wst_getRepairingState(WarContext* context, WarEntity* entity);
WarStateCast*      wst_getCastState(WarContext* context, WarEntity* entity);

bool wst_hasState(WarContext* context, WarEntity* entity, WarStateType type);
bool wst_hasDirectState(WarContext* context, WarEntity* entity, WarStateType type);
bool wst_hasNextState(WarContext* context, WarEntity* entity, WarStateType type);

bool wst_isIdle(WarContext* context, WarEntity* entity);
bool wst_isMoving(WarContext* context, WarEntity* entity);
bool wst_isPatrolling(WarContext* context, WarEntity* entity);
bool wst_isFollowing(WarContext* context, WarEntity* entity);
bool wst_isAttacking(WarContext* context, WarEntity* entity);
bool wst_isDead(WarContext* context, WarEntity* entity);
bool wst_isCollapsing(WarContext* context, WarEntity* entity);
bool wst_isGatheringGold(WarContext* context, WarEntity* entity);
bool wst_isMining(WarContext* context, WarEntity* entity);
bool wst_isGatheringWood(WarContext* context, WarEntity* entity);
bool wst_isChopping(WarContext* context, WarEntity* entity);
bool wst_isDelivering(WarContext* context, WarEntity* entity);
bool wst_isTraining(WarContext* context, WarEntity* entity);
bool wst_isUpgrading(WarContext* context, WarEntity* entity);
bool wst_isBuilding(WarContext* context, WarEntity* entity);
bool wst_isRepairing(WarContext* context, WarEntity* entity);
bool wst_isRepairing2(WarContext* context, WarEntity* entity);
bool wst_isCasting(WarContext* context, WarEntity* entity);

bool wst_isGoingToIdle(WarContext* context, WarEntity* entity);
bool wst_isGoingToMove(WarContext* context, WarEntity* entity);
bool wst_isGoingToPatrol(WarContext* context, WarEntity* entity);
bool wst_isGoingToFollow(WarContext* context, WarEntity* entity);
bool wst_isGoingToAttack(WarContext* context, WarEntity* entity);
bool wst_isGoingToDie(WarContext* context, WarEntity* entity);
bool wst_isGoingToCollapse(WarContext* context, WarEntity* entity);
bool wst_isGoingToGatherGold(WarContext* context, WarEntity* entity);
bool wst_isGoingToMine(WarContext* context, WarEntity* entity);
bool wst_isGoingToGatherWood(WarContext* context, WarEntity* entity);
bool wst_isGoingToChop(WarContext* context, WarEntity* entity);
bool wst_isGoingToDeliver(WarContext* context, WarEntity* entity);
bool wst_isGoingToTrain(WarContext* context, WarEntity* entity);
bool wst_isGoingToUpgrade(WarContext* context, WarEntity* entity);
bool wst_isGoingToBuild(WarContext* context, WarEntity* entity);
bool wst_isGoingToRepair(WarContext* context, WarEntity* entity);
bool wst_isGoingToRepair2(WarContext* context, WarEntity* entity);
bool wst_isGoingToCast(WarContext* context, WarEntity* entity);

bool wst_isInsideBuilding(WarContext* context, WarEntity* entity);

void wst_enterIdleState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterMoveState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterPatrolState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterFollowState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterAttackState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterGatherGoldState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterMiningState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterGatherWoodState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterChoppingState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterDeliverState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterDeathState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterCollapseState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterTrainState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterUpgradeState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterBuildState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterRepairState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterRepairingState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterCastState(WarContext* context, WarEntity* entity, WarState* state);
void wst_enterWaitState(WarContext* context, WarEntity* entity, WarState* state);

void wst_leaveIdleState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveMoveState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leavePatrolState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveFollowState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveAttackState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveGatherGoldState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveMiningState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveGatherWoodState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveChoppingState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveDeliverState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveDeathState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveCollapseState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveTrainState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveUpgradeState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveBuildState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveRepairState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveRepairingState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveCastState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveWaitState(WarContext* context, WarEntity* entity, WarState* state);

void wst_updateIdleState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateMoveState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updatePatrolState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateFollowState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateAttackState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateGatherGoldState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateMiningState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateGatherWoodState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateChoppingState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateDeliverState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateDeathState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateCollapseState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateTrainState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateUpgradeState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateBuildState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateRepairState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateRepairingState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateCastState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateWaitState(WarContext* context, WarEntity* entity, WarState* state);

void wst_freeIdleState(WarContext* context, WarState* state);
void wst_freeMoveState(WarContext* context, WarState* state);
void wst_freePatrolState(WarContext* context, WarState* state);
void wst_freeFollowState(WarContext* context, WarState* state);
void wst_freeAttackState(WarContext* context, WarState* state);
void wst_freeGatherGoldState(WarContext* context, WarState* state);
void wst_freeMiningState(WarContext* context, WarState* state);
void wst_freeGatherWoodState(WarContext* context, WarState* state);
void wst_freeChoppingState(WarContext* context, WarState* state);
void wst_freeDeliverState(WarContext* context, WarState* state);
void wst_freeDeathState(WarContext* context, WarState* state);
void wst_freeCollapseState(WarContext* context, WarState* state);
void wst_freeTrainState(WarContext* context, WarState* state);
void wst_freeUpgradeState(WarContext* context, WarState* state);
void wst_freeBuildState(WarContext* context, WarState* state);
void wst_freeRepairState(WarContext* context, WarState* state);
void wst_freeRepairingState(WarContext* context, WarState* state);
void wst_freeCastState(WarContext* context, WarState* state);
void wst_freeWaitState(WarContext* context, WarState* state);

void wst_enterState(WarContext* context, WarEntity* entity, WarStateBase* state);
void wst_leaveState(WarContext* context, WarEntity* entity, WarStateBase* state);
void wst_updateStateMachine(WarContext* context, WarEntity* entity);
void wst_freeState(WarContext* context, WarStateBase* state);

void wst_initStorage(WarStateStorage* s);
