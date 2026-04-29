#pragma once

#include "war_types.h"

#define isEntityOfType(entity, entityType) ((entity)->type == (entityType))

void we_addTransformComponent(WarContext* context, WarEntity* entity, vec2 position);
void we_removeTransformComponent(WarContext* context, WarEntity* entity);

void we_addSpriteComponent(WarContext* context, WarEntity* entity, WarSprite sprite);
void we_addSpriteComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef spriteResourceRef);
void we_removeSpriteComponent(WarContext* context, WarEntity* entity);

void we_addUnitComponent(WarContext* context,
                      WarEntity* entity,
                      WarUnitType type,
                      s32 x,
                      s32 y,
                      u8 player,
                      WarResourceKind resourceKind,
                      u32 amount);
void we_removeUnitComponent(WarContext* context, WarEntity* entity);

void we_addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces);
void we_removeRoadComponent(WarContext* context, WarEntity* entity);

void we_addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces);
void we_removeWallComponent(WarContext* context, WarEntity* entity);

void we_addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces);
void we_removeRuinComponent(WarContext* context, WarEntity* entity);

void we_addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees);
void we_removeForestComponent(WarContext* context, WarEntity* entity);

void we_addStateMachineComponent(WarContext* context, WarEntity* entity);
void we_removeStateMachineComponent(WarContext* context, WarEntity* entity);

void we_addAnimationsComponent(WarContext* context, WarEntity* entity);
void we_removeAnimationsComponent(WarContext* context, WarEntity* entity);

void we_addUIComponent(WarContext* context, WarEntity* entity, String name);
void we_removeUIComponent(WarContext* context, WarEntity* entity);

void we_addTextComponent(WarContext* context, WarEntity* entity, s32 fontIndex, f32 fontSize, String text);
void we_removeTextComponent(WarContext* context, WarEntity* entity);

void we_addRectComponent(WarContext* context, WarEntity* entity, vec2 size, WarColor color);
void we_removeRectComponent(WarContext* context, WarEntity* entity);

void we_addButtonComponent(WarContext* context, WarEntity* entity, WarSprite normalSprite, WarSprite pressedSprite);
void we_addButtonComponentFromResource(WarContext* context,
                                    WarEntity* entity,
                                    WarSpriteResourceRef normalRef,
                                    WarSpriteResourceRef pressedRef);
void we_removeButtonComponent(WarContext* context, WarEntity* entity);

void we_addAudioComponent(WarContext* context, WarEntity* entity, WarAudioType type, s32 resourceIndex, bool loop);
void we_removeAudioComponent(WarContext* context, WarEntity* entity);

void we_addCursorComponent(WarContext* context, WarEntity* entity, WarCursorType type, vec2 hot);
void we_removeCursorComponent(WarContext* context, WarEntity* entity);

void we_addProjectileComponent(WarContext* context, WarEntity* entity, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target, s32 speed);
void we_removeProjectileComponent(WarContext* context, WarEntity* entity);

void we_addPoisonCloudComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time);
void we_removePoisonCloudComponent(WarContext* context, WarEntity* entity);

void we_addSightComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time);
void we_removeSightComponent(WarContext* context, WarEntity* entity);

// Roads
WarEntity* we_createRoad(WarContext* context);
void we_addRoadPiece(WarEntity* entity, s32 x, s32 y, s32 player);
void we_addRoadPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct);
bool we_hasRoadPieceAtPosition(WarEntity* entity, s32 x, s32 y);
WarRoadPiece* we_getRoadPieceAtPosition(WarEntity* entity, s32 x, s32 y);
void we_removeRoadPiece(WarEntity* entity, WarRoadPiece* piece);
void we_determineRoadTypes(WarContext* context, WarEntity* entity);

// Walls
WarEntity* we_createWall(WarContext* context);
WarWallPiece* we_addWallPiece(WarEntity* entity, s32 x, s32 y, s32 player);
void we_addWallPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct);
bool we_hasWallPieceAtPosition(WarEntity* entity, s32 x, s32 y);
WarWallPiece* we_getWallPieceAtPosition(WarEntity* entity, s32 x, s32 y);
void we_removeWallPiece(WarEntity* entity, WarWallPiece* piece);
void we_determineWallTypes(WarContext* context, WarEntity* entity);
void we_takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage);

// Ruins
WarEntity* we_createRuins(WarContext* context);
void we_addRuinsPieces(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 dim);
bool we_hasRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y);
WarRuinPiece* we_getRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y);
void we_determineRuinTypes(WarContext* context, WarEntity* entity);

// Trees
bool we_hasTreeAtPosition(WarEntity* forest, s32 x, s32 y);
WarTree* we_getTreeAtPosition(WarEntity* forest, s32 x, s32 y);
void we_determineTreeTiles(WarContext* context, WarEntity* forest);
void we_determineAllTreeTiles(WarContext* context);
WarTree* we_findAccesibleTree(WarContext* context, WarEntity* forest, vec2 position);
void we_plantTree(WarContext* context, WarEntity* forest, s32 x, s32 y);
bool we_validTree(WarContext* context, WarEntity* forest, WarTree* tree);
s32 we_chopTree(WarContext* context, WarEntity* forest, WarTree* tree, s32 amount);

// Entities
WarEntity* we_createEntity(WarContext* context, WarEntityType type, bool addToMap);
WarEntity* we_createUnit(WarContext* context, WarUnitType type,
                      s32 x, s32 y, u8 player,
                      WarResourceKind resourceKind, u32 amount,
                      bool addToMap);
WarEntity* we_createDude(WarContext* context, WarUnitType type, s32 x, s32 y, u8 player, bool isGoingToTrain);
WarEntity* we_createBuilding(WarContext* context, WarUnitType type, s32 x, s32 y, u8 player, bool isGoingToBuild);
WarEntity* we_findEntity(WarContext* context, WarEntityId id);
WarEntity* we_findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type);
WarEntity* we_findUIEntity(WarContext* context, StringView name);
WarEntity* we_findEntityUnderCursor(WarContext* context, bool includeTrees, bool includeWalls);
void we_removeEntity(WarContext* context, WarEntity* entity);
void we_removeEntityById(WarContext* context, WarEntityId id);
void we_renderEntity(WarContext* context, WarEntity* entity);
void we_renderEntitiesOfType(WarContext* context, WarEntityType type);
void we_renderUnitSelection(WarContext* context);
bool we_isStaticEntity(WarEntity* entity);

void we_initEntityManager(WarContext* context, WarEntityManager* manager);
WarEntityManager* we_getEntityManager(WarContext* context);
WarEntityList* we_getEntities(WarContext* context);
WarEntityList* we_getEntitiesOfType(WarContext* context, WarEntityType type);
WarEntityList* we_getUnitsOfType(WarContext* context, WarUnitType type);
WarEntityList* we_getUIEntities(WarContext* context);

void we_increaseUpgradeLevel(WarContext* context, WarPlayerInfo* player, WarUpgradeType upgrade);
bool we_enoughPlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
bool we_decreasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
void we_increasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
bool we_increaseUnitHp(WarContext* context, WarEntity* entity, s32 hp);
bool we_decreaseUnitHp(WarContext* context, WarEntity* entity, s32 hp);
bool we_decreaseUnitMana(WarContext* context, WarEntity* entity, s32 mana);
void we_increaseUnitMana(WarContext* context, WarEntity* entity, s32 mana);
bool we_enoughFarmFood(WarContext* context, WarPlayerInfo* player);
bool we_checkFarmFood(WarContext* context, WarPlayerInfo* player);
bool we_checkRectToBuild(WarContext* context, s32 x, s32 y, s32 w, s32 h);
bool we_checkTileToBuild(WarContext* context, WarUnitType buildingToBuild, s32 x, s32 y);
bool we_checkTileToBuildRoadOrWall(WarContext* context, s32 x, s32 y);
WarEntityList* we_getNearUnits(WarContext* context, vec2 tilePosition, s32 distance);
WarEntity* we_getNearEnemy(WarContext* context, WarEntity* entity);
bool we_isBeingAttackedBy(WarEntity* entity, WarEntity* other);
bool we_isBeingAttacked(WarContext* context, WarEntity* entity);
WarEntity* we_getAttacker(WarContext* context, WarEntity* entity);
WarEntity* we_getAttackTarget(WarContext* context, WarEntity* entity);
s32 we_getTotalDamage(s32 minDamage, s32 rndDamage, s32 armor);
void we_takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage);
void we_rangeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity);
void we_rangeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece);
void we_meleeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity);
void we_meleeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece);
s32 we_mine(WarContext* context, WarEntity* goldmine, s32 amount);
