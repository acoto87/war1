#pragma once

#include "war_editor.h"

// -----------------------------------------------------------------------
// WarEditorMap — in-memory representation of a custom scenario map.
//
// Field layout mirrors WarResource.levelInfo plus inline tile arrays.
// Serialised to / from .w1m files (Phase 11).
// Runtime-only fields (terrainSprite, tilesIndex, paletteIndex,
// tilesetIndex) are NOT written to disk.
// -----------------------------------------------------------------------
typedef struct _WarEditorMap
{
    // --- Fields mirroring WarResource.levelInfo ---
    u32   allowId;
    bool  allowedHumanUnits;
    bool  allowedOrcsUnits;
    bool  customMap;
    u8    allowedFeatures[MAX_FEATURES_COUNT];
    u8    allowedUpgrades[MAX_UPGRADES_COUNT][MAX_PLAYERS_COUNT];
    u16   startX;
    u16   startY;
    u16   tilesetType;              // WarMapTilesetType
    u32   lumber[MAX_PLAYERS_COUNT];
    u32   gold[MAX_PLAYERS_COUNT];
    WarRace races[MAX_PLAYERS_COUNT];
    char  objectives[MAX_OBJECTIVES_LENGTH];

    u32               startEntitiesCount;
    WarLevelUnit      startEntities[MAX_ENTITIES_COUNT];

    u32               startRoadsCount;
    WarLevelConstruct startRoads[MAX_CONSTRUCTS_COUNT];

    u32               startWallsCount;
    WarLevelConstruct startWalls[MAX_CONSTRUCTS_COUNT];

    u32               startGoldminesCount;
    WarLevelUnit      startGoldmines[MAX_CUSTOM_MAP_GOLDMINES_COUNT];

    u32                       startConfigurationsCount;
    WarCustomMapConfiguration startConfigurations[MAX_CUSTOM_MAP_CONFIGURATIONS_COUNT];

    // --- Inline tile data (not stored as resource references) ---
    u16 visualData[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];    // 4096 tile indices
    u16 passableData[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];  // 4096 passability flags

    // --- Runtime rendering assets (not serialized to disk) ---
    WarSprite terrainSprite;    // compiled tileset SDL_Texture for canvas rendering
    u16       tilesIndex;       // DATA.WAR resource index for tile->minitile mapping
    u16       paletteIndex;     // DATA.WAR resource index for color palette
    u16       tilesetIndex;     // DATA.WAR resource index for tileset pixel atlas

} WarEditorMap;

// -----------------------------------------------------------------------
// Lifecycle
// -----------------------------------------------------------------------

// Allocate a blank WarEditorMap on globalZone and store in ctx->map.
// tilesetType defaults to MAP_TILESET_FOREST; all tile indices and entity
// counts are zeroed.
bool wemap_createEmpty(WarEditorContext* ctx);

// Destroy the terrain SDL_Texture and free the map allocation.
void wemap_free(WarEditorContext* ctx);

// Load palette + tiles + tileset resources from DATA.WAR and build
// ctx->map->terrainSprite.  Requires ctx->warFile to be valid;
// logs a warning and returns false if it is not.
bool wemap_buildTerrainSprite(WarEditorContext* ctx);

// Move any goldmines stored in startEntities[] into startGoldmines[].
// Used after imports / loads so editor systems can rely on a single
// goldmine representation.
void wemap_normalizeGoldmines(WarEditorMap* map);

// Import an existing DATA.WAR level into the editor map.
//
// levelInfoIndex — DATA.WAR resource index of a DB_ENTRY_TYPE_LEVEL_INFO entry.
// Loads the levelInfo resource, copies all fields into ctx->map (replacing
// any previously created empty map), loads the visual and passable tile arrays,
// and rebuilds the terrain sprite.  Returns false on error.
bool wemap_importFromLevelInfo(WarEditorContext* ctx, s32 levelInfoIndex);

// Reset the editor to a brand-new blank map.
//
// Frees the current map, allocates a fresh empty one with default settings,
// rebuilds the terrain sprite, resets cameraOffset / cameraZoom, clears the
// entity selection, clears currentFilePath / mapName, and clears unsavedChanges.
//
// The caller is responsible for confirming any unsaved-changes dialog BEFORE
// calling this function.
void wemap_newMap(WarEditorContext* ctx);
