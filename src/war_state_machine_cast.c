WarState* createCastState(WarContext* context, WarEntity* entity, WarUnitCommandType spellType, vec2 targetTile, bool loop)
{
    WarState* state = createState(context, entity, WAR_STATE_CAST);
    state->cast.spellType = spellType;
    state->cast.targetTile = targetTile;
    state->cast.loop = loop;
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

    vec2 targetTile = state->cast.targetTile;
    WarUnitCommandType spellType = state->cast.spellType;
    bool loop = state->cast.loop;
    
    if(!entityTilePositionInRange(entity, targetTile, 12))
    {
        WarState* moveState = createMoveState(context, entity, 2, arrayArg(vec2, position, targetTile));
        moveState->nextState = state;

        // do not check for attacks here because if 
        // the unit gets attacked it will comeback to this state
        // and try to attack with it, instead of a regular attack
        moveState->move.checkForAttacks = false;
        changeNextState(context, entity, moveState, false, true);
        return;
    }

    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
    setUnitDirectionFromDiff(entity, targetTile.x - position.x, targetTile.y - position.y);
    setAction(context, entity, WAR_ACTION_TYPE_ATTACK, false, 1.0f);

    WarUnitAction* action = unit->actions.items[unit->actionIndex];
    if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
    {
        WarSpellStats stats = getSpellStats(spellType);
        if (decreaseUnitMana(context, entity, stats.manaCost))
        {
            switch (spellType)
            {
                case WAR_COMMAND_SPELL_RAIN_OF_FIRE:
                {
                    WarEntity* rainOfFire = createEntity(context, WAR_ENTITY_TYPE_RAIN_OF_FIRE, true);
                    addRainOfFireComponent(context, rainOfFire, targetTile);
                    break;
                }

                case WAR_COMMAND_SPELL_POISON_CLOUD:
                {
                    WarEntity* poisonCloud = createEntity(context, WAR_ENTITY_TYPE_POISON_CLOUD, true);
                    addPoisonCloudComponent(context, poisonCloud, targetTile);

                    vec2 targetTilePosition = vec2TileToMapCoordinates(targetTile, true);
                    sprintf(poisonCloud->poisonCloud.animName, "poison_cloud_%.2f_%.2f", targetTilePosition.x, targetTilePosition.y);
                    createPoisonCloudAnimation(context, targetTilePosition, poisonCloud->poisonCloud.animName);
                    break;
                }

                default:
                {
                    logWarning("Trying to cast a spell with typ: %d\n", spellType);
                    break;
                }
            }

            // if the spell caster doesn't stick there casting, then go idle
            // (e.g. Warlocks cast the poison cloud and then go away, but the Conjurer
            // sticks casting the rain of fire until it rans out of mana or a different
            // order is given to it)
            if (!loop)
            {
                WarState* idleState = createIdleState(context, entity, true);
                changeNextState(context, entity, idleState, true, true);
            }
        }
        else
        {
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }
}

void freeCastState(WarState* state)
{
}