WarAI* createAI(WarContext* context);
WarAICommand* createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type);
bool executeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command);

void initAI(WarContext* context);
void updateAI(WarContext* context);