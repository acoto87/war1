s32 getTotalDamage(s32 minDamage, s32 rndDamage, s32 armor)
{
    return minDamage + max(rndDamage - armor, 0);
}

void addTransformComponent(WarContext* context, WarEntity* entity, vec2 position);
void removeTransformComponent(WarContext* context, WarEntity* entity);

void addSpriteComponent(WarContext* context, WarEntity* entity, WarSprite sprite);
void addSpriteComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef spriteResourceRef);
void removeSpriteComponent(WarContext* context, WarEntity* entity);

void addUnitComponent(WarContext* context, 
                      WarEntity* entity, 
                      WarUnitType type, 
                      s32 x, 
                      s32 y, 
                      u8 player, 
                      WarResourceKind resourceKind, 
                      u32 amount);
void removeUnitComponent(WarContext* context, WarEntity* entity);

void addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces);
void removeRoadComponent(WarContext* context, WarEntity* entity);

void addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces);
void removeWallComponent(WarContext* context, WarEntity* entity);

void addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces);
void removeRuinComponent(WarContext* context, WarEntity* entity);

void addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees);
void removeForestComponent(WarContext* context, WarEntity* entity);

void addStateMachineComponent(WarContext* context, WarEntity* entity);
void removeStateMachineComponent(WarContext* context, WarEntity* entity);

void addAnimationsComponent(WarContext* context, WarEntity* entity);
void removeAnimationsComponent(WarContext* context, WarEntity* entity);

void addUIComponent(WarContext* context, WarEntity* entity, char* name);
void removeUIComponent(WarContext* context, WarEntity* entity);

void addTextComponent(WarContext* context, WarEntity* entity, char* text);
void removeTextComponent(WarContext* context, WarEntity* entity);

void addRectComponent(WarContext* context, WarEntity* entity, vec2 size, u8Color color);
void removeRectComponent(WarContext* context, WarEntity* entity);

void addButtonComponent(WarContext* context, WarEntity* entity, WarSprite normalSprite, WarSprite pressedSprite);
void addButtonComponentFromResource(WarContext* context, 
                                    WarEntity* entity, 
                                    WarSpriteResourceRef normalRef,
                                    WarSpriteResourceRef pressedRef);
void removeButtonComponent(WarContext* context, WarEntity* entity);

// Roads
WarEntity* createRoad(WarContext* context);
void addRoadPiece(WarEntity* entity, s32 x, s32 y, s32 player);
void addRoadPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct);
bool hasRoadPieceAtPosition(WarEntity* entity, s32 x, s32 y);
WarRoadPiece* getRoadPieceAtPosition(WarEntity* entity, s32 x, s32 y);
void removeRoadPiece(WarEntity* entity, WarRoadPiece* piece);
void determineRoadTypes(WarContext* context, WarEntity* entity);

// Walls
WarEntity* createWall(WarContext* context);
void addWallPiece(WarEntity* entity, s32 x, s32 y, s32 player);
void addWallPiecesFromConstruct(WarEntity* entity, WarLevelConstruct *construct);
bool hasWallPieceAtPosition(WarEntity* entity, s32 x, s32 y);
WarWallPiece* getWallPieceAtPosition(WarEntity* entity, s32 x, s32 y);
void removeWallPiece(WarEntity* entity, WarWallPiece* piece);
void determineWallTypes(WarContext* context, WarEntity* entity);
void takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage);

// Ruins
WarEntity* createRuins(WarContext* context);
void addRuinsPieces(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 dim);
bool hasRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y);
WarRuinPiece* getRuinPieceAtPosition(WarEntity* ruins, s32 x, s32 y);
void determineRuinTypes(WarContext* context, WarEntity* entity);

// Trees
bool hasTreeAtPosition(WarEntity* forest, s32 x, s32 y);
WarTree* getTreeAtPosition(WarEntity* forest, s32 x, s32 y);
void determineTreeTiles(WarContext* context, WarEntity* forest);
void determineAllTreeTiles(WarContext* context);
WarTree* findAccesibleTree(WarContext* context, WarEntity* forest, vec2 position);
void plantTree(WarContext* context, WarEntity* forest, s32 x, s32 y);
bool validTree(WarContext* context, WarEntity* forest, WarTree* tree);
s32 chopTree(WarContext* context, WarEntity* forest, WarTree* tree, s32 amount);

// Entities
WarEntity* createEntity(WarContext* context, WarEntityType type, bool addToMap);
WarEntity* createUnit(WarContext* context, WarUnitType type, 
                      s32 x, s32 y, u8 player, 
                      WarResourceKind resourceKind, u32 amount,
                      bool isGoingToTrain);
s32 findEntityIndex(WarContext* context, WarEntityId id);
WarEntity* findEntity(WarContext* context, WarEntityId id);
WarEntity* findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type);
WarEntity* findUIEntity(WarContext* context, char* name);
void removeEntityById(WarContext* context, WarEntityId id);
void renderEntity(WarContext* context, WarEntity* entity, bool selected);

void increaseUpgradeLevel(WarContext* context, WarPlayerInfo* player, WarUpgradeType upgrade);
bool withdrawFromPlayer(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood);
bool checkFarmFood(WarContext* context, WarPlayerInfo* player);
void takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage);
s32 mine(WarContext* context, WarEntity* goldmine, s32 amount);