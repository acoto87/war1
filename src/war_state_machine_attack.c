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

    vec2 position = vec2MapToTileCoordinates(entity->transform.position);

    vec2 targetTile = state->attack.targetTile;

    WarEntityId targetEntityId = state->attack.targetEntityId;
    WarEntity* targetEntity = findEntity(context, targetEntityId);

    // if the entity to attack doesn't exists, go idle
    if (!targetEntity)
    {
        if(!positionInRange(entity, targetTile, stats.range))
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

    // if the unit is not in range to attack, chase it
    if (isUnit(targetEntity) && !unitInRange(entity, targetEntity, stats.range))
    {
        WarState* followState = createFollowState(context, entity, targetEntityId, stats.range);
        followState->nextState = state;
        changeNextState(context, entity, followState, false, true);
        return;
    }

    vec2 targetPosition = isUnit(targetEntity) 
        ? unitPointOnTarget(entity, targetEntity) : targetTile;

    if(isWall(targetEntity) && !positionInRange(entity, targetTile, stats.range))
    {
        WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, targetPosition));
        moveState->nextState = state;
        changeNextState(context, entity, moveState, false, true);
        return;
    }

    // if the unit is attacking a worker that is currently gathering and inside of the goldmine or the townhall, 
    // stop the attacking for a moment until the unit come out again
    if (isInsideBuilding(targetEntity))
    {
        WarState* waitState = createWaitState(context, entity, getScaledTime(context, 1.0f));
        waitState->nextState = state;
        changeNextState(context, entity, waitState, false, true);
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
                    s32 resourceIndex;
                    WarProjectileType projectileType;

                    switch (entity->unit.type)
                    {
                        case WAR_UNIT_ARCHER:
                        case WAR_UNIT_SPEARMAN:
                        {
                            resourceIndex = 349;
                            projectileType = WAR_PROJECTILE_ARROW;
                            break;
                        }

                        case WAR_UNIT_CATAPULT_HUMANS:
                        case WAR_UNIT_CATAPULT_ORCS:
                        {
                            resourceIndex = 348;
                            projectileType = WAR_PROJECTILE_CATAPULT;
                            break;
                        }

                        case WAR_UNIT_CONJURER:
                        case WAR_UNIT_WARLOCK:
                        case WAR_UNIT_NECROLYTE:
                        case WAR_UNIT_MEDIVH:
                        {
                            resourceIndex = 358;
                            projectileType = WAR_PROJECTILE_FIREBALL_2;
                            break;
                        }

                        case WAR_UNIT_WATER_ELEMENTAL:
                        {
                            resourceIndex = 357;
                            projectileType = WAR_PROJECTILE_WATER_ELEMENTAL;
                            break;
                        }
                        
                        default:
                        {
                            // unreachable
                            logWarning("Invalid unit firing a projectile: %d\n", entity->unit.type);
                            break;
                        }
                    }
                    
                    vec2 origin = getUnitCenterPosition(entity, false);
                    vec2 target = getUnitCenterPosition(targetEntity, false);

                    WarEntity* projectile = createEntity(context, WAR_ENTITY_TYPE_PROJECTILE, true);
                    addTransformComponent(context, projectile, origin);
                    addSpriteComponentFromResource(context, projectile, imageResourceRef(resourceIndex));
                    addProjectileComponent(context, projectile, entity->id, projectileType, origin, target);
                }
                else
                {
                    // every unit has a 20 percent chance to miss (except catapults)
                    if (chance(80))
                    {
                        takeDamage(context, targetEntity, unit->minDamage, unit->rndDamage);
                    }
                }

                switch (action->lastSoundStep)
                {
                    case WAR_ACTION_STEP_SOUND_SWORD:
                        createAudioRandom(context, WAR_SWORD_ATTACK_1, WAR_SWORD_ATTACK_3, false);
                        break;
                    case WAR_ACTION_STEP_SOUND_FIST:
                        createAudio(context, WAR_FIST_ATTACK, false);
                        break;
                    case WAR_ACTION_STEP_SOUND_FIREBALL:
                        createAudio(context, WAR_FIREBALL, false);
                        break;
                    case WAR_ACTION_STEP_SOUND_CATAPULT:
                        createAudio(context, WAR_CATAPULT_ROCK_FIRED, false);
                        break;
                    case WAR_ACTION_STEP_SOUND_ARROW:
                        createAudio(context, WAR_ARROW_SPEAR, false);
                        break;
                    default:
                        // do nothing here
                        break;
                }
            }
        }
        else if(isWall(targetEntity))
        {
            WarWallPiece* piece = getWallPieceAtPosition(targetEntity, state->attack.targetTile.x, state->attack.targetTile.y);
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
                        // TODO: fire the projectile here
                    }
                    else
                    {
                        // every unit has a 20 percent chance to miss (except catapults)
                        if (chance(80))
                        {
                            takeWallDamage(context, targetEntity, piece, unit->minDamage, unit->rndDamage);
                        }

                        switch (action->lastSoundStep)
                        {
                            case WAR_ACTION_STEP_SOUND_SWORD:
                                createAudioRandom(context, WAR_SWORD_ATTACK_1, WAR_SWORD_ATTACK_3, false);
                                break;
                            case WAR_ACTION_STEP_SOUND_FIST:
                                createAudio(context, WAR_FIST_ATTACK, false);
                                break;
                            case WAR_ACTION_STEP_SOUND_FIREBALL:
                                createAudio(context, WAR_FIREBALL, false);
                                break;
                            case WAR_ACTION_STEP_SOUND_CATAPULT:
                                createAudio(context, WAR_CATAPULT_ROCK_FIRED, false);
                                break;
                            case WAR_ACTION_STEP_SOUND_ARROW:
                                createAudio(context, WAR_ARROW_SPEAR, false);
                                break;
                            default:
                                // do nothing here
                                break;
                        }
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