#include "war_state_machine.h"

#include "war_actions.h"
#include "war_animations.h"
#include "war_audio.h"
#include "war_projectiles.h"
#include "war_units.h"
#include "war_map.h"

WarState* wst_createCastState(WarContext* context, WarEntity* entity, WarSpellType spellType, WarEntityId targetEntityId, vec2 targetTile)
{
    WarState* state = wst_createState(context, entity, WAR_STATE_CAST);
    state->cast.spellType = spellType;
    state->cast.targetEntityId = targetEntityId;
    state->cast.targetTile = targetTile;
    return state;
}

void wst_enterCastState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_leaveCastState(WarContext* context, WarEntity* entity, WarState* state)
{
    NOT_USED(context);
    NOT_USED(entity);
    NOT_USED(state);
}

void wst_updateCastState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;

    vec2 unitSize = wu_getUnitSize(entity);
    vec2 position = wmap_mapToTileCoordinatesV(entity->transform.position);

    WarSpellType spellType = state->cast.spellType;
    WarEntityId targetEntityId = state->cast.targetEntityId;
    vec2 targetTile = state->cast.targetTile;

    WarSpellStats stats = wu_getSpellStats(spellType);

    if (stats.range)
    {
        if(!wu_tileInRange(entity, targetTile, stats.range))
        {
            WarState* followState = wst_createFollowState(context, entity, targetEntityId, targetTile, stats.range);
            followState->nextState = state;
            wst_changeNextState(context, entity, followState, false, true);
            return;
        }
    }

    setStaticEntity(map->finder, (s32)position.x, (s32)position.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
    wu_setUnitDirectionFromDiff(entity, targetTile.x - position.x, targetTile.y - position.y);
    wact_setAction(context, entity, WAR_ACTION_TYPE_ATTACK, false, 1.0f);

    WarUnitAction* action = &unit->actions[unit->actionType];
    if (action->lastActionStep == WAR_ACTION_STEP_ATTACK)
    {
        // when the unit cast a spell, it is not invisible anymore
        unit->invisible = false;
        unit->invisibilityTime = 0;

        switch (spellType)
        {
            case WAR_SPELL_HEALING:
            {
                WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                if (targetEntity && wu_isDudeUnit(targetEntity))
                {
                    WarUnitComponent* targetUnit = &targetEntity->unit;

                    // the healing spell's strength is determined by units of mana.
                    // for every 6 units of mana, the damaged unit gets back 1 hit point.
                    //
                    // take all the hp the cleric can restore according to its mana
                    s32 hpToRestore = unit->mana / stats.manaCost;

                    // take in reality how much hp needs to be restored
                    hpToRestore = MIN(hpToRestore, targetUnit->maxhp - targetUnit->hp);

                    // recalculate how much mana the cleric need to spend
                    s32 manaToSpend = hpToRestore * stats.manaCost;

                    we_increaseUnitHp(context, targetEntity, hpToRestore);
                    we_decreaseUnitMana(context, entity, manaToSpend);

                    vec2 targetPosition = wu_getUnitCenterPosition(targetEntity, false);

                    WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                    we_addAnimationsComponent(context, animEntity);

                    wanim_createSpellAnimation(context, animEntity, targetPosition);
                    wa_createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                }

                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_FAR_SIGHT:
            case WAR_SPELL_DARK_VISION:
            {
                if (we_decreaseUnitMana(context, entity, stats.manaCost))
                {
                    vec2 targetPosition = wmap_tileToMapCoordinatesV(targetTile, true);

                    WarEntity* sight = we_createEntity(context, WAR_ENTITY_TYPE_SIGHT, true);
                    we_addSightComponent(context, sight, targetTile, stats.time);
                    we_addAnimationsComponent(context, sight);

                    wanim_createSpellAnimation(context, sight, targetPosition);
                    wa_createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                }

                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_INVISIBILITY:
            {
                WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                if (targetEntity && wu_isDudeUnit(targetEntity))
                {
                    WarUnitComponent* targetUnit = &targetEntity->unit;

                    if (we_decreaseUnitMana(context, entity, stats.manaCost))
                    {
                        targetUnit->invisible = true;
                        targetUnit->invisibilityTime = stats.time;

                        vec2 targetPosition = wu_getUnitCenterPosition(targetEntity, false);

                        WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                        we_addAnimationsComponent(context, animEntity);

                        wanim_createSpellAnimation(context, animEntity, targetPosition);
                        wa_createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                    }
                }

                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_RAIN_OF_FIRE:
            {
                if (we_decreaseUnitMana(context, entity, stats.manaCost))
                {
                    vec2 targetTilePosition = wmap_tileToMapCoordinatesV(targetTile, true);
                    s32 radius = 2 * MEGA_TILE_WIDTH;

                    s32 projectilesCount = 5;
                    while (projectilesCount--)
                    {
                        f32 offsetx = randomf(-radius, radius);
                        f32 offsety = randomf(-radius, radius);
                        vec2 target = vec2_addv(targetTilePosition, vec2f(offsetx, offsety));

                        offsety = randomf(MEGA_TILE_WIDTH, MEGA_TILE_WIDTH * 4);
                        vec2 origin = vec2f(target.x, map->viewport.y - offsety);

                        wproj_createProjectile(context, WAR_PROJECTILE_RAIN_OF_FIRE, 0, 0, origin, target);
                    }
                }
                else
                {
                    WarState* idleState = wst_createIdleState(context, entity, true);
                    wst_changeNextState(context, entity, idleState, true, true);
                }

                break;
            }

            case WAR_SPELL_RAISE_DEAD:
            {
                WarEntityList* nearUnits = we_getNearUnits(context, targetTile, 4);
                for (s32 i = 0; i < nearUnits->count; i++)
                {
                    WarEntity* targetEntity = nearUnits->items[i];
                    if (targetEntity && wu_isCorpseUnit(targetEntity))
                    {
                        if (we_decreaseUnitMana(context, entity, stats.manaCost))
                        {
                            vec2 targetPosition = wu_getUnitCenterPosition(targetEntity, true);
                            we_createUnit(context, WAR_UNIT_SKELETON, (s32)targetPosition.x, (s32)targetPosition.y,
                                       unit->player, WAR_RESOURCE_NONE, 0, true);

                            targetPosition = wu_getUnitCenterPosition(targetEntity, false);

                            WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                            we_addAnimationsComponent(context, animEntity);

                            wanim_createSpellAnimation(context, animEntity, targetPosition);
                            wa_createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);

                            we_removeEntityById(context, targetEntity->id);
                        }
                    }
                }
                WarEntityListFree(nearUnits);

                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_UNHOLY_ARMOR:
            {
                WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                if (targetEntity && wu_isDudeUnit(targetEntity))
                {
                    WarUnitComponent* targetUnit = &targetEntity->unit;

                    if (we_decreaseUnitMana(context, entity, stats.manaCost))
                    {
                        we_decreaseUnitHp(context, targetEntity, targetUnit->hp/2);

                        targetUnit->invulnerable = true;
                        targetUnit->invulnerabilityTime = stats.time;

                        vec2 targetPosition = wu_getUnitCenterPosition(targetEntity, false);

                        WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                        we_addAnimationsComponent(context, animEntity);

                        wanim_createSpellAnimation(context, animEntity, targetPosition);
                        wa_createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                    }
                }

                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);

                break;
            }

            case WAR_SPELL_POISON_CLOUD:
            {
                if (we_decreaseUnitMana(context, entity, stats.manaCost))
                {
                    vec2 targetPosition = wmap_tileToMapCoordinatesV(targetTile, true);

                    WarEntity* poisonCloud = we_createEntity(context, WAR_ENTITY_TYPE_POISON_CLOUD, true);
                    we_addPoisonCloudComponent(context, poisonCloud, targetTile, stats.time);
                    we_addAnimationsComponent(context, poisonCloud);

                    wanim_createPoisonCloudAnimation(context, poisonCloud, targetPosition);
                    wa_createAudioWithPosition(context, WAR_NORMAL_SPELL, targetPosition, false);
                }

                WarState* idleState = wst_createIdleState(context, entity, true);
                wst_changeNextState(context, entity, idleState, true, true);

                break;
            }

            default:
            {
                logWarning("Trying to cast wrong spell: %d", spellType);
                break;
            }
        }

        // this is not the more elegant solution, but the actions and the state machine have to comunicate somehow
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep =  WAR_ACTION_STEP_NONE;
    }
}

void wst_freeCastState(WarContext* context, WarState* state)
{
    NOT_USED(context);
    NOT_USED(state);
}
