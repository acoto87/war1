#include "../war_state_machine.h"

WarState* createDamagedState(WarContext* context, WarEntity* entity)
{
    WarState* state = createState(context, entity, WAR_STATE_DAMAGED);
    return state;
}

void enterDamagedState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    setStaticEntity(map->finder, position.x, position.y, unitSize.x, unitSize.y, entity->id);
}

void leaveDamagedState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateDamagedState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarUnitComponent* unit = &entity->unit;

    s32 hpPercent = percentabi(unit->hp, unit->maxhp);
    if(hpPercent <= 33)
    {
        if (!containsAnimation(context, entity, "hugeDamage"))
        {
            removeAnimation(context, entity, "littleDamage");
            createDamageAnimation(context, entity, "hugeDamage", 2);
        }
    }
    else if(hpPercent <= 66)
    {
        if (!containsAnimation(context, entity, "littleDamage"))
        {
            createDamageAnimation(context, entity, "littleDamage", 1);
        }
    }
}

void freeDamagedState(WarState* state)
{
}