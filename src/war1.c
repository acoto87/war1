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

#include "log.h"
#include "utils.h"
#include "io.h"
#include "glutils.h"
#include "war_math.h"
#include "war_types.h"
#include "war_database.h"
#include "war_actions.h"
#include "war_file.c"
#include "war_render.c"
#include "war_sprites.c"
#include "war_resources.c"
// #include "war_state_machine.c"
#include "war_entities.c"
#include "war_map.c"
#include "war_game.c"

internal void glfwErrorCallback(int error, const char* description)
{
    logError("Error: %d, %s", error, description);
}

void printFrameNumber(FrameNumbers fn)
{
    printf("{ ");

    printf("{ ");
    for(s32 i = 0; i < fn.walkFramesCount; i++)
    {
        if (i > 0)
            printf(", ");

        printf("%d", fn.walkFrames[i]);
    }
    printf(" }");
    
    printf(", { ");
    for(s32 i = 0; i < fn.attackFramesCount; i++)
    {
        if (i > 0)
            printf(", ");

        printf("%d", fn.attackFrames[i]);
    }
    printf(" }");

    printf(", { ");
    for(s32 i = 0; i < fn.deathFramesCount; i++)
    {
        if (i > 0)
            printf(", ");

        printf("%d", fn.deathFrames[i]);
    }
    printf(" }");

    printf(" }\n");
}

int main(int argc, char *argv[]) 
{
    FrameNumbers frameNumbers_5_5_5_5 = getFrameNumbers(5, a1);
    FrameNumbers frameNumbers_5_5_5_4 = getFrameNumbers(5, a2);
    FrameNumbers frameNumbers_5_5_5_3 = getFrameNumbers(5, a3);
    FrameNumbers frameNumbers_5_5_4_5 = getFrameNumbers(5, a4);
    FrameNumbers frameNumbers_5_5_4_4 = getFrameNumbers(5, a5);
    FrameNumbers frameNumbers_5_5_4_3 = getFrameNumbers(5, a6);
    FrameNumbers frameNumbers_5_5_3_2 = getFrameNumbers(5, a7);
    FrameNumbers frameNumbers_5_5_2_3 = getFrameNumbers(5, a8);
    FrameNumbers frameNumbers_5_3_5_3 = getFrameNumbers(5, a9);
    FrameNumbers frameNumbers_5_2_5_3 = getFrameNumbers(5, aa);
    FrameNumbers frameNumbers_5_4_3_3 = getFrameNumbers(5, ab);

    WarUnitAction* action = buildWalkAnimation(frameNumbers_5_5_2_3.walkFramesCount, frameNumbers_5_5_2_3.walkFrames, 6);

    for(s32 i = 0; i < action->steps.count; i++)
    {
        printf("%d, %d\n", action->steps.items[i].type, action->steps.items[i].param);
    }

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