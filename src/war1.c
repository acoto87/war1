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

#include "cglm.h"

#if _DEBUG
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb_image_write.h"

    #define STB_IMAGE_RESIZE_IMPLEMENTATION
    #define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_BOX
    #include "stb_image_resize.h"
#endif

#include "list.h"

#include "utils.h"
#include "io.h"
#include "glutils.h"
#include "war_math.h"
#include "war_types.h"
#include "war_database.h"
#include "war_file.c"
#include "war_render.c"
#include "war_resources.c"
#include "war_entities.c"
#include "war_map.c"
#include "war_game.c"

internal void glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %d, %s\n", error, description);
}

int main(int argc, char *argv[]) 
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
    {
        printf("Error initializing GLFW!");
        return -1;
    }

    WarContext context = {0};
    if (!initGame(&context))
    {
        printf("Can't initialize the game!\n");
        return -1;
    }

    WarRenderingContext2D *ctx = create(context.windowWidth, context.windowHeight);
    if (!ctx)
    {
        printf("Can't initialize rendering context!\n");
        return -1;
    }

    WarImageData imageData = createImageData(ctx, 128, 128);
    
    for(s32 y = 0; y < 128; y++)
    {
        for(s32 x = 0; x < 128; x++)
        {
            s32 pixel = y * 128 + x;

            u8 r = 255;
            u8 g = 255;
            u8 b = 255;

            if (x < 64 && y < 64)
            {
                g = b = 0;
            }
            else if(x < 64)
            {
                r = b = 0;
            }
            else if(y < 64)
            {
                r = g = 0;
            }

            imageData.data[pixel * 4 + 0] = r;
            imageData.data[pixel * 4 + 1] = g;
            imageData.data[pixel * 4 + 2] = b;
            imageData.data[pixel * 4 + 3] = 255;
        }
    }    

    s32 x = 0;
    s32 angle = 0;

    while (!glfwWindowShouldClose(context.window))
    {
        sprintf(context.windowTitle, "War 1: %.2f at %d fps", context.time, context.fps);
        glfwSetWindowTitle(context.window, context.windowTitle);

        if (glfwGetKey(context.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(context.window, GL_TRUE);
            continue;
        }

        x += (s32)(100 * context.deltaTime);
        angle += (s32)(180 * context.deltaTime);

        beginFrame(ctx);

        clearRect(ctx, 0, 0, context.windowWidth, context.windowHeight);

        translate(ctx, 300, 0);

        translate(ctx, 50, 50);
        rotate(ctx, angle*3.14f/180);
        translate(ctx, -50, -50);

        fillStyle(ctx, WAR_COLOR_RGBA(255, 255, 255, 255));
        drawImage(ctx, imageData, 50, 50);

        fillStyle(ctx, WAR_COLOR_RGBA(255, 0, 0, 150));
        fillRect(ctx, x, 100, 100, 100);

        lineWidth(ctx, 3);
        strokeStyle(ctx, WAR_COLOR_RGBA(255, 255, 0, 120));
        strokeRect(ctx, 100, x, 100, 100);

        endFrame(ctx);

        // updateGame(&context);
        // renderGame(&context);
        presentGame(&context);
    }

    glfwDestroyWindow(context.window);
    glfwTerminate();
	return 0;
}