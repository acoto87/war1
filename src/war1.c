#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
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

// Tracy profiler C API — all macros are no-ops unless TRACY_ENABLE is defined
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
#define TSF_MALLOC(sz) wm_alloc(sz)
#define TSF_REALLOC(p,sz) wm_realloc((p),(sz))
#define TSF_FREE(p) wm_free(p)
#define TSF_IMPLEMENTATION
#include "TinySoundFont/tsf.h"

#define TML_MALLOC(sz) wm_alloc(sz)
#define TML_REALLOC(p,sz) wm_realloc((p),(sz))
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

#define SHL_MALLOC(sz) wm_alloc(sz)
#define SHL_CALLOC(n, sz) wm_calloc((n), (sz))
#define SHL_REALLOC(p, sz) wm_realloc((p), (sz))
#define SHL_FREE(p) wm_free(p)

#include "shl/list.h"
#include "shl/queue.h"
#include "shl/binary_heap.h"
#include "shl/map.h"
#include "shl/set.h"
#define SHL_MEMORY_BUFFER_IMPLEMENTATION
#define MB_MALLOC(sz) wm_alloc(sz)
#define MB_CALLOC(n, sz) wm_calloc((n), (sz))
#define MB_FREE(p) wm_free(p)
#include "shl/memory_buffer.h"
#define SHL_WAV_IMPLEMENTATION
#include "shl/wav.h"
#define WSTR_MALLOC(sz)       wm_alloc(sz)
#define WSTR_REALLOC(p, sz)   wm_realloc((p), (sz))
#define WSTR_FREE(p)          wm_free(p)
#define SHL_WSTR_IMPLEMENTATION
#include "shl/wstr.h"

#include "war_log.h"
#include "common.h"
#include "war_color.h"
#include "war_math.h"
#include "war_types.h"
#include "war.h"
#include "war_net.h"
#include "war_database.h"
#include "war_cheats.h"
#include "war_map_menu.h"
#include "war_map_ui.h"
#include "war_map.h"
#include "war_commands.h"
#include "war_units.h"
#include "war_entities.h"
#include "war_pathfinder.h"
#include "war_state_machine.h"
#include "war_scene_menu.h"
#include "war_scenes.h"
#include "war_ui.h"
#include "war_ai.h"
#include "war_game.h"

#define PERM_SIZE (536870912ULL) // 512 MB
#define FRAME_SIZE (4194304ULL)  // 4 MB

int main(void)
{
    if (!wm_allocInit(PERM_SIZE, FRAME_SIZE))
    {
        logError("Failed to initialize memory allocators!");
        return -1;
    }

    srand((unsigned int)time(NULL));

    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        logError("Error initializing SDL: %s", SDL_GetError());
        return -1;
    }

    WarContext context = {0};
    if (!wg_initGame(&context))
    {
        logError("Can't initialize the game!");
        return -1;
    }

    u32 frameCount = 0;

    bool running = true;

    while (running)
    {
        wg_beginInputFrame(&context);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            wg_processGameEvent(&context, &event);

            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        wstr_setFormat(&context.windowTitle, "War 1: %.2fs at %d fps (%.4fs) - Frames: %u", context.time, context.fps, context.deltaTime, frameCount);
        SDL_SetWindowTitle(context.window, wstr_cstr(&context.windowTitle));

        wg_updateGame(&context);
        wg_renderGame(&context);
        wg_presentGame(&context);
        TracyCFrameMark

        frameCount++;
    }

    wg_quitGame(&context);
    wm_allocFree();
	return 0;
}

#include "war_log.c"
#include "war_alloc.c"
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
#include "war_campaigns.c"
#include "war_cheats.c"
#include "war_map_menu.c"
#include "war_map_ui.c"
#include "war_map.c"
#include "war_cheats_panel.c"
#include "war_scene_download.c"
#include "war_scene_blizzard.c"
#include "war_scene_menu.c"
#include "war_scene_briefing.c"
#include "war_scenes.c"
#include "war_ui.c"
#include "war_ai.c"
#include "war_game.c"
