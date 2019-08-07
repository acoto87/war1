void createMenu(WarContext* context);
void createOptionsMenu(WarContext* context);
void createGameOverMenu(WarContext* context);
void createQuitMenu(WarContext* context);

void enableOrDisableCommandButtons(WarContext* context, bool interactive);
void showOrHideMenu(WarContext* context, bool status);
void showOrHideOptionsMenu(WarContext* context, bool status);
void showOrHideGameOverMenu(WarContext* context, bool status);
void showOrHideQuitMenu(WarContext* context, bool status);

// menu button handlers
void handleMenu(WarContext* context, WarEntity* entity);
void handleContinue(WarContext* context, WarEntity* entity);
void handleQuit(WarContext* context, WarEntity* entity);

void handleOptions(WarContext* context, WarEntity* entity);
void handleGameSpeedDec(WarContext* context, WarEntity* entity);
void handleGameSpeedInc(WarContext* context, WarEntity* entity);
void handleMusicVolDec(WarContext* context, WarEntity* entity);
void handleMusicVolInc(WarContext* context, WarEntity* entity);
void handleSfxVolDec(WarContext* context, WarEntity* entity);
void handleSfxVolInc(WarContext* context, WarEntity* entity);
void handleMouseScrollSpeedDec(WarContext* context, WarEntity* entity);
void handleMouseScrollSpeedInc(WarContext* context, WarEntity* entity);
void handleKeyScrollSpeedDec(WarContext* context, WarEntity* entity);
void handleKeyScrollSpeedInc(WarContext* context, WarEntity* entity);
void handleOptionsOk(WarContext* context, WarEntity* entity);
void handleOptionsCancel(WarContext* context, WarEntity* entity);

void handleGameOverContinue(WarContext* context, WarEntity* entity);

void handleQuitQuit(WarContext* context, WarEntity* entity);
void handleQuitCancel(WarContext* context, WarEntity* entity);