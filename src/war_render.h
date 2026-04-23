#pragma once

#include "SDL3/SDL.h"

#include "war_types.h"

void renderInit(WarContext* context);
void renderSave(WarContext* context);
void renderRestore(WarContext* context);
void renderTranslate(WarContext* context, f32 tx, f32 ty);
void renderScale(WarContext* context, f32 sx, f32 sy);
void renderGlobalAlpha(WarContext* context, f32 a);
void renderFillRect(WarContext* context, rect r, WarColor color);
void renderFillRects(WarContext* context, s32 count, rect r[], WarColor color);
void renderStrokeRect(WarContext* context, rect r, WarColor color, f32 width);
void renderStrokeLine(WarContext* context, vec2 p1, vec2 p2, WarColor color, f32 width);
void renderSubImage(WarContext* context, SDL_Texture* texture, rect rs, rect rd, vec2 scale);

WarColor getColorFromList(s32 index);
