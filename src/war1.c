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

#include <AL/al.h>
#include <AL/alc.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "shl/array.h"
#include "shl/list.h"
#include "shl/queue.h"
#include "shl/binary_heap.h"
#include "shl/map.h"

#include "log.h"
#include "utils.h"
// #include "war_math.h"
// #include "io.h"
// #include "glutils.h"
// #include "war_types.h"
// #include "war.h"
// #include "war_database.h"
// #include "war_map.h"
// #include "war_commands.h"
// #include "war_units.h"
// #include "war_entities.h"
// #include "war_pathfinder.h"
// #include "war_state_machine.h"
// #include "war_file.c"
// #include "war_actions.c"
// #include "war_render.c"
// #include "war_resources.c"
// #include "war_sprites.c"
// #include "war_font.c"
// #include "war_animations.c"
// #include "war_roads.c"
// #include "war_walls.c"
// #include "war_ruins.c"
// #include "war_trees.c"
// #include "war_ui.c"
// #include "war_commands.c"
// #include "war_units.c"
// #include "war_entities.c"
// #include "war_pathfinder.c"
// #include "war_state_machine_idle.c"
// #include "war_state_machine_move.c"
// #include "war_state_machine_follow.c"
// #include "war_state_machine_patrol.c"
// #include "war_state_machine_attack.c"
// #include "war_state_machine_death.c"
// #include "war_state_machine_collapse.c"
// #include "war_state_machine_wait.c"
// #include "war_state_machine_gather_gold.c"
// #include "war_state_machine_mining.c"
// #include "war_state_machine_gather_wood.c"
// #include "war_state_machine_chopping.c"
// #include "war_state_machine_deliver.c"
// #include "war_state_machine_train.c"
// #include "war_state_machine_upgrade.c"
// #include "war_state_machine_build.c"
// #include "war_state_machine_repair.c"
// #include "war_state_machine_repairing.c"
// #include "war_state_machine.c"
// #include "war_map.c"
// #include "war_game.c"

void glfwErrorCallback(int error, const char* description)
{
    logError("Error: %d, %s\n", error, description);
}

// static void list_audio_devices(const ALCchar *devices)
// {
//         const ALCchar *device = devices, *next = devices + 1;
//         size_t len = 0;

//         fprintf(stdout, "Devices list:\n");
//         fprintf(stdout, "----------\n");
//         while (device && *device != '\0' && next && *next != '\0') {
//                 fprintf(stdout, "%s\n", device);
//                 len = strlen(device);
//                 device += (len + 1);
//                 next += (len + 2);
//         }
//         fprintf(stdout, "----------\n");
// }

// static void checkOpenAlError()
// {
//     ALCenum err = alGetError();
//     switch (err) 
//     {
//         case AL_NO_ERROR:
//             printf("AL_NO_ERROR\n");
//             break;
//         case AL_INVALID_NAME:
//             printf("AL_INVALID_NAME\n");
//             break;
//         case AL_INVALID_ENUM:
//             printf("AL_INVALID_ENUM\n");
//             break;
//         case AL_INVALID_VALUE:
//             printf("AL_INVALID_VALUE\n");
//             break;
//         case AL_INVALID_OPERATION:
//             printf("AL_INVALID_OPERATION\n");
//             break;
//         case AL_OUT_OF_MEMORY:
//             printf("AL_OUT_OF_MEMORY\n");
//             break;
//         default:
//             printf("UNKOWN ERROR\n");
//             break;
//     }
// }

// void testAudioStuff()
// {
//     checkOpenAlError();

//     ALCdevice* device = alcOpenDevice(NULL);
//     if (!device)
//     {
//         logError("Error initializing OpenAL!\n");
//         return;
//     }

//     checkOpenAlError();

//     ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
//     if (enumeration == AL_FALSE)
//         printf("enumeration not supported\n");
//     else
//         printf("enumeration supported\n");

//     checkOpenAlError();

//     list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
//     checkOpenAlError();

//     ALCcontext* context = alcCreateContext(device, NULL);
//     if (!alcMakeContextCurrent(context))
//     {
//         printf("failed to make context current\n");
//         return;
//     }

//     checkOpenAlError();

//     ALuint source;
//     alGenSources((ALuint)1, &source);
//     checkOpenAlError();

//     alSourcef(source, AL_PITCH, 1);
//     checkOpenAlError();
//     alSourcef(source, AL_GAIN, 1);
//     checkOpenAlError();
//     alSource3f(source, AL_POSITION, 0, 0, 0);
//     checkOpenAlError();
//     alSource3f(source, AL_VELOCITY, 0, 0, 0);
//     checkOpenAlError();
//     alSourcei(source, AL_LOOPING, AL_FALSE);
//     checkOpenAlError();

//     ALuint buffer;
//     alGenBuffers((ALuint)1, &buffer);
//     checkOpenAlError();

//     ALenum format = AL_FORMAT_MONO16;
//     s32 samplesPerSecond = 44100;
//     s32 toneHz = 256;
//     s32 wavePeriod = samplesPerSecond / toneHz;
//     s32 bytesPerSample = sizeof(s16);
//     s32 len = samplesPerSecond * bytesPerSample * 5;
//     s32 toneVolume = 32767;

//     printf("Start - Buffer fill\n");

//     u32 wavePos = 0;
//     #define PI 3.14159265359f
//     s16 *data = (s16*)malloc(len);
//     for (s32 i = 0; i < len / bytesPerSample; i++)
//     {
//         f32 t = 2.0f * PI * (f32)wavePos / (f32)wavePeriod;
//         f32 sineValue = sinf(t);
//         s16 sampleValue = (s16)(sineValue * toneVolume);
//         data[i] = sampleValue;
//         // printf("%d ", data[i]);
//         ++wavePos;
//     }

//     printf("\n");

//     printf("End - Buffer fill\n");

//     alBufferData(buffer, format, data, len, samplesPerSecond);

//     alSourcei(source, AL_BUFFER, buffer);
//     checkOpenAlError();

//     alSourcePlay(source);
//     checkOpenAlError();

//     ALenum source_state;
//     alGetSourcei(source, AL_SOURCE_STATE, &source_state);
//     checkOpenAlError();

//     while (source_state == AL_PLAYING) 
//     {
//         alGetSourcei(source, AL_SOURCE_STATE, &source_state);
//         // checkOpenAlError();
//     }

//     alDeleteSources(1, &source);
//     alDeleteBuffers(1, &buffer);
//     device = alcGetContextsDevice(context);
//     alcMakeContextCurrent(NULL);
//     alcDestroyContext(context);
//     alcCloseDevice(device);
// }

// int main() 
// {
//     srand(time(NULL));

//     initLog(LOG_SEVERITY_DEBUG);

//     testAudioStuff();

//     // glfwSetErrorCallback(glfwErrorCallback);

//     // if (!glfwInit())
//     // {
//     //     logError("Error initializing GLFW!\n");
//     //     return -1;
//     // }

//     // WarContext context = {0};
//     // if (!initGame(&context))
//     // {
//     //     logError("Can't initialize the game!\n");
//     //     return -1;
//     // }

//     // while (!glfwWindowShouldClose(context.window))
//     // {
//     //     sprintf(context.windowTitle, "War 1: %.2fs at %d fps (%.4fs)", context.time, context.fps, context.deltaTime);
//     //     glfwSetWindowTitle(context.window, context.windowTitle);

//     //     inputGame(&context);
//     //     updateGame(&context);
//     //     renderGame(&context);
//     //     presentGame(&context);
//     // }

//     // nvgDeleteGLES2(context.gfx);
//     // glfwDestroyWindow(context.window);
//     // glfwTerminate();
// 	return 0;
// }

// #include <unistd.h>

// #define CASE_RETURN(err) case (err): return "##err"
// const char* al_err_str(ALenum err) {
//     switch(err) {
//         CASE_RETURN(AL_NO_ERROR);
//         CASE_RETURN(AL_INVALID_NAME);
//         CASE_RETURN(AL_INVALID_ENUM);
//         CASE_RETURN(AL_INVALID_VALUE);
//         CASE_RETURN(AL_INVALID_OPERATION);
//         CASE_RETURN(AL_OUT_OF_MEMORY);
//     }
//     return "unknown";
// }
// #undef CASE_RETURN

// #define __al_check_error(file,line) \
//     do { \
//         ALenum err = alGetError(); \
//         for(; err!=AL_NO_ERROR; err=alGetError()) { \
//             printf("AL Error %s at %s:%d", al_err_str(err), file, line); \
//         } \
//     }while(0)

// #define al_check_error() \
//     __al_check_error(__FILE__, __LINE__)


// void init_al() {
//     ALCdevice *dev = NULL;
//     ALCcontext *ctx = NULL;

//     const char *defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
//     printf("Default device: %s\n", defname);

//     dev = alcOpenDevice(defname);
//     ctx = alcCreateContext(dev, NULL);
//     alcMakeContextCurrent(ctx);
// }

// void exit_al() {
//     ALCdevice *dev = NULL;
//     ALCcontext *ctx = NULL;
//     ctx = alcGetCurrentContext();
//     dev = alcGetContextsDevice(ctx);

//     alcMakeContextCurrent(NULL);
//     alcDestroyContext(ctx);
//     alcCloseDevice(dev);
// }

// int main(int argc, char* argv[]) {
//     /* initialize OpenAL */
//     init_al();

//     /* Create buffer to store samples */
//     ALuint buf;
//     alGenBuffers(1, &buf);
//     al_check_error();

//     /* Fill buffer with Sine-Wave */
//     float freq = 440.f;
//     int seconds = 4;
//     unsigned sample_rate = 22050;
//     size_t buf_size = seconds * sample_rate;

//     short *samples;
//     samples = (short*)malloc(buf_size);
//     for(int i=0; i<buf_size; ++i) {
//      #define M_PI 3.14159265359f
//         samples[i] = 32760 * sin( (2.f*(float)M_PI*freq)/sample_rate * i );
//     }

//     /* Download buffer to OpenAL */
//     alBufferData(buf, AL_FORMAT_MONO16, samples, buf_size, sample_rate);
//     al_check_error();

//     printf("play!\n");

//     /* Set-up sound source and play buffer */
//     ALuint src = 0;
//     alGenSources(1, &src);
//     alSourcei(src, AL_BUFFER, buf);
//     alSourcePlay(src);

//     /* While sound is playing, sleep */
//     al_check_error();
//     sleep(seconds);

//     printf("stop!\n");

//     /* Dealloc OpenAL */
//     exit_al();
//     al_check_error();
//     return 0;
// }

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"  /* Enables FLAC decoding. */
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"   /* Enables MP3 decoding. */
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"   /* Enables WAV decoding. */

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


// check this: https://github.com/dr-soft/miniaudio

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);

    (void)pInput;
}

int main(int argc, char** argv)
{
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    if (argc < 2) {
        printf("No input file.\n");
        return -1;
    }

    result = ma_decoder_init_file(argv[1], NULL, &decoder);
    if (result != MA_SUCCESS) {
        return -2;
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return -3;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -4;
    }

    printf("Press Enter to quit...");
    getchar();

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    return 0;
}
