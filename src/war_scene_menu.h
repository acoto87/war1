#pragma once

#include "war.h"

void wsc_enterSceneMainMenu(WarContext* context);

void wsc_createMainMenu(WarContext* context);
void wsc_createSinglePlayerMenu(WarContext* context);
void wsc_createLoadMenu(WarContext* context);
void wsc_createCustomGameMenu(WarContext* context);

void wsc_showOrHideMainMenu(WarContext* context, bool status);
void wsc_showOrHideSinglePlayer(WarContext* context, bool status);
void wsc_showOrHideCustomGame(WarContext* context, bool status);

// menu button handlers
void wsc_handleMenuSinglePlayer(WarContext* context, WarEntity* entity);
void wsc_handleMenuQuit(WarContext* context, WarEntity* entity);

void wsc_handleSinglePlayerOrc(WarContext* context, WarEntity* entity);
void wsc_handleSinglePlayerHuman(WarContext* context, WarEntity* entity);
void wsc_handleCustomGame(WarContext* context, WarEntity* entity);
void wsc_handleSinglePlayerCancel(WarContext* context, WarEntity* entity);
void wsc_handleYourRaceLeft(WarContext* context, WarEntity* entity);
void wsc_handleYourRaceRight(WarContext* context, WarEntity* entity);
void wsc_handleEnemyRaceLeft(WarContext* context, WarEntity* entity);
void wsc_handleEnemyRaceRight(WarContext* context, WarEntity* entity);
void wsc_handleMapLeft(WarContext* context, WarEntity* entity);
void wsc_handleMapRight(WarContext* context, WarEntity* entity);
void wsc_handleCustomGameOk(WarContext* context, WarEntity* entity);
