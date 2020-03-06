WarState* createCastState(WarContext* context, WarEntity* entity, WarSpellType spellType, WarEntityId targetEntityId, vec2 targetTile)
{
    WarState* state = createState(context, entity, WAR_STATE_CAST);
    state->cast.spellType = spellType;
    state->cast.targetEntityId = targetEntityId;
    state->cast.targetTile = targetTile;
    return state;
}

void enterCastState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void leaveCastState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateCastState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);

    WarSpellType spellType = state->cast.spellType;
    WarEntityId targetEntityId = state->cast.targetEntityId;
    vec2 targetTile = state->cast.targetTile;

    WarSpellStats stats = getSpellStats(spellType);

    if (stats.range)
    {
        if(!tileInRange(entity, targetTile, stats.range))
        {
            WarState* followState = createFollowState(context, entity, targetEntityId, targetTile, stats.range);
            followState->nextState = state;
            changeNextState(context, entity, followState, false, true);
            return;
        }
    }

    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
    setUnitDirectionFromDiff(entity, targetTile.x - position.x, targetTile.y - position.y);
    setAction(context, entity, WAR_ACTION_TYPE_ATTACK, false, 1.0f);

    WarUnitAction* action = unit->actions.items[unit->actionIndex];
    if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
    {
        // when the unit cast a spell, it is not invisible anymore
        unit->invisible = false;
        unit->invisibilityTime = 0;

        switch (spellType)
        {
            case WAR_SPELL_HEALING:
            {
                WarEntity* targetEntity = findEntity(context, targetEntityId);
                if (targetEntity && isDudeUnit(targetEntity))
                {
                    WarUnitComponent* targetUnit = &targetEntity->unit;

                    // the healing spell's strength is determined by units of mana.
                    // for every 6 units of mana, the damaged unit gets back 1 hit point.
                    //
                    // take all the hp the cleric can restore according to its mana
                    s32 hpToRestore = unit->mana / stats.manaCost;

                    // take in reality how much hp needs to be restored
                    hpToRestore = min(hpToRestore, targetUnit->maxhp - targetUnit->hp);

                    // recalculate how much mana the cleric need to spend
                    s32 manaToSpend = hpToRestore * stats.manaCost;

                    increaseUnitHp(context, targetEntity, hpToRestore);
                    decreaseUnitMana(context, entity, manaToSpend);

                    vec2 targetPosition = getUnitCenterPosition(targetEntity, false);

                    WarEntity* animEntity = createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                    addAnimationsComponent(context, animEntity);

                    createSpellAnimation(context, animEntity, targetPosition);
                    createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                }

                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_FAR_SIGHT:
            case WAR_SPELL_DARK_VISION:
            {
                if (decreaseUnitMana(context, entity, stats.manaCost))
                {
                    vec2 targetPosition = vec2TileToMapCoordinates(targetTile, true);

                    WarEntity* sight = createEntity(context, WAR_ENTITY_TYPE_SIGHT, true);
                    addSightComponent(context, sight, targetTile, stats.time);
                    addAnimationsComponent(context, sight);

                    createSpellAnimation(context, sight, targetPosition);
                    createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                }

                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_INVISIBILITY:
            {
                WarEntity* targetEntity = findEntity(context, targetEntityId);
                if (targetEntity && isDudeUnit(targetEntity))
                {
                    WarUnitComponent* targetUnit = &targetEntity->unit;

                    if (decreaseUnitMana(context, entity, stats.manaCost))
                    {
                        targetUnit->invisible = true;
                        targetUnit->invisibilityTime = stats.time;

                        vec2 targetPosition = getUnitCenterPosition(targetEntity, false);

                        WarEntity* animEntity = createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                        addAnimationsComponent(context, animEntity);

                        createSpellAnimation(context, animEntity, targetPosition);
                        createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                    }
                }

                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_RAIN_OF_FIRE:
            {
                if (decreaseUnitMana(context, entity, stats.manaCost))
                {
                    vec2 targetTilePosition = vec2TileToMapCoordinates(targetTile, true);
                    s32 radius = 2 * MEGA_TILE_WIDTH;

                    s32 projectilesCount = 5;
                    while (projectilesCount--)
                    {
                        f32 offsetx = randomf(-radius, radius);
                        f32 offsety = randomf(-radius, radius);
                        vec2 target = vec2Addv(targetTilePosition, vec2f(offsetx, offsety));

                        offsety = randomf(MEGA_TILE_WIDTH, MEGA_TILE_WIDTH * 4);
                        vec2 origin = vec2f(target.x, map->viewport.y - offsety);

                        createProjectile(context, WAR_PROJECTILE_RAIN_OF_FIRE, 0, 0, origin, target);
                    }
                }
                else
                {
                    WarState* idleState = createIdleState(context, entity, true);
                    changeNextState(context, entity, idleState, true, true);
                }

                break;
            }

            case WAR_SPELL_RAISE_DEAD:
            {
                WarEntityList* nearUnits = getNearUnits(context, targetTile, 4);
                for (s32 i = 0; i < nearUnits->count; i++)
                {
                    WarEntity* targetEntity = nearUnits->items[i];
                    if (targetEntity && isCorpseUnit(targetEntity))
                    {
                        if (decreaseUnitMana(context, entity, stats.manaCost))
                        {
                            vec2 targetPosition = getUnitCenterPosition(targetEntity, true);
                            createUnit(context, WAR_UNIT_SKELETON, targetPosition.x, targetPosition.y,
                                       unit->player, WAR_RESOURCE_NONE, 0, true);

                            targetPosition = getUnitCenterPosition(targetEntity, false);

                            WarEntity* animEntity = createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                            addAnimationsComponent(context, animEntity);

                            createSpellAnimation(context, animEntity, targetPosition);
                            createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);

                            removeEntityById(context, targetEntity->id);
                        }
                    }
                }
                WarEntityListFree(nearUnits);

                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_UNHOLY_ARMOR:
            {
                WarEntity* targetEntity = findEntity(context, targetEntityId);
                if (targetEntity && isDudeUnit(targetEntity))
                {
                    WarUnitComponent* targetUnit = &targetEntity->unit;

                    if (decreaseUnitMana(context, entity, stats.manaCost))
                    {
                        decreaseUnitHp(context, targetEntity, halfi(targetUnit->hp));

                        targetUnit->invulnerable = true;
                        targetUnit->invulnerabilityTime = stats.time;

                        vec2 targetPosition = getUnitCenterPosition(targetEntity, false);

                        WarEntity* animEntity = createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                        addAnimationsComponent(context, animEntity);

                        createSpellAnimation(context, animEntity, targetPosition);
                        createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                    }
                }

                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_POISON_CLOUD:
            {
                if (decreaseUnitMana(context, entity, stats.manaCost))
                {
                    vec2 targetPosition = vec2TileToMapCoordinates(targetTile, true);

                    WarEntity* poisonCloud = createEntity(context, WAR_ENTITY_TYPE_POISON_CLOUD, true);
                    addPoisonCloudComponent(context, poisonCloud, targetTile, stats.time);
                    addAnimationsComponent(context, poisonCloud);

                    createPoisonCloudAnimation(context, poisonCloud, targetPosition);
                    createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                }

                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);

                break;
            }

            default:
            {
                logWarning("Trying to cast wrong spell: %d\n", spellType);
                break;
            }
        }

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }
}

void freeCastState(WarState* state)
{
}