#pragma once

#include "war_types.h"

void wmui_createMapUI(WarContext* context);

WarEntity* wmui_createUIMinimap(WarContext* context, String name, vec2 position);

void wmui_updateGoldText(WarContext* context);
void wmui_updateWoodText(WarContext* context);
void wmui_updateSelectedUnitsInfo(WarContext* context);

void wmui_setStatus(WarContext* context, s32 highlightIndex, s32 highlightCount, s32 gold, s32 wood, String text);
void wmui_setFlashStatus(WarContext* context, f32 duration, String text);
void wmui_setLifeBar(WarEntity* rectLifeBar, WarUnitComponent* unit);
void wmui_setManaBar(WarEntity* rectMagicBar, WarUnitComponent* unit);
void wmui_setPercentBar(WarEntity* rectPercentBar, WarEntity* rectPercentText, WarUnitComponent* unit);

void wmui_renderSelectionRect(WarContext* context);
void wmui_renderCommand(WarContext* context);
void wmui_renderMapUI(WarContext* context);
