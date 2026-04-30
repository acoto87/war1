#pragma once

#include "SDL3/SDL_events.h"
#include "shl/memzone.h"

#include "war.h"

bool wg_initGame(WarContext* context);
void wg_quitGame(WarContext* context);
bool wg_loadDataFile(WarContext* context);
void wg_setWindowSize(WarContext* context, s32 width, s32 height);
void wg_setGlobalScale(WarContext* context, f32 scale);
void wg_changeGlobalScale(WarContext* context, f32 deltaScale);
void wg_setGlobalSpeed(WarContext* context, f32 speed);
void wg_changeGlobalSpeed(WarContext* context, f32 deltaSpeed);
void wg_setMusicVolume(WarContext* context, f32 volume);
void wg_changeMusicVolume(WarContext* context, f32 deltaVolume);
void wg_setSoundVolume(WarContext* context, f32 volume);
void wg_changeSoundVolume(WarContext* context, f32 deltaVolume);
void wg_setNextScene(WarContext* context, WarScene* scene, f32 transitionDelay);
void wg_setNextMap(WarContext* context, WarMap* map, f32 transitionDelay);
void wg_setInputButton(WarContext* context, s32 button, bool pressed);
void wg_setInputKey(WarContext* context, s32 key, bool pressed);
void wg_beginInputFrame(WarContext* context);
void wg_processGameEvent(WarContext* context, SDL_Event* event);
void wg_updateGame(WarContext* context);
void wg_renderGame(WarContext *context);
void wg_presentGame(WarContext *context);
