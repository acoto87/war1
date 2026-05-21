#pragma once

#include "war_editor.h"

// Save the current map to a temp file and launch the game binary with
// --map pointing at that file.  Displays an error popup on failure.
void weplay_startPlaytest(WarEditorContext* ctx);

// Must be called once per ImGui frame (after BeginFrame, before EndFrame)
// to render the playtest error popup when it is open.
void weplay_drawErrorPopup(void);
