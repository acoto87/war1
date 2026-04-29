#include "war_commands.h"

#include <assert.h>
#include <stdlib.h>

#include "war_audio.h"
#include "war_entities.h"
#include "war_projectiles.h"
#include "war_state_machine.h"
#include "war_ui.h"
#include "war_units.h"

void wcomm_executeMoveCommand(WarContext* context, vec2 targetPoint)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarPlayerInfo* player = &map->players[0];

    bool goingToMove = false;

    s32 selEntitiesCount = map->selectedEntities.count;

    // wcomm_move the selected units to the target point,
    // but keeping the bounding box that the
    // selected units make, this is an intent to keep the
    // formation of the selected units
    //
    rect* rs = (rect*)wm_allocFrame(selEntitiesCount * sizeof(rect));

    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        rs[i] = wu_getUnitRect(entity);
    }

    rect bbox = rs[0];

    for(s32 i = 1; i < selEntitiesCount; i++)
    {
        if (rs[i].x < bbox.x)
            bbox.x = rs[i].x;
        if (rs[i].y < bbox.y)
            bbox.y = rs[i].y;
        if (rs[i].x + rs[i].width > bbox.x + bbox.width)
            bbox.width = (rs[i].x + rs[i].width) - bbox.x;
        if (rs[i].y + rs[i].height > bbox.y + bbox.height)
            bbox.height = (rs[i].y + rs[i].height) - bbox.y;
    }

    rect targetbbox = rectf(
        targetPoint.x - halff(bbox.width),
        targetPoint.y - halff(bbox.height),
        bbox.width,
        bbox.height);

    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        vec2 position = vec2f(
            rs[i].x + halff(rs[i].width),
            rs[i].y + halff(rs[i].height));

        position = wmap_vec2MapToTileCoordinates(position);

        rect targetRect = rectf(
            targetbbox.x + (rs[i].x - bbox.x),
            targetbbox.y + (rs[i].y - bbox.y),
            rs[i].width,
            rs[i].height);

        vec2 target = vec2f(
            targetRect.x + halff(targetRect.width),
            targetRect.y + halff(targetRect.height));

        target = wmap_vec2MapToTileCoordinates(target);

        if (wu_isDudeUnit(entity) && wu_isFriendlyUnit(context, entity))
        {
            if (isKeyPressed(input, WAR_KEY_SHIFT))
            {
                if (isPatrolling(entity))
                {
                    if(isMoving(entity))
                    {
                        WarState* moveState = getMoveState(entity);
                        vec2ListAdd(&moveState->wcomm_move.positions, target);
                    }

                    WarState* patrolState = getPatrolState(entity);
                    vec2ListAdd(&patrolState->patrol.positions, target);
                }
                else if(isMoving(entity) && !isAttacking(entity))
                {
                    WarState* moveState = getMoveState(entity);
                    vec2ListAdd(&moveState->wcomm_move.positions, target);
                }
                else
                {
                    WarState* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, target));
                    wst_changeNextState(context, entity, moveState, true, true);
                }
            }
            else
            {
                WarState* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, target));
                wst_changeNextState(context, entity, moveState, true, true);

                // WarState* patrolState = wst_createPatrolState(context, entity, 2, arrayArg(vec2, position, target));
                // wst_changeNextState(context, entity, patrolState, true, true);
            }

            goingToMove = true;
        }
    }

    if (goingToMove)
    {
        wa_playAcknowledgementSound(context, player);
    }
}

void wcomm_executeFollowCommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool goingToFollow = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for (s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity))
        {
            WarState* followState = wst_createFollowState(context, entity, targetEntity->id, VEC2_ZERO, 1);
            wst_changeNextState(context, entity, followState, true, true);

            goingToFollow = true;
        }
    }

    if (goingToFollow)
    {
        wa_playAcknowledgementSound(context, player);
    }
}

void wcomm_executeStopCommand(WarContext* context)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for (s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }
    }
}

void wcomm_executeHarvestCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    assert(isUnitOfType(targetEntity, WAR_UNIT_GOLDMINE) ||
           isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST));

    bool goingToHarvest = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity))
        {
            if (wu_isWorkerUnit(entity))
            {
                if (wu_isCarryingResources(entity))
                {
                    // find the closest town hall to wcomm_deliver the gold
                    WarRace race = wu_getUnitRace(entity);
                    WarUnitType townHallType = wu_getTownHallOfRace(race);
                    WarEntity* townHall = we_findClosestUnitOfType(context, entity, townHallType);
                    if (townHall)
                    {
                        WarState* deliverState = wst_createDeliverState(context, entity, townHall->id);
                        deliverState->nextState = isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST)
                            ? wst_createGatherWoodState(context, entity, targetEntity->id, targetTile)
                            : wst_createGatherGoldState(context, entity, targetEntity->id);

                        wst_changeNextState(context, entity, deliverState, true, true);
                    }
                }
                else
                {
                    WarState* gatherGoldOrWoodState = isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST)
                        ? wst_createGatherWoodState(context, entity, targetEntity->id, targetTile)
                        : wst_createGatherGoldState(context, entity, targetEntity->id);

                    wst_changeNextState(context, entity, gatherGoldOrWoodState, true, true);
                }

                goingToHarvest = true;
            }
            else if (wu_isDudeUnit(entity))
            {
                vec2 position = wu_getUnitCenterPosition(entity, true);
                WarState* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position,  targetTile));
                wst_changeNextState(context, entity, moveState, true, true);

                goingToHarvest = true;
            }
        }
    }

    if (goingToHarvest)
    {
        wa_playAcknowledgementSound(context, player);
    }
}

void wcomm_executeDeliverCommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool goingToDeliver = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity))
        {
            WarEntity* townHall = targetEntity;
            if (!townHall)
            {
                WarRace race = wu_getUnitRace(entity);
                WarUnitType townHallType = wu_getTownHallOfRace(race);
                townHall = we_findClosestUnitOfType(context, entity, townHallType);
                assert(townHall);
            }

            if (wu_isWorkerUnit(entity) && wu_isCarryingResources(entity))
            {
                WarState* deliverState = wst_createDeliverState(context, entity, townHall->id);
                wst_changeNextState(context, entity, deliverState, true, true);

                goingToDeliver = true;
            }
            else if (wu_isDudeUnit(entity))
            {
                WarState* followState = wst_createFollowState(context, entity, townHall->id, VEC2_ZERO, 1);
                wst_changeNextState(context, entity, followState, true, true);

                goingToDeliver = true;
            }
        }
    }

    if (goingToDeliver)
    {
        wa_playAcknowledgementSound(context, player);
    }
}

void wcomm_executeRepairCommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool goingToRepair = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity))
        {
            // the unit can't wcomm_repair itself
            if (entity->id == targetEntity->id)
            {
                continue;
            }

            if (wu_isWorkerUnit(entity))
            {
                WarState* repairState = wst_createRepairState(context, entity, targetEntity->id);
                wst_changeNextState(context, entity, repairState, true, true);

                goingToRepair = true;
            }
        }
    }

    if (goingToRepair)
    {
        wa_playAcknowledgementSound(context, player);
    }
}

void wcomm_executeSummonCommand(WarContext* context, WarUnitCommandType summonType)
{
    WarMap* map = context->map;

    bool casted = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isConjurerOrWarlockUnit(entity))
        {
            WarUnitComponent* unit = &entity->unit;

            // when the unit summon another unit, it is not invisible anymore
            unit->invisible = false;
            unit->invisibilityTime = 0;

            WarUnitCommandMapping commandMapping = wu_getCommandMapping(summonType);
            WarSpellMapping spellMapping = wu_getSpellMapping(commandMapping.mappedType);
            WarSpellStats stats = wu_getSpellStats(commandMapping.mappedType);

            while (we_decreaseUnitMana(context, entity, stats.manaCost))
            {
                vec2 position = wu_getUnitCenterPosition(entity, true);
                vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);

                WarEntity* summonedUnit = we_createUnit(context, spellMapping.mappedType,
                                                     (s32)spawnPosition.x, (s32)spawnPosition.y,
                                                     unit->player, WAR_RESOURCE_NONE, 0, true);

                vec2 unitSize = wu_getUnitSize(summonedUnit);
                setStaticEntity(map->finder, (s32)spawnPosition.x, (s32)spawnPosition.y,
                                (s32)unitSize.x, (s32)unitSize.y, summonedUnit->id);

                WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                wanim_createSpellAnimation(context, animEntity, wmap_vec2TileToMapCoordinates(spawnPosition, true));

                casted = true;
            }
        }
    }

    if (casted)
    {
        wa_createAudio(context, WAR_NORMAL_SPELL, false);
    }
}

void wcomm_executeRainOfFireCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isConjurerOrWarlockUnit(entity))
        {
            WarState* castState = wst_createCastState(context, entity, WAR_SPELL_RAIN_OF_FIRE, 0, targetTile);
            wst_changeNextState(context, entity, castState, true, true);
        }
    }
}

void wcomm_executePoisonCloudCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isConjurerOrWarlockUnit(entity))
        {
            WarState* castState = wst_createCastState(context, entity, WAR_SPELL_POISON_CLOUD, 0, targetTile);
            wst_changeNextState(context, entity, castState, true, true);
        }
    }
}

void wcomm_executeHealingCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;

    if (targetEntity && wu_isDudeUnit(targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = we_findEntity(context, entityId);
            assert(entity);

            if (wu_isClericOrNecrolyteUnit(entity))
            {
                // the unit can't heal itself
                if (entity->id != targetEntity->id)
                {
                    WarState* castState = wst_createCastState(context, entity, WAR_SPELL_HEALING, targetEntity->id, targetTile);
                    wst_changeNextState(context, entity, castState, true, true);
                }
            }
        }
    }
}

void wcomm_executeInvisiblityCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;

    if (targetEntity && wu_isDudeUnit(targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = we_findEntity(context, entityId);
            assert(entity);

            if (wu_isClericOrNecrolyteUnit(entity))
            {
                WarState* castState = wst_createCastState(context, entity, WAR_SPELL_INVISIBILITY, targetEntity->id, targetTile);
                wst_changeNextState(context, entity, castState, true, true);
            }
        }
    }
}

void wcomm_executeUnholyArmorCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;

    if (targetEntity && wu_isDudeUnit(targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = we_findEntity(context, entityId);
            assert(entity);

            if (wu_isClericOrNecrolyteUnit(entity))
            {
                WarState* castState = wst_createCastState(context, entity, WAR_SPELL_UNHOLY_ARMOR, targetEntity->id, targetTile);
                wst_changeNextState(context, entity, castState, true, true);
            }
        }
    }
}

void wcomm_executeRaiseDeadCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isClericOrNecrolyteUnit(entity))
        {
            WarState* castState = wst_createCastState(context, entity, WAR_SPELL_RAISE_DEAD, 0, targetTile);
            wst_changeNextState(context, entity, castState, true, true);
        }
    }
}

void wcomm_executeSightCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isClericOrNecrolyteUnit(entity))
        {
            WarSpellType spellType = isHumanUnit(entity) ? WAR_SPELL_FAR_SIGHT : WAR_SPELL_DARK_VISION;
            WarState* castState = wst_createCastState(context, entity, spellType, 0, targetTile);
            wst_changeNextState(context, entity, castState, true, true);
        }
    }
}

void wcomm_executeAttackCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool playSound = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity))
        {
            if (targetEntity)
            {
                // the unit can't wcomm_attack itself
                if (entity->id != targetEntity->id)
                {
                    if (wu_canAttack(context, entity, targetEntity))
                    {
                        WarState* attackState = wst_createAttackState(context, entity, targetEntity->id, targetTile);
                        wst_changeNextState(context, entity, attackState, true, true);

                        playSound = true;
                    }
                    else if (wu_isWorkerUnit(entity))
                    {
                        WarState* followState = wst_createFollowState(context, entity, targetEntity->id, VEC2_ZERO, 1);
                        wst_changeNextState(context, entity, followState, true, true);
                    }
                }
            }
            else
            {
                WarState* attackState = wst_createAttackState(context, entity, 0, targetTile);
                wst_changeNextState(context, entity, attackState, true, true);

                playSound = true;
            }
        }
    }

    if (playSound)
    {
        wa_playAcknowledgementSound(context, player);
    }
}

bool wcomm_executeCommand(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarPlayerInfo* player = &map->players[0];
    WarUnitCommand* command = &map->command;

    if (command->type == WAR_COMMAND_NONE)
    {
        return false;
    }

    switch (command->type)
    {
        case WAR_COMMAND_MOVE:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);

                    wcomm_executeMoveCommand(context, targetPoint);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    vec2 targetPoint = wmap_vec2TileToMapCoordinates(targetTile, true);
                    wcomm_executeMoveCommand(context, targetPoint);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_STOP:
        {
            wcomm_executeStopCommand(context);

            command->type = WAR_COMMAND_NONE;
            return true;
        }

        case WAR_COMMAND_HARVEST:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                    if (targetEntity)
                    {
                        if (isUnitOfType(targetEntity, WAR_UNIT_GOLDMINE))
                        {
                            if (!isUnitUnknown(map, targetEntity))
                                wcomm_executeHarvestCommand(context, targetEntity, targetTile);
                            else
                                wcomm_executeMoveCommand(context, targetPoint);
                        }
                        else if (isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST))
                        {
                            if (!isTileUnkown(map, (s32)targetTile.x, (s32)targetTile.y))
                            {
                                wcomm_executeHarvestCommand(context, targetEntity, targetTile);
                            }
                            else
                            {
                                WarTree* tree = we_findAccesibleTree(context, targetEntity, targetTile);
                                if (tree)
                                {
                                    targetTile = vec2i(tree->tilex, tree->tiley);
                                    wcomm_executeHarvestCommand(context, targetEntity, targetTile);
                                }
                                else
                                {
                                    wcomm_executeMoveCommand(context, targetPoint);
                                }
                            }
                        }
                    }

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_DELIVER:
        {
            wcomm_executeDeliverCommand(context, NULL);

            command->type = WAR_COMMAND_NONE;
            return true;
        }

        case WAR_COMMAND_REPAIR:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);
                    if (isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y) ||
                        isTileFog(map, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        WarEntityId targetEntityId = getTileEntityId(map->finder, (s32)targetTile.x, (s32)targetTile.y);
                        WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                        if (targetEntity && wu_isBuildingUnit(targetEntity))
                        {
                            wcomm_executeRepairCommand(context, targetEntity);
                        }
                    }

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_ATTACK:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                    if (targetEntity)
                    {
                        if (isUnit(targetEntity))
                        {
                            // if the target entity is not visible or partially visible, just wcomm_attack to the point
                            if (isUnitUnknown(map, targetEntity))
                                targetEntity = NULL;
                        }
                        else if (isWall(targetEntity))
                        {
                            // if the target wall piece is not visible, just wcomm_attack to the point
                            if (!isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y))
                                targetEntity = NULL;
                        }
                    }

                    wcomm_executeAttackCommand(context, targetEntity, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    wcomm_executeAttackCommand(context, NULL, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_TRAIN_FOOTMAN:
        case WAR_COMMAND_TRAIN_GRUNT:
        case WAR_COMMAND_TRAIN_PEASANT:
        case WAR_COMMAND_TRAIN_PEON:
        case WAR_COMMAND_TRAIN_CATAPULT_HUMANS:
        case WAR_COMMAND_TRAIN_CATAPULT_ORCS:
        case WAR_COMMAND_TRAIN_KNIGHT:
        case WAR_COMMAND_TRAIN_RAIDER:
        case WAR_COMMAND_TRAIN_ARCHER:
        case WAR_COMMAND_TRAIN_SPEARMAN:
        case WAR_COMMAND_TRAIN_CONJURER:
        case WAR_COMMAND_TRAIN_WARLOCK:
        case WAR_COMMAND_TRAIN_CLERIC:
        case WAR_COMMAND_TRAIN_NECROLYTE:
        {
            WarUnitType unitToTrain = command->train.unitToTrain;
            WarUnitType buildingUnit = command->train.buildingUnit;

            assert(map->selectedEntities.count == 1);

            WarEntity* selectedEntity = we_findEntity(context, map->selectedEntities.items[0]);
            assert(selectedEntity && wu_isBuildingUnit(selectedEntity));
            assert(selectedEntity->unit.type == buildingUnit);

            WarUnitStats stats = wu_getUnitStats(unitToTrain);
            if (we_checkFarmFood(context, player) &&
                we_decreasePlayerResources(context, player, stats.goldCost, stats.woodCost))
            {
                WarState* trainState = wst_createTrainState(context, selectedEntity, unitToTrain, (f32)stats.buildTime);
                wst_changeNextState(context, selectedEntity, trainState, true, true);
            }

            command->type = WAR_COMMAND_NONE;
            return true;
        }

        case WAR_COMMAND_UPGRADE_SWORDS:
        case WAR_COMMAND_UPGRADE_AXES:
        case WAR_COMMAND_UPGRADE_SHIELD_HUMANS:
        case WAR_COMMAND_UPGRADE_SHIELD_ORCS:
        case WAR_COMMAND_UPGRADE_ARROWS:
        case WAR_COMMAND_UPGRADE_SPEARS:
        case WAR_COMMAND_UPGRADE_HORSES:
        case WAR_COMMAND_UPGRADE_WOLVES:
        case WAR_COMMAND_UPGRADE_SCORPION:
        case WAR_COMMAND_UPGRADE_SPIDER:
        case WAR_COMMAND_UPGRADE_RAIN_OF_FIRE:
        case WAR_COMMAND_UPGRADE_POISON_CLOUD:
        case WAR_COMMAND_UPGRADE_WATER_ELEMENTAL:
        case WAR_COMMAND_UPGRADE_DAEMON:
        case WAR_COMMAND_UPGRADE_HEALING:
        case WAR_COMMAND_UPGRADE_RAISE_DEAD:
        case WAR_COMMAND_UPGRADE_FAR_SIGHT:
        case WAR_COMMAND_UPGRADE_DARK_VISION:
        case WAR_COMMAND_UPGRADE_INVISIBILITY:
        case WAR_COMMAND_UPGRADE_UNHOLY_ARMOR:
        {
            WarUpgradeType upgradeToBuild = command->upgrade.upgradeToBuild;
            WarUnitType buildingUnit = command->upgrade.buildingUnit;

            assert(map->selectedEntities.count == 1);

            WarEntity* selectedEntity = we_findEntity(context, map->selectedEntities.items[0]);
            assert(selectedEntity && wu_isBuildingUnit(selectedEntity));
            assert(selectedEntity->unit.type == buildingUnit);

            assert(hasRemainingUpgrade(player, upgradeToBuild));

            WarUpgradeStats stats = wu_getUpgradeStats(upgradeToBuild);
            s32 level = getUpgradeLevel(player, upgradeToBuild);
            if (we_decreasePlayerResources(context, player, stats.goldCost[level], 0))
            {
                WarState* upgradeState = wst_createUpgradeState(context, selectedEntity, upgradeToBuild, (f32)stats.buildTime);
                wst_changeNextState(context, selectedEntity, upgradeState, true, true);
            }

            command->type = WAR_COMMAND_NONE;
            return true;
        }

        case WAR_COMMAND_BUILD_FARM_HUMANS:
        case WAR_COMMAND_BUILD_FARM_ORCS:
        case WAR_COMMAND_BUILD_BARRACKS_HUMANS:
        case WAR_COMMAND_BUILD_BARRACKS_ORCS:
        case WAR_COMMAND_BUILD_CHURCH:
        case WAR_COMMAND_BUILD_TEMPLE:
        case WAR_COMMAND_BUILD_TOWER_HUMANS:
        case WAR_COMMAND_BUILD_TOWER_ORCS:
        case WAR_COMMAND_BUILD_TOWNHALL_HUMANS:
        case WAR_COMMAND_BUILD_TOWNHALL_ORCS:
        case WAR_COMMAND_BUILD_LUMBERMILL_HUMANS:
        case WAR_COMMAND_BUILD_LUMBERMILL_ORCS:
        case WAR_COMMAND_BUILD_STABLE:
        case WAR_COMMAND_BUILD_KENNEL:
        case WAR_COMMAND_BUILD_BLACKSMITH_HUMANS:
        case WAR_COMMAND_BUILD_BLACKSMITH_ORCS:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    assert(map->selectedEntities.count > 0);

                    WarEntityId workerId = map->selectedEntities.items[0];
                    WarEntity* worker = we_findEntity(context, workerId);
                    assert(worker);

                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    WarUnitType buildingToBuild = command->build.buildingToBuild;

                    WarBuildingStats stats = wu_getBuildingStats(buildingToBuild);
                    if (we_checkTileToBuild(context, buildingToBuild, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        if (we_decreasePlayerResources(context, player, stats.goldCost, stats.woodCost))
                        {
                            WarEntity* building = we_createBuilding(context, buildingToBuild, (s32)targetTile.x, (s32)targetTile.y, 0, true);
                            WarState* repairState = wst_createRepairState(context, worker, building->id);
                            wst_changeNextState(context, worker, repairState, true, true);

                            command->type = WAR_COMMAND_NONE;
                        }
                    }
                    else
                    {
                        wa_createAudio(context, WAR_UI_CANCEL, false);
                    }

                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_BUILD_WALL:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    assert(map->selectedEntities.count > 0);

                    WarEntityId townHallId = map->selectedEntities.items[0];
                    WarEntity* townHall = we_findEntity(context, townHallId);

                    WarUnitType townHallType = wu_getTownHallOfRace(player->race);
                    assert(isUnitOfType(townHall, townHallType));

                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    if (we_checkTileToBuildRoadOrWall(context, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        if (we_decreasePlayerResources(context, player, WAR_WALL_GOLD_COST, WAR_WALL_WOOD_COST))
                        {
                            WarEntity* wall = map->wall;
                            WarWallPiece* piece = we_addWallPiece(wall, (s32)targetTile.x, (s32)targetTile.y, 0);
                            piece->hp = WAR_WALL_MAX_HP;
                            piece->maxhp = WAR_WALL_MAX_HP;

                            we_determineWallTypes(context, wall);

                            // don't reset the current command if the player is building
                            // roads or walls, to allow rapid construction of those structures
                            //
                            // command->type = WAR_COMMAND_NONE;

                            wa_createAudio(context, WAR_BUILD_ROAD, false);
                        }
                    }
                    else
                    {
                        wa_createAudio(context, WAR_UI_CANCEL, false);
                    }

                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_BUILD_ROAD:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if (rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    assert(map->selectedEntities.count > 0);

                    WarEntityId townHallId = map->selectedEntities.items[0];
                    WarEntity* townHall = we_findEntity(context, townHallId);

                    WarUnitType townHallType = wu_getTownHallOfRace(player->race);
                    assert(isUnitOfType(townHall, townHallType));

                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    if (we_checkTileToBuildRoadOrWall(context, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        if (we_decreasePlayerResources(context, player, WAR_ROAD_GOLD_COST, WAR_ROAD_WOOD_COST))
                        {
                            WarEntity* road = map->road;
                            we_addRoadPiece(road, (s32)targetTile.x, (s32)targetTile.y, 0);

                            we_determineRoadTypes(context, road);

                            // don't reset the current command if the player is building
                            // roads or walls, to allow rapid construction of those structures
                            //
                            // command->type = WAR_COMMAND_NONE;

                            wa_createAudio(context, WAR_BUILD_ROAD, false);
                        }
                    }
                    else
                    {
                        wa_createAudio(context, WAR_UI_CANCEL, false);
                    }

                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SUMMON_SPIDER:
        case WAR_COMMAND_SUMMON_SCORPION:
        case WAR_COMMAND_SUMMON_DAEMON:
        case WAR_COMMAND_SUMMON_WATER_ELEMENTAL:
        {
            wcomm_executeSummonCommand(context, command->type);

            command->type = WAR_COMMAND_NONE;
            return true;
        }

        case WAR_COMMAND_SPELL_RAIN_OF_FIRE:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    wcomm_executeRainOfFireCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    wcomm_executeRainOfFireCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_POISON_CLOUD:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    wcomm_executePoisonCloudCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    wcomm_executePoisonCloudCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_HEALING:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);

                    wcomm_executeHealingCommand(context, targetEntity, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_INVISIBILITY:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);

                    wcomm_executeInvisiblityCommand(context, targetEntity, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_UNHOLY_ARMOR:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);

                    wcomm_executeUnholyArmorCommand(context, targetEntity, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_RAISE_DEAD:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    wcomm_executeRaiseDeadCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_SPELL_FAR_SIGHT:
        case WAR_COMMAND_SPELL_DARK_VISION:
        {
            if (wasButtonPressed(input, WAR_MOUSE_LEFT))
            {
                if(rectContainsf(map->mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    wcomm_executeSightCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    wcomm_executeSightCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_BUILD_BASIC:
        case WAR_COMMAND_BUILD_ADVANCED:
        {
            // do nothing here
            break;
        }

        default:
        {
            logError("Not implemented command: %d", command->type);
            return false;
        }
    }

    return false;
}

// train units
void trainUnit(WarContext* context, WarUnitCommandType commandType, WarUnitType unitToTrain, WarUnitType buildingUnit)
{
    WarMap* map = context->map;

    map->command.type = commandType;
    map->command.train.unitToTrain = unitToTrain;
    map->command.train.buildingUnit = buildingUnit;
}

void wcomm_trainFootman(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_FOOTMAN, WAR_UNIT_FOOTMAN, WAR_UNIT_BARRACKS_HUMANS);
}

void wcomm_trainGrunt(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_GRUNT, WAR_UNIT_GRUNT, WAR_UNIT_BARRACKS_ORCS);
}

void wcomm_trainPeasant(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_PEASANT, WAR_UNIT_PEASANT, WAR_UNIT_TOWNHALL_HUMANS);
}

void wcomm_trainPeon(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_PEON, WAR_UNIT_PEON, WAR_UNIT_TOWNHALL_ORCS);
}

void wcomm_trainHumanCatapult(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_CATAPULT_HUMANS, WAR_UNIT_CATAPULT_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void wcomm_trainOrcCatapult(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_CATAPULT_ORCS, WAR_UNIT_CATAPULT_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void wcomm_trainKnight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_KNIGHT, WAR_UNIT_KNIGHT, WAR_UNIT_BARRACKS_HUMANS);
}

void wcomm_trainRaider(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_RAIDER, WAR_UNIT_RAIDER, WAR_UNIT_BARRACKS_ORCS);
}

void wcomm_trainArcher(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_ARCHER, WAR_UNIT_ARCHER, WAR_UNIT_BARRACKS_HUMANS);
}

void wcomm_trainSpearman(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_SPEARMAN, WAR_UNIT_SPEARMAN, WAR_UNIT_BARRACKS_ORCS);
}

void wcomm_trainConjurer(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_CONJURER, WAR_UNIT_CONJURER, WAR_UNIT_TOWER_HUMANS);
}

void wcomm_trainWarlock(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_WARLOCK, WAR_UNIT_WARLOCK, WAR_UNIT_TOWER_ORCS);
}

void wcomm_trainCleric(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_CLERIC, WAR_UNIT_CLERIC, WAR_UNIT_CHURCH);
}

void wcomm_trainNecrolyte(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_NECROLYTE, WAR_UNIT_NECROLYTE, WAR_UNIT_TEMPLE);
}

// upgrades
void upgradeUpgrade(WarContext* context, WarUnitCommandType commandType, WarUpgradeType upgradeToBuild, WarUnitType buildingUnit)
{
    WarMap* map = context->map;

    map->command.type = commandType;
    map->command.upgrade.upgradeToBuild = upgradeToBuild;
    map->command.upgrade.buildingUnit = buildingUnit;
}

void wcomm_upgradeSwords(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SWORDS, WAR_UPGRADE_SWORDS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void wcomm_upgradeAxes(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_AXES, WAR_UPGRADE_AXES, WAR_UNIT_BLACKSMITH_ORCS);
}

void wcomm_upgradeHumanShields(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SHIELD_HUMANS, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_HUMANS);
}

void wcomm_upgradeOrcsShields(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SHIELD_ORCS, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_ORCS);
}

void wcomm_upgradeArrows(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_ARROWS, WAR_UPGRADE_ARROWS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void wcomm_upgradeSpears(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SPEARS, WAR_UPGRADE_SPEARS, WAR_UNIT_LUMBERMILL_ORCS);
}

void wcomm_upgradeHorses(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_HORSES, WAR_UPGRADE_HORSES, WAR_UNIT_STABLE);
}

void wcomm_upgradeWolves(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_WOLVES, WAR_UPGRADE_WOLVES, WAR_UNIT_KENNEL);
}

void wcomm_upgradeScorpions(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SCORPION, WAR_UPGRADE_SCORPIONS, WAR_UNIT_TOWER_HUMANS);
}

void wcomm_upgradeSpiders(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SPIDER, WAR_UPGRADE_SPIDERS, WAR_UNIT_TOWER_ORCS);
}

void wcomm_upgradeRainOfFire(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_RAIN_OF_FIRE, WAR_UPGRADE_RAIN_OF_FIRE, WAR_UNIT_TOWER_HUMANS);
}

void wcomm_upgradePoisonCloud(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_POISON_CLOUD, WAR_UPGRADE_POISON_CLOUD, WAR_UNIT_TOWER_ORCS);
}

void wcomm_upgradeWaterElemental(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_WATER_ELEMENTAL, WAR_UPGRADE_WATER_ELEMENTAL, WAR_UNIT_TOWER_HUMANS);
}

void wcomm_upgradeDaemon(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_DAEMON, WAR_UPGRADE_DAEMON, WAR_UNIT_TOWER_ORCS);
}

void wcomm_upgradeHealing(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_HEALING, WAR_UPGRADE_HEALING, WAR_UNIT_CHURCH);
}

void wcomm_upgradeRaiseDead(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_RAISE_DEAD, WAR_UPGRADE_RAISE_DEAD, WAR_UNIT_TEMPLE);
}

void wcomm_upgradeFarSight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_FAR_SIGHT, WAR_UPGRADE_FAR_SIGHT, WAR_UNIT_CHURCH);
}

void wcomm_upgradeDarkVision(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_DARK_VISION, WAR_UPGRADE_DARK_VISION, WAR_UNIT_TEMPLE);
}

void wcomm_upgradeInvisibility(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_INVISIBILITY, WAR_UPGRADE_INVISIBILITY, WAR_UNIT_CHURCH);
}

void wcomm_upgradeUnholyArmor(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_UNHOLY_ARMOR, WAR_UPGRADE_UNHOLY_ARMOR, WAR_UNIT_TEMPLE);
}

// wcomm_cancel
void wcomm_cancel(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    map->command.type = WAR_COMMAND_NONE;

    for (s32 i = 0; i < map->selectedEntities.count; i++)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[i];
        WarEntity* selectedEntity = we_findEntity(context, selectedEntityId);

        if (wu_isBuildingUnit(selectedEntity))
        {
            if (isBuilding(selectedEntity) || isGoingToBuild(selectedEntity))
            {
                WarBuildingStats stats = wu_getBuildingStats(selectedEntity->unit.type);
                we_increasePlayerResources(context, player, stats.goldCost, stats.woodCost);

                WarState* collapseState = wst_createCollapseState(context, selectedEntity);
                wst_changeNextState(context, selectedEntity, collapseState, true, true);

                wa_createAudioRandom(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, false);
            }
            else if (selectedEntity->unit.building)
            {
                if (isTraining(selectedEntity) || isGoingToTrain(selectedEntity))
                {
                    WarState* trainState = getTrainState(selectedEntity);
                    WarUnitType unitToBuild = trainState->train.unitToBuild;

                    WarUnitStats stats = wu_getUnitStats(unitToBuild);
                    we_increasePlayerResources(context, player, stats.goldCost, stats.woodCost);
                }
                else if (isUpgrading(selectedEntity) || isGoingToUpgrade(selectedEntity))
                {
                    WarState* upgradeState = getUpgradeState(selectedEntity);
                    WarUpgradeType upgradeToBuild = upgradeState->upgrade.upgradeToBuild;
                    assert(hasRemainingUpgrade(player, upgradeToBuild));

                    s32 upgradeLevel = getUpgradeLevel(player, upgradeToBuild);
                    WarUpgradeStats stats = wu_getUpgradeStats(upgradeToBuild);
                    we_increasePlayerResources(context, player, stats.goldCost[upgradeLevel], 0);
                }

                WarState* idleState = wst_createIdleState(context, entity, false);
                wst_changeNextState(context, selectedEntity, idleState, true, true);
            }
        }
    }
}

// basic
void wcomm_move(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_MOVE;
}

void wcomm_stop(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_STOP;
}

void wcomm_harvest(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_HARVEST;
}

void wcomm_deliver(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_DELIVER;
}

void wcomm_repair(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_REPAIR;
}

void wcomm_attack(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_ATTACK;
}

void wcomm_buildBasic(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_BASIC;
}

void wcomm_buildAdvanced(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_ADVANCED;
}

void buildBuilding(WarContext* context, WarUnitCommandType commandType, WarUnitType buildingToBuild)
{
    WarMap* map = context->map;

    map->command.type = commandType;
    map->command.build.buildingToBuild = buildingToBuild;
}

void wcomm_buildFarmHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_FARM_HUMANS, WAR_UNIT_FARM_HUMANS);
}

void wcomm_buildFarmOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_FARM_ORCS, WAR_UNIT_FARM_ORCS);
}

void wcomm_buildBarracksHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_BARRACKS_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void wcomm_buildBarracksOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_BARRACKS_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void wcomm_buildChurch(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_CHURCH, WAR_UNIT_CHURCH);
}

void wcomm_buildTemple(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TEMPLE, WAR_UNIT_TEMPLE);
}

void wcomm_buildTowerHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TOWER_HUMANS, WAR_UNIT_TOWER_HUMANS);
}

void wcomm_buildTowerOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TOWER_ORCS, WAR_UNIT_TOWER_ORCS);
}

void wcomm_buildTownHallHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TOWNHALL_HUMANS, WAR_UNIT_TOWNHALL_HUMANS);
}

void wcomm_buildTownHallOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TOWNHALL_ORCS, WAR_UNIT_TOWNHALL_ORCS);
}

void wcomm_buildLumbermillHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_LUMBERMILL_HUMANS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void wcomm_buildLumbermillOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_LUMBERMILL_ORCS, WAR_UNIT_LUMBERMILL_ORCS);
}

void wcomm_buildStable(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_STABLE, WAR_UNIT_STABLE);
}

void wcomm_buildKennel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_KENNEL, WAR_UNIT_KENNEL);
}

void wcomm_buildBlacksmithHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_BLACKSMITH_HUMANS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void wcomm_buildBlacksmithOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_BLACKSMITH_ORCS, WAR_UNIT_BLACKSMITH_ORCS);
}

void wcomm_buildWall(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_WALL;
}

void wcomm_buildRoad(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_ROAD;
}

// spells
void wcomm_castRainOfFire(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_RAIN_OF_FIRE;
}

void wcomm_castPoisonCloud(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_POISON_CLOUD;
}

void wcomm_castHeal(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_HEALING;
}

void wcomm_castFarSight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_FAR_SIGHT;
}

void wcomm_castDarkVision(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_DARK_VISION;
}

void wcomm_castInvisibility(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_INVISIBILITY;
}

void wcomm_castUnHolyArmor(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_UNHOLY_ARMOR;
}

void wcomm_castRaiseDead(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_RAISE_DEAD;
}

// summons
void wcomm_summonSpider(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_SPIDER;
}

void wcomm_summonScorpion(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_SCORPION;
}

void wcomm_summonDaemon(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_DAEMON;
}

void wcomm_summonWaterElemental(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_WATER_ELEMENTAL;
}
