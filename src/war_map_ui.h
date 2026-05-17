#pragma once

#include "war.h"

void wmui_createMapUI(WarContext* context);

WarEntity* wmui_createUIMinimap(WarContext* context, String name, vec2 position);

void wmui_setStatus(WarContext* context, s32 highlightIndex, s32 highlightCount, s32 gold, s32 wood, StringView text);
void wmui_setFlashStatus(WarContext* context, f32 duration, String text);

void wmui_renderSelectionRect(WarContext* context);
void wmui_renderCommand(WarContext* context);
void wmui_renderMapUI(WarContext* context);
