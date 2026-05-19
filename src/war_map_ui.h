#pragma once

#include "war.h"

void wmui_createMapUI(WarContext* context);

WarEntity* wmui_createUIMinimap(WarContext* context, String name, vec2 position);

void wmui_setFlashStatus(WarContext* context, f32 duration, String text);

void wmui_renderMapUI(WarContext* context);
