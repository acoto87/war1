#pragma once

#include <stddef.h>

#include "war_types.h"

bool wa_initAudio(WarContext* context);
void wa_removeAudiosOfType(WarContext* context, WarAudioType type);
WarEntity* wa_createAudio(WarContext* context, WarAudioId audioId, bool loop);
WarEntity* wa_createAudioWithPosition(WarContext* context, WarAudioId audioId, vec2 position, bool loop);
WarEntity* wa_createAudioRandom(WarContext* context, WarAudioId fromId, WarAudioId toId, bool loop);
WarEntity* wa_createAudioRandomWithPosition(WarContext* context, WarAudioId fromId, WarAudioId toId, vec2 position, bool loop);
WarEntity* wa_playAttackSound(WarContext* context, vec2 position, WarUnitActionStepType soundStep);
WarEntity* wa_playDudeSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* wa_playBuildingSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* wa_playAcknowledgementSound(WarContext* context, WarPlayerInfo* player);
u8* wa_transcodeXmiToMid(WarContext* context, u8* xmiData, size_t xmiLength, size_t* midLength);
u8* wa_changeSampleRate(WarContext* context, u8* samplesIn, s32 length, s32 factor);
