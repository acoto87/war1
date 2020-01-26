void addTransformComponent(WarContext* context, WarEntity* entity, vec2 position)
{
    entity->transform = (WarTransformComponent){0};
    entity->transform.enabled = true;
    entity->transform.position = position;
    entity->transform.rotation = VEC2_ZERO;
    entity->transform.scale = VEC2_ONE;
}

void removeTransformComponent(WarContext* context, WarEntity* entity)
{
    entity->transform = (WarTransformComponent){0};
}

void addSpriteComponent(WarContext* context, WarEntity* entity, WarSprite sprite)
{
    entity->sprite = (WarSpriteComponent){0};
    entity->sprite.enabled = true;
    entity->sprite.frameIndex = 0;
    entity->sprite.resourceIndex = 0;
    entity->sprite.sprite = sprite;
}

void addSpriteComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef spriteResourceRef)
{
    if (spriteResourceRef.resourceIndex < 0)
    {
        logWarning("Trying to create a sprite component with invalid resource index: %d\n", spriteResourceRef.resourceIndex);
        return;
    }

    WarSprite sprite = createSpriteFromResourceIndex(context, spriteResourceRef);
    addSpriteComponent(context, entity, sprite);
    entity->sprite.resourceIndex = spriteResourceRef.resourceIndex;
}

void removeSpriteComponent(WarContext* context, WarEntity* entity)
{
    freeSprite(context, entity->sprite.sprite);

    entity->sprite = (WarSpriteComponent){0};
}

void addUnitComponent(WarContext* context,
                      WarEntity* entity,
                      WarUnitType type,
                      s32 x,
                      s32 y,
                      u8 player,
                      WarResourceKind resourceKind,
                      u32 amount)
{
    WarUnitData unitData = getUnitData(type);

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
    entity->unit.actionIndex = 0;

    WarUnitActionListInit(&entity->unit.actions, WarUnitActionListDefaultOptions);
}

void removeUnitComponent(WarContext* context, WarEntity* entity)
{
    WarUnitActionListFree(&entity->unit.actions);
    entity->unit = (WarUnitComponent){0};
}

void addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces)
{
    entity->road = (WarRoadComponent){0};
    entity->road.enabled = true;
    entity->road.pieces = pieces;
}

void removeRoadComponent(WarContext* context, WarEntity* entity)
{
    WarRoadPieceListFree(&entity->road.pieces);
    entity->road = (WarRoadComponent){0};
}

void addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces)
{
    entity->wall = (WarWallComponent){0};
    entity->wall.enabled = true;
    entity->wall.pieces = pieces;
}

void removeWallComponent(WarContext* context, WarEntity* entity)
{
    WarWallPieceListFree(&entity->wall.pieces);
    entity->wall = (WarWallComponent){0};
}

void addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces)
{
    entity->ruin = (WarRuinComponent){0};
    entity->ruin.enabled = true;
    entity->ruin.pieces = pieces;
}

void removeRuinComponent(WarContext* context, WarEntity* entity)
{
    WarRuinPieceListFree(&entity->ruin.pieces);
    entity->ruin = (WarRuinComponent){0};
}

void addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees)
{
    entity->forest = (WarForestComponent){0};
    entity->forest.enabled = true;
    entity->forest.trees = trees;
}

void removeForestComponent(WarContext* context, WarEntity* entity)
{
    WarTreeListFree(&entity->forest.trees);
    entity->forest = (WarForestComponent){0};
}

void addStateMachineComponent(WarContext* context, WarEntity* entity)
{
    entity->stateMachine = (WarStateMachineComponent){0};
    entity->stateMachine.enabled = true;
    entity->stateMachine.currentState = NULL;
    entity->stateMachine.nextState = NULL;
}

void removeStateMachineComponent(WarContext* context, WarEntity* entity)
{
    WarStateMachineComponent* stateMachine = &entity->stateMachine;

    if (stateMachine->currentState)
        leaveState(context, entity, stateMachine->currentState);

    if (stateMachine->nextState)
        leaveState(context, entity, stateMachine->nextState);

    entity->stateMachine = (WarStateMachineComponent){0};
}

void addAnimationsComponent(WarContext* context, WarEntity* entity)
{
    entity->animations = (WarAnimationsComponent){0};
    entity->animations.enabled = true;
    WarSpriteAnimationListInit(&entity->animations.animations, WarSpriteAnimationListDefaultOptions);
}

void removeAnimationsComponent(WarContext* context, WarEntity* entity)
{
    WarSpriteAnimationListFree(&entity->animations.animations);
    entity->animations = (WarAnimationsComponent){0};
}

void addUIComponent(WarContext* context, WarEntity* entity, char* name)
{
    entity->ui = (WarUIComponent){0};
    entity->ui.enabled = true;
    entity->ui.name = name;
}

void removeUIComponent(WarContext* context, WarEntity* entity)
{
    entity->ui = (WarUIComponent){0};
}

void addTextComponent(WarContext* context, WarEntity* entity, s32 fontIndex, f32 fontSize, const char* text)
{
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

    setUIText(entity, text);
}

void removeTextComponent(WarContext* context, WarEntity* entity)
{
    clearUIText(entity);
    entity->text = (WarTextComponent){0};
}

void addRectComponent(WarContext* context, WarEntity* entity, vec2 size, u8Color color)
{
    entity->rect = (WarRectComponent){0};
    entity->rect.enabled = true;
    entity->rect.size = size;
    entity->rect.color = color;
}

void removeRectComponent(WarContext* context, WarEntity* entity)
{
    entity->rect = (WarRectComponent){0};
}

void addButtonComponent(WarContext* context, WarEntity* entity, WarSprite normalSprite, WarSprite pressedSprite)
{
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

void addButtonComponentFromResource(WarContext* context,
                                    WarEntity* entity,
                                    WarSpriteResourceRef normalRef,
                                    WarSpriteResourceRef pressedRef)
{
    if (normalRef.resourceIndex < 0)
    {
        logWarning("Trying to create a button component with invalid resource index: %d\n", normalRef.resourceIndex);
        return;
    }

    if (pressedRef.resourceIndex < 0)
    {
        logWarning("Trying to create a button component with invalid resource index: %d\n", pressedRef.resourceIndex);
        return;
    }

    WarSprite normalSprite = createSpriteFromResourceIndex(context, normalRef);
    WarSprite pressedSprite = createSpriteFromResourceIndex(context, pressedRef);
    addButtonComponent(context, entity, normalSprite, pressedSprite);
}

void removeButtonComponent(WarContext* context, WarEntity* entity)
{
    clearUITooltip(entity);
    freeSprite(context, entity->button.normalSprite);
    freeSprite(context, entity->button.pressedSprite);
    entity->button = (WarButtonComponent){0};
}

void addAudioComponent(WarContext* context, WarEntity* entity, WarAudioType type, s32 resourceIndex, bool loop)
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
            logError("Could not load MIDI from resource: %d\n", resourceIndex);
        }
    }
}

void removeAudioComponent(WarContext* context, WarEntity* entity)
{
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

void addProjectileComponent(WarContext* context, WarEntity* entity, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target, s32 speed)
{
    entity->projectile = (WarProjectileComponent){0};
    entity->projectile.enabled = true;
    entity->projectile.type = type;
    entity->projectile.sourceEntityId = sourceEntityId;
    entity->projectile.targetEntityId = targetEntityId;
    entity->projectile.origin = origin;
    entity->projectile.target = target;
    entity->projectile.speed = speed;
}

void removeProjectileComponent(WarContext* context, WarEntity* entity)
{
    entity->projectile = (WarProjectileComponent){0};
}

void addCursorComponent(WarContext* context, WarEntity* entity, WarCursorType type, vec2 hot)
{
    entity->cursor = (WarCursorComponent){0};
    entity->cursor.enabled = true;
    entity->cursor.type = type;
    entity->cursor.hot = hot;
}

void removeCursorComponent(WarContext* context, WarEntity* entity)
{
    entity->cursor = (WarCursorComponent){0};
}

void addPoisonCloudComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time)
{
    entity->poisonCloud = (WarPoisonCloudComponent){0};
    entity->poisonCloud.enabled = true;
    entity->poisonCloud.position = position;
    entity->poisonCloud.time = time;
    entity->poisonCloud.damageTime = 0;
}

void removePoisonCloudComponent(WarContext* context, WarEntity* entity)
{
    entity->poisonCloud = (WarPoisonCloudComponent){0};
}

void addSightComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time)
{
    entity->sight = (WarSightComponent){0};
    entity->sight.enabled = true;
    entity->sight.position = position;
    entity->sight.time = time;
}

void removeSightComponent(WarContext* context, WarEntity* entity)
{
    entity->sight = (WarSightComponent){0};
}

// Entities
WarEntity* createEntity(WarContext* context, WarEntityType type, bool addToScene)
{
    WarEntityManager* manager = getEntityManager(context);

    WarEntity* entity = (WarEntity *)xcalloc(1, sizeof(WarEntity));
    entity->id = ++manager->staticEntityId;
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

        WarEntityList* entitiesOfType = getEntitiesOfType(context, type);
        WarEntityListAdd(entitiesOfType, entity);

        if (isUIEntity(entity))
        {
            WarEntityListAdd(&manager->uiEntities, entity);
        }
    }

    return entity;
}

WarEntity* createUnit(WarContext* context,
                      WarUnitType type,
                      s32 x,
                      s32 y,
                      u8 player,
                      WarResourceKind resourceKind,
                      u32 amount,
                      bool addToMap)
{
    WarMap* map = context->map;

    WarEntity* entity = createEntity(context, WAR_ENTITY_TYPE_UNIT, addToMap);
    addUnitComponent(context, entity, type, x, y, player, resourceKind, amount);
    addTransformComponent(context, entity, vec2i(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT));

    WarUnitData unitData = getUnitData(type);

    s32 spriteIndex = unitData.resourceIndex;
    if (spriteIndex == 0)
    {
        logError("Sprite for unit of type %d is not configure properly. Default to footman sprite.\n", type);
        spriteIndex = 279;
    }
    addSpriteComponentFromResource(context, entity, imageResourceRef(spriteIndex));

    addUnitActions(entity);
    addAnimationsComponent(context, entity);
    addStateMachineComponent(context, entity);

    if (isDudeUnit(entity))
    {
        WarUnitStats unitStats = getUnitStats(type);

        entity->unit.maxhp = unitStats.hp;
        entity->unit.hp = unitStats.hp;
        entity->unit.maxMana = unitStats.mana;
        entity->unit.mana = isSummonUnit(entity) ? unitStats.mana : 100;
        entity->unit.armor = unitStats.armor;
        entity->unit.range = unitStats.range;
        entity->unit.minDamage = unitStats.minDamage;
        entity->unit.rndDamage = unitStats.rndDamage;
        entity->unit.decay = unitStats.decay;
        entity->unit.manaTime = 1;
    }
    else if(isBuildingUnit(entity))
    {
        WarBuildingStats buildingStats = getBuildingStats(type);

        entity->unit.maxhp = buildingStats.hp;
        entity->unit.hp = buildingStats.hp;
        entity->unit.armor = buildingStats.armor;
    }

    WarState* idleState = createIdleState(context, entity, isDudeUnit(entity));
    changeNextState(context, entity, idleState, true, true);

    if (addToMap)
    {
        WarEntityManager* manager = &map->entityManager;
        WarEntityList* list = WarUnitMapGet(&manager->unitsByType, type);
        WarEntityListAdd(list, entity);
    }

    return entity;
}

WarEntity* createDude(WarContext* context,
                      WarUnitType type,
                      s32 x,
                      s32 y,
                      u8 player,
                      bool isGoingToTrain)
{
    assert(isDudeUnitType(type));

    return createUnit(context, type, x, y, player, WAR_RESOURCE_NONE, 0, !isGoingToTrain);
}

WarEntity* createBuilding(WarContext* context,
                          WarUnitType type,
                          s32 x,
                          s32 y,
                          u8 player,
                          bool isGoingToBuild)
{
    assert(isBuildingUnitType(type));

    WarEntity* entity = createUnit(context, type, x, y, player, WAR_RESOURCE_NONE, 0, true);

    if (isGoingToBuild)
    {
        WarBuildingStats stats = getBuildingStats(type);
        f32 buildTime = getMapScaledTime(context, stats.buildTime);
        WarState* buildState = createBuildState(context, entity, buildTime);
        changeNextState(context, entity, buildState, true, true);
    }

    return entity;
}

WarEntity* findEntity(WarContext* context, WarEntityId id)
{
    WarEntityManager* manager = getEntityManager(context);
    return WarEntityIdMapGet(&manager->entitiesById, id);
}

WarEntity* findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type)
{
    WarMap* map = context->map;
    WarEntityManager* manager = &map->entityManager;

    WarEntity* result = NULL;
    f32 minDst = INT32_MAX;

    WarEntityList* units = WarEntityMapGet(&manager->entitiesByType, WAR_ENTITY_TYPE_UNIT);
    assert(units);

    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* target = units->items[i];
        if (isUnitOfType(target, type))
        {
            s32 dst = unitDistanceInTiles(entity, target);
            if (dst < minDst)
            {
                result = target;
                minDst = dst;
            }
        }
    }

    return result;
}

WarEntity* findUIEntity(WarContext* context, const char* name)
{
    WarEntityList* entities = getUIEntities(context);
    for (s32 i = 0; i < entities->count; i++)
    {
        WarEntity* entity = entities->items[i];
        if (entity &&
            isUIEntity(entity) &&
            strEquals(entity->ui.name, name))
        {
            return entity;
        }
    }

    return NULL;
}

WarEntity* findEntityUnderCursor(WarContext* context, bool includeTrees, bool includeWalls)
{
    WarInput* input = &context->input;

    WarMap* map = context->map;
    assert(map);

    vec2 targetPoint = vec2ScreenToMapCoordinates(context, input->pos);
    vec2 targetTile = vec2MapToTileCoordinates(targetPoint);

    WarEntity* entityUnderCursor = NULL;

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            WarUnitComponent* unit = &entity->unit;
            if (unit->enabled)
            {
                // don't change the cursor for dead units or corpses
                if (isDead(entity) || isGoingToDie(entity) || isCorpseUnit(entity))
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

            rect unitRect = getUnitRect(entity);
            if (rectContainsf(unitRect, targetPoint.x, targetPoint.y))
            {
                entityUnderCursor = entity;
                break;
            }
        }
    }

    if (includeTrees && !entityUnderCursor)
    {
        WarEntityList* forests = getEntitiesOfType(context, WAR_ENTITY_TYPE_FOREST);
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
        WarEntityList* walls = getEntitiesOfType(context, WAR_ENTITY_TYPE_WALL);
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

void removeEntity(WarContext* context, WarEntity* entity)
{
    removeTransformComponent(context, entity);
    removeSpriteComponent(context, entity);
    removeRoadComponent(context, entity);
    removeWallComponent(context, entity);
    removeRuinComponent(context, entity);
    removeForestComponent(context, entity);
    removeUnitComponent(context, entity);
    removeStateMachineComponent(context, entity);
    removeAnimationsComponent(context, entity);
    removeUIComponent(context, entity);
    removeTextComponent(context, entity);
    removeRectComponent(context, entity);
    removeButtonComponent(context, entity);
    removeAudioComponent(context, entity);
    removeCursorComponent(context, entity);
}

void removeEntityById(WarContext* context, WarEntityId id)
{
    WarEntityManager* manager = getEntityManager(context);

    logDebug("trying to remove entity with id: %d\n", id);

    WarEntity* entity = findEntity(context, id);
    if (entity)
    {
        removeEntity(context, entity);

        if (isUIEntity(entity))
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

        logDebug("removed entity with id: %d\n", id);
    }
}

bool isStaticEntity(WarEntity* entity)
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

void initEntityManager(WarEntityManager* manager)
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
        WarEntityList* list = (WarEntityList*)xmalloc(sizeof(WarEntityList));
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
        WarEntityList* list = (WarEntityList*)xmalloc(sizeof(WarEntityList));
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

WarEntityManager* getEntityManager(WarContext* context)
{
    if (context->scene)
        return &context->scene->entityManager;

    if (context->map)
        return &context->map->entityManager;

    logError("There is no map or scene active.\n");
    return NULL;
}

WarEntityList* getEntities(WarContext* context)
{
    WarEntityManager* manager = getEntityManager(context);
    return &manager->entities;
}

WarEntityList* getEntitiesOfType(WarContext* context, WarEntityType type)
{
    WarEntityManager* manager = getEntityManager(context);
    return WarEntityMapGet(&manager->entitiesByType, type);
}

WarEntityList* getUnitsOfType(WarContext* context, WarUnitType type)
{
    WarEntityManager* manager = getEntityManager(context);
    return WarUnitMapGet(&manager->unitsByType, type);
}

WarEntityList* getUIEntities(WarContext* context)
{
    WarEntityManager* manager = getEntityManager(context);
    return &manager->uiEntities;
}

// Render entities
s32 renderCompareUnits(const WarEntity* e1, const WarEntity* e2)
{
    assert(isUnit(e1));
    assert(isUnit(e2));

    bool isDead1 = isCorpseUnit((WarEntity*)e1) || isDead((WarEntity*)e1) || isGoingToDie((WarEntity*)e1);
    bool isDead2 = isCorpseUnit((WarEntity*)e2) || isDead((WarEntity*)e2) || isGoingToDie((WarEntity*)e2);

    if (isDead1 && !isDead2)
        return -1;

    if (!isDead1 && isDead2)
        return 1;

    vec2 p1 = getUnitPosition((WarEntity*)e1, false);
    vec2 p2 = getUnitPosition((WarEntity*)e2, false);

    return p1.y - p2.y;
}

s32 renderCompareProjectiles(const WarEntity* e1, const WarEntity* e2)
{
    vec2 p1 = e1->transform.position;
    vec2 p2 = e2->transform.position;

    return p1.y - p2.y;
}

void renderImage(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent transform = entity->transform;
    WarUIComponent* ui = &entity->ui;
    WarSpriteComponent* sprite = &entity->sprite;

    if (ui->enabled && sprite->enabled && sprite->frameIndex >= 0)
    {
        nvgSave(gfx);

        if (sprite->sprite.framesCount > 1)
        {
            WarSpriteFrame frame = getSpriteFrame(context, sprite->sprite, sprite->frameIndex);
            updateSpriteImage(context, sprite->sprite, frame.data);

            nvgTranslate(gfx, -frame.dx, -frame.dy);
        }

        nvgTranslate(gfx, transform.position.x, transform.position.y);
        renderSprite(context, sprite->sprite, VEC2_ZERO, VEC2_ONE);
        nvgRestore(gfx);
    }
}

void renderRoad(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarSpriteComponent* sprite = &entity->sprite;
    WarRoadComponent* road = &entity->road;

    // the roads are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled && road->enabled)
    {
        WarRoadPieceList* pieces = &road->pieces;

        NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, road->pieces.count);

        for (s32 i = 0; i < pieces->count; i++)
        {
            // get the index of the tile in the spritesheet of the map,
            // corresponding to the current tileset type (forest, swamp)
            WarRoadData roadsData = getRoadData(pieces->items[i].type);

            s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                ? roadsData.tileIndexForest : roadsData.tileIndexSwamp;

            // the position in the world of the road piece tile
            s32 x = pieces->items[i].tilex;
            s32 y = pieces->items[i].tiley;

            // coordinates in pixels of the road piece tile
            s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            nvgSave(gfx);
            nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

            rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

            nvgRestore(gfx);
        }

        nvgEndImageBatch(gfx, batch);
    }
}

void renderWall(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarSpriteComponent* sprite = &entity->sprite;
    WarWallComponent* wall = &entity->wall;

    // the walls are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled && wall->enabled)
    {
        WarWallPieceList* pieces = &wall->pieces;

        NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, wall->pieces.count);

        for (s32 i = 0; i < pieces->count; i++)
        {
            WarWallPiece* piece = &pieces->items[i];

            // get the index of the tile in the spritesheet of the map,
            // corresponding to the current tileset type (forest, swamp)
            WarWallData wallsData = getWallData(piece->type);

            s32 tileIndex = 0;

            s32 hpPercent = percentabi(piece->hp, piece->maxhp);
            if (hpPercent <= 0)
            {
                tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? wallsData.tileDestroyedForest : wallsData.tileDestroyedSwamp;
            }
            else if(hpPercent < 50)
            {
                tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? wallsData.tileDamagedForest : wallsData.tileDamagedSwamp;
            }
            else
            {
                tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? wallsData.tileForest : wallsData.tileSwamp;
            }

            // the position in the world of the wall piece tile
            s32 x = piece->tilex;
            s32 y = piece->tiley;

            // coordinates in pixels of the wall piece tile
            s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            nvgSave(gfx);
            nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

            rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

            nvgRestore(gfx);
        }

        nvgEndImageBatch(gfx, batch);
    }
}

void renderRuin(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarSpriteComponent* sprite = &entity->sprite;
    WarRuinComponent* ruin = &entity->ruin;

    // the walls are only for forest and swamp maps
    WarMapTilesetType tilesetType = context->map->tilesetType;
    assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

    if (sprite->enabled && ruin->enabled)
    {
        WarRuinPieceList* pieces = &ruin->pieces;

        NVGimageBatch* batch = nvgBeginImageBatch(gfx, sprite->sprite.image, ruin->pieces.count);

        for (s32 i = 0; i < ruin->pieces.count; i++)
        {
            WarRuinPiece* piece = &pieces->items[i];
            if (piece->type == WAR_RUIN_PIECE_NONE)
                continue;

            // get the index of the tile in the spritesheet of the map,
            // corresponding to the current tileset type (forest, swamp)
            WarRuinData ruinsData = getRuinData(piece->type);

            s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                ? ruinsData.tileIndexForest : ruinsData.tileIndexSwamp;

            // the position in the world of the road piece tile
            s32 x = piece->tilex;
            s32 y = piece->tiley;

            // coordinates in pixels of the road piece tile
            s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            nvgSave(gfx);
            nvgTranslate(gfx, x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);

            rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            nvgRenderBatchImage(gfx, batch, rs, rd, VEC2_ONE);

            nvgRestore(gfx);
        }

        nvgEndImageBatch(gfx, batch);
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

            WarTreeData data = getTreeData(tree->type);

            // the position in the world of the wood tile
            s32 x = tree->tilex;
            s32 y = tree->tiley;

            s32 prevTileIndex = getMapTileIndex(context, x, y);
            s32 newTileIndex = (tilesetType == MAP_TILESET_FOREST) ? data.tileIndexForest : data.tileIndexSwamp;

            if (prevTileIndex != newTileIndex)
                logDebug("different tile index for tree (%d, %d), prev: %d, new: %d\n", x, y, prevTileIndex, newTileIndex);

            setMapTileIndex(context, x, y, newTileIndex);
        }
    }
}

void renderUnit(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    NVGcontext* gfx = context->gfx;

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
    vec2 frameSize = getUnitFrameSize(entity);

    // size of the unit
    vec2 unitSize = getUnitSpriteSize(entity);

    // the unit is visible if it's partially on the clear areas of the fog
    bool isVisible = isUnitPartiallyVisible(map, entity);

    nvgTranslate(gfx, -halff(frameSize.x), -halff(frameSize.y));
    nvgTranslate(gfx, halff(unitSize.x), halff(unitSize.y));
    nvgTranslate(gfx, position.x, position.y);

#ifdef DEBUG_RENDER_UNIT_INFO
    nvgFillRect(gfx, getUnitFrameRect(entity), nvgRGBA(0, 0, 128, 128));
    nvgFillRect(gfx, getUnitSpriteRect(entity), NVG_GRAY_TRANSPARENT);
    nvgFillRect(gfx, rectv(getUnitSpriteCenter(entity), VEC2_ONE), nvgRGB(255, 0, 0));
#endif

#ifdef DEBUG_RENDER_UNIT_STATS
    rect spriteRect = getUnitSpriteRect(entity);

    char debugText[50];

    if (unit->hp == 0)
        sprintf(debugText, "hp: dead");
    else
        sprintf(debugText, "hp: %d", percentabi(unit->hp, unit->maxhp));

    nvgFontSize(gfx, 5.0f);
    nvgFontFace(gfx, "roboto-r");
    nvgFillColor(gfx, nvgRGBA(200, 200, 200, 255));
    nvgTextAlign(gfx, NVG_ALIGN_LEFT);
    nvgText(gfx, spriteRect.x, spriteRect.y, debugText, NULL);
#endif

    if (sprite->enabled && (isVisible || unit->hasBeenSeen))
    {
        nvgSave(gfx);

        if (isDudeUnit(entity))
        {
            WarUnitComponent* unit = &entity->unit;

            if (unit->invisible)
            {
                nvgGlobalAlpha(gfx, 0.5f);
            }

            if (unit->invulnerable)
            {
                rect unitRect = getUnitSpriteRect(entity);
                unitRect = rectExpand(unitRect, -1, -1);
                nvgStrokeRect(gfx, unitRect, NVG_BLUE_INVULNERABLE, 1);
            }
        }

        WarSpriteFrame frame = getSpriteFrame(context, sprite->sprite, sprite->frameIndex);
        updateSpriteImage(context, sprite->sprite, frame.data);
        renderSprite(context, sprite->sprite, VEC2_ZERO, scale);

        nvgRestore(gfx);
    }

    if (animations->enabled && isVisible)
    {
        for (s32 i = 0; i < animations->animations.count; i++)
        {
            WarSpriteAnimation* anim = animations->animations.items[i];
            if (anim->status == WAR_ANIM_STATUS_RUNNING)
            {
                nvgSave(gfx);

                nvgTranslate(gfx, anim->offset.x, anim->offset.y);
                nvgScale(gfx, anim->scale.x, anim->scale.y);

#ifdef DEBUG_RENDER_UNIT_ANIMATIONS
                // size of the original sprite
                vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

                nvgFillRect(gfx, rectv(VEC2_ZERO, animFrameSize), NVG_GRAY_TRANSPARENT);
#endif

                s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
                animFrameIndex = clamp(animFrameIndex, 0, anim->frames.count - 1);

                s32 spriteFrameIndex = anim->frames.items[animFrameIndex];
                WarSpriteFrame frame = getSpriteFrame(context, anim->sprite, spriteFrameIndex);

                updateSpriteImage(context, anim->sprite, frame.data);
                renderSprite(context, anim->sprite, VEC2_ZERO, VEC2_ONE);

                nvgRestore(gfx);
            }
        }
    }
}

void renderText(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent* transform = &entity->transform;
    WarUIComponent* ui = &entity->ui;
    WarTextComponent* text = &entity->text;

    if (ui->enabled && text->enabled && text->text)
    {
        nvgSave(gfx);
        nvgTranslate(gfx, transform->position.x, transform->position.y);
        nvgScale(gfx, transform->scale.x, transform->scale.y);

        NVGfontParams params;
        params.fontIndex = text->fontIndex;
        params.fontSize = text->fontSize;
        params.lineHeight = text->lineHeight;
        params.fontColor = u8ColorToNVGcolor(text->fontColor);
        params.highlightColor = u8ColorToNVGcolor(text->highlightColor);
        params.highlightIndex = text->highlightIndex;
        params.highlightCount = text->highlightCount;
        params.boundings = text->boundings;
        params.horizontalAlign = textAlignToNVGalign(text->horizontalAlign);
        params.verticalAlign = textAlignToNVGalign(text->verticalAlign);
        params.lineAlign = textAlignToNVGalign(text->lineAlign);
        params.wrapping = textWrappingToNVGwrap(text->wrapping);
        params.trimming = textTrimmingToNVGtrim(text->trimming);
        params.fontSprite = context->fontSprites[text->fontIndex];
        params.fontData = fontsData[text->fontIndex];

        if (entity->text.multiline)
            nvgMultiSpriteText(gfx, text->text, 0, 0, params);
        else
            nvgSingleSpriteText(gfx, text->text, 0, 0, params);

        nvgRestore(gfx);
    }
}

void renderRect(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent* transform = &entity->transform;
    WarUIComponent* ui = &entity->ui;
    WarRectComponent* rect = &entity->rect;

    if (ui->enabled && rect->enabled)
    {
        nvgSave(gfx);
        nvgTranslate(gfx, transform->position.x, transform->position.y);
        nvgScale(gfx, transform->scale.x, transform->scale.y);

        NVGcolor color = nvgRGBA(rect->color.r, rect->color.g, rect->color.b, rect->color.a);
        nvgFillRect(gfx, rectf(0.0f, 0.0f, rect->size.x, rect->size.y), color);

        nvgRestore(gfx);
    }
}

void renderButton(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent* transform = &entity->transform;
    WarUIComponent* ui = &entity->ui;
    WarButtonComponent* button = &entity->button;
    WarTextComponent* text = &entity->text;
    WarSpriteComponent* sprite = &entity->sprite;

    if (ui->enabled && button->enabled)
    {
        nvgSave(gfx);
        nvgTranslate(gfx, transform->position.x, transform->position.y);
        nvgScale(gfx, transform->scale.x, transform->scale.y);

        // render background
        {
            WarSprite backgroundSprite = button->active ? button->pressedSprite : button->normalSprite;
            renderSprite(context, backgroundSprite, VEC2_ZERO, VEC2_ONE);
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

                nvgTranslate(gfx, offset.x, offset.y);

                WarSpriteFrame frame = getSpriteFrame(context, sprite->sprite, sprite->frameIndex);
                updateSpriteImage(context, sprite->sprite, frame.data);
                renderSprite(context, sprite->sprite, VEC2_ZERO, VEC2_ONE);
            }
        }

        // render text
        {
            if (text->enabled)
            {
                NVGfontParams params;
                params.fontIndex = text->fontIndex;
                params.fontSize = text->fontSize;
                params.fontColor = u8ColorToNVGcolor(text->fontColor);
                params.highlightColor = u8ColorToNVGcolor(text->highlightColor);
                params.highlightIndex = button->hot ? ALL_HIGHLIGHT : text->highlightIndex;
                params.highlightCount = text->highlightCount;
                params.boundings = text->boundings;
                params.horizontalAlign = textAlignToNVGalign(text->horizontalAlign);
                params.verticalAlign = textAlignToNVGalign(text->verticalAlign);
                params.lineAlign = textAlignToNVGalign(text->lineAlign);
                params.wrapping = textWrappingToNVGwrap(text->wrapping);
                params.fontSprite = context->fontSprites[text->fontIndex];
                params.fontData = fontsData[text->fontIndex];

                nvgSingleSpriteText(gfx, text->text, 0, 0, params);
            }
        }

        nvgRestore(gfx);
    }
}

void renderProjectile(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;

    vec2 position = transform->position;
    vec2 scale = transform->scale;

    if (sprite->enabled)
    {
        WarSpriteFrame frame = getSpriteFrame(context, sprite->sprite, sprite->frameIndex);

#ifdef DEBUG_RENDER_PROJECTILES
        {
            nvgSave(gfx);

            nvgTranslate(gfx, -halfi(sprite->sprite.frameWidth),-halfi(sprite->sprite.frameHeight));
            nvgTranslate(gfx, position.x, position.y);

            rect r = rectf(0, 0, sprite->sprite.frameWidth, sprite->sprite.frameHeight);
            nvgFillRect(gfx, r, NVG_GRAY_TRANSPARENT);

            nvgRestore(gfx);
        }

        {
            nvgSave(gfx);

            nvgTranslate(gfx, -halfi(frame.w),-halfi(frame.h));
            nvgTranslate(gfx, position.x, position.y);

            rect r = rectf(0, 0, frame.w, frame.h);
            nvgFillRect(gfx, r, NVG_RED_TRANSPARENT);

            nvgRestore(gfx);
        }

        {
            WarProjectileComponent* projectile = &entity->projectile;

            nvgSave(gfx);

            nvgStrokeLine(gfx, projectile->origin, projectile->target, getColorFromList(entity->id), 0.5f);
            nvgFillRect(gfx, rectv(projectile->origin, VEC2_ONE), nvgRGB(255, 0, 255));
            nvgFillRect(gfx, rectv(projectile->target, VEC2_ONE), nvgRGB(255, 0, 255));

            nvgRestore(gfx);
        }
#endif

        nvgTranslate(gfx, -frame.dx, -frame.dy);
        nvgTranslate(gfx, -halff(frame.w), -halff(frame.h));
        nvgTranslate(gfx, position.x, position.y);

        updateSpriteImage(context, sprite->sprite, frame.data);
        renderSprite(context, sprite->sprite, VEC2_ZERO, scale);
    }
}

void renderMinimap(WarContext* context, WarEntity* entity)
{
    WarMap* map = context->map;

    NVGcontext* gfx = context->gfx;

    vec2 position = entity->transform.position;

    WarSpriteFrame* frame0 = &map->minimapSprite.frames[0];
    WarSpriteFrame* frame1 = &map->minimapSprite.frames[1];

    // copy the minimap base to the first frame which is the one that will be rendered
    // copy only the visible tiles/pixels
    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            WarMapTile* tile = getMapTileState(map, x, y);
            if (tile->state == MAP_TILE_STATE_VISIBLE ||
                tile->state == MAP_TILE_STATE_FOG)
            {
                s32 index = y * MAP_TILES_WIDTH + x;
                frame0->data[index * 4 + 0] = frame1->data[index * 4 + 0];
                frame0->data[index * 4 + 1] = frame1->data[index * 4 + 1];
                frame0->data[index * 4 + 2] = frame1->data[index * 4 + 2];
                frame0->data[index * 4 + 3] = frame1->data[index * 4 + 3];
            }
        }
    }

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            WarUnitComponent* unit = &entity->unit;
            WarTransformComponent* transform = &entity->transform;

            if (displayUnitOnMinimap(entity) && (isUnitPartiallyVisible(map, entity) || entity->unit.hasBeenSeen))
            {
                s32 tileX = (s32)(transform->position.x / MEGA_TILE_WIDTH);
                s32 tileY = (s32)(transform->position.y / MEGA_TILE_HEIGHT);

                u8Color color = getUnitColorOnMinimap(entity);

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

    nvgSave(gfx);
    nvgTranslate(gfx, position.x, position.y);

    // render base
    updateSpriteImage(context, map->minimapSprite, map->minimapSprite.frames[0].data);
    renderSprite(context, map->minimapSprite, VEC2_ZERO, VEC2_ONE);

    // render viewport
    nvgTranslate(gfx, map->viewport.x * MINIMAP_MAP_WIDTH_RATIO, map->viewport.y * MINIMAP_MAP_HEIGHT_RATIO);
    nvgStrokeRect(gfx, recti(0, 0, MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT), NVG_WHITE, 1.0f/context->globalScale);

    nvgRestore(gfx);
}

void renderAnimation(WarContext* context, WarEntity* entity)
{
    NVGcontext* gfx = context->gfx;

    WarTransformComponent* transform = &entity->transform;
    WarAnimationsComponent* animations = &entity->animations;

    vec2 position = transform->position;
    nvgTranslate(gfx, position.x, position.y);

    if (animations->enabled)
    {
        for (s32 i = 0; i < animations->animations.count; i++)
        {
            WarSpriteAnimation* anim = animations->animations.items[i];
            if (anim->status == WAR_ANIM_STATUS_RUNNING)
            {
                nvgSave(gfx);

                nvgTranslate(gfx, anim->offset.x, anim->offset.y);
                nvgScale(gfx, anim->scale.x, anim->scale.y);

                s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
                animFrameIndex = clamp(animFrameIndex, 0, anim->frames.count - 1);

                s32 spriteFrameIndex = anim->frames.items[animFrameIndex];
                WarSpriteFrame frame = getSpriteFrame(context, anim->sprite, spriteFrameIndex);

                updateSpriteImage(context, anim->sprite, frame.data);
                renderSprite(context, anim->sprite, VEC2_ZERO, VEC2_ONE);

                nvgRestore(gfx);
            }
        }
    }
}

void renderEntity(WarContext* context, WarEntity* entity)
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
        NULL,               // WAR_ENTITY_TYPE_POISON_CLOUD
        NULL,               // WAR_ENTITY_TYPE_SIGHT
        renderMinimap,      // WAR_ENTITY_TYPE_MINIMAP
        renderAnimation,    // WAR_ENTITY_TYPE_ANIMATION
    };

    NVGcontext* gfx = context->gfx;

    if (entity->id && entity->enabled)
    {
        WarRenderFunc renderFunc = renderFuncs[(s32)entity->type];
        if (!renderFunc)
        {
            logError("Entity of type %d can't be render. renderFunc = NULL\n", entity->type);
            return;
        }

        nvgSave(gfx);
        renderFunc(context, entity);
        nvgRestore(gfx);
    }
}

void renderEntitiesOfType(WarContext* context, WarEntityType type)
{
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

    WarEntityList* entities = getEntitiesOfType(context, type);

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
            renderEntity(context, entity);
        }
    }
}

void renderUnitSelection(WarContext* context)
{
    WarMap* map = context->map;

    NVGcontext* gfx = context->gfx;

    WarEntityIdList* selectedEntities = &map->selectedEntities;
    for (s32 i = 0; i < selectedEntities->count; i++)
    {
        WarEntityId entityId = selectedEntities->items[i];
        WarEntity* entity = findEntity(context, entityId);
        if (entity)
        {
            WarTransformComponent* transform = &entity->transform;
            WarSpriteComponent* sprite = &entity->sprite;

            if (sprite->enabled)
            {
                // size of the original sprite
                vec2 frameSize = getUnitFrameSize(entity);

                // size of the unit
                vec2 unitSize = getUnitSpriteSize(entity);

                // position of the unit in the map
                vec2 position = transform->position;

                nvgSave(gfx);
                nvgTranslate(gfx, -halff(frameSize.x), -halff(frameSize.y));
                nvgTranslate(gfx, halff(unitSize.x), halff(unitSize.y));
                nvgTranslate(gfx, position.x, position.y);

                rect selr = rectf(halff(frameSize.x - unitSize.x), halff(frameSize.y - unitSize.y), unitSize.x, unitSize.y);
                NVGcolor color = NVG_WHITE_SELECTION;
                if (isFriendlyUnit(context, entity))
                    color = NVG_GREEN_SELECTION;
                else if (isEnemyUnit(context, entity))
                    color = NVG_RED_SELECTION;
                nvgStrokeRect(gfx, selr, color, 1.0f);

                nvgRestore(gfx);
            }
        }
    }
}

void increaseUpgradeLevel(WarContext* context, WarPlayerInfo* player, WarUpgradeType upgrade)
{
    assert(hasRemainingUpgrade(player, upgrade));

    incrementUpgradeLevel(player, upgrade);

    switch (upgrade)
    {
        case WAR_UPGRADE_ARROWS:
        case WAR_UPGRADE_SPEARS:
        {
            WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
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
            WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
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
            WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                if (entity && entity->unit.player == player->index)
                {
                    if (isDudeUnit(entity))
                    {
                        entity->unit.speed++;
                    }
                }
            }

            break;
        }

        case WAR_UPGRADE_SHIELD:
        {
            WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
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
            logInfo("This upgrade type %d doesn't increase any value of the units\n", upgrade);
            break;
        }
    }
}

bool decreasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood)
{
    if (player->gold < gold)
    {
        setFlashStatus(context, 1.5f, "NOT ENOUGH GOLD... MINE MORE GOLD");
        return false;
    }

    if (player->wood < wood)
    {
        setFlashStatus(context, 1.5f, "NOT ENOUGH LUMBER... CHOP MORE TREES");
        return false;
    }

    player->gold -= gold;
    player->wood -= wood;
    return true;
}

void increasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood)
{
    player->gold += gold;
    player->wood += wood;
}

bool increaseUnitHp(WarContext* context, WarEntity* entity, s32 hp)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    unit->hp += hp;
    unit->hp = min(unit->hp, unit->maxhp);

    return true;
}

bool decreaseUnitHp(WarContext* context, WarEntity* entity, s32 hp)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    unit->hp -= hp;
    unit->hp = max(unit->hp, 0);

    return true;
}

bool decreaseUnitMana(WarContext* context, WarEntity* entity, s32 mana)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    if (unit->mana < mana)
    {
        setFlashStatus(context, 1.5f, "NOT ENOUGH MANA");
        return false;
    }

    unit->mana = max(unit->mana - mana, 0);
    return true;
}

void increaseUnitMana(WarContext* context, WarEntity* entity, s32 mana)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;
    unit->mana = clamp(unit->mana + mana, 0, unit->maxMana);
}

bool checkFarmFood(WarContext* context, WarPlayerInfo* player)
{
    s32 dudesCount = getTotalNumberOfDudes(context, player->index);

    WarUnitType farmType = isHumanPlayer(player)
        ? WAR_UNIT_FARM_HUMANS : WAR_UNIT_FARM_ORCS;

    s32 farmCount = getNumberOfBuildingsOfType(context, player->index, farmType, true);
    s32 foodCount = farmCount * 4 + 1;

    if (dudesCount + 1 > foodCount)
    {
        setFlashStatus(context, 1.5f, "NOT ENOUGH FOOD... BUILD MORE FARMS");
        return false;
    }

    return true;
}

bool checkRectToBuild(WarContext* context, s32 x, s32 y, s32 w, s32 h)
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

bool checkTileToBuild(WarContext* context, WarUnitType buildingToBuild, s32 x, s32 y)
{
    WarUnitData data = getUnitData(buildingToBuild);

    if (!checkRectToBuild(context, x, y, data.sizex, data.sizey))
    {
        setFlashStatus(context, 1.5f, "CAN'T BUILD THERE");
        return false;
    }

    return true;
}

bool checkTileToBuildRoadOrWall(WarContext* context, s32 x, s32 y)
{
    if (!checkRectToBuild(context, x, y, 1, 1))
    {
        setFlashStatus(context, 1.5f, "CAN'T BUILD THERE");
        return false;
    }

    return true;
}

WarEntityList* getNearUnits(WarContext* context, vec2 tilePosition, s32 distance)
{
    WarEntityList* nearUnits = (WarEntityList*)xmalloc(sizeof(WarEntityList));
    WarEntityListInit(nearUnits, WarEntityListNonFreeOptions);

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (other)
        {
            if (tileInRange(other, tilePosition, distance))
            {
                WarEntityListAdd(nearUnits, other);
            }
        }
    }

    return nearUnits;
}

WarEntity* getNearEnemy(WarContext* context, WarEntity* entity)
{
    vec2 position = getUnitCenterPosition(entity, true);

    WarEntityList* entities = getEntities(context);
    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity* other = entities->items[i];
        if (other && areEnemies(context, entity, other) && canAttack(context, entity, other))
        {
            if (isUnit(other))
            {
                if (other->unit.invisible)
                    continue;
            }

            if (tileInRange(other, position, NEAR_ENEMY_RADIUS))
            {
                return other;
            }
        }
    }

    return NULL;
}

bool isBeingAttackedBy(WarEntity* entity, WarEntity* other)
{
    if (!isFollowing(other) && !isMoving(other))
    {
        WarState* attackState = getAttackState(other);
        return attackState && attackState->attack.targetEntityId == entity->id;
    }

    return false;
}

bool isBeingAttacked(WarContext* context, WarEntity* entity)
{

    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (isBeingAttackedBy(entity, other))
        {
            return true;
        }
    }

    return false;
}

WarEntity* getAttacker(WarContext* context, WarEntity* entity)
{
    WarEntityList* units = getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (other && isBeingAttackedBy(entity, other))
        {
            return other;
        }
    }

    return NULL;
}

WarEntity* getAttackTarget(WarContext* context, WarEntity* entity)
{
    if (!isFollowing(entity) && !isMoving(entity))
    {
        WarState* attackState = getAttackState(entity);
        if (attackState)
        {
            WarEntityId targetEntityId = attackState->attack.targetEntityId;
            return findEntity(context, targetEntityId);
        }
    }

    return NULL;
}

s32 getTotalDamage(s32 minDamage, s32 rndDamage, s32 armor)
{
    return minDamage + max(rndDamage - armor, 0);
}

void takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage)
{
    assert(isUnit(entity));

    WarUnitComponent *unit = &entity->unit;

    if (unit->invulnerable)
        return;

    // Minimal damage + [Random damage - Enemy's Armor]
    s32 damage = getTotalDamage(minDamage, rndDamage, unit->armor);
    decreaseUnitHp(context, entity, damage);

    if (unit->hp == 0)
    {
        if (isBuildingUnit(entity))
        {
            WarState* collapseState = createCollapseState(context, entity);
            changeNextState(context, entity, collapseState, true, true);

            createAudioRandom(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, false);
        }
        else
        {
            vec2 position = getUnitCenterPosition(entity, false);

            WarState* deathState = createDeathState(context, entity);
            changeNextState(context, entity, deathState, true, true);

            if (entity->unit.type == WAR_UNIT_SCORPION ||
                entity->unit.type == WAR_UNIT_SPIDER)
            {
                createAudioWithPosition(context, WAR_DEAD_SPIDER_SCORPION, position, false);
            }
            else if (entity->unit.type == WAR_UNIT_CATAPULT_HUMANS ||
                     entity->unit.type == WAR_UNIT_CATAPULT_ORCS)
            {
                createAudioRandomWithPosition(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, position, false);
            }
            else
            {
                WarAudioId audioId = isHumanUnit(entity)? WAR_HUMAN_DEAD : WAR_ORC_DEAD;
                createAudioWithPosition(context, audioId, position, false);
            }
        }
    }
    else if (isBuildingUnit(entity))
    {
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
}

void takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage)
{
    assert(isWall(entity));

    // Minimal damage + [Random damage - Enemy's Armor]
    s32 damage = getTotalDamage(minDamage, rndDamage, 0);
    piece->hp -= damage;
    piece->hp = max(piece->hp, 0);
}

void rangeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    if (isMagicUnit(entity))
    {
        // Since the attack of magic units are considered "kind of spells"
        // it will consume mana, at 2 per shot.
        if (decreaseUnitMana(context, entity, 2))
        {
            vec2 origin = getUnitCenterPosition(entity, false);
            vec2 target = getUnitCenterPosition(targetEntity, false);
            WarProjectileType projectileType = getProjectileType(unit->type);
            createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
        }
        else
        {
            // stop attacking if the magic unit rans out of mana
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }
    }
    else
    {
        vec2 origin = getUnitCenterPosition(entity, false);
        vec2 target = getUnitCenterPosition(targetEntity, false);
        WarProjectileType projectileType = getProjectileType(unit->type);
        createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
    }
}

void rangeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    if (isMagicUnit(entity))
    {
        // Since the attack of magic units are considered "kind of spells"
        // it will consume mana, at 2 per shot.
        if (decreaseUnitMana(context, entity, 2))
        {
            vec2 origin = getUnitCenterPosition(entity, false);
            vec2 target = vec2TileToMapCoordinates(vec2f(piece->tilex, piece->tiley), true);
            WarProjectileType projectileType = getProjectileType(unit->type);
            createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
        }
        else
        {
            // stop attacking if the magic unit rans out of mana
            WarState* idleState = createIdleState(context, entity, true);
            changeNextState(context, entity, idleState, true, true);
        }
    }
    else
    {
        vec2 origin = getUnitCenterPosition(entity, false);
        vec2 target = vec2TileToMapCoordinates(vec2i(piece->tilex, piece->tiley), true);
        WarProjectileType projectileType = getProjectileType(unit->type);
        createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
    }
}

void meleeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    // every unit has a 20 percent chance to miss (except catapults)
    if (isCatapultUnit(entity) || chance(80))
    {
        takeDamage(context, targetEntity, unit->minDamage, unit->rndDamage);
    }
}

void meleeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    // every unit has a 20 percent chance to miss (except catapults)
    if (isCatapultUnit(entity) || chance(80))
    {
        takeWallDamage(context, targetEntity, piece, unit->minDamage, unit->rndDamage);
    }
}

s32 mine(WarContext* context, WarEntity* goldmine, s32 amount)
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
            WarState* collapseState = createCollapseState(context, goldmine);
            changeNextState(context, goldmine, collapseState, true, true);

            createAudioRandom(context, WAR_BUILDING_COLLAPSE_1, WAR_BUILDING_COLLAPSE_3, false);
        }
    }

    return amount;
}
