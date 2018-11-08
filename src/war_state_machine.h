#pragma once

#include "war_types.h"

WarState* createState(WarContext* context, WarEntity* entity, WarStateType type);
WarState* createIdleState(WarContext* context, WarEntity* entity, bool lookAround);
WarState* createMoveState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[]);
WarState* createPatrolState(WarContext* context, WarEntity* entity, s32 positionCount, vec2 positions[]);
WarState* createFollowState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, s32 distance);
WarState* createAttackState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId);
WarState* createDeathState(WarContext* context, WarEntity* entity);
WarState* createDamagedState(WarContext* context, WarEntity* entity);
WarState* createCollapseState(WarContext* context, WarEntity* entity);
WarState* createWaitState(WarContext* context, WarEntity* entity, f32 waitTime);

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

bool hasState(WarEntity* entity, WarStateType type);
bool hasDirectState(WarEntity* entity, WarStateType type);
bool hasNextState(WarEntity* entity, WarStateType type);

#define isIdle(entity) hasDirectState(entity, WAR_STATE_IDLE)
#define isMoving(entity) hasState(entity, WAR_STATE_MOVE)
#define isPatrolling(entity) hasState(entity, WAR_STATE_PATROL)
#define isFollowing(entity) hasState(entity, WAR_STATE_FOLLOW)
#define isAttacking(entity) hasState(entity, WAR_STATE_ATTACK)
#define isDeath(entity) hasState(entity, WAR_STATE_DEATH)
#define isDamaged(entity) hasState(entity, WAR_STATE_DAMAGED)
#define isCollapsing(entity) hasState(entity, WAR_STATE_COLLAPSE)

#define isGoingToIdle(entity) hasNextState(entity, WAR_STATE_IDLE)
#define isGoingToMove(entity) hasNextState(entity, WAR_STATE_MOVE)
#define isGoingToPatrol(entity) hasNextState(entity, WAR_STATE_PATROL)
#define isGoingToFollow(entity) hasNextState(entity, WAR_STATE_FOLLOW)
#define isGoingToAttack(entity) hasNextState(entity, WAR_STATE_ATTACK)
#define isGoingToDie(entity) hasNextState(entity, WAR_STATE_DEATH)
#define isGoingToBeDamaged(entity) hasNextState(entity, WAR_STATE_DAMAGED)
#define isGoingToCollapse(entity) hasNextState(entity, WAR_STATE_COLLAPSE)

void enterState(WarContext* context, WarEntity* entity, WarState* state);
void leaveState(WarContext* context, WarEntity* entity, WarState* state);
void freeState(WarState* state);
void updateStateMachine(WarContext* context, WarEntity* entity);
