#define isNeutral(player) (player == 4)
#define isEnemy(player) (player != 0 && !isNeutral(player))
#define isHuman(player) (player == 0)

inline s32 getTotalDamage(s32 minDamage, s32 rndDamage, s32 armour)
{
    return minDamage + maxi(rndDamage - armour, 0);
}

void addTransformComponent(WarContext* context, WarEntity* entity, vec2 position);
void removeTransformComponent(WarContext* context, WarEntity* entity);

void addSpriteComponent(WarContext* context, WarEntity* entity, WarSprite sprite);
void addSpriteComponentFromResource(WarContext* context, WarEntity* entity, s32 resourceIndex);
void removeSpriteComponent(WarContext* context, WarEntity* entity);

void addUnitComponent(WarContext* context, WarEntity* entity, WarUnitType type, s32 x, s32 y, u8 player, WarResourceKind resourceKind, u32 amount);
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
void determineWallTypes(WarEntity* entity);
void takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage);

// Ruins
WarEntity* createRuins(WarContext* context);
void addRuinsPieces(WarContext* context, WarEntity* entity, s32 x, s32 y, s32 dim);
void determineRuinTypes(WarEntity* entity);

// Trees
bool hasTreeAtPosition(WarEntity* forest, s32 x, s32 y);
WarTree* getTreeAtPosition(WarEntity* forest, s32 x, s32 y);
void determineTreeTiles(WarContext* context, WarEntity* forest);
WarTree* findAccesibleTree(WarContext* context, WarEntity* forest, vec2 position);
void plantTree(WarContext* context, WarEntity* forest, s32 x, s32 y);
bool validTree(WarContext* context, WarEntity* forest, WarTree* tree);
void chopTree(WarContext* context, WarEntity* forest, WarTree* tree, s32 amount);

// Entities
WarEntity* createEntity(WarContext* context, WarEntityType type);
s32 findEntityIndex(WarContext* context, WarEntityId id);
WarEntity* findEntity(WarContext* context, WarEntityId id);
WarEntity* findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type);
void removeEntityById(WarContext* context, WarEntityId id);
void renderEntity(WarContext* context, WarEntity* entity, bool selected);

void takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage);
