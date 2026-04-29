#pragma once

#include "SDL3/SDL.h"

#include "war_types.h"

void wr_renderInit(WarContext* context);
void wr_renderSave(WarContext* context);
void wr_renderRestore(WarContext* context);
void wr_renderTranslate(WarContext* context, f32 tx, f32 ty);
void wr_renderScale(WarContext* context, f32 sx, f32 sy);
void wr_renderGlobalAlpha(WarContext* context, f32 a);
void wr_renderFillRect(WarContext* context, rect r, WarColor color);
void wr_renderFillRects(WarContext* context, s32 count, rect r[], WarColor color);
void wr_renderStrokeRect(WarContext* context, rect r, WarColor color, f32 width);
void wr_renderStrokeLine(WarContext* context, vec2 p1, vec2 p2, WarColor color, f32 width);
void wr_renderSubImage(WarContext* context, SDL_Texture* texture, rect rs, rect rd, vec2 scale);

WarColor getColorFromList(s32 index);
