#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "war_color.h"
#include "war_math.h"
#include "war_fwd.h"

#define FRAMES_PER_SECONDS 60
#define SECONDS_PER_FRAME (1.0f/FRAMES_PER_SECONDS)

#define MOVE_EPSILON 0.5f
#define MOVE_WAIT_INTENTS 2
#define MOVE_WAIT_TIME 1.0f

#define MAP_EDGE_SCROLL_GAP 5.0f

#define NEAR_ENEMY_RADIUS 5
#define NEAR_CATAPULT_RADIUS 2
#define NEAR_RAIN_OF_FIRE_RADIUS 1

#define RAIN_OF_FIRE_PROJECTILE_DAMAGE 10
#define POISON_CLOUD_DAMAGE 10

#define PLAYBACK_FREQ 44100

#define FOG_OF_WAR_UPDATE_TIME 1.0f

// #define DEBUG_RENDER_MAP_GRID
// #define DEBUG_RENDER_PASSABLE_INFO
// #define DEBUG_RENDER_UNIT_PATHS
// #define DEBUG_RENDER_UNIT_INFO
// #define DEBUG_RENDER_UNIT_STATS
// #define DEBUG_RENDER_UNIT_ANIMATIONS
// #define DEBUG_RENDER_MAP_ANIMATIONS
// #define DEBUG_RENDER_FONT
// #define DEBUG_RENDER_PROJECTILES

#define DATAWAR_FILE_PATH "./DATA.WAR"
#define ONLINE_DEMO_DATAWAR_FILE_URL "http://ia801608.us.archive.org/view_archive.php?archive=/11/items/WarcraftOrcsHumansDemo/WCRFT.ZIP&file=DEMODATA%2FDATA.WAR"

// Counts
#define MAX_RESOURCES_COUNT 583
#define MAX_TEXTURES_COUNT 583
#define MAX_ENTITIES_COUNT 100
#define MAX_SPRITE_FRAME_COUNT 100
#define MAX_CONSTRUCTS_COUNT 100
#define MAX_CUSTOM_MAP_GOLDMINES_COUNT 10
#define MAX_CUSTOM_MAP_CONFIGURATIONS_COUNT 10
#define MAX_CUSTOM_MAP_ENTITIES_COUNT 100

// all palettes have 768 colors
#define PALETTE_LENGTH 768

// size of mini-tiles from the tiles resources
#define MINI_TILE_WIDTH 8
#define MINI_TILE_HEIGHT 8

// size of the mega-tiles (2x2 mini-tiles) from the tileset
#define MEGA_TILE_WIDTH (MINI_TILE_WIDTH*2)
#define MEGA_TILE_HEIGHT (MINI_TILE_HEIGHT*2)

// size of the map in pixels
#define MAP_WIDTH (64*MEGA_TILE_WIDTH)
#define MAP_HEIGHT (64*MEGA_TILE_HEIGHT)

// size of the map in tiles
#define MAP_TILES_WIDTH (MAP_WIDTH/MEGA_TILE_WIDTH)
#define MAP_TILES_HEIGHT (MAP_HEIGHT/MEGA_TILE_HEIGHT)

// size of the viewport of the map in pixels
#define MAP_VIEWPORT_WIDTH 240
#define MAP_VIEWPORT_HEIGHT 176

// size of the minimap in pixels
#define MINIMAP_WIDTH 64
#define MINIMAP_HEIGHT 64

// ratio of the minimap and map sizes
#define MINIMAP_MAP_WIDTH_RATIO ((f32)MINIMAP_WIDTH/MAP_WIDTH)
#define MINIMAP_MAP_HEIGHT_RATIO ((f32)MINIMAP_HEIGHT/MAP_HEIGHT)

// size of the viewport of the minimap in pixels
#define MINIMAP_VIEWPORT_WIDTH (MAP_VIEWPORT_WIDTH*MINIMAP_MAP_WIDTH_RATIO)
#define MINIMAP_VIEWPORT_HEIGHT (MAP_VIEWPORT_HEIGHT*MINIMAP_MAP_HEIGHT_RATIO)

// size of the tileset for the terrain of the maps
#define TILESET_WIDTH 512
#define TILESET_HEIGHT 256
#define TILESET_TILES_PER_ROW (TILESET_WIDTH/MEGA_TILE_WIDTH)

#define MAX_OBJECTIVES_LENGTH 512
#define MAX_PLAYERS_COUNT 5
#define MAX_FEATURES_COUNT 22
#define MAX_UPGRADES_COUNT 10
#define MAX_TILES_COUNT 1024

// Game constants used across multiple modules
#define TREE_MAX_WOOD 100
#define GOLDMINE_MAX_GOLD 300
#define UNIT_MAX_CARRY_WOOD 100
#define UNIT_MAX_CARRY_GOLD 100

// Maximum number of samples the audio mix buffer can hold.
// Sized to cover any SDL3 audio device buffer at PLAYBACK_FREQ.
#define AUDIO_MIX_BUFFER_MAX_SAMPLES 8192u

// Maximum entity removals that the audio callback can queue per callback invocation.
#define AUDIO_REMOVE_PENDING_MAX 64

#define MAX_RENDER_STATE_STACK 32

#define isRetail(context) ((context)->warFile->type == WAR_FILE_TYPE_RETAIL)
#define isDemo(context) ((context)->warFile->type == WAR_FILE_TYPE_DEMO)

#define isButtonHeld(input, btn) ((input)->buttons[btn].held)
#define isButtonJustPressed(input, btn) ((input)->buttons[btn].justPressed)
#define isButtonJustReleased(input, btn) ((input)->buttons[btn].justReleased)

#define isKeyHeld(input, key) ((input)->keys[key].held)
#define isKeyJustPressed(input, key) ((input)->keys[key].justPressed)
#define isKeyJustReleased(input, key) ((input)->keys[key].justReleased)

#define isMapDragging(input) ((input)->mapDragActive)

#define getScaledSpeed(context, t) ((t) * (context)->globalSpeed)
#define getScaledTime(context, t) ((t) / (context)->globalSpeed)

struct _WarInputState
{
    bool held;
    bool justPressed;
    bool justReleased;
};

struct _WarInput
{
    // current mouse position
    vec2 pos;

    // state of the mouse buttons
    WarInputState buttons[WAR_MOUSE_COUNT];

    // state of the keys
    WarInputState keys[WAR_KEY_COUNT];

    // button currently owning the mouse press
    WarEntityId capturedUIButtonId;

    // map selection drag that started inside mapPanel
    bool mapDragActive;
    vec2 mapDragStartPos;
    rect mapDragRect;
};

struct _WarRenderState
{
    f32 offsetX, offsetY;
    f32 scaleX, scaleY;
    f32 alpha;
};

#include "war_sprites.h"

struct _WarContext
{
    f32 time;
    f32 deltaTime;
    u32 fps;

    bool paused;

    f32 globalScale;
    f32 globalSpeed;

    s32 originalWindowWidth;
    s32 originalWindowHeight;
    s32 windowWidth;
    s32 windowHeight;
    String windowTitle;
    SDL_Window* window;
    SDL_Renderer* renderer;

    // render state stack for save/restore/translate/scale/alpha
    WarRenderState renderState[MAX_RENDER_STATE_STACK];
    s32 renderStateTop;

    WarFile* warFile;
    WarResource* resources[MAX_RESOURCES_COUNT];
    WarSprite fontSprites[2];

    SDL_AudioStream* audioStream;
    tsf* soundFont;
    // this is shortcut to disable all audios in the map
    // to avoid crashes when freeing the map and the audio thread
    // trying to reproduce audios
    bool audioEnabled;
    bool musicEnabled;
    bool soundEnabled;
    f32 musicVolume;
    f32 soundVolume;

    // Pre-allocated mix buffer owned exclusively by the audio callback.
    // Allocated on the main thread before audio starts so that the audio
    // callback thread never calls wm_calloc / war_free, which would race
    // with main-thread allocations on permanentZone.
    s16* audioMixBuffer;
    u32  audioMixBufferCapacity; // capacity in samples

    // Pending audio-entity removals queued by the audio callback thread.
    // The callback posts finished entity IDs here (under audioRemoveMutex)
    // instead of calling we_removeEntityById directly.  The main thread drains
    // this queue at the top of each wg_updateGame tick.
    SDL_Mutex*  audioRemoveMutex;
    WarEntityId audioRemovePending[AUDIO_REMOVE_PENDING_MAX];
    s32         audioRemovePendingCount;

    bool cheatsEnabled;

    // this is a mutex used to make the deletion of the entities thread-safe
    // since the audio thread will delete audio entities, that could lead
    // to inconsistent states in the internal lists when the game try to also
    // delete other entities.
    SDL_Mutex* __mutex;

    WarInput input;

    f32 transitionDelay;
    WarScene* scene;
    WarScene* nextScene;
    WarMap* map;
    WarMap* nextMap;
};

void wg_setGlobalScale(WarContext* context, f32 scale);
void wg_changeGlobalScale(WarContext* context, f32 deltaScale);

void wg_setGlobalSpeed(WarContext* context, f32 speed);
void wg_changeGlobalSpeed(WarContext* context, f32 deltaSpeed);

void wg_setMusicVolume(WarContext* context, f32 volume);
void wg_changeMusicVolume(WarContext* context, f32 deltaVolume);

void wg_setSoundVolume(WarContext* context, f32 volume);
void wg_changeSoundVolume(WarContext* context, f32 deltaVolume);

void wg_setNextScene(WarContext* context, WarScene* scene, f32 transitionDelay);
void wg_setNextMap(WarContext* context, WarMap* map, f32 transitionDelay);

bool wg_loadDataFile(WarContext* context);

#include "war_map.h"
#include "war_scenes.h"
