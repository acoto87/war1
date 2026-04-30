#pragma once

#include "war_math.h"
#include "war_units.h"
#include "war_pathfinder.h"

struct _WarState
{
    WarStateType type;
    s32 entityId;
    f32 nextUpdateTime;
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
            s32 positionIndex;
            vec2List positions;

            s32 pathNodeIndex;
            WarMapPath path;

            s32 waitCount;
            bool checkForAttacks;
        } move;

        struct
        {
            s32 positionIndex;
            vec2List positions;
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
            f32 waitTime;
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

WarState* wst_getState(WarEntity* entity, WarStateType type);
WarState* wst_getDirectState(WarEntity* entity, WarStateType type);
WarState* wst_getNextState(WarEntity* entity, WarStateType type);

#define getIdleState(entity) wst_getDirectState(entity, WAR_STATE_IDLE)
#define getMoveState(entity) wst_getState(entity, WAR_STATE_MOVE)
#define getPatrolState(entity) wst_getState(entity, WAR_STATE_PATROL)
#define getFollowState(entity) wst_getState(entity, WAR_STATE_FOLLOW)
#define getAttackState(entity) wst_getState(entity, WAR_STATE_ATTACK)
#define getDeathState(entity) wst_getState(entity, WAR_STATE_DEATH)
#define getCollapseState(entity) wst_getState(entity, WAR_STATE_COLLAPSE)
#define getGatherGoldState(entity) wst_getState(entity, WAR_STATE_GOLD)
#define getMiningState(entity) wst_getState(entity, WAR_STATE_MINING)
#define getGatherWoodState(entity) wst_getState(entity, WAR_STATE_WOOD)
#define getChoppingState(entity) wst_getState(entity, WAR_STATE_CHOP)
#define getDeliverState(entity) wst_getState(entity, WAR_STATE_DELIVER)
#define getTrainState(entity) wst_getState(entity, WAR_STATE_TRAIN)
#define getUpgradeState(entity) wst_getState(entity, WAR_STATE_UPGRADE)
#define getBuildState(entity) wst_getState(entity, WAR_STATE_BUILD)
#define getRepairState(entity) wst_getState(entity, WAR_STATE_REPAIR)
#define getRepairingState(entity) wst_getState(entity, WAR_STATE_REPAIRING)
#define getCastState(entity) wst_getState(entity, WAR_STATE_CAST)

bool wst_hasState(WarEntity* entity, WarStateType type);
bool wst_hasDirectState(WarEntity* entity, WarStateType type);
bool wst_hasNextState(WarEntity* entity, WarStateType type);

#define isIdle(entity) wst_hasDirectState(entity, WAR_STATE_IDLE)
#define isMoving(entity) wst_hasState(entity, WAR_STATE_MOVE)
#define isPatrolling(entity) wst_hasState(entity, WAR_STATE_PATROL)
#define isFollowing(entity) wst_hasState(entity, WAR_STATE_FOLLOW)
#define isAttacking(entity) wst_hasState(entity, WAR_STATE_ATTACK)
#define isDead(entity) wst_hasState(entity, WAR_STATE_DEATH)
#define isCollapsing(entity) wst_hasState(entity, WAR_STATE_COLLAPSE)
#define isGatheringGold(entity) wst_hasState(entity, WAR_STATE_GOLD)
#define isMining(entity) wst_hasState(entity, WAR_STATE_MINING)
#define isGatheringWood(entity) wst_hasState(entity, WAR_STATE_WOOD)
#define isChopping(entity) wst_hasState(entity, WAR_STATE_CHOP)
#define isDelivering(entity) wst_hasState(entity, WAR_STATE_DELIVER)
#define isTraining(entity) wst_hasState(entity, WAR_STATE_TRAIN)
#define isUpgrading(entity) wst_hasState(entity, WAR_STATE_UPGRADE)
#define isBuilding(entity) wst_hasState(entity, WAR_STATE_BUILD)
#define isRepairing(entity) wst_hasState(entity, WAR_STATE_REPAIR)
#define isRepairing2(entity) wst_hasState(entity, WAR_STATE_REPAIRING)
#define isCasting(entity) wst_hasState(entity, WAR_STATE_CAST)

#define isGoingToIdle(entity) wst_hasNextState(entity, WAR_STATE_IDLE)
#define isGoingToMove(entity) wst_hasNextState(entity, WAR_STATE_MOVE)
#define isGoingToPatrol(entity) wst_hasNextState(entity, WAR_STATE_PATROL)
#define isGoingToFollow(entity) wst_hasNextState(entity, WAR_STATE_FOLLOW)
#define isGoingToAttack(entity) wst_hasNextState(entity, WAR_STATE_ATTACK)
#define isGoingToDie(entity) wst_hasNextState(entity, WAR_STATE_DEATH)
#define isGoingToCollapse(entity) wst_hasNextState(entity, WAR_STATE_COLLAPSE)
#define isGoingToGatherGold(entity) wst_hasNextState(entity, WAR_STATE_GOLD)
#define isGoingToMine(entity) wst_hasNextState(entity, WAR_STATE_MINING)
#define isGoingToGatherWood(entity) wst_hasNextState(entity, WAR_STATE_WOOD)
#define isGoingToChop(entity) wst_hasNextState(entity, WAR_STATE_CHOP)
#define isGoingToDeliver(entity) wst_hasNextState(entity, WAR_STATE_DELIVER)
#define isGoingToTrain(entity) wst_hasNextState(entity, WAR_STATE_TRAIN)
#define isGoingToUpgrade(entity) wst_hasNextState(entity, WAR_STATE_UPGRADE)
#define isGoingToBuild(entity) wst_hasNextState(entity, WAR_STATE_BUILD)
#define isGoingToRepair(entity) wst_hasNextState(entity, WAR_STATE_REPAIR)
#define isGoingToRepair2(entity) wst_hasNextState(entity, WAR_STATE_REPAIRING)
#define isGoingToCast(entity) wst_hasNextState(entity, WAR_STATE_CAST)

#define setDelay(state, seconds) ((state)->delay = (seconds))

bool wst_isInsideBuilding(WarEntity* entity);

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
