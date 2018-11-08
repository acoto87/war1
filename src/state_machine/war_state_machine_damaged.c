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

            WarSprite sprite = createSpriteFromResourceIndex(context, BUILDING_DAMAGE_2_RESOURCE);
            WarSpriteAnimation* anim = createAnimation("hugeDamage", sprite, 0.2f, true);
            anim->offset = vec2Subv(getUnitSpriteCenter(entity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));
            
            for(s32 i = 0; i < 4; i++)
                addAnimationFrame(anim, i);
            
            addAnimation(entity, anim);
        }
    }
    else if(hpPercent <= 66)
    {
        if (!containsAnimation(context, entity, "littleDamage"))
        {
            WarSprite sprite = createSpriteFromResourceIndex(context, BUILDING_DAMAGE_1_RESOURCE);
            WarSpriteAnimation* anim = createAnimation("littleDamage", sprite, 0.2f, true);
            anim->offset = vec2Subv(getUnitSpriteCenter(entity), vec2i(halfi(sprite.frameWidth), sprite.frameHeight));
            
            for(s32 i = 0; i < 4; i++)
                addAnimationFrame(anim, i);
            
            addAnimation(entity, anim);
        }
    }
}

void freeDamagedState(WarState* state)
{
}