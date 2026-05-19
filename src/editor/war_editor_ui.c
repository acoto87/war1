#include "war_editor_ui.h"
#include "war_editor.h"
#include "war_editor_canvas.h"
#include "war_editor_map.h"

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
            igMenuItem_Bool("Show Grid",       "G",            false, true);
            igMenuItem_Bool("Show Passability","P",            false, true);
            igMenuItem_Bool("Show Minimap",    NULL,           false, true);
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
    // Canvas panel (4.5 / Phase 4)
    // Writes tile coordinate into ctx->statusText when hovered.
    // -----------------------------------------------------------------------
    wecanvas_drawPanel(ctx, ctx->statusText, (s32)sizeof(ctx->statusText));

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
