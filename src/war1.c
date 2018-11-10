#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

// #define NDEBUG // define this to deactivate assertions
#include <assert.h>

#include <glew.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "nanovg.c"
#define NVG_DISABLE_CULL_FACE
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#if _DEBUG
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb_image_write.h"

    #define STB_IMAGE_RESIZE_IMPLEMENTATION
    #define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_BOX
    #include "stb_image_resize.h"
#endif

#include "array.h"
#include "list.h"
#include "queue.h"
#include "binary_heap.h"
#include "map.h"

#include "log.h"
#include "utils.h"
#include "io.h"
#include "glutils.h"
#include "war_math.h"
#include "war_types.h"
#include "war.h"
#include "war_database.h"
#include "war_map.h"
#include "war_units.h"
#include "war_state_machine.h"
#include "war_file.c"
#include "war_actions.c"
#include "war_render.c"
#include "war_resources.c"
#include "war_sprites.c"
#include "war_animations.c"
#include "war_entities.c"
#include "war_pathfinder.c"
#include "war_state_machine.c"
#include "war_map.c"
#include "war_game.c"

internal void glfwErrorCallback(int error, const char* description)
{
    logError("Error: %d, %s", error, description);
}

int main(int argc, char *argv[]) 
{
    srand(time(NULL));

    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
    {
        logError("Error initializing GLFW!");
        return -1;
    }

    WarContext context = {0};
    if (!initGame(&context))
    {
        logError("Can't initialize the game!");
        return -1;
    }

    while (!glfwWindowShouldClose(context.window))
    {
        sprintf(context.windowTitle, "War 1: %.2f at %d fps", context.time, context.fps);
        glfwSetWindowTitle(context.window, context.windowTitle);

        if (glfwGetKey(context.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(context.window, GL_TRUE);
            continue;
        }

        inputGame(&context);
        updateGame(&context);
        renderGame(&context);
        presentGame(&context);
    }

    nvgDeleteGL3(context.gfx);
    glfwDestroyWindow(context.window);
    glfwTerminate();
	return 0;
}