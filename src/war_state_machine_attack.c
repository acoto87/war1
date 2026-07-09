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
            moveState->checkForAttacks = true;
            wst_pushState(context, entity, (WarStateBase*)moveState);
            TracyCZoneEnd(ctx);
            return;
        }

        wst_popState(context, entity);
        TracyCZoneEnd(ctx);
        return;
    }

    if (wu_isUnit(targetEntity))
    {
        // if the target entity is an unit the instead of using the tile where
        // the player click, use a point on the target unit that is closer to
        // the attacking unit
        targetTile = wu_unitTileOnTarget(context, entity, targetEntity);
    }

    // if the unit is not in range to attack, chase it
    if (wu_isUnit(targetEntity) && !wu_unitInRange(context, entity, targetEntity, stats->range))
    {
        WarStateFollow* followState = wst_createFollowState(context, entity, targetEntityId, targetTile, stats->range * MEGA_TILE_WIDTH);
        wst_pushState(context, entity, (WarStateBase*)followState);
        TracyCZoneEnd(ctx);
        return;
    }

    if(wu_isWall(targetEntity) && !wu_tileInRange(context, entity, targetTile, stats->range))
    {
        WarStateFollow* followState = wst_createFollowState(context, entity, 0, targetTile, stats->range * MEGA_TILE_WIDTH);
        wst_pushState(context, entity, (WarStateBase*)followState);
        TracyCZoneEnd(ctx);
        return;
    }

    // if the unit is attacking a worker that is currently gathering and inside of the goldmine or the townhall,
    // wcmd_stop the attacking for a moment until the unit come out again
    if (wst_isInsideBuilding(context, targetEntity))
    {
        WarStateWait* waitState = wst_createWaitState(context, entity, 1.0f);
        wst_pushState(context, entity, (WarStateBase*)waitState);
        TracyCZoneEnd(ctx);
        return;
    }

    wpath_setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
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
                wst_popState(context, entity);
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
                    vec2 targetPosition = wu_getUnitCenterPosition(context, targetEntity);
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
                    wst_popState(context, entity);
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


void wst_updateAttackStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateAttackStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateAttack*      states  = storage->attack;
    bool*             occupied = storage->occupied[WAR_STATE_ATTACK];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateAttack*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->depth == 0 || sm->stack[sm->depth - 1].type != WAR_STATE_ATTACK || sm->stack[sm->depth - 1].idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateAttackState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
