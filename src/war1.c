#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef __GNUC__
// this is to silence GCC about this warning since nanovg is full of them
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>

#if __DEBUG__
#include <execinfo.h>
#endif

// #define NDEBUG // define this to deactivate assertions
#include <assert.h>

// Guide to predefined macros in C compilers gcc, clang, msvc, etc.
// https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html
#include <glad/glad.h>

#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "nanovg/nanovg.c"
#define NVG_DISABLE_CULL_FACE
#define NANOVG_GLES2_IMPLEMENTATION
#include "nanovg/nanovg_gl.h"
#include "nanovg/nanovg_gl_utils.h"

// https://github.com/schellingb/TinySoundFont
#define TSF_IMPLEMENTATION
#include "TinySoundFont/tsf.h"
#define TML_IMPLEMENTATION
#include "TinySoundFont/tml.h"

// https://github.com/dr-soft/miniaudio
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "shl/array.h"
#include "shl/list.h"
#include "shl/queue.h"
#include "shl/binary_heap.h"
#include "shl/map.h"
#define SHL_MEMORY_BUFFER_IMPLEMENTATION
#include "shl/memory_buffer.h"
#define SHL_WAVE_WRITER_IMPLEMENTATION
#include "shl/wave_writer.h"

#include "log.h"
#include "utils.h"
#include "war_math.h"
#include "io.h"
#include "glutils.h"
#include "war_types.h"
#include "war.h"
#include "war_database.h"
#include "war_map.h"
#include "war_commands.h"
#include "war_units.h"
#include "war_entities.h"
#include "war_pathfinder.h"
#include "war_state_machine.h"
#include "war_file.c"
#include "war_audio.c"
#include "war_actions.c"
#include "war_render.c"
#include "war_resources.c"
#include "war_sprites.c"
#include "war_font.c"
#include "war_animations.c"
#include "war_roads.c"
#include "war_walls.c"
#include "war_ruins.c"
#include "war_trees.c"
#include "war_ui.c"
#include "war_commands.c"
#include "war_units.c"
#include "war_entities.c"
#include "war_pathfinder.c"
#include "war_state_machine_idle.c"
#include "war_state_machine_move.c"
#include "war_state_machine_follow.c"
#include "war_state_machine_patrol.c"
#include "war_state_machine_attack.c"
#include "war_state_machine_death.c"
#include "war_state_machine_collapse.c"
#include "war_state_machine_wait.c"
#include "war_state_machine_gather_gold.c"
#include "war_state_machine_mining.c"
#include "war_state_machine_gather_wood.c"
#include "war_state_machine_chopping.c"
#include "war_state_machine_deliver.c"
#include "war_state_machine_train.c"
#include "war_state_machine_upgrade.c"
#include "war_state_machine_build.c"
#include "war_state_machine_repair.c"
#include "war_state_machine_repairing.c"
#include "war_state_machine.c"
#include "war_map.c"
#include "war_game.c"

// Holds the global instance pointer
static tsf* g_TinySoundFont;

// Holds global MIDI playback state
static double g_Msec;               //current playback time
static tml_message* g_MidiMessage;  //next message to be played

void glfwErrorCallback(int error, const char* description)
{
    logError("Error: %d, %s\n", error, description);
}

void data_callback(ma_device* pDevice, void* output, const void* input, ma_uint32 frameCount)
{
    NOT_USED(input);

    WarContext* context = (WarContext*)pDevice->pUserData;
    if (!context) 
    {
        return;
    }

    s16* stream = (s16*)output;

    // s32 samplesPerSecond = 44100;
    // s32 toneHz = 256;
    // s32 wavePeriod = samplesPerSecond / toneHz;
    // s32 sampleSize = sizeof(s16);
    // s32 toneVolume = 32767;
    // #define PI 3.14159265359f

    // static u32 wavePos = 0;
    // s16 *data = (s16*)output;
    // for (s32 i = 0; i < frameCount ; i++)
    // {
    //     f32 t = 2.0f * PI * (f32)wavePos / (f32)wavePeriod;
    //     f32 sineValue = sinf(t);
    //     s16 sampleValue = (s16)(sineValue * toneVolume);
    //     data[i] = sampleValue;
    //     ++wavePos;
    // }

    u8* data = context->resources[501]->wave.data;
    s32 dataLength = context->resources[501]->wave.length;
    
    // printf("frameCount: %d\n", frameCount);
    // printf("pos: %d\n", context->resources[501]->wave.pos);

    for (s32 i = 0; i < frameCount; i++)
    {
        if (context->resources[501]->wave.pos >= dataLength)
        {
            context->resources[501]->wave.pos = 0;
        }

        stream[i] = (s16)(data[context->resources[501]->wave.pos++] - 0x80) << 8;
    }

	// s32 sampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK;

    // while (frameCount)
    // {
    //     //We progress the MIDI playback and then process TSF_RENDER_EFFECTSAMPLEBLOCK samples at once
	// 	if (sampleBlock > frameCount)
    //     {
    //         sampleBlock = frameCount;
    //     }

    //     g_Msec += sampleBlock * (1000.0 / 44100.0);

    //     //Loop through all MIDI messages which need to be played up until the current playback time
    //     while (g_MidiMessage && g_MidiMessage->time <= g_Msec)
    //     {
    //         switch (g_MidiMessage->type)
	// 		{
	// 			case TML_PROGRAM_CHANGE: //channel program (preset) change (special handling for 10th MIDI channel with drums)
	// 				tsf_channel_set_presetnumber(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->program, (g_MidiMessage->channel == 9));
	// 				break;
	// 			case TML_NOTE_ON: //play a note
	// 				tsf_channel_note_on(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key, g_MidiMessage->velocity / 127.0f);
	// 				break;
	// 			case TML_NOTE_OFF: //stop a note
	// 				tsf_channel_note_off(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key);
	// 				break;
	// 			case TML_PITCH_BEND: //pitch wheel modification
	// 				tsf_channel_set_pitchwheel(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->pitch_bend);
	// 				break;
	// 			case TML_CONTROL_CHANGE: //MIDI controller messages
	// 				tsf_channel_midi_control(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->control, g_MidiMessage->control_value);
	// 				break;
	// 		}

    //         g_MidiMessage = g_MidiMessage->next;
    //     }

	// 	// Render the block of audio samples in float format
	// 	tsf_render_short(g_TinySoundFont, stream, sampleBlock, 0);

    //     frameCount -= sampleBlock;
    //     stream += sampleBlock;
    // }
}

int main() 
{
    srand(time(NULL));

    initLog(LOG_SEVERITY_DEBUG);

    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
    {
        logError("Error initializing GLFW!\n");
        return -1;
    }

    WarContext context = {0};
    if (!initGame(&context))
    {
        logError("Can't initialize the game!\n");
        return -1;
    }

    u8* midiData = context.resources[1]->xmi.data;
    size32 midiLength = context.resources[1]->xmi.length;

    g_MidiMessage = tml_load_memory(midiData, midiLength);
	if (!g_MidiMessage)
	{
		logError("Could not load MIDI file\n");
		return 1;
	}

    g_TinySoundFont = tsf_load_filename("GMGeneric.SF2");
	if (!g_TinySoundFont)
	{
		logError("Could not load SoundFont\n");
		return 1;
	}

    //Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
	tsf_channel_set_bank_preset(g_TinySoundFont, 9, 128, 0);

	// Set the SoundFont rendering output mode
	tsf_set_output(g_TinySoundFont, TSF_MONO, 44100, 0.0f);

    ma_device_config deviceConfig;
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_s16;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate = 11025;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &context;

    ma_device device;
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        logError("Failed to open playback device.\n");
        return -1;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        logError("Failed to start playback device.\n");
        ma_device_uninit(&device);
        return -1;
    }

    while (!glfwWindowShouldClose(context.window))
    {
        sprintf(context.windowTitle, "War 1: %.2fs at %d fps (%.4fs)", context.time, context.fps, context.deltaTime);
        glfwSetWindowTitle(context.window, context.windowTitle);

        inputGame(&context);
        updateGame(&context);
        renderGame(&context);
        presentGame(&context);
    }

    ma_device_uninit(&device);

    nvgDeleteGLES2(context.gfx);
    glfwDestroyWindow(context.window);
    glfwTerminate();
	return 0;
}
