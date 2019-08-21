void enterSceneMainMenu(WarContext* context);

void createMainMenu(WarContext* context);
void createSinglePlayerMenu(WarContext* context);
void createLoadMenu(WarContext* context);

void showOrHideMainMenu(WarContext* context, bool status);
void showOrHideSinglePlayer(WarContext* context, bool status);

// menu button handlers
void handleMenuSinglePlayer(WarContext* context, WarEntity* entity);
void handleMenuQuit(WarContext* context, WarEntity* entity);

void handleSinglePlayerOrc(WarContext* context, WarEntity* entity);
void handleSinglePlayerHuman(WarContext* context, WarEntity* entity);
void handleSinglePlayerCancel(WarContext* context, WarEntity* entity);