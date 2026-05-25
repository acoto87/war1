#include <stdio.h>
#include <string.h>

#include "war_editor_config.h"

#define WECFG_PATH "war1_editor.cfg"
#define WECFG_MAX_LINE 1024

static void wecfg_trimLine(char* line)
{
    size_t len;

    if (!line)
    {
        return;
    }

    len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
    {
        line[len - 1] = '\0';
        len--;
    }
}

void wecfg_addRecentFile(WarEditorContext* ctx, const char* path)
{
    int existingIndex;
    int insertIndex;

    if (!ctx || !path || !path[0])
    {
        return;
    }

    existingIndex = -1;
    for (int i = 0; i < ctx->recentFileCount; i++)
    {
        if (strcmp(ctx->recentFiles[i], path) == 0)
        {
            existingIndex = i;
            break;
        }
    }

    if (existingIndex > 0)
    {
        char existingPath[512];
        SDL_strlcpy(existingPath, ctx->recentFiles[existingIndex], sizeof(existingPath));
        for (int i = existingIndex; i > 0; i--)
        {
            SDL_strlcpy(ctx->recentFiles[i], ctx->recentFiles[i - 1], sizeof(ctx->recentFiles[i]));
        }
        SDL_strlcpy(ctx->recentFiles[0], existingPath, sizeof(ctx->recentFiles[0]));
        return;
    }

    if (existingIndex == 0)
    {
        return;
    }

    insertIndex = ctx->recentFileCount;
    if (insertIndex >= (int)SDL_arraysize(ctx->recentFiles))
    {
        insertIndex = (int)SDL_arraysize(ctx->recentFiles) - 1;
    }
    else
    {
        ctx->recentFileCount++;
    }

    for (int i = insertIndex; i > 0; i--)
    {
        SDL_strlcpy(ctx->recentFiles[i], ctx->recentFiles[i - 1], sizeof(ctx->recentFiles[i]));
    }
    SDL_strlcpy(ctx->recentFiles[0], path, sizeof(ctx->recentFiles[0]));
}

bool wecfg_load(WarEditorContext* ctx)
{
    FILE* file;
    char line[WECFG_MAX_LINE];

    if (!ctx)
    {
        return false;
    }

    file = fopen(WECFG_PATH, "rb");
    if (!file)
    {
        return false;
    }

    ctx->recentFileCount = 0;
    memset(ctx->recentFiles, 0, sizeof(ctx->recentFiles));

    while (fgets(line, sizeof(line), file))
    {
        int index;
        int intValue;
        float floatValue;
        char pathValue[512];

        wecfg_trimLine(line);

        if (sscanf(line, "window_width=%d", &intValue) == 1)
        {
            if (intValue > 320)
            {
                int currentHeight = 0;
                SDL_GetWindowSize(ctx->window, NULL, &currentHeight);
                SDL_SetWindowSize(ctx->window, intValue, currentHeight > 0 ? currentHeight : 720);
            }
            continue;
        }

        if (sscanf(line, "window_height=%d", &intValue) == 1)
        {
            if (intValue > 200)
            {
                int currentWidth = 0;
                SDL_GetWindowSize(ctx->window, &currentWidth, NULL);
                SDL_SetWindowSize(ctx->window, currentWidth > 0 ? currentWidth : 1280, intValue);
            }
            continue;
        }

        if (sscanf(line, "zoom=%f", &floatValue) == 1)
        {
            if (floatValue >= 0.5f && floatValue <= 4.0f)
            {
                ctx->cameraZoom = floatValue;
            }
            continue;
        }

        if (sscanf(line, "camera_x=%f", &floatValue) == 1)
        {
            ctx->cameraOffset.x = floatValue;
            continue;
        }

        if (sscanf(line, "camera_y=%f", &floatValue) == 1)
        {
            ctx->cameraOffset.y = floatValue;
            continue;
        }

        if (sscanf(line, "show_grid=%d", &intValue) == 1)
        {
            ctx->showGrid = (intValue != 0);
            continue;
        }

        if (sscanf(line, "show_passability=%d", &intValue) == 1)
        {
            ctx->showPassability = (intValue != 0);
            continue;
        }

        if (sscanf(line, "show_start_location=%d", &intValue) == 1)
        {
            ctx->showStartLocation = (intValue != 0);
            continue;
        }

        if (sscanf(line, "show_minimap=%d", &intValue) == 1)
        {
            ctx->showMinimap = (intValue != 0);
            continue;
        }

        pathValue[0] = '\0';
        if (sscanf(line, "recent_file_%d=%511[^\n]", &index, pathValue) == 2)
        {
            if (index >= 0 && index < (int)SDL_arraysize(ctx->recentFiles) && pathValue[0])
            {
                SDL_strlcpy(ctx->recentFiles[index], pathValue, sizeof(ctx->recentFiles[index]));
                if (index >= ctx->recentFileCount)
                {
                    ctx->recentFileCount = index + 1;
                }
            }
        }
    }

    fclose(file);
    return true;
}

bool wecfg_save(WarEditorContext* ctx)
{
    FILE* file;
    int windowW;
    int windowH;

    if (!ctx)
    {
        return false;
    }

    file = fopen(WECFG_PATH, "wb");
    if (!file)
    {
        return false;
    }

    SDL_GetWindowSize(ctx->window, &windowW, &windowH);

    fprintf(file, "window_width=%d\n", windowW);
    fprintf(file, "window_height=%d\n", windowH);
    fprintf(file, "zoom=%.4f\n", (double)ctx->cameraZoom);
    fprintf(file, "camera_x=%.4f\n", (double)ctx->cameraOffset.x);
    fprintf(file, "camera_y=%.4f\n", (double)ctx->cameraOffset.y);
    fprintf(file, "show_grid=%d\n", ctx->showGrid ? 1 : 0);
    fprintf(file, "show_passability=%d\n", ctx->showPassability ? 1 : 0);
    fprintf(file, "show_start_location=%d\n", ctx->showStartLocation ? 1 : 0);
    fprintf(file, "show_minimap=%d\n", ctx->showMinimap ? 1 : 0);

    for (int i = 0; i < ctx->recentFileCount && i < (int)SDL_arraysize(ctx->recentFiles); i++)
    {
        fprintf(file, "recent_file_%d=%s\n", i, ctx->recentFiles[i]);
    }

    fclose(file);
    return true;
}