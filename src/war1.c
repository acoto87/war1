#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#if defined(_MSC_VER) && !defined(__clang__)
#include <io.h>
#ifndef F_OK
#define F_OK 0
#endif
#define access _access
#else
#include <unistd.h>
#endif
#include <errno.h>

#if defined(_WIN32) && defined(_MSC_VER) && !defined(__clang__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

// #define NDEBUG // define this to deactivate assertions
#include <assert.h>

#include "SDL3/SDL.h"
#include "TracyC.h"

#define SHL_MZ_IMPLEMENTATION
#ifdef SHL_MZ_DEBUG
#   define SHL_MZ_AUDIT_IMPLEMENTATION
#   include "shl/memzone_audit.h"
#else
#   include "shl/memzone.h"
#endif

#include "war_alloc.h"

// https://github.com/schellingb/TinySoundFont
#define TSF_MALLOC(sz) wm_allocAudio(sz)
#define TSF_REALLOC(p,sz) wm_reallocAudio((p),(sz))
#define TSF_FREE(p) wm_free(p)
#define TSF_IMPLEMENTATION
#include "TinySoundFont/tsf.h"

#define TML_MALLOC(sz) wm_allocAudio(sz)
#define TML_REALLOC(p,sz) wm_reallocAudio((p),(sz))
#define TML_FREE(p) wm_free(p)
#define TML_ERROR(msg) printf("ERROR: %s\n", msg)
#define TML_WARN(msg) printf("WARNING: %s\n", msg)
#define TML_IMPLEMENTATION
#include "TinySoundFont/tml.h"

#define STBI_MALLOC(sz)           wm_alloc(sz)
#define STBI_REALLOC(p,newsz)     wm_realloc(p,newsz)
#define STBI_FREE(p)              wm_free(p)
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STBIW_MALLOC(sz)          wm_alloc(sz)
#define STBIW_REALLOC(p,newsz)    wm_realloc(p,newsz)
#define STBIW_FREE(p)             wm_free(p)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define STBIR_MALLOC(sz,c)        ((void)(c), wm_alloc(sz))
#define STBIR_FREE(p,c)           ((void)(c), wm_free(p))
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"

#include "shl/alloc.h"
#include "shl/list.h"
#include "shl/queue.h"
#include "shl/binary_heap.h"
#include "shl/map.h"
#include "shl/set.h"

#define SHL_MEMORY_BUFFER_IMPLEMENTATION
#define MB_MALLOC(sz) wm_alloc(sz)
#define MB_CALLOC(n, sz) wm_alloc((n) * (sz))
#define MB_FREE(p) wm_free(p)
#include "shl/memory_buffer.h"

#define WSTR_MALLOC(sz)       wm_alloc(sz)
#define WSTR_REALLOC(p, sz)   wm_realloc((p), (sz))
#define WSTR_FREE(p)          wm_free(p)
#define SHL_WSTR_IMPLEMENTATION
#include "shl/wstr.h"

#define X2M_REALLOC(ptr, size) wm_realloc(ptr, size)
#define X2M_FREE(ptr)          wm_free(ptr)
#define XMI2MID_IMPLEMENTATION
#include "shl/xmi2mid.h"

#define MINIWAVE_MALLOC(sz)       wm_alloc(sz)
#define MINIWAVE_REALLOC(ptr, sz) wm_realloc((ptr), (sz))
#define MINIWAVE_FREE(ptr)        wm_free(ptr)
#define MINIWAVE_IMPLEMENTATION
#include "shl/wav.h"

#define MINIVOC_MALLOC(sz)       wm_alloc(sz)
#define MINIVOC_REALLOC(ptr, sz) wm_realloc((ptr), (sz))
#define MINIVOC_FREE(ptr)        wm_free(ptr)
#define MINIVOC_IMPLEMENTATION
#include "shl/voc.h"

#include "war_log.h"
#include "war.h"
#include "war_game.h"

#define PERM_SIZE (536870912ULL) // 512 MB
#define FRAME_SIZE (4194304ULL)  // 4 MB
#define AUDIO_SIZE (33554432ULL)  // 32 MB

int main(int argc, char** argv)
{
    if (!wm_allocInit(PERM_SIZE, FRAME_SIZE, AUDIO_SIZE))
    {
        logError("Failed to initialize memory allocators!");
        return -1;
    }

    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        logError("Error initializing SDL: %s", SDL_GetError());
        wm_allocFree();
        return -1;
    }

    WarContext* context = (WarContext*)wm_alloc(sizeof(WarContext));
    if (!context)
    {
        logError("Failed to allocate WarContext!");
        wm_allocFree();
        SDL_Quit();
        return -1;
    }

    // Parse command-line arguments.
    for (int i = 1; i < argc; i++)
    {
        StringView arg = wsv_fromCString(argv[i]);
        if (wsv_equals(arg, WSV_LITERAL("--skip-intro")))
        {
            context->skipIntro = true;
        }
        else if ((wsv_equals(arg, WSV_LITERAL("--map")) ||
                  wsv_equals(arg, WSV_LITERAL("-m"))) && i + 1 < argc)
        {
            SDL_strlcpy(context->customMapPath, argv[i + 1], sizeof(context->customMapPath));
            logInfo("Custom map path: %s", context->customMapPath);
            i++; // consume value
        }
    }

    if (!wg_initGame(context))
    {
        logError("Can't initialize the game!");
        wm_allocFree();
        SDL_Quit();
        return -1;
    }

    bool running = true;

    while (running)
    {
        wg_beginFrame(context);
        wg_beginInputFrame(context);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            wg_processGameEvent(context, &event);

            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        wstr_setFormat(
            &context->windowTitle,
            "War 1: %.2fs at %d avg fps (%.4fs) - Wait time: %.4fs - Frames: %d - Transition Delay: %.4fs",
            context->realTime,
            context->fpsMetric.avg,
            ns2s(context->frameTimeMetric.avg),
            ns2s(context->waitTimeMetric.avg),
            context->frameCount,
            MAX(context->transitionEndRealTime - context->realTime, 0.0)
        );
        SDL_SetWindowTitle(context->window, wstr_cstr(&context->windowTitle));

        wg_updateGame(context);
        wg_renderGame(context);
        wg_presentGame(context);
        TracyCFrameMark

        context->frameCount++;
    }

    wg_quitGame(context);
    wm_allocFree();
	return 0;
}

#include "war_log.c"
#include "war_alloc.c"
#include "war_math.c"
#include "war_collections.c"
#include "war_file.c"
#include "war_audio.c"
#include "war_net.c"
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
#include "war_commands.c"
#include "war_units.c"
#include "war_projectiles.c"
#include "war_entities.c"
#include "war_pathfinder.c"
#include "war_rvo.c"
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
#include "war_state_machine_cast.c"
#include "war_state_machine.c"
#include "war_state_machine_debug.c"
#include "war_campaigns.c"
#include "war_cheats.c"
#include "war_map_menu.c"
#include "war_map_ui.c"
#include "war_map_grid.c"
#include "war_map.c"
#include "war_cheats_panel.c"
#include "war_scene_download.c"
#include "war_scene_blizzard.c"
#include "war_scene_menu.c"
#include "war_scene_briefing.c"
#include "war_scenes.c"
#include "war_ui.c"
#include "war_imui.c"
#include "war_ai.c"
#include "war_game.c"
