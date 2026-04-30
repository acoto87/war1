#pragma once

#include "war.h"

void wmm_createMenu(WarContext* context);
void wmm_createOptionsMenu(WarContext* context);
void wmm_createGameOverMenu(WarContext* context);
void wmm_createQuitMenu(WarContext* context);

void wmm_enableOrDisableCommandButtons(WarContext* context, bool interactive);
void wmm_showOrHideMenu(WarContext* context, bool status);
void wmm_showOrHideOptionsMenu(WarContext* context, bool status);
void wmm_showOrHideGameOverMenu(WarContext* context, bool status);
void wmm_showOrHideQuitMenu(WarContext* context, bool status);
void wmm_showDemoEndMenu(WarContext* context, bool status);

// menu button handlers
void wmm_handleMenu(WarContext* context, WarEntity* entity);
void wmm_handleOptions(WarContext* context, WarEntity* entity);
void wmm_handleObjectives(WarContext* context, WarEntity* entity);
void wmm_handleRestart(WarContext* context, WarEntity* entity);
void wmm_handleContinue(WarContext* context, WarEntity* entity);
void wmm_handleQuit(WarContext* context, WarEntity* entity);

void wmm_handleGameSpeedDec(WarContext* context, WarEntity* entity);
void wmm_handleGameSpeedInc(WarContext* context, WarEntity* entity);
void wmm_handleMusicVolDec(WarContext* context, WarEntity* entity);
void wmm_handleMusicVolInc(WarContext* context, WarEntity* entity);
void wmm_handleSfxVolDec(WarContext* context, WarEntity* entity);
void wmm_handleSfxVolInc(WarContext* context, WarEntity* entity);
void wmm_handleMouseScrollSpeedDec(WarContext* context, WarEntity* entity);
void wmm_handleMouseScrollSpeedInc(WarContext* context, WarEntity* entity);
void wmm_handleKeyScrollSpeedDec(WarContext* context, WarEntity* entity);
void wmm_handleKeyScrollSpeedInc(WarContext* context, WarEntity* entity);
void wmm_handleOptionsOk(WarContext* context, WarEntity* entity);
void wmm_handleOptionsCancel(WarContext* context, WarEntity* entity);

void wmm_handleObjectivesMenu(WarContext* context, WarEntity* entity);

void wmm_handleRestartRestart(WarContext* context, WarEntity* entity);
void wmm_handleRestartCancel(WarContext* context, WarEntity* entity);

void wmm_handleGameOverSave(WarContext* context, WarEntity* entity);
void wmm_handleGameOverContinue(WarContext* context, WarEntity* entity);

void wmm_handleQuitQuit(WarContext* context, WarEntity* entity);
void wmm_handleQuitMenu(WarContext* context, WarEntity* entity);
void wmm_handleQuitCancel(WarContext* context, WarEntity* entity);

void wmm_handleDemoEndMenu(WarContext* context, WarEntity* entity);
