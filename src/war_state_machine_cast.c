WarState* createCastState(WarContext* context, WarEntity* entity, vec2 targetTile)
{
    WarState* state = createState(context, entity, WAR_STATE_CAST);
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

    vec2 targetTile = state->cast.targetTile;
    
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
        WarSpellStats stats = getSpellStats(WAR_SPELL_RAIN_OF_FIRE);
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

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }
}

void freeCastState(WarState* state)
{
}