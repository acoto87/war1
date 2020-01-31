#define STATUS_TEXT_MAX_LENGTH 28

void createMapUI(WarContext* context);

WarEntity* createUIMinimap(WarContext* context, char* name, vec2 position);

void updateGoldText(WarContext* context);
void updateWoodText(WarContext* context);
void updateSelectedUnitsInfo(WarContext* context);

void setStatus(WarContext* context, s32 highlightIndex, s32 highlightCount, s32 gold, s32 wood, char* text, ...);
void setFlashStatus(WarContext* context, f32 duration, char* text);
void setLifeBar(WarEntity* rectLifeBar, WarUnitComponent* unit);
void setManaBar(WarEntity* rectMagicBar, WarUnitComponent* unit);
void setPercentBar(WarEntity* rectPercentBar, WarEntity* rectPercentText, WarUnitComponent* unit);

void renderSelectionRect(WarContext* context);
void renderCommand(WarContext* context);
void renderMapUI(WarContext* context);