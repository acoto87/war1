#include "war_editor_ui.h"
#include "war_editor.h"
#include "war_editor_canvas.h"
#include "war_editor_map.h"
#include "war_editor_tools.h"

// Module-level storage so weui_beginFrame can access the window without
// requiring it as a parameter (matching the void signature in the spec).
static SDL_Window*   s_window   = NULL;
static SDL_Renderer* s_renderer = NULL;

// -----------------------------------------------------------------------
// Import Campaign Level dialog state
// -----------------------------------------------------------------------
static bool s_showImportDialog  = false;
static s32  s_importDbArrayIdx  = -1;   // index into assets[] of current selection

// Draw the "Import Campaign Level" modal popup.
// Opens when s_showImportDialog is true; calls wemap_importFromLevelInfo on confirm.
static void weui_drawImportDialog(WarEditorContext* ctx)
{
    if (s_showImportDialog)
    {
        igOpenPopup_Str("Import Campaign Level##modal", 0);
        s_showImportDialog = false;

        // Default selection: first DB_ENTRY_TYPE_LEVEL_INFO entry.
        if (s_importDbArrayIdx < 0)
        {
            for (s32 i = 0; i < (s32)arrayLength(assets); ++i)
            {
                if (assets[i].type == DB_ENTRY_TYPE_LEVEL_INFO)
                {
                    s_importDbArrayIdx = i;
                    break;
                }
            }
        }
    }

    // Centre the popup in the display.
    ImGuiViewport* vp = igGetMainViewport();
    ImVec2 centre;
    centre.x = vp->Pos.x + vp->Size.x * 0.5f;
    centre.y = vp->Pos.y + vp->Size.y * 0.5f;
    igSetNextWindowPos(centre, ImGuiCond_Appearing, (ImVec2){0.5f, 0.5f});
    igSetNextWindowSize((ImVec2){420, 480}, ImGuiCond_Appearing);

    if (igBeginPopupModal("Import Campaign Level##modal", NULL, ImGuiWindowFlags_None))
    {
        igText("Select a level to import into the editor:");
        igSeparator();

        // Scrollable list of all DB_ENTRY_TYPE_LEVEL_INFO entries.
        igBeginChild_Str("##levellist",
                         (ImVec2){0.0f, -40.0f},
                         ImGuiChildFlags_Borders,
                         ImGuiWindowFlags_None);

        for (s32 i = 0; i < (s32)arrayLength(assets); ++i)
        {
            if (assets[i].type != DB_ENTRY_TYPE_LEVEL_INFO)
                continue;

            bool selected = (s_importDbArrayIdx == i);
            if (igSelectable_Bool(assets[i].name, selected, 0, (ImVec2){0, 0}))
                s_importDbArrayIdx = i;
        }
        igEndChild();

        // Buttons
        if (igButton("Import", (ImVec2){80.0f, 0.0f}))
        {
            if (s_importDbArrayIdx >= 0)
            {
                s32 resIndex = assets[s_importDbArrayIdx].index;
                if (wemap_importFromLevelInfo(ctx, resIndex))
                {
                    ctx->unsavedChanges    = true;
                    ctx->currentFilePath[0] = '\0';
                    snprintf(ctx->statusText, sizeof(ctx->statusText),
                             "Imported: %s", assets[s_importDbArrayIdx].name);
                }
                else
                {
                    snprintf(ctx->statusText, sizeof(ctx->statusText),
                             "Import failed: %s", assets[s_importDbArrayIdx].name);
                }
            }
            igCloseCurrentPopup();
        }

        igSameLine(0.0f, 10.0f);

        if (igButton("Cancel", (ImVec2){80.0f, 0.0f}))
            igCloseCurrentPopup();

        igEndPopup();
    }
}

// -----------------------------------------------------------------------
// Toolbox panel (7.2) — vertical button strip; sets ctx->activeTool.
// -----------------------------------------------------------------------
static void weui_drawToolboxPanel(WarEditorContext* ctx)
{
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    if (igBegin("Toolbox##toolbox", NULL, flags))
    {
        static const struct { WarEditorToolType type; const char* label; } tools[] =
        {
            { WE_TOOL_SELECT,       "Select" },
            { WE_TOOL_PENCIL,       "Pencil" },
            { WE_TOOL_FILL,         "Fill"   },
            { WE_TOOL_ERASE,        "Erase"  },
            { WE_TOOL_PLACE_ENTITY, "Entity" },
        };
        s32 toolCount = (s32)(sizeof(tools) / sizeof(tools[0]));

        ImVec2_c btnSize;
        btnSize.x = igGetContentRegionAvail().x;
        btnSize.y = 0.0f;

        for (s32 i = 0; i < toolCount; i++)
        {
            bool active = (ctx->activeTool == tools[i].type);
            if (active)
            {
                // Highlight the active tool in gold
                igPushStyleColor_Vec4(ImGuiCol_Button,
                    (ImVec4_c){ 0.80f, 0.60f, 0.10f, 1.0f });
                igPushStyleColor_Vec4(ImGuiCol_ButtonHovered,
                    (ImVec4_c){ 0.90f, 0.70f, 0.20f, 1.0f });
            }

            if (igButton(tools[i].label, btnSize))
                ctx->activeTool = tools[i].type;

            if (active)
                igPopStyleColor(2);
        }
    }
    igEnd();
}

// -----------------------------------------------------------------------
// Tile palette panel (7.8 + 7.9) — grid of tileset thumbnails + tileset
// type combo box.  Clicking a tile sets ctx->selectedTileIndex.
// -----------------------------------------------------------------------

// Tile zoom: base size 24 px, range [0.5×, 4.0×], step 0.25×.
static f32 s_tileZoom = 1.0f;

static void weui_drawTilePalettePanel(WarEditorContext* ctx)
{
    if (igBegin("Tiles##tiles", NULL, ImGuiWindowFlags_None))
    {
        // 7.9 — Tileset type combo box
        s32 currentType = (s32)((ctx->map) ? ctx->map->tilesetType : MAP_TILESET_FOREST);
        if (igCombo_Str("Tileset", &currentType, "Forest\0Swamp\0Dungeon\0", 3))
        {
            if (ctx->map)
            {
                ctx->map->tilesetType = (u16)currentType;
                wemap_buildTerrainSprite(ctx);
            }
        }

        // Zoom controls
        if (igButton("-##zoom", (ImVec2_c){ 24.0f, 0.0f }))
            s_tileZoom = fmaxf(0.5f, s_tileZoom - 0.25f);
        igSameLine(0.0f, 4.0f);
        igText("Zoom: %.2fx", (double)s_tileZoom);
        igSameLine(0.0f, 4.0f);
        if (igButton("+##zoom", (ImVec2_c){ 24.0f, 0.0f }))
            s_tileZoom = fminf(4.0f, s_tileZoom + 0.25f);

        igSeparator();

        // 7.8 — Tile thumbnail grid
        WarEditorMap* m = ctx->map;
        if (m && m->terrainSprite.texture)
        {
            ImTextureRef_c texRef;
            texRef._TexData = NULL;
            texRef._TexID   = (ImTextureID)(uintptr_t)m->terrainSprite.texture;

            // Tile display size driven by zoom; base = 24×24 px.
            f32      tileSize    = 24.0f * s_tileZoom;
            ImVec2_c displaySize = { tileSize, tileSize };

            f32 tileUW = (f32)MEGA_TILE_WIDTH  / (f32)TILESET_WIDTH;
            f32 tileUH = (f32)MEGA_TILE_HEIGHT / (f32)TILESET_HEIGHT;
            s32 tilesPerRow = TILESET_TILES_PER_ROW;
            s32 totalRows   = TILESET_HEIGHT / MEGA_TILE_HEIGHT;
            s32 totalTiles  = tilesPerRow * totalRows;

            // Zero padding between buttons so tiles are packed tight.
            igPushStyleVar_Vec2(ImGuiStyleVar_ItemSpacing,  (ImVec2_c){ 1.0f, 1.0f });
            igPushStyleVar_Vec2(ImGuiStyleVar_FramePadding, (ImVec2_c){ 0.0f, 0.0f });

            // Fetch the window draw list once for overlay rendering.
            ImDrawList* dl = igGetWindowDrawList();

            for (s32 i = 0; i < totalTiles; i++)
            {
                s32 col = i % tilesPerRow;
                s32 row = i / tilesPerRow;

                ImVec2_c uv0 = { col * tileUW,          row * tileUH          };
                ImVec2_c uv1 = { col * tileUW + tileUW, row * tileUH + tileUH };

                // Record the screen-space top-left corner BEFORE the button
                // so we can draw overlays on top of the image afterward.
                ImVec2_c itemPos = igGetCursorScreenPos();

                igPushID_Int(i);
                if (igImageButton("##t", texRef, displaySize, uv0, uv1,
                                  (ImVec4_c){ 0.0f, 0.0f, 0.0f, 0.0f },  // bg_col: transparent
                                  (ImVec4_c){ 1.0f, 1.0f, 1.0f, 1.0f })) // tint_col: none
                    ctx->selectedTileIndex = (u16)i;
                igPopID();

                // Draw hover / selected highlights via ImDrawList so they are
                // visible on top of the image regardless of FramePadding=0.
                bool isHovered  = igIsItemHovered(0);
                bool isSelected = (ctx->selectedTileIndex == (u16)i);

                if (isHovered || isSelected)
                {
                    ImVec2_c itemMax;
                    itemMax.x = itemPos.x + tileSize;
                    itemMax.y = itemPos.y + tileSize;

                    // White 25% alpha fill for hover
                    if (isHovered)
                        ImDrawList_AddRectFilled(dl, itemPos, itemMax,
                                                 0x40FFFFFFu, 0.0f, 0);

                    // Gold 2 px border for selected
                    // IM_COL32(255,215,0,255) = R=0xFF, G=0xD7, B=0x00, A=0xFF
                    if (isSelected)
                        ImDrawList_AddRect(dl, itemPos, itemMax,
                                           0xFF00D7FFu, 0.0f, 2.0f, 0);
                }

                // Same-line for every tile except the last in the row
                if (col < tilesPerRow - 1)
                    igSameLine(0.0f, 1.0f);
            }

            igPopStyleVar(2);
        }
        else
        {
            igText("No tileset loaded. Import a level first.");
        }
    }
    igEnd();
}

void weui_init(SDL_Window* window, SDL_Renderer* renderer)
{
    s_window   = window;
    s_renderer = renderer;

    igCreateContext(NULL);

    ImGuiIO* io = igGetIO_Nil();
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Add the default font so text renders at startup.
    ImFontAtlas_AddFontDefault(io->Fonts, NULL);
}

void weui_shutdown(void)
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    igDestroyContext(NULL);

    s_window   = NULL;
    s_renderer = NULL;
}

void weui_beginFrame(WarEditorContext* ctx)
{

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    igNewFrame();

    // -----------------------------------------------------------------------
    // DPI framebuffer scale sync (2.6)
    // -----------------------------------------------------------------------
    {
        int fbW = 0, fbH = 0, winW = 0, winH = 0;
        SDL_GetWindowSizeInPixels(s_window, &fbW, &fbH);
        SDL_GetWindowSize(s_window, &winW, &winH);
        ImGuiIO* io = igGetIO_Nil();
        ImVec2 scale;
        scale.x = (winW > 0) ? (f32)fbW / winW : 1.0f;
        scale.y = (winH > 0) ? (f32)fbH / winH : 1.0f;
        io->DisplayFramebufferScale = scale;
    }

    // -----------------------------------------------------------------------
    // Full-window dockspace (2.8)
    // -----------------------------------------------------------------------
    igDockSpaceOverViewport(0, igGetMainViewport(), ImGuiDockNodeFlags_None, NULL);

    // -----------------------------------------------------------------------
    // Main menu bar (2.9)
    // -----------------------------------------------------------------------
    if (igBeginMainMenuBar())
    {
        if (igBeginMenu("File", true))
        {
            igMenuItem_Bool("New Map",                 "Ctrl+N",       false, true);
            igMenuItem_Bool("Open...",                 "Ctrl+O",       false, true);
            igMenuItem_Bool("Save",                    "Ctrl+S",       false, true);
            igMenuItem_Bool("Save As...",              "Ctrl+Shift+S", false, true);
            igSeparator();
            if (igMenuItem_Bool("Import Campaign Level...", NULL, false, true))
                s_showImportDialog = true;
            igSeparator();
            igMenuItem_Bool("Exit",                    NULL,           false, true);
            igEndMenu();
        }

        if (igBeginMenu("Edit", true))
        {
            igMenuItem_Bool("Undo",            "Ctrl+Z",       false, true);
            igMenuItem_Bool("Redo",            "Ctrl+Y",       false, true);
            igSeparator();
            igMenuItem_Bool("Copy",            "Ctrl+C",       false, true);
            igMenuItem_Bool("Paste",           "Ctrl+V",       false, true);
            igEndMenu();
        }

        if (igBeginMenu("View", true))
        {
            if (igMenuItem_Bool("Show Grid",        "G", ctx->showGrid,        true))
                ctx->showGrid = !ctx->showGrid;
            if (igMenuItem_Bool("Show Passability", "P", ctx->showPassability, true))
                ctx->showPassability = !ctx->showPassability;
            if (igMenuItem_Bool("Show Minimap",     NULL, ctx->showMinimap,    true))
                ctx->showMinimap = !ctx->showMinimap;
            igEndMenu();
        }

        if (igBeginMenu("Map", true))
        {
            igMenuItem_Bool("Validate Map",              NULL, false, true);
            igEndMenu();
        }

        if (igBeginMenu("Help", true))
        {
            igMenuItem_Bool("Keyboard Shortcuts", NULL, false, true);
            igMenuItem_Bool("About",              NULL, false, true);
            igEndMenu();
        }

        // -----------------------------------------------------------------------
        // Play button (2.10) — stub; logs a message for now
        // -----------------------------------------------------------------------
        igSeparator();
        ImVec2 buttonSize;
        buttonSize.x = 0;
        buttonSize.y = 0;
        if (igButton("Play", buttonSize))
        {
            logInfo("Playtest not implemented");
        }

        igEndMainMenuBar();
    }

    // -----------------------------------------------------------------------
    // Per-frame tool state update (7.11 — G/P shortcuts, fill drag release)
    // -----------------------------------------------------------------------
    wetools_update(ctx);

    // -----------------------------------------------------------------------
    // Canvas panel (4.5 / Phase 4)
    // Writes tile coordinate into ctx->statusText when hovered.
    // -----------------------------------------------------------------------
    wecanvas_drawPanel(ctx, ctx->statusText, (s32)sizeof(ctx->statusText));

    // -----------------------------------------------------------------------
    // Toolbox panel (7.2)
    // -----------------------------------------------------------------------
    weui_drawToolboxPanel(ctx);

    // -----------------------------------------------------------------------
    // Tile palette panel (7.8 + 7.9)
    // -----------------------------------------------------------------------
    weui_drawTilePalettePanel(ctx);

    // -----------------------------------------------------------------------
    // Import Campaign Level dialog (Phase 5)
    // -----------------------------------------------------------------------
    weui_drawImportDialog(ctx);

    // -----------------------------------------------------------------------
    // Status bar (2.11) — pinned at the bottom of the display
    // -----------------------------------------------------------------------
    {
        ImGuiIO*    io       = igGetIO_Nil();
        ImVec2      dispSize = io->DisplaySize;
        f32         height   = igGetFrameHeight();
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration  |
            ImGuiWindowFlags_NoInputs      |
            ImGuiWindowFlags_NoMove        |
            ImGuiWindowFlags_NoScrollbar   |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImVec2 statusPos;
        statusPos.x = 0;
        statusPos.y = dispSize.y - height;
        ImVec2 statusSize;
        statusSize.x = dispSize.x;
        statusSize.y = height;
        igSetNextWindowPos(statusPos, ImGuiCond_Always, (ImVec2){0, 0});
        igSetNextWindowSize(statusSize, ImGuiCond_Always);
        if (igBegin("##statusbar", NULL, flags))
        {
            igText("War1-C Scenario Editor  |  %s", ctx->statusText);
        }
        igEnd();
    }
}

void weui_endFrame(SDL_Renderer* renderer)
{
    igRender();
    ImGui_ImplSDLRenderer3_RenderDrawData(igGetDrawData(), renderer);
}
