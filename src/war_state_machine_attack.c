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
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);

    WarUnitStats stats = getUnitStats(unit->type);

    WarEntityId targetEntityId = state->attack.targetEntityId;
    WarEntity* targetEntity = findEntity(context, targetEntityId);

    vec2 targetTile = state->attack.targetTile;

    // if the entity to attack doesn't exists, go to the attacking point or go idle
    if (!targetEntity)
    {
        // when going to an attacking point (where there is no target unit)
        // check if the attacking unit is in range 1, no matter if the range
        // of the attacking unit is greater
        if(!tileInRange(entity, targetTile, 1))
        {
            WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, targetTile));
            moveState->nextState = state;
            moveState->move.checkForAttacks = true;
            changeNextState(context, entity, moveState, false, true);
            return;
        }

        WarState* idleState = createIdleState(context, entity, true);
        changeNextState(context, entity, idleState, true, true);
        return;
    }

    if (isUnit(targetEntity))
    {
        // if the target entity is an unit the instead of using the tile where
        // the player click, use a point on the target unit that is closer to
        // the attacking unit
        targetTile = unitPointOnTarget(entity, targetEntity);
    }

    // if the unit is not in range to attack, chase it
    if (isUnit(targetEntity) && !unitInRange(entity, targetEntity, stats.range))
    {
        WarState* followState = createFollowState(context, entity, targetEntityId, targetTile, stats.range);
        followState->nextState = state;
        changeNextState(context, entity, followState, false, true);
        return;
    }

    if(isWall(targetEntity) && !tileInRange(entity, targetTile, stats.range))
    {
        WarState* followState = createFollowState(context, entity, 0, targetTile, stats.range);
        followState->nextState = state;
        changeNextState(context, entity, followState, false, true);
        return;
    }

    // if the unit is attacking a worker that is currently gathering and inside of the goldmine or the townhall,
    // stop the attacking for a moment until the unit come out again
    if (isInsideBuilding(targetEntity))
    {
        WarState* waitState = createWaitState(context, entity, 1.0f);
        waitState->nextState = state;
        changeNextState(context, entity, waitState, false, true);
        return;
    }

    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
    setUnitDirectionFromDiff(entity, targetTile.x - position.x, targetTile.y - position.y);
    setAction(context, entity, WAR_ACTION_TYPE_ATTACK, false, 1.0f);

    WarUnitAction* action = unit->actions.items[unit->actionIndex];
    if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
    {
        // when the unit begin an attack, it is not invisible anymore
        unit->invisible = false;
        unit->invisibilityTime = 0;

        // do damage
        if (isUnit(targetEntity))
        {
            // if the target entity is dead or is collapsing (in case of buildings), go to idle
            // do this check before apply damage in case of multiple units attacking.
            // one of them could cause the unit to die, so the other should stop doing further damage.
            if (isDead(targetEntity) || isGoingToDie(targetEntity) ||
                isCollapsing(targetEntity) || isGoingToCollapse(targetEntity))
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);
            }
            else
            {
                if (isRangeUnit(entity))
                {
                    rangeAttack(context, entity, targetEntity);
                }
                else
                {
                    meleeAttack(context, entity, targetEntity);
                }

                vec2 targetPosition = getUnitCenterPosition(targetEntity, false);
                playAttackSound(context, targetPosition, action->lastSoundStep);
            }
        }
        else if(isWall(targetEntity))
        {
            WarWallPiece* piece = getWallPieceAtPosition(targetEntity, targetTile.x, targetTile.y);
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
                    if (isRangeUnit(entity))
                    {
                        rangeWallAttack(context, entity, targetEntity, piece);
                    }
                    else
                    {
                        meleeWallAttack(context, entity, targetEntity, piece);
                    }

                    vec2 targetPosition = vec2TileToMapCoordinates(targetTile, true);
                    playAttackSound(context, targetPosition, action->lastSoundStep);
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