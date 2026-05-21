#include "war_editor_playtest.h"
#include "war_editor_serialization.h"

#include "war_log.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif

// Temp map path written before launching the game.
#define WEPLAY_TEMP_DIR      "maps"
#define WEPLAY_TEMP_MAP_PATH "maps/temp_playtest.w1m"

// Game binary path relative to the editor's working directory.
#if defined(_WIN32)
#   define WEPLAY_GAME_BIN "war1.exe"
#else
#   define WEPLAY_GAME_BIN "./war1"
#endif

// -----------------------------------------------------------------------
// Error popup state
// -----------------------------------------------------------------------
static bool s_openErrorPopup     = false;
static char s_errorMessage[256];

// -----------------------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------------------

// Launch the game binary with --map <path> as a detached child process.
// Returns true on success.
static bool weplay_launchGame(const char* mapPath)
{
#if defined(_WIN32)
    char cmdLine[512];
    SDL_snprintf(cmdLine, sizeof(cmdLine), "%s --map %s", WEPLAY_GAME_BIN, mapPath);

    STARTUPINFOA        si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    memset(&pi, 0, sizeof(pi));

    BOOL ok = CreateProcessA(
        NULL,             // module name — derived from cmdLine
        cmdLine,          // command line
        NULL,             // process security attributes
        NULL,             // thread security attributes
        FALSE,            // inherit handles
        DETACHED_PROCESS, // creation flags
        NULL,             // environment (inherit)
        NULL,             // current directory (inherit)
        &si,
        &pi
    );

    if (!ok)
    {
        return false;
    }

    // We don't monitor the child; close handles immediately.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
#else
    pid_t pid = fork();
    if (pid < 0)
    {
        return false;
    }
    if (pid == 0)
    {
        // Child process.
        char* const args[] = {
            (char*)WEPLAY_GAME_BIN,
            "--map",
            (char*)mapPath,
            NULL
        };
        execv(WEPLAY_GAME_BIN, args);
        // execv only returns on error.
        _exit(1);
    }
    // Parent continues immediately; child is independent.
    return true;
#endif
}

// -----------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------

void weplay_startPlaytest(WarEditorContext* ctx)
{
    if (!ctx->map)
    {
        SDL_strlcpy(s_errorMessage, "No map is open.", sizeof(s_errorMessage));
        s_openErrorPopup = true;
        return;
    }

    // Ensure the temp directory exists.
    SDL_CreateDirectory(WEPLAY_TEMP_DIR);

    // Save the current map to the temp path.
    if (!wesave_saveMap(WEPLAY_TEMP_MAP_PATH, ctx))
    {
        SDL_strlcpy(s_errorMessage,
                    "Failed to save temp map.\nCheck the log for details.",
                    sizeof(s_errorMessage));
        s_openErrorPopup = true;
        return;
    }

    if (!weplay_launchGame(WEPLAY_TEMP_MAP_PATH))
    {
        SDL_snprintf(s_errorMessage, sizeof(s_errorMessage),
                     "Could not launch \"%s\".\n"
                     "Make sure the game binary is in the same directory as the editor.",
                     WEPLAY_GAME_BIN);
        s_openErrorPopup = true;
        return;
    }

    logInfo("weplay_startPlaytest: launched %s --map %s", WEPLAY_GAME_BIN, WEPLAY_TEMP_MAP_PATH);
}

void weplay_drawErrorPopup(void)
{
    if (s_openErrorPopup)
    {
        igOpenPopup_Str("Playtest Error##modal", 0);
        s_openErrorPopup = false;
    }

    ImGuiViewport* vp = igGetMainViewport();
    ImVec2 centre;
    centre.x = vp->Pos.x + vp->Size.x * 0.5f;
    centre.y = vp->Pos.y + vp->Size.y * 0.5f;
    igSetNextWindowPos(centre, ImGuiCond_Appearing, (ImVec2){ 0.5f, 0.5f });
    igSetNextWindowSize((ImVec2){ 380.0f, 0.0f }, ImGuiCond_Appearing);

    if (igBeginPopupModal("Playtest Error##modal", NULL,
                          ImGuiWindowFlags_AlwaysAutoResize))
    {
        igTextWrapped(s_errorMessage);
        igSeparator();
        if (igButton("OK", (ImVec2){ 80.0f, 0.0f }))
        {
            igCloseCurrentPopup();
        }
        igEndPopup();
    }
}
