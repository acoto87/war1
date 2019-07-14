typedef struct
{
    WarStateType type;
    void (*enterStateFunc)(WarContext* context, WarEntity* entity, WarState* state);
    void (*leaveStateFunc)(WarContext* context, WarEntity* entity, WarState* state);
    void (*updateStateFunc)(WarContext* context, WarEntity* entity, WarState* state);
    void (*freeStateFunc)(WarState* state);
} WarStateDescriptor;

WarState* createState(WarContext* context, WarEntity* entity, WarStateType type);
WarState* createIdleState(WarContext* context, WarEntity* entity, bool lookAround);
WarState* createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[]);
WarState* createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[]);
WarState* createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, s32 distance);
WarState* createAttackState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile);
WarState* createDeathState(WarContext* context, WarEntity* entity);
WarState* createDamagedState(WarContext* context, WarEntity* entity);
WarState* createCollapseState(WarContext* context, WarEntity* entity);
WarState* createWaitState(WarContext* context, WarEntity* entity, f32 waitTime);
WarState* createGatherGoldState(WarContext* context, WarEntity* entity, WarEntityId goldmineId);
WarState* createMiningState(WarContext* context, WarEntity* entity, WarEntityId goldmineId);
WarState* createGatherWoodState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 position);
WarState* createChoppingState(WarContext* context, WarEntity* entity, WarEntityId forestId, vec2 position);
WarState* createDeliverState(WarContext* context, WarEntity* entity, WarEntityId townHallId);
WarState* createTrainState(WarContext* context, WarEntity* entity, WarUnitType unitToBuild, f32 buildTime);
WarState* createUpgradeState(WarContext* context, WarEntity* entity, WarUpgradeType upgradeToBuild, f32 buildTime);
WarState* createBuildState(WarContext* context, WarEntity* entity, f32 buildTime);
WarState* createRepairState(WarContext* context, WarEntity* entity, WarEntityId buildingId);
WarState* createRepairingState(WarContext* context, WarEntity* entity, WarEntityId buildingId);
WarState* createCastState(WarContext* context, WarEntity* entity, WarSpellType spellType, WarEntityId targetEntityId, vec2 targetTile);

void changeNextState(WarContext* context, WarEntity* entity, WarState* state, bool leaveState, bool enterState);
bool changeStateNextState(WarContext* context, WarEntity* entity, WarState* state);

WarState* getState(WarEntity* entity, WarStateType type);
WarState* getDirectState(WarEntity* entity, WarStateType type);
WarState* getNextState(WarEntity* entity, WarStateType type);

#define getIdleState(entity) getDirectState(entity, WAR_STATE_IDLE)
#define getMoveState(entity) getState(entity, WAR_STATE_MOVE)
#define getPatrolState(entity) getState(entity, WAR_STATE_PATROL)
#define getFollowState(entity) getState(entity, WAR_STATE_FOLLOW)
#define getAttackState(entity) getState(entity, WAR_STATE_ATTACK)
#define getDeathState(entity) getState(entity, WAR_STATE_DEATH)
#define getCollapseState(entity) getState(entity, WAR_STATE_COLLAPSE)
#define getGatherGoldState(entity) getState(entity, WAR_STATE_GOLD)
#define getMiningState(entity) getState(entity, WAR_STATE_MINING)
#define getGatherWoodState(entity) getState(entity, WAR_STATE_WOOD)
#define getChoppingState(entity) getState(entity, WAR_STATE_CHOP)
#define getDeliverState(entity) getState(entity, WAR_STATE_DELIVER)
#define getTrainState(entity) getState(entity, WAR_STATE_TRAIN)
#define getUpgradeState(entity) getState(entity, WAR_STATE_UPGRADE)
#define getBuildState(entity) getState(entity, WAR_STATE_BUILD)
#define getRepairState(entity) getState(entity, WAR_STATE_REPAIR)
#define getRepairingState(entity) getState(entity, WAR_STATE_REPAIRING)
#define getCastState(entity) getState(entity, WAR_STATE_CAST)

bool hasState(WarEntity* entity, WarStateType type);
bool hasDirectState(WarEntity* entity, WarStateType type);
bool hasNextState(WarEntity* entity, WarStateType type);

#define isIdle(entity) hasDirectState(entity, WAR_STATE_IDLE)
#define isMoving(entity) hasState(entity, WAR_STATE_MOVE)
#define isPatrolling(entity) hasState(entity, WAR_STATE_PATROL)
#define isFollowing(entity) hasState(entity, WAR_STATE_FOLLOW)
#define isAttacking(entity) hasState(entity, WAR_STATE_ATTACK)
#define isDead(entity) hasState(entity, WAR_STATE_DEATH)
#define isCollapsing(entity) hasState(entity, WAR_STATE_COLLAPSE)
#define isGatheringGold(entity) hasState(entity, WAR_STATE_GOLD)
#define isMining(entity) hasState(entity, WAR_STATE_MINING)
#define isGatheringWood(entity) hasState(entity, WAR_STATE_WOOD)
#define isChopping(entity) hasState(entity, WAR_STATE_CHOP)
#define isDelivering(entity) hasState(entity, WAR_STATE_DELIVER)
#define isTraining(entity) hasState(entity, WAR_STATE_TRAIN)
#define isUpgrading(entity) hasState(entity, WAR_STATE_UPGRADE)
#define isBuilding(entity) hasState(entity, WAR_STATE_BUILD)
#define isRepairing(entity) hasState(entity, WAR_STATE_REPAIR)
#define isRepairing2(entity) hasState(entity, WAR_STATE_REPAIRING)
#define isCasting(entity) hasState(entity, WAR_STATE_CAST)

#define isGoingToIdle(entity) hasNextState(entity, WAR_STATE_IDLE)
#define isGoingToMove(entity) hasNextState(entity, WAR_STATE_MOVE)
#define isGoingToPatrol(entity) hasNextState(entity, WAR_STATE_PATROL)
#define isGoingToFollow(entity) hasNextState(entity, WAR_STATE_FOLLOW)
#define isGoingToAttack(entity) hasNextState(entity, WAR_STATE_ATTACK)
#define isGoingToDie(entity) hasNextState(entity, WAR_STATE_DEATH)
#define isGoingToCollapse(entity) hasNextState(entity, WAR_STATE_COLLAPSE)
#define isGoingToGatherGold(entity) hasNextState(entity, WAR_STATE_GOLD)
#define isGoingToMine(entity) hasNextState(entity, WAR_STATE_MINING)
#define isGoingToGatherWood(entity) hasNextState(entity, WAR_STATE_WOOD)
#define isGoingToChop(entity) hasNextState(entity, WAR_STATE_CHOP)
#define isGoingToDeliver(entity) hasNextState(entity, WAR_STATE_DELIVER)
#define isGoingToTrain(entity) hasNextState(entity, WAR_STATE_TRAIN)
#define isGoingToUpgrade(entity) hasNextState(entity, WAR_STATE_UPGRADE)
#define isGoingToBuild(entity) hasNextState(entity, WAR_STATE_BUILD)
#define isGoingToRepair(entity) hasNextState(entity, WAR_STATE_REPAIR)
#define isGoingToRepair2(entity) hasNextState(entity, WAR_STATE_REPAIRING)
#define isGoingToCast(entity) hasNextState(entity, WAR_STATE_CAST)

#define setDelay(state, seconds) ((state)->delay = (seconds))

bool isInsideBuilding(WarEntity* entity)
{
    if (isMining(entity))
    {
        return true;
    }

    if(isDelivering(entity))
    {
        WarState* deliver = getDeliverState(entity);
        return deliver->deliver.insideBuilding;
    }

    return false;
}

void enterState(WarContext* context, WarEntity* entity, WarState* state);
void leaveState(WarContext* context, WarEntity* entity, WarState* state);
void updateStateMachine(WarContext* context, WarEntity* entity);
void freeState(WarState* state);
