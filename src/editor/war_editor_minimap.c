#include "war_editor_minimap.h"

#include "war_editor_map.h"

#define WEMINIMAP_SIZE 64
#define WEMINIMAP_SCALE 2.0f

static bool s_minimapDragActive = false;

static u8 weminimap_tileColorIndex(WarEditorContext* ctx, WarEditorMap* m, u16 tileIndex)
{
    if (!ctx || !m)
    {
        return 0;
    }

    WarResource* tilesRes = ctx->resources[m->tilesIndex];
    if (!tilesRes || tilesRes->type != WAR_RESOURCE_TYPE_TILES)
    {
        return 0;
    }

    u32 base = (u32)tileIndex * 8u;
    u16 miniOffset = readu16(tilesRes->tilesData.data, base + 0);
    miniOffset = (u16)((miniOffset & 0xFFFCu) << 1);
    return tilesRes->tilesData.data[miniOffset];
}

void weminimap_markDirty(WarEditorContext* ctx)
{
    if (ctx)
    {
        ctx->minimapDirty = true;
    }
}

bool weminimap_create(WarEditorContext* ctx)
{
    if (!ctx || !ctx->renderer)
    {
        return false;
    }

    if (ctx->minimapTexture)
    {
        return true;
    }

    ctx->minimapTexture = SDL_CreateTexture(ctx->renderer,
                                            SDL_PIXELFORMAT_RGBA32,
                                            SDL_TEXTUREACCESS_TARGET,
                                            WEMINIMAP_SIZE,
                                            WEMINIMAP_SIZE);
    if (!ctx->minimapTexture)
    {
        logWarning("weminimap_create: failed to create minimap texture: %s", SDL_GetError());
        return false;
    }

    SDL_SetTextureScaleMode(ctx->minimapTexture, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(ctx->minimapTexture, SDL_BLENDMODE_BLEND);
    return true;
}

void weminimap_rebuild(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx ? ctx->map : NULL;
    if (!ctx || !m)
    {
        return;
    }

    if (!weminimap_create(ctx))
    {
        return;
    }

    // Ensure palette+tiles resources exist for color sampling.
    wemap_loadPaletteRes(ctx, (s32)m->paletteIndex);
    wemap_loadPaletteRes(ctx, 217);
    wemap_loadTilesRes(ctx, (s32)m->tilesIndex, (s32)m->paletteIndex, 217);

    u8 palette[PALETTE_LENGTH];
    wemap_getPalette(ctx, (s32)m->paletteIndex, 217, palette);

    SDL_SetRenderTarget(ctx->renderer, ctx->minimapTexture);
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(ctx->renderer);

    for (s32 ty = 0; ty < MAP_TILES_HEIGHT; ty++)
    {
        for (s32 tx = 0; tx < MAP_TILES_WIDTH; tx++)
        {
            u16 tile = m->visualData[ty * MAP_TILES_WIDTH + tx];
            u8 colorIndex = weminimap_tileColorIndex(ctx, m, tile);
            u8 r = palette[colorIndex * 3 + 0];
            u8 g = palette[colorIndex * 3 + 1];
            u8 b = palette[colorIndex * 3 + 2];

            SDL_SetRenderDrawColor(ctx->renderer, r, g, b, 255);
            SDL_RenderPoint(ctx->renderer, (float)tx, (float)ty);
        }
    }

    static const u8 s_playerColors[5][3] =
    {
        { 0,   0,   199 },
        { 199, 0,   0   },
        { 0,   199, 0   },
        { 199, 199, 0   },
        { 199, 199, 199 }
    };

    for (u32 i = 0; i < m->startEntitiesCount; i++)
    {
        WarLevelUnit* lu = &m->startEntities[i];
        u8 player = (lu->player < 5) ? lu->player : 0u;
        SDL_SetRenderDrawColor(ctx->renderer,
                               s_playerColors[player][0],
                               s_playerColors[player][1],
                               s_playerColors[player][2],
                               255);
        SDL_RenderPoint(ctx->renderer, (float)lu->x, (float)lu->y);
    }

    for (u32 i = 0; i < m->startGoldminesCount; i++)
    {
        WarLevelUnit* lu = &m->startGoldmines[i];
        SDL_SetRenderDrawColor(ctx->renderer, 255, 215, 0, 255);
        SDL_RenderPoint(ctx->renderer, (float)lu->x, (float)lu->y);
    }

    SDL_SetRenderTarget(ctx->renderer, NULL);

    ctx->minimapDirty = false;
    ctx->minimapLastRebuildTime = ctx->time;
}

void weminimap_drawPanel(WarEditorContext* ctx)
{
    if (!ctx || !ctx->showMinimap)
    {
        s_minimapDragActive = false;
        return;
    }

    if (!igBegin("Minimap##minimap", NULL, ImGuiWindowFlags_None))
    {
        s_minimapDragActive = false;
        igEnd();
        return;
    }

    if (!ctx->map)
    {
        s_minimapDragActive = false;
        igTextDisabled("No map loaded.");
        igEnd();
        return;
    }

    if (!ctx->minimapTexture)
    {
        weminimap_create(ctx);
        ctx->minimapDirty = true;
    }

    if (ctx->minimapDirty && (ctx->time - ctx->minimapLastRebuildTime >= 1.0f))
    {
        weminimap_rebuild(ctx);
    }

    ImTextureRef_c texRef;
    texRef._TexData = NULL;
    texRef._TexID = (ImTextureID)(uintptr_t)ctx->minimapTexture;

    ImVec2_c avail = igGetContentRegionAvail();

    ImVec2_c imageSize = { WEMINIMAP_SIZE * WEMINIMAP_SCALE, WEMINIMAP_SIZE * WEMINIMAP_SCALE };

    // Center the minimap in the available region.
    ImVec2_c startPos = igGetCursorPos();
    f32 cx = startPos.x + (avail.x - imageSize.x) * 0.5f;
    f32 cy = startPos.y + (avail.y - imageSize.y) * 0.5f;
    igSetCursorPos((ImVec2_c){cx, cy});

    ImVec2_c imagePos = igGetCursorScreenPos();
    igImage(texRef, imageSize, (ImVec2_c){0.0f, 0.0f}, (ImVec2_c){1.0f, 1.0f});

    ImDrawList* dl = igGetWindowDrawList();

    // Draw viewport rectangle.
    f32 mapViewW = ((f32)ctx->canvasPanelW / ctx->cameraZoom) / (f32)MEGA_TILE_WIDTH;
    f32 mapViewH = ((f32)ctx->canvasPanelH / ctx->cameraZoom) / (f32)MEGA_TILE_HEIGHT;
    f32 mapViewX = ctx->cameraOffset.x / (f32)MEGA_TILE_WIDTH;
    f32 mapViewY = ctx->cameraOffset.y / (f32)MEGA_TILE_HEIGHT;

    f32 rx0 = imagePos.x + mapViewX * WEMINIMAP_SCALE;
    f32 ry0 = imagePos.y + mapViewY * WEMINIMAP_SCALE;
    f32 rx1 = rx0 + mapViewW * WEMINIMAP_SCALE;
    f32 ry1 = ry0 + mapViewH * WEMINIMAP_SCALE;

    ImDrawList_AddRect(dl,
                       (ImVec2_c){ rx0, ry0 },
                       (ImVec2_c){ rx1, ry1 },
                       0xFFFFFFFFu,
                       0.0f, 1.5f, 0);

    ImGuiIO* io = igGetIO_Nil();

    if (igIsItemHovered(0) && io->MouseClicked[0])
    {
        s_minimapDragActive = true;
    }

    if (!io->MouseDown[0])
    {
        s_minimapDragActive = false;
    }

    // Click-to-navigate or drag. Continue while mouse is held so movement
    // does not stop when the pointer leaves the minimap widget.
    if (s_minimapDragActive)
    {
        ImVec2_c mp = igGetMousePos();
        f32 localX = mp.x - imagePos.x;
        f32 localY = mp.y - imagePos.y;

        f32 centerTileX = localX / WEMINIMAP_SCALE;
        f32 centerTileY = localY / WEMINIMAP_SCALE;

        f32 viewMapW = (f32)ctx->canvasPanelW / ctx->cameraZoom;
        f32 viewMapH = (f32)ctx->canvasPanelH / ctx->cameraZoom;

        ctx->cameraOffset.x = centerTileX * MEGA_TILE_WIDTH - viewMapW * 0.5f;
        ctx->cameraOffset.y = centerTileY * MEGA_TILE_HEIGHT - viewMapH * 0.5f;
    }

    igEnd();
}
