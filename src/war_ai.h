WarAI* createAI(WarContext* context);
WarAICommand* createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type);

WarAICommand* createRequestAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count);
WarAICommand* createWaitAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count);
WarAICommand* createSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, f32 time);
WarAICommand* createGoldAICommand(WarContext* context, WarPlayerInfo* aiPlayer, s32 count, bool freeWorker);
WarAICommand* createWoodAICommand(WarContext* context, WarPlayerInfo* aiPlayer, s32 count, bool freeWorker);

void initAIPlayers(WarContext* context);
void updateAIPlayers(WarContext* context);
