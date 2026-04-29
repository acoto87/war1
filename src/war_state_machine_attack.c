#include "war_state_machine.h"

#include "war_map.h"
#include "war_actions.h"
#include "war_audio.h"
#include "war_units.h"

WarState* wst_createAttackState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_ATTACK);
    state->wcomm_attack.targetEntityId = targetEntityId;
    state->wcomm_attack.targetTile = targetTile;
    return state;
}

void wst_enterAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_leaveAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updateAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = wu_getUnitSize(entity);
    vec2 position = wmap_vec2MapToTileCoordinates(entity->transform.position);

    WarUnitStats stats = wu_getUnitStats(unit->type);

    WarEntityId targetEntityId = (WarEntityId)state->wcomm_attack.targetEntityId;
    WarEntity* targetEntity = we_findEntity(context, targetEntityId);

    vec2 targetTile = state->wcomm_attack.targetTile;

    // if the entity to wcomm_attack doesn't exists, go to the attacking point or go idle
    if (!targetEntity)
    {
        // when going to an attacking point (where there is no target unit)
        // check if the attacking unit is in range 1, no matter if the range
        // of the attacking unit is greater
        if(!wu_tileInRange(entity, targetTile, 1))
        {
            WarState* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, targetTile));
            moveState->nextState = state;
            moveState->wcomm_move.checkForAttacks = true;
            wst_changeNextState(context, entity, moveState, false, true);
            return;
        }

        WarState* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, idleState, true, true);
        return;
    }

    if (isUnit(targetEntity))
    {
        // if the target entity is an unit the instead of using the tile where
        // the player click, use a point on the target unit that is closer to
        // the attacking unit
        targetTile = wu_unitPointOnTarget(entity, targetEntity);
    }

    // if the unit is not in range to wcomm_attack, chase it
    if (isUnit(targetEntity) && !wu_unitInRange(entity, targetEntity, stats.range))
    {
        WarState* followState = wst_createFollowState(context, entity, targetEntityId, targetTile, stats.range);
        followState->nextState = state;
        wst_changeNextState(context, entity, followState, false, true);
        return;
    }

    if(isWall(targetEntity) && !wu_tileInRange(entity, targetTile, stats.range))
    {
        WarState* followState = wst_createFollowState(context, entity, 0, targetTile, stats.range);
        followState->nextState = state;
        wst_changeNextState(context, entity, followState, false, true);
        return;
    }

    // if the unit is attacking a worker that is currently gathering and inside of the goldmine or the townhall,
    // wcomm_stop the attacking for a moment until the unit come out again
    if (wst_isInsideBuilding(targetEntity))
    {
        WarState* waitState = wst_createWaitState(context, entity, 1.0f);
        waitState->nextState = state;
        wst_changeNextState(context, entity, waitState, false, true);
        return;
    }

    setStaticEntity(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
    wu_setUnitDirectionFromDiff(entity, targetTile.x - position.x, targetTile.y - position.y);
    wact_setAction(context, entity, WAR_ACTION_TYPE_ATTACK, false, 1.0f);

    WarUnitAction* action = &unit->actions[unit->actionType];
    if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
    {
        // when the unit begin an wcomm_attack, it is not invisible anymore
        unit->invisible = false;
        unit->invisibilityTime = 0;

        // do damage
        if (isUnit(targetEntity))
        {
            // if the target entity is dead or is collapsing (in case of buildings), go to idle
            // do this check before apply damage in case of multiple units attacking.
            // one of them could cause the unit to die, so the other should wcomm_stop doing further damage.
            if (isDead(targetEntity) || isGoingToDie(targetEntity) ||
                isCollapsing(targetEntity) || isGoingToCollapse(targetEntity))
            {
                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);
            }
            else
            {
                if (wu_isRangeUnit(entity))
                {
                    we_rangeAttack(context, entity, targetEntity);
                }
                else
                {
                    we_meleeAttack(context, entity, targetEntity);
                }

                vec2 targetPosition = wu_getUnitCenterPosition(targetEntity, false);
                wa_playAttackSound(context, targetPosition, action->lastSoundStep);
            }
        }
        else if(isWall(targetEntity))
        {
                WarWallPiece* piece = we_getWallPieceAtPosition(targetEntity, (s32)targetTile.x, (s32)targetTile.y);
            if (piece)
            {
                // if the piece of the wall the unit is attacking has no more hit points, go to idle.
                // do this check before apply damage in case of multiple units attacking.
                // one of them could destroy the piece, so the other should wcomm_stop doing further damage.
                if (piece->hp == 0)
                {
                    WarState* idleState = wst_createIdleState(context, entity, true);
                    wst_changeNextState(context, entity, idleState, true, true);
                }
                else
                {
                    if (wu_isRangeUnit(entity))
                    {
                        we_rangeWallAttack(context, entity, targetEntity, piece);
                    }
                    else
                    {
                        we_meleeWallAttack(context, entity, targetEntity, piece);
                    }

                    vec2 targetPosition = wmap_vec2TileToMapCoordinates(targetTile, true);
                    wa_playAttackSound(context, targetPosition, action->lastSoundStep);
                }
            }
        }

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }
}

void wst_freeAttackState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}
