#include "war_editor_playtest.h"
#include "war_editor_serialization.h"
#include "war_editor_validator.h"

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

static bool s_openValidationPopup = false;
static bool s_pendingPlaytest = false;
static WarEditorContext* s_pendingCtx = NULL;
static WarValidationResult s_pendingValidation[WE_VALIDATION_MAX_RESULTS];
static s32 s_pendingValidationCount = 0;

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

static bool weplay_executeStart(WarEditorContext* ctx)
{
    if (!ctx || !ctx->map)
    {
        SDL_strlcpy(s_errorMessage, "No map is open.", sizeof(s_errorMessage));
        s_openErrorPopup = true;
        return false;
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
        return false;
    }

    if (!weplay_launchGame(WEPLAY_TEMP_MAP_PATH))
    {
        SDL_snprintf(s_errorMessage, sizeof(s_errorMessage),
                     "Could not launch \"%s\".\n"
                     "Make sure the game binary is in the same directory as the editor.",
                     WEPLAY_GAME_BIN);
        s_openErrorPopup = true;
        return false;
    }

    logInfo("weplay_startPlaytest: launched %s --map %s", WEPLAY_GAME_BIN, WEPLAY_TEMP_MAP_PATH);
    return true;
}

void weplay_startPlaytest(WarEditorContext* ctx)
{
    if (!ctx || !ctx->map)
    {
        SDL_strlcpy(s_errorMessage, "No map is open.", sizeof(s_errorMessage));
        s_openErrorPopup = true;
        return;
    }

    // Always validate before playtest. Any result opens warnings modal,
    // but Play Anyway still allows launching regardless of severity.
    wevalid_run(ctx);
    s_pendingValidationCount = 0;
    wevalid_validate(ctx->map, s_pendingValidation, &s_pendingValidationCount);
    if (s_pendingValidationCount > 0)
    {
        s_openValidationPopup = true;
        s_pendingPlaytest = true;
        s_pendingCtx = ctx;
        return;
    }

    weplay_executeStart(ctx);
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

    if (s_openValidationPopup)
    {
        igOpenPopup_Str("Validation Warnings##modal", 0);
        s_openValidationPopup = false;
    }

    vp = igGetMainViewport();
    centre.x = vp->Pos.x + vp->Size.x * 0.5f;
    centre.y = vp->Pos.y + vp->Size.y * 0.5f;
    igSetNextWindowPos(centre, ImGuiCond_Appearing, (ImVec2){ 0.5f, 0.5f });
    igSetNextWindowSize((ImVec2){ 620.0f, 380.0f }, ImGuiCond_Appearing);

    if (igBeginPopupModal("Validation Warnings##modal", NULL, ImGuiWindowFlags_None))
    {
        igText("Validation found %d issue(s):", s_pendingValidationCount);
        igSeparator();

        if (igBeginChild_Str("##validationlist", (ImVec2){ 0.0f, -40.0f },
                             ImGuiChildFlags_Borders, ImGuiWindowFlags_None))
        {
            for (s32 i = 0; i < s_pendingValidationCount; i++)
            {
                const WarValidationResult* r = &s_pendingValidation[i];
                const char* sev = "INFO";
                if (r->severity == WE_VALID_ERROR) sev = "ERROR";
                else if (r->severity == WE_VALID_WARNING) sev = "WARN";

                if (r->hasTile)
                    igText("[%s] %s (tile %d,%d)", sev, r->message, r->tx, r->ty);
                else
                    igText("[%s] %s", sev, r->message);
            }
        }
        igEndChild();

        if (igButton("Play Anyway", (ImVec2){ 120.0f, 0.0f }))
        {
            if (s_pendingPlaytest && s_pendingCtx)
            {
                weplay_executeStart(s_pendingCtx);
            }
            s_pendingPlaytest = false;
            s_pendingCtx = NULL;
            igCloseCurrentPopup();
        }
        igSameLine(0.0f, 8.0f);
        if (igButton("Cancel", (ImVec2){ 120.0f, 0.0f }))
        {
            s_pendingPlaytest = false;
            s_pendingCtx = NULL;
            igCloseCurrentPopup();
        }

        igEndPopup();
    }
}
