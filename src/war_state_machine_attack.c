#include "war_state_machine.h"

#include "war.h"
#include "war_map.h"
#include "war_actions.h"
#include "war_audio.h"
#include "war_units.h"

#include "TracyC.h"

WarStateAttack* wst_createAttackState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile)
{
    TracyCZoneN(ctx, "wst_createAttackState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_ATTACK, entity->id);
    WarStateAttack* state = (WarStateAttack*)wst_deref(context, ref);
    state->targetEntityId = targetEntityId;
    state->targetTile = targetTile;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_enterAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_enterAttackState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_leaveAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveAttackState", true);

    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}

void wst_updateAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateAttackState", true);

    WarStateAttack* s = (WarStateAttack*)state;

    WarMap* map = context->map;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    vec2 unitSize = wu_getUnitSize(context, entity);
    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    vec2 position = wmap_mapToTileCoordinatesV(transform->position);

    const WarUnitStats* stats = wu_getUnitStats(unit->type);

    WarEntityId targetEntityId = (WarEntityId)s->targetEntityId;
    WarEntity* targetEntity = we_findEntity(context, targetEntityId);

    vec2 targetTile = s->targetTile;

    // if the entity to attack doesn't exists, go to the attacking point or go idle
    if (!targetEntity)
    {
        // when going to an attacking point (where there is no target unit)
        // check if the attacking unit is in range 1, no matter if the range
        // of the attacking unit is greater
        if(!wu_tileInRange(context, entity, targetTile, 1))
        {
            WarStateMove* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, targetTile));
            wst_chainNext(context, (WarStateBase*)moveState, (WarStateBase*)state);
            moveState->checkForAttacks = true;
            wst_changeNextState(context, entity, (WarStateBase*)moveState, false, true);
            TracyCZoneEnd(ctx);
            return;
        }

        WarStateIdle* idleState = wst_createIdleState(context, entity, true);
        wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
        TracyCZoneEnd(ctx);
        return;
    }

    if (wu_isUnit(targetEntity))
    {
        // if the target entity is an unit the instead of using the tile where
        // the player click, use a point on the target unit that is closer to
        // the attacking unit
        targetTile = wu_unitPointOnTarget(context, entity, targetEntity);
    }

    // if the unit is not in range to attack, chase it
    if (wu_isUnit(targetEntity) && !wu_unitInRange(context, entity, targetEntity, stats->range))
    {
        WarStateFollow* followState = wst_createFollowState(context, entity, targetEntityId, targetTile, stats->range);
        wst_chainNext(context, (WarStateBase*)followState, (WarStateBase*)state);
        wst_changeNextState(context, entity, (WarStateBase*)followState, false, true);
        TracyCZoneEnd(ctx);
        return;
    }

    if(wu_isWall(targetEntity) && !wu_tileInRange(context, entity, targetTile, stats->range))
    {
        WarStateFollow* followState = wst_createFollowState(context, entity, 0, targetTile, stats->range);
        wst_chainNext(context, (WarStateBase*)followState, (WarStateBase*)state);
        wst_changeNextState(context, entity, (WarStateBase*)followState, false, true);
        TracyCZoneEnd(ctx);
        return;
    }

    // if the unit is attacking a worker that is currently gathering and inside of the goldmine or the townhall,
    // wcmd_stop the attacking for a moment until the unit come out again
    if (wst_isInsideBuilding(context, targetEntity))
    {
        WarStateWait* waitState = wst_createWaitState(context, entity, 1.0f);
        wst_chainNext(context, (WarStateBase*)waitState, (WarStateBase*)state);
        wst_changeNextState(context, entity, (WarStateBase*)waitState, false, true);
        TracyCZoneEnd(ctx);
        return;
    }

    setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
    wu_setUnitDirectionFromDiff(context, entity, targetTile.x - position.x, targetTile.y - position.y);
    wact_setAction(context, entity, WAR_ACTION_TYPE_ATTACK, false, 1.0f);

    WarUnitAction* action = &unit->actions[unit->actionType];
    if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
    {
        // when the unit begin an attack, it is not invisible anymore
        unit->invisible = false;
        unit->invisibilityTime = 0;

        // do damage
        if (wu_isUnit(targetEntity))
        {
            // if the target entity is dead or is collapsing (in case of buildings), go to idle
            // do this check before apply damage in case of multiple units attacking.
            // one of them could cause the unit to die, so the other should wcmd_stop doing further damage.
            if (wst_isDead(context, targetEntity) || wst_isGoingToDie(context, targetEntity) ||
                wst_isCollapsing(context, targetEntity) || wst_isGoingToCollapse(context, targetEntity))
            {
                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
            }
            else
            {
                if (wu_isRangeUnit(context, entity))
                {
                    we_rangeAttack(context, entity, targetEntity);
                }
                else
                {
                    we_meleeAttack(context, entity, targetEntity);
                }

                if (context->gameTime - unit->lastAttackSoundGameTime >= MIN_ATTACK_SOUND_INTERVAL)
                {
                    vec2 targetPosition = wu_getUnitCenterPosition(context, targetEntity, false);
                    wa_playAttackSound(context, targetPosition, action->lastSoundStep);
                    unit->lastAttackSoundGameTime = context->gameTime;
                }
            }
        }
        else if(wu_isWall(targetEntity))
        {
            WarWallPiece* piece = we_getWallPieceAtPosition(context, targetEntity, (s32)targetTile.x, (s32)targetTile.y);
            if (piece)
            {
                // if the piece of the wall the unit is attacking has no more hit points, go to idle.
                // do this check before apply damage in case of multiple units attacking.
                // one of them could destroy the piece, so the other should wcmd_stop doing further damage.
                if (piece->hp == 0)
                {
                    WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                    wst_changeNextState(context, entity, (WarStateBase*)idleState, true, true);
                }
                else
                {
                    if (wu_isRangeUnit(context, entity))
                    {
                        we_rangeWallAttack(context, entity, targetEntity, piece);
                    }
                    else
                    {
                        we_meleeWallAttack(context, entity, targetEntity, piece);
                    }

                    if (context->gameTime - unit->lastAttackSoundGameTime >= MIN_ATTACK_SOUND_INTERVAL)
                    {
                        vec2 targetPosition = wmap_tileToMapCoordinatesV(targetTile, true);
                        wa_playAttackSound(context, targetPosition, action->lastSoundStep);
                        unit->lastAttackSoundGameTime = context->gameTime;
                    }
                }
            }
        }

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }

    TracyCZoneEnd(ctx);
}

void wst_freeAttackState(WarContext* context, WarState* state)
{
    TracyCZoneN(ctx, "wst_freeAttackState", true);

    NOT_USED(context);
    NOT_USED(state);

    TracyCZoneEnd(ctx);
}
