#pragma once

#include "war_types.h"

#define isEntityOfType(entity, entityType) ((entity)->type == (entityType))

void went_addTransformComponent(WarContext* context, WarEntity* entity, vec2 position);
void went_removeTransformComponent(WarContext* context, WarEntity* entity);

void went_addSpriteComponent(WarContext* context, WarEntity* entity, WarSprite sprite);
void went_addSpriteComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef spriteResourceRef);
void went_removeSpriteComponent(WarContext* context, WarEntity* entity);

void went_addUnitComponent(WarContext* context,
                      WarEntity* entity,
                      WarUnitType type,
                      s32 x,
                      s32 y,
                      u8 player,
                      WarResourceKind resourceKind,
                      u32 amount);
void went_removeUnitComponent(WarContext* context, WarEntity* entity);

void went_addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces);
void went_removeRoadComponent(WarContext* context, WarEntity* entity);

void went_addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces);
void went_removeWallComponent(WarContext* context, WarEntity* entity);

void went_addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces);
void went_removeRuinComponent(WarContext* context, WarEntity* entity);

void went_addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees);
void went_removeForestComponent(WarContext* context, WarEntity* entity);

void went_addStateMachineComponent(WarContext* context, WarEntity* entity);
void went_removeStateMachineComponent(WarContext* context, WarEntity* entity);

void went_addAnimationsComponent(WarContext* context, WarEntity* entity);
void went_removeAnimationsComponent(WarContext* context, WarEntity* entity);

void went_addUIComponent(WarContext* context, WarEntity* entity, String name);
void went_removeUIComponent(WarContext* context, WarEntity* entity);

void went_addTextComponent(WarContext* context, WarEntity* entity, s32 fontIndex, f32 fontSize, String text);
void went_removeTextComponent(WarContext* context, WarEntity* entity);

void went_addRectComponent(WarContext* context, WarEntity* entity, vec2 size, WarColor color);
void went_removeRectComponent(WarContext* context, WarEntity* entity);

void went_addButtonComponent(WarContext* context, WarEntity* entity, WarSprite normalSprite, WarSprite pressedSprite);
void went_addButtonComponentFromResource(WarContext* context,
                                    WarEntity* entity,
                                    WarSpriteResourceRef normalRef,
                                    WarSpriteResourceRef pressedRef);
void went_removeButtonComponent(WarContext* context, WarEntity* entity);

void went_addAudioComponent(WarContext* context, WarEntity* entity, WarAudioType type, s32 resourceIndex, bool loop);
void went_removeAudioComponent(WarContext* context, WarEntity* entity);

void went_addCursorComponent(WarContext* context, WarEntity* entity, WarCursorType type, vec2 hot);
void went_removeCursorComponent(WarContext* context, WarEntity* entity);

void went_addProjectileComponent(WarContext* context, WarEntity* entity, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target, s32 speed);
void went_removeProjectileComponent(WarContext* context, WarEntity* entity);

void went_addPoisonCloudComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time);
void went_removePoisonCloudComponent(WarContext* context, WarEntity* entity);

void went_addSightComponent(WarContext* context, WarEntity* entity, vec2 position, f32 time);
void went_removeSightComponent(WarContext* context, WarEntity* entity);

// Roads
WarEntity* went_createRoad(WarContext* context);
void went_addRoadPiece(WarEntity* entity, s32 x, s32 y, s32 player);
void went_addRoadPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct);
bool went_hasRoadPieceAtPosition(WarEntity* entity, s32 x, s32 y);
WarRoadPiece* went_getRoadPieceAtPosition(WarEntity* entity, s32 x, s32 y);
void went_removeRoadPiece(WarEntity* entity, WarRoadPiece* piece);
void went_determineRoadTypes(WarContext* context, WarEntity* entity);

// Walls
WarEntity* went_createWall(WarContext* context);
WarWallPiece* went_addWallPiece(WarEntity* entity, s32 x, s32 y, s32 player);
void went_addWallPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct);
bool went_hasWallPieceAtPosition(WarEntity* entity, s32 x, s32 y);
WarWallPiece* went_getWallPieceAtPosition(WarEntity* entity, s32 x, s32 y);
void went_removeWallPiece(WarEntity* entity, WarWallPiece* piece);
void went_determineWallTypes(WarContext* context, WarEntity* entity);
void went_takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage);

// Ruins
WarEntity* went_createRuins(WarContext* context);
void went_addRuinsPieces(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 dim);
bool went_hasRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y);
WarRuinPiece* went_getRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y);
void went_determineRuinTypes(WarContext* context, WarEntity* entity);

// Trees
bool went_hasTreeAtPosition(WarEntity* forest, s32 x, s32 y);
WarTree* went_getTreeAtPosition(WarEntity* forest, s32 x, s32 y);
void went_determineTreeTiles(WarContext* context, WarEntity* forest);
void went_determineAllTreeTiles(WarContext* context);
WarTree* went_findAccesibleTree(WarContext* context, WarEntity* forest, vec2 position);
void went_plantTree(WarContext* context, WarEntity* forest, s32 x, s32 y);
bool went_validTree(WarContext* context, WarEntity* forest, WarTree* tree);
s32 went_chopTree(WarContext* context, WarEntity* forest, WarTree* tree, s32 amount);

// Entities
WarEntity* went_createEntity(WarContext* context, WarEntityType type, bool addToMap);
WarEntity* went_createUnit(WarContext* context, WarUnitType type,
                      s32 x, s32 y, u8 player,
                      WarResourceKind resourceKind, u32 amount,
                      bool addToMap);
WarEntity* went_createDude(WarContext* context, WarUnitType type, s32 x, s32 y, u8 player, bool isGoingToTrain);
WarEntity* went_createBuilding(WarContext* context, WarUnitType type, s32 x, s32 y, u8 player, bool isGoingToBuild);
WarEntity* went_findEntity(WarContext* context, WarEntityId id);
WarEntity* went_findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type);
WarEntity* went_findUIEntity(WarContext* context, StringView name);
WarEntity* went_findEntityUnderCursor(WarContext* context, bool includeTrees, bool includeWalls);
void went_removeEntity(WarContext* context, WarEntity* entity);
void went_removeEntityById(WarContext* context, WarEntityId id);
void went_renderEntity(WarContext* context, WarEntity* entity);
void went_renderEntitiesOfType(WarContext* context, WarEntityType type);
void went_renderUnitSelection(WarContext* context);
bool went_isStaticEntity(WarEntity* entity);

void went_initEntityManager(WarContext* context, WarEntityManager* manager);
WarEntityManager* went_getEntityManager(WarContext* context);
WarEntityList* went_getEntities(WarContext* context);
WarEntityList* went_getEntitiesOfType(WarContext* context, WarEntityType type);
WarEntityList* went_getUnitsOfType(WarContext* context, WarUnitType type);
WarEntityList* went_getUIEntities(WarContext* context);

void went_increaseUpgradeLevel(WarContext* context, WarPlayerInfo* player, WarUpgradeType upgrade);
bool went_enoughPlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
bool went_decreasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
void went_increasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
bool went_increaseUnitHp(WarContext* context, WarEntity* entity, s32 hp);
bool went_decreaseUnitHp(WarContext* context, WarEntity* entity, s32 hp);
bool went_decreaseUnitMana(WarContext* context, WarEntity* entity, s32 mana);
void went_increaseUnitMana(WarContext* context, WarEntity* entity, s32 mana);
bool went_enoughFarmFood(WarContext* context, WarPlayerInfo* player);
bool went_checkFarmFood(WarContext* context, WarPlayerInfo* player);
bool went_checkRectToBuild(WarContext* context, s32 x, s32 y, s32 w, s32 h);
bool went_checkTileToBuild(WarContext* context, WarUnitType buildingToBuild, s32 x, s32 y);
bool went_checkTileToBuildRoadOrWall(WarContext* context, s32 x, s32 y);
WarEntityList* went_getNearUnits(WarContext* context, vec2 tilePosition, s32 distance);
WarEntity* went_getNearEnemy(WarContext* context, WarEntity* entity);
bool went_isBeingAttackedBy(WarEntity* entity, WarEntity* other);
bool went_isBeingAttacked(WarContext* context, WarEntity* entity);
WarEntity* went_getAttacker(WarContext* context, WarEntity* entity);
WarEntity* went_getAttackTarget(WarContext* context, WarEntity* entity);
s32 went_getTotalDamage(s32 minDamage, s32 rndDamage, s32 armor);
void went_takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage);
void went_rangeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity);
void went_rangeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece);
void went_meleeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity);
void went_meleeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece);
s32 went_mine(WarContext* context, WarEntity* goldmine, s32 amount);
