#pragma once

#include "SDL3/SDL_events.h"
#include "shl/memzone.h"

#include "common.h"
#include "war_types.h"

bool initGame(WarContext* context);
void quitGame(WarContext* context);
bool loadDataFile(WarContext* context);
void setWindowSize(WarContext* context, s32 width, s32 height);
void setGlobalScale(WarContext* context, f32 scale);
void changeGlobalScale(WarContext* context, f32 deltaScale);
void setGlobalSpeed(WarContext* context, f32 speed);
void changeGlobalSpeed(WarContext* context, f32 deltaSpeed);
void setMusicVolume(WarContext* context, f32 volume);
void changeMusicVolume(WarContext* context, f32 deltaVolume);
void setSoundVolume(WarContext* context, f32 volume);
void changeSoundVolume(WarContext* context, f32 deltaVolume);
void setNextScene(WarContext* context, WarScene* scene, f32 transitionDelay);
void setNextMap(WarContext* context, WarMap* map, f32 transitionDelay);
void setInputButton(WarContext* context, s32 button, bool pressed);
void setInputKey(WarContext* context, s32 key, bool pressed);
void beginInputFrame(WarContext* context);
void processGameEvent(WarContext* context, SDL_Event* event);
void updateGame(WarContext* context);
void renderGame(WarContext *context);
void presentGame(WarContext *context);
