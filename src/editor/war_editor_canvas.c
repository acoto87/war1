#include "war_editor_canvas.h"

// ---------------------------------------------------------------------------
// 4.2 — Create an off-screen SDL_TEXTUREACCESS_TARGET texture
// ---------------------------------------------------------------------------
SDL_Texture* wecanvas_createTarget(SDL_Renderer* renderer, int w, int h)
{
    SDL_Texture* tex = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        w, h);

    if (!tex)
    {
        logError("wecanvas_createTarget: SDL_CreateTexture(%dx%d) failed: %s",
                 w, h, SDL_GetError());
        return NULL;
    }

    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    return tex;
}

// ---------------------------------------------------------------------------
// 4.4 — Render visible terrain tiles to the active SDL render target.
//        Called only from wecanvas_render (render target already set).
// ---------------------------------------------------------------------------
static void wecanvas_renderTerrain(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx->map;
    if (!m || !m->terrainSprite.texture)
        return;

    SDL_Texture* tex    = m->terrainSprite.texture;
    f32          zoom   = ctx->cameraZoom;
    f32          camX   = ctx->cameraOffset.x;
    f32          camY   = ctx->cameraOffset.y;
    f32          cw     = (f32)ctx->canvasPanelW;
    f32          ch     = (f32)ctx->canvasPanelH;

    for (s32 ty = 0; ty < MAP_TILES_HEIGHT; ty++)
    {
        for (s32 tx = 0; tx < MAP_TILES_WIDTH; tx++)
        {
            u16 tileIndex = m->visualData[ty * MAP_TILES_WIDTH + tx];

            // Source rect inside the tileset atlas
            s32 srcX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 srcY = (tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT;
            SDL_FRect rs = { (f32)srcX, (f32)srcY,
                             (f32)MEGA_TILE_WIDTH, (f32)MEGA_TILE_HEIGHT };

            // Destination rect in canvas pixels (camera transform applied)
            f32 dstX = ((f32)(tx * MEGA_TILE_WIDTH)  - camX) * zoom;
            f32 dstY = ((f32)(ty * MEGA_TILE_HEIGHT) - camY) * zoom;
            f32 dstW = (f32)MEGA_TILE_WIDTH  * zoom;
            f32 dstH = (f32)MEGA_TILE_HEIGHT * zoom;

            // Frustum cull — skip tiles fully outside the canvas panel
            if (dstX + dstW < 0.0f || dstX >= cw) continue;
            if (dstY + dstH < 0.0f || dstY >= ch) continue;

            SDL_FRect rd = { dstX, dstY, dstW, dstH };
            SDL_RenderTexture(ctx->renderer, tex, &rs, &rd);
        }
    }
}

// ---------------------------------------------------------------------------
// 4.3 — Render the map to ctx->canvasTarget
// ---------------------------------------------------------------------------
void wecanvas_render(WarEditorContext* ctx)
{
    if (!ctx->canvasTarget)
        return;

    // Redirect SDL rendering to the off-screen target
    SDL_SetRenderTarget(ctx->renderer, ctx->canvasTarget);
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(ctx->renderer);

    wecanvas_renderTerrain(ctx);

    // Reset to default render target (the window framebuffer)
    SDL_SetRenderTarget(ctx->renderer, NULL);
}

// ---------------------------------------------------------------------------
// 4.5 + 4.6 + 4.7 + 4.8 — cimgui canvas panel
// ---------------------------------------------------------------------------
void wecanvas_drawPanel(WarEditorContext* ctx, char* statusBuf, s32 statusBufLen)
{
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2_c){ 0.0f, 0.0f });
    if (igBegin("Canvas##canvas", NULL, flags))
    {
        // --- 4.5: panel size + resize detection ---
        ImVec2_c avail = igGetContentRegionAvail();
        int w = (int)avail.x;
        int h = (int)avail.y;
        if (w < 1) w = 1;
        if (h < 1) h = 1;

        if (w != ctx->canvasPanelW || h != ctx->canvasPanelH)
        {
            if (ctx->canvasTarget)
            {
                SDL_DestroyTexture(ctx->canvasTarget);
                ctx->canvasTarget = NULL;
            }
            ctx->canvasTarget = wecanvas_createTarget(ctx->renderer, w, h);
            ctx->canvasPanelW = w;
            ctx->canvasPanelH = h;
        }

        // --- 4.5: display canvas texture via igImage ---
        if (ctx->canvasTarget)
        {
            ImTextureRef_c texRef;
            texRef._TexData = NULL;
            texRef._TexID   = (ImTextureID)(uintptr_t)ctx->canvasTarget;
            ImVec2_c imgSize = { (f32)w, (f32)h };
            ImVec2_c uv0     = { 0.0f, 0.0f };
            ImVec2_c uv1     = { 1.0f, 1.0f };
            igImage(texRef, imgSize, uv0, uv1);
        }

        // Record the canvas origin (top-left of the last-drawn item)
        ImVec2_c origin = igGetItemRectMin();

        // --- 4.6 + 4.7 + 4.8: input only when canvas is hovered ---
        ImGuiIO* io = igGetIO_Nil();
        if (igIsItemHovered(0))
        {
            // 4.6 — Camera pan with middle mouse button
            if (io->MouseDown[2])
            {
                f32 zoom = ctx->cameraZoom;
                ctx->cameraOffset.x -= io->MouseDelta.x / zoom;
                ctx->cameraOffset.y -= io->MouseDelta.y / zoom;

                // Clamp to map bounds
                f32 mapW = (f32)(MAP_TILES_WIDTH  * MEGA_TILE_WIDTH);
                f32 mapH = (f32)(MAP_TILES_HEIGHT * MEGA_TILE_HEIGHT);
                f32 maxX = mapW - (f32)w / zoom;
                f32 maxY = mapH - (f32)h / zoom;
                if (ctx->cameraOffset.x < 0.0f)   ctx->cameraOffset.x = 0.0f;
                if (ctx->cameraOffset.y < 0.0f)   ctx->cameraOffset.y = 0.0f;
                if (maxX > 0.0f && ctx->cameraOffset.x > maxX) ctx->cameraOffset.x = maxX;
                if (maxY > 0.0f && ctx->cameraOffset.y > maxY) ctx->cameraOffset.y = maxY;
            }

            // 4.7 — Zoom with mouse wheel
            if (io->MouseWheel != 0.0f)
            {
                f32 factor = (io->MouseWheel > 0.0f) ? 1.1f : (1.0f / 1.1f);
                ctx->cameraZoom *= factor;
                if (ctx->cameraZoom < 0.5f) ctx->cameraZoom = 0.5f;
                if (ctx->cameraZoom > 4.0f) ctx->cameraZoom = 4.0f;
            }

            // 4.8 — Tile coordinate in status bar
            if (statusBuf && statusBufLen > 0)
            {
                vec2 screenPos = vec2f(io->MousePos.x, io->MousePos.y);
                vec2 canvasOrigin = vec2f(origin.x, origin.y);
                vec2 mapPos  = wecanvas_screenToMap(ctx, screenPos, canvasOrigin);
                vec2 tilePos = wecanvas_mapToTile(mapPos);
                SDL_snprintf(statusBuf, (size_t)statusBufLen,
                             "Tile (%d, %d)",
                             (int)tilePos.x, (int)tilePos.y);
            }
        }
    }
    igEnd();
    igPopStyleVar(1);
}

// ---------------------------------------------------------------------------
// Coordinate helpers
// ---------------------------------------------------------------------------

// Convert an absolute screen pixel position to map-space pixels.
vec2 wecanvas_screenToMap(WarEditorContext* ctx, vec2 screenPos, vec2 canvasOrigin)
{
    // 1. Relative to canvas top-left
    f32 relX = screenPos.x - canvasOrigin.x;
    f32 relY = screenPos.y - canvasOrigin.y;

    // 2. Divide by zoom and add camera offset to get map pixels
    f32 mapX = relX / ctx->cameraZoom + ctx->cameraOffset.x;
    f32 mapY = relY / ctx->cameraZoom + ctx->cameraOffset.y;

    return vec2f(mapX, mapY);
}

// Convert map-space pixels to tile (column, row) — truncating integers.
vec2 wecanvas_mapToTile(vec2 mapPos)
{
    s32 tx = (s32)(mapPos.x / MEGA_TILE_WIDTH);
    s32 ty = (s32)(mapPos.y / MEGA_TILE_HEIGHT);

    // Clamp to valid tile range
    if (tx < 0) tx = 0;
    if (ty < 0) ty = 0;
    if (tx >= MAP_TILES_WIDTH)  tx = MAP_TILES_WIDTH  - 1;
    if (ty >= MAP_TILES_HEIGHT) ty = MAP_TILES_HEIGHT - 1;

    return vec2f((f32)tx, (f32)ty);
}
