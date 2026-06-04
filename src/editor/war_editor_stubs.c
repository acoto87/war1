// war_editor_stubs.c — No-op stubs for game symbols not used in the editor.
//
// This file is #included at the end of war1_editor.c (unity build).
// It provides empty implementations so the editor binary links without
// pulling in the full game modules (war_commands.c, war_audio.c,
// war_map.c, war_map_ui.c, war_ui.c, war_projectiles.c, TML).
//
// All stubs return safe zero/null defaults and suppress unused-parameter
// warnings via (void) casts.

// ---------------------------------------------------------------------------
// TML (TinySoundFont MIDI loader)
// tml_message is forward-declared in war_fwd.h
// ---------------------------------------------------------------------------

tml_message* tml_load_memory(const void* buffer, int size)
{
    (void)buffer;
    (void)size;
    return NULL;
}

void tml_free(tml_message* f)
{
    (void)f;
}

// ---------------------------------------------------------------------------
// war_audio stubs
// ---------------------------------------------------------------------------

WarEntity* wa_createAudio(WarContext* context, const CreateAudioArgs* args)
{
    (void)context;
    (void)args;
    return NULL;
}

WarEntity* wa_createAudioWithPosition(WarContext* context, const CreateAudioArgs* args)
{
    (void)context;
    (void)args;
    return NULL;
}

WarEntity* wa_createAudioRandom(WarContext* context, const CreateAudioArgs* args)
{
    (void)context;
    (void)args;
    return NULL;
}

WarEntity* wa_createAudioRandomWithPosition(WarContext* context, const CreateAudioArgs* args)
{
    (void)context;
    (void)args;
    return NULL;
}

WarEntity* wa_playAttackSound(WarContext* context, vec2 position, WarUnitActionStepType soundStep)
{
    (void)context;
    (void)position;
    (void)soundStep;
    return NULL;
}

// ---------------------------------------------------------------------------
// war_map stubs
// ---------------------------------------------------------------------------

void wmap_removeEntityFromSelection(WarContext* context, WarEntityId id)
{
    (void)context;
    (void)id;
}

vec2 wmap_screenToMapCoordinatesV(WarContext* context, vec2 v)
{
    (void)context;
    return v;
}

vec2 wmap_mapToTileCoordinatesV(vec2 v)
{
    return v;
}

vec2 wmap_tileToMapCoordinatesV(vec2 v, bool centeredInTile)
{
    (void)centeredInTile;
    return v;
}

s32 wmap_getMapTileIndex(WarContext* context, s32 x, s32 y)
{
    (void)context;
    (void)x;
    (void)y;
    return 0;
}

void wmap_setMapTileIndex(WarContext* context, s32 x, s32 y, s32 tile)
{
    (void)context;
    (void)x;
    (void)y;
    (void)tile;
}

bool wmap_isUnitPartiallyVisible(WarContext* context, WarMap* map, WarEntity* entity)
{
    (void)context;
    (void)map;
    (void)entity;
    return true;
}

bool wmap_isTileUnkown(WarMap* map, s32 x, s32 y)
{
    (void)map;
    (void)x;
    (void)y;
    return false;
}

bool wmap_isTileFog(WarMap* map, s32 x, s32 y)
{
    (void)map;
    (void)x;
    (void)y;
    return false;
}

bool wmap_isTileVisible(WarMap* map, s32 x, s32 y)
{
    (void)map;
    (void)x;
    (void)y;
    return true;
}

f32 wmap_getMapScaledTime(WarContext* context, f32 t)
{
    (void)context;
    return t;
}

// ---------------------------------------------------------------------------
// war_map_ui stubs
// ---------------------------------------------------------------------------

void wmui_setFlashStatus(WarContext* context, f32 duration, String text)
{
    (void)context;
    (void)duration;
    (void)text;
}

// ---------------------------------------------------------------------------
// war_projectiles stubs
// ---------------------------------------------------------------------------

WarEntity* wproj_createProjectile(
    WarContext* context, WarProjectileType type,
    WarEntityId sourceEntityId, WarEntityId targetEntityId,
    vec2 origin, vec2 target)
{
    (void)context;
    (void)type;
    (void)sourceEntityId;
    (void)targetEntityId;
    (void)origin;
    (void)target;
    return NULL;
}

// ---------------------------------------------------------------------------
// war_ui stubs
// ---------------------------------------------------------------------------

bool wui_isUIEntity(WarEntity* entity)
{
    (void)entity;
    return false;
}

void wui_clearUIText(WarContext* context, WarEntity* uiText)
{
    (void)context;
    (void)uiText;
}

void wui_setUIText(WarContext* context, WarEntity* uiText, String text)
{
    (void)context;
    (void)uiText;
    (void)text;
}

void wui_clearUITooltip(WarContext* context, WarEntity* uiButton)
{
    (void)context;
    (void)uiButton;
}

// ---------------------------------------------------------------------------
// war_commands stubs
// All functions share the same (WarContext*, WarEntity*) → void signature
// except wcmd_executeCommand which returns bool.
// ---------------------------------------------------------------------------

bool wcmd_executeCommand(WarContext* context)
{
    (void)context;
    return false;
}

void move(WarContext* context, WarEntity* entity)    { (void)context; (void)entity; }
void deliver(WarContext* context, WarEntity* entity) { (void)context; (void)entity; }
void repair(WarContext* context, WarEntity* entity)  { (void)context; (void)entity; }
void attack(WarContext* context, WarEntity* entity)  { (void)context; (void)entity; }

void wcmd_stop(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_cancel(WarContext* context, WarEntity* entity)        { (void)context; (void)entity; }
void wcmd_harvest(WarContext* context, WarEntity* entity)       { (void)context; (void)entity; }
void wcmd_buildBasic(WarContext* context, WarEntity* entity)    { (void)context; (void)entity; }
void wcmd_buildAdvanced(WarContext* context, WarEntity* entity) { (void)context; (void)entity; }

// train
void wcmd_trainFootman(WarContext* context, WarEntity* entity)       { (void)context; (void)entity; }
void wcmd_trainGrunt(WarContext* context, WarEntity* entity)         { (void)context; (void)entity; }
void wcmd_trainPeasant(WarContext* context, WarEntity* entity)       { (void)context; (void)entity; }
void wcmd_trainPeon(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_trainHumanCatapult(WarContext* context, WarEntity* entity) { (void)context; (void)entity; }
void wcmd_trainOrcCatapult(WarContext* context, WarEntity* entity)   { (void)context; (void)entity; }
void wcmd_trainKnight(WarContext* context, WarEntity* entity)        { (void)context; (void)entity; }
void wcmd_trainRaider(WarContext* context, WarEntity* entity)        { (void)context; (void)entity; }
void wcmd_trainArcher(WarContext* context, WarEntity* entity)        { (void)context; (void)entity; }
void wcmd_trainSpearman(WarContext* context, WarEntity* entity)      { (void)context; (void)entity; }
void wcmd_trainConjurer(WarContext* context, WarEntity* entity)      { (void)context; (void)entity; }
void wcmd_trainWarlock(WarContext* context, WarEntity* entity)       { (void)context; (void)entity; }
void wcmd_trainCleric(WarContext* context, WarEntity* entity)        { (void)context; (void)entity; }
void wcmd_trainNecrolyte(WarContext* context, WarEntity* entity)     { (void)context; (void)entity; }

// upgrades
void wcmd_upgradeSwords(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_upgradeAxes(WarContext* context, WarEntity* entity)            { (void)context; (void)entity; }
void wcmd_upgradeHumanShields(WarContext* context, WarEntity* entity)    { (void)context; (void)entity; }
void wcmd_upgradeOrcsShields(WarContext* context, WarEntity* entity)     { (void)context; (void)entity; }
void wcmd_upgradeArrows(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_upgradeSpears(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_upgradeHorses(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_upgradeWolves(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_upgradeScorpions(WarContext* context, WarEntity* entity)       { (void)context; (void)entity; }
void wcmd_upgradeSpiders(WarContext* context, WarEntity* entity)         { (void)context; (void)entity; }
void wcmd_upgradeRainOfFire(WarContext* context, WarEntity* entity)      { (void)context; (void)entity; }
void wcmd_upgradePoisonCloud(WarContext* context, WarEntity* entity)     { (void)context; (void)entity; }
void wcmd_upgradeWaterElemental(WarContext* context, WarEntity* entity)  { (void)context; (void)entity; }
void wcmd_upgradeDaemon(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_upgradeHealing(WarContext* context, WarEntity* entity)         { (void)context; (void)entity; }
void wcmd_upgradeRaiseDead(WarContext* context, WarEntity* entity)       { (void)context; (void)entity; }
void wcmd_upgradeFarSight(WarContext* context, WarEntity* entity)        { (void)context; (void)entity; }
void wcmd_upgradeDarkVision(WarContext* context, WarEntity* entity)      { (void)context; (void)entity; }
void wcmd_upgradeInvisibility(WarContext* context, WarEntity* entity)    { (void)context; (void)entity; }
void wcmd_upgradeUnholyArmor(WarContext* context, WarEntity* entity)     { (void)context; (void)entity; }

// build buildings
void wcmd_buildFarmHumans(WarContext* context, WarEntity* entity)       { (void)context; (void)entity; }
void wcmd_buildFarmOrcs(WarContext* context, WarEntity* entity)         { (void)context; (void)entity; }
void wcmd_buildBarracksHumans(WarContext* context, WarEntity* entity)   { (void)context; (void)entity; }
void wcmd_buildBarracksOrcs(WarContext* context, WarEntity* entity)     { (void)context; (void)entity; }
void wcmd_buildChurch(WarContext* context, WarEntity* entity)           { (void)context; (void)entity; }
void wcmd_buildTemple(WarContext* context, WarEntity* entity)           { (void)context; (void)entity; }
void wcmd_buildTowerHumans(WarContext* context, WarEntity* entity)      { (void)context; (void)entity; }
void wcmd_buildTowerOrcs(WarContext* context, WarEntity* entity)        { (void)context; (void)entity; }
void wcmd_buildTownHallHumans(WarContext* context, WarEntity* entity)   { (void)context; (void)entity; }
void wcmd_buildTownHallOrcs(WarContext* context, WarEntity* entity)     { (void)context; (void)entity; }
void wcmd_buildLumbermillHumans(WarContext* context, WarEntity* entity) { (void)context; (void)entity; }
void wcmd_buildLumbermillOrcs(WarContext* context, WarEntity* entity)   { (void)context; (void)entity; }
void wcmd_buildStable(WarContext* context, WarEntity* entity)           { (void)context; (void)entity; }
void wcmd_buildKennel(WarContext* context, WarEntity* entity)           { (void)context; (void)entity; }
void wcmd_buildBlacksmithHumans(WarContext* context, WarEntity* entity) { (void)context; (void)entity; }
void wcmd_buildBlacksmithOrcs(WarContext* context, WarEntity* entity)   { (void)context; (void)entity; }
void wcmd_buildWall(WarContext* context, WarEntity* entity)             { (void)context; (void)entity; }
void wcmd_buildRoad(WarContext* context, WarEntity* entity)             { (void)context; (void)entity; }

// spells
void wcmd_castRainOfFire(WarContext* context, WarEntity* entity)   { (void)context; (void)entity; }
void wcmd_castPoisonCloud(WarContext* context, WarEntity* entity)   { (void)context; (void)entity; }
void wcmd_castHeal(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_castFarSight(WarContext* context, WarEntity* entity)      { (void)context; (void)entity; }
void wcmd_castDarkVision(WarContext* context, WarEntity* entity)    { (void)context; (void)entity; }
void wcmd_castInvisibility(WarContext* context, WarEntity* entity)  { (void)context; (void)entity; }
void wcmd_castUnHolyArmor(WarContext* context, WarEntity* entity)   { (void)context; (void)entity; }
void wcmd_castRaiseDead(WarContext* context, WarEntity* entity)     { (void)context; (void)entity; }

// summons
void wcmd_summonSpider(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_summonScorpion(WarContext* context, WarEntity* entity)        { (void)context; (void)entity; }
void wcmd_summonDaemon(WarContext* context, WarEntity* entity)          { (void)context; (void)entity; }
void wcmd_summonWaterElemental(WarContext* context, WarEntity* entity)  { (void)context; (void)entity; }
