WarAI* createAI(WarContext* context);
WarAICommand* createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type);

WarAICommand* createUnitRequest(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count);
WarAICommand* createWaitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count);
WarAICommand* createSleepForTime(WarContext* context, WarPlayerInfo* aiPlayer, f32 time);

void initAIPlayers(WarContext* context);
void updateAIPlayers(WarContext* context);
