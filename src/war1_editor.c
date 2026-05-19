// war1_editor.c — War1-C Scenario Editor unity entry point (Phase 1 stub)
//
// This file is the single translation unit compiled for the editor binary.
// Editor-specific source files are #included here (added phase by phase).
// Shared game core source files are also #included here in Phase 2.
//
// Build:
//   nob editor --cc msvc --target win64 [--check]
//   nob editor --cc gcc  --target linux64

// -------------------------------------------------------------------------
// cimgui C API (Dear ImGui wrapped for C99)
// Must be included before any SDL3 headers so CIMGUI_API is defined first.
// The following macros are passed via -D by nob.c build_editor():
//   CIMGUI_DEFINE_ENUMS_AND_STRUCTS, CIMGUI_NO_EXPORT,
//   CIMGUI_USE_SDL3, CIMGUI_USE_SDLRENDERER3
// -------------------------------------------------------------------------
#include "cimgui/cimgui.h"
#include "cimgui/cimgui_impl.h"

// -------------------------------------------------------------------------
// SDL3
// -------------------------------------------------------------------------
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

// -------------------------------------------------------------------------
// Entry point (Phase 2 will replace this with we_init / we_run / we_quit)
// -------------------------------------------------------------------------
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}
