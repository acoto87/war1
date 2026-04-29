#pragma once

#include "SDL3/SDL.h"

#include "war_types.h"

void wrend_renderInit(WarContext* context);
void wrend_renderSave(WarContext* context);
void wrend_renderRestore(WarContext* context);
void wrend_renderTranslate(WarContext* context, f32 tx, f32 ty);
void wrend_renderScale(WarContext* context, f32 sx, f32 sy);
void wrend_renderGlobalAlpha(WarContext* context, f32 a);
void wrend_renderFillRect(WarContext* context, rect r, WarColor color);
void wrend_renderFillRects(WarContext* context, s32 count, rect r[], WarColor color);
void wrend_renderStrokeRect(WarContext* context, rect r, WarColor color, f32 width);
void wrend_renderStrokeLine(WarContext* context, vec2 p1, vec2 p2, WarColor color, f32 width);
void wrend_renderSubImage(WarContext* context, SDL_Texture* texture, rect rs, rect rd, vec2 scale);

WarColor getColorFromList(s32 index);
