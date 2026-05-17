#pragma once

#include "war.h"

void wsc_enterSceneMainMenu(WarContext* context);
void wsc_renderSceneMainMenu(WarContext* context);

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
