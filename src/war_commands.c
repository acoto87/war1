#include "war_commands.h"

#include <assert.h>
#include <stdlib.h>

#include "war_audio.h"
#include "war_entities.h"
#include "war_projectiles.h"
#include "war_state_machine.h"
#include "war_ui.h"
#include "war_units.h"

void wcmd_executeMoveCommand(WarContext* context, vec2 targetPoint)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarPlayerInfo* player = &map->players[0];

    bool goingToMove = false;

    s32 selEntitiesCount = map->selectedEntities.count;

    // wcmd_move the selected units to the target point,
    // but keeping the bounding box that the
    // selected units make, this is an intent to keep the
    // formation of the selected units
    //
    rect* rs = (rect*)war_malloc_frame(selEntitiesCount * sizeof(rect));

    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        rs[i] = wun_getUnitRect(entity);
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
        WarEntity* entity = went_findEntity(context, entityId);
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

        if (wun_isDudeUnit(entity) && wun_isFriendlyUnit(context, entity))
        {
            if (isKeyPressed(input, WAR_KEY_SHIFT))
            {
                if (isPatrolling(entity))
                {
                    if(isMoving(entity))
                    {
                        WarState* moveState = getMoveState(entity);
                        vec2ListAdd(&moveState->wcmd_move.positions, target);
                    }

                    WarState* patrolState = getPatrolState(entity);
                    vec2ListAdd(&patrolState->patrol.positions, target);
                }
                else if(isMoving(entity) && !isAttacking(entity))
                {
                    WarState* moveState = getMoveState(entity);
                    vec2ListAdd(&moveState->wcmd_move.positions, target);
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
        waud_playAcknowledgementSound(context, player);
    }
}

void wcmd_executeFollowCommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool goingToFollow = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for (s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isFriendlyUnit(context, entity))
        {
            WarState* followState = wst_createFollowState(context, entity, targetEntity->id, VEC2_ZERO, 1);
            wst_changeNextState(context, entity, followState, true, true);

            goingToFollow = true;
        }
    }

    if (goingToFollow)
    {
        waud_playAcknowledgementSound(context, player);
    }
}

void wcmd_executeStopCommand(WarContext* context)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for (s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isFriendlyUnit(context, entity))
        {
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }
    }
}

void wcmd_executeHarvestCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
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
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isFriendlyUnit(context, entity))
        {
            if (wun_isWorkerUnit(entity))
            {
                if (wun_isCarryingResources(entity))
                {
                    // find the closest town hall to wcmd_deliver the gold
                    WarRace race = wun_getUnitRace(entity);
                    WarUnitType townHallType = wun_getTownHallOfRace(race);
                    WarEntity* townHall = went_findClosestUnitOfType(context, entity, townHallType);
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
            else if (wun_isDudeUnit(entity))
            {
                vec2 position = wun_getUnitCenterPosition(entity, true);
                WarState* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position,  targetTile));
                wst_changeNextState(context, entity, moveState, true, true);

                goingToHarvest = true;
            }
        }
    }

    if (goingToHarvest)
    {
        waud_playAcknowledgementSound(context, player);
    }
}

void wcmd_executeDeliverCommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool goingToDeliver = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isFriendlyUnit(context, entity))
        {
            WarEntity* townHall = targetEntity;
            if (!townHall)
            {
                WarRace race = wun_getUnitRace(entity);
                WarUnitType townHallType = wun_getTownHallOfRace(race);
                townHall = went_findClosestUnitOfType(context, entity, townHallType);
                assert(townHall);
            }

            if (wun_isWorkerUnit(entity) && wun_isCarryingResources(entity))
            {
                WarState* deliverState = wst_createDeliverState(context, entity, townHall->id);
                wst_changeNextState(context, entity, deliverState, true, true);

                goingToDeliver = true;
            }
            else if (wun_isDudeUnit(entity))
            {
                WarState* followState = wst_createFollowState(context, entity, townHall->id, VEC2_ZERO, 1);
                wst_changeNextState(context, entity, followState, true, true);

                goingToDeliver = true;
            }
        }
    }

    if (goingToDeliver)
    {
        waud_playAcknowledgementSound(context, player);
    }
}

void wcmd_executeRepairCommand(WarContext* context, WarEntity* targetEntity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool goingToRepair = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isFriendlyUnit(context, entity))
        {
            // the unit can't wcmd_repair itself
            if (entity->id == targetEntity->id)
            {
                continue;
            }

            if (wun_isWorkerUnit(entity))
            {
                WarState* repairState = wst_createRepairState(context, entity, targetEntity->id);
                wst_changeNextState(context, entity, repairState, true, true);

                goingToRepair = true;
            }
        }
    }

    if (goingToRepair)
    {
        waud_playAcknowledgementSound(context, player);
    }
}

void wcmd_executeSummonCommand(WarContext* context, WarUnitCommandType summonType)
{
    WarMap* map = context->map;

    bool casted = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isConjurerOrWarlockUnit(entity))
        {
            WarUnitComponent* unit = &entity->unit;

            // when the unit summon another unit, it is not invisible anymore
            unit->invisible = false;
            unit->invisibilityTime = 0;

            WarUnitCommandMapping commandMapping = wun_getCommandMapping(summonType);
            WarSpellMapping spellMapping = wun_getSpellMapping(commandMapping.mappedType);
            WarSpellStats stats = wun_getSpellStats(commandMapping.mappedType);

            while (went_decreaseUnitMana(context, entity, stats.manaCost))
            {
                vec2 position = wun_getUnitCenterPosition(entity, true);
                vec2 spawnPosition = wpath_findEmptyPosition(map->finder, position);

                WarEntity* summonedUnit = went_createUnit(context, spellMapping.mappedType,
                                                     (s32)spawnPosition.x, (s32)spawnPosition.y,
                                                     unit->player, WAR_RESOURCE_NONE, 0, true);

                vec2 unitSize = wun_getUnitSize(summonedUnit);
                setStaticEntity(map->finder, (s32)spawnPosition.x, (s32)spawnPosition.y,
                                (s32)unitSize.x, (s32)unitSize.y, summonedUnit->id);

                WarEntity* animEntity = went_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                wani_createSpellAnimation(context, animEntity, wmap_vec2TileToMapCoordinates(spawnPosition, true));

                casted = true;
            }
        }
    }

    if (casted)
    {
        waud_createAudio(context, WAR_NORMAL_SPELL, false);
    }
}

void wcmd_executeRainOfFireCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isConjurerOrWarlockUnit(entity))
        {
            WarState* castState = wst_createCastState(context, entity, WAR_SPELL_RAIN_OF_FIRE, 0, targetTile);
            wst_changeNextState(context, entity, castState, true, true);
        }
    }
}

void wcmd_executePoisonCloudCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isConjurerOrWarlockUnit(entity))
        {
            WarState* castState = wst_createCastState(context, entity, WAR_SPELL_POISON_CLOUD, 0, targetTile);
            wst_changeNextState(context, entity, castState, true, true);
        }
    }
}

void wcmd_executeHealingCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;

    if (targetEntity && wun_isDudeUnit(targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = went_findEntity(context, entityId);
            assert(entity);

            if (wun_isClericOrNecrolyteUnit(entity))
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

void wcmd_executeInvisiblityCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;

    if (targetEntity && wun_isDudeUnit(targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = went_findEntity(context, entityId);
            assert(entity);

            if (wun_isClericOrNecrolyteUnit(entity))
            {
                WarState* castState = wst_createCastState(context, entity, WAR_SPELL_INVISIBILITY, targetEntity->id, targetTile);
                wst_changeNextState(context, entity, castState, true, true);
            }
        }
    }
}

void wcmd_executeUnholyArmorCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;

    if (targetEntity && wun_isDudeUnit(targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = went_findEntity(context, entityId);
            assert(entity);

            if (wun_isClericOrNecrolyteUnit(entity))
            {
                WarState* castState = wst_createCastState(context, entity, WAR_SPELL_UNHOLY_ARMOR, targetEntity->id, targetTile);
                wst_changeNextState(context, entity, castState, true, true);
            }
        }
    }
}

void wcmd_executeRaiseDeadCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isClericOrNecrolyteUnit(entity))
        {
            WarState* castState = wst_createCastState(context, entity, WAR_SPELL_RAISE_DEAD, 0, targetTile);
            wst_changeNextState(context, entity, castState, true, true);
        }
    }
}

void wcmd_executeSightCommand(WarContext* context, vec2 targetTile)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isClericOrNecrolyteUnit(entity))
        {
            WarSpellType spellType = isHumanUnit(entity) ? WAR_SPELL_FAR_SIGHT : WAR_SPELL_DARK_VISION;
            WarState* castState = wst_createCastState(context, entity, spellType, 0, targetTile);
            wst_changeNextState(context, entity, castState, true, true);
        }
    }
}

void wcmd_executeAttackCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    bool playSound = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = went_findEntity(context, entityId);
        assert(entity);

        if (wun_isFriendlyUnit(context, entity))
        {
            if (targetEntity)
            {
                // the unit can't wcmd_attack itself
                if (entity->id != targetEntity->id)
                {
                    if (wun_canAttack(context, entity, targetEntity))
                    {
                        WarState* attackState = wst_createAttackState(context, entity, targetEntity->id, targetTile);
                        wst_changeNextState(context, entity, attackState, true, true);

                        playSound = true;
                    }
                    else if (wun_isWorkerUnit(entity))
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
        waud_playAcknowledgementSound(context, player);
    }
}

bool wcmd_executeCommand(WarContext* context)
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

                    wcmd_executeMoveCommand(context, targetPoint);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    vec2 targetPoint = wmap_vec2TileToMapCoordinates(targetTile, true);
                    wcmd_executeMoveCommand(context, targetPoint);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
            }

            return false;
        }

        case WAR_COMMAND_STOP:
        {
            wcmd_executeStopCommand(context);

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
                    WarEntity* targetEntity = went_findEntity(context, targetEntityId);
                    if (targetEntity)
                    {
                        if (isUnitOfType(targetEntity, WAR_UNIT_GOLDMINE))
                        {
                            if (!isUnitUnknown(map, targetEntity))
                                wcmd_executeHarvestCommand(context, targetEntity, targetTile);
                            else
                                wcmd_executeMoveCommand(context, targetPoint);
                        }
                        else if (isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST))
                        {
                            if (!isTileUnkown(map, (s32)targetTile.x, (s32)targetTile.y))
                            {
                                wcmd_executeHarvestCommand(context, targetEntity, targetTile);
                            }
                            else
                            {
                                WarTree* tree = went_findAccesibleTree(context, targetEntity, targetTile);
                                if (tree)
                                {
                                    targetTile = vec2i(tree->tilex, tree->tiley);
                                    wcmd_executeHarvestCommand(context, targetEntity, targetTile);
                                }
                                else
                                {
                                    wcmd_executeMoveCommand(context, targetPoint);
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
            wcmd_executeDeliverCommand(context, NULL);

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
                        WarEntity* targetEntity = went_findEntity(context, targetEntityId);
                        if (targetEntity && wun_isBuildingUnit(targetEntity))
                        {
                            wcmd_executeRepairCommand(context, targetEntity);
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
                    WarEntity* targetEntity = went_findEntity(context, targetEntityId);
                    if (targetEntity)
                    {
                        if (isUnit(targetEntity))
                        {
                            // if the target entity is not visible or partially visible, just wcmd_attack to the point
                            if (isUnitUnknown(map, targetEntity))
                                targetEntity = NULL;
                        }
                        else if (isWall(targetEntity))
                        {
                            // if the target wall piece is not visible, just wcmd_attack to the point
                            if (!isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y))
                                targetEntity = NULL;
                        }
                    }

                    wcmd_executeAttackCommand(context, targetEntity, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    wcmd_executeAttackCommand(context, NULL, targetTile);

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

            WarEntity* selectedEntity = went_findEntity(context, map->selectedEntities.items[0]);
            assert(selectedEntity && wun_isBuildingUnit(selectedEntity));
            assert(selectedEntity->unit.type == buildingUnit);

            WarUnitStats stats = wun_getUnitStats(unitToTrain);
            if (went_checkFarmFood(context, player) &&
                went_decreasePlayerResources(context, player, stats.goldCost, stats.woodCost))
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

            WarEntity* selectedEntity = went_findEntity(context, map->selectedEntities.items[0]);
            assert(selectedEntity && wun_isBuildingUnit(selectedEntity));
            assert(selectedEntity->unit.type == buildingUnit);

            assert(hasRemainingUpgrade(player, upgradeToBuild));

            WarUpgradeStats stats = wun_getUpgradeStats(upgradeToBuild);
            s32 level = getUpgradeLevel(player, upgradeToBuild);
            if (went_decreasePlayerResources(context, player, stats.goldCost[level], 0))
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
                    WarEntity* worker = went_findEntity(context, workerId);
                    assert(worker);

                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    WarUnitType buildingToBuild = command->build.buildingToBuild;

                    WarBuildingStats stats = wun_getBuildingStats(buildingToBuild);
                    if (went_checkTileToBuild(context, buildingToBuild, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        if (went_decreasePlayerResources(context, player, stats.goldCost, stats.woodCost))
                        {
                            WarEntity* building = went_createBuilding(context, buildingToBuild, (s32)targetTile.x, (s32)targetTile.y, 0, true);
                            WarState* repairState = wst_createRepairState(context, worker, building->id);
                            wst_changeNextState(context, worker, repairState, true, true);

                            command->type = WAR_COMMAND_NONE;
                        }
                    }
                    else
                    {
                        waud_createAudio(context, WAR_UI_CANCEL, false);
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
                    WarEntity* townHall = went_findEntity(context, townHallId);

                    WarUnitType townHallType = wun_getTownHallOfRace(player->race);
                    assert(isUnitOfType(townHall, townHallType));

                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    if (went_checkTileToBuildRoadOrWall(context, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        if (went_decreasePlayerResources(context, player, WAR_WALL_GOLD_COST, WAR_WALL_WOOD_COST))
                        {
                            WarEntity* wall = map->wall;
                            WarWallPiece* piece = went_addWallPiece(wall, (s32)targetTile.x, (s32)targetTile.y, 0);
                            piece->hp = WAR_WALL_MAX_HP;
                            piece->maxhp = WAR_WALL_MAX_HP;

                            went_determineWallTypes(context, wall);

                            // don't reset the current command if the player is building
                            // roads or walls, to allow rapid construction of those structures
                            //
                            // command->type = WAR_COMMAND_NONE;

                            waud_createAudio(context, WAR_BUILD_ROAD, false);
                        }
                    }
                    else
                    {
                        waud_createAudio(context, WAR_UI_CANCEL, false);
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
                    WarEntity* townHall = went_findEntity(context, townHallId);

                    WarUnitType townHallType = wun_getTownHallOfRace(player->race);
                    assert(isUnitOfType(townHall, townHallType));

                    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
                    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

                    if (went_checkTileToBuildRoadOrWall(context, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        if (went_decreasePlayerResources(context, player, WAR_ROAD_GOLD_COST, WAR_ROAD_WOOD_COST))
                        {
                            WarEntity* road = map->road;
                            went_addRoadPiece(road, (s32)targetTile.x, (s32)targetTile.y, 0);

                            went_determineRoadTypes(context, road);

                            // don't reset the current command if the player is building
                            // roads or walls, to allow rapid construction of those structures
                            //
                            // command->type = WAR_COMMAND_NONE;

                            waud_createAudio(context, WAR_BUILD_ROAD, false);
                        }
                    }
                    else
                    {
                        waud_createAudio(context, WAR_UI_CANCEL, false);
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
            wcmd_executeSummonCommand(context, command->type);

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

                    wcmd_executeRainOfFireCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    wcmd_executeRainOfFireCommand(context, targetTile);

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

                    wcmd_executePoisonCloudCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    wcmd_executePoisonCloudCommand(context, targetTile);

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
                    WarEntity* targetEntity = went_findEntity(context, targetEntityId);

                    wcmd_executeHealingCommand(context, targetEntity, targetTile);

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
                    WarEntity* targetEntity = went_findEntity(context, targetEntityId);

                    wcmd_executeInvisiblityCommand(context, targetEntity, targetTile);

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
                    WarEntity* targetEntity = went_findEntity(context, targetEntityId);

                    wcmd_executeUnholyArmorCommand(context, targetEntity, targetTile);

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

                    wcmd_executeRaiseDeadCommand(context, targetTile);

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

                    wcmd_executeSightCommand(context, targetTile);

                    command->type = WAR_COMMAND_NONE;
                    return true;
                }
                else if (rectContainsf(map->minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_vec2ScreenToMinimapCoordinates(context, input->pos);
                    wcmd_executeSightCommand(context, targetTile);

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

void wcmd_trainFootman(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_FOOTMAN, WAR_UNIT_FOOTMAN, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_trainGrunt(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_GRUNT, WAR_UNIT_GRUNT, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_trainPeasant(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_PEASANT, WAR_UNIT_PEASANT, WAR_UNIT_TOWNHALL_HUMANS);
}

void wcmd_trainPeon(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_PEON, WAR_UNIT_PEON, WAR_UNIT_TOWNHALL_ORCS);
}

void wcmd_trainHumanCatapult(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_CATAPULT_HUMANS, WAR_UNIT_CATAPULT_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_trainOrcCatapult(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_CATAPULT_ORCS, WAR_UNIT_CATAPULT_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_trainKnight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_KNIGHT, WAR_UNIT_KNIGHT, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_trainRaider(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_RAIDER, WAR_UNIT_RAIDER, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_trainArcher(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_ARCHER, WAR_UNIT_ARCHER, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_trainSpearman(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_SPEARMAN, WAR_UNIT_SPEARMAN, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_trainConjurer(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_CONJURER, WAR_UNIT_CONJURER, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_trainWarlock(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_WARLOCK, WAR_UNIT_WARLOCK, WAR_UNIT_TOWER_ORCS);
}

void wcmd_trainCleric(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    trainUnit(context, WAR_COMMAND_TRAIN_CLERIC, WAR_UNIT_CLERIC, WAR_UNIT_CHURCH);
}

void wcmd_trainNecrolyte(WarContext* context, WarEntity* entity)
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

void wcmd_upgradeSwords(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SWORDS, WAR_UPGRADE_SWORDS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void wcmd_upgradeAxes(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_AXES, WAR_UPGRADE_AXES, WAR_UNIT_BLACKSMITH_ORCS);
}

void wcmd_upgradeHumanShields(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SHIELD_HUMANS, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_HUMANS);
}

void wcmd_upgradeOrcsShields(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SHIELD_ORCS, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_ORCS);
}

void wcmd_upgradeArrows(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_ARROWS, WAR_UPGRADE_ARROWS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void wcmd_upgradeSpears(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SPEARS, WAR_UPGRADE_SPEARS, WAR_UNIT_LUMBERMILL_ORCS);
}

void wcmd_upgradeHorses(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_HORSES, WAR_UPGRADE_HORSES, WAR_UNIT_STABLE);
}

void wcmd_upgradeWolves(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_WOLVES, WAR_UPGRADE_WOLVES, WAR_UNIT_KENNEL);
}

void wcmd_upgradeScorpions(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SCORPION, WAR_UPGRADE_SCORPIONS, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_upgradeSpiders(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SPIDER, WAR_UPGRADE_SPIDERS, WAR_UNIT_TOWER_ORCS);
}

void wcmd_upgradeRainOfFire(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_RAIN_OF_FIRE, WAR_UPGRADE_RAIN_OF_FIRE, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_upgradePoisonCloud(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_POISON_CLOUD, WAR_UPGRADE_POISON_CLOUD, WAR_UNIT_TOWER_ORCS);
}

void wcmd_upgradeWaterElemental(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_WATER_ELEMENTAL, WAR_UPGRADE_WATER_ELEMENTAL, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_upgradeDaemon(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_DAEMON, WAR_UPGRADE_DAEMON, WAR_UNIT_TOWER_ORCS);
}

void wcmd_upgradeHealing(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_HEALING, WAR_UPGRADE_HEALING, WAR_UNIT_CHURCH);
}

void wcmd_upgradeRaiseDead(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_RAISE_DEAD, WAR_UPGRADE_RAISE_DEAD, WAR_UNIT_TEMPLE);
}

void wcmd_upgradeFarSight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_FAR_SIGHT, WAR_UPGRADE_FAR_SIGHT, WAR_UNIT_CHURCH);
}

void wcmd_upgradeDarkVision(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_DARK_VISION, WAR_UPGRADE_DARK_VISION, WAR_UNIT_TEMPLE);
}

void wcmd_upgradeInvisibility(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_INVISIBILITY, WAR_UPGRADE_INVISIBILITY, WAR_UNIT_CHURCH);
}

void wcmd_upgradeUnholyArmor(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    upgradeUpgrade(context, WAR_COMMAND_UPGRADE_UNHOLY_ARMOR, WAR_UPGRADE_UNHOLY_ARMOR, WAR_UNIT_TEMPLE);
}

// wcmd_cancel
void wcmd_cancel(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    map->command.type = WAR_COMMAND_NONE;

    for (s32 i = 0; i < map->selectedEntities.count; i++)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[i];
        WarEntity* selectedEntity = went_findEntity(context, selectedEntityId);

        if (wun_isBuildingUnit(selectedEntity))
        {
            if (isBuilding(selectedEntity) || isGoingToBuild(selectedEntity))
            {
                WarBuildingStats stats = wun_getBuildingStats(selectedEntity->unit.type);
                went_increasePlayerResources(context, player, stats.goldCost, stats.woodCost);

                WarState* collapseState = wst_createCollapseState(context, selectedEntity);
                wst_changeNextState(context, selectedEntity, collapseState, true, true);

                waud_createAudioRandom(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, false);
            }
            else if (selectedEntity->unit.building)
            {
                if (isTraining(selectedEntity) || isGoingToTrain(selectedEntity))
                {
                    WarState* trainState = getTrainState(selectedEntity);
                    WarUnitType unitToBuild = trainState->train.unitToBuild;

                    WarUnitStats stats = wun_getUnitStats(unitToBuild);
                    went_increasePlayerResources(context, player, stats.goldCost, stats.woodCost);
                }
                else if (isUpgrading(selectedEntity) || isGoingToUpgrade(selectedEntity))
                {
                    WarState* upgradeState = getUpgradeState(selectedEntity);
                    WarUpgradeType upgradeToBuild = upgradeState->upgrade.upgradeToBuild;
                    assert(hasRemainingUpgrade(player, upgradeToBuild));

                    s32 upgradeLevel = getUpgradeLevel(player, upgradeToBuild);
                    WarUpgradeStats stats = wun_getUpgradeStats(upgradeToBuild);
                    went_increasePlayerResources(context, player, stats.goldCost[upgradeLevel], 0);
                }

                WarState* idleState = wst_createIdleState(context, entity, false);
                wst_changeNextState(context, selectedEntity, idleState, true, true);
            }
        }
    }
}

// basic
void wcmd_move(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_MOVE;
}

void wcmd_stop(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_STOP;
}

void wcmd_harvest(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_HARVEST;
}

void wcmd_deliver(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_DELIVER;
}

void wcmd_repair(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_REPAIR;
}

void wcmd_attack(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_ATTACK;
}

void wcmd_buildBasic(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_BASIC;
}

void wcmd_buildAdvanced(WarContext* context, WarEntity* entity)
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

void wcmd_buildFarmHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_FARM_HUMANS, WAR_UNIT_FARM_HUMANS);
}

void wcmd_buildFarmOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_FARM_ORCS, WAR_UNIT_FARM_ORCS);
}

void wcmd_buildBarracksHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_BARRACKS_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_buildBarracksOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_BARRACKS_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_buildChurch(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_CHURCH, WAR_UNIT_CHURCH);
}

void wcmd_buildTemple(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TEMPLE, WAR_UNIT_TEMPLE);
}

void wcmd_buildTowerHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TOWER_HUMANS, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_buildTowerOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TOWER_ORCS, WAR_UNIT_TOWER_ORCS);
}

void wcmd_buildTownHallHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TOWNHALL_HUMANS, WAR_UNIT_TOWNHALL_HUMANS);
}

void wcmd_buildTownHallOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_TOWNHALL_ORCS, WAR_UNIT_TOWNHALL_ORCS);
}

void wcmd_buildLumbermillHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_LUMBERMILL_HUMANS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void wcmd_buildLumbermillOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_LUMBERMILL_ORCS, WAR_UNIT_LUMBERMILL_ORCS);
}

void wcmd_buildStable(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_STABLE, WAR_UNIT_STABLE);
}

void wcmd_buildKennel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_KENNEL, WAR_UNIT_KENNEL);
}

void wcmd_buildBlacksmithHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_BLACKSMITH_HUMANS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void wcmd_buildBlacksmithOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    buildBuilding(context, WAR_COMMAND_BUILD_BLACKSMITH_ORCS, WAR_UNIT_BLACKSMITH_ORCS);
}

void wcmd_buildWall(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_WALL;
}

void wcmd_buildRoad(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_BUILD_ROAD;
}

// spells
void wcmd_castRainOfFire(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_RAIN_OF_FIRE;
}

void wcmd_castPoisonCloud(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_POISON_CLOUD;
}

void wcmd_castHeal(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_HEALING;
}

void wcmd_castFarSight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_FAR_SIGHT;
}

void wcmd_castDarkVision(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_DARK_VISION;
}

void wcmd_castInvisibility(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_INVISIBILITY;
}

void wcmd_castUnHolyArmor(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_UNHOLY_ARMOR;
}

void wcmd_castRaiseDead(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SPELL_RAISE_DEAD;
}

// summons
void wcmd_summonSpider(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_SPIDER;
}

void wcmd_summonScorpion(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_SCORPION;
}

void wcmd_summonDaemon(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_DAEMON;
}

void wcmd_summonWaterElemental(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->command.type = WAR_COMMAND_SUMMON_WATER_ELEMENTAL;
}
