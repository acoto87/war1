#include "war_state_machine.h"

#include "war_actions.h"
#include "war_animations.h"
#include "war_audio.h"
#include "war_projectiles.h"
#include "war_units.h"
#include "war_map.h"

#include "TracyC.h"

WarStateCast* wst_createCastState(WarContext* context, WarEntity* entity, WarSpellType spellType, WarEntityId targetEntityId, vec2 targetPosition)
{
    TracyCZoneN(ctx, "wst_createCastState", true);

    WarStateRef ref = wst_allocState(context, WAR_STATE_CAST, entity->id);
    if (!WAR_STATE_REF_IS_VALID(ref))
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarStateCast* state = (WarStateCast*)wst_deref(context, ref);
    if (!state)
    {
        TracyCZoneEnd(ctx);
        return NULL;
    }

    state->spellType = spellType;
    state->targetEntityId = targetEntityId;
    state->targetPosition = targetPosition;

    TracyCZoneEnd(ctx);
    return state;
}

void wst_updateCastState(WarContext* context, WarEntity* entity, WarState* state)
{
    TracyCZoneN(ctx, "wst_updateCastState", true);

    WarStateCast* s = (WarStateCast*)state;

    WarMap* map = context->map;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    vec2 unitTile = wu_getUnitCenterTile(context, entity);
    vec2 unitSize = wu_getUnitSize(context, entity);

    WarSpellType spellType = s->spellType;
    WarEntityId targetEntityId = s->targetEntityId;
    vec2 targetPosition = s->targetPosition;
    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPosition);

    const WarSpellStats* stats = wu_getSpellStats(spellType);

    if (stats->range)
    {
        if(!wu_tileInRange(context, entity, targetTile, stats->range))
        {
            WarStateFollow* followState = wst_createFollowState(context, entity, targetEntityId, targetPosition, stats->range * MEGA_TILE_WIDTH);
            wst_pushState(context, entity, (WarStateBase*)followState, WAR_TRANSITION_CAUSE_COMPLETION);
            TracyCZoneEnd(ctx);
            return;
        }
    }

    wpath_setStaticEntity(&map->finder, (s32)unitTile.x, (s32)unitTile.y, (s32)unitSize.x, (s32)unitSize.y, entity->id);
    wu_setUnitDirectionFromDiff(context, entity, targetTile.x - unitTile.x, targetTile.y - unitTile.y);
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
                if (targetEntity && wu_isDudeUnit(context, targetEntity))
                {
                    WarUnitComponent* targetUnit = we_getUnitComponent(context, targetEntity);
                    assert(targetUnit);

                    // the healing spell's strength is determined by units of mana.
                    // for every 6 units of mana, the damaged unit gets back 1 hit point.
                    //
                    // take all the hp the cleric can restore according to its mana
                    s32 hpToRestore = unit->mana / stats->manaCost;

                    // take in reality how much hp needs to be restored
                    hpToRestore = MIN(hpToRestore, targetUnit->maxhp - targetUnit->hp);

                    // recalculate how much mana the cleric need to spend
                    s32 manaToSpend = hpToRestore * stats->manaCost;

                    we_increaseUnitHp(context, targetEntity, hpToRestore);
                    we_decreaseUnitMana(context, entity, manaToSpend);

                    vec2 targetEntityPosition = wu_getUnitCenterPosition(context, targetEntity);

                    WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                    we_addAnimationsComponent(context, animEntity);

                    wanim_createSpellAnimation(context, animEntity, targetEntityPosition);
                    wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_NORMAL_SPELL, .position=targetEntityPosition, .hasPosition=true, .loop=false));
                }

                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);

                break;
            }

            case WAR_SPELL_FAR_SIGHT:
            case WAR_SPELL_DARK_VISION:
            {
                if (we_decreaseUnitMana(context, entity, stats->manaCost))
                {
                    vec2 targetEntityPosition = wmap_tileToMapCoordinatesV(targetTile, true);

                    WarEntity* sight = we_createEntity(context, WAR_ENTITY_TYPE_SIGHT, true);
                    we_addSightComponent(context, sight, WAR_SIGHT_COMPONENT_INIT(
                        .position = targetTile,
                        .time     = stats->time,
                    ));
                    we_addAnimationsComponent(context, sight);

                    wanim_createSpellAnimation(context, sight, targetEntityPosition);
                    wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_NORMAL_SPELL, .position=targetEntityPosition, .hasPosition=true, .loop=false));
                }

                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);

                break;
            }

            case WAR_SPELL_INVISIBILITY:
            {
                WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                if (targetEntity && wu_isDudeUnit(context, targetEntity))
                {
                    WarUnitComponent* targetUnit = we_getUnitComponent(context, targetEntity);
                    assert(targetUnit);

                    if (we_decreaseUnitMana(context, entity, stats->manaCost))
                    {
                        targetUnit->invisible = true;
                        targetUnit->invisibilityTime = stats->time;

                        vec2 targetEntityPosition = wu_getUnitCenterPosition(context, targetEntity);

                        WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                        we_addAnimationsComponent(context, animEntity);

                        wanim_createSpellAnimation(context, animEntity, targetEntityPosition);
                        wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_NORMAL_SPELL, .position=targetEntityPosition, .hasPosition=true, .loop=false));
                    }
                }

                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);

                break;
            }

            case WAR_SPELL_RAIN_OF_FIRE:
            {
                if (we_decreaseUnitMana(context, entity, stats->manaCost))
                {
                    vec2 targetEntityPosition = wmap_tileToMapCoordinatesV(targetTile, true);
                    s32 radius = 2 * MEGA_TILE_WIDTH;

                    s32 projectilesCount = 5;
                    while (projectilesCount--)
                    {
                        f32 offsetx = randomf(-radius, radius);
                        f32 offsety = randomf(-radius, radius);
                        vec2 target = vec2_addv(targetEntityPosition, vec2f(offsetx, offsety));

                        offsety = randomf(MEGA_TILE_WIDTH, MEGA_TILE_WIDTH * 4);
                        vec2 origin = vec2f(target.x, map->camera.viewport.y - offsety);

                        wproj_createProjectile(context, WAR_PROJECTILE_RAIN_OF_FIRE, 0, 0, origin, target);
                    }
                }
                else
                {
                    WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                    wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
                }

                break;
            }

            case WAR_SPELL_RAISE_DEAD:
            {
                WarEntityList* nearUnits = (WarEntityList*)wm_allocFrame(sizeof(WarEntityList));
                WarEntityListInit(nearUnits, wm_frameAllocator());
                we_getNearUnits(context, targetTile, 4, nearUnits);

                for (s32 i = 0; i < nearUnits->count; i++)
                {
                    WarEntity* targetEntity = nearUnits->items[i];
                    if (targetEntity && wu_isCorpseUnit(context, targetEntity))
                    {
                        if (we_decreaseUnitMana(context, entity, stats->manaCost))
                        {
                            vec2 targetEntityTile = wu_getUnitCenterTile(context, targetEntity);
                            vec2 targetEntityPosition = wu_getUnitCenterPosition(context, targetEntity);

                            WarEntity* skeleton = we_createUnit(context, CREATE_UNIT_ARGS_INIT(
                                .type=WAR_UNIT_SKELETON,
                                .x=(s32)targetEntityTile.x,
                                .y=(s32)targetEntityTile.y,
                                .player=unit->player,
                                .resourceKind=WAR_RESOURCE_NONE,
                                .amount=0,
                                .addToMap=true
                            ));
                            we_setInitialIdleState(context, skeleton);

                            WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                            we_addAnimationsComponent(context, animEntity);

                            wanim_createSpellAnimation(context, animEntity, targetEntityPosition);
                            wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_NORMAL_SPELL, .position=targetEntityPosition, .hasPosition=true, .loop=false));

                            we_removeEntityById(context, targetEntity->id);
                        }
                    }
                }

                WarEntityListFree(nearUnits);

                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);
                break;
            }

            case WAR_SPELL_UNHOLY_ARMOR:
            {
                WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                if (targetEntity && wu_isDudeUnit(context, targetEntity))
                {
                    WarUnitComponent* targetUnit = we_getUnitComponent(context, targetEntity);
                    assert(targetUnit);

                    if (we_decreaseUnitMana(context, entity, stats->manaCost))
                    {
                        we_decreaseUnitHp(context, targetEntity, targetUnit->hp/2);

                        targetUnit->invulnerable = true;
                        targetUnit->invulnerabilityTime = stats->time;

                        vec2 targetEntityPosition = wu_getUnitCenterPosition(context, targetEntity);

                        WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                        we_addAnimationsComponent(context, animEntity);

                        wanim_createSpellAnimation(context, animEntity, targetEntityPosition);
                        wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_NORMAL_SPELL, .position=targetEntityPosition, .hasPosition=true, .loop=false));
                    }
                }

                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);

                break;
            }

            case WAR_SPELL_POISON_CLOUD:
            {
                if (we_decreaseUnitMana(context, entity, stats->manaCost))
                {
                    vec2 targetEntityPosition = wmap_tileToMapCoordinatesV(targetTile, true);

                    WarEntity* poisonCloud = we_createEntity(context, WAR_ENTITY_TYPE_POISON_CLOUD, true);
                    we_addPoisonCloudComponent(context, poisonCloud, WAR_POISON_CLOUD_COMPONENT_INIT(
                        .tile = targetTile,
                        .time     = stats->time,
                    ));
                    we_addAnimationsComponent(context, poisonCloud);

                    wanim_createPoisonCloudAnimation(context, poisonCloud, targetEntityPosition);
                    wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_NORMAL_SPELL, .position=targetEntityPosition, .hasPosition=true, .loop=false));
                }

                WarStateIdle* idleState = wst_createIdleState(context, entity, true);
                wst_replaceState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_COMPLETION);

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

    TracyCZoneEnd(ctx);
}


void wst_updateCastStates(WarContext* context)
{
    TracyCZoneN(ctx, "wst_updateCastStates", true);

    WarEntityManager* manager = we_getEntityManager(context);
    WarStateStorage*  storage = &manager->stateStorage;
    WarStateCast*      states  = storage->cast;
    bool*             occupied = storage->occupied[WAR_STATE_CAST];

    for (s32 i = 0; i < MAX_STATES_PER_TYPE; i++)
    {
        if (!occupied[i]) continue;

        WarStateCast*  state  = &states[i];
        WarEntity*    entity = we_findEntity(context, state->base.entityId);
        if (!entity) continue;

        if (!wst_isCurrentState(context, entity, (WarStateBase*)state)) continue;
        if (!wst_isNextUpdateTime(context, (WarStateBase*)state)) continue;

        wst_updateCastState(context, entity, (WarStateBase*)state);
    }

    TracyCZoneEnd(ctx);
}
