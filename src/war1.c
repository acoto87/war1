#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef __GNUC__
// this is to silence GCC about this warning since nanovg is full of them
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>

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
#define TML_ERROR(msg) printf("ERROR: %s\n", msg)
#define TML_WARN(msg) printf("WARNING: %s\n", msg)
#include "TinySoundFont/tml.h"

// https://github.com/dr-soft/miniaudio
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"

#include "shl/array.h"
#include "shl/list.h"
#include "shl/queue.h"
#include "shl/binary_heap.h"
#include "shl/map.h"
#include "shl/set.h"
#define SHL_MEMORY_BUFFER_IMPLEMENTATION
#include "shl/memory_buffer.h"
#define SHL_WAVE_WRITER_IMPLEMENTATION
#include "shl/wave_writer.h"

#include "mutex.h"
#include "log.h"
#include "utils.h"
#include "war_math.h"
#include "io.h"
#include "glutils.h"
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

void glfwErrorCallback(int error, const char* description)
{
    logError("Error: %d, %s\n", error, description);
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

    glfwSetWindowUserPointer(context.window, &context);
    glfwSetCharCallback(context.window, inputCharCallback);

    while (!glfwWindowShouldClose(context.window))
    {
        sprintf(context.windowTitle, "War 1: %.2fs at %d fps (%.4fs)", context.time, context.fps, context.deltaTime);
        glfwSetWindowTitle(context.window, context.windowTitle);

        inputGame(&context);
        updateGame(&context);
        renderGame(&context);
        presentGame(&context);
    }

    ma_device_uninit(&context.sfx);
    nvgDeleteGLES2(context.gfx);
    glfwDestroyWindow(context.window);
    glfwTerminate();
	return 0;
}
