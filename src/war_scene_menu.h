#pragma once

#include "war_types.h"

void wscm_enterSceneMainMenu(WarContext* context);

void wscm_createMainMenu(WarContext* context);
void wscm_createSinglePlayerMenu(WarContext* context);
void wscm_createLoadMenu(WarContext* context);
void wscm_createCustomGameMenu(WarContext* context);

void wscm_showOrHideMainMenu(WarContext* context, bool status);
void wscm_showOrHideSinglePlayer(WarContext* context, bool status);
void wscm_showOrHideCustomGame(WarContext* context, bool status);

// menu button handlers
void wscm_handleMenuSinglePlayer(WarContext* context, WarEntity* entity);
void wscm_handleMenuQuit(WarContext* context, WarEntity* entity);

void wscm_handleSinglePlayerOrc(WarContext* context, WarEntity* entity);
void wscm_handleSinglePlayerHuman(WarContext* context, WarEntity* entity);
void wscm_handleCustomGame(WarContext* context, WarEntity* entity);
void wscm_handleSinglePlayerCancel(WarContext* context, WarEntity* entity);
void wscm_handleYourRaceLeft(WarContext* context, WarEntity* entity);
void wscm_handleYourRaceRight(WarContext* context, WarEntity* entity);
void wscm_handleEnemyRaceLeft(WarContext* context, WarEntity* entity);
void wscm_handleEnemyRaceRight(WarContext* context, WarEntity* entity);
void wscm_handleMapLeft(WarContext* context, WarEntity* entity);
void wscm_handleMapRight(WarContext* context, WarEntity* entity);
void wscm_handleCustomGameOk(WarContext* context, WarEntity* entity);
