#include "war_editor_canvas.h"
#include "war_editor_tools.h"

// ---------------------------------------------------------------------------
// Phase 6 — Sprite texture cache
//
// Unit sprites are global DATA.WAR assets that do not vary between map
// imports; the cache is built lazily and lives for the entire editor session.
// ---------------------------------------------------------------------------

#define WECANVAS_SPRITE_CACHE_MAX 64

typedef struct
{
    s32          resourceIndex;
    SDL_Texture* texture;
} WeSpriteEntry;

static WeSpriteEntry s_spriteCache[WECANVAS_SPRITE_CACHE_MAX];
static s32           s_spriteCacheCount = 0;

// Decode frame 0 of the sprite resource at resourceIndex and upload it as an
// SDL_Texture.  Returns NULL on any validation or GPU error.
// All unit sprites in Warcraft I use palette pair (191, 217).
static SDL_Texture* wecanvas_decodeSpriteTexture(WarEditorContext* ctx, s32 resourceIndex)
{
    if (!ctx->warFile || resourceIndex <= 0 || resourceIndex >= MAX_RESOURCES_COUNT)
        return NULL;

    WarRawResource* raw = &ctx->warFile->resources[resourceIndex];
    if (raw->placeholder || !raw->data || raw->length < 12)
        return NULL;

    // Ensure both palette resources are decoded (no-op if already loaded).
    wemap_loadPaletteRes(ctx, 191);
    wemap_loadPaletteRes(ctx, 217);

    u8 palette[PALETTE_LENGTH];
    wemap_getPalette(ctx, 191, 217, palette);

    // --- Sprite header (bytes 0-3) ---
    u16 framesCount = readu16(raw->data, 0);
    u8  frameW      = readu8(raw->data, 2);
    u8  frameH      = readu8(raw->data, 3);
    if (framesCount == 0 || frameW == 0 || frameH == 0)
        return NULL;

    // --- Frame 0 descriptor (bytes 4-11) ---
    u8  dx  = readu8(raw->data,  4);
    u8  dy  = readu8(raw->data,  5);
    u8  fw  = readu8(raw->data,  6);
    u8  fh  = readu8(raw->data,  7);
    u32 off = readu32(raw->data, 8);

    // --- Decode palette-indexed pixels to RGBA32 ---
    u32 pixelBytes = (u32)frameW * (u32)frameH * 4u;
    u8* pixels     = (u8*)wm_alloc(pixelBytes);
    memset(pixels, 0, pixelBytes);

    for (s32 y = (s32)dy; y < (s32)dy + (s32)fh; y++)
    {
        for (s32 x = (s32)dx; x < (s32)dx + (s32)fw; x++)
        {
            u32 p  = (u32)(x + y * (s32)frameW) * 4u;
            u32 ci = raw->data[off++];
            pixels[p + 0] = palette[ci * 3 + 0];
            pixels[p + 1] = palette[ci * 3 + 1];
            pixels[p + 2] = palette[ci * 3 + 2];
            pixels[p + 3] = (pixels[p] || pixels[p+1] || pixels[p+2] || ci) ? 255u : 0u;
        }
    }

    // --- Upload to GPU ---
    SDL_Texture* tex = SDL_CreateTexture(
        ctx->renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC,
        (int)frameW, (int)frameH);

    if (!tex)
    {
        logWarning("wecanvas_decodeSpriteTexture: SDL_CreateTexture(%dx%d) failed: %s",
                   (int)frameW, (int)frameH, SDL_GetError());
        return NULL;
    }

    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(tex, NULL, pixels, (int)frameW * 4);
    return tex;
}

// Returns the cached SDL_Texture for resourceIndex, decoding it on first call.
// Caches NULL results to avoid retrying a failed decode every frame.
// Non-static: also called from war_editor_ui.c for palette thumbnails.
SDL_Texture* wecanvas_getSpriteTexture(WarEditorContext* ctx, s32 resourceIndex)
{
    // O(n) cache lookup — sprite count is small (≤ 50 unit types)
    for (s32 i = 0; i < s_spriteCacheCount; i++)
    {
        if (s_spriteCache[i].resourceIndex == resourceIndex)
            return s_spriteCache[i].texture;
    }

    SDL_Texture* tex = wecanvas_decodeSpriteTexture(ctx, resourceIndex);

    if (s_spriteCacheCount < WECANVAS_SPRITE_CACHE_MAX)
    {
        s_spriteCache[s_spriteCacheCount].resourceIndex = resourceIndex;
        s_spriteCache[s_spriteCacheCount].texture       = tex;
        s_spriteCacheCount++;
    }
    return tex;
}

// ---------------------------------------------------------------------------
// Phase 6 — Render start entities (units, buildings) and goldmines
//
// For each entity the sprite frame is centered over the unit's tile footprint,
// mirroring the wr_translate sequence in renderUnit():
//   net offset = (tileX*16, tileY*16) - frameSize/2 + unitSize/2
//
// A small player-color chip is drawn at the unit's tile origin so that
// player assignment is visible even when sprites overlap.
// ---------------------------------------------------------------------------
static void wecanvas_renderEntities(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx->map;
    if (!m) return;

    f32 zoom = ctx->cameraZoom;
    f32 camX = ctx->cameraOffset.x;
    f32 camY = ctx->cameraOffset.y;
    f32 cw   = (f32)ctx->canvasPanelW;
    f32 ch   = (f32)ctx->canvasPanelH;

    // Player color chips — (R, G, B) matching WAR_COLOR_* constants
    static const u8 playerColors[5][3] =
    {
        {   0,   0, 199 },  // player 0: blue
        { 199,   0,   0 },  // player 1: red
        {   0, 199,   0 },  // player 2: green
        { 199, 199,   0 },  // player 3: yellow
        { 199, 199, 199 },  // player 4: white
    };

    // Two passes: campaign start entities, then custom-map goldmines
    for (s32 pass = 0; pass < 2; pass++)
    {
        u32          count = (pass == 0) ? m->startEntitiesCount : m->startGoldminesCount;
        WarLevelUnit* arr  = (pass == 0) ? m->startEntities     : m->startGoldmines;

        for (u32 i = 0; i < count; i++)
        {
            WarLevelUnit*      lu = &arr[i];
            const WarUnitData* ud = wu_getUnitData(lu->type);
            if (!ud || ud->resourceIndex <= 0)
                continue;

            SDL_Texture* tex = wecanvas_getSpriteTexture(ctx, ud->resourceIndex);
            if (!tex) continue;

            float texW, texH;
            if (!SDL_GetTextureSize(tex, &texW, &texH)) continue;

            // Center the sprite frame over the unit's tile footprint
            f32 unitW = (f32)(ud->sizex * MEGA_TILE_WIDTH);
            f32 unitH = (f32)(ud->sizey * MEGA_TILE_HEIGHT);
            f32 mapX  = (f32)(lu->x * MEGA_TILE_WIDTH)  - texW * 0.5f + unitW * 0.5f;
            f32 mapY  = (f32)(lu->y * MEGA_TILE_HEIGHT) - texH * 0.5f + unitH * 0.5f;

            f32 dstX = (mapX - camX) * zoom;
            f32 dstY = (mapY - camY) * zoom;
            f32 dstW = texW * zoom;
            f32 dstH = texH * zoom;

            // Frustum cull
            if (dstX + dstW < 0.0f || dstX >= cw) continue;
            if (dstY + dstH < 0.0f || dstY >= ch) continue;

            SDL_FRect dst = { dstX, dstY, dstW, dstH };
            SDL_RenderTexture(ctx->renderer, tex, NULL, &dst);

            // Player color chip at the tile origin (top-left corner)
            u8 player = (lu->player < 5) ? lu->player : 0u;
            SDL_SetRenderDrawColor(ctx->renderer,
                playerColors[player][0],
                playerColors[player][1],
                playerColors[player][2],
                255);
            f32 chipMapX = (f32)(lu->x * MEGA_TILE_WIDTH);
            f32 chipMapY = (f32)(lu->y * MEGA_TILE_HEIGHT);
            f32 chipX    = (chipMapX - camX) * zoom;
            f32 chipY    = (chipMapY - camY) * zoom;
            f32 chipSz   = 3.0f * zoom;
            if (chipSz < 1.0f) chipSz = 1.0f;
            SDL_FRect chip = { chipX, chipY, chipSz, chipSz };
            SDL_RenderFillRect(ctx->renderer, &chip);
        }
    }
}

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
// 8.4 — Ghost preview of the selected unit type at the hover tile.
// Renders with 50% alpha; red tint + fill when position is already occupied.
// Must be called while ctx->canvasTarget is the active render target.
// ---------------------------------------------------------------------------
void wecanvas_renderEntityGhost(WarEditorContext* ctx)
{
    if (ctx->activeTool != WE_TOOL_PLACE_ENTITY) return;
    if (!ctx->isHoveringCanvas) return;

    WarEditorMap* m = ctx->map;
    if (!m) return;

    const WarUnitData* ud = wu_getUnitData(ctx->selectedUnitType);
    if (!ud || ud->resourceIndex <= 0) return;

    SDL_Texture* tex = wecanvas_getSpriteTexture(ctx, ud->resourceIndex);
    if (!tex) return;

    float texW, texH;
    if (!SDL_GetTextureSize(tex, &texW, &texH)) return;

    s32 tx = ctx->hoverTx;
    s32 ty = ctx->hoverTy;

    f32 unitW = (f32)(ud->sizex * MEGA_TILE_WIDTH);
    f32 unitH = (f32)(ud->sizey * MEGA_TILE_HEIGHT);
    f32 mapX  = (f32)(tx * MEGA_TILE_WIDTH)  - texW * 0.5f + unitW * 0.5f;
    f32 mapY  = (f32)(ty * MEGA_TILE_HEIGHT) - texH * 0.5f + unitH * 0.5f;

    f32 zoom = ctx->cameraZoom;
    f32 camX = ctx->cameraOffset.x;
    f32 camY = ctx->cameraOffset.y;

    f32 dstX = (mapX - camX) * zoom;
    f32 dstY = (mapY - camY) * zoom;
    f32 dstW = texW * zoom;
    f32 dstH = texH * zoom;

    bool occupied = !wetools_canPlace(ctx, tx, ty, ud->sizex, ud->sizey);

    // 50% alpha preview; red tint when blocked
    SDL_SetTextureAlphaMod(tex, 128);
    if (occupied)
        SDL_SetTextureColorMod(tex, 255, 60, 60);

    SDL_FRect dst = { dstX, dstY, dstW, dstH };
    SDL_RenderTexture(ctx->renderer, tex, NULL, &dst);

    // Restore texture state
    SDL_SetTextureAlphaMod(tex, 255);
    if (occupied)
        SDL_SetTextureColorMod(tex, 255, 255, 255);

    // Semi-transparent red fill over the footprint when blocked
    if (occupied)
    {
        SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 80);
        f32 fx = ((f32)(tx * MEGA_TILE_WIDTH)  - camX) * zoom;
        f32 fy = ((f32)(ty * MEGA_TILE_HEIGHT) - camY) * zoom;
        f32 fw = (f32)(ud->sizex * MEGA_TILE_WIDTH)  * zoom;
        f32 fh = (f32)(ud->sizey * MEGA_TILE_HEIGHT) * zoom;
        SDL_FRect fill = { fx, fy, fw, fh };
        SDL_RenderFillRect(ctx->renderer, &fill);
        SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);
    }
}

// ---------------------------------------------------------------------------
// 9.3 — Yellow selection border for every entity in ctx->selectedEntities.
// Applies move-drag offset while a move drag is in progress.
// Must be called while ctx->canvasTarget is the active render target.
// ---------------------------------------------------------------------------
void wecanvas_renderSelection(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx->map;
    if (!m) return;
    if (ctx->selectedEntities.count == 0) return;

    f32 zoom = ctx->cameraZoom;
    f32 camX = ctx->cameraOffset.x;
    f32 camY = ctx->cameraOffset.y;

    s32 dtx = 0, dty = 0;
    wetools_getMoveDelta(ctx, &dtx, &dty);

    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
    // Gold: (255, 215, 0, 220)
    SDL_SetRenderDrawColor(ctx->renderer, 255, 215, 0, 220);

    for (s32 i = 0; i < ctx->selectedEntities.count; i++)
    {
        WarEntityId id = ctx->selectedEntities.items[i];

        s32 tx, ty, sizeX, sizeY;

        if (id & 0x8000u)
        {
            u32 idx = (u32)(id & 0x7FFFu);
            if (idx >= m->startGoldminesCount) continue;
            WarLevelUnit*      lu = &m->startGoldmines[idx];
            const WarUnitData* ud = wu_getUnitData(lu->type);
            if (!ud) continue;
            tx    = (s32)lu->x;
            ty    = (s32)lu->y;
            sizeX = ud->sizex;
            sizeY = ud->sizey;
        }
        else
        {
            u32 idx = (u32)(id - 1u);
            if (idx >= m->startEntitiesCount) continue;
            WarLevelUnit*      lu = &m->startEntities[idx];
            const WarUnitData* ud = wu_getUnitData(lu->type);
            if (!ud) continue;
            tx    = (s32)lu->x;
            ty    = (s32)lu->y;
            sizeX = ud->sizex;
            sizeY = ud->sizey;
        }

        tx += dtx;
        ty += dty;

        f32 rx = ((f32)(tx * MEGA_TILE_WIDTH)  - camX) * zoom;
        f32 ry = ((f32)(ty * MEGA_TILE_HEIGHT) - camY) * zoom;
        f32 rw = (f32)(sizeX * MEGA_TILE_WIDTH)  * zoom;
        f32 rh = (f32)(sizeY * MEGA_TILE_HEIGHT) * zoom;

        SDL_FRect r = { rx, ry, rw, rh };
        SDL_RenderRect(ctx->renderer, &r);
    }

    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);
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
    SDL_SetRenderDrawColor(ctx->renderer, 40, 40, 40, 255);
    SDL_RenderClear(ctx->renderer);

    wecanvas_renderTerrain(ctx);
    wecanvas_renderEntities(ctx);

    if (ctx->showPassability)
        wecanvas_renderPassability(ctx);

    if (ctx->showGrid)
        wecanvas_renderGrid(ctx);

    wecanvas_renderEntityGhost(ctx);
    wecanvas_renderSelection(ctx);

    // Reset to default render target (the window framebuffer)
    SDL_SetRenderTarget(ctx->renderer, NULL);
}

// ---------------------------------------------------------------------------
// 7.6 — Semi-transparent passability overlay
// ---------------------------------------------------------------------------
void wecanvas_renderPassability(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx->map;
    if (!m) return;

    f32 zoom = ctx->cameraZoom;
    f32 camX = ctx->cameraOffset.x;
    f32 camY = ctx->cameraOffset.y;
    f32 cw   = (f32)ctx->canvasPanelW;
    f32 ch   = (f32)ctx->canvasPanelH;
    f32 dstW = (f32)MEGA_TILE_WIDTH  * zoom;
    f32 dstH = (f32)MEGA_TILE_HEIGHT * zoom;

    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 80);

    for (s32 ty = 0; ty < MAP_TILES_HEIGHT; ty++)
    {
        for (s32 tx = 0; tx < MAP_TILES_WIDTH; tx++)
        {
            if (m->passableData[ty * MAP_TILES_WIDTH + tx] == 0)
                continue;

            f32 dstX = ((f32)(tx * MEGA_TILE_WIDTH)  - camX) * zoom;
            f32 dstY = ((f32)(ty * MEGA_TILE_HEIGHT) - camY) * zoom;

            if (dstX + dstW < 0.0f || dstX >= cw) continue;
            if (dstY + dstH < 0.0f || dstY >= ch) continue;

            SDL_FRect r = { dstX, dstY, dstW, dstH };
            SDL_RenderFillRect(ctx->renderer, &r);
        }
    }

    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);
}

// ---------------------------------------------------------------------------
// 7.7 — Grid overlay
// ---------------------------------------------------------------------------
void wecanvas_renderGrid(WarEditorContext* ctx)
{
    f32 zoom = ctx->cameraZoom;
    f32 camX = ctx->cameraOffset.x;
    f32 camY = ctx->cameraOffset.y;
    f32 cw   = (f32)ctx->canvasPanelW;
    f32 ch   = (f32)ctx->canvasPanelH;

    // Map extent in canvas-relative pixels.
    f32 mapLeft   = (            -camX) * zoom;
    f32 mapRight  = ((f32)(MAP_TILES_WIDTH  * MEGA_TILE_WIDTH)  - camX) * zoom;
    f32 mapTop    = (            -camY) * zoom;
    f32 mapBottom = ((f32)(MAP_TILES_HEIGHT * MEGA_TILE_HEIGHT) - camY) * zoom;

    // Clamp endpoints to the intersection of canvas area and map extent.
    f32 lineX0 = mapLeft   > 0.0f ? mapLeft   : 0.0f;
    f32 lineX1 = mapRight  < cw   ? mapRight  : cw;
    f32 lineY0 = mapTop    > 0.0f ? mapTop    : 0.0f;
    f32 lineY1 = mapBottom < ch   ? mapBottom : ch;

    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 55);

    // Vertical lines (tile column boundaries) — span only the map's Y extent.
    for (s32 tx = 0; tx <= MAP_TILES_WIDTH; tx++)
    {
        f32 x = ((f32)(tx * MEGA_TILE_WIDTH) - camX) * zoom;
        if (x < 0.0f || x >= cw) continue;
        SDL_RenderLine(ctx->renderer, x, lineY0, x, lineY1);
    }

    // Horizontal lines (tile row boundaries) — span only the map's X extent.
    for (s32 ty = 0; ty <= MAP_TILES_HEIGHT; ty++)
    {
        f32 y = ((f32)(ty * MEGA_TILE_HEIGHT) - camY) * zoom;
        if (y < 0.0f || y >= ch) continue;
        SDL_RenderLine(ctx->renderer, lineX0, y, lineX1, y);
    }

    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);
}

// ---------------------------------------------------------------------------
// 4.5 + 4.6 + 4.7 + 4.8 — cimgui canvas panel
// ---------------------------------------------------------------------------
void wecanvas_drawPanel(WarEditorContext* ctx, char* statusBuf, s32 statusBufLen)
{
    // Reset hover state each frame; set below if hovered this frame.
    ctx->isHoveringCanvas = false;

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
        vec2 canvasOrigin = vec2f(origin.x, origin.y);

        // 7.4 — Fill-tool preview overlay (drawn into the window's draw list)
        wetools_drawOverlay(ctx, igGetWindowDrawList(), canvasOrigin);

        // --- 4.6 + 4.7 + 4.8 + 7.3/7.4/7.5 + 7.10: input when canvas is hovered ---
        ImGuiIO* io = igGetIO_Nil();
        if (igIsItemHovered(0))
        {
            // 4.6 — Camera pan with middle mouse button
            if (io->MouseDown[2])
            {
                f32 zoom = ctx->cameraZoom;
                ctx->cameraOffset.x -= io->MouseDelta.x / zoom;
                ctx->cameraOffset.y -= io->MouseDelta.y / zoom;
            }

            // 4.7 — Zoom with mouse wheel, anchored to pointer position
            if (io->MouseWheel != 0.0f)
            {
                f32 oldZoom = ctx->cameraZoom;
                f32 factor  = (io->MouseWheel > 0.0f) ? 1.1f : (1.0f / 1.1f);
                f32 newZoom = oldZoom * factor;
                if (newZoom < 0.5f) newZoom = 0.5f;
                if (newZoom > 4.0f) newZoom = 4.0f;

                // Compute the map-space point under the mouse, then adjust
                // cameraOffset so that point stays under the mouse after zoom.
                f32 mx = io->MousePos.x - canvasOrigin.x;
                f32 my = io->MousePos.y - canvasOrigin.y;
                ctx->cameraOffset.x = mx / oldZoom + ctx->cameraOffset.x - mx / newZoom;
                ctx->cameraOffset.y = my / oldZoom + ctx->cameraOffset.y - my / newZoom;
                ctx->cameraZoom = newZoom;
            }

            // Compute tile under the cursor (used by status bar, tool input, and tooltip)
            vec2 screenPos = vec2f(io->MousePos.x, io->MousePos.y);
            vec2 mapPos    = wecanvas_screenToMap(ctx, screenPos, canvasOrigin);
            vec2 tilePos   = wecanvas_mapToTile(mapPos);
            s32  tx        = (s32)tilePos.x;
            s32  ty        = (s32)tilePos.y;

            // 8.3 — Update hover state so ghost/selection renderers can read it.
            ctx->isHoveringCanvas = true;
            ctx->hoverTx          = tx;
            ctx->hoverTy          = ty;

            // 4.8 — Tile coordinate in status bar
            if (statusBuf && statusBufLen > 0)
            {
                SDL_snprintf(statusBuf, (size_t)statusBufLen,
                             "Tile (%d, %d)", tx, ty);
            }

            // 7.3/7.4/7.5 — Dispatch to active tool (only when not panning)
            if (!io->MouseDown[2])
                wetools_handleInput(ctx, tx, ty, io, canvasOrigin);

            // 7.10 — Tile hover tooltip
            if (ctx->map && igBeginTooltip())
            {
                u16 tileIdx  = ctx->map->visualData[ty  * MAP_TILES_WIDTH + tx];
                u16 passable = ctx->map->passableData[ty * MAP_TILES_WIDTH + tx];
                igText("Tile 0x%04X", (u32)tileIdx);
                igText("%s", passable != 0 ? "Blocked" : "Passable");
                igEndTooltip();
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
