#pragma once

#include "war_editor.h"

// Create the minimap texture if needed.
bool weminimap_create(WarEditorContext* ctx);

// Rebuild the 64x64 minimap texture from current map data.
void weminimap_rebuild(WarEditorContext* ctx);

// Draw minimap panel and viewport rectangle; supports click-to-navigate.
void weminimap_drawPanel(WarEditorContext* ctx);

// Mark minimap as dirty so it will rebuild on the next throttle window.
void weminimap_markDirty(WarEditorContext* ctx);
