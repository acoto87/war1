#include <assert.h>
#include <stdlib.h>

#include "war_commands.h"
#include "war_alloc.h"
#include "war_audio.h"
#include "war_entities.h"
#include "war_projectiles.h"
#include "war_state_machine.h"
#include "war_ui.h"
#include "war_units.h"
#include "war_pathfinder.h"

static inline void consumeCommand(WarMap* map, WarUnitCommand* command)
{
    // Only suppress the subsequent map-selection-on-release for commands that
    // are executed via a left click on the map/minimap.
    map->commandState.suppressSelectionOnRelease =
        command->type == WAR_COMMAND_MOVE ||
        command->type == WAR_COMMAND_HARVEST ||
        command->type == WAR_COMMAND_REPAIR ||
        command->type == WAR_COMMAND_ATTACK ||
        (command->type >= WAR_COMMAND_SPELL_HEALING && command->type <= WAR_COMMAND_SPELL_UNHOLY_ARMOR) ||
        (command->type >= WAR_COMMAND_BUILD_FARM_HUMANS && command->type <= WAR_COMMAND_BUILD_WALL);

    command->type = WAR_COMMAND_NONE;
}

static bool wcmd_tryPlaceBuilding(WarContext* context, WarEntity* worker, WarPlayerInfo* player, WarUnitType buildingType, vec2 targetTile, const WarBuildingStats* stats)
{
    if (!wst_canSubmitTransition(context, worker, WAR_INTERRUPT_PLAYER_ORDER))
    {
        return false;
    }

    if (!we_decreasePlayerResources(context, player, stats->goldCost, stats->woodCost))
    {
        return false;
    }

    WarEntity* building = we_createBuilding(context, CREATE_UNIT_ARGS_INIT(
        .type = buildingType,
        .x = (s32)targetTile.x,
        .y = (s32)targetTile.y,
        .player = player->index,
        .isGoingToBuild = true
    ));
    if (!building)
    {
        we_increasePlayerResources(context, player, stats->goldCost, stats->woodCost);
        return false;
    }

    WarStateRepair* repairState = wst_createRepairState(context, worker, building->id);
    if (!repairState)
    {
        we_removeEntity(context, building);
        we_increasePlayerResources(context, player, stats->goldCost, stats->woodCost);
        return false;
    }

    if (!wst_resetState(context, worker, (WarStateBase*)repairState, WAR_TRANSITION_CAUSE_PLAYER_ORDER))
    {
        we_removeEntity(context, building);
        we_increasePlayerResources(context, player, stats->goldCost, stats->woodCost);
        return false;
    }

    return true;
}

void wcmd_executeMoveCommand(WarContext* context, vec2 targetPosition)
{
    TracyCZoneN(ctx, "wcmd_executeMoveCommand", 1);

    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarPlayerInfo* player = &map->players[0];
    WarPathFinder* finder = &map->finder;

    bool goingToMove = false;

    s32 selEntitiesCount = map->selectedEntities.count;
    if (selEntitiesCount == 0)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPosition);
    WarMapFlowField* flowField = wpath_computeFlowField(finder, (s32)targetTile.x, (s32)targetTile.y);
    if (!flowField)
    {
        logWarning("Failed to compute flow field for move command at tile (%.2f, %.2f)", targetTile.x, targetTile.y);
        TracyCZoneEnd(ctx);
        return;
    }

        for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isDudeUnit(context, entity) && wu_isFriendlyUnit(context, entity))
        {
            if (!wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
            {
                continue;
            }

            if (isKeyHeld(input, WAR_KEY_SHIFT))
            {
                if (wst_isPatrolling(context, entity))
                {
                    if(wst_isMoving(context, entity))
                    {
                        WarStateMove* moveState = wst_getMoveState(context, entity);
                        if (moveState->waypointsCount < arrayLength(moveState->waypoints))
                        {
                            moveState->waypoints[moveState->waypointsCount] = targetPosition;
                            moveState->waypointsCount++;
                        }
                    }

                    WarStatePatrol* patrolState = wst_getPatrolState(context, entity);
                    if (patrolState->waypointsCount < arrayLength(patrolState->waypoints))
                    {
                        patrolState->waypoints[patrolState->waypointsCount] = targetPosition;
                        patrolState->waypointsCount++;
                    }
                }
                else if(wst_isMoving(context, entity) && !wst_isAttacking(context, entity))
                {
                    WarStateMove* moveState = wst_getMoveState(context, entity);
                    if (moveState->waypointsCount < arrayLength(moveState->waypoints))
                    {
                        moveState->waypoints[moveState->waypointsCount] = targetPosition;
                        moveState->waypointsCount++;
                    }
                }
                else
                {
                    vec2 position = wu_getUnitCenterPosition(context, entity);
                    WarStateMove* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, targetPosition), false);
                    wst_resetState(context, entity, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
                }
            }
            else
            {
                vec2 position = wu_getUnitCenterPosition(context, entity);
                WarStateMove* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, targetPosition), false);
                wst_resetState(context, entity, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
            }

            goingToMove = true;
        }
    }

    if (goingToMove)
    {
        wa_playAcknowledgementSound(context, player);
    }

    TracyCZoneEnd(ctx);
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
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            WarStateFollow* followState = wst_createFollowState(context, entity, targetEntity->id, VEC2_ZERO, MEGA_TILE_WIDTH);
            wst_resetState(context, entity, (WarStateBase*)followState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

            goingToFollow = true;
        }
    }

    if (goingToFollow)
    {
        wa_playAcknowledgementSound(context, player);
    }
}

void wcmd_executeStopCommand(WarContext* context)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for (s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            WarStateIdle* idleState = wst_createIdleState(context, entity, true);
            wst_resetState(context, entity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
        }
    }
}

void wcmd_executeHarvestCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    assert(wu_isUnitOfType(context, targetEntity, WAR_UNIT_GOLDMINE) ||
           isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST));

    bool goingToHarvest = false;

    vec2 targetPosition = wmap_tileToMapCoordinatesV(targetTile, true);

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            if (wu_isWorkerUnit(context, entity))
            {
                if (wu_isCarryingResources(context, entity))
                {
                    // find the closest town hall to deliver the gold
                    WarRace race = wu_getUnitRace(context, entity);
                    WarUnitType townHallType = wu_getTownHallOfRace(race);
                    WarEntity* townHall = we_findClosestUnitOfType(context, entity, townHallType);
                    if (townHall)
                    {
                        WarStateDeliver* deliverState = wst_createDeliverState(context, entity, townHall->id);
                        deliverState->cycle = true;
                        deliverState->sourceKind = isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST)
                            ? WAR_RESOURCE_WOOD
                            : WAR_RESOURCE_GOLD;
                        deliverState->sourceId = targetEntity->id;
                        if (isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST))
                            deliverState->sourcePosition = targetPosition;
                        wst_resetState(context, entity, (WarStateBase*)deliverState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
                    }
                }
                else
                {
                    WarStateBase* gatherGoldOrWoodState = isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST)
                        ? (WarStateBase*)wst_createGatherWoodState(context, entity, targetEntity->id, targetPosition)
                        : (WarStateBase*)wst_createGatherGoldState(context, entity, targetEntity->id);

                    wst_resetState(context, entity, (WarStateBase*)gatherGoldOrWoodState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
                }

                goingToHarvest = true;
            }
            else if (wu_isDudeUnit(context, entity))
            {
                vec2 position = wu_getUnitCenterPosition(context, entity);
                WarStateMove* moveState = wst_createMoveState(context, entity, 2, arrayArg(vec2, position, targetPosition), false);
                wst_resetState(context, entity, (WarStateBase*)moveState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

                goingToHarvest = true;
            }
        }
    }

    if (goingToHarvest)
    {
        wa_playAcknowledgementSound(context, player);
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
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            WarEntity* townHall = targetEntity;
            if (!townHall)
            {
                WarRace race = wu_getUnitRace(context, entity);
                WarUnitType townHallType = wu_getTownHallOfRace(race);
                townHall = we_findClosestUnitOfType(context, entity, townHallType);
                assert(townHall);
            }

            if (wu_isWorkerUnit(context, entity) && wu_isCarryingResources(context, entity))
            {
                WarStateDeliver* deliverState = wst_createDeliverState(context, entity, townHall->id);
                wst_resetState(context, entity, (WarStateBase*)deliverState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

                goingToDeliver = true;
            }
            else if (wu_isDudeUnit(context, entity))
            {
                WarStateFollow* followState = wst_createFollowState(context, entity, townHall->id, VEC2_ZERO, MEGA_TILE_WIDTH);
                wst_resetState(context, entity, (WarStateBase*)followState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

                goingToDeliver = true;
            }
        }
    }

    if (goingToDeliver)
    {
        wa_playAcknowledgementSound(context, player);
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
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isFriendlyUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            // the unit can't repair itself
            if (entity->id == targetEntity->id)
            {
                continue;
            }

            if (wu_isWorkerUnit(context, entity))
            {
                WarStateRepair* repairState = wst_createRepairState(context, entity, targetEntity->id);
                wst_resetState(context, entity, (WarStateBase*)repairState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

                goingToRepair = true;
            }
        }
    }

    if (goingToRepair)
    {
        wa_playAcknowledgementSound(context, player);
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
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isConjurerOrWarlockUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            WarUnitComponent* unit = we_getUnitComponent(context, entity);

            // when the unit summon another unit, it is not invisible anymore
            unit->invisible = false;
            unit->invisibilityTime = 0;

            const WarUnitCommandMapping* commandMapping = wu_getCommandMapping(summonType);
            const WarSpellMapping* spellMapping = wu_getSpellMapping(commandMapping->mappedType);
            const WarSpellStats* stats = wu_getSpellStats(commandMapping->mappedType);

            while (we_decreaseUnitMana(context, entity, stats->manaCost))
            {
                vec2 tile = wu_getUnitCenterTile(context, entity);
                vec2 spawnTile = wpath_findEmptyTile(&map->finder, (s32)tile.x, (s32)tile.y);

                WarEntity* summonedUnit = we_createUnit(context, CREATE_UNIT_ARGS_INIT(
                    .type = spellMapping->mappedType,
                    .x = (s32)spawnTile.x,
                    .y = (s32)spawnTile.y,
                    .player = unit->player,
                    .resourceKind = WAR_RESOURCE_NONE,
                    .amount = 0,
                    .addToMap = true
                ));
                we_setInitialIdleState(context, summonedUnit);

                vec2 unitSize = wu_getUnitSize(context, summonedUnit);
                wpath_setStaticEntity(&map->finder, (s32)spawnTile.x, (s32)spawnTile.y, (s32)unitSize.x, (s32)unitSize.y, summonedUnit->id);

                WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                wanim_createSpellAnimation(context, animEntity, wmap_tileToMapCoordinatesV(spawnTile, true));

                casted = true;
            }
        }
    }

    if (casted)
    {
        wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_NORMAL_SPELL, .loop=false));
    }
}

void wcmd_executeRainOfFireCommand(WarContext* context, vec2 targetPosition)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isConjurerOrWarlockUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            WarStateCast* castState = wst_createCastState(context, entity, WAR_SPELL_RAIN_OF_FIRE, 0, targetPosition);
            wst_resetState(context, entity, (WarStateBase*)castState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
        }
    }
}

void wcmd_executePoisonCloudCommand(WarContext* context, vec2 targetPosition)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isConjurerOrWarlockUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            WarStateCast* castState = wst_createCastState(context, entity, WAR_SPELL_POISON_CLOUD, 0, targetPosition);
            wst_resetState(context, entity, (WarStateBase*)castState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
        }
    }
}

void wcmd_executeHealingCommand(WarContext* context, WarEntity* targetEntity, vec2 targetPosition)
{
    WarMap* map = context->map;

    if (targetEntity && wu_isDudeUnit(context, targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = we_findEntity(context, entityId);
            assert(entity);

            if (wu_isClericOrNecrolyteUnit(context, entity) &&
                wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
            {
                // the unit can't heal itself
                if (entity->id != targetEntity->id)
                {
                    WarStateCast* castState = wst_createCastState(context, entity, WAR_SPELL_HEALING, targetEntity->id, targetPosition);
                    wst_resetState(context, entity, (WarStateBase*)castState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
                }
            }
        }
    }
}

void wcmd_executeInvisiblityCommand(WarContext* context, WarEntity* targetEntity, vec2 targetPosition)
{
    WarMap* map = context->map;

    if (targetEntity && wu_isDudeUnit(context, targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = we_findEntity(context, entityId);
            assert(entity);

            if (wu_isClericOrNecrolyteUnit(context, entity) &&
                wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
            {
                WarStateCast* castState = wst_createCastState(context, entity, WAR_SPELL_INVISIBILITY, targetEntity->id, targetPosition);
                wst_resetState(context, entity, (WarStateBase*)castState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
            }
        }
    }
}

void wcmd_executeUnholyArmorCommand(WarContext* context, WarEntity* targetEntity, vec2 targetPosition)
{
    WarMap* map = context->map;

    if (targetEntity && wu_isDudeUnit(context, targetEntity))
    {
        s32 selEntitiesCount = map->selectedEntities.count;
        for(s32 i = 0; i < selEntitiesCount; i++)
        {
            WarEntityId entityId = map->selectedEntities.items[i];
            WarEntity* entity = we_findEntity(context, entityId);
            assert(entity);

            if (wu_isClericOrNecrolyteUnit(context, entity) &&
                wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
            {
                WarStateCast* castState = wst_createCastState(context, entity, WAR_SPELL_UNHOLY_ARMOR, targetEntity->id, targetPosition);
                wst_resetState(context, entity, (WarStateBase*)castState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
            }
        }
    }
}

void wcmd_executeRaiseDeadCommand(WarContext* context, vec2 targetPosition)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isClericOrNecrolyteUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            WarStateCast* castState = wst_createCastState(context, entity, WAR_SPELL_RAISE_DEAD, 0, targetPosition);
            wst_resetState(context, entity, (WarStateBase*)castState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
        }
    }
}

void wcmd_executeSightCommand(WarContext* context, vec2 targetPosition)
{
    WarMap* map = context->map;

    s32 selEntitiesCount = map->selectedEntities.count;
    for(s32 i = 0; i < selEntitiesCount; i++)
    {
        WarEntityId entityId = map->selectedEntities.items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        assert(entity);

        if (wu_isClericOrNecrolyteUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            WarSpellType spellType = wu_isHumanUnit(context, entity) ? WAR_SPELL_FAR_SIGHT : WAR_SPELL_DARK_VISION;
            WarStateCast* castState = wst_createCastState(context, entity, spellType, 0, targetPosition);
            wst_resetState(context, entity, (WarStateBase*)castState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
        }
    }
}

void wcmd_executeAttackCommand(WarContext* context, WarEntity* targetEntity, vec2 targetPosition)
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

        if (wu_isFriendlyUnit(context, entity) &&
            wst_canSubmitTransition(context, entity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            if (targetEntity)
            {
                // the unit can't attack itself
                if (entity->id != targetEntity->id)
                {
                    if (wu_canAttack(context, entity, targetEntity))
                    {
                        WarStateAttack* attackState = wst_createAttackState(context, entity, targetEntity->id, targetPosition);
                        wst_resetState(context, entity, (WarStateBase*)attackState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

                        playSound = true;
                    }
                    else if (wu_isWorkerUnit(context, entity))
                    {
                        WarStateFollow* followState = wst_createFollowState(context, entity, targetEntity->id, VEC2_ZERO, MEGA_TILE_WIDTH);
                        wst_resetState(context, entity, (WarStateBase*)followState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
                    }
                }
            }
            else
            {
                WarStateAttack* attackState = wst_createAttackState(context, entity, 0, targetPosition);
                wst_resetState(context, entity, (WarStateBase*)attackState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);

                playSound = true;
            }
        }
    }

    if (playSound)
    {
        wa_playAcknowledgementSound(context, player);
    }
}

bool wcmd_executeCommand(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarPlayerInfo* player = &map->players[0];
    WarUnitCommand* command = &map->commandState.command;

    if (command->type == WAR_COMMAND_NONE)
    {
        return false;
    }

    switch (command->type)
    {
        case WAR_COMMAND_MOVE:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);

                    wcmd_executeMoveCommand(context, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
                else if (rect_containsf(map->ui.minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_screenToMinimapCoordinatesV(context, input->pos);
                    vec2 targetPoint = wmap_tileToMapCoordinatesV(targetTile, true);
                    map->commandState.suppressMinimapViewportOnRelease = true;
                    wcmd_executeMoveCommand(context, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_STOP:
        {
            wcmd_executeStopCommand(context);
            consumeCommand(map, command);
            return true;
        }
        case WAR_COMMAND_HARVEST:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                    WarEntityId targetEntityId = wpath_getTileEntityId(&map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                    if (targetEntity)
                    {
                        if (wu_isUnitOfType(context, targetEntity, WAR_UNIT_GOLDMINE))
                        {
                            if (!wmap_isUnitUnknown(context, map, targetEntity))
                                wcmd_executeHarvestCommand(context, targetEntity, targetTile);
                            else
                                wcmd_executeMoveCommand(context, targetPoint);
                        }
                        else if (isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST))
                        {
                            if (!wmap_isTileUnknown(map, (s32)targetTile.x, (s32)targetTile.y))
                            {
                                wcmd_executeHarvestCommand(context, targetEntity, targetTile);
                            }
                            else
                            {
                                WarTree* tree = we_findAccesibleTree(context, targetEntity, targetTile);
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

                    consumeCommand(map, command);
                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_DELIVER:
        {
            wcmd_executeDeliverCommand(context, NULL);

            consumeCommand(map, command);
            return true;
        }
        case WAR_COMMAND_REPAIR:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);
                    if (wmap_isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y) ||
                        wmap_isTileFog(map, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        WarEntityId targetEntityId = wpath_getTileEntityId(&map->finder, (s32)targetTile.x, (s32)targetTile.y);
                        WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                        if (targetEntity && wu_isBuildingUnit(context, targetEntity))
                        {
                            wcmd_executeRepairCommand(context, targetEntity);
                        }
                    }

                    consumeCommand(map, command);
                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_ATTACK:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                    WarEntityId targetEntityId = wpath_getTileEntityId(&map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                    if (targetEntity)
                    {
                        if (wu_isUnit(targetEntity))
                        {
                            // if the target entity is not visible or partially visible, just attack to the point
                            if (wmap_isUnitUnknown(context, map, targetEntity))
                                targetEntity = NULL;
                        }
                        else if (wu_isWall(targetEntity))
                        {
                            // if the target wall piece is not visible, just attack to the point
                            if (!wmap_isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y))
                                targetEntity = NULL;
                        }
                    }

                    wcmd_executeAttackCommand(context, targetEntity, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
                else if (rect_containsf(map->ui.minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetTile = wmap_screenToMinimapCoordinatesV(context, input->pos);
                    vec2 targetPoint = wmap_tileToMapCoordinatesV(targetTile, true);
                    map->commandState.suppressMinimapViewportOnRelease = true;
                    wcmd_executeAttackCommand(context, NULL, targetPoint);

                    consumeCommand(map, command);
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
            assert(selectedEntity && wu_isBuildingUnit(context, selectedEntity));

            WarUnitComponent* selectedUnit = we_getUnitComponent(context, selectedEntity);
            assert(selectedUnit);
            assert(selectedUnit->type == buildingUnit);
            NOT_USED(buildingUnit);
            NOT_USED(selectedUnit);

            if (wst_canSubmitTransition(context, selectedEntity, WAR_INTERRUPT_PLAYER_ORDER))
            {
                const WarUnitStats* stats = wu_getUnitStats(unitToTrain);
                if (we_checkFarmFood(context, player) &&
                    we_enoughPlayerResources(context, player, stats->goldCost, stats->woodCost))
                {
                    WarStateTrain* trainState = wst_createTrainState(context, selectedEntity, unitToTrain, (f32)stats->buildTime, stats->goldCost, stats->woodCost, NULL);
                    wst_resetState(context, selectedEntity, (WarStateBase*)trainState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
                }
            }

            consumeCommand(map, command);
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
            assert(selectedEntity && wu_isBuildingUnit(context, selectedEntity));

            WarUnitComponent* selectedUnit = we_getUnitComponent(context, selectedEntity);
            assert(selectedUnit);
            assert(selectedUnit->type == buildingUnit);
            NOT_USED(buildingUnit);
            NOT_USED(selectedUnit);

            if (wst_canSubmitTransition(context, selectedEntity, WAR_INTERRUPT_PLAYER_ORDER))
            {
                assert(hasRemainingUpgrade(player, upgradeToBuild));

                const WarUpgradeStats* stats = wu_getUpgradeStats(upgradeToBuild);
                s32 level = getUpgradeLevel(player, upgradeToBuild);
                if (we_enoughPlayerResources(context, player, stats->goldCost[level], 0))
                {
                    WarStateUpgrade* upgradeState = wst_createUpgradeState(context, selectedEntity, upgradeToBuild, (f32)stats->buildTime, stats->goldCost[level], 0);
                    wst_resetState(context, selectedEntity, (WarStateBase*)upgradeState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
                }
            }

            consumeCommand(map, command);
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
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    assert(map->selectedEntities.count > 0);

                    WarEntityId workerId = map->selectedEntities.items[0];
                    WarEntity* worker = we_findEntity(context, workerId);
                    assert(worker);

                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                    WarUnitType buildingToBuild = command->build.buildingToBuild;

                    const WarBuildingStats* stats = wu_getBuildingStats(buildingToBuild);
                    if (we_checkTileToBuild(context, buildingToBuild, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        if (wcmd_tryPlaceBuilding(context, worker, player, buildingToBuild, targetTile, stats))
                        {
                            consumeCommand(map, command);
                        }
                    }
                    else
                    {
                        wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_UI_CANCEL, .loop=false));
                    }

                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_BUILD_WALL:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    assert(map->selectedEntities.count > 0);

                    WarEntityId townHallId = map->selectedEntities.items[0];
                    WarEntity* townHall = we_findEntity(context, townHallId);
                    assert(townHall);

                    WarUnitType townHallType = wu_getTownHallOfRace(player->race);
                    assert(wu_isUnitOfType(context, townHall, townHallType));
                    NOT_USED(townHall);
                    NOT_USED(townHallType);

                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                    if (we_checkTileToBuildRoadOrWall(context, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        if (we_decreasePlayerResources(context, player, WAR_WALL_GOLD_COST, WAR_WALL_WOOD_COST))
                        {
                            WarEntity* wall = map->editing.wall;
                            WarWallPiece* piece = we_addWallPiece(context, wall, (s32)targetTile.x, (s32)targetTile.y, 0);
                            piece->hp = WAR_WALL_MAX_HP;
                            piece->maxhp = WAR_WALL_MAX_HP;

                            we_determineWallTypes(context, wall);

                            // don't reset the current command if the player is building
                            // roads or walls, to allow rapid construction of those structures
                            //
                            // consumeCommand(map, command);

                            wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_BUILD_ROAD, .loop=false));
                        }
                    }
                    else
                    {
                        wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_UI_CANCEL, .loop=false));
                    }

                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_BUILD_ROAD:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    assert(map->selectedEntities.count > 0);

                    WarEntityId townHallId = map->selectedEntities.items[0];
                    WarEntity* townHall = we_findEntity(context, townHallId);
                    assert(townHall);

                    WarUnitType townHallType = wu_getTownHallOfRace(player->race);
                    assert(wu_isUnitOfType(context, townHall, townHallType));
                    NOT_USED(townHall);
                    NOT_USED(townHallType);

                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                    if (we_checkTileToBuildRoadOrWall(context, (s32)targetTile.x, (s32)targetTile.y))
                    {
                        if (we_decreasePlayerResources(context, player, WAR_ROAD_GOLD_COST, WAR_ROAD_WOOD_COST))
                        {
                            WarEntity* road = map->editing.road;
                            we_addRoadPiece(context, road, (s32)targetTile.x, (s32)targetTile.y, 0);

                            we_determineRoadTypes(context, road);

                            // don't reset the current command if the player is building
                            // roads or walls, to allow rapid construction of those structures
                            //
                            // consumeCommand(map, command);

                            wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_BUILD_ROAD, .loop=false));
                        }
                    }
                    else
                    {
                        wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_UI_CANCEL, .loop=false));
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
            consumeCommand(map, command);
            return true;
        }
        case WAR_COMMAND_SPELL_RAIN_OF_FIRE:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    wcmd_executeRainOfFireCommand(context, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
                else if (rect_containsf(map->ui.minimapPanel, input->pos.x, input->pos.y))
                {
                    map->commandState.suppressMinimapViewportOnRelease = true;
                    vec2 targetTile = wmap_screenToMinimapCoordinatesV(context, input->pos);
                    vec2 targetPoint = wmap_tileToMapCoordinatesV(targetTile, true);
                    wcmd_executeRainOfFireCommand(context, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_SPELL_POISON_CLOUD:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    wcmd_executePoisonCloudCommand(context, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
                else if (rect_containsf(map->ui.minimapPanel, input->pos.x, input->pos.y))
                {
                    map->commandState.suppressMinimapViewportOnRelease = true;
                    vec2 targetTile = wmap_screenToMinimapCoordinatesV(context, input->pos);
                    vec2 targetPoint = wmap_tileToMapCoordinatesV(targetTile, true);
                    wcmd_executePoisonCloudCommand(context, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_SPELL_HEALING:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                    WarEntityId targetEntityId = wpath_getTileEntityId(&map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);

                    wcmd_executeHealingCommand(context, targetEntity, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_SPELL_INVISIBILITY:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                    WarEntityId targetEntityId = wpath_getTileEntityId(&map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);

                    wcmd_executeInvisiblityCommand(context, targetEntity, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_SPELL_UNHOLY_ARMOR:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                    WarEntityId targetEntityId = wpath_getTileEntityId(&map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);

                    wcmd_executeUnholyArmorCommand(context, targetEntity, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_SPELL_RAISE_DEAD:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    wcmd_executeRaiseDeadCommand(context, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
            }

            return false;
        }
        case WAR_COMMAND_SPELL_FAR_SIGHT:
        case WAR_COMMAND_SPELL_DARK_VISION:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if(rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    wcmd_executeSightCommand(context, targetPoint);
                    consumeCommand(map, command);
                    return true;
                }
                else if (rect_containsf(map->ui.minimapPanel, input->pos.x, input->pos.y))
                {
                    map->commandState.suppressMinimapViewportOnRelease = true;
                    vec2 targetTile = wmap_screenToMinimapCoordinatesV(context, input->pos);
                    vec2 targetPoint = wmap_tileToMapCoordinatesV(targetTile, true);
                    wcmd_executeSightCommand(context, targetPoint);
                    consumeCommand(map, command);
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
void wcmd_trainUnit(WarContext* context, WarUnitCommandType commandType, WarUnitType unitToTrain, WarUnitType buildingUnit)
{
    WarMap* map = context->map;

    map->commandState.command.type = commandType;
    map->commandState.command.train.unitToTrain = unitToTrain;
    map->commandState.command.train.buildingUnit = buildingUnit;
}

void wcmd_trainFootman(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_FOOTMAN, WAR_UNIT_FOOTMAN, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_trainGrunt(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_GRUNT, WAR_UNIT_GRUNT, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_trainPeasant(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_PEASANT, WAR_UNIT_PEASANT, WAR_UNIT_TOWNHALL_HUMANS);
}

void wcmd_trainPeon(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_PEON, WAR_UNIT_PEON, WAR_UNIT_TOWNHALL_ORCS);
}

void wcmd_trainHumanCatapult(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_CATAPULT_HUMANS, WAR_UNIT_CATAPULT_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_trainOrcCatapult(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_CATAPULT_ORCS, WAR_UNIT_CATAPULT_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_trainKnight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_KNIGHT, WAR_UNIT_KNIGHT, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_trainRaider(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_RAIDER, WAR_UNIT_RAIDER, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_trainArcher(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_ARCHER, WAR_UNIT_ARCHER, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_trainSpearman(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_SPEARMAN, WAR_UNIT_SPEARMAN, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_trainConjurer(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_CONJURER, WAR_UNIT_CONJURER, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_trainWarlock(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_WARLOCK, WAR_UNIT_WARLOCK, WAR_UNIT_TOWER_ORCS);
}

void wcmd_trainCleric(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_CLERIC, WAR_UNIT_CLERIC, WAR_UNIT_CHURCH);
}

void wcmd_trainNecrolyte(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_trainUnit(context, WAR_COMMAND_TRAIN_NECROLYTE, WAR_UNIT_NECROLYTE, WAR_UNIT_TEMPLE);
}

// upgrades
void wcmd_upgradeUpgrade(WarContext* context, WarUnitCommandType commandType, WarUpgradeType upgradeToBuild, WarUnitType buildingUnit)
{
    WarMap* map = context->map;

    map->commandState.command.type = commandType;
    map->commandState.command.upgrade.upgradeToBuild = upgradeToBuild;
    map->commandState.command.upgrade.buildingUnit = buildingUnit;
}

void wcmd_upgradeSwords(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SWORDS, WAR_UPGRADE_SWORDS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void wcmd_upgradeAxes(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_AXES, WAR_UPGRADE_AXES, WAR_UNIT_BLACKSMITH_ORCS);
}

void wcmd_upgradeHumanShields(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SHIELD_HUMANS, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_HUMANS);
}

void wcmd_upgradeOrcsShields(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SHIELD_ORCS, WAR_UPGRADE_SHIELD, WAR_UNIT_BLACKSMITH_ORCS);
}

void wcmd_upgradeArrows(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_ARROWS, WAR_UPGRADE_ARROWS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void wcmd_upgradeSpears(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SPEARS, WAR_UPGRADE_SPEARS, WAR_UNIT_LUMBERMILL_ORCS);
}

void wcmd_upgradeHorses(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_HORSES, WAR_UPGRADE_HORSES, WAR_UNIT_STABLE);
}

void wcmd_upgradeWolves(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_WOLVES, WAR_UPGRADE_WOLVES, WAR_UNIT_KENNEL);
}

void wcmd_upgradeScorpions(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SCORPION, WAR_UPGRADE_SCORPIONS, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_upgradeSpiders(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_SPIDER, WAR_UPGRADE_SPIDERS, WAR_UNIT_TOWER_ORCS);
}

void wcmd_upgradeRainOfFire(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_RAIN_OF_FIRE, WAR_UPGRADE_RAIN_OF_FIRE, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_upgradePoisonCloud(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_POISON_CLOUD, WAR_UPGRADE_POISON_CLOUD, WAR_UNIT_TOWER_ORCS);
}

void wcmd_upgradeWaterElemental(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_WATER_ELEMENTAL, WAR_UPGRADE_WATER_ELEMENTAL, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_upgradeDaemon(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_DAEMON, WAR_UPGRADE_DAEMON, WAR_UNIT_TOWER_ORCS);
}

void wcmd_upgradeHealing(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_HEALING, WAR_UPGRADE_HEALING, WAR_UNIT_CHURCH);
}

void wcmd_upgradeRaiseDead(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_RAISE_DEAD, WAR_UPGRADE_RAISE_DEAD, WAR_UNIT_TEMPLE);
}

void wcmd_upgradeFarSight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_FAR_SIGHT, WAR_UPGRADE_FAR_SIGHT, WAR_UNIT_CHURCH);
}

void wcmd_upgradeDarkVision(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_DARK_VISION, WAR_UPGRADE_DARK_VISION, WAR_UNIT_TEMPLE);
}

void wcmd_upgradeInvisibility(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_INVISIBILITY, WAR_UPGRADE_INVISIBILITY, WAR_UNIT_CHURCH);
}

void wcmd_upgradeUnholyArmor(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_upgradeUpgrade(context, WAR_COMMAND_UPGRADE_UNHOLY_ARMOR, WAR_UPGRADE_UNHOLY_ARMOR, WAR_UNIT_TEMPLE);
}

// wcmd_cancel
void wcmd_cancel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);

    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_NONE;

    for (s32 i = 0; i < map->selectedEntities.count; i++)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[i];
        WarEntity* selectedEntity = we_findEntity(context, selectedEntityId);
        assert(selectedEntity);

        if (wu_isBuildingUnit(context, selectedEntity) &&
            wst_canSubmitTransition(context, selectedEntity, WAR_INTERRUPT_PLAYER_ORDER))
        {
            if (wst_isBuilding(context, selectedEntity) || wst_isGoingToBuild(context, selectedEntity))
            {
                WarStateCollapse* collapseState = wst_createCollapseState(context, selectedEntity);
                wst_resetStateForCancellation(context, selectedEntity, (WarStateBase*)collapseState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
            }
            else if (wst_isTraining(context, selectedEntity) || wst_isGoingToTrain(context, selectedEntity))
            {
                WarStateIdle* idleState = wst_createIdleState(context, selectedEntity, false);
                wst_resetStateForCancellation(context, selectedEntity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
            }
            else if (wst_isUpgrading(context, selectedEntity) || wst_isGoingToUpgrade(context, selectedEntity))
            {
                WarStateIdle* idleState = wst_createIdleState(context, selectedEntity, false);
                wst_resetStateForCancellation(context, selectedEntity, (WarStateBase*)idleState, WAR_TRANSITION_CAUSE_PLAYER_ORDER);
            }
        }
    }
}

// basic
void move(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_MOVE;
}

void wcmd_stop(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_STOP;
}

void wcmd_harvest(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_HARVEST;
}

void deliver(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_DELIVER;
}

void repair(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_REPAIR;
}

void attack(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_ATTACK;
}

void wcmd_buildBasic(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_BUILD_BASIC;
}

void wcmd_buildAdvanced(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_BUILD_ADVANCED;
}

void wcmd_buildBuilding(WarContext* context, WarUnitCommandType commandType, WarUnitType buildingToBuild)
{
    WarMap* map = context->map;

    map->commandState.command.type = commandType;
    map->commandState.command.build.buildingToBuild = buildingToBuild;
}

void wcmd_buildFarmHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_FARM_HUMANS, WAR_UNIT_FARM_HUMANS);
}

void wcmd_buildFarmOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_FARM_ORCS, WAR_UNIT_FARM_ORCS);
}

void wcmd_buildBarracksHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_BARRACKS_HUMANS, WAR_UNIT_BARRACKS_HUMANS);
}

void wcmd_buildBarracksOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_BARRACKS_ORCS, WAR_UNIT_BARRACKS_ORCS);
}

void wcmd_buildChurch(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_CHURCH, WAR_UNIT_CHURCH);
}

void wcmd_buildTemple(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_TEMPLE, WAR_UNIT_TEMPLE);
}

void wcmd_buildTowerHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_TOWER_HUMANS, WAR_UNIT_TOWER_HUMANS);
}

void wcmd_buildTowerOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_TOWER_ORCS, WAR_UNIT_TOWER_ORCS);
}

void wcmd_buildTownHallHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_TOWNHALL_HUMANS, WAR_UNIT_TOWNHALL_HUMANS);
}

void wcmd_buildTownHallOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_TOWNHALL_ORCS, WAR_UNIT_TOWNHALL_ORCS);
}

void wcmd_buildLumbermillHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_LUMBERMILL_HUMANS, WAR_UNIT_LUMBERMILL_HUMANS);
}

void wcmd_buildLumbermillOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_LUMBERMILL_ORCS, WAR_UNIT_LUMBERMILL_ORCS);
}

void wcmd_buildStable(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_STABLE, WAR_UNIT_STABLE);
}

void wcmd_buildKennel(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_KENNEL, WAR_UNIT_KENNEL);
}

void wcmd_buildBlacksmithHumans(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_BLACKSMITH_HUMANS, WAR_UNIT_BLACKSMITH_HUMANS);
}

void wcmd_buildBlacksmithOrcs(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    wcmd_buildBuilding(context, WAR_COMMAND_BUILD_BLACKSMITH_ORCS, WAR_UNIT_BLACKSMITH_ORCS);
}

void wcmd_buildWall(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_BUILD_WALL;
}

void wcmd_buildRoad(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_BUILD_ROAD;
}

// spells
void wcmd_castRainOfFire(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SPELL_RAIN_OF_FIRE;
}

void wcmd_castPoisonCloud(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SPELL_POISON_CLOUD;
}

void wcmd_castHeal(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SPELL_HEALING;
}

void wcmd_castFarSight(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SPELL_FAR_SIGHT;
}

void wcmd_castDarkVision(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SPELL_DARK_VISION;
}

void wcmd_castInvisibility(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SPELL_INVISIBILITY;
}

void wcmd_castUnHolyArmor(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SPELL_UNHOLY_ARMOR;
}

void wcmd_castRaiseDead(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SPELL_RAISE_DEAD;
}

// summons
void wcmd_summonSpider(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SUMMON_SPIDER;
}

void wcmd_summonScorpion(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SUMMON_SCORPION;
}

void wcmd_summonDaemon(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SUMMON_DAEMON;
}

void wcmd_summonWaterElemental(WarContext* context, WarEntity* entity)
{
    NOT_USED(entity);
    WarMap* map = context->map;

    map->commandState.command.type = WAR_COMMAND_SUMMON_WATER_ELEMENTAL;
}
