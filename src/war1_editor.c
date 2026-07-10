// war1_editor.c — War1-C Scenario Editor unity entry point (Phase 2)
//
// This is the single translation unit compiled for the editor binary.
// Shared game-core source files and editor-only source files are #included
// at the bottom of this file.
//
// Build:
//   nob editor --cc msvc --target win64 [--check]
//   nob editor --cc gcc  --target linux64

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _DEFAULT_SOURCE

// -------------------------------------------------------------------------
// cimgui C API (must come before SDL3 to define CIMGUI_API first)
// The following macros are passed via -D by nob.c build_editor():
//   CIMGUI_DEFINE_ENUMS_AND_STRUCTS, CIMGUI_NO_EXPORT,
//   CIMGUI_USE_SDL3, CIMGUI_USE_SDLRENDERER3
// -------------------------------------------------------------------------
#include "cimgui/cimgui.h"
#include "cimgui/cimgui_impl.h"

// -------------------------------------------------------------------------
// Standard headers
// -------------------------------------------------------------------------
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
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp
#endif

// #define NDEBUG // define this to deactivate assertions
#include <assert.h>

// -------------------------------------------------------------------------
// SDL3
// -------------------------------------------------------------------------
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

// -------------------------------------------------------------------------
// Tracy profiler C API — all macros are no-ops unless TRACY_ENABLE is defined
// -------------------------------------------------------------------------
#include "TracyC.h"

// -------------------------------------------------------------------------
// Memory zone
// -------------------------------------------------------------------------
#define SHL_MZ_IMPLEMENTATION
#ifdef SHL_MZ_DEBUG
#   define SHL_MZ_AUDIT_IMPLEMENTATION
#   include "shl/memzone_audit.h"
#else
#   include "shl/memzone.h"
#endif

#include "war_alloc.h"

// -------------------------------------------------------------------------
// stb_image (sprite loading; no audio stb libs needed in the editor)
// -------------------------------------------------------------------------
#define STBI_MALLOC(sz)       wm_alloc(sz)
#define STBI_REALLOC(p,newsz) wm_realloc(p,newsz)
#define STBI_FREE(p)          wm_free(p)
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STBIW_MALLOC(sz)       wm_alloc(sz)
#define STBIW_REALLOC(p,newsz) wm_realloc(p,newsz)
#define STBIW_FREE(p)          wm_free(p)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define STBIR_MALLOC(sz,c) ((void)(c), wm_alloc(sz))
#define STBIR_FREE(p,c)    ((void)(c), wm_free(p))
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"

// -------------------------------------------------------------------------
// SHL data structures
// -------------------------------------------------------------------------
#include "shl/alloc.h"
#include "shl/list.h"
#include "shl/queue.h"
#include "shl/binary_heap.h"
#include "shl/map.h"
#include "shl/set.h"

#define SHL_MEMORY_BUFFER_IMPLEMENTATION
#define MB_MALLOC(sz)     wm_alloc(sz)
#define MB_CALLOC(n, sz)  wm_alloc((n) * (sz))
#define MB_FREE(p)        wm_free(p)
#include "shl/memory_buffer.h"

#define WSTR_MALLOC(sz)     wm_alloc(sz)
#define WSTR_REALLOC(p, sz) wm_realloc((p), (sz))
#define WSTR_FREE(p)        wm_free(p)
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

// -------------------------------------------------------------------------
// War1-C headers
// -------------------------------------------------------------------------
#include "war_log.h"
#include "war.h"
#include "war_ui.h"               // declares wui_* used by war_entities.c
#include "TinySoundFont/tml.h"    // declares tml_load_memory/tml_free used by war_entities.c
#include "editor/war_editor.h"

// -------------------------------------------------------------------------
// Memory sizes for the editor (smaller than the game; no audio zone used)
// -------------------------------------------------------------------------
#define PERM_SIZE  (134217728ULL)   // 128 MB
#define FRAME_SIZE (4194304ULL)     //   4 MB
#define AUDIO_SIZE (1048576ULL)     //   1 MB (stub — audio zone unused)

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!wm_allocInit(PERM_SIZE, FRAME_SIZE, AUDIO_SIZE))
    {
        logError("Failed to initialize memory allocators!");
        return -1;
    }

    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);

    WarEditorContext* ctx = (WarEditorContext*)wm_alloc(sizeof(WarEditorContext));
    if (!ctx)
    {
        logError("Failed to allocate WarEditorContext!");
        wm_allocFree();
        return -1;
    }

    if (!we_init(ctx))
    {
        logError("Failed to initialize the editor!");
        wm_allocFree();
        return -1;
    }

    we_run(ctx);
    we_quit(ctx);
    wm_allocFree();
    return 0;
}

// -------------------------------------------------------------------------
// Shared core source files (subset of war1.c — no game-only modules)
// -------------------------------------------------------------------------
#include "war_log.c"
#include "war_alloc.c"
#include "war_math.c"
#include "war_collections.c"
#include "war_file.c"
#include "war_render.c"
#include "war_resources.c"
#include "war_sprites.c"
#include "war_font.c"
#include "war_animations.c"
#include "war_roads.c"
#include "war_walls.c"
#include "war_ruins.c"
#include "war_trees.c"
#include "war_units.c"
#include "war_projectiles.h"  // forward-declare before war_entities.c calls wproj_createProjectile
#include "war_entities.c"
#include "war_actions.c"      // defines wact_* and __frameCountToSeconds (used by state machines)
#include "war_map_grid.c"
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

// -------------------------------------------------------------------------
// Editor-only source files
// -------------------------------------------------------------------------
#include "editor/war_editor_ui.c"
#include "editor/war_editor_config.c"
#include "editor/war_editor_map.c"
#include "editor/war_editor_canvas.c"
#include "editor/war_editor_autotile.c"
#include "editor/war_editor_history.c"
#include "editor/war_editor_tools.c"
#include "editor/war_editor_inspector.c"
#include "editor/war_editor_serialization.c"
#include "editor/war_editor_playtest.c"
#include "editor/war_editor_minimap.c"
#include "editor/war_editor_validator.c"
#include "editor/war_editor.c"
#include "editor/war_editor_stubs.c"
