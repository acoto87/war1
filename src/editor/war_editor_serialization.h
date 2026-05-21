#pragma once

// war_editor_serialization.h — .w1m map file save / load (Phase 11)
//
// Binary layout of a .w1m file (all multi-byte values little-endian):
//
//  Offset  Size  Field
//  ------  ----  -----
//   0       4    magic        = 0x57314D41  ("W1MA")
//   4       2    version      = 1
//   6       2    flags        = 0 (reserved)
//   8       8    reserved     = 0
//  16       4    allowId      (u32)
//  20       1    allowedHumanUnits (u8)
//  21       1    allowedOrcsUnits  (u8)
//  22       1    customMap         (u8, always 1)
//  23       1    pad
//  24      22    allowedFeatures[MAX_FEATURES_COUNT]
//  46      50    allowedUpgrades[MAX_UPGRADES_COUNT][MAX_PLAYERS_COUNT]
//  96       2    startX       (u16)
//  98       2    startY       (u16)
// 100       2    tilesetType  (u16)
// 102       2    pad (2-byte align)
// 104      20    lumber[MAX_PLAYERS_COUNT]  (u32 × 5)
// 124      20    gold[MAX_PLAYERS_COUNT]    (u32 × 5)
// 144      20    races[MAX_PLAYERS_COUNT]   (WarRace × 5, sizeof(WarRace)==4)
// 164       3    pad (align to 4)
// 167     512    objectives[MAX_OBJECTIVES_LENGTH]
// 679+          variable-length arrays (count u32 then N × sizeof(element))
//               startEntities, startRoads, startWalls, startGoldmines, startConfigurations
//               followed by: visualData[4096 × u16], passableData[4096 × u16]

#include "war_editor.h"

// -----------------------------------------------------------------------
// On-disk file header (16 bytes, static_assert verified)
// -----------------------------------------------------------------------
#define W1M_MAGIC    0x57314D41u   // 'W' '1' 'M' 'A' in little-endian form
#define W1M_VERSION  1u

typedef struct
{
    u32 magic;          // must equal W1M_MAGIC
    u16 version;        // must equal W1M_VERSION
    u16 flags;          // reserved, must be 0
    u32 reserved[2];    // reserved, must be 0
} W1MFileHeader;

static_assert(sizeof(W1MFileHeader) == 16, "W1MFileHeader must be exactly 16 bytes");

// -----------------------------------------------------------------------
// Save
// -----------------------------------------------------------------------

// Write ctx->map to a .w1m file at path.
// Returns true on success, false on any I/O error.
bool wesave_saveMap(const char* path, const WarEditorContext* ctx);

// -----------------------------------------------------------------------
// Load
// -----------------------------------------------------------------------

// Load a .w1m file into ctx->map.
// Frees the previous map via wemap_free, allocates a fresh one, fills all
// fields, and calls wemap_buildTerrainSprite.
// Returns false on any I/O error or format mismatch (magic / version).
bool wesave_loadMap(const char* path, WarEditorContext* ctx);
