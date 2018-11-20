#include "../war_state_machine.h"

WarState* createAttackState(WarContext* context, WarEntity* entity, WarEntityId targetEntityId, vec2 targetTile)
{
    WarState* state = createState(context, entity, WAR_STATE_ATTACK);
    state->attack.targetEntityId = targetEntityId;
    state->attack.targetTile = targetTile;
    return state;
}

void enterAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void leaveAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateAttackState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;
    vec2 unitSize = getUnitSize(entity);
    WarUnitStats stats = getUnitStats(unit->type);

    WarEntity* targetEntity = findEntity(context, state->attack.targetEntityId);

    // if the entity to attack doesn't exists, go idle
    if (!targetEntity)
    {
        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    vec2 targetPosition = isUnit(targetEntity) ? unitPointOnTarget(entity, targetEntity) : state->attack.targetTile;

    // if the unit is not in range to attack, chase it
    if (isUnit(targetEntity) && !unitInRange(entity, targetEntity, stats.range))
    {
        WarState* followState = createFollowState(context, entity, state->attack.targetEntityId, stats.range);
        followState->nextState = state;
        changeNextState(context, entity, followState, false, true);
        return;
    }
    
    if(isWall(targetEntity) && !positionInRange(entity, state->attack.targetTile, stats.range))
    {
        WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, targetPosition));
        moveState->nextState = state;
        changeNextState(context, entity, moveState, false, true);
        return;
    }

    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
    setUnitDirectionFromDiff(entity, targetPosition.x - position.x, targetPosition.y - position.y);
    setAction(context, entity, WAR_ACTION_TYPE_ATTACK, false, 1.0f);

    WarUnitAction* action = unit->actions.items[unit->actionIndex];
    if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
    {
        // do damage
        if (isUnit(targetEntity))
        {
            // if the target entity is dead or is collapsing (in case of buildings), go to idle
            // do this check before apply damage in case of multiple units attacking.
            // one of them could cause the unit to die, so the other should stop doing further damage.
            if (isDeath(targetEntity) || isGoingToDie(targetEntity) || 
                isCollapsing(targetEntity) || isGoingToCollapse(targetEntity))
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);
            }
            else
            {
                // every unit has a 20 percent chance to miss
                if (chance(80))
                {
                    takeDamage(context, targetEntity, unit->minDamage, unit->rndDamage);
                }
            }
        }
        else if(isWall(targetEntity))
        {
            WarWallPiece* piece = getWallPiece(targetEntity, state->attack.targetTile);
            if (piece)
            {
                // if the piece of the wall the unit is attacking has no more hit points, go to idle.
                // do this check before apply damage in case of multiple units attacking.
                // one of them could destroy the piece, so the other should stop doing further damage.
                if (piece->hp == 0)
                {
                    WarState* idleState = createIdleState(context, entity, true);
                    changeNextState(context, entity, idleState, true, true);
                }
                else 
                {
                    // every unit has a 20 percent chance to miss
                    if (chance(80))
                    {
                        takeWallDamage(context, targetEntity, piece, unit->minDamage, unit->rndDamage);
                    }
                }
            }
        }

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }
}

void freeAttackState(WarState* state)
{
}