# Function Prefix Renaming Plan

This document is the working plan for renaming functions across the codebase.

Naming rule:
- Old name: current function name in the source tree.
- Long new name: `war_<module>_<function>`.
- Short new name: shorter prefix alias chosen for the final refactor.
- The short prefix is the preferred end state.

## Step 1. Rename low-risk utility and support modules first
These modules have low fan-out and are safe to rename before the core gameplay code.

### `src/war_log.h`
| Old name | Long new name | Short new name |
|---|---|---|
| `initLog` | `war_log_initLog` | `wlog_initLog` |
| `__logInternal` | `war_log_internal` | `wlog_internal` |

### `src/war_types.h`
| Old name | Long new name | Short new name |
|---|---|---|
| `equalsS32` | `war_types_equalsS32` | `wt_equalsS32` |
| `compareS32` | `war_types_compareS32` | `wt_compareS32` |
| `equalsVec2` | `war_types_equalsVec2` | `wt_equalsVec2` |
| `equalsRect` | `war_types_equalsRect` | `wt_equalsRect` |

### `src/war_math.h` (DO NOT RENAME THESE!)
| Old name | Long new name | Short new name |
|---|---|---|
| `vec2f` | `vec2f` | `vec2f` |
| `vec2i` | `vec2i` | `vec2i` |
| `vec2Addv` | `vec2Addv` | `vec2Addv` |
| `vec2Addi` | `vec2Addi` | `vec2Addi` |
| `vec2Addf` | `vec2Addf` | `vec2Addf` |
| `vec2Subv` | `vec2Subv` | `vec2Subv` |
| `vec2Subi` | `vec2Subi` | `vec2Subi` |
| `vec2Subf` | `vec2Subf` | `vec2Subf` |
| `vec2Mulf` | `vec2Mulf` | `vec2Mulf` |
| `vec2Muli` | `vec2Muli` | `vec2Muli` |
| `vec2Mulv` | `vec2Mulv` | `vec2Mulv` |
| `vec2Half` | `vec2Half` | `vec2Half` |
| `vec2Translatef` | `vec2Translatef` | `vec2Translatef` |
| `vec2Translatei` | `vec2Translatei` | `vec2Translatei` |
| `vec2Scalef` | `vec2Scalef` | `vec2Scalef` |
| `vec2Scalei` | `vec2Scalei` | `vec2Scalei` |
| `vec2Scalev` | `vec2Scalev` | `vec2Scalev` |
| `vec2Inverse` | `vec2Inverse` | `vec2Inverse` |
| `vec2LengthSqr` | `vec2LengthSqr` | `vec2LengthSqr` |
| `vec2Length` | `vec2Length` | `vec2Length` |
| `vec2DistanceSqr` | `vec2DistanceSqr` | `vec2DistanceSqr` |
| `vec2Distance` | `vec2Distance` | `vec2Distance` |
| `vec2DistanceInTiles` | `vec2DistanceInTiles` | `vec2DistanceInTiles` |
| `vec2Normalize` | `vec2Normalize` | `vec2Normalize` |
| `vec2Dot` | `vec2Dot` | `vec2Dot` |
| `vec2Determinant` | `vec2Determinant` | `vec2Determinant` |
| `vec2Orientation` | `vec2Orientation` | `vec2Orientation` |
| `vec2Angle` | `vec2Angle` | `vec2Angle` |
| `vec2ClockwiseAngle` | `vec2ClockwiseAngle` | `vec2ClockwiseAngle` |
| `vec2Clampf` | `vec2Clampf` | `vec2Clampf` |
| `vec2Clampi` | `vec2Clampi` | `vec2Clampi` |
| `vec2Clampv` | `vec2Clampv` | `vec2Clampv` |
| `vec2Floor` | `vec2Floor` | `vec2Floor` |
| `vec2Ceil` | `vec2Ceil` | `vec2Ceil` |
| `vec2Round` | `vec2Round` | `vec2Round` |
| `vec2Print` | `vec2Print` | `vec2Print` |
| `rectf` | `rectf` | `rectf` |
| `recti` | `recti` | `recti` |
| `rectpf` | `rectpf` | `rectpf` |
| `rectv` | `rectv` | `rectv` |
| `rects` | `rects` | `rects` |
| `rectContainsf` | `rectContainsf` | `rectContainsf` |
| `rectIntersects` | `rectIntersects` | `rectIntersects` |
| `rectScalef` | `rectScalef` | `rectScalef` |
| `rectTranslatef` | `rectTranslatef` | `rectTranslatef` |
| `rectCenter` | `rectCenter` | `rectCenter` |
| `rectExpand` | `rectExpand` | `rectExpand` |
| `getClosestPointOnRect` | `getClosestPointOnRect` | `getClosestPointOnRect` |
| `rectPrint` | `rectPrint` | `rectPrint` |

## Step 2. Rename archive, resource, render, and media modules
These files are still mostly leaf modules, but they are foundational to the runtime.

### `src/war_file.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `loadWarFile` | `war_file_loadWarFile` | `wfile_loadWarFile` |

### `src/war_resources.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `getOrCreateResource` | `war_resources_getOrCreateResource` | `wres_getOrCreateResource` |
| `getPalette` | `war_resources_getPalette` | `wres_getPalette` |
| `loadPaletteResource` | `war_resources_loadPaletteResource` | `wres_loadPaletteResource` |
| `loadImageResource` | `war_resources_loadImageResource` | `wres_loadImageResource` |
| `loadSpriteResource` | `war_resources_loadSpriteResource` | `wres_loadSpriteResource` |
| `loadStartEntities` | `war_resources_loadStartEntities` | `wres_loadStartEntities` |
| `loadStartRoads` | `war_resources_loadStartRoads` | `wres_loadStartRoads` |
| `loadStartWalls` | `war_resources_loadStartWalls` | `wres_loadStartWalls` |
| `loadCustomStartGoldmines` | `war_resources_loadCustomStartGoldmines` | `wres_loadCustomStartGoldmines` |
| `loadCustomStartEntities` | `war_resources_loadCustomStartEntities` | `wres_loadCustomStartEntities` |
| `loadLevelInfo` | `war_resources_loadLevelInfo` | `wres_loadLevelInfo` |
| `loadLevelVisual` | `war_resources_loadLevelVisual` | `wres_loadLevelVisual` |
| `loadLevelPassable` | `war_resources_loadLevelPassable` | `wres_loadLevelPassable` |
| `loadTileset` | `war_resources_loadTileset` | `wres_loadTileset` |
| `loadTiles` | `war_resources_loadTiles` | `wres_loadTiles` |
| `loadText` | `war_resources_loadText` | `wres_loadText` |
| `loadXmi` | `war_resources_loadXmi` | `wres_loadXmi` |
| `loadWave` | `war_resources_loadWave` | `wres_loadWave` |
| `loadVoc` | `war_resources_loadVoc` | `wres_loadVoc` |
| `loadCursor` | `war_resources_loadCursor` | `wres_loadCursor` |
| `loadResource` | `war_resources_loadResource` | `wres_loadResource` |

### `src/war_sprites.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `createSprite` | `war_sprites_createSprite` | `wspr_createSprite` |
| `createSpriteFromFrames` | `war_sprites_createSpriteFromFrames` | `wspr_createSpriteFromFrames` |
| `createSpriteFromResource` | `war_sprites_createSpriteFromResource` | `wspr_createSpriteFromResource` |
| `createSpriteFromResourceIndex` | `war_sprites_createSpriteFromResourceIndex` | `wspr_createSpriteFromResourceIndex` |
| `updateSpriteImage` | `war_sprites_updateSpriteImage` | `wspr_updateSpriteImage` |
| `renderSubSprite` | `war_sprites_renderSubSprite` | `wspr_renderSubSprite` |
| `renderSprite` | `war_sprites_renderSprite` | `wspr_renderSprite` |
| `getSpriteFrame` | `war_sprites_getSpriteFrame` | `wspr_getSpriteFrame` |
| `freeSprite` | `war_sprites_freeSprite` | `wspr_freeSprite` |

### `src/war_font.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `loadFontSprite` | `war_font_loadFontSprite` | `wfont_loadFontSprite` |
| `getAlignmentOffset` | `war_font_getAlignmentOffset` | `wfont_getAlignmentOffset` |
| `getLineAlignmentOffset` | `war_font_getLineAlignmentOffset` | `wfont_getLineAlignmentOffset` |
| `splitTextIntoLines` | `war_font_splitTextIntoLines` | `wfont_splitTextIntoLines` |
| `measureSingleSpriteText` | `war_font_measureSingleSpriteText` | `wfont_measureSingleSpriteText` |
| `measureMultiSpriteText` | `war_font_measureMultiSpriteText` | `wfont_measureMultiSpriteText` |
| `renderSingleSpriteTextSpan` | `war_font_renderSingleSpriteTextSpan` | `wfont_renderSingleSpriteTextSpan` |
| `renderSingleSpriteText` | `war_font_renderSingleSpriteText` | `wfont_renderSingleSpriteText` |
| `renderMultiSpriteText` | `war_font_renderMultiSpriteText` | `wfont_renderMultiSpriteText` |

### `src/war_render.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `renderInit` | `war_render_renderInit` | `wr_renderInit` |
| `renderGetState` | `war_render_renderGetState` | `wr_renderGetState` |
| `renderSave` | `war_render_renderSave` | `wr_renderSave` |
| `renderRestore` | `war_render_renderRestore` | `wr_renderRestore` |
| `renderTranslate` | `war_render_renderTranslate` | `wr_renderTranslate` |
| `renderScale` | `war_render_renderScale` | `wr_renderScale` |
| `renderGlobalAlpha` | `war_render_renderGlobalAlpha` | `wr_renderGlobalAlpha` |
| `renderTransformPoint` | `war_render_renderTransformPoint` | `wr_renderTransformPoint` |
| `renderSetDrawColor` | `war_render_renderSetDrawColor` | `wr_renderSetDrawColor` |
| `renderFillRect` | `war_render_renderFillRect` | `wr_renderFillRect` |
| `renderFillRects` | `war_render_renderFillRects` | `wr_renderFillRects` |
| `renderStrokeRect` | `war_render_renderStrokeRect` | `wr_renderStrokeRect` |
| `renderStrokeLine` | `war_render_renderStrokeLine` | `wr_renderStrokeLine` |
| `renderSubImage` | `war_render_renderSubImage` | `wr_renderSubImage` |

### `src/war_audio.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `getAudioData` | `war_audio_getAudioData` | `wa_getAudioData` |
| `playMidi` | `war_audio_playMidi` | `wa_playMidi` |
| `playWave` | `war_audio_playWave` | `wa_playWave` |
| `initAudio` | `war_audio_initAudio` | `wa_initAudio` |
| `removeAudiosOfType` | `war_audio_removeAudiosOfType` | `wa_removeAudiosOfType` |
| `createAudio` | `war_audio_createAudio` | `wa_createAudio` |
| `createAudioWithPosition` | `war_audio_createAudioWithPosition` | `wa_createAudioWithPosition` |
| `createAudioRandom` | `war_audio_createAudioRandom` | `wa_createAudioRandom` |
| `createAudioRandomWithPosition` | `war_audio_createAudioRandomWithPosition` | `wa_createAudioRandomWithPosition` |
| `playAttackSound` | `war_audio_playAttackSound` | `wa_playAttackSound` |
| `playDudeSelectionSound` | `war_audio_playDudeSelectionSound` | `wa_playDudeSelectionSound` |
| `playBuildingSelectionSound` | `war_audio_playBuildingSelectionSound` | `wa_playBuildingSelectionSound` |
| `playAcknowledgementSound` | `war_audio_playAcknowledgementSound` | `wa_playAcknowledgementSound` |
| `mbReadUIntVar` | `war_audio_mbReadUIntVar` | `wa_mbReadUIntVar` |
| `mbWriteUIntVar` | `war_audio_mbWriteUIntVar` | `wa_mbWriteUIntVar` |
| `compareTokens` | `war_audio_compareTokens` | `wa_compareTokens` |

### `src/war_animations.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `createAnimation` | `war_animations_createAnimation` | `wanim_createAnimation` |
| `createAnimationFromResourceIndex` | `war_animations_createAnimationFromResourceIndex` | `wanim_createAnimationFromResourceIndex` |
| `addAnimation` | `war_animations_addAnimation` | `wanim_addAnimation` |
| `addAnimationFrame` | `war_animations_addAnimationFrame` | `wanim_addAnimationFrame` |
| `addAnimationFrames` | `war_animations_addAnimationFrames` | `wanim_addAnimationFrames` |
| `addAnimationFramesRange` | `war_animations_addAnimationFramesRange` | `wanim_addAnimationFramesRange` |
| `getAnimationDuration` | `war_animations_getAnimationDuration` | `wanim_getAnimationDuration` |
| `freeAnimation` | `war_animations_freeAnimation` | `wanim_freeAnimation` |
| `findAnimationIndex` | `war_animations_findAnimationIndex` | `wanim_findAnimationIndex` |
| `removeAnimation` | `war_animations_removeAnimation` | `wanim_removeAnimation` |
| `resetAnimation` | `war_animations_resetAnimation` | `wanim_resetAnimation` |
| `updateAnimation` | `war_animations_updateAnimation` | `wanim_updateAnimation` |
| `updateAnimations` | `war_animations_updateAnimations` | `wanim_updateAnimations` |
| `findAnimation` | `war_animations_findAnimation` | `wanim_findAnimation` |
| `containsAnimation` | `war_animations_containsAnimation` | `wanim_containsAnimation` |
| `createDamageAnimation` | `war_animations_createDamageAnimation` | `wanim_createDamageAnimation` |
| `createCollapseAnimation` | `war_animations_createCollapseAnimation` | `wanim_createCollapseAnimation` |
| `createExplosionAnimation` | `war_animations_createExplosionAnimation` | `wanim_createExplosionAnimation` |
| `createRainOfFireExplosionAnimation` | `war_animations_createRainOfFireExplosionAnimation` | `wanim_createRainOfFireExplosionAnimation` |
| `createSpellAnimation` | `war_animations_createSpellAnimation` | `wanim_createSpellAnimation` |
| `createPoisonCloudAnimation` | `war_animations_createPoisonCloudAnimation` | `wanim_createPoisonCloudAnimation` |

## Step 3. Rename map, UI, campaign, network, and cheat modules
These are mid-level gameplay modules with moderate fan-out.

### `src/war_campaigns.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `getCampaignData` | `war_campaigns_getCampaignData` | `wcamp_getCampaignData` |
| `checkMap01Objectives` | `war_campaigns_checkMap01Objectives` | `wcamp_checkMap01Objectives` |
| `checkMap02Objectives` | `war_campaigns_checkMap02Objectives` | `wcamp_checkMap02Objectives` |
| `checkCustomMapObjectives` | `war_campaigns_checkCustomMapObjectives` | `wcamp_checkCustomMapObjectives` |

### `src/war_cheats.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `applyCheat` | `war_cheats_applyCheat` | `wcheat_applyCheat` |
| `applyGoldCheat` | `war_cheats_applyGoldCheat` | `wcheat_applyGoldCheat` |
| `applySpellsCheat` | `war_cheats_applySpellsCheat` | `wcheat_applySpellsCheat` |
| `applyUpgradesCheat` | `war_cheats_applyUpgradesCheat` | `wcheat_applyUpgradesCheat` |
| `applyEndCheat` | `war_cheats_applyEndCheat` | `wcheat_applyEndCheat` |
| `applyEnableCheat` | `war_cheats_applyEnableCheat` | `wcheat_applyEnableCheat` |
| `applyGodModeCheat` | `war_cheats_applyGodModeCheat` | `wcheat_applyGodModeCheat` |
| `applyWinCheat` | `war_cheats_applyWinCheat` | `wcheat_applyWinCheat` |
| `applyLossCheat` | `war_cheats_applyLossCheat` | `wcheat_applyLossCheat` |
| `applyFogOfWarCheat` | `war_cheats_applyFogOfWarCheat` | `wcheat_applyFogOfWarCheat` |
| `applySkipHumanCheat` | `war_cheats_applySkipHumanCheat` | `wcheat_applySkipHumanCheat` |
| `applySkipOrcCheat` | `war_cheats_applySkipOrcCheat` | `wcheat_applySkipOrcCheat` |
| `applySpeedCheat` | `war_cheats_applySpeedCheat` | `wcheat_applySpeedCheat` |
| `applyMusicCheat` | `war_cheats_applyMusicCheat` | `wcheat_applyMusicCheat` |
| `applySoundCheat` | `war_cheats_applySoundCheat` | `wcheat_applySoundCheat` |
| `applyMusicVolCheat` | `war_cheats_applyMusicVolCheat` | `wcheat_applyMusicVolCheat` |
| `applySoundVolCheat` | `war_cheats_applySoundVolCheat` | `wcheat_applySoundVolCheat` |
| `applyGlobalScaleCheat` | `war_cheats_applyGlobalScaleCheat` | `wcheat_applyGlobalScaleCheat` |
| `applyGlobalSpeedCheat` | `war_cheats_applyGlobalSpeedCheat` | `wcheat_applyGlobalSpeedCheat` |
| `applyEditCheat` | `war_cheats_applyEditCheat` | `wcheat_applyEditCheat` |
| `applyRainOfFireCheat` | `war_cheats_applyRainOfFireCheat` | `wcheat_applyRainOfFireCheat` |
| `applyAddUnitCheat` | `war_cheats_applyAddUnitCheat` | `wcheat_applyAddUnitCheat` |

### `src/war_cheats_panel.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `setCheatsPanelVisible` | `war_cheats_panel_setCheatsPanelVisible` | `wcheatp_setCheatsPanelVisible` |
| `setCheatsFeedback` | `war_cheats_panel_setCheatsFeedback` | `wcheatp_setCheatsFeedback` |
| `setCheatsFeedbackFormat` | `war_cheats_panel_setCheatsFeedbackFormat` | `wcheatp_setCheatsFeedbackFormat` |
| `createCheatsPanel` | `war_cheats_panel_createCheatsPanel` | `wcheatp_createCheatsPanel` |
| `setCheatText` | `war_cheats_panel_setCheatText` | `wcheatp_setCheatText` |
| `updateCheatsPanel` | `war_cheats_panel_updateCheatsPanel` | `wcheatp_updateCheatsPanel` |

### `src/war_net.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `initNetwork` | `war_net_initNetwork` | `wnet_initNetwork` |
| `cleanNetwork` | `war_net_cleanNetwork` | `wnet_cleanNetwork` |
| `connectToHost` | `war_net_connectToHost` | `wnet_connectToHost` |
| `requestResource` | `war_net_requestResource` | `wnet_requestResource` |
| `readResponse` | `war_net_readResponse` | `wnet_readResponse` |
| `parseHeadersFromResponse` | `war_net_parseHeadersFromResponse` | `wnet_parseHeadersFromResponse` |
| `downloadFileFromUrl` | `war_net_downloadFileFromUrl` | `wnet_downloadFileFromUrl` |

### `src/war_ai.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `createAI` | `war_ai_createAI` | `wai_createAI` |
| `createAICommand` | `war_ai_createAICommand` | `wai_createAICommand` |
| `createUnitRequest` | `war_ai_createUnitRequest` | `wai_createUnitRequest` |
| `createWaitForUnit` | `war_ai_createWaitForUnit` | `wai_createWaitForUnit` |
| `createSleepForTime` | `war_ai_createSleepForTime` | `wai_createSleepForTime` |
| `initAI` | `war_ai_initAI` | `wai_initAI` |
| `getNextAICommand` | `war_ai_getNextAICommand` | `wai_getNextAICommand` |
| `initAIPlayer` | `war_ai_initAIPlayer` | `wai_initAIPlayer` |
| `initAIPlayers` | `war_ai_initAIPlayers` | `wai_initAIPlayers` |
| `tryCreateUnit` | `war_ai_tryCreateUnit` | `wai_tryCreateUnit` |
| `executeRequestAICommand` | `war_ai_executeRequestAICommand` | `wai_executeRequestAICommand` |
| `executeWaitAICommand` | `war_ai_executeWaitAICommand` | `wai_executeWaitAICommand` |
| `executeSleepAICommand` | `war_ai_executeSleepAICommand` | `wai_executeSleepAICommand` |
| `executeAICommand` | `war_ai_executeAICommand` | `wai_executeAICommand` |
| `updateAICurrentCommands` | `war_ai_updateAICurrentCommands` | `wai_updateAICurrentCommands` |
| `updateAINextCommands` | `war_ai_updateAINextCommands` | `wai_updateAINextCommands` |
| `removeCompletedAICommands` | `war_ai_removeCompletedAICommands` | `wai_removeCompletedAICommands` |
| `updateAIPlayer` | `war_ai_updateAIPlayer` | `wai_updateAIPlayer` |
| `updateAIPlayers` | `war_ai_updateAIPlayers` | `wai_updateAIPlayers` |

### `src/war_map.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `addEntityToSelection` | `war_map_addEntityToSelection` | `wmap_addEntityToSelection` |
| `removeEntityFromSelection` | `war_map_removeEntityFromSelection` | `wmap_removeEntityFromSelection` |
| `clearSelection` | `war_map_clearSelection` | `wmap_clearSelection` |
| `vec2ScreenToMapCoordinates` | `war_map_vec2ScreenToMapCoordinates` | `wmap_vec2ScreenToMapCoordinates` |
| `vec2ScreenToMinimapCoordinates` | `war_map_vec2ScreenToMinimapCoordinates` | `wmap_vec2ScreenToMinimapCoordinates` |
| `rectScreenToMapCoordinates` | `war_map_rectScreenToMapCoordinates` | `wmap_rectScreenToMapCoordinates` |
| `vec2MapToScreenCoordinates` | `war_map_vec2MapToScreenCoordinates` | `wmap_vec2MapToScreenCoordinates` |
| `rectMapToScreenCoordinates` | `war_map_rectMapToScreenCoordinates` | `wmap_rectMapToScreenCoordinates` |
| `vec2MapToTileCoordinates` | `war_map_vec2MapToTileCoordinates` | `wmap_vec2MapToTileCoordinates` |
| `vec2TileToMapCoordinates` | `war_map_vec2TileToMapCoordinates` | `wmap_vec2TileToMapCoordinates` |
| `vec2MinimapToViewportCoordinates` | `war_map_vec2MinimapToViewportCoordinates` | `wmap_vec2MinimapToViewportCoordinates` |
| `getMapTileState` | `war_map_getMapTileState` | `wmap_getMapTileState` |
| `setMapTileState` | `war_map_setMapTileState` | `wmap_setMapTileState` |
| `setUnitMapTileState` | `war_map_setUnitMapTileState` | `wmap_setUnitMapTileState` |
| `isTileInState` | `war_map_isTileInState` | `wmap_isTileInState` |
| `isAnyTileInStates` | `war_map_isAnyTileInStates` | `wmap_isAnyTileInStates` |
| `isAnyUnitTileInStates` | `war_map_isAnyUnitTileInStates` | `wmap_isAnyUnitTileInStates` |
| `areAllTilesInState` | `war_map_areAllTilesInState` | `wmap_areAllTilesInState` |
| `areAllUnitTilesInState` | `war_map_areAllUnitTilesInState` | `wmap_areAllUnitTilesInState` |
| `updateMinimapTile` | `war_map_updateMinimapTile` | `wmap_updateMinimapTile` |
| `getMapTileIndex` | `war_map_getMapTileIndex` | `wmap_getMapTileIndex` |
| `setMapTileIndex` | `war_map_setMapTileIndex` | `wmap_setMapTileIndex` |
| `getMapScaledSpeed` | `war_map_getMapScaledSpeed` | `wmap_getMapScaledSpeed` |
| `getMapScaledTime` | `war_map_getMapScaledTime` | `wmap_getMapScaledTime` |
| `createMap` | `war_map_createMap` | `wmap_createMap` |
| `createCustomMap` | `war_map_createCustomMap` | `wmap_createCustomMap` |
| `freeMap` | `war_map_freeMap` | `wmap_freeMap` |
| `enterMap` | `war_map_enterMap` | `wmap_enterMap` |
| `leaveMap` | `war_map_leaveMap` | `wmap_leaveMap` |
| `updateViewport` | `war_map_updateViewport` | `wmap_updateViewport` |
| `updateDragRect` | `war_map_updateDragRect` | `wmap_updateDragRect` |
| `updateSelection` | `war_map_updateSelection` | `wmap_updateSelection` |
| `updateTreesEdit` | `war_map_updateTreesEdit` | `wmap_updateTreesEdit` |
| `updateRoadsEdit` | `war_map_updateRoadsEdit` | `wmap_updateRoadsEdit` |
| `updateWallsEdit` | `war_map_updateWallsEdit` | `wmap_updateWallsEdit` |
| `updateRuinsEdit` | `war_map_updateRuinsEdit` | `wmap_updateRuinsEdit` |
| `updateRainOfFireEdit` | `war_map_updateRainOfFireEdit` | `wmap_updateRainOfFireEdit` |
| `updateAddUnit` | `war_map_updateAddUnit` | `wmap_updateAddUnit` |
| `updateCommandButtons` | `war_map_updateCommandButtons` | `wmap_updateCommandButtons` |
| `updateCommandFromRightClick` | `war_map_updateCommandFromRightClick` | `wmap_updateCommandFromRightClick` |
| `updateStatus` | `war_map_updateStatus` | `wmap_updateStatus` |
| `updateMapCursor` | `war_map_updateMapCursor` | `wmap_updateMapCursor` |
| `updateStateMachines` | `war_map_updateStateMachines` | `wmap_updateStateMachines` |
| `updateActions` | `war_map_updateActions` | `wmap_updateActions` |
| `updateProjectiles` | `war_map_updateProjectiles` | `wmap_updateProjectiles` |
| `updateMagic` | `war_map_updateMagic` | `wmap_updateMagic` |
| `updatePoisonCloud` | `war_map_updatePoisonCloud` | `wmap_updatePoisonCloud` |
| `updateSight` | `war_map_updateSight` | `wmap_updateSight` |
| `updateSpells` | `war_map_updateSpells` | `wmap_updateSpells` |
| `updateFoW` | `war_map_updateFoW` | `wmap_updateFoW` |
| `determineFoWTypes` | `war_map_determineFoWTypes` | `wmap_determineFoWTypes` |
| `getCampaignMapTypeByLevelInfoIndex` | `war_map_getCampaignMapTypeByLevelInfoIndex` | `wmap_getCampaignMapTypeByLevelInfoIndex` |
| `checkObjectives` | `war_map_checkObjectives` | `wmap_checkObjectives` |
| `updateObjectives` | `war_map_updateObjectives` | `wmap_updateObjectives` |
| `updateMap` | `war_map_updateMap` | `wmap_updateMap` |
| `renderTerrain` | `war_map_renderTerrain` | `wmap_renderTerrain` |
| `renderFoW` | `war_map_renderFoW` | `wmap_renderFoW` |
| `renderUnitPaths` | `war_map_renderUnitPaths` | `wmap_renderUnitPaths` |
| `renderPassableInfo` | `war_map_renderPassableInfo` | `wmap_renderPassableInfo` |
| `renderMapGrid` | `war_map_renderMapGrid` | `wmap_renderMapGrid` |
| `renderMapPanel` | `war_map_renderMapPanel` | `wmap_renderMapPanel` |
| `renderMap` | `war_map_renderMap` | `wmap_renderMap` |

### `src/war_map_ui.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `createMapUI` | `war_map_ui_createMapUI` | `wmui_createMapUI` |
| `createUIMinimap` | `war_map_ui_createUIMinimap` | `wmui_createUIMinimap` |
| `updateGoldText` | `war_map_ui_updateGoldText` | `wmui_updateGoldText` |
| `updateWoodText` | `war_map_ui_updateWoodText` | `wmui_updateWoodText` |
| `updateSelectedUnitsInfo` | `war_map_ui_updateSelectedUnitsInfo` | `wmui_updateSelectedUnitsInfo` |
| `setStatus` | `war_map_ui_setStatus` | `wmui_setStatus` |
| `setFlashStatus` | `war_map_ui_setFlashStatus` | `wmui_setFlashStatus` |
| `setLifeBar` | `war_map_ui_setLifeBar` | `wmui_setLifeBar` |
| `setManaBar` | `war_map_ui_setManaBar` | `wmui_setManaBar` |
| `setPercentBar` | `war_map_ui_setPercentBar` | `wmui_setPercentBar` |
| `renderSelectionRect` | `war_map_ui_renderSelectionRect` | `wmui_renderSelectionRect` |
| `renderCommand` | `war_map_ui_renderCommand` | `wmui_renderCommand` |
| `renderMapUI` | `war_map_ui_renderMapUI` | `wmui_renderMapUI` |

### `src/war_map_menu.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `setSpeedStr` | `war_map_menu_setSpeedStr` | `wmm_setSpeedStr` |
| `setUITextSpeedValueByName` | `war_map_menu_setUITextSpeedValueByName` | `wmm_setUITextSpeedValueByName` |
| `setUITextS32ValueByName` | `war_map_menu_setUITextS32ValueByName` | `wmm_setUITextS32ValueByName` |
| `createMenu` | `war_map_menu_createMenu` | `wmm_createMenu` |
| `createOptionsMenu` | `war_map_menu_createOptionsMenu` | `wmm_createOptionsMenu` |
| `createObjectivesMenu` | `war_map_menu_createObjectivesMenu` | `wmm_createObjectivesMenu` |
| `createRestartMenu` | `war_map_menu_createRestartMenu` | `wmm_createRestartMenu` |
| `createGameOverMenu` | `war_map_menu_createGameOverMenu` | `wmm_createGameOverMenu` |
| `createQuitMenu` | `war_map_menu_createQuitMenu` | `wmm_createQuitMenu` |
| `createDemoEndMenu` | `war_map_menu_createDemoEndMenu` | `wmm_createDemoEndMenu` |
| `enableOrDisableCommandButtons` | `war_map_menu_enableOrDisableCommandButtons` | `wmm_enableOrDisableCommandButtons` |
| `showOrHideMenu` | `war_map_menu_showOrHideMenu` | `wmm_showOrHideMenu` |
| `showOrHideOptionsMenu` | `war_map_menu_showOrHideOptionsMenu` | `wmm_showOrHideOptionsMenu` |
| `showOrHideObjectivesMenu` | `war_map_menu_showOrHideObjectivesMenu` | `wmm_showOrHideObjectivesMenu` |
| `showOrHideRestartMenu` | `war_map_menu_showOrHideRestartMenu` | `wmm_showOrHideRestartMenu` |
| `showOrHideGameOverMenu` | `war_map_menu_showOrHideGameOverMenu` | `wmm_showOrHideGameOverMenu` |
| `showOrHideQuitMenu` | `war_map_menu_showOrHideQuitMenu` | `wmm_showOrHideQuitMenu` |
| `showDemoEndMenu` | `war_map_menu_showDemoEndMenu` | `wmm_showDemoEndMenu` |
| `handleMenu` | `war_map_menu_handleMenu` | `wmm_handleMenu` |
| `handleOptions` | `war_map_menu_handleOptions` | `wmm_handleOptions` |
| `handleObjectives` | `war_map_menu_handleObjectives` | `wmm_handleObjectives` |
| `handleRestart` | `war_map_menu_handleRestart` | `wmm_handleRestart` |
| `handleContinue` | `war_map_menu_handleContinue` | `wmm_handleContinue` |
| `handleQuit` | `war_map_menu_handleQuit` | `wmm_handleQuit` |
| `handleGameSpeedDec` | `war_map_menu_handleGameSpeedDec` | `wmm_handleGameSpeedDec` |
| `handleGameSpeedInc` | `war_map_menu_handleGameSpeedInc` | `wmm_handleGameSpeedInc` |
| `handleMusicVolDec` | `war_map_menu_handleMusicVolDec` | `wmm_handleMusicVolDec` |
| `handleMusicVolInc` | `war_map_menu_handleMusicVolInc` | `wmm_handleMusicVolInc` |
| `handleSfxVolDec` | `war_map_menu_handleSfxVolDec` | `wmm_handleSfxVolDec` |
| `handleSfxVolInc` | `war_map_menu_handleSfxVolInc` | `wmm_handleSfxVolInc` |
| `handleMouseScrollSpeedDec` | `war_map_menu_handleMouseScrollSpeedDec` | `wmm_handleMouseScrollSpeedDec` |
| `handleMouseScrollSpeedInc` | `war_map_menu_handleMouseScrollSpeedInc` | `wmm_handleMouseScrollSpeedInc` |
| `handleKeyScrollSpeedDec` | `war_map_menu_handleKeyScrollSpeedDec` | `wmm_handleKeyScrollSpeedDec` |
| `handleKeyScrollSpeedInc` | `war_map_menu_handleKeyScrollSpeedInc` | `wmm_handleKeyScrollSpeedInc` |
| `handleOptionsOk` | `war_map_menu_handleOptionsOk` | `wmm_handleOptionsOk` |
| `handleOptionsCancel` | `war_map_menu_handleOptionsCancel` | `wmm_handleOptionsCancel` |
| `handleObjectivesMenu` | `war_map_menu_handleObjectivesMenu` | `wmm_handleObjectivesMenu` |
| `handleRestartRestart` | `war_map_menu_handleRestartRestart` | `wmm_handleRestartRestart` |
| `handleRestartCancel` | `war_map_menu_handleRestartCancel` | `wmm_handleRestartCancel` |

## Step 4. Rename gameplay engine modules
These modules are more interconnected, so they should move after the support layers above are stable.

### `src/war_entities.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `addTransformComponent` | `war_entities_addTransformComponent` | `we_addTransformComponent` |
| `removeTransformComponent` | `war_entities_removeTransformComponent` | `we_removeTransformComponent` |
| `addSpriteComponent` | `war_entities_addSpriteComponent` | `we_addSpriteComponent` |
| `addSpriteComponentFromResource` | `war_entities_addSpriteComponentFromResource` | `we_addSpriteComponentFromResource` |
| `removeSpriteComponent` | `war_entities_removeSpriteComponent` | `we_removeSpriteComponent` |
| `addUnitComponent` | `war_entities_addUnitComponent` | `we_addUnitComponent` |
| `removeUnitComponent` | `war_entities_removeUnitComponent` | `we_removeUnitComponent` |
| `addRoadComponent` | `war_entities_addRoadComponent` | `we_addRoadComponent` |
| `removeRoadComponent` | `war_entities_removeRoadComponent` | `we_removeRoadComponent` |
| `addWallComponent` | `war_entities_addWallComponent` | `we_addWallComponent` |
| `removeWallComponent` | `war_entities_removeWallComponent` | `we_removeWallComponent` |
| `addRuinComponent` | `war_entities_addRuinComponent` | `we_addRuinComponent` |
| `removeRuinComponent` | `war_entities_removeRuinComponent` | `we_removeRuinComponent` |
| `addForestComponent` | `war_entities_addForestComponent` | `we_addForestComponent` |
| `removeForestComponent` | `war_entities_removeForestComponent` | `we_removeForestComponent` |
| `addStateMachineComponent` | `war_entities_addStateMachineComponent` | `we_addStateMachineComponent` |
| `removeStateMachineComponent` | `war_entities_removeStateMachineComponent` | `we_removeStateMachineComponent` |
| `addAnimationsComponent` | `war_entities_addAnimationsComponent` | `we_addAnimationsComponent` |
| `removeAnimationsComponent` | `war_entities_removeAnimationsComponent` | `we_removeAnimationsComponent` |
| `addUIComponent` | `war_entities_addUIComponent` | `we_addUIComponent` |
| `removeUIComponent` | `war_entities_removeUIComponent` | `we_removeUIComponent` |
| `addTextComponent` | `war_entities_addTextComponent` | `we_addTextComponent` |
| `removeTextComponent` | `war_entities_removeTextComponent` | `we_removeTextComponent` |
| `addRectComponent` | `war_entities_addRectComponent` | `we_addRectComponent` |
| `removeRectComponent` | `war_entities_removeRectComponent` | `we_removeRectComponent` |
| `addButtonComponent` | `war_entities_addButtonComponent` | `we_addButtonComponent` |
| `addButtonComponentFromResource` | `war_entities_addButtonComponentFromResource` | `we_addButtonComponentFromResource` |
| `removeButtonComponent` | `war_entities_removeButtonComponent` | `we_removeButtonComponent` |
| `addAudioComponent` | `war_entities_addAudioComponent` | `we_addAudioComponent` |
| `removeAudioComponent` | `war_entities_removeAudioComponent` | `we_removeAudioComponent` |
| `addCursorComponent` | `war_entities_addCursorComponent` | `we_addCursorComponent` |
| `removeCursorComponent` | `war_entities_removeCursorComponent` | `we_removeCursorComponent` |
| `addProjectileComponent` | `war_entities_addProjectileComponent` | `we_addProjectileComponent` |
| `removeProjectileComponent` | `war_entities_removeProjectileComponent` | `we_removeProjectileComponent` |
| `addPoisonCloudComponent` | `war_entities_addPoisonCloudComponent` | `we_addPoisonCloudComponent` |
| `removePoisonCloudComponent` | `war_entities_removePoisonCloudComponent` | `we_removePoisonCloudComponent` |
| `addSightComponent` | `war_entities_addSightComponent` | `we_addSightComponent` |
| `removeSightComponent` | `war_entities_removeSightComponent` | `we_removeSightComponent` |
| `createEntity` | `war_entities_createEntity` | `we_createEntity` |
| `createUnit` | `war_entities_createUnit` | `we_createUnit` |
| `createDude` | `war_entities_createDude` | `we_createDude` |
| `createBuilding` | `war_entities_createBuilding` | `we_createBuilding` |
| `findEntity` | `war_entities_findEntity` | `we_findEntity` |
| `findClosestUnitOfType` | `war_entities_findClosestUnitOfType` | `we_findClosestUnitOfType` |
| `findUIEntity` | `war_entities_findUIEntity` | `we_findUIEntity` |
| `findEntityUnderCursor` | `war_entities_findEntityUnderCursor` | `we_findEntityUnderCursor` |
| `removeEntity` | `war_entities_removeEntity` | `we_removeEntity` |
| `removeEntityById` | `war_entities_removeEntityById` | `we_removeEntityById` |
| `isStaticEntity` | `war_entities_isStaticEntity` | `we_isStaticEntity` |
| `initEntityManager` | `war_entities_initEntityManager` | `we_initEntityManager` |
| `getEntityManager` | `war_entities_getEntityManager` | `we_getEntityManager` |
| `getEntities` | `war_entities_getEntities` | `we_getEntities` |
| `getEntitiesOfType` | `war_entities_getEntitiesOfType` | `we_getEntitiesOfType` |
| `getUnitsOfType` | `war_entities_getUnitsOfType` | `we_getUnitsOfType` |
| `getUIEntities` | `war_entities_getUIEntities` | `we_getUIEntities` |
| `renderCompareUnits` | `war_entities_renderCompareUnits` | `we_renderCompareUnits` |
| `renderCompareProjectiles` | `war_entities_renderCompareProjectiles` | `we_renderCompareProjectiles` |
| `renderImage` | `war_entities_renderImage` | `we_renderImage` |
| `renderRoad` | `war_entities_renderRoad` | `we_renderRoad` |
| `renderWall` | `war_entities_renderWall` | `we_renderWall` |
| `renderRuin` | `war_entities_renderRuin` | `we_renderRuin` |
| `renderForest` | `war_entities_renderForest` | `we_renderForest` |
| `renderUnit` | `war_entities_renderUnit` | `we_renderUnit` |
| `renderText` | `war_entities_renderText` | `we_renderText` |
| `renderRect` | `war_entities_renderRect` | `we_renderRect` |
| `renderButton` | `war_entities_renderButton` | `we_renderButton` |
| `renderProjectile` | `war_entities_renderProjectile` | `we_renderProjectile` |
| `renderMinimap` | `war_entities_renderMinimap` | `we_renderMinimap` |
| `renderAnimation` | `war_entities_renderAnimation` | `we_renderAnimation` |
| `renderEntity` | `war_entities_renderEntity` | `we_renderEntity` |
| `renderEntitiesOfType` | `war_entities_renderEntitiesOfType` | `we_renderEntitiesOfType` |
| `renderUnitSelection` | `war_entities_renderUnitSelection` | `we_renderUnitSelection` |
| `increaseUpgradeLevel` | `war_entities_increaseUpgradeLevel` | `we_increaseUpgradeLevel` |
| `enoughPlayerResources` | `war_entities_enoughPlayerResources` | `we_enoughPlayerResources` |
| `decreasePlayerResources` | `war_entities_decreasePlayerResources` | `we_decreasePlayerResources` |
| `increasePlayerResources` | `war_entities_increasePlayerResources` | `we_increasePlayerResources` |
| `increaseUnitHp` | `war_entities_increaseUnitHp` | `we_increaseUnitHp` |
| `decreaseUnitHp` | `war_entities_decreaseUnitHp` | `we_decreaseUnitHp` |
| `decreaseUnitMana` | `war_entities_decreaseUnitMana` | `we_decreaseUnitMana` |
| `increaseUnitMana` | `war_entities_increaseUnitMana` | `we_increaseUnitMana` |
| `enoughFarmFood` | `war_entities_enoughFarmFood` | `we_enoughFarmFood` |
| `checkFarmFood` | `war_entities_checkFarmFood` | `we_checkFarmFood` |
| `checkRectToBuild` | `war_entities_checkRectToBuild` | `we_checkRectToBuild` |
| `checkTileToBuild` | `war_entities_checkTileToBuild` | `we_checkTileToBuild` |
| `checkTileToBuildRoadOrWall` | `war_entities_checkTileToBuildRoadOrWall` | `we_checkTileToBuildRoadOrWall` |
| `getNearUnits` | `war_entities_getNearUnits` | `we_getNearUnits` |
| `getNearEnemy` | `war_entities_getNearEnemy` | `we_getNearEnemy` |
| `isBeingAttackedBy` | `war_entities_isBeingAttackedBy` | `we_isBeingAttackedBy` |
| `isBeingAttacked` | `war_entities_isBeingAttacked` | `we_isBeingAttacked` |
| `getAttacker` | `war_entities_getAttacker` | `we_getAttacker` |
| `getAttackTarget` | `war_entities_getAttackTarget` | `we_getAttackTarget` |
| `getTotalDamage` | `war_entities_getTotalDamage` | `we_getTotalDamage` |
| `takeDamage` | `war_entities_takeDamage` | `we_takeDamage` |
| `takeWallDamage` | `war_entities_takeWallDamage` | `we_takeWallDamage` |
| `rangeAttack` | `war_entities_rangeAttack` | `we_rangeAttack` |
| `rangeWallAttack` | `war_entities_rangeWallAttack` | `we_rangeWallAttack` |
| `meleeAttack` | `war_entities_meleeAttack` | `we_meleeAttack` |
| `meleeWallAttack` | `war_entities_meleeWallAttack` | `we_meleeWallAttack` |
| `mine` | `war_entities_mine` | `we_mine` |

### `src/war_units.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `isFriendlyUnit` | `war_units_isFriendlyUnit` | `wu_isFriendlyUnit` |
| `isEnemyUnit` | `war_units_isEnemyUnit` | `wu_isEnemyUnit` |
| `areEnemies` | `war_units_areEnemies` | `wu_areEnemies` |
| `canAttack` | `war_units_canAttack` | `wu_canAttack` |
| `displayUnitOnMinimap` | `war_units_displayUnitOnMinimap` | `wu_displayUnitOnMinimap` |
| `getUnitColorOnMinimap` | `war_units_getUnitColorOnMinimap` | `wu_getUnitColorOnMinimap` |
| `getTotalNumberOfUnits` | `war_units_getTotalNumberOfUnits` | `wu_getTotalNumberOfUnits` |
| `getTotalNumberOfDudes` | `war_units_getTotalNumberOfDudes` | `wu_getTotalNumberOfDudes` |
| `getTotalNumberOfBuildings` | `war_units_getTotalNumberOfBuildings` | `wu_getTotalNumberOfBuildings` |
| `getNumberOfBuildingsOfType` | `war_units_getNumberOfBuildingsOfType` | `wu_getNumberOfBuildingsOfType` |
| `getNumberOfUnitsOfType` | `war_units_getNumberOfUnitsOfType` | `wu_getNumberOfUnitsOfType` |
| `getUnitTypeForRace` | `war_units_getUnitTypeForRace` | `wu_getUnitTypeForRace` |
| `getUnitCommands` | `war_units_getUnitCommands` | `wu_getUnitCommands` |
| `getUnitCommandData` | `war_units_getUnitCommandData` | `wu_getUnitCommandData` |

### `src/war_commands.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `executeMoveCommand` | `war_commands_executeMoveCommand` | `wcmd_executeMoveCommand` |
| `executeFollowCommand` | `war_commands_executeFollowCommand` | `wcmd_executeFollowCommand` |
| `executeStopCommand` | `war_commands_executeStopCommand` | `wcmd_executeStopCommand` |
| `executeHarvestCommand` | `war_commands_executeHarvestCommand` | `wcmd_executeHarvestCommand` |
| `executeDeliverCommand` | `war_commands_executeDeliverCommand` | `wcmd_executeDeliverCommand` |
| `executeRepairCommand` | `war_commands_executeRepairCommand` | `wcmd_executeRepairCommand` |
| `executeSummonCommand` | `war_commands_executeSummonCommand` | `wcmd_executeSummonCommand` |
| `executeRainOfFireCommand` | `war_commands_executeRainOfFireCommand` | `wcmd_executeRainOfFireCommand` |
| `executePoisonCloudCommand` | `war_commands_executePoisonCloudCommand` | `wcmd_executePoisonCloudCommand` |
| `executeHealingCommand` | `war_commands_executeHealingCommand` | `wcmd_executeHealingCommand` |
| `executeInvisiblityCommand` | `war_commands_executeInvisiblityCommand` | `wcmd_executeInvisiblityCommand` |
| `executeUnholyArmorCommand` | `war_commands_executeUnholyArmorCommand` | `wcmd_executeUnholyArmorCommand` |
| `executeRaiseDeadCommand` | `war_commands_executeRaiseDeadCommand` | `wcmd_executeRaiseDeadCommand` |
| `executeSightCommand` | `war_commands_executeSightCommand` | `wcmd_executeSightCommand` |
| `executeAttackCommand` | `war_commands_executeAttackCommand` | `wcmd_executeAttackCommand` |
| `executeCommand` | `war_commands_executeCommand` | `wcmd_executeCommand` |
| `trainUnit` | `war_commands_trainUnit` | `wcmd_trainUnit` |
| `trainFootman` | `war_commands_trainFootman` | `wcmd_trainFootman` |
| `trainGrunt` | `war_commands_trainGrunt` | `wcmd_trainGrunt` |
| `trainPeasant` | `war_commands_trainPeasant` | `wcmd_trainPeasant` |
| `trainPeon` | `war_commands_trainPeon` | `wcmd_trainPeon` |
| `trainHumanCatapult` | `war_commands_trainHumanCatapult` | `wcmd_trainHumanCatapult` |
| `trainOrcCatapult` | `war_commands_trainOrcCatapult` | `wcmd_trainOrcCatapult` |
| `trainKnight` | `war_commands_trainKnight` | `wcmd_trainKnight` |
| `trainRaider` | `war_commands_trainRaider` | `wcmd_trainRaider` |
| `trainArcher` | `war_commands_trainArcher` | `wcmd_trainArcher` |
| `trainSpearman` | `war_commands_trainSpearman` | `wcmd_trainSpearman` |
| `trainConjurer` | `war_commands_trainConjurer` | `wcmd_trainConjurer` |
| `trainWarlock` | `war_commands_trainWarlock` | `wcmd_trainWarlock` |
| `trainCleric` | `war_commands_trainCleric` | `wcmd_trainCleric` |
| `trainNecrolyte` | `war_commands_trainNecrolyte` | `wcmd_trainNecrolyte` |
| `upgradeUpgrade` | `war_commands_upgradeUpgrade` | `wcmd_upgradeUpgrade` |
| `upgradeSwords` | `war_commands_upgradeSwords` | `wcmd_upgradeSwords` |
| `upgradeAxes` | `war_commands_upgradeAxes` | `wcmd_upgradeAxes` |
| `upgradeHumanShields` | `war_commands_upgradeHumanShields` | `wcmd_upgradeHumanShields` |
| `upgradeOrcsShields` | `war_commands_upgradeOrcsShields` | `wcmd_upgradeOrcsShields` |
| `upgradeArrows` | `war_commands_upgradeArrows` | `wcmd_upgradeArrows` |
| `upgradeSpears` | `war_commands_upgradeSpears` | `wcmd_upgradeSpears` |
| `upgradeHorses` | `war_commands_upgradeHorses` | `wcmd_upgradeHorses` |
| `upgradeWolves` | `war_commands_upgradeWolves` | `wcmd_upgradeWolves` |
| `upgradeScorpions` | `war_commands_upgradeScorpions` | `wcmd_upgradeScorpions` |
| `upgradeSpiders` | `war_commands_upgradeSpiders` | `wcmd_upgradeSpiders` |
| `upgradeRainOfFire` | `war_commands_upgradeRainOfFire` | `wcmd_upgradeRainOfFire` |
| `upgradePoisonCloud` | `war_commands_upgradePoisonCloud` | `wcmd_upgradePoisonCloud` |
| `upgradeWaterElemental` | `war_commands_upgradeWaterElemental` | `wcmd_upgradeWaterElemental` |
| `upgradeDaemon` | `war_commands_upgradeDaemon` | `wcmd_upgradeDaemon` |
| `upgradeHealing` | `war_commands_upgradeHealing` | `wcmd_upgradeHealing` |
| `upgradeRaiseDead` | `war_commands_upgradeRaiseDead` | `wcmd_upgradeRaiseDead` |
| `upgradeFarSight` | `war_commands_upgradeFarSight` | `wcmd_upgradeFarSight` |
| `upgradeDarkVision` | `war_commands_upgradeDarkVision` | `wcmd_upgradeDarkVision` |
| `upgradeInvisibility` | `war_commands_upgradeInvisibility` | `wcmd_upgradeInvisibility` |
| `upgradeUnholyArmor` | `war_commands_upgradeUnholyArmor` | `wcmd_upgradeUnholyArmor` |
| `trainUnit` | `war_commands_trainUnit` | `wcmd_trainUnit` |
| `buildBuilding` | `war_commands_buildBuilding` | `wcmd_buildBuilding` |
| `upgradeUpgrade` | `war_commands_upgradeUpgrade` | `wcmd_upgradeUpgrade` |
| `cancel` | `war_commands_cancel` | `wcmd_cancel` |
| `move` | `war_commands_move` | `wcmd_move` |
| `stop` | `war_commands_stop` | `wcmd_stop` |
| `harvest` | `war_commands_harvest` | `wcmd_harvest` |
| `deliver` | `war_commands_deliver` | `wcmd_deliver` |
| `repair` | `war_commands_repair` | `wcmd_repair` |
| `attack` | `war_commands_attack` | `wcmd_attack` |
| `buildBasic` | `war_commands_buildBasic` | `wcmd_buildBasic` |
| `buildAdvanced` | `war_commands_buildAdvanced` | `wcmd_buildAdvanced` |
| `buildFarmHumans` | `war_commands_buildFarmHumans` | `wcmd_buildFarmHumans` |
| `buildFarmOrcs` | `war_commands_buildFarmOrcs` | `wcmd_buildFarmOrcs` |
| `buildBarracksHumans` | `war_commands_buildBarracksHumans` | `wcmd_buildBarracksHumans` |
| `buildBarracksOrcs` | `war_commands_buildBarracksOrcs` | `wcmd_buildBarracksOrcs` |
| `buildChurch` | `war_commands_buildChurch` | `wcmd_buildChurch` |
| `buildTemple` | `war_commands_buildTemple` | `wcmd_buildTemple` |
| `buildTowerHumans` | `war_commands_buildTowerHumans` | `wcmd_buildTowerHumans` |
| `buildTowerOrcs` | `war_commands_buildTowerOrcs` | `wcmd_buildTowerOrcs` |
| `buildTownHallHumans` | `war_commands_buildTownHallHumans` | `wcmd_buildTownHallHumans` |
| `buildTownHallOrcs` | `war_commands_buildTownHallOrcs` | `wcmd_buildTownHallOrcs` |
| `buildLumbermillHumans` | `war_commands_buildLumbermillHumans` | `wcmd_buildLumbermillHumans` |
| `buildLumbermillOrcs` | `war_commands_buildLumbermillOrcs` | `wcmd_buildLumbermillOrcs` |
| `buildStable` | `war_commands_buildStable` | `wcmd_buildStable` |
| `buildKennel` | `war_commands_buildKennel` | `wcmd_buildKennel` |
| `buildBlacksmithHumans` | `war_commands_buildBlacksmithHumans` | `wcmd_buildBlacksmithHumans` |
| `buildBlacksmithOrcs` | `war_commands_buildBlacksmithOrcs` | `wcmd_buildBlacksmithOrcs` |
| `buildWall` | `war_commands_buildWall` | `wcmd_buildWall` |
| `buildRoad` | `war_commands_buildRoad` | `wcmd_buildRoad` |
| `castRainOfFire` | `war_commands_castRainOfFire` | `wcmd_castRainOfFire` |
| `castPoisonCloud` | `war_commands_castPoisonCloud` | `wcmd_castPoisonCloud` |
| `castHeal` | `war_commands_castHeal` | `wcmd_castHeal` |
| `castFarSight` | `war_commands_castFarSight` | `wcmd_castFarSight` |
| `castDarkVision` | `war_commands_castDarkVision` | `wcmd_castDarkVision` |
| `castInvisibility` | `war_commands_castInvisibility` | `wcmd_castInvisibility` |
| `castUnHolyArmor` | `war_commands_castUnHolyArmor` | `wcmd_castUnHolyArmor` |
| `castRaiseDead` | `war_commands_castRaiseDead` | `wcmd_castRaiseDead` |
| `summonSpider` | `war_commands_summonSpider` | `wcmd_summonSpider` |
| `summonScorpion` | `war_commands_summonScorpion` | `wcmd_summonScorpion` |
| `summonDaemon` | `war_commands_summonDaemon` | `wcmd_summonDaemon` |
| `summonWaterElemental` | `war_commands_summonWaterElemental` | `wcmd_summonWaterElemental` |

### `src/war_actions.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `getFrameNumbers` | `war_actions_getFrameNumbers` | `wact_getFrameNumbers` (static helper) |
| `resetAction` | `war_actions_resetAction` | `wact_resetAction` (internal helper) |
| `createUnitAction` | `war_actions_createUnitAction` | `wact_createUnitAction` |
| `addActionStep` | `war_actions_addActionStep` | `wact_addActionStep` |
| `buildWalkAction` | `war_actions_buildWalkAction` | `wact_buildWalkAction` |
| `buildAttackAction` | `war_actions_buildAttackAction` | `wact_buildAttackAction` |
| `buildRepairAction` | `war_actions_buildRepairAction` | `wact_buildRepairAction` |
| `buildHarvestAction` | `war_actions_buildHarvestAction` | `wact_buildHarvestAction` |
| `buildDeathAction` | `war_actions_buildDeathAction` | `wact_buildDeathAction` |
| `buildBuildAction` | `war_actions_buildBuildAction` | `wact_buildBuildAction` |
| `buildIdleAction` | `war_actions_buildIdleAction` | `wact_buildIdleAction` |
| `buildDefaultIdleAction` | `war_actions_buildDefaultIdleAction` | `wact_buildDefaultIdleAction` |
| `addActions` | `war_actions_addActions` | `wact_addActions` |
| `addUnitActions` | `war_actions_addUnitActions` | `wact_addUnitActions` |
| `findActionIndex` | `war_actions_findActionIndex` | `wact_findActionIndex` |
| `getActionDuration` | `war_actions_getActionDuration` | `wact_getActionDuration` |
| `resetAction` | `war_actions_resetAction` | `wact_resetAction` |
| `setAction` | `war_actions_setAction` | `wact_setAction` |
| `updateAction` | `war_actions_updateAction` | `wact_updateAction` |

### `src/war_projectiles.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `getProjectileData` | `war_projectiles_getProjectileData` | `wproj_getProjectileData` |
| `doProjectileTargetDamage` | `war_projectiles_doProjectileTargetDamage` | `wproj_doProjectileTargetDamage` |
| `doProjectileSplashDamage` | `war_projectiles_doProjectileSplashDamage` | `wproj_doProjectileSplashDamage` |
| `doRainOfFireProjectileSplashDamage` | `war_projectiles_doRainOfFireProjectileSplashDamage` | `wproj_doRainOfFireProjectileSplashDamage` |
| `updateProjectilePosition` | `war_projectiles_updateProjectilePosition` | `wproj_updateProjectilePosition` |
| `updateProjectileSprite` | `war_projectiles_updateProjectileSprite` | `wproj_updateProjectileSprite` |
| `updateRainOfFireProjectileSprite` | `war_projectiles_updateRainOfFireProjectileSprite` | `wproj_updateRainOfFireProjectileSprite` |
| `updateProjectile` | `war_projectiles_updateProjectile` | `wproj_updateProjectile` |
| `createProjectile` | `war_projectiles_createProjectile` | `wproj_createProjectile` |

### `src/war_pathfinder.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `getTileValue` | `war_pathfinder_getTileValue` | `wpath_getTileValue` |
| `setTilesValue` | `war_pathfinder_setTilesValue` | `wpath_setTilesValue` |
| `initPathFinder` | `war_pathfinder_initPathFinder` | `wpath_initPathFinder` |
| `isInside` | `war_pathfinder_isInside` | `wpath_isInside` |
| `findPath` | `war_pathfinder_findPath` | `wpath_findPath` |
| `reRoutePath` | `war_pathfinder_reRoutePath` | `wpath_reRoutePath` |
| `pathExists` | `war_pathfinder_pathExists` | `wpath_pathExists` |
| `freePath` | `war_pathfinder_freePath` | `wpath_freePath` |
| `findEmptyPosition` | `war_pathfinder_findEmptyPosition` | `wpath_findEmptyPosition` |
| `isPositionAccesible` | `war_pathfinder_isPositionAccesible` | `wpath_isPositionAccesible` |

## Step 5. Rename roads, walls, ruins, and trees
These are map-structure modules that are still fairly isolated.

### `src/war_roads.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `hasRoadPieceAtPosition` | `war_roads_hasRoadPieceAtPosition` | `wroad_hasRoadPieceAtPosition` |
| `getRoadPieceAtPosition` | `war_roads_getRoadPieceAtPosition` | `wroad_getRoadPieceAtPosition` |
| `determineRoadTypes` | `war_roads_determineRoadTypes` | `wroad_determineRoadTypes` |
| `addRoadPiece` | `war_roads_addRoadPiece` | `wroad_addRoadPiece` |
| `addRoadPiecesFromConstruct` | `war_roads_addRoadPiecesFromConstruct` | `wroad_addRoadPiecesFromConstruct` |
| `removeRoadPiece` | `war_roads_removeRoadPiece` | `wroad_removeRoadPiece` |
| `createRoad` | `war_roads_createRoad` | `wroad_createRoad` |

### `src/war_walls.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `hasWallPieceAtPosition` | `war_walls_hasWallPieceAtPosition` | `wwall_hasWallPieceAtPosition` |
| `getWallPieceAtPosition` | `war_walls_getWallPieceAtPosition` | `wwall_getWallPieceAtPosition` |
| `determineWallTypes` | `war_walls_determineWallTypes` | `wwall_determineWallTypes` |
| `addWallPiece` | `war_walls_addWallPiece` | `wwall_addWallPiece` |
| `addWallPiecesFromConstruct` | `war_walls_addWallPiecesFromConstruct` | `wwall_addWallPiecesFromConstruct` |
| `removeWallPiece` | `war_walls_removeWallPiece` | `wwall_removeWallPiece` |
| `createWall` | `war_walls_createWall` | `wwall_createWall` |

### `src/war_ruins.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `hasRuinPieceAtPosition` | `war_ruins_hasRuinPieceAtPosition` | `wruin_hasRuinPieceAtPosition` |
| `getRuinPieceAtPosition` | `war_ruins_getRuinPieceAtPosition` | `wruin_getRuinPieceAtPosition` |
| `determineRuinTypes` | `war_ruins_determineRuinTypes` | `wruin_determineRuinTypes` |
| `createRuins` | `war_ruins_createRuins` | `wruin_createRuins` |
| `addRuinsPieces` | `war_ruins_addRuinsPieces` | `wruin_addRuinsPieces` |
| `removeRuinPiece` | `war_ruins_removeRuinPiece` | `wruin_removeRuinPiece` |

### `src/war_trees.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `hasTreeAtPosition` | `war_trees_hasTreeAtPosition` | `wtree_hasTreeAtPosition` |
| `getTreeAtPosition` | `war_trees_getTreeAtPosition` | `wtree_getTreeAtPosition` |
| `determineTreeTiles` | `war_trees_determineTreeTiles` | `wtree_determineTreeTiles` |
| `determineAllTreeTiles` | `war_trees_determineAllTreeTiles` | `wtree_determineAllTreeTiles` |
| `findAccesibleTree` | `war_trees_findAccesibleTree` | `wtree_findAccesibleTree` |
| `plantTree` | `war_trees_plantTree` | `wtree_plantTree` |
| `validTree` | `war_trees_validTree` | `wtree_validTree` |
| `takeTreeDown` | `war_trees_takeTreeDown` | `wtree_takeTreeDown` |
| `chopTree` | `war_trees_chopTree` | `wtree_chopTree` |
| `createForest` | `war_trees_createForest` | `wtree_createForest` |

## Step 6. Rename high-fanout core modules last
These files depend on most of the codebase, so they should be changed after the support layers and gameplay helpers are stable.

### `src/war_ui.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `isUIEntity` | `war_ui_isUIEntity` | `wui_isUIEntity` |
| `clearUIText` | `war_ui_clearUIText` | `wui_clearUIText` |
| `setUIText` | `war_ui_setUIText` | `wui_setUIText` |
| `setUITextFormatv` | `war_ui_setUITextFormatv` | `wui_setUITextFormatv` |
| `setUITextFormat` | `war_ui_setUITextFormat` | `wui_setUITextFormat` |
| `setUIImage` | `war_ui_setUIImage` | `wui_setUIImage` |
| `setUIRectWidth` | `war_ui_setUIRectWidth` | `wui_setUIRectWidth` |
| `clearUITooltip` | `war_ui_clearUITooltip` | `wui_clearUITooltip` |
| `setUITooltip` | `war_ui_setUITooltip` | `wui_setUITooltip` |
| `setUIButtonStatusByName` | `war_ui_setUIButtonStatusByName` | `wui_setUIButtonStatusByName` |
| `setUIButtonInteractiveByName` | `war_ui_setUIButtonInteractiveByName` | `wui_setUIButtonInteractiveByName` |
| `setUIButtonHotKeyByName` | `war_ui_setUIButtonHotKeyByName` | `wui_setUIButtonHotKeyByName` |
| `setUIEntityStatusByName` | `war_ui_setUIEntityStatusByName` | `wui_setUIEntityStatusByName` |
| `createUIText` | `war_ui_createUIText` | `wui_createUIText` |
| `createUIRect` | `war_ui_createUIRect` | `wui_createUIRect` |
| `createUIImage` | `war_ui_createUIImage` | `wui_createUIImage` |
| `createUICursor` | `war_ui_createUICursor` | `wui_createUICursor` |
| `createUITextButton` | `war_ui_createUITextButton` | `wui_createUITextButton` |
| `createUIImageButton` | `war_ui_createUIImageButton` | `wui_createUIImageButton` |
| `changeCursorType` | `war_ui_changeCursorType` | `wui_changeCursorType` |
| `updateUICursor` | `war_ui_updateUICursor` | `wui_updateUICursor` |
| `updateUIButtons` | `war_ui_updateUIButtons` | `wui_updateUIButtons` |
| `renderUIEntities` | `war_ui_renderUIEntities` | `wui_renderUIEntities` |

### `src/war_scenes.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `createScene` | `war_scene_createScene` | `wsc_createScene` |
| `freeScene` | `war_scene_freeScene` | `wsc_freeScene` |
| `enterScene` | `war_scene_enterScene` | `wsc_enterScene` |
| `updateScene` | `war_scene_updateScene` | `wsc_updateScene` |
| `leaveScene` | `war_scene_leaveScene` | `wsc_leaveScene` |
| `renderScene` | `war_scene_renderScene` | `wsc_renderScene` |

### `src/war_scene_download.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `enterSceneDownload` | `war_scene_download_enterSceneDownload` | `wsd_enterSceneDownload` |
| `updateSceneDownload` | `war_scene_download_updateSceneDownload` | `wsd_updateSceneDownload` |

### `src/war_scene_blizzard.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `enterSceneBlizzard` | `war_scene_blizzard_enterSceneBlizzard` | `wsbl_enterSceneBlizzard` |
| `updateSceneBlizzard` | `war_scene_blizzard_updateSceneBlizzard` | `wsbl_updateSceneBlizzard` |

### `src/war_scene_briefing.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `enterSceneBriefingHumans` | `war_scene_briefing_enterSceneBriefingHumans` | `wsbr_enterSceneBriefingHumans` |
| `enterSceneBriefingOrcs` | `war_scene_briefing_enterSceneBriefingOrcs` | `wsbr_enterSceneBriefingOrcs` |
| `enterSceneBriefing` | `war_scene_briefing_enterSceneBriefing` | `wsbr_enterSceneBriefing` |
| `updateSceneBriefing` | `war_scene_briefing_updateSceneBriefing` | `wsbr_updateSceneBriefing` |

### `src/war_scene_menu.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `setCustomGameRaceStr` | `war_scene_menu_setCustomGameRaceStr` | `wsm_setCustomGameRaceStr` |
| `setCustomMapStr` | `war_scene_menu_setCustomMapStr` | `wsm_setCustomMapStr` |
| `setUIRaceValueByName` | `war_scene_menu_setUIRaceValueByName` | `wsm_setUIRaceValueByName` |
| `setCustomMapValueByName` | `war_scene_menu_setCustomMapValueByName` | `wsm_setCustomMapValueByName` |
| `enterSceneMainMenu` | `war_scene_menu_enterSceneMainMenu` | `wsm_enterSceneMainMenu` |
| `createMainMenu` | `war_scene_menu_createMainMenu` | `wsm_createMainMenu` |
| `createSinglePlayerMenu` | `war_scene_menu_createSinglePlayerMenu` | `wsm_createSinglePlayerMenu` |
| `createLoadMenu` | `war_scene_menu_createLoadMenu` | `wsm_createLoadMenu` |
| `createCustomGameMenu` | `war_scene_menu_createCustomGameMenu` | `wsm_createCustomGameMenu` |
| `showOrHideMainMenu` | `war_scene_menu_showOrHideMainMenu` | `wsm_showOrHideMainMenu` |
| `showOrHideSinglePlayer` | `war_scene_menu_showOrHideSinglePlayer` | `wsm_showOrHideSinglePlayer` |
| `showOrHideCustomGame` | `war_scene_menu_showOrHideCustomGame` | `wsm_showOrHideCustomGame` |
| `handleMenuSinglePlayer` | `war_scene_menu_handleMenuSinglePlayer` | `wsm_handleMenuSinglePlayer` |
| `handleMenuQuit` | `war_scene_menu_handleMenuQuit` | `wsm_handleMenuQuit` |
| `handleSinglePlayerOrc` | `war_scene_menu_handleSinglePlayerOrc` | `wsm_handleSinglePlayerOrc` |
| `handleSinglePlayerHuman` | `war_scene_menu_handleSinglePlayerHuman` | `wsm_handleSinglePlayerHuman` |
| `handleCustomGame` | `war_scene_menu_handleCustomGame` | `wsm_handleCustomGame` |
| `handleSinglePlayerCancel` | `war_scene_menu_handleSinglePlayerCancel` | `wsm_handleSinglePlayerCancel` |
| `handleYourRaceLeft` | `war_scene_menu_handleYourRaceLeft` | `wsm_handleYourRaceLeft` |
| `handleYourRaceRight` | `war_scene_menu_handleYourRaceRight` | `wsm_handleYourRaceRight` |
| `handleEnemyRaceLeft` | `war_scene_menu_handleEnemyRaceLeft` | `wsm_handleEnemyRaceLeft` |
| `handleEnemyRaceRight` | `war_scene_menu_handleEnemyRaceRight` | `wsm_handleEnemyRaceRight` |
| `handleMapLeft` | `war_scene_menu_handleMapLeft` | `wsm_handleMapLeft` |
| `handleMapRight` | `war_scene_menu_handleMapRight` | `wsm_handleMapRight` |
| `handleCustomGameOk` | `war_scene_menu_handleCustomGameOk` | `wsm_handleCustomGameOk` |

### `src/war_state_machine.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `createState` | `war_state_createState` | `wst_createState` |
| `changeNextState` | `war_state_changeNextState` | `wst_changeNextState` |
| `changeStateNextState` | `war_state_changeStateNextState` | `wst_changeStateNextState` |
| `getState` | `war_state_getState` | `wst_getState` |
| `getDirectState` | `war_state_getDirectState` | `wst_getDirectState` |
| `getNextState` | `war_state_getNextState` | `wst_getNextState` |
| `hasState` | `war_state_hasState` | `wst_hasState` |
| `hasDirectState` | `war_state_hasDirectState` | `wst_hasDirectState` |
| `hasNextState` | `war_state_hasNextState` | `wst_hasNextState` |
| `isInsideBuilding` | `war_state_isInsideBuilding` | `wst_isInsideBuilding` |
| `enterState` | `war_state_enterState` | `wst_enterState` |
| `leaveState` | `war_state_leaveState` | `wst_leaveState` |
| `updateStateMachine` | `war_state_updateStateMachine` | `wst_updateStateMachine` |
| `freeState` | `war_state_freeState` | `wst_freeState` |

### `src/war_state_machine_*.c`
Each state file follows the same pattern: `createXState`, `enterXState`, `leaveXState`, `updateXState`, `freeXState`.

| File | Long new name pattern | Short new name pattern |
|---|---|---|
| `war_state_machine_idle.c` | `war_state_machine_idle_createIdleState`, `war_state_machine_idle_enterIdleState`, `war_state_machine_idle_leaveIdleState`, `war_state_machine_idle_updateIdleState`, `war_state_machine_idle_freeIdleState` | `wst_idle_createIdleState`, `wst_idle_enterIdleState`, `wst_idle_leaveIdleState`, `wst_idle_updateIdleState`, `wst_idle_freeIdleState` |
| `war_state_machine_move.c` | `war_state_machine_move_createMoveState`, `war_state_machine_move_enterMoveState`, `war_state_machine_move_leaveMoveState`, `war_state_machine_move_updateMoveState`, `war_state_machine_move_freeMoveState` | `wst_move_createMoveState`, `wst_move_enterMoveState`, `wst_move_leaveMoveState`, `wst_move_updateMoveState`, `wst_move_freeMoveState` |
| `war_state_machine_follow.c` | `war_state_machine_follow_createFollowState`, `war_state_machine_follow_enterFollowState`, `war_state_machine_follow_leaveFollowState`, `war_state_machine_follow_updateFollowState`, `war_state_machine_follow_freeFollowState` | `wst_follow_createFollowState`, `wst_follow_enterFollowState`, `wst_follow_leaveFollowState`, `wst_follow_updateFollowState`, `wst_follow_freeFollowState` |
| `war_state_machine_patrol.c` | `war_state_machine_patrol_createPatrolState`, `war_state_machine_patrol_enterPatrolState`, `war_state_machine_patrol_leavePatrolState`, `war_state_machine_patrol_updatePatrolState`, `war_state_machine_patrol_freePatrolState` | `wst_patrol_createPatrolState`, `wst_patrol_enterPatrolState`, `wst_patrol_leavePatrolState`, `wst_patrol_updatePatrolState`, `wst_patrol_freePatrolState` |
| `war_state_machine_attack.c` | `war_state_machine_attack_createAttackState`, `war_state_machine_attack_enterAttackState`, `war_state_machine_attack_leaveAttackState`, `war_state_machine_attack_updateAttackState`, `war_state_machine_attack_freeAttackState` | `wst_attack_createAttackState`, `wst_attack_enterAttackState`, `wst_attack_leaveAttackState`, `wst_attack_updateAttackState`, `wst_attack_freeAttackState` |
| `war_state_machine_death.c` | `war_state_machine_death_createDeathState`, `war_state_machine_death_enterDeathState`, `war_state_machine_death_leaveDeathState`, `war_state_machine_death_updateDeathState`, `war_state_machine_death_freeDeathState` | `wst_death_createDeathState`, `wst_death_enterDeathState`, `wst_death_leaveDeathState`, `wst_death_updateDeathState`, `wst_death_freeDeathState` |
| `war_state_machine_collapse.c` | `war_state_machine_collapse_createCollapseState`, `war_state_machine_collapse_enterCollapseState`, `war_state_machine_collapse_leaveCollapseState`, `war_state_machine_collapse_updateCollapseState`, `war_state_machine_collapse_freeCollapseState` | `wst_collapse_createCollapseState`, `wst_collapse_enterCollapseState`, `wst_collapse_leaveCollapseState`, `wst_collapse_updateCollapseState`, `wst_collapse_freeCollapseState` |
| `war_state_machine_wait.c` | `war_state_machine_wait_createWaitState`, `war_state_machine_wait_enterWaitState`, `war_state_machine_wait_leaveWaitState`, `war_state_machine_wait_updateWaitState`, `war_state_machine_wait_freeWaitState` | `wst_wait_createWaitState`, `wst_wait_enterWaitState`, `wst_wait_leaveWaitState`, `wst_wait_updateWaitState`, `wst_wait_freeWaitState` |
| `war_state_machine_gather_gold.c` | `war_state_machine_gather_gold_createGatherGoldState`, `war_state_machine_gather_gold_enterGatherGoldState`, `war_state_machine_gather_gold_leaveGatherGoldState`, `war_state_machine_gather_gold_updateGatherGoldState`, `war_state_machine_gather_gold_freeGatherGoldState` | `wst_gold_createGatherGoldState`, `wst_gold_enterGatherGoldState`, `wst_gold_leaveGatherGoldState`, `wst_gold_updateGatherGoldState`, `wst_gold_freeGatherGoldState` |
| `war_state_machine_mining.c` | `war_state_machine_mining_createMiningState`, `war_state_machine_mining_enterMiningState`, `war_state_machine_mining_leaveMiningState`, `war_state_machine_mining_updateMiningState`, `war_state_machine_mining_freeMiningState` | `wst_mining_createMiningState`, `wst_mining_enterMiningState`, `wst_mining_leaveMiningState`, `wst_mining_updateMiningState`, `wst_mining_freeMiningState` |
| `war_state_machine_gather_wood.c` | `war_state_machine_gather_wood_createGatherWoodState`, `war_state_machine_gather_wood_enterGatherWoodState`, `war_state_machine_gather_wood_leaveGatherWoodState`, `war_state_machine_gather_wood_updateGatherWoodState`, `war_state_machine_gather_wood_freeGatherWoodState` | `wst_wood_createGatherWoodState`, `wst_wood_enterGatherWoodState`, `wst_wood_leaveGatherWoodState`, `wst_wood_updateGatherWoodState`, `wst_wood_freeGatherWoodState` |
| `war_state_machine_chopping.c` | `war_state_machine_chopping_createChoppingState`, `war_state_machine_chopping_enterChoppingState`, `war_state_machine_chopping_leaveChoppingState`, `war_state_machine_chopping_updateChoppingState`, `war_state_machine_chopping_freeChoppingState` | `wst_chop_createChoppingState`, `wst_chop_enterChoppingState`, `wst_chop_leaveChoppingState`, `wst_chop_updateChoppingState`, `wst_chop_freeChoppingState` |
| `war_state_machine_deliver.c` | `war_state_machine_deliver_createDeliverState`, `war_state_machine_deliver_enterDeliverState`, `war_state_machine_deliver_leaveDeliverState`, `war_state_machine_deliver_updateDeliverState`, `war_state_machine_deliver_freeDeliverState` | `wst_deliver_createDeliverState`, `wst_deliver_enterDeliverState`, `wst_deliver_leaveDeliverState`, `wst_deliver_updateDeliverState`, `wst_deliver_freeDeliverState` |
| `war_state_machine_train.c` | `war_state_machine_train_createTrainState`, `war_state_machine_train_enterTrainState`, `war_state_machine_train_leaveTrainState`, `war_state_machine_train_updateTrainState`, `war_state_machine_train_freeTrainState` | `wst_train_createTrainState`, `wst_train_enterTrainState`, `wst_train_leaveTrainState`, `wst_train_updateTrainState`, `wst_train_freeTrainState` |
| `war_state_machine_upgrade.c` | `war_state_machine_upgrade_createUpgradeState`, `war_state_machine_upgrade_enterUpgradeState`, `war_state_machine_upgrade_leaveUpgradeState`, `war_state_machine_upgrade_updateUpgradeState`, `war_state_machine_upgrade_freeUpgradeState` | `wst_upgrade_createUpgradeState`, `wst_upgrade_enterUpgradeState`, `wst_upgrade_leaveUpgradeState`, `wst_upgrade_updateUpgradeState`, `wst_upgrade_freeUpgradeState` |
| `war_state_machine_build.c` | `war_state_machine_build_createBuildState`, `war_state_machine_build_enterBuildState`, `war_state_machine_build_leaveBuildState`, `war_state_machine_build_updateBuildState`, `war_state_machine_build_freeBuildState` | `wst_build_createBuildState`, `wst_build_enterBuildState`, `wst_build_leaveBuildState`, `wst_build_updateBuildState`, `wst_build_freeBuildState` |
| `war_state_machine_repair.c` | `war_state_machine_repair_createRepairState`, `war_state_machine_repair_enterRepairState`, `war_state_machine_repair_leaveRepairState`, `war_state_machine_repair_updateRepairState`, `war_state_machine_repair_freeRepairState` | `wst_repair_createRepairState`, `wst_repair_enterRepairState`, `wst_repair_leaveRepairState`, `wst_repair_updateRepairState`, `wst_repair_freeRepairState` |
| `war_state_machine_repairing.c` | `war_state_machine_repairing_createRepairingState`, `war_state_machine_repairing_enterRepairingState`, `war_state_machine_repairing_leaveRepairingState`, `war_state_machine_repairing_updateRepairingState`, `war_state_machine_repairing_freeRepairingState` | `wst_repairing_createRepairingState`, `wst_repairing_enterRepairingState`, `wst_repairing_leaveRepairingState`, `wst_repairing_updateRepairingState`, `wst_repairing_freeRepairingState` |
| `war_state_machine_cast.c` | `war_state_machine_cast_createCastState`, `war_state_machine_cast_enterCastState`, `war_state_machine_cast_leaveCastState`, `war_state_machine_cast_updateCastState`, `war_state_machine_cast_freeCastState` | `wst_cast_createCastState`, `wst_cast_enterCastState`, `wst_cast_leaveCastState`, `wst_cast_updateCastState`, `wst_cast_freeCastState` |

## Step 7. Rename the core runtime last
These modules have the widest reach and should be changed only after the lower layers are stable.

### `src/war_game.c`
| Old name | Long new name | Short new name |
|---|---|---|
| `initGame` | `war_game_initGame` | `wg_initGame` |
| `quitGame` | `war_game_quitGame` | `wg_quitGame` |
| `loadDataFile` | `war_game_loadDataFile` | `wg_loadDataFile` |
| `setWindowSize` | `war_game_setWindowSize` | `wg_setWindowSize` |
| `setGlobalScale` | `war_game_setGlobalScale` | `wg_setGlobalScale` |
| `changeGlobalScale` | `war_game_changeGlobalScale` | `wg_changeGlobalScale` |
| `setGlobalSpeed` | `war_game_setGlobalSpeed` | `wg_setGlobalSpeed` |
| `changeGlobalSpeed` | `war_game_changeGlobalSpeed` | `wg_changeGlobalSpeed` |
| `setMusicVolume` | `war_game_setMusicVolume` | `wg_setMusicVolume` |
| `changeMusicVolume` | `war_game_changeMusicVolume` | `wg_changeMusicVolume` |
| `setSoundVolume` | `war_game_setSoundVolume` | `wg_setSoundVolume` |
| `changeSoundVolume` | `war_game_changeSoundVolume` | `wg_changeSoundVolume` |
| `setNextScene` | `war_game_setNextScene` | `wg_setNextScene` |
| `setNextMap` | `war_game_setNextMap` | `wg_setNextMap` |
| `setInputButton` | `war_game_setInputButton` | `wg_setInputButton` |
| `setInputKey` | `war_game_setInputKey` | `wg_setInputKey` |
| `beginInputFrame` | `war_game_beginInputFrame` | `wg_beginInputFrame` |
| `getWarKeyFromSDLKey` | `war_game_getWarKeyFromSDLKey` | `wg_getWarKeyFromSDLKey` |
| `appendCheatTextInput` | `war_game_appendCheatTextInput` | `wg_appendCheatTextInput` |
| `processGameEvent` | `war_game_processGameEvent` | `wg_processGameEvent` |
| `updateGame` | `war_game_updateGame` | `wg_updateGame` |
| `renderGame` | `war_game_renderGame` | `wg_renderGame` |
| `presentGame` | `war_game_presentGame` | `wg_presentGame` |
| `getDirFromArrowKeys` | `war_game_getDirFromArrowKeys` | `wg_getDirFromArrowKeys` |
| `getDirFromMousePos` | `war_game_getDirFromMousePos` | `wg_getDirFromMousePos` |
| `printDirection` | `war_game_printDirection` | `wg_printDirection` |

## Step 8. Rename the module families in dependency order
1. Rename utility modules first: `war_types`, `war_math`, `war_log`.
2. Rename asset and runtime modules next: `war_file`, `war_resources`, `war_sprites`, `war_font`, `war_render`, `war_audio`, `war_animations`.
3. Rename map-structure and gameplay helpers: `war_campaigns`, `war_cheats`, `war_cheats_panel`, `war_net`, `war_ai`, `war_roads`, `war_walls`, `war_ruins`, `war_trees`, `war_pathfinder`.
4. Rename gameplay systems: `war_entities`, `war_units`, `war_commands`, `war_actions`, `war_projectiles`.
5. Rename UI and scene modules: `war_ui`, `war_scenes`, `war_scene_*`, `war_map_ui`, `war_map_menu`.
6. Rename state machine core and each state file.
7. Rename `war_map` and `war_game` last, then do a final global search to catch call sites, headers, and `war1.c` includes.

## Step 9. Refactor rules to keep the rename safe
- Rename one module at a time.
- Update declarations and definitions together.
- Fix call sites immediately after each module rename.
- Keep the short prefix as the final target, and use the long prefix only as a mechanical stepping stone if needed.
- After each phase, run a global search for the old prefix and clean up leftovers before moving on.
