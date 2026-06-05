#pragma once

#include "war_editor.h"

// ---------------------------------------------------------------------------
// — Inspector Panel
//
// Draws a cimgui window with three collapsing sections:
//
//   Entity      — tile position, type, player, resource kind + amount for
//                 the single selected entity.  Hidden when the selection is
//                 empty or contains more than one entity.
//
//   Per-player  — race combo, gold, lumber, and upgrade toggles for each
//                 of the five players.
//
//   Map         — map name, objectives text, camera start position, and
//                 per-feature checkboxes for allowedFeatures[].
// ---------------------------------------------------------------------------

void weinspect_drawPanel(WarEditorContext* ctx);
