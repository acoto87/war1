#include "war_editor_ui.h"
#include "war_editor.h"
#include "war_editor_canvas.h"
#include "war_editor_inspector.h"
#include "war_editor_map.h"
#include "war_editor_serialization.h"
#include "war_editor_tools.h"

// Module-level storage so weui_beginFrame can access the window without
// requiring it as a parameter (matching the void signature in the spec).
static SDL_Window*   s_window   = NULL;
static SDL_Renderer* s_renderer = NULL;

// -----------------------------------------------------------------------
// File I/O dialog state (Phase 11)
// -----------------------------------------------------------------------

// Action waiting to happen after an "unsaved changes?" confirmation or
// after a file-picker dialog resolves.
typedef enum
{
    WE_PENDING_NONE  = 0,
    WE_PENDING_NEW   = 1,   // create a blank map
    WE_PENDING_OPEN  = 2,   // open a file-picker, then load the map
    WE_PENDING_QUIT  = 3,   // quit the editor
} WePendingAction;

static WePendingAction s_pendingAction   = WE_PENDING_NONE;

// Pending open-dialog result.  Set by weui_openFileCallback; consumed by
// weui_processPendingPaths at the start of the next frame.
static bool s_openPathReady              = false;
static char s_pendingOpenPath[512];

// Pending save-dialog result.  Set by weui_saveFileCallback; consumed by
// weui_processPendingPaths at the start of the next frame.
static bool s_savePathReady              = false;
static char s_pendingSavePath[512];

// Window title cache — only calls SDL_SetWindowTitle when the title changes.
static char s_cachedWindowTitle[640];

// -----------------------------------------------------------------------
// Window title sync
// -----------------------------------------------------------------------
static void weui_syncWindowTitle(WarEditorContext* ctx)
{
    char title[640];
    if (ctx->mapName[0])
    {
        if (ctx->unsavedChanges)
            SDL_snprintf(title, sizeof(title), "war1_editor - %s *", ctx->mapName);
        else
            SDL_snprintf(title, sizeof(title), "war1_editor - %s", ctx->mapName);
    }
    else
    {
        if (ctx->unsavedChanges)
            SDL_strlcpy(title, "war1_editor - untitled *", sizeof(title));
        else
            SDL_strlcpy(title, "war1_editor - untitled", sizeof(title));
    }

    if (strcmp(title, s_cachedWindowTitle) != 0)
    {
        SDL_strlcpy(s_cachedWindowTitle, title, sizeof(s_cachedWindowTitle));
        SDL_SetWindowTitle(s_window, title);
    }
}

// -----------------------------------------------------------------------
// Map-name extraction from a file path
// -----------------------------------------------------------------------
static void weui_extractMapName(WarEditorContext* ctx)
{
    const char* path = ctx->currentFilePath;

    // Walk forward to find the last path separator.
    const char* base = path;
    for (const char* p = path; *p; p++)
    {
        if (*p == '/' || *p == '\\')
            base = p + 1;
    }

    // Copy base name up to the first '.', clamped to mapName's capacity.
    s32 i = 0;
    s32 limit = (s32)sizeof(ctx->mapName) - 1;
    while (base[i] && base[i] != '.' && i < limit)
    {
        ctx->mapName[i] = base[i];
        i++;
    }
    ctx->mapName[i] = '\0';
}

// -----------------------------------------------------------------------
// SDL3 file-dialog callbacks
// -----------------------------------------------------------------------
static void weui_openFileCallback(void* userdata,
                                   const char* const* filelist, int filter)
{
    (void)userdata;
    (void)filter;

    s_pendingOpenPath[0] = '\0';
    if (filelist && filelist[0])
        SDL_strlcpy(s_pendingOpenPath, filelist[0], sizeof(s_pendingOpenPath));

    s_openPathReady = true;
}

static void weui_saveFileCallback(void* userdata,
                                   const char* const* filelist, int filter)
{
    (void)userdata;
    (void)filter;

    s_pendingSavePath[0] = '\0';
    if (filelist && filelist[0])
        SDL_strlcpy(s_pendingSavePath, filelist[0], sizeof(s_pendingSavePath));

    s_savePathReady = true;
}

// -----------------------------------------------------------------------
// File-picker filter arrays (static — valid for the lifetime of the process)
// -----------------------------------------------------------------------
static const SDL_DialogFileFilter s_w1mFilters[] =
{
    { "War1-C Map (*.w1m)", "w1m" },
    { "All Files",           "*"  },
};

// -----------------------------------------------------------------------
// Forward declarations for action helpers
// (defined below; referenced by the unsaved-changes modal)
// -----------------------------------------------------------------------
static void weui_requestNew(WarEditorContext* ctx);
static void weui_requestOpen(WarEditorContext* ctx);
static void weui_requestSave(WarEditorContext* ctx);
static void weui_requestSaveAs(WarEditorContext* ctx);
static void weui_requestQuit(WarEditorContext* ctx);
static void weui_executeAction(WarEditorContext* ctx, WePendingAction action);

// -----------------------------------------------------------------------
// Pending path processing — called at the top of each frame
// -----------------------------------------------------------------------
static void weui_processPendingPaths(WarEditorContext* ctx)
{
    // --- Save-dialog result ---
    if (s_savePathReady)
    {
        s_savePathReady = false;

        bool savedOk = false;
        if (s_pendingSavePath[0])
        {
            // Auto-append .w1m if the user didn't type it.
            size_t len = SDL_strlen(s_pendingSavePath);
            if (len < 4 || !wsv_equalsIgnoreCase(
                    wsv_fromParts(s_pendingSavePath + len - 4, 4),
                    WSV_LITERAL(".w1m")))
            {
                SDL_strlcat(s_pendingSavePath, ".w1m", sizeof(s_pendingSavePath));
            }

            if (wesave_saveMap(s_pendingSavePath, ctx))
            {
                SDL_strlcpy(ctx->currentFilePath, s_pendingSavePath,
                            sizeof(ctx->currentFilePath));
                weui_extractMapName(ctx);
                ctx->unsavedChanges = false;
                savedOk = true;
                SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                             "Saved: %s", ctx->currentFilePath);
            }
            else
            {
                SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                             "Save failed: %s", s_pendingSavePath);
            }
        }
        // else: user cancelled the save dialog — do not execute pending action

        if (s_pendingAction != WE_PENDING_NONE)
        {
            WePendingAction action = s_pendingAction;
            s_pendingAction        = WE_PENDING_NONE;
            if (savedOk)
                weui_executeAction(ctx, action);
            // else: save failed / cancelled — pending action is dropped
        }
    }

    // --- Open-dialog result ---
    if (s_openPathReady)
    {
        s_openPathReady = false;

        if (s_pendingOpenPath[0])
        {
            if (wesave_loadMap(s_pendingOpenPath, ctx))
            {
                SDL_strlcpy(ctx->currentFilePath, s_pendingOpenPath,
                            sizeof(ctx->currentFilePath));
                weui_extractMapName(ctx);
                ctx->unsavedChanges = false;
                WarEntityIdListClear(&ctx->selectedEntities);
                SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                             "Opened: %s", ctx->currentFilePath);
            }
            else
            {
                SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                             "Open failed: %s", s_pendingOpenPath);
            }
        }
        // else: user cancelled the open dialog — nothing to do
    }
}

// -----------------------------------------------------------------------
// Unsaved-changes confirmation modal (11.9)
// -----------------------------------------------------------------------
static void weui_drawUnsavedChangesModal(WarEditorContext* ctx)
{
    // Centre the popup.
    ImGuiViewport* vp = igGetMainViewport();
    ImVec2 centre;
    centre.x = vp->Pos.x + vp->Size.x * 0.5f;
    centre.y = vp->Pos.y + vp->Size.y * 0.5f;
    igSetNextWindowPos(centre, ImGuiCond_Appearing, (ImVec2){ 0.5f, 0.5f });
    igSetNextWindowSize((ImVec2){ 360.0f, 0.0f }, ImGuiCond_Appearing);

    if (igBeginPopupModal("Unsaved Changes##modal", NULL,
                          ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ctx->unsavedChanges)
        {
            igText("You have unsaved changes.");
            igText("Save before continuing?");
        }
        else
        {
            igText("Are you sure you want to discard the current map?");
        }
        igSeparator();

        // "Save" is only shown when there are unsaved changes.
        if (ctx->unsavedChanges)
        {
            if (igButton("Save", (ImVec2){ 80.0f, 0.0f }))
            {
                igCloseCurrentPopup();

                if (ctx->currentFilePath[0])
                {
                    // Save directly to the known path, then execute the pending action.
                    if (wesave_saveMap(ctx->currentFilePath, ctx))
                    {
                        ctx->unsavedChanges = false;
                        WePendingAction action = s_pendingAction;
                        s_pendingAction        = WE_PENDING_NONE;
                        weui_executeAction(ctx, action);
                    }
                    else
                    {
                        SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                                     "Save failed!");
                        s_pendingAction = WE_PENDING_NONE;
                    }
                }
                else
                {
                    // No path yet — show Save As dialog.
                    // s_pendingAction stays set; weui_processPendingPaths will
                    // execute it after the save dialog resolves.
                    SDL_ShowSaveFileDialog(weui_saveFileCallback, NULL, s_window,
                                           s_w1mFilters, 2, NULL);
                }
            }

            igSameLine(0.0f, 8.0f);
        }

        // Label is "Ok" when there are no unsaved changes, "Discard" otherwise.
        const char* discardLabel = ctx->unsavedChanges ? "Discard" : "Ok";
        if (igButton(discardLabel, (ImVec2){ 80.0f, 0.0f }))
        {
            igCloseCurrentPopup();
            ctx->unsavedChanges = false;
            WePendingAction action = s_pendingAction;
            s_pendingAction        = WE_PENDING_NONE;
            weui_executeAction(ctx, action);
        }

        igSameLine(0.0f, 8.0f);

        if (igButton("Cancel", (ImVec2){ 80.0f, 0.0f }))
        {
            igCloseCurrentPopup();
            s_pendingAction   = WE_PENDING_NONE;
            s_openPathReady   = false;
            s_savePathReady   = false;
        }

        igEndPopup();
    }
}

// -----------------------------------------------------------------------
// Action helpers (11.6)
// -----------------------------------------------------------------------

// Execute a pending action directly (no unsaved-changes check).
static void weui_executeAction(WarEditorContext* ctx, WePendingAction action)
{
    if (action == WE_PENDING_NEW)
    {
        wemap_newMap(ctx);
    }
    else if (action == WE_PENDING_OPEN)
    {
        SDL_ShowOpenFileDialog(weui_openFileCallback, NULL, s_window,
                               s_w1mFilters, 2, NULL, false);
    }
    else if (action == WE_PENDING_QUIT)
    {
        ctx->running = false;
    }
}

// Request a New Map; shows the unsaved-changes modal if a map is open.
static void weui_requestNew(WarEditorContext* ctx)
{
    if (ctx->map != NULL)
    {
        s_pendingAction = WE_PENDING_NEW;
        igOpenPopup_Str("Unsaved Changes##modal", 0);
    }
    else
    {
        wemap_newMap(ctx);
    }
}

// Request Open; shows the unsaved-changes modal or file picker as needed.
static void weui_requestOpen(WarEditorContext* ctx)
{
    if (ctx->unsavedChanges)
    {
        s_pendingAction = WE_PENDING_OPEN;
        igOpenPopup_Str("Unsaved Changes##modal", 0);
    }
    else
    {
        SDL_ShowOpenFileDialog(weui_openFileCallback, NULL, s_window,
                               s_w1mFilters, 2, NULL, false);
    }
}

// Save to the current path if known; otherwise show Save As.
static void weui_requestSave(WarEditorContext* ctx)
{
    if (ctx->currentFilePath[0])
    {
        if (wesave_saveMap(ctx->currentFilePath, ctx))
        {
            ctx->unsavedChanges = false;
            SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                         "Saved: %s", ctx->currentFilePath);
        }
        else
        {
            SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                         "Save failed!");
        }
    }
    else
    {
        weui_requestSaveAs(ctx);
    }
}

// Show Save As file picker.
static void weui_requestSaveAs(WarEditorContext* ctx)
{
    (void)ctx;
    SDL_ShowSaveFileDialog(weui_saveFileCallback, NULL, s_window,
                           s_w1mFilters, 2, NULL);
}

// Request quit; shows the unsaved-changes modal if needed.
static void weui_requestQuit(WarEditorContext* ctx)
{
    if (ctx->unsavedChanges)
    {
        s_pendingAction = WE_PENDING_QUIT;
        igOpenPopup_Str("Unsaved Changes##modal", 0);
    }
    else
    {
        ctx->running = false;
    }
}

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
// Tools are grouped: Select (global), Terrain (pencil/fill/erase),
// Entities (entity placement).
// -----------------------------------------------------------------------
static void weui_drawToolButton(WarEditorContext* ctx,
                                WarEditorToolType type,
                                const char*       label,
                                f32               width)
{
    bool active = (ctx->activeTool == type);
    if (active)
    {
        igPushStyleColor_Vec4(ImGuiCol_Button,
            (ImVec4_c){ 0.80f, 0.60f, 0.10f, 1.0f });
        igPushStyleColor_Vec4(ImGuiCol_ButtonHovered,
            (ImVec4_c){ 0.90f, 0.70f, 0.20f, 1.0f });
    }

    ImVec2_c sz = { width, 0.0f };
    if (igButton(label, sz))
        ctx->activeTool = type;

    if (active)
        igPopStyleColor(2);
}

static void weui_drawToolboxPanel(WarEditorContext* ctx)
{
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    if (igBegin("Toolbox##toolbox", NULL, flags))
    {
        f32 w = igGetContentRegionAvail().x;

        // --- Global ---
        weui_drawToolButton(ctx, WE_TOOL_SELECT, "Select", w);

        igSeparator();
        igTextDisabled("Terrain");
        weui_drawToolButton(ctx, WE_TOOL_PENCIL, "Pencil", w);
        weui_drawToolButton(ctx, WE_TOOL_FILL,   "Fill",   w);
        weui_drawToolButton(ctx, WE_TOOL_ERASE,  "Erase",  w);

        igSeparator();
        igTextDisabled("Entities");
        weui_drawToolButton(ctx, WE_TOOL_PLACE_ENTITY, "Entity", w);
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

// -----------------------------------------------------------------------
// Entity palette panel (Phase 8) — player selection + unit/building palette.
// Clicking a thumbnail sets ctx->selectedUnitType and switches to the
// WE_TOOL_PLACE_ENTITY tool.
// -----------------------------------------------------------------------

// Player colors used for the active-player selector buttons.
static const ImVec4_c s_playerBtnColors[5] =
{
    { 0.00f, 0.00f, 0.78f, 1.0f },  // player 0: blue
    { 0.78f, 0.00f, 0.00f, 1.0f },  // player 1: red
    { 0.00f, 0.78f, 0.00f, 1.0f },  // player 2: green
    { 0.78f, 0.78f, 0.00f, 1.0f },  // player 3: yellow
    { 0.78f, 0.78f, 0.78f, 1.0f },  // player 4: white
};

static void weui_drawEntityPalettePanel(WarEditorContext* ctx)
{
    if (igBegin("Entities##entities", NULL, ImGuiWindowFlags_None))
    {
        // --- Player selector ---
        igText("Player:");
        igSameLine(0.0f, 6.0f);

        for (s32 p = 0; p < 5; p++)
        {
            ImVec4_c col = s_playerBtnColors[p];
            ImVec4_c hov = { fminf(col.x + 0.2f, 1.0f),
                             fminf(col.y + 0.2f, 1.0f),
                             fminf(col.z + 0.2f, 1.0f),
                             1.0f };
            igPushStyleColor_Vec4(ImGuiCol_Button,        col);
            igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, hov);

            char label[16];
            bool active = (ctx->activePlayer == (u8)p);
            SDL_snprintf(label, sizeof(label),
                         active ? "[%d]##p%d" : " %d ##p%d", p, p);

            if (igButton(label, (ImVec2_c){ 28.0f, 0.0f }))
                ctx->activePlayer = (u8)p;

            igPopStyleColor(2);

            if (p < 4)
                igSameLine(0.0f, 2.0f);
        }

        igSeparator();

        // --- Entity groups ---
        static const struct
        {
            const char* groupName;
            WarUnitType types[9];
            s32         count;
        } groups[] =
        {
            {
                "Human Units",
                { WAR_UNIT_FOOTMAN, WAR_UNIT_PEASANT, WAR_UNIT_ARCHER,
                  WAR_UNIT_KNIGHT,  WAR_UNIT_CLERIC,  WAR_UNIT_CONJURER,
                  WAR_UNIT_CATAPULT_HUMANS },
                7
            },
            {
                "Orc Units",
                { WAR_UNIT_GRUNT,    WAR_UNIT_PEON,      WAR_UNIT_SPEARMAN,
                  WAR_UNIT_RAIDER,   WAR_UNIT_NECROLYTE, WAR_UNIT_WARLOCK,
                  WAR_UNIT_CATAPULT_ORCS },
                7
            },
            {
                "Human Buildings",
                { WAR_UNIT_TOWNHALL_HUMANS, WAR_UNIT_FARM_HUMANS,
                  WAR_UNIT_BARRACKS_HUMANS, WAR_UNIT_LUMBERMILL_HUMANS,
                  WAR_UNIT_BLACKSMITH_HUMANS, WAR_UNIT_CHURCH,
                  WAR_UNIT_TOWER_HUMANS, WAR_UNIT_STABLE, WAR_UNIT_STORMWIND },
                9
            },
            {
                "Orc Buildings",
                { WAR_UNIT_TOWNHALL_ORCS, WAR_UNIT_FARM_ORCS,
                  WAR_UNIT_BARRACKS_ORCS, WAR_UNIT_LUMBERMILL_ORCS,
                  WAR_UNIT_BLACKSMITH_ORCS, WAR_UNIT_TEMPLE,
                  WAR_UNIT_TOWER_ORCS, WAR_UNIT_KENNEL, WAR_UNIT_BLACKROCK },
                9
            },
            {
                "Neutral",
                { WAR_UNIT_GOLDMINE },
                1
            },
        };
        s32 groupCount = (s32)(sizeof(groups) / sizeof(groups[0]));

        f32      thumbSize   = 32.0f;
        ImVec2_c displaySize = { thumbSize, thumbSize };
        ImVec2_c uv0         = { 0.0f, 0.0f };
        ImVec2_c uv1         = { 1.0f, 1.0f };

        igPushStyleVar_Vec2(ImGuiStyleVar_ItemSpacing,  (ImVec2_c){ 2.0f, 2.0f });
        igPushStyleVar_Vec2(ImGuiStyleVar_FramePadding, (ImVec2_c){ 1.0f, 1.0f });

        ImDrawList* dl = igGetWindowDrawList();

        for (s32 g = 0; g < groupCount; g++)
        {
            if (igCollapsingHeader_TreeNodeFlags(groups[g].groupName,
                                                ImGuiTreeNodeFlags_DefaultOpen))
            {
                f32 panelW = igGetContentRegionAvail().x;
                s32 perRow = (s32)((panelW + 2.0f) / (thumbSize + 2.0f));
                if (perRow < 1) perRow = 1;

                for (s32 j = 0; j < groups[g].count; j++)
                {
                    WarUnitType        type = groups[g].types[j];
                    const WarUnitData* ud   = wu_getUnitData(type);

                    ImTextureRef_c texRef;
                    texRef._TexData = NULL;
                    texRef._TexID   = 0;

                    if (ud && ud->resourceIndex > 0)
                    {
                        SDL_Texture* tex = wecanvas_getSpriteTexture(ctx, ud->resourceIndex);
                        if (tex)
                            texRef._TexID = (ImTextureID)(uintptr_t)tex;
                    }

                    ImVec2_c itemPos = igGetCursorScreenPos();

                    char btnId[32];
                    SDL_snprintf(btnId, sizeof(btnId), "##ent%d", (s32)type);

                    bool clicked = igImageButton(btnId, texRef, displaySize, uv0, uv1,
                                                 (ImVec4_c){ 0.0f, 0.0f, 0.0f, 1.0f },
                                                 (ImVec4_c){ 1.0f, 1.0f, 1.0f, 1.0f });
                    if (clicked)
                    {
                        ctx->selectedUnitType = type;
                        ctx->activeTool       = WE_TOOL_PLACE_ENTITY;
                    }

                    // Highlight selected / hovered thumbnail
                    bool isSelected = (ctx->selectedUnitType == type &&
                                       ctx->activeTool == WE_TOOL_PLACE_ENTITY);
                    bool isHovered  = igIsItemHovered(0);

                    if (isSelected || isHovered)
                    {
                        ImVec2_c itemMax;
                        itemMax.x = itemPos.x + thumbSize + 2.0f;
                        itemMax.y = itemPos.y + thumbSize + 2.0f;

                        if (isSelected)
                            ImDrawList_AddRect(dl, itemPos, itemMax,
                                               0xFF00D7FFu, 0.0f, 2.0f, 0);
                        else
                            ImDrawList_AddRectFilled(dl, itemPos, itemMax,
                                                     0x30FFFFFFu, 0.0f, 0);
                    }

                    // Tooltip: unit name
                    if (igIsItemHovered(0) && ud)
                        igSetTooltip("%.*s", (int)ud->name.length, ud->name.data);

                    // Same-line within each row
                    if ((j + 1) % perRow != 0 && j < groups[g].count - 1)
                        igSameLine(0.0f, 2.0f);
                }
            }
        }

        igPopStyleVar(2);
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
    // Window title sync (11.8) — cheaply updates only when needed
    // -----------------------------------------------------------------------
    weui_syncWindowTitle(ctx);

    // -----------------------------------------------------------------------
    // Quit request forwarded from the SDL_EVENT_QUIT event (11.9)
    // -----------------------------------------------------------------------
    if (ctx->quitRequested)
    {
        ctx->quitRequested = false;
        weui_requestQuit(ctx);
    }

    // -----------------------------------------------------------------------
    // Keyboard shortcuts (11.6)
    // -----------------------------------------------------------------------
    {
        ImGuiIO* io = igGetIO_Nil();
        if (io->KeyCtrl)
        {
            if (!io->KeyShift && igIsKeyPressed_Bool(ImGuiKey_N, false))
                weui_requestNew(ctx);
            else if (!io->KeyShift && igIsKeyPressed_Bool(ImGuiKey_O, false))
                weui_requestOpen(ctx);
            else if (!io->KeyShift && igIsKeyPressed_Bool(ImGuiKey_S, false))
                weui_requestSave(ctx);
            else if (io->KeyShift && igIsKeyPressed_Bool(ImGuiKey_S, false))
                weui_requestSaveAs(ctx);
        }
    }

    // -----------------------------------------------------------------------
    // Pending file-dialog results (11.6) — process at start of frame so that
    // save/load completes before any ImGui widgets react to new map state.
    // -----------------------------------------------------------------------
    weui_processPendingPaths(ctx);

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
            if (igMenuItem_Bool("New Map",    "Ctrl+N",       false, true))
                weui_requestNew(ctx);
            if (igMenuItem_Bool("Open...",    "Ctrl+O",       false, true))
                weui_requestOpen(ctx);
            if (igMenuItem_Bool("Save",       "Ctrl+S",       false, true))
                weui_requestSave(ctx);
            if (igMenuItem_Bool("Save As...", "Ctrl+Shift+S", false, true))
                weui_requestSaveAs(ctx);
            igSeparator();
            if (igMenuItem_Bool("Import Campaign Level...", NULL, false, true))
                s_showImportDialog = true;
            igSeparator();
            if (igMenuItem_Bool("Exit", NULL, false, true))
                weui_requestQuit(ctx);
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
    // Entity palette panel (Phase 8)
    // -----------------------------------------------------------------------
    weui_drawEntityPalettePanel(ctx);

    // -----------------------------------------------------------------------
    // Inspector panel (Phase 10)
    // -----------------------------------------------------------------------
    weinspect_drawPanel(ctx);

    // -----------------------------------------------------------------------
    // Import Campaign Level dialog (Phase 5)
    // -----------------------------------------------------------------------
    weui_drawImportDialog(ctx);

    // -----------------------------------------------------------------------
    // Unsaved-changes confirmation modal (11.9)
    // -----------------------------------------------------------------------
    weui_drawUnsavedChangesModal(ctx);

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
