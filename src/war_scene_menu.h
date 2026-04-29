#pragma once

#include "war_types.h"

void wsm_enterSceneMainMenu(WarContext* context);

void wsm_createMainMenu(WarContext* context);
void wsm_createSinglePlayerMenu(WarContext* context);
void wsm_createLoadMenu(WarContext* context);
void wsm_createCustomGameMenu(WarContext* context);

void wsm_showOrHideMainMenu(WarContext* context, bool status);
void wsm_showOrHideSinglePlayer(WarContext* context, bool status);
void wsm_showOrHideCustomGame(WarContext* context, bool status);

// menu button handlers
void wsm_handleMenuSinglePlayer(WarContext* context, WarEntity* entity);
void wsm_handleMenuQuit(WarContext* context, WarEntity* entity);

void wsm_handleSinglePlayerOrc(WarContext* context, WarEntity* entity);
void wsm_handleSinglePlayerHuman(WarContext* context, WarEntity* entity);
void wsm_handleCustomGame(WarContext* context, WarEntity* entity);
void wsm_handleSinglePlayerCancel(WarContext* context, WarEntity* entity);
void wsm_handleYourRaceLeft(WarContext* context, WarEntity* entity);
void wsm_handleYourRaceRight(WarContext* context, WarEntity* entity);
void wsm_handleEnemyRaceLeft(WarContext* context, WarEntity* entity);
void wsm_handleEnemyRaceRight(WarContext* context, WarEntity* entity);
void wsm_handleMapLeft(WarContext* context, WarEntity* entity);
void wsm_handleMapRight(WarContext* context, WarEntity* entity);
void wsm_handleCustomGameOk(WarContext* context, WarEntity* entity);
