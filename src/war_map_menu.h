void createMenu(WarContext* context);
void createOptionsMenu(WarContext* context);
void createGameOverMenu(WarContext* context);

void enableOrDisableCommandButtons(WarContext* context, bool interactive);
void showOrHideMenu(WarContext* context, bool status);
void showOrHideOptions(WarContext* context, bool status);
void showOrHideGameOver(WarContext* context, bool status);

void handleOpenMenu(WarContext* context, WarEntity* entity);
void handleContinue(WarContext* context, WarEntity* entity);
void handleOpenOptions(WarContext* context, WarEntity* entity);
void handleOptionsOk(WarContext* context, WarEntity* entity);
void handleOptionsCancel(WarContext* context, WarEntity* entity);
void handleGameOverContinue(WarContext* context, WarEntity* entity);