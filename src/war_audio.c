#include <assert.h>

#include "SDL3/SDL.h"
#include "TracyC.h"

#include "war_audio.h"
#include "war_entities.h"
#include "war_log.h"
#include "war_map.h"
#include "war_math.h"
#include "war_units.h"

typedef struct
{
    WarAudioId id;
    WarAudioType type;
} WarAudioData;

WarAudioData audioData[] =
{
    { WAR_MUSIC_00,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_01,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_02,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_03,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_04,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_05,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_06,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_07,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_08,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_09,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_10,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_11,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_12,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_13,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_14,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_15,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_16,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_17,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_18,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_19,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_20,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_21,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_22,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_23,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_24,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_25,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_26,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_27,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_28,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_29,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_30,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_31,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_32,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_33,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_34,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_35,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_36,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_37,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_38,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_39,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_40,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_41,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_42,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_43,                     WAR_AUDIO_MIDI },
    { WAR_MUSIC_44,                     WAR_AUDIO_MIDI },
    { WAR_LOGO,                         WAR_AUDIO_WAVE },
    { WAR_INTRO_DOOR,                   WAR_AUDIO_WAVE },
    { WAR_BUILDING,                     WAR_AUDIO_WAVE },
    { WAR_EXPLOSION,                    WAR_AUDIO_WAVE },
    { WAR_CATAPULT_ROCK_FIRED,          WAR_AUDIO_WAVE },
    { WAR_TREE_CHOPPING_1,              WAR_AUDIO_WAVE },
    { WAR_TREE_CHOPPING_2,              WAR_AUDIO_WAVE },
    { WAR_TREE_CHOPPING_3,              WAR_AUDIO_WAVE },
    { WAR_TREE_CHOPPING_4,              WAR_AUDIO_WAVE },
    { WAR_BUILDING_COLLAPSE_1,          WAR_AUDIO_WAVE },
    { WAR_BUILDING_COLLAPSE_2,          WAR_AUDIO_WAVE },
    { WAR_BUILDING_COLLAPSE_3,          WAR_AUDIO_WAVE },
    { WAR_UI_CHIME,                     WAR_AUDIO_WAVE },
    { WAR_UI_CLICK,                     WAR_AUDIO_WAVE },
    { WAR_UI_CANCEL,                    WAR_AUDIO_WAVE },
    { WAR_SWORD_ATTACK_1,               WAR_AUDIO_WAVE },
    { WAR_SWORD_ATTACK_2,               WAR_AUDIO_WAVE },
    { WAR_SWORD_ATTACK_3,               WAR_AUDIO_WAVE },
    { WAR_FIST_ATTACK,                  WAR_AUDIO_WAVE },
    { WAR_CATAPULT_FIRE_EXPLOSION,      WAR_AUDIO_WAVE },
    { WAR_FIREBALL,                     WAR_AUDIO_WAVE },
    { WAR_ARROW_SPEAR,                  WAR_AUDIO_WAVE },
    { WAR_ARROW_SPEAR_HIT,              WAR_AUDIO_WAVE },
    { WAR_ORC_HELP_1,                   WAR_AUDIO_WAVE },
    { WAR_ORC_HELP_2,                   WAR_AUDIO_WAVE },
    { WAR_HUMAN_HELP_2,                 WAR_AUDIO_WAVE },
    { WAR_HUMAN_HELP_1,                 WAR_AUDIO_WAVE },
    { WAR_ORC_DEAD,                     WAR_AUDIO_WAVE },
    { WAR_HUMAN_DEAD,                   WAR_AUDIO_WAVE },
    { WAR_ORC_WORK_COMPLETE,            WAR_AUDIO_WAVE },
    { WAR_HUMAN_WORK_COMPLETE,          WAR_AUDIO_WAVE },
    { WAR_ORC_HELP_3,                   WAR_AUDIO_WAVE },
    { WAR_ORC_HELP_4,                   WAR_AUDIO_WAVE },
    { WAR_HUMAN_HELP_3,                 WAR_AUDIO_WAVE },
    { WAR_HUMAN_HELP_4,                 WAR_AUDIO_WAVE },
    { WAR_ORC_READY,                    WAR_AUDIO_WAVE },
    { WAR_HUMAN_READY,                  WAR_AUDIO_WAVE },
    { WAR_ORC_ACKNOWLEDGEMENT_1,        WAR_AUDIO_WAVE },
    { WAR_ORC_ACKNOWLEDGEMENT_2,        WAR_AUDIO_WAVE },
    { WAR_ORC_ACKNOWLEDGEMENT_3,        WAR_AUDIO_WAVE },
    { WAR_ORC_ACKNOWLEDGEMENT_4,        WAR_AUDIO_WAVE },
    { WAR_HUMAN_ACKNOWLEDGEMENT_1,      WAR_AUDIO_WAVE },
    { WAR_HUMAN_ACKNOWLEDGEMENT_2,      WAR_AUDIO_WAVE },
    { WAR_ORC_SELECTED_1,               WAR_AUDIO_WAVE },
    { WAR_ORC_SELECTED_2,               WAR_AUDIO_WAVE },
    { WAR_ORC_SELECTED_3,               WAR_AUDIO_WAVE },
    { WAR_ORC_SELECTED_4,               WAR_AUDIO_WAVE },
    { WAR_ORC_SELECTED_5,               WAR_AUDIO_WAVE },
    { WAR_HUMAN_SELECTED_1,             WAR_AUDIO_WAVE },
    { WAR_HUMAN_SELECTED_2,             WAR_AUDIO_WAVE },
    { WAR_HUMAN_SELECTED_3,             WAR_AUDIO_WAVE },
    { WAR_HUMAN_SELECTED_4,             WAR_AUDIO_WAVE },
    { WAR_HUMAN_SELECTED_5,             WAR_AUDIO_WAVE },
    { WAR_ORC_ANNOYED_1,                WAR_AUDIO_WAVE },
    { WAR_ORC_ANNOYED_2,                WAR_AUDIO_WAVE },
    { WAR_ORC_ANNOYED_3,                WAR_AUDIO_WAVE },
    { WAR_HUMAN_ANNOYED_1,              WAR_AUDIO_WAVE },
    { WAR_HUMAN_ANNOYED_2,              WAR_AUDIO_WAVE },
    { WAR_HUMAN_ANNOYED_3,              WAR_AUDIO_WAVE },
    { WAR_DEAD_SPIDER_SCORPION,         WAR_AUDIO_WAVE },
    { WAR_NORMAL_SPELL,                 WAR_AUDIO_WAVE },
    { WAR_BUILD_ROAD,                   WAR_AUDIO_WAVE },
    { WAR_ORC_TEMPLE,                   WAR_AUDIO_WAVE },
    { WAR_HUMAN_CHURCH,                 WAR_AUDIO_WAVE },
    { WAR_ORC_KENNEL,                   WAR_AUDIO_WAVE },
    { WAR_HUMAN_STABLE,                 WAR_AUDIO_WAVE },
    { WAR_BLACKSMITH,                   WAR_AUDIO_WAVE },
    { WAR_FIRE_CRACKLING,               WAR_AUDIO_WAVE },
    { WAR_CANNON,                       WAR_AUDIO_WAVE },
    { WAR_CANNON2,                      WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_ENDING_1,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_ENDING_2,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_ENDING_1,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_ENDING_2,       WAR_AUDIO_WAVE },
    { WAR_INTRO_1,                      WAR_AUDIO_WAVE },
    { WAR_INTRO_2,                      WAR_AUDIO_WAVE },
    { WAR_INTRO_3,                      WAR_AUDIO_WAVE },
    { WAR_INTRO_4,                      WAR_AUDIO_WAVE },
    { WAR_INTRO_5,                      WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_01_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_02_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_03_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_04_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_05_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_06_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_07_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_08_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_09_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_10_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_11_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_HUMAN_12_INTRO,     WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_01_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_02_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_03_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_04_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_05_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_06_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_07_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_08_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_09_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_10_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_11_INTRO,       WAR_AUDIO_WAVE },
    { WAR_CAMPAIGNS_ORC_12_INTRO,       WAR_AUDIO_WAVE },
    { WAR_HUMAN_DEFEAT,                 WAR_AUDIO_WAVE },
    { WAR_ORC_DEFEAT,                   WAR_AUDIO_WAVE },
    { WAR_ORC_VICTORY_1,                WAR_AUDIO_WAVE },
    { WAR_ORC_VICTORY_2,                WAR_AUDIO_WAVE },
    { WAR_ORC_VICTORY_3,                WAR_AUDIO_WAVE },
    { WAR_HUMAN_VICTORY_1,              WAR_AUDIO_WAVE },
    { WAR_HUMAN_VICTORY_2,              WAR_AUDIO_WAVE },
    { WAR_HUMAN_VICTORY_3,              WAR_AUDIO_WAVE },
};

WarAudioData wa_getAudioData(WarAudioId audioId)
{
    s32 index = 0;
    s32 length = arrayLength(audioData);
    while (index < length && audioData[index].id != audioId)
        index++;

    assert(index < length);
    return audioData[index];
}

bool wa_playMidi(WarContext* context, WarEntity* entity, u32 sampleCount, s16* outputStream, f32 volume)
{
    WarAudioComponent* audio = we_getAudioComponent(context, entity);
    assert(audio);

    tsf* soundFont = context->soundFont;

    while (sampleCount)
    {
        //We progress the MIDI playback and then process TSF_RENDER_EFFECTSAMPLEBLOCK samples at once
        u32 sampleBlock = MIN(TSF_RENDER_EFFECTSAMPLEBLOCK, sampleCount);
        audio->playbackTime += sampleBlock * (1000.0f / PLAYBACK_FREQ);

        //Loop through all MIDI messages which need to be played up until the current playback time
        tml_message* midiMessage = audio->currentMessage;
        tml_message* prevMessage = midiMessage;
        while (midiMessage)
        {
            // end of track
            if (midiMessage->type == TML_EOT)
            {
                midiMessage = audio->loop ? audio->firstMessage : NULL;
                audio->playbackTime -= prevMessage->time;
            }

            if (!midiMessage || midiMessage->time > audio->playbackTime)
            {
                break;
            }

            u8 channel = midiMessage->channel;

            switch (midiMessage->type)
            {
                // channel program (preset) change (special handling for 10th MIDI channel with drums)
                case TML_PROGRAM_CHANGE:
                {
                    s8 program = midiMessage->program;
                    tsf_channel_set_presetnumber(soundFont, channel, program, (channel == 9));
                    break;
                }

                // play a note
                case TML_NOTE_ON:
                {
                    s8 key = midiMessage->key;
                    f32 velocity = midiMessage->velocity / 127.0f;
                    tsf_channel_note_on(soundFont, channel, key, velocity * volume);
                    break;
                }

                // wcmd_stop a note
                case TML_NOTE_OFF:
                {
                    s8 key = midiMessage->key;
                    tsf_channel_note_off(soundFont, channel, key);
                    break;
                }

                // pitch wheel modification
                case TML_PITCH_BEND:
                {
                    u16 pitchBend = midiMessage->pitch_bend;
                    tsf_channel_set_pitchwheel(soundFont, channel, pitchBend);
                    break;
                }

                // MIDI controller messages
                case TML_CONTROL_CHANGE:
                {
                    s8 control = midiMessage->control;
                    s8 controlValue = midiMessage->control_value;
                    tsf_channel_midi_control(soundFont, channel, control, controlValue);
                    break;
                }
            }

            prevMessage = midiMessage;
            midiMessage = midiMessage->next;
        }

        // Render the block of audio samples in float format
        tsf_render_short(soundFont, outputStream, sampleBlock, TSF_TRUE);

        sampleCount -= sampleBlock;
        outputStream += sampleBlock;

        audio->currentMessage = midiMessage;
    }

    return !audio->currentMessage && !audio->loop;
}

bool wa_playWave(WarContext* context, WarEntity* entity, u32 sampleCount, s16* outputStream, f32 volume)
{
    WarAudioComponent* audio = we_getAudioComponent(context, entity);
    assert(audio);

    WarResource* resource = &context->resources[audio->resourceIndex];
    if (resource->type != WAR_RESOURCE_TYPE_XMID &&
        resource->type != WAR_RESOURCE_TYPE_WAVE &&
        resource->type != WAR_RESOURCE_TYPE_VOC)
    {
        logError("Can't play audio %d, resource: %d", entity->id, audio->resourceIndex);
        return false;
    }

    s32 waveLength = resource->audio.length;

    do
    {
        if (audio->sampleIndex >= waveLength)
        {
            audio->sampleIndex = 0;
            audio->playbackTime = 0;
        }

        u8* waveData = &resource->audio.data[audio->sampleIndex];

        u32 sampleBlock = MIN(sampleCount, (u32)(waveLength - audio->sampleIndex));
        for (u32 i = 0; i < sampleBlock; i++)
        {
            s32 value = *waveData;
            value = value - 128;
            value = value << 8;
            value = (s32)(value * volume);
            value += *outputStream;
            value = CLAMP(value, INT16_MIN, INT16_MAX);
            *outputStream = (s16)value;

            waveData++;
            outputStream++;
        }

        audio->playbackTime += sampleBlock * (1000.0f / PLAYBACK_FREQ);
        audio->sampleIndex += sampleBlock;

        sampleCount -= sampleBlock;
    } while (sampleCount > 0 && audio->loop);

    return audio->sampleIndex >= waveLength && !audio->loop;
}

void SDLCALL audioDataCallback(void* userdata, SDL_AudioStream* stream, int additionalAmount, int totalAmount)
{
    NOT_USED(totalAmount);

    WarContext* context = (WarContext*)userdata;
    if (!context || additionalAmount <= 0)
    {
        return;
    }

    if (!context->audioEnabled || context->realTime < context->transitionEndRealTime)
    {
        tsf_note_off_all(context->soundFont, TSF_TRUE);
        return;
    }

    // additionalAmount is in bytes, format is S16 mono (2 bytes per sample)
    u32 sampleCount = (u32)(additionalAmount / sizeof(s16));
    if (sampleCount == 0)
    {
        return;
    }

    // Use the pre-allocated mix buffer (owned by WarContext, never freed here).
    // Cap to the allocated capacity; if SDL requests more than expected, we mix
    // what we can and the stream will request the remainder on the next callback.
    if (sampleCount > context->audioMixBufferCapacity)
    {
        logWarning("Audio callback requested %u samples; capping to pre-allocated capacity %u.",
                   sampleCount, context->audioMixBufferCapacity);
        sampleCount = context->audioMixBufferCapacity;
    }

    s16* outputBuffer = context->audioMixBuffer;
    memset(outputBuffer, 0, sampleCount * sizeof(s16));

    f32 musicVolume = context->musicEnabled ? context->musicVolume : 0;
    f32 soundVolume = context->soundEnabled ? context->soundVolume : 0;

    // Fixed-size stack array: no heap allocation from the audio thread.
    WarEntityId toRemove[AUDIO_REMOVE_PENDING_MAX];
    s32 toRemoveCount = 0;

    s16* outputStream = outputBuffer;

    WarEntityList* audios = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_AUDIO);
    for (s32 i = 0; i < audios->count; i++)
    {
        WarEntity* entity = audios->items[i];
        if (entity)
        {
            if (we_isComponentEnabled(context, entity, COMP_AUDIO))
            {
                WarAudioComponent* audio = we_getAudioComponent(context, entity);
                assert(audio);

                switch (audio->type)
                {
                    case WAR_AUDIO_MIDI:
                    {
                        if (musicVolume > 0 && wa_playMidi(context, entity, sampleCount, outputStream, musicVolume))
                        {
                            // if the audio finish, mark it to remove it
                            if (toRemoveCount < AUDIO_REMOVE_PENDING_MAX)
                                toRemove[toRemoveCount++] = entity->id;
                        }

                        break;
                    }

                    case WAR_AUDIO_WAVE:
                    {
                        f32 volume = soundVolume;

                        if (we_isComponentEnabled(context, entity, COMP_TRANSFORM))
                        {
                            WarTransformComponent* transform = we_getTransformComponent(context, entity);
                            assert(transform);

                            // does positional audios only makes sense on maps?
                            WarMap* map = context->map;
                            assert(map);

                            // it's a positional audio, so check if the audio is inside the viewport
                            vec2 position = transform->position;

                            rect viewport = map->camera.viewport;
                            if (!rect_containsf(viewport, position.x, position.y))
                            {
                                vec2 viewportCenter = rect_center(viewport);
                                f32 distance = vec2_distance(position, viewportCenter);

                                // approximately the distances from the center of the viewport
                                // begin at 120 pixels, I'm going to set an audible range of 120-200
                                // when the audios gets out of viewport
                                // distance             vol
                                // < 120            ->  1
                                // > 120 && < 200   ->  interpolate range 120-200 to range 0.75-0
                                // >= 200           ->  0
                                volume = 0.75f * (1 - ((distance - 120) / 80));
                                volume = MAX(volume, 0);
                            }
                        }

                        if (wa_playWave(context, entity, sampleCount, outputStream, volume))
                        {
                            // if the audio finish, mark it to remove it
                            if (toRemoveCount < AUDIO_REMOVE_PENDING_MAX)
                                toRemove[toRemoveCount++] = entity->id;
                        }

                        break;
                    }

                    default:
                    {
                        logWarning("Unkown audio type: %d", audio->type);
                        break;
                    }
                }
            }
        }
    }

    // Post finished entity IDs to the main thread for removal.
    // Never call we_removeEntityById from the audio thread: it calls wm_free which
    // would race with main-thread allocations on globalZone.
    if (toRemoveCount > 0)
    {
        SDL_LockMutex(context->audioRemoveMutex);
        for (s32 i = 0; i < toRemoveCount; i++)
        {
            if (context->audioRemovePendingCount < AUDIO_REMOVE_PENDING_MAX)
                context->audioRemovePending[context->audioRemovePendingCount++] = toRemove[i];
        }
        SDL_UnlockMutex(context->audioRemoveMutex);
    }

    // push the mixed audio data into the stream
    SDL_PutAudioStreamData(stream, outputBuffer, (int)(sampleCount * sizeof(s16)));
}

bool wa_initAudio(WarContext* context)
{
    context->soundFont = tsf_load_filename("GMGeneric.SF2");
    if (!context->soundFont)
    {
        logError("Could not load SoundFont at %s", "GMGeneric.SF2");
        return false;
    }

    //Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
    tsf_channel_set_bank_preset(context->soundFont, 9, 128, 0);

    // Set the SoundFont rendering output mode
    tsf_set_output(context->soundFont, TSF_MONO, PLAYBACK_FREQ, 0.0f);

    // Pre-allocate the mix buffer on the main thread before audio starts.
    // This ensures the audio callback thread never needs to call wm_calloc
    // or wm_free, which would race with main-thread allocations on globalZone.
    context->audioMixBuffer = (s16*)wm_alloc(AUDIO_MIX_BUFFER_MAX_SAMPLES * sizeof(s16));
    if (!context->audioMixBuffer)
    {
        logError("Failed to allocate audio mix buffer.");
        tsf_close(context->soundFont);
        context->soundFont = NULL;
        return false;
    }
    context->audioMixBufferCapacity = AUDIO_MIX_BUFFER_MAX_SAMPLES;

    // Mutex that guards audioRemovePending / audioRemovePendingCount.
    context->audioRemoveMutex = SDL_CreateMutex();
    if (!context->audioRemoveMutex)
    {
        logError("Failed to create audio remove mutex: %s", SDL_GetError());
        wm_free(context->audioMixBuffer);
        context->audioMixBuffer = NULL;
        tsf_close(context->soundFont);
        context->soundFont = NULL;
        return false;
    }
    context->audioRemovePendingCount = 0;

    // Open an SDL3 audio device stream with S16 mono at PLAYBACK_FREQ
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;
    spec.freq = PLAYBACK_FREQ;

    context->audioStream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &spec,
        audioDataCallback,
        context
    );

    if (!context->audioStream)
    {
        logError("Failed to open audio device stream: %s", SDL_GetError());
        return false;
    }

    // SDL_OpenAudioDeviceStream opens paused; resume to start playback
    if (!SDL_ResumeAudioStreamDevice(context->audioStream))
    {
        logError("Failed to resume audio stream device: %s", SDL_GetError());
        SDL_DestroyAudioStream(context->audioStream);
        context->audioStream = NULL;
        return false;
    }

    context->musicEnabled = true;
    context->soundEnabled = true;
    context->musicVolume = 1.0f;
    context->soundVolume = 1.0f;

    return true;
}

void wa_removeAudiosOfType(WarContext* context, WarAudioType type)
{
    WarEntityIdList toRemove;
    WarEntityIdListInit(&toRemove, wm_frameAllocator());

    WarEntityList* audios = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_AUDIO);
    for (s32 i = 0; i < audios->count; i++)
    {
        WarEntity* entity = audios->items[i];
        if (entity)
        {
            WarAudioComponent* audio = we_getAudioComponent(context, entity);
            assert(audio);

            if (audio->type == type)
            {
                WarEntityIdListAdd(&toRemove, entity->id);
            }
        }
    }

    for (s32 i = 0; i < toRemove.count; i++)
    {
        WarEntityId entityId = toRemove.items[i];
        we_removeEntityById(context, entityId);
    }

    WarEntityIdListFree(&toRemove);

    if (type == WAR_AUDIO_MIDI)
    {
        // at this point the audio callback shouldn't be playing any midi audio
        // so turn of notes off
        tsf_note_off_all(context->soundFont, TSF_TRUE);
    }
}

WarEntity* wa_createAudio(WarContext* context, const CreateAudioArgs* args)
{
    WarAudioId audioId = args->audioId;
    bool loop = args->loop;
    WarAudioData data = wa_getAudioData(audioId);

    s32 resourceIndex = (s32)audioId;
    WarAudioType type = data.type;

    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_AUDIO, true);
    we_addAudioComponent(context, entity, WAR_AUDIO_COMPONENT_INIT(
        .type          = type,
        .resourceIndex = resourceIndex,
        .loop          = loop,
    ));

    return entity;
}

WarEntity* wa_createAudioWithPosition(WarContext* context, const CreateAudioArgs* args)
{
    WarAudioId audioId = args->audioId;
    vec2 position = args->position;
    bool loop = args->loop;

    WarMap* map = context->map;
    if (map && map->fowEnabled)
    {
        if (!wmap_isPositionVisible(map, position))
        {
            return NULL;
        }
    }

    WarAudioData data = wa_getAudioData(audioId);

    s32 resourceIndex = (s32)audioId;
    WarAudioType type = data.type;

    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_AUDIO, true);
    we_addAudioComponent(context, entity, WAR_AUDIO_COMPONENT_INIT(
        .type          = type,
        .resourceIndex = resourceIndex,
        .loop          = loop,
    ));
    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = position,
    ));

    return entity;
}

WarEntity* wa_createAudioRandom(WarContext* context, const CreateAudioArgs* args)
{
    WarAudioId fromId = args->randomFromId;
    WarAudioId toId = args->randomToId;
    bool loop = args->loop;
    return wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=randomi(fromId, toId + 1), .loop=loop));
}

WarEntity* wa_createAudioRandomWithPosition(WarContext* context, const CreateAudioArgs* args)
{
    WarAudioId fromId = args->randomFromId;
    WarAudioId toId = args->randomToId;
    vec2 position = args->position;
    bool loop = args->loop;
    return wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=randomi(fromId, toId + 1), .position=position, .hasPosition=true, .loop=loop));
}

WarEntity* wa_playAttackSound(WarContext* context, vec2 position, WarUnitActionStepType soundStep)
{
    switch (soundStep)
    {
        case WAR_ACTION_STEP_SOUND_SWORD:
            return wa_createAudioRandomWithPosition(context, CREATE_AUDIO_ARGS_INIT(.randomFromId=WAR_SWORD_ATTACK_1, .randomToId=WAR_SWORD_ATTACK_3, .position=position, .hasPosition=true, .loop=false));
        case WAR_ACTION_STEP_SOUND_FIST:
            return wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_FIST_ATTACK, .position=position, .hasPosition=true, .loop=false));
        case WAR_ACTION_STEP_SOUND_FIREBALL:
            return wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_FIREBALL, .position=position, .hasPosition=true, .loop=false));
        case WAR_ACTION_STEP_SOUND_CATAPULT:
            return wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_CATAPULT_ROCK_FIRED, .position=position, .hasPosition=true, .loop=false));
        case WAR_ACTION_STEP_SOUND_ARROW:
            return wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_ARROW_SPEAR, .position=position, .hasPosition=true, .loop=false));
        default:
            logWarning("Trying to play sound with step: %d", soundStep);
            return NULL;
    }
}

WarEntity* wa_playDudeSelectionSound(WarContext* context, WarEntity* entity)
{
    assert(wu_isDudeUnit(context, entity));

    WarMap* map = context->map;

    if (map->selectedEntities.count == 1)
    {
        WarEntityId selectedEntityId = map->selectedEntities.items[0];
        if (selectedEntityId == entity->id)
        {
            return wu_isHumanUnit(context, entity)
                ? wa_createAudioRandom(context, CREATE_AUDIO_ARGS_INIT(.randomFromId=WAR_HUMAN_ANNOYED_1, .randomToId=WAR_HUMAN_ANNOYED_3, .loop=false))
                : wa_createAudioRandom(context, CREATE_AUDIO_ARGS_INIT(.randomFromId=WAR_ORC_ANNOYED_1, .randomToId=WAR_ORC_ANNOYED_3, .loop=false));
        }
    }

    return wu_isHumanUnit(context, entity)
        ? wa_createAudioRandom(context, CREATE_AUDIO_ARGS_INIT(.randomFromId=WAR_HUMAN_SELECTED_1, .randomToId=WAR_HUMAN_SELECTED_5, .loop=false))
        : wa_createAudioRandom(context, CREATE_AUDIO_ARGS_INIT(.randomFromId=WAR_ORC_SELECTED_1, .randomToId=WAR_ORC_SELECTED_5, .loop=false));
}

WarEntity* wa_playBuildingSelectionSound(WarContext* context, WarEntity* entity)
{
    assert(wu_isBuildingUnit(context, entity));

    if (wst_isBuilding(context, entity) || wst_isGoingToBuild(context, entity))
        return wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_BUILDING, .loop=false));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    s32 hpPercent = PERCENTABI(unit->hp, unit->maxhp);
    if(hpPercent <= 33)
        return wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_FIRE_CRACKLING, .loop=false));

    switch (unit->type)
    {
        case WAR_UNIT_CHURCH:
            return wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_HUMAN_CHURCH, .loop=false));
        case WAR_UNIT_TEMPLE:
            return wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_ORC_TEMPLE, .loop=false));
        case WAR_UNIT_STABLE:
            return wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_HUMAN_STABLE, .loop=false));
        case WAR_UNIT_KENNEL:
            return wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_ORC_KENNEL, .loop=false));
        case WAR_UNIT_BLACKSMITH_HUMANS:
        case WAR_UNIT_BLACKSMITH_ORCS:
            return wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_BLACKSMITH, .loop=false));
        default:
            return wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_UI_CLICK, .loop=false));
    }
}

WarEntity* wa_playAcknowledgementSound(WarContext* context, WarPlayerInfo* player)
{
    return isHumanPlayer(player)
        ? wa_createAudioRandom(context, CREATE_AUDIO_ARGS_INIT(.randomFromId=WAR_HUMAN_ACKNOWLEDGEMENT_1, .randomToId=WAR_HUMAN_ACKNOWLEDGEMENT_2, .loop=false))
        : wa_createAudioRandom(context, CREATE_AUDIO_ARGS_INIT(.randomFromId=WAR_ORC_ACKNOWLEDGEMENT_1, .randomToId=WAR_ORC_ACKNOWLEDGEMENT_4, .loop=false));
}
