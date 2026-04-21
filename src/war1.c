#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
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

// #define NDEBUG // define this to deactivate assertions
#include <assert.h>
#include "SDL3/SDL.h"

// https://github.com/schellingb/TinySoundFont
#define TSF_IMPLEMENTATION
#include "TinySoundFont/tsf.h"
#define TML_IMPLEMENTATION
#define TML_ERROR(msg) printf("ERROR: %s\n", msg)
#define TML_WARN(msg) printf("WARNING: %s\n", msg)
#include "TinySoundFont/tml.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

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

#include "log.h"
#include "utils.h"
#include "war_math.h"
#include "io.h"
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

int main()
{
    srand((unsigned int)time(NULL));

    initLog(LOG_SEVERITY_DEBUG);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        logError("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    WarContext context = {0};
    if (!initGame(&context))
    {
        logError("Can't initialize the game!\n", NO_ARG_STR);
        return -1;
    }

    bool running = true;

    while (running)
    {
        beginInputFrame(&context);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            processGameEvent(&context, &event);

            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        sprintf(context.windowTitle, "War 1: %.2fs at %d fps (%.4fs)", context.time, context.fps, context.deltaTime);
        SDL_SetWindowTitle(context.window, context.windowTitle);

        updateGame(&context);
        renderGame(&context);
        presentGame(&context);
    }

    quitGame(&context);
	return 0;
}
