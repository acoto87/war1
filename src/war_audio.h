#pragma once

#include <stddef.h>

#include "war_types.h"

bool waud_initAudio(WarContext* context);
void waud_removeAudiosOfType(WarContext* context, WarAudioType type);
WarEntity* waud_createAudio(WarContext* context, WarAudioId audioId, bool loop);
WarEntity* waud_createAudioWithPosition(WarContext* context, WarAudioId audioId, vec2 position, bool loop);
WarEntity* waud_createAudioRandom(WarContext* context, WarAudioId fromId, WarAudioId toId, bool loop);
WarEntity* waud_createAudioRandomWithPosition(WarContext* context, WarAudioId fromId, WarAudioId toId, vec2 position, bool loop);
WarEntity* waud_playAttackSound(WarContext* context, vec2 position, WarUnitActionStepType soundStep);
WarEntity* waud_playDudeSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* waud_playBuildingSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* waud_playAcknowledgementSound(WarContext* context, WarPlayerInfo* player);
u8* waud_transcodeXmiToMid(WarContext* context, u8* xmiData, size_t xmiLength, size_t* midLength);
u8* waud_changeSampleRate(WarContext* context, u8* samplesIn, s32 length, s32 factor);
