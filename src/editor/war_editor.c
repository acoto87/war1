#include "war_editor.h"
#include "war_editor_canvas.h"
#include "war_editor_config.h"
#include "war_editor_history.h"
#include "war_editor_inspector.h"
#include "war_editor_map.h"
#include "war_editor_ui.h"

bool we_init(WarEditorContext* ctx)
{
    memset(ctx, 0, sizeof(WarEditorContext));

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        logError("Error initializing SDL: %s", SDL_GetError());
        return false;
    }

    ctx->window = SDL_CreateWindow(
        "War1-C Scenario Editor",
        1280, 720,
        SDL_WINDOW_RESIZABLE
    );
    if (!ctx->window)
    {
        logError("Error creating window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    ctx->renderer = SDL_CreateRenderer(ctx->window, NULL);
    if (!ctx->renderer)
    {
        logError("Error creating renderer: %s", SDL_GetError());
        SDL_DestroyWindow(ctx->window);
        SDL_Quit();
        return false;
    }

    // Load DATA.WAR (ignores context — wfile_loadWarFile uses NULL safely).
    ctx->warFile = wfile_loadWarFile((WarContext*)NULL, wsv_fromCString(DATAWAR_FILE_PATH));
    if (!ctx->warFile)
    {
        logWarning("DATA.WAR not found at %s; some features will be unavailable.", DATAWAR_FILE_PATH);
    }

    weui_init(ctx->window, ctx->renderer);

    ctx->running           = true;
    ctx->showGrid          = true;
    ctx->showMinimap       = true;
    ctx->showStartLocation = true;
    ctx->cameraZoom        = 1.0f;
    ctx->activeTool        = WE_TOOL_SELECT;
    ctx->minimapDirty      = true;
    SDL_strlcpy(ctx->statusText, "Ready", sizeof(ctx->statusText));

    wecfg_load(ctx);

    WarEntityIdListInit(&ctx->selectedEntities, WarEntityIdListDefaultOptions);

    ctx->history = (WarEditorHistory*)wm_alloc(sizeof(WarEditorHistory));
    memset(ctx->history, 0, sizeof(WarEditorHistory));

    if (!wemap_createEmpty(ctx))
    {
        logError("Failed to create empty map!");
        weui_shutdown();
        SDL_DestroyRenderer(ctx->renderer);
        SDL_DestroyWindow(ctx->window);
        SDL_Quit();
        return false;
    }

    wemap_buildTerrainSprite(ctx);

    return true;
}

void we_run(WarEditorContext* ctx)
{
    u64 prevTime = SDL_GetTicks();

    while (ctx->running)
    {
        // -----------------------------------------------------------------------
        // Timing
        // -----------------------------------------------------------------------
        u64 now       = SDL_GetTicks();
        f32 deltaTime = (f32)(now - prevTime) / 1000.0f;
        prevTime      = now;

        ctx->deltaTime  = deltaTime;
        ctx->time      += deltaTime;
        ctx->frameCount++;

        // -----------------------------------------------------------------------
        // Event loop
        // -----------------------------------------------------------------------
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT)
            {
                // Route through weui_beginFrame so the unsaved-changes modal
                // can intercept if ctx->unsavedChanges is true.
                ctx->quitRequested = true;
            }
        }

        // -----------------------------------------------------------------------
        // Render
        // -----------------------------------------------------------------------
        SDL_SetRenderDrawColor(ctx->renderer, 30, 30, 30, 255);
        SDL_RenderClear(ctx->renderer);

        weui_beginFrame(ctx);
        wecanvas_render(ctx);
        weui_endFrame(ctx->renderer);

        SDL_RenderPresent(ctx->renderer);
    }
}

void we_quit(WarEditorContext* ctx)
{
    wecfg_save(ctx);

    WarEntityIdListFree(&ctx->selectedEntities);
    wemap_free(ctx);
    weui_shutdown();

    if (ctx->minimapTexture)
    {
        SDL_DestroyTexture(ctx->minimapTexture);
        ctx->minimapTexture = NULL;
    }

    if (ctx->renderer)
    {
        SDL_DestroyRenderer(ctx->renderer);
        ctx->renderer = NULL;
    }

    if (ctx->window)
    {
        SDL_DestroyWindow(ctx->window);
        ctx->window = NULL;
    }

    SDL_Quit();
}
