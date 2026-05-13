#pragma once

#include <stddef.h>

#include "war_math.h"
#include "war_actions.h"

typedef struct tsf tsf;
typedef struct tml_message tml_message;

struct _WarAudioComponent
{
    bool enabled;
    WarAudioType type;
    s32 resourceIndex;
    bool loop;
    f32 playbackTime;
    s32 sampleIndex;
    tml_message* firstMessage;
    tml_message* currentMessage;
};

#define WAR_AUDIO_COMPONENT_INIT_FIELDS(...) { \
    .enabled        = false,                   \
    .type           = 0,                       \
    .resourceIndex  = -1,                      \
    .loop           = false,                   \
    .playbackTime   = 0,                       \
    .sampleIndex    = 0,                       \
    .firstMessage   = NULL,                    \
    .currentMessage = NULL,                    \
    __VA_ARGS__                                \
}
#define WAR_AUDIO_COMPONENT_INIT(...) ((WarAudioComponent)WAR_AUDIO_COMPONENT_INIT_FIELDS(__VA_ARGS__))

bool wa_initAudio(WarContext* context);
void wa_removeAudiosOfType(WarContext* context, WarAudioType type);

typedef struct {
    WarAudioId audioId;
    WarAudioId randomFromId;
    WarAudioId randomToId;
    vec2 position;
    bool hasPosition;
    bool loop;
} CreateAudioArgs;

#define CREATE_AUDIO_ARGS_INIT_CONST(...) { \
    .audioId = -1, \
    .randomFromId = -1, \
    .randomToId = -1, \
    .hasPosition = false, \
    .loop = false, \
    __VA_ARGS__ \
}
#define CREATE_AUDIO_ARGS_INIT(...) (&(CreateAudioArgs)CREATE_AUDIO_ARGS_INIT_CONST(__VA_ARGS__))

WarEntity* wa_createAudio(WarContext* context, const CreateAudioArgs* args);
WarEntity* wa_createAudioWithPosition(WarContext* context, const CreateAudioArgs* args);
WarEntity* wa_createAudioRandom(WarContext* context, const CreateAudioArgs* args);
WarEntity* wa_createAudioRandomWithPosition(WarContext* context, const CreateAudioArgs* args);

WarEntity* wa_playAttackSound(WarContext* context, vec2 position, WarUnitActionStepType soundStep);
WarEntity* wa_playDudeSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* wa_playBuildingSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* wa_playAcknowledgementSound(WarContext* context, WarPlayerInfo* player);
u8* wa_transcodeXmiToMid(WarContext* context, u8* xmiData, size_t xmiLength, size_t* midLength);
u8* wa_changeSampleRate(WarContext* context, u8* samplesIn, s32 length, s32 factor);
