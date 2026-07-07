#pragma once

#include <stdbool.h>

#include "war_math.h"
#include "war_units.h"
#include "war_rvo.h"
#include "war_pathfinder.h"
#include "war_collections.h"

struct _WarState
{
    WarStateType type;
    s32 entityId;
    f64 nextUpdateGameTime;
    f32 delay;
    struct _WarState* nextState;

    union
    {
        struct
        {
            bool lookAround;
        } idle;

        struct
        {
            s32 waypointsIndex;
            s32 waypointsCount;
            vec2 waypoints[64];

            bool checkForAttacks;

            vec2 rvoVelocity;       // last frame's RVO output, pixels/sec
            f32 settleTimer;        // seconds without progress toward final goal
            f32 closestGoalDistSq;  // best squared distance to final goal achieved

            vec2 rvoPreferredVelocity;
            vec2 rvoAdjustedVelocity;
            vec2 rvoPosition;
            f32  rvoRadius;
            s32  rvoNumCandidates;
            s32  rvoBestIndex;
            vec2 rvoCandidates[RVO_MAX_CANDIDATES];
            bool rvoCandidateHadCollision[RVO_MAX_CANDIDATES];
        } move;

        struct
        {
            s32 waypointsIndex;
            Vec2List positions;
            s32 dir;
        } patrol;

        struct
        {
            // the follow state can follow an entity or a point
            s32 targetEntityId;
            vec2 targetTile;

            // the range distance (in tiles) in which the follower stops
            s32 distance;
        } follow;

        struct
        {
            f64 waitEndGameTime;
        } wait;

        struct
        {
            s32 targetEntityId;
            vec2 targetTile;
        } attack;

        struct
        {
            s32 goldmineId;
        } gold;

        struct
        {
            s32 goldmineId;
            f32 miningTime;
        } mine;

        struct
        {
            s32 forestId;
            vec2 position;
        } wood;

        struct
        {
            s32 forestId;
            vec2 position;
        } chop;

        struct
        {
            s32 townHallId;
            bool insideBuilding;
        } deliver;

        struct
        {
            WarUnitType unitToBuild;
            f32 buildTime;
            f32 totalBuildTime;
            bool cancelled;
        } train;

        struct
        {
            WarUpgradeType upgradeToBuild;
            f32 buildTime;
            f32 totalBuildTime;
            bool cancelled;
        } upgrade;

        struct
        {
            WarEntityId workerId;
            f32 buildTime;
            f32 totalBuildTime;
            bool cancelled;
        } build;

        struct
        {
            WarEntityId buildingId;
        } repair;

        struct
        {
            WarEntityId buildingId;
            bool insideBuilding;
        } repairing;

        struct
        {
            WarSpellType spellType;
            WarEntityId targetEntityId;
            vec2 targetTile;
        } cast;
    };
};

typedef struct
{
    WarStateType type;
    void (*enterStateFunc)(WarContext* context, WarEntity* entity, WarState* state);
    void (*leaveStateFunc)(WarContext* context, WarEntity* entity, WarState* state);
    void (*updateStateFunc)(WarContext* context, WarEntity* entity, WarState* state);
    void (*freeStateFunc)(WarContext* context, WarState* state);
} WarStateDescriptor;

WarState* wst_createState(WarContext* context, WarEntity* entity, WarStateType type);
WarState* wst_createIdleState(WarContext* context, WarEntity* entity, bool lookAround);
WarState* wst_createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[]);
WarState* wst_createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[]);
WarState* wst_createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile, s32 distance);
WarState* wst_createAttackState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile);
WarState* wst_createDeathState(WarContext* context, WarEntity* entity);
WarState* wst_createCollapseState(WarContext* context, WarEntity* entity);
WarState* wst_createWaitState(WarContext* context, WarEntity* entity, f32 waitTime);
WarState* wst_createGatherGoldState(WarContext* context, WarEntity* entity, WarEntityId goldmineId);
WarState* wst_createMiningState(WarContext* context, WarEntity* entity, WarEntityId goldmineId);
WarState* wst_createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 position);
WarState* wst_createChoppingState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position);
WarState* wst_createDeliverState(WarContext* context, WarEntity* entity, WarEntityId townHallId);
WarState* wst_createTrainState(WarContext* context, WarEntity* entity, WarUnitType unitToBuild, f32 buildTime);
WarState* wst_createUpgradeState(WarContext* context, WarEntity* entity, WarUpgradeType upgradeToBuild, f32 buildTime);
WarState* wst_createBuildState(WarContext* context, WarEntity* entity, f32 buildTime);
WarState* wst_createRepairState(WarContext* context, WarEntity* entity, WarEntityId buildingId);
WarState* wst_createRepairingState(WarContext* context, WarEntity* entity, WarEntityId buildingId);
WarState* wst_createCastState(WarContext* context, WarEntity* entity, WarSpellType spellType, WarEntityId targetEntityId, vec2 targetTile);

void wst_changeNextState(WarContext* context, WarEntity* entity, WarState* state, bool wst_leaveState, bool wst_enterState);
bool wst_changeStateNextState(WarContext* context, WarEntity* entity, WarState* state);

WarState* wst_getState(WarContext* context, WarEntity* entity, WarStateType type);
WarState* wst_getDirectState(WarContext* context, WarEntity* entity, WarStateType type);
WarState* wst_getNextState(WarContext* context, WarEntity* entity, WarStateType type);

WarState* wst_getIdleState(WarContext* context, WarEntity* entity);
WarState* wst_getMoveState(WarContext* context, WarEntity* entity);
WarState* wst_getPatrolState(WarContext* context, WarEntity* entity);
WarState* wst_getFollowState(WarContext* context, WarEntity* entity);
WarState* wst_getAttackState(WarContext* context, WarEntity* entity);
WarState* wst_getDeathState(WarContext* context, WarEntity* entity);
WarState* wst_getCollapseState(WarContext* context, WarEntity* entity);
WarState* wst_getGatherGoldState(WarContext* context, WarEntity* entity);
WarState* wst_getMiningState(WarContext* context, WarEntity* entity);
WarState* wst_getGatherWoodState(WarContext* context, WarEntity* entity);
WarState* wst_getChoppingState(WarContext* context, WarEntity* entity);
WarState* wst_getDeliverState(WarContext* context, WarEntity* entity);
WarState* wst_getTrainState(WarContext* context, WarEntity* entity);
WarState* wst_getUpgradeState(WarContext* context, WarEntity* entity);
WarState* wst_getBuildState(WarContext* context, WarEntity* entity);
WarState* wst_getRepairState(WarContext* context, WarEntity* entity);
WarState* wst_getRepairingState(WarContext* context, WarEntity* entity);
WarState* wst_getCastState(WarContext* context, WarEntity* entity);

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

void wst_enterState(WarContext* context, WarEntity* entity, WarState* state);
void wst_leaveState(WarContext* context, WarEntity* entity, WarState* state);
void wst_updateStateMachine(WarContext* context, WarEntity* entity);
void wst_freeState(WarContext* context, WarState* state);
