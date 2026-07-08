#include "war_state_machine.h"

#include "war_actions.h"
#include "war_map.h"
#include "war_units.h"

#include "TracyC.h"

WarStateIdle* wst_createIdleState(WarContext* context, WarEntity* entity, bool lookAround)
{
    TracyCZoneN(ctx, "wst_createIdleState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_IDLE, entity->id);
    WarStateIdle* state = (WarStateIdle*)wst_deref(context, ref);
    state->lookAround = lookAround;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_leaveIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_leaveIdleState", true);

    if (!state->initialized)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    if (wu_isUnit(entity))
    {
        WarMap* map = context->map;
        vec2 unitSize = wu_getUnitSize(context, entity);
        vec2 position = wu_getUnitPosition(context, entity, true);
        setFreeTiles(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y);
    }

    TracyCZoneEnd(ctx);
}

void wst_updateIdleState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateIdleState", true);

    WarMap* map = context->map;

    WarStateIdle* s = (WarStateIdle*)state;

    if (!state->initialized)
    {
        if (wu_isUnit(entity))
        {
            vec2 unitSize = wu_getUnitSize(context, entity);
            vec2 position = wu_getUnitPosition(context, entity, true);
            setStaticEntity(&map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
            wact_setAction(context, entity, WAR_ACTION_TYPE_IDLE, true, 1.0f);
        }
        state->initialized = true;
    }

    if (wu_isUnit(entity))
    {
        if (s->lookAround)
        {
            if (chance(20))
            {
                WarUnitComponent* unit = we_getUnitComponent(context, entity);
                assert(unit);

                unit->direction += randomi(-1, 2);
                if (unit->direction < 0)
                    unit->direction = WAR_DIRECTION_NORTH_WEST;
                else if(unit->direction >= WAR_DIRECTION_COUNT)
                    unit->direction = WAR_DIRECTION_NORTH;
            }
        }

        // look for foe units to attack them if they are in range
        if (wu_isWarriorUnit(context, entity))
        {
            WarEntity* enemy = we_getNearEnemy(context, entity);
            if (enemy)
            {
                vec2 enemyPosition = wu_getUnitPosition(context, enemy, true);
                WarStateAttack* attackState = wst_createAttackState(context, entity, enemy->id, enemyPosition);
                wst_replaceState(context, entity, (WarStateBase*)attackState);
            }
        }

        // this is a way to tell the state machine engine to not update this state for the specified amount of time
        state->delay = 1.0f;
    }
    else if(wu_isWall(entity))
    {
        WarWallComponent* wall = we_getWallComponent(context, entity);
        assert(wall);

        for(s32 i = 0; i < wall->pieces.count; i++)
        {
            WarWallPiece* piece = &wall->pieces.items[i];
            s32 hpPercent = PERCENTABI(piece->hp, piece->maxhp);
            if (hpPercent <= 0)
                setFreeTiles(&map->finder, piece->tilex, piece->tiley, 1, 1);
            else
                setStaticEntity(&map->finder, piece->tilex, piece->tiley, 1, 1, entity->id);
        }
    }

    TracyCZoneEnd(ctx);
}

void wst_updateIdleStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateIdleStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateIdle*     states  = storage->idle;
    bool*             occupied = storage->occupied[WAR_STATE_IDLE];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateIdle* state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!we_isComponentEnabled(context, entity, COMP_STATE_MACHINE)) continue;
        WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
        assert(sm);

        if (sm->depth == 0 || sm->stack[sm->depth - 1].type != WAR_STATE_IDLE || sm->stack[sm->depth - 1].idx != i) continue;

        if (state->base.delay > 0)
        {
            state->base.nextUpdateGameTime = context->gameTime + state->base.delay;
            state->base.delay = 0;
        }
        if (context->gameTime < state->base.nextUpdateGameTime) continue;

        wst_updateIdleState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}

