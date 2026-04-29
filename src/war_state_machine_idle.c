#include "war_state_machine.h"

#include "war_actions.h"
#include "war_map.h"
#include "war_units.h"

WarState* wst_createIdleState(WarContext* context, WarEntity* entity, bool lookAround)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_IDLE);
    state->idle.lookAround = lookAround;
    return state;
}

void wst_enterIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    if (isUnit(entity))
    {
        WarMap* map = context->map;
        vec2 unitSize = wu_getUnitSize(entity);
        vec2 position = wu_getUnitPosition(entity, true);
        setStaticEntity(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
        wact_setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, 1.0f);
    }
}

void wst_leaveIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(state);

    if (isUnit(entity))
    {
        WarMap* map = context->map;
        vec2 unitSize = wu_getUnitSize(entity);
        vec2 position = wu_getUnitPosition(entity, true);
        setFreeTiles(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    }
}

void wst_updateIdleState(WarContext* context, WarEntity* entity, WarState* state)
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

        // look for foe units to wcmd_attack them if they are in range
        if (wu_isWarriorUnit(entity))
        {
            WarEntity* enemy = we_getNearEnemy(context, entity);
            if (enemy)
            {
                vec2 enemyPosition = wu_getUnitPosition(enemy, true);
                WarState* attackState = wst_createAttackState(context, entity, enemy->id, enemyPosition);
                wst_changeNextState(context, entity, attackState, true, true);
            }
        }

        // this is a way to tell the state machine engine to not update this state for the specified amount of time
        state->delay = 1.0f;
    }
    else if(isWall(entity))
    {
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

void wst_freeIdleState(WarContext* context, WarState* state)
{
    NOT_USED(state);
}
