#pragma once

#include "SDL3/SDL.h"

#include "war.h"
#include "war_color.h"
#include "war_math.h"

void wr_init(WarContext* context);
void wr_save(WarContext* context);
void wr_restore(WarContext* context);
void wr_translate(WarContext* context, f32 tx, f32 ty);
void wr_scale(WarContext* context, f32 sx, f32 sy);
void wr_globalAlpha(WarContext* context, f32 a);
void wr_fillRect(WarContext* context, rect r, WarColor color);
void wr_fillRects(WarContext* context, s32 count, rect r[], WarColor color);
void wr_strokeRect(WarContext* context, rect r, WarColor color, f32 width);
void wr_strokeLine(WarContext* context, vec2 p1, vec2 p2, WarColor color, f32 width);
void wr_subImage(WarContext* context, SDL_Texture* texture, rect rs, rect rd, vec2 scale);

WarColor wr_getColorFromList(s32 index);
