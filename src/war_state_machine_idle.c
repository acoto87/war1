WarState* createIdleState(WarContext* context, WarEntity* entity, bool lookAround)
{
    WarState* state = createState(context, entity, WAR_STATE_IDLE);
    state->idle.lookAround = lookAround;
    return state;
}

void enterIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (isUnit(entity))
    {
        WarMap* map = context->map;
        vec2 unitSize = getUnitSize(entity);
        vec2 position = getUnitPosition(entity, true);
        setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
        setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, 1.0f);
    }
}

void leaveIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    if (isUnit(entity))
    {
        WarMap* map = context->map;
        vec2 unitSize = getUnitSize(entity);
        vec2 position = getUnitPosition(entity, true);
        setFreeTiles(map->finder, position.x, position.y, unitSize.x, unitSize.y);
    }
}

void updateIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    if (isUnit(entity))
    {
        if (state->idle.lookAround)
        {
            if (chance(20))
            {
                WarUnitComponent* unit = &entity->unit;
                unit->direction += randomi(-1, 2);
                if (unit->direction < 0)
                    unit->direction = WAR_DIRECTION_NORTH_WEST;
                else if(unit->direction >= WAR_DIRECTION_COUNT)
                    unit->direction = WAR_DIRECTION_NORTH;
            }
        }

        // look for foe units to attack them if they are in range
        //
        if (isWarriorUnit(entity))
        {
            vec2 position = getUnitCenterPosition(entity, true);

            for(s32 i = 0; i < map->entities.count; i++)
            {
                WarEntity* other = map->entities.items[i];
                if (other && isUnit(other) && isEnemy(context, entity, other))
                {
                    vec2 targetPosition = getUnitCenterPosition(other, true);
                    if (vec2Distance(position, targetPosition) < 5)
                    {
                        WarState* attackState = createAttackState(context, entity, other->id, targetPosition);
                        changeNextState(context, entity, attackState, true, true);

                        break;
                    }
                }
            }
        }

        // this is a way to tell the state machine engine to not update this state for the specified amount of time
        state->delay = 1.0f;
    }
    else if(isWall(entity))
    {
        WarMap* map = context->map;
        WarWallComponent* wall = &entity->wall;
        for(s32 i = 0; i < wall->pieces.count; i++)
        {
            WarWallPiece* piece = &wall->pieces.items[i];
            s32 hpPercent = percentabi(piece->hp, piece->maxhp);
            if (hpPercent <= 0)
                setFreeTiles(map->finder, piece->tilex, piece->tiley, 1, 1);
            else
                setStaticEntity(map->finder, piece->tilex, piece->tiley, 1, 1, entity->id);
        }
    }
}

void freeIdleState(WarState* state)
{
}