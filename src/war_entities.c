#include "war_entities.h"

#include <assert.h>
#include <stdlib.h>

#include "shl/wstr.h"

#include "war_animations.h"
#include "war_audio.h"
#include "war_font.h"
#include "war_render.h"
#include "war_sprites.h"
#include "war_units.h"
#include "war_map.h"

void we_addTransformComponent(WarContext* context, WarEntity* entity, vec2 position)
{
    NOT_USED(context);

    entity->transform = (WarTransformComponent){0};
    entity->transform.enabled = true;
    entity->transform.position = position;
    entity->transform.rotation = VEC2_ZERO;
    entity->transform.scale = VEC2_ONE;
}

void we_removeTransformComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->transform = (WarTransformComponent){0};
}

void we_addSpriteComponent(WarContext* context, WarEntity* entity, WarSprite sprite)
{
    NOT_USED(context);

    entity->sprite = (WarSpriteComponent){0};
    entity->sprite.enabled = true;
    entity->sprite.frameIndex = 0;
    entity->sprite.resourceIndex = 0;
    entity->sprite.sprite = sprite;
}

void we_addSpriteComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef spriteResourceRef)
{
    if (spriteResourceRef.resourceIndex < 0)
    {
        logWarning("Trying to create a sprite component with invalid resource index: %d", spriteResourceRef.resourceIndex);
        return;
    }

    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);
    we_addSpriteComponent(context, entity, sprite);
    entity->sprite.resourceIndex = spriteResourceRef.resourceIndex;
}

void we_removeSpriteComponent(WarContext* context, WarEntity* entity)
{
    wspr_freeSprite(context, entity->sprite.sprite);

    entity->sprite = (WarSpriteComponent){0};
}

void we_addUnitComponent(WarContext* context,
                      WarEntity* entity,
                      WarUnitType type,
                      s32 x,
                      s32 y,
                      u8 player,
                      WarResourceKind resourceKind,
                      u32 amount)
{
    NOT_USED(context);

    WarUnitData unitData = wu_getUnitData(type);

    entity->unit = (WarUnitComponent){0};
    entity->unit.enabled = true;
    entity->unit.type = type;
    entity->unit.direction = rand() % WAR_DIRECTION_COUNT;
    entity->unit.tilex = x;
    entity->unit.tiley = y;
    entity->unit.sizex = unitData.sizex;
    entity->unit.sizey = unitData.sizey;
    entity->unit.player = player;
    entity->unit.resourceKind = resourceKind;
    entity->unit.amount = amount;
    entity->unit.speed = 0;
    entity->unit.hp = 0;
    entity->unit.maxhp = 0;
    entity->unit.armor = 0;
    entity->unit.range = 0;
    entity->unit.minDamage = 0;
    entity->unit.rndDamage = 0;
    entity->unit.decay = 0;
    entity->unit.invisible = false;
    entity->unit.invulnerable = false;
    entity->unit.hasBeenSeen = false;
    entity->unit.actionType = WAR_ACTION_TYPE_IDLE;
}

void we_removeUnitComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->unit = (WarUnitComponent){0};
}

void we_addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces)
{
    NOT_USED(context);

    entity->road = (WarRoadComponent){0};
    entity->road.enabled = true;
    entity->road.pieces = pieces;
}

void we_removeRoadComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    WarRoadPieceListFree(&entity->road.pieces);
    entity->road = (WarRoadComponent){0};
}

void we_addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces)
{
    NOT_USED(context);

    entity->wall = (WarWallComponent){0};
    entity->wall.enabled = true;
    entity->wall.pieces = pieces;
}

void we_removeWallComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    WarWallPieceListFree(&entity->wall.pieces);
    entity->wall = (WarWallComponent){0};
}

void we_addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces)
{
    NOT_USED(context);

    entity->ruin = (WarRuinComponent){0};
    entity->ruin.enabled = true;
    entity->ruin.pieces = pieces;
}

void we_removeRuinComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    WarRuinPieceListFree(&entity->ruin.pieces);
    entity->ruin = (WarRuinComponent){0};
}

void we_addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees)
{
    NOT_USED(context);

    entity->forest = (WarForestComponent){0};
    entity->forest.enabled = true;
    entity->forest.trees = trees;
}

void we_removeForestComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    WarTreeListFree(&entity->forest.trees);
    entity->forest = (WarForestComponent){0};
}

void we_addStateMachineComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->stateMachine = (WarStateMachineComponent){0};
    entity->stateMachine.enabled = true;
    entity->stateMachine.currentState = NULL;
    entity->stateMachine.nextState = NULL;
}

void we_removeStateMachineComponent(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;

    if (stateMachine->currentState)
        wst_leaveState(context, entity, stateMachine->currentState);

    if (stateMachine->nextState)
        wst_leaveState(context, entity, stateMachine->nextState);

    entity->stateMachine = (WarStateMachineComponent){0};
}

void we_addAnimationsComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->animations = (WarAnimationsComponent){0};
    entity->animations.enabled = true;
    WarSpriteAnimationListInit(&entity->animations.animations, WarSpriteAnimationListDefaultOptions);
}

void we_removeAnimationsComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    WarSpriteAnimationListFree(&entity->animations.animations);
    entity->animations = (WarAnimationsComponent){0};
}

void we_addUIComponent(WarContext* context, WarEntity* entity, String name)
{
    NOT_USED(context);

    entity->ui = (WarUIComponent){0};
    entity->ui.enabled = true;
    entity->ui.name = name;
}

void we_removeUIComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->ui = (WarUIComponent){0};
}

void we_addTextComponent(WarContext* context, WarEntity* entity, s32 fontIndex, f32 fontSize, String text)
{
    NOT_USED(context);

    entity->text = (WarTextComponent){0};
    entity->text.enabled = true;
    entity->text.fontIndex = fontIndex;
    entity->text.fontSize = fontSize;
    entity->text.lineHeight = 0;
    entity->text.fontColor = FONT_NORMAL_COLOR;
    entity->text.highlightColor = FONT_HIGHLIGHT_COLOR;
    entity->text.highlightIndex = NO_HIGHLIGHT;
    entity->text.highlightCount = 0;
    entity->text.boundings = VEC2_ZERO;
    entity->text.horizontalAlign = WAR_TEXT_ALIGN_LEFT;
    entity->text.verticalAlign = WAR_TEXT_ALIGN_TOP;
    entity->text.lineAlign = WAR_TEXT_ALIGN_LEFT;
    entity->text.wrapping = WAR_TEXT_WRAP_NONE;
    entity->text.multiline = false;

    wui_setUIText(entity, text);
}

void we_removeTextComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    wui_clearUIText(entity);
    entity->text = (WarTextComponent){0};
}

void we_addRectComponent(WarContext* context, WarEntity* entity, vec2 size, WarColor color)
{
    NOT_USED(context);

    entity->rect = (WarRectComponent){0};
    entity->rect.enabled = true;
    entity->rect.size = size;
    entity->rect.color = color;
}

void we_removeRectComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->rect = (WarRectComponent){0};
}

void we_addButtonComponent(WarContext* context, WarEntity* entity, WarSprite normalSprite, WarSprite pressedSprite)
{
    NOT_USED(context);

    entity->button = (WarButtonComponent){0};
    entity->button.enabled = true;
    entity->button.interactive = true;
    entity->button.hot = false;
    entity->button.active = false;
    entity->button.hotKey = WAR_KEY_NONE;
    entity->button.highlightIndex = NO_HIGHLIGHT;
    entity->button.normalSprite = normalSprite;
    entity->button.pressedSprite = pressedSprite;
    entity->button.clickHandler = NULL;
}

void we_addButtonComponentFromResource(WarContext* context,
                                    WarEntity* entity,
                                    WarSpriteResourceRef normalRef,
                                    WarSpriteResourceRef pressedRef)
{
    if (normalRef.resourceIndex < 0)
    {
        logWarning("Trying to create a button component with invalid resource index: %d", normalRef.resourceIndex);
        return;
    }

    if (pressedRef.resourceIndex < 0)
    {
        logWarning("Trying to create a button component with invalid resource index: %d", pressedRef.resourceIndex);
        return;
    }

    WarSprite normalSprite = wspr_createSpriteFromResourceIndex(context, normalRef);
    WarSprite pressedSprite = wspr_createSpriteFromResourceIndex(context, pressedRef);
    we_addButtonComponent(context, entity, normalSprite, pressedSprite);
}

void we_removeButtonComponent(WarContext* context, WarEntity* entity)
{
    wui_clearUITooltip(entity);
    wspr_freeSprite(context, entity->button.normalSprite);
    wspr_freeSprite(context, entity->button.pressedSprite);
    entity->button = (WarButtonComponent){0};
}

void we_addAudioComponent(WarContext* context, WarEntity* entity, WarAudioType type, s32 resourceIndex, bool loop)
{
    entity->audio = (WarAudioComponent){0};
    entity->audio.enabled = true;
    entity->audio.type = type;
    entity->audio.resourceIndex = resourceIndex;
    entity->audio.loop = loop;

    if (type == WAR_AUDIO_MIDI)
    {
        WarResource* resource = context->resources[resourceIndex];
        u8* midiData = resource->audio.data;
        s32 midiLength = resource->audio.length;

        entity->audio.firstMessage = tml_load_memory(midiData, midiLength);
        entity->audio.currentMessage = entity->audio.firstMessage;
        if (!entity->audio.firstMessage)
        {
            logError("Could not load MIDI from resource: %d", resourceIndex);
        }
    }
}

void we_removeAudioComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    if (entity->audio.firstMessage)
    {
        // this will deallocate the whole list of message
        // because even though it looks like a linked list, it's an array
        tml_free(entity->audio.firstMessage);
    }

    entity->audio.firstMessage = NULL;
    entity->audio.currentMessage = NULL;

    entity->audio = (WarAudioComponent){0};
}

void we_addProjectileComponent(WarContext* context, WarEntity* entity, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target, s32 speed)
{
    NOT_USED(context);

    entity->projectile = (WarProjectileComponent){0};
    entity->projectile.enabled = true;
    entity->projectile.type = type;
    entity->projectile.sourceEntityId = sourceEntityId;
    entity->projectile.targetEntityId = targetEntityId;
    entity->projectile.origin = origin;
    entity->projectile.target = target;
    entity->projectile.speed = speed;
}

void we_removeProjectileComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->projectile = (WarProjectileComponent){0};
}

void we_addCursorComponent(WarContext* context, WarEntity* entity, WarCursorType type, vec2 hot)
{
    NOT_USED(context);

    entity->cursor = (WarCursorComponent){0};
    entity->cursor.enabled = true;
    entity->cursor.type = type;
    entity->cursor.hot = hot;
}

void we_removeCursorComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->cursor = (WarCursorComponent){0};
}

void we_addPoisonCloudComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time)
{
    NOT_USED(context);

    entity->poisonCloud = (WarPoisonCloudComponent){0};
    entity->poisonCloud.enabled = true;
    entity->poisonCloud.position = position;
    entity->poisonCloud.time = time;
    entity->poisonCloud.damageTime = 0;
}

void we_removePoisonCloudComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->poisonCloud = (WarPoisonCloudComponent){0};
}

void we_addSightComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time)
{
    NOT_USED(context);

    entity->sight = (WarSightComponent){0};
    entity->sight.enabled = true;
    entity->sight.position = position;
    entity->sight.time = time;
}

void we_removeSightComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    entity->sight = (WarSightComponent){0};
}

// Entities
WarEntity* we_createEntity(WarContext* context, WarEntityType type, bool addToScene)
{
    WarEntityManager* manager = we_getEntityManager(context);

    WarEntity* entity = (WarEntity *)wm_alloc(sizeof(WarEntity));
    manager->staticEntityId++;
    assert(manager->staticEntityId <= UINT16_MAX);
    entity->id = (WarEntityId)manager->staticEntityId;
    entity->type = type;
    entity->enabled = true;

    // transform component
    entity->transform = (WarTransformComponent){0};
    entity->sprite = (WarSpriteComponent){0};
    entity->unit = (WarUnitComponent){0};
    entity->road = (WarRoadComponent){0};
    entity->wall = (WarWallComponent){0};
    entity->forest = (WarForestComponent){0};
    entity->stateMachine = (WarStateMachineComponent){0};
    entity->animations = (WarAnimationsComponent){0};

    if (addToScene)
    {
        WarEntityListAdd(&manager->entities, entity);
        WarEntityIdMapSet(&manager->entitiesById, entity->id, entity);

        WarEntityList* entitiesOfType = we_getEntitiesOfType(context, type);
        WarEntityListAdd(entitiesOfType, entity);

        if (wui_isUIEntity(entity))
        {
            WarEntityListAdd(&manager->uiEntities, entity);
        }
    }

    return entity;
}

WarEntity* we_createUnit(WarContext* context,
                      WarUnitType type,
                      s32 x,
                      s32 y,
                      u8 player,
                      WarResourceKind resourceKind,
                      u32 amount,
                      bool addToMap)
{
    WarMap* map = context->map;

    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_UNIT, addToMap);
    we_addUnitComponent(context, entity, type, x, y, player, resourceKind, amount);
    we_addTransformComponent(context, entity, vec2i(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT));

    WarUnitData unitData = wu_getUnitData(type);

    s32 spriteIndex = unitData.resourceIndex;
    if (spriteIndex == 0)
    {
        logError("Sprite for unit of type %d is not configure properly. Default to footman sprite.", type);
        spriteIndex = 279;
    }
    we_addSpriteComponentFromResource(context, entity, imageResourceRef(spriteIndex));

    wact_addUnitActions(entity);
    we_addAnimationsComponent(context, entity);
    we_addStateMachineComponent(context, entity);

    if (wu_isDudeUnit(entity))
    {
        WarUnitStats stats = wu_getUnitStats(type);

        entity->unit.maxhp = stats.hp;
        entity->unit.hp = stats.hp;
        entity->unit.maxMana = stats.mana;
        entity->unit.mana = wu_isSummonUnit(entity) ? stats.mana : 100;
        entity->unit.armor = stats.armor;
        entity->unit.range = stats.range;
        entity->unit.minDamage = stats.minDamage;
        entity->unit.rndDamage = stats.rndDamage;
        entity->unit.decay = stats.decay;
        entity->unit.manaTime = 1;
    }
    else if(wu_isBuildingUnit(entity))
    {
        WarBuildingStats stats = wu_getBuildingStats(type);

        entity->unit.maxhp = stats.hp;
        entity->unit.hp = stats.hp;
        entity->unit.armor = stats.armor;
    }

    WarState* idleState = wst_createIdleState(context, entity, wu_isDudeUnit(entity));
    wst_changeNextState(context, entity, idleState, true, true);

    if (addToMap)
    {
        WarEntityManager* manager = &map->entityManager;
        WarEntityList* list = WarUnitMapGet(&manager->unitsByType, type);
        WarEntityListAdd(list, entity);
    }

    return entity;
}

WarEntity* we_createDude(WarContext* context,
                      WarUnitType type,
                      s32 x,
                      s32 y,
                      u8 player,
                      bool isGoingToTrain)
{
    assert(wu_isDudeUnitType(type));

    return we_createUnit(context, type, x, y, player, WAR_RESOURCE_NONE, 0, !isGoingToTrain);
}

WarEntity* we_createBuilding(WarContext* context,
                          WarUnitType type,
                          s32 x,
                          s32 y,
                          u8 player,
                          bool isGoingToBuild)
{
    assert(wu_isBuildingUnitType(type));

    WarEntity* entity = we_createUnit(context, type, x, y, player, WAR_RESOURCE_NONE, 0, true);

    if (isGoingToBuild)
    {
        WarBuildingStats stats = wu_getBuildingStats(type);
        WarState* buildState = wst_createBuildState(context, entity, (f32)stats.buildTime);
        wst_changeNextState(context, entity, buildState, true, true);
    }

    return entity;
}

WarEntity* we_findEntity(WarContext* context, WarEntityId id)
{
    WarEntityManager* manager = we_getEntityManager(context);
    return WarEntityIdMapGet(&manager->entitiesById, id);
}

WarEntity* we_findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type)
{
    WarEntity* result = NULL;
    s32 minDst = INT32_MAX;

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    assert(units);

    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* target = units->items[i];
        if (isUnitOfType(target, type))
        {
            s32 dst = wu_unitDistanceInTiles(entity, target);
            if (dst < minDst)
            {
                result = target;
                minDst = dst;
            }
        }
    }

    return result;
}

WarEntity* we_findUIEntity(WarContext* context, StringView name)
{
    WarEntityList* entities = we_getUIEntities(context);
    for (s32 i = 0; i < entities->count; i++)
    {
        WarEntity* entity = entities->items[i];
        if (entity &&
            wui_isUIEntity(entity) &&
            wsv_equals(wsv_fromString(&entity->ui.name), name))
        {
            return entity;
        }
    }

    return NULL;
}

WarEntity* we_findEntityUnderCursor(WarContext* context, bool includeTrees, bool includeWalls)
{
    WarInput* input = &context->input;

    WarMap* map = context->map;
    assert(map);

    vec2 targetPoint = wmap_vec2ScreenToMapCoordinates(context, input->pos);
    vec2 targetTile = wmap_vec2MapToTileCoordinates(targetPoint);

    WarEntity* entityUnderCursor = NULL;

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            WarUnitComponent* unit = &entity->unit;
            if (unit->enabled)
            {
                // don't change the cursor for dead units or corpses
                if (isDead(entity) || isGoingToDie(entity) || wu_isCorpseUnit(entity))
                {
                    continue;
                }

                // don't change the cursor for collased buildings
                if (isCollapsing(entity) || isGoingToCollapse(entity))
                {
                    continue;
                }

                // don't change the cursor for non-visible units
                if (!isUnitPartiallyVisible(map, entity))
                {
                    continue;
                }
            }

            rect unitRect = wu_getUnitRect(entity);
            if (rectContainsf(unitRect, targetPoint.x, targetPoint.y))
            {
                entityUnderCursor = entity;
                break;
            }
        }
    }

    if (includeTrees && !entityUnderCursor)
    {
        WarEntityList* forests = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_FOREST);
        for (s32 i = 0; i < forests->count; i++)
        {
            WarEntity* forest = forests->items[i];
            if (forest)
            {
                WarTreeList* trees = &forest->forest.trees;
                for (s32 k = 0; k < trees->count; k++)
                {
                    WarTree tree = trees->items[k];
                    if (tree.tilex == (s32)targetTile.x &&
                        tree.tiley == (s32)targetTile.y)
                    {
                        if (tree.type != WAR_TREE_NONE &&
                            tree.type != WAR_TREE_CHOPPED)
                        {
                            entityUnderCursor = forest;
                        }

                        break;
                    }
                }
            }

            if (entityUnderCursor)
            {
                break;
            }
        }
    }

    // if there is no unit under the cursor, check the walls
    if (includeWalls && !entityUnderCursor)
    {
        WarEntityList* walls = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_WALL);
        for (s32 i = 0; i < walls->count; i++)
        {
            WarEntity* wall = walls->items[i];
            if (wall)
            {
                WarWallPieceList* pieces = &wall->wall.pieces;
                for (s32 k = 0; k < pieces->count; k++)
                {
                    WarWallPiece piece = pieces->items[k];
                    if (piece.tilex == (s32)targetTile.x &&
                        piece.tiley == (s32)targetTile.y)
                    {
                        if (piece.hp > 0)
                        {
                            entityUnderCursor = wall;
                        }

                        break;
                    }
                }
            }

            if (entityUnderCursor)
            {
                break;
            }
        }
    }

    return entityUnderCursor;
}

void we_removeEntity(WarContext* context, WarEntity* entity)
{
    we_removeTransformComponent(context, entity);
    we_removeSpriteComponent(context, entity);
    we_removeRoadComponent(context, entity);
    we_removeWallComponent(context, entity);
    we_removeRuinComponent(context, entity);
    we_removeForestComponent(context, entity);
    we_removeUnitComponent(context, entity);
    we_removeStateMachineComponent(context, entity);
    we_removeAnimationsComponent(context, entity);
    we_removeUIComponent(context, entity);
    we_removeTextComponent(context, entity);
    we_removeRectComponent(context, entity);
    we_removeButtonComponent(context, entity);
    we_removeAudioComponent(context, entity);
    we_removeCursorComponent(context, entity);
}

void we_removeEntityById(WarContext* context, WarEntityId id)
{
    WarEntityManager* manager = we_getEntityManager(context);

    WarEntity* entity = we_findEntity(context, id);
    if (entity)
    {
        SDL_LockMutex(context->__mutex);

        we_removeEntity(context, entity);

        if (wui_isUIEntity(entity))
        {
            WarEntityListRemove(&manager->uiEntities, entity);
        }
        else if (isUnit(entity))
        {
            WarEntityList* unitTypeList = WarUnitMapGet(&manager->unitsByType, entity->unit.type);
            WarEntityListRemove(unitTypeList, entity);
        }

        WarEntityList* entityTypeList = WarEntityMapGet(&manager->entitiesByType, entity->type);
        WarEntityListRemove(entityTypeList, entity);

        WarEntityIdMapRemove(&manager->entitiesById, entity->id);
        WarEntityListRemove(&manager->entities, entity);

        SDL_UnlockMutex(context->__mutex);
    }
}

bool we_isStaticEntity(WarEntity* entity)
{
    if (isUnit(entity))
    {
        switch (entity->unit.type)
        {
            case WAR_UNIT_FARM_HUMANS:
            case WAR_UNIT_FARM_ORCS:
            case WAR_UNIT_BARRACKS_HUMANS:
            case WAR_UNIT_BARRACKS_ORCS:
            case WAR_UNIT_CHURCH:
            case WAR_UNIT_TEMPLE:
            case WAR_UNIT_TOWER_HUMANS:
            case WAR_UNIT_TOWER_ORCS:
            case WAR_UNIT_TOWNHALL_HUMANS:
            case WAR_UNIT_TOWNHALL_ORCS:
            case WAR_UNIT_LUMBERMILL_HUMANS:
            case WAR_UNIT_LUMBERMILL_ORCS:
            case WAR_UNIT_STABLE:
            case WAR_UNIT_KENNEL:
            case WAR_UNIT_BLACKSMITH_HUMANS:
            case WAR_UNIT_BLACKSMITH_ORCS:
            case WAR_UNIT_STORMWIND:
            case WAR_UNIT_BLACKROCK:
            case WAR_UNIT_GOLDMINE:
            case WAR_UNIT_HUMAN_CORPSE:
            case WAR_UNIT_ORC_CORPSE:
            case WAR_UNIT_WOUNDED:
                return true;

            default:
                return false;
        }
    }

    return false;
}

void we_initEntityManager(WarContext* context, WarEntityManager* manager)
{
    manager->staticEntityId = 0;

    // initialize entities list
    WarEntityListInit(&manager->entities, WarEntityListDefaultOptions);

    // initialize entity by type map
    WarEntityMapOptions entitiesByTypeOptions = (WarEntityMapOptions){0};
    entitiesByTypeOptions.defaultValue = NULL;
    entitiesByTypeOptions.hashFn = hashEntityType;
    entitiesByTypeOptions.equalsFn = equalsEntityType;
    entitiesByTypeOptions.freeFn = freeEntityList;
    WarEntityMapInit(&manager->entitiesByType, entitiesByTypeOptions);
    for (WarEntityType type = WAR_ENTITY_TYPE_IMAGE; type < WAR_ENTITY_TYPE_COUNT; type++)
    {
        WarEntityList* list = (WarEntityList*)wm_alloc(sizeof(WarEntityList));
        WarEntityListInit(list, WarEntityListNonFreeOptions);
        WarEntityMapSet(&manager->entitiesByType, type, list);
    }

    // initialize unit by type map
    WarUnitMapOptions unitsByTypeOptions = (WarUnitMapOptions){0};
    unitsByTypeOptions.defaultValue = NULL;
    unitsByTypeOptions.hashFn = hashUnitType;
    unitsByTypeOptions.equalsFn = equalsUnitType;
    unitsByTypeOptions.freeFn = freeEntityList;
    WarUnitMapInit(&manager->unitsByType, unitsByTypeOptions);
    for (WarUnitType type = WAR_UNIT_FOOTMAN; type < WAR_UNIT_COUNT; type++)
    {
        WarEntityList* list = (WarEntityList*)wm_alloc(sizeof(WarEntityList));
        WarEntityListInit(list, WarEntityListNonFreeOptions);
        WarUnitMapSet(&manager->unitsByType, type, list);
    }

    // initialize the entities by id map
    WarEntityIdMapOptions entitiesByIdOptions = (WarEntityIdMapOptions){0};
    entitiesByIdOptions.defaultValue = NULL;
    entitiesByIdOptions.hashFn = hashEntityId;
    entitiesByIdOptions.equalsFn = equalsEntityId;
    WarEntityIdMapInit(&manager->entitiesById, entitiesByIdOptions);

    // initialize ui entities list
    WarEntityListInit(&manager->uiEntities, WarEntityListNonFreeOptions);
}

WarEntityManager* we_getEntityManager(WarContext* context)
{
    if (context->scene)
        return &context->scene->entityManager;

    if (context->map)
        return &context->map->entityManager;

    logError("There is no map or scene active.");
    return NULL;
}

WarEntityList* we_getEntities(WarContext* context)
{
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->entities;
}

WarEntityList* we_getEntitiesOfType(WarContext* context, WarEntityType type)
{
    WarEntityManager* manager = we_getEntityManager(context);
    return WarEntityMapGet(&manager->entitiesByType, type);
}

WarEntityList* we_getUnitsOfType(WarContext* context, WarUnitType type)
{
    WarEntityManager* manager = we_getEntityManager(context);
    return WarUnitMapGet(&manager->unitsByType, type);
}

WarEntityList* we_getUIEntities(WarContext* context)
{
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->uiEntities;
}

// Render entities
s32 renderCompareUnits(const WarEntity* e1, const WarEntity* e2)
{
    assert(isUnit(e1));
    assert(isUnit(e2));

    bool isDead1 = wu_isCorpseUnit((WarEntity*)e1) || isDead((WarEntity*)e1) || isGoingToDie((WarEntity*)e1);
    bool isDead2 = wu_isCorpseUnit((WarEntity*)e2) || isDead((WarEntity*)e2) || isGoingToDie((WarEntity*)e2);

    if (isDead1 && !isDead2)
        return -1;

    if (!isDead1 && isDead2)
        return 1;

    vec2 p1 = wu_getUnitPosition((WarEntity*)e1, false);
    vec2 p2 = wu_getUnitPosition((WarEntity*)e2, false);

    return (s32)(p1.y - p2.y);
}

s32 renderCompareProjectiles(const WarEntity* e1, const WarEntity* e2)
{
    vec2 p1 = e1->transform.position;
    vec2 p2 = e2->transform.position;

    return (s32)(p1.y - p2.y);
}

void renderImage(WarContext* context, WarEntity* entity)
{
    WarTransformComponent transform = entity->transform;
    WarUIComponent* ui = &entity->ui;
    WarSpriteComponent* sprite = &entity->sprite;

    if (ui->enabled && sprite->enabled && sprite->frameIndex >= 0)
    {
        wr_renderSave(context);

        if (sprite->sprite.framesCount > 1)
        {
            WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite->sprite, sprite->frameIndex);
            wspr_updateSpriteImage(context, sprite->sprite, frame.data);

            wr_renderTranslate(context, -(f32)frame.dx, -(f32)frame.dy);
        }

        wr_renderTranslate(context, transform.position.x, transform.position.y);
        wspr_renderSprite(context, sprite->sprite, VEC2_ZERO, VEC2_ONE);
        wr_renderRestore(context);
    }
}

void renderRoad(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    WarRoadComponent* road = &entity->road;

    // the roads are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled && road->enabled)
    {
        WarRoadPieceList* pieces = &road->pieces;

        for (s32 i = 0; i < pieces->count; i++)
        {
            // get the index of the tile in the spritesheet of the map,
            // corresponding to the current tileset type (forest, swamp)
            WarRoadData roadData = wu_getRoadData(pieces->items[i].type);

            s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                ? roadData.tileIndexForest : roadData.tileIndexSwamp;

            // the position in the world of the road piece tile
            s32 x = pieces->items[i].tilex;
            s32 y = pieces->items[i].tiley;

            // coordinates in pixels of the road piece tile
            s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            wr_renderSave(context);
            wr_renderTranslate(context, (f32)(x * MEGA_TILE_WIDTH), (f32)(y * MEGA_TILE_HEIGHT));

            rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            wr_renderSubImage(context, sprite->sprite.texture, rs, rd, VEC2_ONE);

            wr_renderRestore(context);
        }
    }
}

void renderWall(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    WarWallComponent* wall = &entity->wall;

    // the walls are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled && wall->enabled)
    {
        WarWallPieceList* pieces = &wall->pieces;

        for (s32 i = 0; i < pieces->count; i++)
        {
            WarWallPiece* piece = &pieces->items[i];

            // get the index of the tile in the spritesheet of the map,
            // corresponding to the current tileset type (forest, swamp)
            WarWallData wallData = wu_getWallData(piece->type);

            s32 tileIndex = 0;

            s32 hpPercent = percentabi(piece->hp, piece->maxhp);
            if (hpPercent <= 0)
            {
                tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? wallData.tileDestroyedForest : wallData.tileDestroyedSwamp;
            }
            else if(hpPercent < 50)
            {
                tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? wallData.tileDamagedForest : wallData.tileDamagedSwamp;
            }
            else
            {
                tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? wallData.tileForest : wallData.tileSwamp;
            }

            // the position in the world of the wall piece tile
            s32 x = piece->tilex;
            s32 y = piece->tiley;

            // coordinates in pixels of the wall piece tile
            s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            wr_renderSave(context);
            wr_renderTranslate(context, (f32)(x * MEGA_TILE_WIDTH), (f32)(y * MEGA_TILE_HEIGHT));

            rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            wr_renderSubImage(context, sprite->sprite.texture, rs, rd, VEC2_ONE);

            wr_renderRestore(context);
        }
    }
}

void renderRuin(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    WarRuinComponent* ruin = &entity->ruin;

    // the walls are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled && ruin->enabled)
    {
        WarRuinPieceList* pieces = &ruin->pieces;

        for (s32 i = 0; i < ruin->pieces.count; i++)
        {
            WarRuinPiece* piece = &pieces->items[i];
            if (piece->type == WAR_RUIN_PIECE_NONE)
                continue;

            // get the index of the tile in the spritesheet of the map,
            // corresponding to the current tileset type (forest, swamp)
            WarRuinData ruinData = wu_getRuinData(piece->type);

            s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                ? ruinData.tileIndexForest : ruinData.tileIndexSwamp;

            // the position in the world of the road piece tile
            s32 x = piece->tilex;
            s32 y = piece->tiley;

            // coordinates in pixels of the road piece tile
            s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            wr_renderSave(context);
            wr_renderTranslate(context, (f32)(x * MEGA_TILE_WIDTH), (f32)(y * MEGA_TILE_HEIGHT));

            rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            wr_renderSubImage(context, sprite->sprite.texture, rs, rd, VEC2_ONE);

            wr_renderRestore(context);
        }
    }
}

void renderForest(WarContext* context, WarEntity* entity)
{
    WarForestComponent* forest = &entity->forest;

    // the wood are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (forest->enabled)
    {
        WarTreeList* trees = &forest->trees;
        for (s32 i = 0; i < trees->count; i++)
        {
            WarTree* tree = &trees->items[i];

            if (tree->type == WAR_TREE_NONE)
                continue;

            WarTreeData data = wu_getTreeData(tree->type);

            // the position in the world of the wood tile
            s32 x = tree->tilex;
            s32 y = tree->tiley;

            s32 prevTileIndex = wmap_getMapTileIndex(context, x, y);
            s32 newTileIndex = (tilesetType == MAP_TILESET_FOREST) ? data.tileIndexForest : data.tileIndexSwamp;

            if (prevTileIndex != newTileIndex)
                logDebug("different tile index for tree (%d, %d), prev: %d, new: %d", x, y, prevTileIndex, newTileIndex);

            wmap_setMapTileIndex(context, x, y, newTileIndex);
        }
    }
}

void renderUnit(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    WarUnitComponent* unit = &entity->unit;
    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarAnimationsComponent* animations = &entity->animations;

    // position of the unit in the map
    vec2 position = transform->position;

    // scale of the unit: this is modified by animations when the animation indicates that it
    // should flip horizontally or vertically or both
    vec2 scale = transform->scale;

    // size of the original sprite
    vec2 frameSize = wu_getUnitFrameSize(entity);

    // size of the unit
    vec2 unitSize = wu_getUnitSpriteSize(entity);

    // the unit is visible if it's partially on the clear areas of the fog
    bool isVisible = isUnitPartiallyVisible(map, entity);

    wr_renderTranslate(context, -halff(frameSize.x), -halff(frameSize.y));
    wr_renderTranslate(context, halff(unitSize.x), halff(unitSize.y));
    wr_renderTranslate(context, position.x, position.y);

#ifdef DEBUG_RENDER_UNIT_INFO
    wr_renderFillRect(context, wu_getUnitFrameRect(entity), WAR_COLOR_RGBA(0, 0, 128, 128));
    wr_renderFillRect(context, wu_getUnitSpriteRect(entity), WAR_COLOR_GRAY_TRANSPARENT);
    wr_renderFillRect(context, rectv(wu_getUnitSpriteCenter(entity), VEC2_ONE), WAR_COLOR_RGB(255, 0, 0));
#endif

#ifdef DEBUG_RENDER_UNIT_STATS
    // NOTE: TTF text rendering not available in SDL_Renderer path.
    // Use sprite text if needed in the future.
#endif

    if (sprite->enabled && (isVisible || unit->hasBeenSeen))
    {
        wr_renderSave(context);

        if (wu_isDudeUnit(entity))
        {
            WarUnitComponent* unitComponent = &entity->unit;

            if (unitComponent->invisible)
            {
                wr_renderGlobalAlpha(context, 0.5f);
            }

            if (unitComponent->invulnerable)
            {
                rect unitRect = wu_getUnitSpriteRect(entity);
                unitRect = rectExpand(unitRect, -1, -1);
                wr_renderStrokeRect(context, unitRect, WAR_COLOR_BLUE_INVULNERABLE, 1);
            }
        }

        WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite->sprite, sprite->frameIndex);
        wspr_updateSpriteImage(context, sprite->sprite, frame.data);
        wspr_renderSprite(context, sprite->sprite, VEC2_ZERO, scale);

        wr_renderRestore(context);
    }

    if (animations->enabled && isVisible)
    {
        for (s32 i = 0; i < animations->animations.count; i++)
        {
            WarSpriteAnimation* anim = animations->animations.items[i];
            if (anim->status == WAR_ANIM_STATUS_RUNNING)
            {
                wr_renderSave(context);

                wr_renderTranslate(context, anim->offset.x, anim->offset.y);
                wr_renderScale(context, anim->scale.x, anim->scale.y);

#ifdef DEBUG_RENDER_UNIT_ANIMATIONS
                // size of the original sprite
                vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

                wr_renderFillRect(context, rectv(VEC2_ZERO, animFrameSize), WAR_COLOR_GRAY_TRANSPARENT);
#endif

                s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
                animFrameIndex = clamp(animFrameIndex, 0, anim->frames.count - 1);

                s32 spriteFrameIndex = anim->frames.items[animFrameIndex];
                WarSpriteFrame frame = wspr_getSpriteFrame(context, anim->sprite, spriteFrameIndex);

                wspr_updateSpriteImage(context, anim->sprite, frame.data);
                wspr_renderSprite(context, anim->sprite, VEC2_ZERO, VEC2_ONE);

                wr_renderRestore(context);
            }
        }
    }
}

void renderText(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarUIComponent* ui = &entity->ui;
    WarTextComponent* text = &entity->text;

    if (ui->enabled && text->enabled && text->text.data)
    {
        wr_renderSave(context);
        wr_renderTranslate(context, transform->position.x, transform->position.y);
        wr_renderScale(context, transform->scale.x, transform->scale.y);

        WarFontParams params;
        params.fontIndex = text->fontIndex;
        params.fontSize = text->fontSize;
        params.lineHeight = text->lineHeight;
        params.fontColor = text->fontColor;
        params.highlightColor = text->highlightColor;
        params.highlightIndex = text->highlightIndex;
        params.highlightCount = text->highlightCount;
        params.boundings = text->boundings;
        params.horizontalAlign = text->horizontalAlign;
        params.verticalAlign = text->verticalAlign;
        params.lineAlign = text->lineAlign;
        params.wrapping = text->wrapping;
        params.trimming = text->trimming;
        params.fontSprite = context->fontSprites[text->fontIndex];
        params.fontData = getFontData(text->fontIndex);

        if (entity->text.multiline)
            wfont_renderMultiSpriteText(context, wstr_view(&text->text), 0, 0, params);
        else
            wfont_renderSingleSpriteText(context, wstr_view(&text->text), 0, 0, params);

        wr_renderRestore(context);
    }
}

void renderRect(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarUIComponent* ui = &entity->ui;
    WarRectComponent* rect = &entity->rect;

    if (ui->enabled && rect->enabled)
    {
        wr_renderSave(context);
        wr_renderTranslate(context, transform->position.x, transform->position.y);
        wr_renderScale(context, transform->scale.x, transform->scale.y);

        wr_renderFillRect(context, rectf(0.0f, 0.0f, rect->size.x, rect->size.y), rect->color);

        wr_renderRestore(context);
    }
}

void renderButton(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarUIComponent* ui = &entity->ui;
    WarButtonComponent* button = &entity->button;
    WarTextComponent* text = &entity->text;
    WarSpriteComponent* sprite = &entity->sprite;

    if (ui->enabled && button->enabled)
    {
        wr_renderSave(context);
        wr_renderTranslate(context, transform->position.x, transform->position.y);
        wr_renderScale(context, transform->scale.x, transform->scale.y);

        // render background
        {
            WarSprite backgroundSprite = button->active ? button->pressedSprite : button->normalSprite;
            wspr_renderSprite(context, backgroundSprite, VEC2_ZERO, VEC2_ONE);
        }

        // render foreground
        {
            if (sprite->enabled)
            {
                vec2 backgroundSize = vec2i(button->normalSprite.frameWidth, button->normalSprite.frameHeight);
                vec2 foregroundSize = vec2i(sprite->sprite.frameWidth, sprite->sprite.frameHeight);
                vec2 offset = vec2Half(vec2Subv(backgroundSize, foregroundSize));

                if (button->active)
                    offset = vec2Addv(offset, vec2i(0, 1));

                wr_renderTranslate(context, offset.x, offset.y);

                WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite->sprite, sprite->frameIndex);
                wspr_updateSpriteImage(context, sprite->sprite, frame.data);
                wspr_renderSprite(context, sprite->sprite, VEC2_ZERO, VEC2_ONE);
            }
        }

        // render text
        {
            if (text->enabled)
            {
                WarFontParams params;
                params.fontIndex = text->fontIndex;
                params.fontSize = text->fontSize;
                params.fontColor = text->fontColor;
                params.highlightColor = text->highlightColor;
                params.highlightIndex = button->hot ? ALL_HIGHLIGHT : text->highlightIndex;
                params.highlightCount = text->highlightCount;
                params.boundings = text->boundings;
                params.horizontalAlign = text->horizontalAlign;
                params.verticalAlign = text->verticalAlign;
                params.lineAlign = text->lineAlign;
                params.wrapping = text->wrapping;
                params.fontSprite = context->fontSprites[text->fontIndex];
                params.fontData = getFontData(text->fontIndex);

                wfont_renderSingleSpriteText(context, wstr_view(&text->text), 0, 0, params);
            }
        }

        wr_renderRestore(context);
    }
}

void renderProjectile(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;

    vec2 position = transform->position;
    vec2 scale = transform->scale;

    if (sprite->enabled)
    {
        WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite->sprite, sprite->frameIndex);

#ifdef DEBUG_RENDER_PROJECTILES
        {
            wr_renderSave(context);

            wr_renderTranslate(context, -halfi(sprite->sprite.frameWidth),-halfi(sprite->sprite.frameHeight));
            wr_renderTranslate(context, position.x, position.y);

            rect r = rectf(0, 0, sprite->sprite.frameWidth, sprite->sprite.frameHeight);
            wr_renderFillRect(context, r, WAR_COLOR_GRAY_TRANSPARENT);

            wr_renderRestore(context);
        }

        {
            wr_renderSave(context);

            wr_renderTranslate(context, -halfi(frame.w),-halfi(frame.h));
            wr_renderTranslate(context, position.x, position.y);

            rect r = rectf(0, 0, frame.w, frame.h);
            wr_renderFillRect(context, r, WAR_COLOR_RED_TRANSPARENT);

            wr_renderRestore(context);
        }

        {
            WarProjectileComponent* projectile = &entity->projectile;

            wr_renderSave(context);

            wr_renderStrokeLine(context, projectile->origin, projectile->target, getColorFromList(entity->id), 0.5f);
            wr_renderFillRect(context, rectv(projectile->origin, VEC2_ONE), WAR_COLOR_RGB(255, 0, 255));
            wr_renderFillRect(context, rectv(projectile->target, VEC2_ONE), WAR_COLOR_RGB(255, 0, 255));

            wr_renderRestore(context);
        }
#endif

        wr_renderTranslate(context, -(f32)frame.dx, -(f32)frame.dy);
        wr_renderTranslate(context, -halff(frame.w), -halff(frame.h));
        wr_renderTranslate(context, position.x, position.y);

        wspr_updateSpriteImage(context, sprite->sprite, frame.data);
        wspr_renderSprite(context, sprite->sprite, VEC2_ZERO, scale);
    }
}

void renderMinimap(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "RenderMinimap", 1);
    WarMap* map = context->map;

    vec2 position = entity->transform.position;

    WarSpriteFrame* frame0 = &map->minimapSprite.frames[0];
    WarSpriteFrame* frame1 = &map->minimapSprite.frames[1];

    // copy the minimap base to the first frame which is the one that will be rendered
    // copy only the visible tiles/pixels
    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            s32 index = y * MAP_TILES_WIDTH + x;
            WarMapTile* tile = &map->tiles[index];

            if (!map->fowEnabled ||
                tile->state == MAP_TILE_STATE_VISIBLE ||
                tile->state == MAP_TILE_STATE_FOG)
            {
                frame0->data[index * 4 + 0] = frame1->data[index * 4 + 0];
                frame0->data[index * 4 + 1] = frame1->data[index * 4 + 1];
                frame0->data[index * 4 + 2] = frame1->data[index * 4 + 2];
                frame0->data[index * 4 + 3] = frame1->data[index * 4 + 3];
            }
            else
            {
                frame0->data[index * 4 + 0] = 0;
                frame0->data[index * 4 + 1] = 0;
                frame0->data[index * 4 + 2] = 0;
                frame0->data[index * 4 + 3] = 255;
            }
        }
    }

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* unitEntity = units->items[i];
        if (unitEntity)
        {
            WarUnitComponent* unit = &unitEntity->unit;
            WarTransformComponent* transform = &unitEntity->transform;

            if (wu_displayUnitOnMinimap(unitEntity) && (isUnitPartiallyVisible(map, unitEntity) || unitEntity->unit.hasBeenSeen))
            {
                s32 tileX = (s32)(transform->position.x / MEGA_TILE_WIDTH);
                s32 tileY = (s32)(transform->position.y / MEGA_TILE_HEIGHT);

                WarColor color = wu_getUnitColorOnMinimap(unitEntity);

                for(s32 y = 0; y < unit->sizey; y++)
                {
                    for(s32 x = 0; x < unit->sizex; x++)
                    {
                        s32 pixel = (tileY + y) * MINIMAP_WIDTH + (tileX + x);
                        frame0->data[pixel * 4 + 0] = color.r;
                        frame0->data[pixel * 4 + 1] = color.g;
                        frame0->data[pixel * 4 + 2] = color.b;
                    }
                }
            }
        }
    }

    wr_renderSave(context);
    wr_renderTranslate(context, position.x, position.y);

    // render base
    wspr_updateSpriteImage(context, map->minimapSprite, map->minimapSprite.frames[0].data);
    wspr_renderSprite(context, map->minimapSprite, VEC2_ZERO, VEC2_ONE);

    // render viewport
    wr_renderTranslate(context, (f32)map->viewport.x * MINIMAP_MAP_WIDTH_RATIO, (f32)map->viewport.y * MINIMAP_MAP_HEIGHT_RATIO);
    wr_renderStrokeRect(context, rectf(0.0f, 0.0f, (f32)MINIMAP_VIEWPORT_WIDTH, (f32)MINIMAP_VIEWPORT_HEIGHT), WAR_COLOR_WHITE, 1.0f);

    wr_renderRestore(context);
    TracyCZoneEnd(ctx);
}

void renderAnimation(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarAnimationsComponent* animations = &entity->animations;

    vec2 position = transform->position;
    wr_renderTranslate(context, position.x, position.y);

    if (animations->enabled)
    {
        for (s32 i = 0; i < animations->animations.count; i++)
        {
            WarSpriteAnimation* anim = animations->animations.items[i];
            if (anim->status == WAR_ANIM_STATUS_RUNNING)
            {
                wr_renderSave(context);

                wr_renderTranslate(context, anim->offset.x, anim->offset.y);
                wr_renderScale(context, anim->scale.x, anim->scale.y);

                s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
                animFrameIndex = clamp(animFrameIndex, 0, anim->frames.count - 1);

                s32 spriteFrameIndex = anim->frames.items[animFrameIndex];
                WarSpriteFrame frame = wspr_getSpriteFrame(context, anim->sprite, spriteFrameIndex);

                wspr_updateSpriteImage(context, anim->sprite, frame.data);
                wspr_renderSprite(context, anim->sprite, VEC2_ZERO, VEC2_ONE);

                wr_renderRestore(context);
            }
        }
    }
}

void we_renderEntity(WarContext* context, WarEntity* entity)
{
    static WarRenderFunc renderFuncs[WAR_ENTITY_TYPE_COUNT] =
    {
        NULL,               // WAR_ENTITY_TYPE_NONE
        renderImage,        // WAR_ENTITY_TYPE_IMAGE
        renderUnit,         // WAR_ENTITY_TYPE_UNIT
        renderRoad,         // WAR_ENTITY_TYPE_ROAD
        renderWall,         // WAR_ENTITY_TYPE_WALL
        renderRuin,         // WAR_ENTITY_TYPE_RUIN
        renderForest,       // WAR_ENTITY_TYPE_FOREST
        renderText,         // WAR_ENTITY_TYPE_TEXT
        renderRect,         // WAR_ENTITY_TYPE_RECT
        renderButton,       // WAR_ENTITY_TYPE_BUTTON
        renderImage,        // WAR_ENTITY_TYPE_CURSOR
        NULL,               // WAR_ENTITY_TYPE_AUDIO
        renderProjectile,   // WAR_ENTITY_TYPE_PROJECTILE
        NULL,               // WAR_ENTITY_TYPE_RAIN_OF_FIRE
        renderAnimation,    // WAR_ENTITY_TYPE_POISON_CLOUD
        NULL,               // WAR_ENTITY_TYPE_SIGHT
        renderMinimap,      // WAR_ENTITY_TYPE_MINIMAP
        renderAnimation,    // WAR_ENTITY_TYPE_ANIMATION
    };

    if (entity->id && entity->enabled)
    {
        WarRenderFunc renderFunc = renderFuncs[(s32)entity->type];
        if (!renderFunc)
        {
            logError("Entity of type %d can't be render. renderFunc = NULL", entity->type);
            return;
        }

        wr_renderSave(context);
        renderFunc(context, entity);
        wr_renderRestore(context);
    }
}

void we_renderEntitiesOfType(WarContext* context, WarEntityType type)
{
    TracyCZoneN(ctx, "RenderEntities", 1);
    static WarRenderCompareFunc renderCompareFuncs[WAR_ENTITY_TYPE_COUNT] =
    {
        NULL,                       // WAR_ENTITY_TYPE_NONE,
        NULL,                       // WAR_ENTITY_TYPE_IMAGE,
        renderCompareUnits,         // WAR_ENTITY_TYPE_UNIT,
        NULL,                       // WAR_ENTITY_TYPE_ROAD,
        NULL,                       // WAR_ENTITY_TYPE_WALL,
        NULL,                       // WAR_ENTITY_TYPE_RUIN,
        NULL,                       // WAR_ENTITY_TYPE_FOREST,
        NULL,                       // WAR_ENTITY_TYPE_TEXT,
        NULL,                       // WAR_ENTITY_TYPE_RECT,
        NULL,                       // WAR_ENTITY_TYPE_BUTTON,
        NULL,                       // WAR_ENTITY_TYPE_CURSOR,
        NULL,                       // WAR_ENTITY_TYPE_AUDIO,
        renderCompareProjectiles,   // WAR_ENTITY_TYPE_PROJECTILE,
        NULL,                       // WAR_ENTITY_TYPE_RAIN_OF_FIRE,
        NULL,                       // WAR_ENTITY_TYPE_POISON_CLOUD,
        NULL,                       // WAR_ENTITY_TYPE_SIGHT,
        NULL,                       // WAR_ENTITY_TYPE_MINIMAP,
        NULL,                       // WAR_ENTITY_TYPE_ANIMATION,
    };

    WarEntityList* entities = we_getEntitiesOfType(context, type);

    // lookup the render compare function and sort the list
    if (inRange(type, WAR_ENTITY_TYPE_NONE, WAR_ENTITY_TYPE_COUNT))
    {
        WarRenderCompareFunc compareFunc = renderCompareFuncs[type];
        if (compareFunc)
        {
            // the idea of sorting the entities before render is
            // to allow render order to be the most correct as possible
            // for instance, corpses need to render before every other unit
            // and then render units by the `y` position on the map

            WarEntityListSort(entities, compareFunc);
        }
    }

    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity* entity = entities->items[i];
        if (entity)
        {
            we_renderEntity(context, entity);
        }
    }
    TracyCZoneEnd(ctx);
}

void we_renderUnitSelection(WarContext* context)
{
    WarMap* map = context->map;

    WarEntityIdList* selectedEntities = &map->selectedEntities;
    for (s32 i = 0; i < selectedEntities->count; i++)
    {
        WarEntityId entityId = selectedEntities->items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        if (entity)
        {
            WarTransformComponent* transform = &entity->transform;
            WarSpriteComponent* sprite = &entity->sprite;

            if (sprite->enabled)
            {
                // size of the original sprite
                vec2 frameSize = wu_getUnitFrameSize(entity);

                // size of the unit
                vec2 unitSize = wu_getUnitSpriteSize(entity);

                // position of the unit in the map
                vec2 position = transform->position;

                wr_renderSave(context);
                wr_renderTranslate(context, -halff(frameSize.x), -halff(frameSize.y));
                wr_renderTranslate(context, halff(unitSize.x), halff(unitSize.y));
                wr_renderTranslate(context, position.x, position.y);

                rect selr = rectf(halff(frameSize.x - unitSize.x), halff(frameSize.y - unitSize.y), unitSize.x, unitSize.y);
                WarColor color = WAR_COLOR_WHITE_SELECTION;
                if (wu_isFriendlyUnit(context, entity))
                    color = WAR_COLOR_GREEN_SELECTION;
                else if (wu_isEnemyUnit(context, entity))
                    color = WAR_COLOR_RED_SELECTION;
                wr_renderStrokeRect(context, selr, color, 1.0f);

                wr_renderRestore(context);
            }
        }
    }
}

void we_increaseUpgradeLevel(WarContext* context, WarPlayerInfo* player, WarUpgradeType upgrade)
{
    assert(hasRemainingUpgrade(player, upgrade));

    incrementUpgradeLevel(player, upgrade);

    switch (upgrade)
    {
        case WAR_UPGRADE_ARROWS:
        case WAR_UPGRADE_SPEARS:
        {
            WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                if (entity && entity->unit.player == player->index)
                {
                    if (entity->unit.type == WAR_UNIT_ARCHER ||
                        entity->unit.type == WAR_UNIT_SPEARMAN)
                    {
                        entity->unit.minDamage += 2;
                    }
                }
            }

            break;
        }

        case WAR_UPGRADE_SWORDS:
        case WAR_UPGRADE_AXES:
        {
            WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                if (entity && entity->unit.player == player->index)
                {
                    if (entity->unit.type == WAR_UNIT_FOOTMAN ||
                        entity->unit.type == WAR_UNIT_GRUNT ||
                        entity->unit.type == WAR_UNIT_KNIGHT ||
                        entity->unit.type == WAR_UNIT_RAIDER)
                    {
                        if (getUpgradeLevel(player, upgrade) == 1)
                        {
                            entity->unit.minDamage += 1;
                            entity->unit.rndDamage += 1;
                        }
                        else if (getUpgradeLevel(player, upgrade) == 2)
                        {
                            entity->unit.minDamage += 2;
                            entity->unit.rndDamage += 2;
                        }
                    }
                }
            }

            break;
        }

        case WAR_UPGRADE_HORSES:
        case WAR_UPGRADE_WOLVES:
        {
            WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                if (entity && entity->unit.player == player->index)
                {
                    if (wu_isDudeUnit(entity))
                    {
                        entity->unit.speed++;
                    }
                }
            }

            break;
        }

        case WAR_UPGRADE_SHIELD:
        {
            WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                if (entity && entity->unit.player == player->index)
                {
                    if (getUpgradeLevel(player, upgrade) == 1)
                    {
                        entity->unit.armor += 1;
                    }
                    else if (getUpgradeLevel(player, upgrade) == 2)
                    {
                        entity->unit.armor += 2;
                    }
                }
            }

            break;
        }

        default:
        {
            logInfo("This upgrade type %d doesn't increase any value of the units", upgrade);
            break;
        }
    }
}

bool we_enoughPlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood)
{
    NOT_USED(context);

    return player->gold >= gold && player->wood >= wood;
}

bool we_decreasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood)
{
    if (player->gold < gold)
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("NOT ENOUGH GOLD... MINE MORE GOLD"));
        return false;
    }

    if (player->wood < wood)
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("NOT ENOUGH LUMBER... CHOP MORE TREES"));
        return false;
    }

    player->gold -= gold;
    player->wood -= wood;
    return true;
}

void we_increasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood)
{
    NOT_USED(context);

    player->gold += gold;
    player->wood += wood;
}

bool we_increaseUnitHp(WarContext* context, WarEntity* entity, s32 hp)
{
    NOT_USED(context);

    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    unit->hp += hp;
    unit->hp = min(unit->hp, unit->maxhp);

    return true;
}

bool we_decreaseUnitHp(WarContext* context, WarEntity* entity, s32 hp)
{
    NOT_USED(context);

    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    unit->hp -= hp;
    unit->hp = max(unit->hp, 0);

    return true;
}

bool we_decreaseUnitMana(WarContext* context, WarEntity* entity, s32 mana)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    if (unit->mana < mana)
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("NOT ENOUGH MANA"));
        return false;
    }

    unit->mana = max(unit->mana - mana, 0);
    return true;
}

void we_increaseUnitMana(WarContext* context, WarEntity* entity, s32 mana)
{
    NOT_USED(context);

    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    unit->mana = clamp(unit->mana + mana, 0, unit->maxMana);
}

bool we_enoughFarmFood(WarContext* context, WarPlayerInfo* player)
{
    WarUnitType farmType = wu_getUnitTypeForRace(WAR_UNIT_FARM_HUMANS, player->race);
    s32 farmCount = wu_getNumberOfBuildingsOfType(context, player->index, farmType, true);
    s32 foodCount = farmCount * 4 + 1;

    s32 dudesCount = wu_getTotalNumberOfDudes(context, player->index);
    return dudesCount + 1 <= foodCount;
}

bool we_checkFarmFood(WarContext* context, WarPlayerInfo* player)
{
    if (!we_enoughFarmFood(context, player))
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("NOT ENOUGH FOOD... BUILD MORE FARMS"));
        return false;
    }

    return true;
}

bool we_checkRectToBuild(WarContext* context, s32 x, s32 y, s32 w, s32 h)
{
    WarMap* map = context->map;

    for (s32 dy = 0; dy < h; dy++)
    {
        for (s32 dx = 0; dx < w; dx++)
        {
            s32 xx = x + dx;
            s32 yy = y + dy;
            if (inRange(xx, 0, MAP_TILES_WIDTH) && inRange(yy, 0, MAP_TILES_HEIGHT))
            {
                if (!isEmpty(map->finder, xx, yy) || isTileUnkown(map, xx, yy))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool we_checkTileToBuild(WarContext* context, WarUnitType buildingToBuild, s32 x, s32 y)
{
    WarUnitData data = wu_getUnitData(buildingToBuild);

    if (!we_checkRectToBuild(context, x, y, data.sizex, data.sizey))
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("CAN'T BUILD THERE"));
        return false;
    }

    return true;
}

bool we_checkTileToBuildRoadOrWall(WarContext* context, s32 x, s32 y)
{
    if (!we_checkRectToBuild(context, x, y, 1, 1))
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("CAN'T BUILD THERE"));
        return false;
    }

    return true;
}

WarEntityList* we_getNearUnits(WarContext* context, vec2 tilePosition, s32 distance)
{
    TracyCZoneN(ctx, "GetNearUnits", 1);
    WarEntityList* nearUnits = (WarEntityList*)wm_alloc(sizeof(WarEntityList));
    WarEntityListInit(nearUnits, WarEntityListNonFreeOptions);

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (other)
        {
            if (wu_tileInRange(other, tilePosition, distance))
            {
                WarEntityListAdd(nearUnits, other);
            }
        }
    }

    TracyCZoneEnd(ctx);
    return nearUnits;
}

WarEntity* we_getNearEnemy(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "GetNearEnemy", 1);
    vec2 position = wu_getUnitCenterPosition(entity, true);

    WarEntityList* entities = we_getEntities(context);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity* other = entities->items[i];
        if (other && wu_areEnemies(context, entity, other) && wu_canAttack(context, entity, other))
        {
            if (isUnit(other))
            {
                if (other->unit.invisible)
                    continue;
            }

            if (wu_tileInRange(other, position, NEAR_ENEMY_RADIUS))
            {
                TracyCZoneEnd(ctx);
                return other;
            }
        }
    }

    TracyCZoneEnd(ctx);
    return NULL;
}

bool we_isBeingAttackedBy(WarEntity* entity, WarEntity* other)
{
    if (!isFollowing(other) && !isMoving(other))
    {
        WarState* attackState = getAttackState(other);
        return attackState && attackState->wcomm_attack.targetEntityId == entity->id;
    }

    return false;
}

bool we_isBeingAttacked(WarContext* context, WarEntity* entity)
{

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (we_isBeingAttackedBy(entity, other))
        {
            return true;
        }
    }

    return false;
}

WarEntity* we_getAttacker(WarContext* context, WarEntity* entity)
{
    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (other && we_isBeingAttackedBy(entity, other))
        {
            return other;
        }
    }

    return NULL;
}

WarEntity* we_getAttackTarget(WarContext* context, WarEntity* entity)
{
    if (!isFollowing(entity) && !isMoving(entity))
    {
        WarState* attackState = getAttackState(entity);
        if (attackState)
        {
            WarEntityId targetEntityId = (WarEntityId)attackState->wcomm_attack.targetEntityId;
            return we_findEntity(context, targetEntityId);
        }
    }

    return NULL;
}

s32 we_getTotalDamage(s32 minDamage, s32 rndDamage, s32 armor)
{
    return minDamage + max(rndDamage - armor, 0);
}

void we_takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage)
{
    WarMap* map = context->map;

    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    if (unit->invulnerable)
        return;

    WarPlayerInfo* player = &map->players[unit->player];
    if (player->godMode)
        return;

    // Minimal damage + [Random damage - Enemy's Armor]
    s32 damage = we_getTotalDamage(minDamage, rndDamage, unit->armor);
    we_decreaseUnitHp(context, entity, damage);

    if (unit->hp == 0)
    {
        if (wu_isBuildingUnit(entity))
        {
            WarState* collapseState = wst_createCollapseState(context, entity);
            wst_changeNextState(context, entity, collapseState, true, true);

            wa_createAudioRandom(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, false);
        }
        else
        {
            vec2 position = wu_getUnitCenterPosition(entity, false);

            WarState* deathState = wst_createDeathState(context, entity);
            wst_changeNextState(context, entity, deathState, true, true);

            if (entity->unit.type == WAR_UNIT_SCORPION ||
                entity->unit.type == WAR_UNIT_SPIDER)
            {
                wa_createAudioWithPosition(context, WAR_DEAD_SPIDER_SCORPION, position, false);
            }
            else if (entity->unit.type == WAR_UNIT_CATAPULT_HUMANS ||
                     entity->unit.type == WAR_UNIT_CATAPULT_ORCS)
            {
                wa_createAudioRandomWithPosition(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, position, false);
            }
            else
            {
                WarAudioId audioId = isHumanUnit(entity)? WAR_HUMAN_DEAD : WAR_ORC_DEAD;
                wa_createAudioWithPosition(context, audioId, position, false);
            }
        }
    }
    else if (wu_isBuildingUnit(entity))
    {
        s32 hpPercent = percentabi(unit->hp, unit->maxhp);
        if(hpPercent <= 33)
        {
            if (!wanim_containsAnimation(context, entity, wsv_fromCString("hugeDamage")))
            {
                wanim_removeAnimation(context, entity, wsv_fromCString("littleDamage"));
                wanim_createDamageAnimation(context, entity, wstr_fromCString("hugeDamage"), 2);
            }
        }
        else if(hpPercent <= 66)
        {
            if (!wanim_containsAnimation(context, entity, wsv_fromCString("littleDamage")))
            {
                wanim_createDamageAnimation(context, entity, wstr_fromCString("littleDamage"), 1);
            }
        }
    }
}

void we_takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage)
{
    NOT_USED(context);

    assert(isWall(entity));

    // Minimal damage + [Random damage - Enemy's Armor]
    s32 damage = we_getTotalDamage(minDamage, rndDamage, 0);
    piece->hp -= damage;
    piece->hp = max(piece->hp, 0);
}

void we_rangeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    if (wu_isMagicUnit(entity))
    {
        // Since the wcomm_attack of magic units are considered "kind of spells"
        // it will consume mana, at 2 per shot.
        if (we_decreaseUnitMana(context, entity, 2))
        {
            vec2 origin = wu_getUnitCenterPosition(entity, false);
            vec2 target = wu_getUnitCenterPosition(targetEntity, false);
            WarProjectileType projectileType = wu_getProjectileType(unit->type);
            wproj_createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
        }
        else
        {
            // wcomm_stop attacking if the magic unit rans out of mana
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }
    }
    else
    {
        vec2 origin = wu_getUnitCenterPosition(entity, false);
        vec2 target = wu_getUnitCenterPosition(targetEntity, false);
        WarProjectileType projectileType = wu_getProjectileType(unit->type);
        wproj_createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
    }
}

void we_rangeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    if (wu_isMagicUnit(entity))
    {
        // Since the wcomm_attack of magic units are considered "kind of spells"
        // it will consume mana, at 2 per shot.
        if (we_decreaseUnitMana(context, entity, 2))
        {
            vec2 origin = wu_getUnitCenterPosition(entity, false);
            vec2 target = wmap_vec2TileToMapCoordinates(vec2i(piece->tilex, piece->tiley), true);
            WarProjectileType projectileType = wu_getProjectileType(unit->type);
            wproj_createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
        }
        else
        {
            // wcomm_stop attacking if the magic unit rans out of mana
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }
    }
    else
    {
        vec2 origin = wu_getUnitCenterPosition(entity, false);
        vec2 target = wmap_vec2TileToMapCoordinates(vec2i(piece->tilex, piece->tiley), true);
        WarProjectileType projectileType = wu_getProjectileType(unit->type);
        wproj_createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
    }
}

void we_meleeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity)
{
    WarMap* map = context->map;

    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    // every unit has a 20 percent chance to miss (except catapults)
    if (wu_isCatapultUnit(entity) || chance(80))
    {
        s32 minDamage = unit->minDamage;
        s32 rndDamage = unit->rndDamage;

        WarPlayerInfo* player = &map->players[unit->player];
        if (player->godMode)
        {
            minDamage = GOD_MODE_MIN_DAMAGE;
        }

        we_takeDamage(context, targetEntity, minDamage, rndDamage);
    }
}

void we_meleeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece)
{
    WarMap* map = context->map;

    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    // every unit has a 20 percent chance to miss (except catapults)
    if (wu_isCatapultUnit(entity) || chance(80))
    {
        s32 minDamage = unit->minDamage;
        s32 rndDamage = unit->rndDamage;

        WarPlayerInfo* player = &map->players[unit->player];
        if (player->godMode)
        {
            minDamage = GOD_MODE_MIN_DAMAGE;
        }

        we_takeWallDamage(context, targetEntity, piece, minDamage, rndDamage);
    }
}

s32 we_mine(WarContext* context, WarEntity* goldmine, s32 amount)
{
    assert(goldmine);
    assert(isUnit(goldmine));

    WarUnitComponent* unit = &goldmine->unit;

    if (unit->amount < amount)
        amount = unit->amount;

    unit->amount -= amount;
    unit->amount = max(unit->amount, 0);

    if (unit->amount == 0)
    {
        if (!isCollapsing(goldmine) && !isGoingToCollapse(goldmine))
        {
            WarState* collapseState = wst_createCollapseState(context, goldmine);
            wst_changeNextState(context, goldmine, collapseState, true, true);

            wa_createAudioRandom(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, false);
        }
    }

    return amount;
}
