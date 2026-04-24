#pragma once

#include <stddef.h>

#include "war_types.h"

bool initAudio(WarContext* context);
void removeAudiosOfType(WarContext* context, WarAudioType type);
WarEntity* createAudio(WarContext* context, WarAudioId audioId, bool loop);
WarEntity* createAudioWithPosition(WarContext* context, WarAudioId audioId, vec2 position, bool loop);
WarEntity* createAudioRandom(WarContext* context, WarAudioId fromId, WarAudioId toId, bool loop);
WarEntity* createAudioRandomWithPosition(WarContext* context, WarAudioId fromId, WarAudioId toId, vec2 position, bool loop);
WarEntity* playAttackSound(WarContext* context, vec2 position, WarUnitActionStepType soundStep);
WarEntity* playDudeSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* playBuildingSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* playAcknowledgementSound(WarContext* context, WarPlayerInfo* player);
u8* transcodeXmiToMid(WarContext* context, u8* xmiData, size_t xmiLength, size_t* midLength);
u8* changeSampleRate(WarContext* context, u8* samplesIn, s32 length, s32 factor);
