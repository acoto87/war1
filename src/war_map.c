#include <assert.h>
#include <float.h>

#include "TracyC.h"
#include "shl/memzone.h"
#include "shl/wstr.h"

#include "war_map.h"
#include "war_actions.h"
#include "war_animations.h"
#include "war_audio.h"
#include "war_campaigns.h"
#include "war_entities.h"
#include "war_font.h"
#include "war_map_menu.h"
#include "war_map_ui.h"
#include "war_projectiles.h"
#include "war_render.h"
#include "war_resources.h"
#include "war_sprites.h"
#include "war_state_machine.h"
#include "war_units.h"
#include "war_pathfinder.h"
#include "war_rvo.h"

#define MAP_SELECTION_DRAG_THRESHOLD 3.0f

// Sets the near-units debug overlay to show `we_getNearUnits` results at (targetTile, distance).
// The render path replays the query each frame, so the display stays current as units move.
static void setNearUnitsDebugParams(WarContext* context, vec2 targetTile, s32 distance)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_NEAR_UNITS])
        return;

    WarMap* map = context->map;
    assert(map);

    map->debug.nearUnitsEnabled  = true;
    map->debug.nearUnitsTargetTile = targetTile;
    map->debug.nearUnitsDistance   = distance;
}

static void clearNearUnitsDebug(WarContext* context)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_NEAR_UNITS])
        return;

    WarMap* map = context->map;
    assert(map);

    map->debug.nearUnitsEnabled = false;
}

static bool isSpellCheatEditModeActive(WarContext* context)
{
    WarMap* map = context->map;
    WarMapEditMode mode = map->editing.mode;
    return mode == WAR_MAP_EDIT_MODE_RAIN_OF_FIRE  ||
           mode == WAR_MAP_EDIT_MODE_POISON_CLOUD  ||
           mode == WAR_MAP_EDIT_MODE_RAISE_DEAD;
}

// Called at the end of each game update. When exactly one unit is selected and
// no spell-cheat edit mode is active, updates the overlay to follow that unit.
static void refreshSelectedUnitNearUnitsDebug(WarContext* context)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_NEAR_UNITS])
        return;

    WarMap* map = context->map;
    assert(map);

    // Let cheat edit modes hold their own overlay parameters.
    if (isSpellCheatEditModeActive(context))
        return;

    if (map->selectedEntities.count != 1)
    {
        clearNearUnitsDebug(context);
        return;
    }

    WarEntity* entity = we_findEntity(context, map->selectedEntities.items[0]);
    if (!entity || !wu_isUnit(entity))
    {
        clearNearUnitsDebug(context);
        return;
    }

    setNearUnitsDebugParams(context, wu_getUnitCenterPosition(context, entity, true), NEAR_ENEMY_RADIUS);
}

static void castDebugRainOfFire(WarContext* context, vec2 targetTile)
{
    setNearUnitsDebugParams(context, targetTile, NEAR_RAIN_OF_FIRE_RADIUS);

    vec2 targetTilePosition = wmap_tileToMapCoordinatesV(targetTile, true);
    s32 radius = 2 * MEGA_TILE_WIDTH;

    s32 projectilesCount = 5;
    while (projectilesCount--)
    {
        f32 offsetx = randomf(-radius, radius);
        f32 offsety = randomf(-radius, radius);
        vec2 target = vec2_addv(targetTilePosition, vec2f(offsetx, offsety));

        offsety = randomf(MEGA_TILE_WIDTH, MEGA_TILE_WIDTH * 4);
        vec2 origin = vec2f(target.x, context->map->camera.viewport.y - offsety);

        wproj_createProjectile(context, WAR_PROJECTILE_RAIN_OF_FIRE, 0, 0, origin, target);
    }
}

static void castDebugPoisonCloud(WarContext* context, vec2 targetTile)
{
    const WarSpellStats* stats = wu_getSpellStats(WAR_SPELL_POISON_CLOUD);
    assert(stats);

    vec2 targetPosition = wmap_tileToMapCoordinatesV(targetTile, true);

    WarEntity* poisonCloud = we_createEntity(context, WAR_ENTITY_TYPE_POISON_CLOUD, true);
    we_addPoisonCloudComponent(context, poisonCloud, WAR_POISON_CLOUD_COMPONENT_INIT(
        .position = targetTile,
        .time     = stats->time,
    ));
    we_addAnimationsComponent(context, poisonCloud);

    wanim_createPoisonCloudAnimation(context, poisonCloud, targetPosition);
    wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_NORMAL_SPELL, .position=targetPosition, .hasPosition=true, .loop=false));

    // Show the cloud's damage radius. The render path re-runs the query each
    // frame so the unit highlights stay current as units move in/out.
    setNearUnitsDebugParams(context, targetTile, 2);
}

static void castDebugRaiseDead(WarContext* context, vec2 targetTile)
{
    setNearUnitsDebugParams(context, targetTile, 4);

    WarEntityList nearUnits;
    WarEntityListInit(&nearUnits, wm_frameAllocator());
    we_getNearUnits(context, targetTile, 4, &nearUnits);

    for (s32 i = 0; i < nearUnits.count; i++)
    {
        WarEntity* targetEntity = nearUnits.items[i];
        if (targetEntity && wu_isCorpseUnit(context, targetEntity))
        {
            vec2 targetPosition = wu_getUnitCenterPosition(context, targetEntity, true);
            WarEntity* skeleton = we_createUnit(context, CREATE_UNIT_ARGS_INIT(
                .type=WAR_UNIT_SKELETON,
                .x=(s32)targetPosition.x,
                .y=(s32)targetPosition.y,
                .player=0,
                .resourceKind=WAR_RESOURCE_NONE,
                .amount=0,
                .addToMap=true
            ));
            we_setInitialIdleState(context, skeleton);

            targetPosition = wu_getUnitCenterPosition(context, targetEntity, false);

            WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
            we_addAnimationsComponent(context, animEntity);

            wanim_createSpellAnimation(context, animEntity, targetPosition);
            wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_NORMAL_SPELL, .position=targetPosition, .hasPosition=true, .loop=false));

            we_removeEntityById(context, targetEntity->id);
        }
    }

    WarEntityListFree(&nearUnits);
}

static void initCamera(WarMap *map, WarResource *levelInfo)
{
    s32 startX = levelInfo->levelInfo.startX * MEGA_TILE_WIDTH;
    s32 startY = levelInfo->levelInfo.startY * MEGA_TILE_HEIGHT;
    map->camera.viewport = recti(startX, startY, MAP_VIEWPORT_WIDTH, MAP_VIEWPORT_HEIGHT);
    map->camera.isScrolling = false;
    map->camera.wasScrolling = false;
}

static void initPathFinder(WarMap* map, WarResource* levelPassable)
{
    map->finder = wpath_initPathFinder(PATH_FINDING_ASTAR, levelPassable->levelPassable.data);
}

static void initBlackSprite(WarContext* context, WarMap* map)
{
    u8 data[MEGA_TILE_WIDTH * MEGA_TILE_HEIGHT * 4];
    memset(data, 0, MEGA_TILE_WIDTH * MEGA_TILE_HEIGHT * 4);
    for (s32 i = 0; i < MEGA_TILE_WIDTH * MEGA_TILE_HEIGHT; i++)
        data[4 * i + 3] = 255;

    map->blackSprite = wspr_createSprite(context, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT, data);
}

static void setInitialTileState(WarMap* map)
{
    for (s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        WarMapTile* tile = &map->tiles[i];

        tile->state = MAP_TILE_STATE_UNKOWN;
        tile->type = WAR_FOG_PIECE_NONE;
        tile->boundary = WAR_FOG_BOUNDARY_NONE;
    }
}

static void createMapAndMinimapSprites(WarContext* context, WarMap* map, WarResource* levelInfo)
{
    WarResource* levelVisual = wres_getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
    assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

    WarResource* tileset = wres_getOrCreateResource(context, levelInfo->levelInfo.tilesetIndex);
    assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

    // DEBUG:
    // print level visual data to console to see the sprites of the map
    //
    // for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    // {
    //     for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
    //     {
    //         // index of the tile in the tilesheet
    //         u16 tileIndex = levelVisual->levelVisual.data[y * MAP_TILES_WIDTH + x];
    //         printf("%d ", tileIndex);
    //     }

    //     printf("\n");
    // }

    map->sprite = wspr_createSprite(context, TILESET_WIDTH, TILESET_HEIGHT, tileset->tilesetData.data);

    // the minimap sprite will be a 2 frames sprite
    // the first one will be the frame that actually render
    // the second one will be the minimap for the terrain, created at startup time,
    // that way I only have to memcpy to the first frame and do the work only for the units
    // that way I also don't have to allocate memory for the minimap each frame
    WarSpriteFrame minimapFrames[2];

    for(s32 i = 0; i < 2; i++)
    {
        minimapFrames[i].dx = 0;
        minimapFrames[i].dy = 0;
        minimapFrames[i].w = MINIMAP_WIDTH;
        minimapFrames[i].h = MINIMAP_HEIGHT;
        minimapFrames[i].off = 0;
        minimapFrames[i].data = (u8*)wm_alloc(MINIMAP_WIDTH * MINIMAP_HEIGHT * 4 * sizeof(u8));

        // make the frame black
        for (s32 k = 0; k < MINIMAP_WIDTH * MINIMAP_HEIGHT; k++)
            minimapFrames[i].data[k * 4 + 3] = 255;
    }

    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            WarColor color = wmap_getMapTileAverage(levelVisual, tileset, x, y);
            s32 index = y * MAP_TILES_WIDTH + x;
            minimapFrames[1].data[index * 4 + 0] = color.r;
            minimapFrames[1].data[index * 4 + 1] = color.g;
            minimapFrames[1].data[index * 4 + 2] = color.b;
            minimapFrames[1].data[index * 4 + 3] = color.a;
        }
    }

    map->minimapSprite = wspr_createSpriteFromFrames(context, MINIMAP_WIDTH, MINIMAP_HEIGHT, arrayLength(minimapFrames), minimapFrames);
}

static void createForestEntities(WarContext* context, WarMap* map, WarResource* levelPassable)
{
    const s32 dirC = 8;
    const s32 dirX[] = {  0,  1, 1, 1, 0, -1, -1, -1 };
    const s32 dirY[] = { -1, -1, 0, 1, 1,  1,  0, -1 };

    bool processed[MAP_TILES_WIDTH * MAP_TILES_HEIGHT];
    for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
        processed[i] = false;

    u16* passableData = levelPassable->levelPassable.data;
    for(s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        if (!processed[i] && passableData[i] == 128)
        {
            s32 x = i % MAP_TILES_WIDTH;
            s32 y = i / MAP_TILES_WIDTH;

            WarTreeList trees;
            WarTreeListInit(&trees, wm_globalAllocator());
            WarTreeListAdd(&trees, createTree(x, y, TREE_MAX_WOOD));
            processed[i] = true;

            for(s32 j = 0; j < trees.count; j++)
            {
                WarTree tree = trees.items[j];
                for(s32 d = 0; d < dirC; d++)
                {
                    s32 xx = tree.tilex + dirX[d];
                    s32 yy = tree.tiley + dirY[d];
                    if (wpath_isInside(xx, yy))
                    {
                        s32 k = yy * MAP_TILES_WIDTH + xx;
                        if (!processed[k] && passableData[k] == 128)
                        {
                            // mark it processed right away, to not process it later
                            processed[k] = true;

                            WarTree newTree = createTree(xx, yy, TREE_MAX_WOOD);
                            WarTreeListAdd(&trees, newTree);
                        }
                    }
                }
            }

            WarEntity* forest = we_createEntity(context, WAR_ENTITY_TYPE_FOREST, true);
            we_addSpriteComponent(context, forest, WAR_SPRITE_COMPONENT_INIT(
                .sprite = map->sprite
            ));
            we_addForestComponent(context, forest, trees);

            for (s32 treeIndex = 0; treeIndex < trees.count; treeIndex++)
            {
                WarTree* tree = &trees.items[treeIndex];
                setStaticEntity(&map->finder, tree->tilex, tree->tiley, 1, 1, forest->id);
            }

            we_determineTreeTiles(context, forest);
        }
    }

    map->editing.forest = we_createForest(context);
}

static void createStartingRoads(WarContext* context, WarMap* map, WarResource* levelInfo)
{
    WarEntity* road = we_createRoad(context);

    for(s32 i = 0; i < (s32)levelInfo->levelInfo.startRoadsCount; i++)
    {
        WarLevelConstruct *construct = &levelInfo->levelInfo.startRoads[i];
        if (construct->type == WAR_CONSTRUCT_ROAD)
        {
            we_addRoadPiecesFromConstruct(context, road, construct);
        }
    }

    we_determineRoadTypes(context, road);

    map->editing.road = road;
}

static void createStartingWalls(WarContext* context, WarMap* map, WarResource* levelInfo)
{
    WarEntity* wall = we_createWall(context);

    for(s32 i = 0; i < (s32)levelInfo->levelInfo.startWallsCount; i++)
    {
        WarLevelConstruct *construct = &levelInfo->levelInfo.startWalls[i];
        if (construct->type == WAR_CONSTRUCT_WALL)
        {
            we_addWallPiecesFromConstruct(context, wall, construct);
        }
    }

    we_determineWallTypes(context, wall);

    WarWallComponent* wallComp = we_getWallComponent(context, wall);
    assert(wallComp);

    for(s32 i = 0; i < wallComp->pieces.count; i++)
    {
        WarWallPiece* piece = &wallComp->pieces.items[i];
        piece->hp = WAR_WALL_MAX_HP;
        piece->maxhp = WAR_WALL_MAX_HP;
    }

    we_addStateMachineComponent(context, wall);

    WarStateIdle* idleState = wst_createIdleState(context, wall, false);
    wst_changeNextState(context, wall, (WarStateBase*)idleState, true);

    map->editing.wall = wall;
}

static void createRuinEntity(WarContext* context, WarMap* map)
{
    map->editing.ruin = we_createRuins(context);
}

static void initPlayersInfo(WarMap* map, WarResource* levelInfo)
{
    for (s32 i = 0; i < MAX_PLAYERS_COUNT; i++)
    {
        WarPlayerInfo* player = &map->players[i];

        player->index = (u8)i;
        player->race = levelInfo->levelInfo.races[i];
        player->gold = 4000; // levelInfo->levelInfo.gold[i];
        player->wood = 4000; // levelInfo->levelInfo.lumber[i];
        player->godMode = false;

        for (s32 j = 0; j < MAX_FEATURES_COUNT; j++)
        {
            player->features[j] = levelInfo->levelInfo.allowedFeatures[j];
        }

        for (s32 j = 0; j < MAX_UPGRADES_COUNT; j++)
        {
            player->upgrades[j].allowed = levelInfo->levelInfo.allowedUpgrades[j][i];
            player->upgrades[j].level = 0;
        }
    }
}

static void createStartingEntities(WarContext* context, WarResource* levelInfo)
{
    for (s32 i = 0; i < (s32)levelInfo->levelInfo.startEntitiesCount; i++)
    {
        WarLevelUnit startUnit = levelInfo->levelInfo.startEntities[i];
        WarEntity* startEntity = we_createUnit(context, CREATE_UNIT_ARGS_INIT(
            .type=startUnit.type,
            .x=startUnit.x,
            .y=startUnit.y,
            .player=startUnit.player,
            .resourceKind=startUnit.resourceKind,
            .amount=startUnit.amount,
            .addToMap=true
        ));
        we_setInitialIdleState(context, startEntity);
    }
}

void wmap_addEntityToSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;

    if (map->selectedEntities.count >= MAX_SELECTED_ENTITIES_COUNT)
        return;

    // subtitute this with a set data structure that doesn't allow duplicates
    if (!WarEntityIdListContains(&map->selectedEntities, id, we_equalsEntityId))
        WarEntityIdListAdd(&map->selectedEntities, id);
}

void wmap_removeEntityFromSelection(WarContext* context, WarEntityId id)
{
    WarMap* map = context->map;
    WarEntityIdListRemove(&map->selectedEntities, id, we_equalsEntityId);
}

void wmap_clearSelection(WarContext* context)
{
    WarMap* map = context->map;
    WarEntityIdListClear(&map->selectedEntities);
}

vec2 wmap_getDirFromArrowKeys(WarContext* context)
{
    WarInput* input = &context->input;

    vec2 dir = VEC2_ZERO;

    if (isKeyHeld(input, WAR_KEY_LEFT))
        dir.x = -1;
    else if (isKeyHeld(input, WAR_KEY_RIGHT))
        dir.x = 1;

    if (isKeyHeld(input, WAR_KEY_DOWN))
        dir.y = 1;
    else if (isKeyHeld(input, WAR_KEY_UP))
        dir.y = -1;

    dir = vec2_normalize(dir);
    return dir;
}

vec2 wmap_getDirFromMousePos(WarContext* context)
{
    WarInput* input = &context->input;

    vec2 dir = VEC2_ZERO;

    if (input->pos.x < MAP_EDGE_SCROLL_GAP)
        dir.x = -1;
    else if (input->pos.x > context->originalWindowWidth - MAP_EDGE_SCROLL_GAP)
        dir.x = 1;

    if (input->pos.y < MAP_EDGE_SCROLL_GAP)
        dir.y = -1;
    else if (input->pos.y > context->originalWindowHeight - MAP_EDGE_SCROLL_GAP)
        dir.y = 1;

    dir = vec2_normalize(dir);
    return dir;
}

vec2 wmap_screenToMapCoordinatesV(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    rect mapPanel = map->ui.mapPanel;
    rect viewport = map->camera.viewport;

    v = vec2_translatef(v, -mapPanel.x, -mapPanel.y);
    v = vec2_translatef(v, viewport.x, viewport.y);
    return v;
}

vec2 wmap_screenToMinimapCoordinatesV(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    rect minimapPanel = map->ui.minimapPanel;

    v = vec2_translatef(v, -minimapPanel.x, -minimapPanel.y);
    return v;
}

rect wmap_screenToMapCoordinatesR(WarContext* context, rect r)
{
    WarMap* map = context->map;

    rect mapPanel = map->ui.mapPanel;
    rect viewport = map->camera.viewport;

    r = rect_translatef(r, -mapPanel.x, -mapPanel.y);
    r = rect_translatef(r, viewport.x, viewport.y);
    return r;
}

vec2 wmap_mapToScreenCoordinatesV(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    v = vec2_translatef(v, -map->camera.viewport.x, -map->camera.viewport.y);
    v = vec2_translatef(v, map->ui.mapPanel.x, map->ui.mapPanel.y);
    return v;
}

rect wmap_mapToScreenCoordinatesR(WarContext* context, rect r)
{
    WarMap* map = context->map;

    r = rect_translatef(r, -map->camera.viewport.x, -map->camera.viewport.y);
    r = rect_translatef(r, map->ui.mapPanel.x, map->ui.mapPanel.y);
    return r;
}

vec2 wmap_mapToTileCoordinatesV(vec2 v)
{
    v.x = floorf(v.x / MEGA_TILE_WIDTH);
    v.y = floorf(v.y / MEGA_TILE_HEIGHT);
    return v;
}

vec2 wmap_tileToMapCoordinatesV(vec2 v, bool centeredInTile)
{
    v.x *= MEGA_TILE_WIDTH;
    v.y *= MEGA_TILE_HEIGHT;

    if (centeredInTile)
    {
        v.x += MEGA_TILE_WIDTH/2;
        v.y += MEGA_TILE_HEIGHT/2;
    }

    return v;
}

vec2 wmap_minimapToViewportCoordinatesV(WarContext* context, vec2 v)
{
    WarMap* map = context->map;

    rect minimapPanel = map->ui.minimapPanel;
    vec2 minimapPanelSize = vec2f(minimapPanel.width, minimapPanel.height);

    vec2 minimapViewportSize = vec2f(MINIMAP_VIEWPORT_WIDTH, MINIMAP_VIEWPORT_HEIGHT);

    v = vec2_translatef(v, -minimapViewportSize.x / 2, -minimapViewportSize.y / 2);
    v = vec2_clampv(v, VEC2_ZERO, vec2_subv(minimapPanelSize, minimapViewportSize));
    return v;
}

WarMapTile* wmap_getMapTileState(WarMap* map, s32 x, s32 y)
{
    assert(inRange(x, 0, MAP_TILES_WIDTH) && inRange(y, 0, MAP_TILES_HEIGHT));
    return &map->tiles[y * MAP_TILES_WIDTH + x];
}

void wmap_setMapTileState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState tileState)
{
    if (startX <= 0)
        startX = 0;

    if (startY <= 0)
        startY = 0;

    if (startX + width >= MAP_TILES_WIDTH)
        width = MAP_TILES_WIDTH - startX;

    if (startY + height >= MAP_TILES_HEIGHT)
        height = MAP_TILES_HEIGHT - startY;

    s32 endX = startX + width;
    s32 endY = startY + height;

    for(s32 y = startY; y < endY; y++)
    {
        for(s32 x = startX; x < endX; x++)
        {
            // exclude the corners of the area to get a more "rounded" shape
            if ((y == startY || y == endY - 1) && (x == startX || x == endX - 1))
                continue;

            WarMapTile* tile = wmap_getMapTileState(map, x, y);
            tile->state = tileState;
        }
    }
}

void wmap_setUnitMapTileState(WarContext* context, WarMap* map, WarEntity* entity, WarMapTileState tileState)
{
    assert(wu_isUnit(entity));

    s32 sight = wu_getUnitSightRange(context, entity);
    vec2 position = wu_getUnitPosition(context, entity, true);
    vec2 unitSize = wu_getUnitSize(context, entity);
    rect unitRect = rectv(position, unitSize);
    unitRect = rect_expand(unitRect, (f32)sight, (f32)sight);

    wmap_setMapTileState(map, (s32)unitRect.x, (s32)unitRect.y, (s32)unitRect.width, (s32)unitRect.height, tileState);
}

bool wmap_isTileInState(WarMap* map, s32 x, s32 y, WarMapTileState state)
{
    if (!map->fowEnabled)
    {
        switch (state)
        {
            case MAP_TILE_STATE_UNKOWN: return false;
            case MAP_TILE_STATE_FOG: return false;
            case MAP_TILE_STATE_VISIBLE: return true;
            default:
            {
                logError("Unkown state: %d. Defaulting to true.", state);
                return true;
            }
        }
    }

    WarMapTile* tile = wmap_getMapTileState(map, x, y);
    return tile->state == state;
}

bool wmap_isAnyTileInStates(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state)
{
    if (!map->fowEnabled)
    {
        switch (state)
        {
            case MAP_TILE_STATE_UNKOWN: return false;
            case MAP_TILE_STATE_FOG: return false;
            case MAP_TILE_STATE_VISIBLE: return true;
            default:
            {
                logError("Unkown state: %d. Defaulting to true.", state);
                return true;
            }
        }
    }

    if (startX <= 0)
        startX = 0;

    if (startY <= 0)
        startY = 0;

    if (startX + width >= MAP_TILES_WIDTH)
        width = MAP_TILES_WIDTH - startX;

    if (startY + height >= MAP_TILES_HEIGHT)
        height = MAP_TILES_HEIGHT - startY;

    s32 endX = startX + width;
    s32 endY = startY + height;

    for(s32 y = startY; y < endY; y++)
    {
        for(s32 x = startX; x < endX; x++)
        {
            WarMapTile* tile = wmap_getMapTileState(map, x, y);
            if (tile->state == state)
            {
                return true;
            }
        }
    }

    return false;
}

bool wmap_isAnyUnitTileInStates(WarContext* context, WarMap* map, WarEntity* entity, WarMapTileState state)
{
    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    vec2 position = wu_getUnitPosition(context, entity, true);
    return wmap_isAnyTileInStates(map, (s32)position.x, (s32)position.y, unit->sizex, unit->sizey, state);
}

bool wmap_areAllTilesInState(WarMap* map, s32 startX, s32 startY, s32 width, s32 height, WarMapTileState state)
{
    if (!map->fowEnabled)
    {
        switch (state)
        {
            case MAP_TILE_STATE_UNKOWN: return false;
            case MAP_TILE_STATE_FOG: return false;
            case MAP_TILE_STATE_VISIBLE: return true;
            default:
            {
                logError("Unkown state: %d. Defaulting to true.", state);
                return true;
            }
        }
    }

    if (startX <= 0)
        startX = 0;

    if (startY <= 0)
        startY = 0;

    if (startX + width >= MAP_TILES_WIDTH)
        width = MAP_TILES_WIDTH - startX;

    if (startY + height >= MAP_TILES_HEIGHT)
        height = MAP_TILES_HEIGHT - startY;

    s32 endX = startX + width;
    s32 endY = startY + height;

    for(s32 y = startY; y < endY; y++)
    {
        for(s32 x = startX; x < endX; x++)
        {
            WarMapTile* tile = wmap_getMapTileState(map, x, y);
            if (tile->state != state)
            {
                return false;
            }
        }
    }

    return true;
}

bool wmap_areAllUnitTilesInState(WarContext* context, WarMap* map, WarEntity* entity, WarMapTileState state)
{
    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    vec2 position = wu_getUnitPosition(context, entity, true);
    return wmap_areAllTilesInState(map, (s32)position.x, (s32)position.y, unit->sizex, unit->sizey, state);
}

bool wmap_isUnitPartiallyVisible(WarContext* context, WarMap* map, WarEntity* entity)
{
    return wmap_isAnyUnitTileInStates(context, map, entity, MAP_TILE_STATE_VISIBLE);
}

bool wmap_isUnitVisible(WarContext* context, WarMap* map, WarEntity* entity)
{
    return wmap_areAllUnitTilesInState(context, map, entity, MAP_TILE_STATE_VISIBLE);
}

bool wmap_isUnitPartiallyFog(WarContext* context, WarMap* map, WarEntity* entity)
{
    return wmap_isAnyUnitTileInStates(context, map, entity, MAP_TILE_STATE_FOG);
}

bool wmap_isUnitFog(WarContext* context, WarMap* map, WarEntity* entity)
{
    return wmap_areAllUnitTilesInState(context, map, entity, MAP_TILE_STATE_FOG);
}

bool wmap_isUnitPartiallyUnkown(WarContext* context, WarMap* map, WarEntity* entity)
{
    return wmap_isAnyUnitTileInStates(context, map, entity, MAP_TILE_STATE_UNKOWN);
}

bool wmap_isUnitUnknown(WarContext* context, WarMap* map, WarEntity* entity)
{
    return wmap_areAllUnitTilesInState(context, map, entity, MAP_TILE_STATE_UNKOWN);
}

bool wmap_isTileUnkown(WarMap* map, s32 x, s32 y)
{
    return wmap_isTileInState(map, x, y, MAP_TILE_STATE_UNKOWN);
}

bool wmap_isTileFog(WarMap* map, s32 x, s32 y)
{
    return wmap_isTileInState(map, x, y, MAP_TILE_STATE_FOG);
}

bool wmap_isTileVisible(WarMap* map, s32 x, s32 y)
{
    return wmap_isTileInState(map, x, y, MAP_TILE_STATE_VISIBLE);
}

bool wmap_isPositionVisible(WarMap* map, vec2 position)
{
    vec2 tilePos = wmap_mapToTileCoordinatesV(position);
    s32 tileX = (s32)tilePos.x;
    s32 tileY = (s32)tilePos.y;
    if (tileX < 0 || tileX >= MAP_TILES_WIDTH || tileY < 0 || tileY >= MAP_TILES_HEIGHT)
    {
        return false;
    }
    return wmap_isTileVisible(map, tileX, tileY);
}


WarColor wmap_getMapTileAverage(WarResource* levelVisual, WarResource* tileset, s32 x, s32 y)
{
    s32 index = y * MAP_TILES_WIDTH + x;
    u16 tileIndex = levelVisual->levelVisual.data[index];

    s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
    s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

    s32 r = 0, g = 0, b = 0;

    for(s32 ty = 0; ty < MEGA_TILE_HEIGHT; ty++)
    {
        for(s32 tx = 0; tx < MEGA_TILE_WIDTH; tx++)
        {
            s32 pixel = (tilePixelY + ty) * TILESET_WIDTH + (tilePixelX + tx);
            r += tileset->tilesetData.data[pixel * 4 + 0];
            g += tileset->tilesetData.data[pixel * 4 + 1];
            b += tileset->tilesetData.data[pixel * 4 + 2];
        }
    }

    r /= 256;
    g /= 256;
    b /= 256;

    WarColor color = {0};
    color.r = (u8)r;
    color.g = (u8)g;
    color.b = (u8)b;
    color.a = 255;
    return color;
}

void wmap_updateMinimapTile(WarContext* context, WarResource* levelVisual, WarResource* tileset, s32 x, s32 y)
{
    WarMap* map = context->map;
    WarSpriteFrame* minimapFrame = &map->minimapSprite.frames[1];

    WarColor color = WAR_COLOR_BLACK;

    s32 index = y * MAP_TILES_WIDTH + x;
    WarMapTile* tile = &map->tiles[index];

    if (!map->fowEnabled ||
        tile->state == MAP_TILE_STATE_VISIBLE ||
        tile->state == MAP_TILE_STATE_FOG)
    {
        color = wmap_getMapTileAverage(levelVisual, tileset, x, y);
    }

    minimapFrame->data[index * 4 + 0] = color.r;
    minimapFrame->data[index * 4 + 1] = color.g;
    minimapFrame->data[index * 4 + 2] = color.b;
    minimapFrame->data[index * 4 + 3] = color.a;
}

s32 wmap_getMapTileIndex(WarContext* context, s32 x, s32 y)
{
    WarMap* map = context->map;

    WarResource* levelInfo = wres_getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource* levelVisual = wres_getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
    assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

    return levelVisual->levelVisual.data[y * MAP_TILES_WIDTH + x];
}

void wmap_setMapTileIndex(WarContext* context, s32 x, s32 y, s32 tile)
{
    WarMap* map = context->map;

    WarResource* levelInfo = wres_getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource* levelVisual = wres_getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
    assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

    WarResource* tileset = wres_getOrCreateResource(context, levelInfo->levelInfo.tilesetIndex);
    assert(tileset && tileset->type == WAR_RESOURCE_TYPE_TILESET);

    assert(tile >= 0 && tile <= UINT16_MAX);
    levelVisual->levelVisual.data[y * MAP_TILES_WIDTH + x] = (u16)tile;

    wmap_updateMinimapTile(context, levelVisual, tileset, x, y);
}

f32 wmap_getMapScaledTime(WarContext* context, f32 t)
{
    WarMap* map = context->map;

    t /= context->globalSpeed;

    if (map->settings.gameSpeed < WAR_SPEED_NORMAL)
        t /= 1.0f - (WAR_SPEED_NORMAL - map->settings.gameSpeed) * 0.25f;
    else if (map->settings.gameSpeed > WAR_SPEED_NORMAL)
        t /= 1.0f + (map->settings.gameSpeed - WAR_SPEED_NORMAL) * 0.5f;

    return t;
}

f32 wmap_getMapScrollSpeed(WarContext* context, f32 value)
{
    NOT_USED(context);

    const f32 baseSpeed = 100.0f;
    const f32 speedMultiplier = 50.0f;

    return baseSpeed + value * speedMultiplier;
}

WarMap* wmap_createMap(WarContext* context, s32 levelInfoIndex)
{
    WarMap *map = (WarMap*)wm_alloc(sizeof(WarMap));
    map->levelInfoIndex = levelInfoIndex;

    we_initEntityManager(context, &map->entityManager);

    WarEntityIdListInit(&map->selectedEntities, wm_globalAllocator());
    for (s32 i = 0; i < MAX_SELECTION_GROUPS; i++)
    {
        WarEntityIdListInit(&map->selectionGroups[i], wm_globalAllocator());
    }

    return map;
}

WarMap* wmap_createCustomMap(WarContext* context, s32 levelInfoIndex, WarRace yourRace, WarRace enemyRace)
{
    WarMap* map = wmap_createMap(context, levelInfoIndex);

    WarResource* levelInfo = wres_getOrCreateResource(context, levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO && levelInfo->levelInfo.customMap);
    assert(levelInfo->levelInfo.startConfigurationsCount > 0);

    levelInfo->levelInfo.startEntitiesCount = 0;

    if (!levelInfo->levelInfo.startEntities)
    {
        levelInfo->levelInfo.startEntities = (WarLevelUnit*)wm_alloc(MAX_ENTITIES_COUNT * sizeof(WarLevelUnit));
        assert(levelInfo->levelInfo.startEntities);
    }

    levelInfo->levelInfo.races[0] = yourRace;
    levelInfo->levelInfo.races[1] = enemyRace;

    for (s32 i = 0; i < (s32)levelInfo->levelInfo.startGoldminesCount; i++)
    {
        WarLevelUnit* startUnitConf = &levelInfo->levelInfo.startGoldmines[i];

        assert(levelInfo->levelInfo.startEntitiesCount < MAX_ENTITIES_COUNT);
        WarLevelUnit* startUnit = &levelInfo->levelInfo.startEntities[levelInfo->levelInfo.startEntitiesCount];
        startUnit->x = startUnitConf->x;
        startUnit->y = startUnitConf->y;
        startUnit->type = startUnitConf->type;
        startUnit->player = startUnitConf->player;
        startUnit->resourceKind = WAR_RESOURCE_GOLD;
        startUnit->amount = (u16)randomi(20000, 30000);

        levelInfo->levelInfo.startEntitiesCount++;
    }

    s32 configurationIndex = randomi(0, (s32)levelInfo->levelInfo.startConfigurationsCount);
    WarCustomMapConfiguration* configuration = &levelInfo->levelInfo.startConfigurations[configurationIndex];
    assert(configuration->startEntities);

    for (s32 i = 0; i < (s32)configuration->startEntitiesCount; i++)
    {
        WarLevelUnit* startUnitConf = &configuration->startEntities[i];

        assert(levelInfo->levelInfo.startEntitiesCount < MAX_ENTITIES_COUNT);
        WarLevelUnit* startUnit = &levelInfo->levelInfo.startEntities[levelInfo->levelInfo.startEntitiesCount];
        startUnit->x = startUnitConf->x;
        startUnit->y = startUnitConf->y;
        startUnit->player = startUnitConf->player;
        startUnit->type = startUnit->player == 0
            ? wu_getUnitTypeForRace(startUnitConf->type, yourRace)
            : wu_getUnitTypeForRace(startUnitConf->type, enemyRace);

        levelInfo->levelInfo.startEntitiesCount++;
    }

    return map;
}

bool wmap_loadCustomMap(WarContext* context, StringView mapPath)
{
    // Resource indices for the three tilesets (forest=0, swamp=1, dungeon=2).
    static const s32 tilesetResourceIndices[] = { 189, 192, 195 };

    WarResource* levelInfoRes = wres_getOrCreateResource(context, WAR_CUSTOM_LEVEL_INFO_INDEX);
    assert(levelInfoRes);

    WarResource* visualInfoRes = wres_getOrCreateResource(context, WAR_CUSTOM_VISUAL_INDEX);
    assert(visualInfoRes);

    WarResource* passableInfoRes = wres_getOrCreateResource(context, WAR_CUSTOM_PASSABLE_INDEX);
    assert(passableInfoRes);

    if (!wfile_loadWarMapFile(mapPath, levelInfoRes, visualInfoRes, passableInfoRes))
    {
        logError("wmap_loadCustomMap: wfile_loadWarMapFile failed for '%.*s'", (s32)mapPath.length, mapPath.data);
        return false;
    }

    levelInfoRes->type = WAR_RESOURCE_TYPE_LEVEL_INFO;
    visualInfoRes->type = WAR_RESOURCE_TYPE_LEVEL_VISUAL;
    passableInfoRes->type = WAR_RESOURCE_TYPE_LEVEL_PASSABLE;

    // Wire the levelInfo to the new tile resources.
    levelInfoRes->levelInfo.visualIndex   = WAR_CUSTOM_VISUAL_INDEX;
    levelInfoRes->levelInfo.passableIndex = WAR_CUSTOM_PASSABLE_INDEX;

    // Select the pre-loaded tileset by tilesetType (bounds-clamped).
    s32 tidx = CLAMP((s32)levelInfoRes->levelInfo.tilesetType, 0, (s32)arrayLength(tilesetResourceIndices) - 1);
    levelInfoRes->levelInfo.tilesetIndex = (u16)tilesetResourceIndices[tidx];

    WarMap* map = wmap_createMap(context, WAR_CUSTOM_LEVEL_INFO_INDEX);
    if (!map)
    {
        logError("wmap_loadCustomMap: wmap_createMap failed for '%.*s'", (s32)mapPath.length, mapPath.data);
        return false;
    }

    map->custom = true;
    wg_setNextMap(context, map, 0.0f);

    logInfo("wmap_loadCustomMap: loaded '%.*s' (entities=%u, tilesetType=%d)",
            (s32)mapPath.length, mapPath.data,
            levelInfoRes->levelInfo.startEntitiesCount,
            (s32)levelInfoRes->levelInfo.tilesetType);

    return true;
}

void wmap_freeMap(WarContext* context, WarMap* map)
{
    wspr_freeSprite(context, map->sprite);
    wspr_freeSprite(context, map->minimapSprite);
    wspr_freeSprite(context, map->blackSprite);

    WarEntityManager* manager = &map->entityManager;
    WarEntityMapFree(&manager->entitiesByType);
    WarUnitMapFree(&manager->unitsByType);
    WarEntityIdMapFree(&manager->entitiesById);
    WarEntityListFree(&manager->uiEntities);

    WarEntityIdListFree(&map->selectedEntities);
    for (s32 i = 0; i < MAX_SELECTION_GROUPS; i++)
    {
        WarEntityIdListFree(&map->selectionGroups[i]);
    }
}

void wmap_enterMap(WarContext* context)
{
    WarMap* map = context->map;

    s32 levelInfoIndex = map->levelInfoIndex;

    WarResource* levelInfo = wres_getOrCreateResource(context, levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource* levelPassable = wres_getOrCreateResource(context, levelInfo->levelInfo.passableIndex);
    assert(levelPassable && levelPassable->type == WAR_RESOURCE_TYPE_LEVEL_PASSABLE);

    map->playing = true;
    map->custom = levelInfo->levelInfo.customMap;
    map->tilesetType = levelInfo->levelInfo.tilesetType;
    map->fowEnabled = true;
    map->result = WAR_LEVEL_RESULT_NONE;
    map->objectivesTime = 1;

    map->settings.gameSpeed = WAR_SPEED_NORMAL;
    map->settings.mouseScrollSpeed = WAR_SPEED_NORMAL;
    map->settings.keyScrollSpeed = WAR_SPEED_NORMAL;

    map->ui.leftTopPanel = recti(0, 0, 72, 72);
    map->ui.leftBottomPanel = recti(0, 72, 72, 128);
    map->ui.rightPanel = recti(312, 0, 8, 200);
    map->ui.topPanel = recti(72, 0, 240, 12);
    map->ui.bottomPanel = recti(72, 188, 240, 12);
    map->ui.mapPanel = recti(72, 12, MAP_VIEWPORT_WIDTH, MAP_VIEWPORT_HEIGHT);
    map->ui.minimapPanel = recti(3, 6, MINIMAP_WIDTH, MINIMAP_HEIGHT);
    map->ui.menuPanel = recti(84, 32, 152, 136);
    map->ui.messagePanel = recti(17, 76, 286, 48);
    map->ui.saveLoadPanel = recti(48, 27, 223, 146);

    initCamera(map, levelInfo);
    initPathFinder(map, levelPassable);
    wgrid_clear(context);
    initBlackSprite(context, map);
    setInitialTileState(map);
    createMapAndMinimapSprites(context, map, levelInfo);
    createForestEntities(context, map, levelPassable);
    createStartingRoads(context, map, levelInfo);
    createStartingWalls(context, map, levelInfo);
    createRuinEntity(context, map);
    initPlayersInfo(map, levelInfo);
    createStartingEntities(context, levelInfo);
    wai_initAIPlayers(context);
    wmui_createMapUI(context);

    if (!isDemo(context))
        wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_MUSIC_00, .loop=true));
}

void wmap_leaveMap(WarContext* context)
{
    if (context->map)
    {
        wmap_freeMap(context, context->map);
        context->map = NULL;
    }
}

static void updateViewport(WarContext *context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    map->camera.wasScrolling = false;

    if (map->commandState.suppressMinimapViewportOnRelease)
    {
        map->camera.isScrolling = false;

        if (!isButtonHeld(input, WAR_MOUSE_LEFT))
            map->commandState.suppressMinimapViewportOnRelease = false;

        return;
    }

    vec2 dir = VEC2_ZERO;
    bool wasScrolling = map->camera.isScrolling;
    bool mouseScroll = false;
    bool keyScroll = false;

    // if there was a click in the minimap, then update the position of the viewport
    if (isButtonHeld(input, WAR_MOUSE_LEFT) && map->commandState.command.type == WAR_COMMAND_NONE)
    {
        // check if the click is inside the minimap panel
        if (rect_containsf(map->ui.minimapPanel, input->pos.x, input->pos.y))
        {
            vec2 minimapSize = vec2i(MINIMAP_WIDTH, MINIMAP_HEIGHT);
            vec2 offset = wmap_screenToMinimapCoordinatesV(context, input->pos);
            offset = wmap_minimapToViewportCoordinatesV(context, offset);

            map->camera.viewport.x = offset.x * MAP_WIDTH / minimapSize.x;
            map->camera.viewport.y = offset.y * MAP_HEIGHT / minimapSize.y;
        }
    }
    // check for the arrows keys and update the position of the viewport
    else
    {
        if (!isMapDragging(input))
        {
            dir = wmap_getDirFromMousePos(context);
            mouseScroll = !VEC2_IS_ZERO(dir);
        }

        // don't scroll with arrow keys if Control or Shift are pressed
        // don't scroll with arrow keys if the cheat status is active
        if (!mouseScroll &&
            !isKeyHeld(input, WAR_KEY_CTRL) &&
            !isKeyHeld(input, WAR_KEY_SHIFT) &&
            !(map->status.cheatStatus.enabled && map->status.cheatStatus.visible))
        {
            dir = wmap_getDirFromArrowKeys(context);
            keyScroll = !VEC2_IS_ZERO(dir);
        }
    }

    map->camera.isScrolling = !VEC2_IS_ZERO(dir);
    if (map->camera.isScrolling)
    {
        assert(mouseScroll || keyScroll);

        f32 scrollSpeed = 0.0f;
        if (mouseScroll)
            scrollSpeed = wmap_getMapScrollSpeed(context, map->settings.mouseScrollSpeed);
        else if (keyScroll)
            scrollSpeed = wmap_getMapScrollSpeed(context, map->settings.keyScrollSpeed);

        map->camera.viewport.x += scrollSpeed * dir.x * context->realDeltaTime;
        map->camera.viewport.x = CLAMP(map->camera.viewport.x, 0.0f, MAP_WIDTH - map->camera.viewport.width);

        map->camera.viewport.y += scrollSpeed * dir.y * context->realDeltaTime;
        map->camera.viewport.y = CLAMP(map->camera.viewport.y, 0.0f, MAP_HEIGHT - map->camera.viewport.height);
    }
    else
    {
        map->camera.wasScrolling = wasScrolling;
    }
}

static void updateDragRect(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->camera.isScrolling)
    {
        input->mapDragState.status = WAR_DRAG_NONE;
        input->mapDragState.startPos = VEC2_ZERO;
        input->mapDragState.rect = RECT_EMPTY;
        return;
    }

    switch (input->mapDragState.status)
    {
        case WAR_DRAG_STARTED:
        case WAR_DRAG_ACTIVE:
        {
            if (isButtonHeld(input, WAR_MOUSE_LEFT))
            {
                input->mapDragState.status = WAR_DRAG_ACTIVE;
            }
            else if (isButtonJustReleased(input, WAR_MOUSE_LEFT))
            {
                input->mapDragState.status = WAR_DRAG_RELEASED;
            }

            input->mapDragState.rect = rectpf(input->mapDragState.startPos.x, input->mapDragState.startPos.y, input->pos.x, input->pos.y);
            break;
        }
        case WAR_DRAG_RELEASED:
        {
            input->mapDragState.status = WAR_DRAG_NONE;
            input->mapDragState.startPos = VEC2_ZERO;
            input->mapDragState.rect = RECT_EMPTY;
            break;
        }
        default:
        {
            if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
            {
                if (!input->capturedUIButtonId && rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    input->mapDragState.status = WAR_DRAG_STARTED;
                    input->mapDragState.startPos = input->pos;
                    input->mapDragState.rect = rectv(input->pos, VEC2_ZERO);
                }
            }
            break;
        }
    }
}

static bool isMapSelectionRect(rect rect)
{
    return rect.width >= MAP_SELECTION_DRAG_THRESHOLD ||
           rect.height >= MAP_SELECTION_DRAG_THRESHOLD;
}

static bool isEntitySelectable(WarContext* context, WarMap* map, WarEntity* entity)
{
    if (!entity || !we_isComponentEnabled(context, entity, COMP_UNIT))
        return false;

    if (wst_isDead(context, entity) ||
        wst_isGoingToDie(context, entity) ||
        wu_isCorpseUnit(context, entity) ||
        wst_isCollapsing(context, entity) ||
        wst_isGoingToCollapse(context, entity) ||
        (wu_isWorkerUnit(context, entity) && wst_isInsideBuilding(context, entity)) ||
        !wmap_isUnitPartiallyVisible(context, map, entity))
    {
        return false;
    }

    return true;
}

static void updateSelectionFromList(WarContext* context, WarEntityList* newSelectedEntities)
{
    TracyCZoneN(ctx, "UpdateSelectionFromList", 1);

    WarMap* map = context->map;

    for (s32 i = newSelectedEntities->count - 1; i >= 0; i--)
    {
        WarEntity* entity = newSelectedEntities->items[i];
        for (s32 j = 0; j < i; j++)
        {
            if (newSelectedEntities->items[j] == entity)
            {
                WarEntityListRemoveAt(newSelectedEntities, i);
                break;
            }
        }
    }

    for (s32 i = newSelectedEntities->count - 1; i >= 0; i--)
    {
        WarEntity* entity = newSelectedEntities->items[i];
        if (!isEntitySelectable(context, map, entity))
            WarEntityListRemoveAt(newSelectedEntities, i);
    }

    bool areDudesSelected = false;
    bool areBuildingSelected = false;

    // calculate the number of dudes and buildings in the selection
    for (s32 i = 0; i < newSelectedEntities->count; i++)
    {
        WarEntity* entity = newSelectedEntities->items[i];
        if (wu_isDudeUnit(context, entity))
            areDudesSelected = true;
        else if (wu_isBuildingUnit(context, entity))
            areBuildingSelected = true;
    }

    if (areDudesSelected)
    {
        // remove all new selected buildings
        for (s32 i = newSelectedEntities->count - 1; i >= 0; i--)
        {
            WarEntity* entity = newSelectedEntities->items[i];
            if (wu_isBuildingUnit(context, entity))
                WarEntityListRemoveAt(newSelectedEntities, i);
        }
    }
    else if (areBuildingSelected)
    {
        // remove all other new selected buildings
        WarEntityListRemoveAtRange(newSelectedEntities, 1, newSelectedEntities->count - 1);
    }

    if (areDudesSelected)
    {
        if (newSelectedEntities->count == 1)
        {
            WarEntity* newSelectedEntity = newSelectedEntities->items[0];
            if (wu_isFriendlyUnit(context, newSelectedEntity))
            {
                wa_playDudeSelectionSound(context, newSelectedEntity);
            }
            else
            {
                wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_UI_CLICK, .loop=false));
            }
        }
    }
    else if (areBuildingSelected)
    {
        WarEntity* newSelectedEntity = newSelectedEntities->items[0];
        if (wu_isFriendlyUnit(context, newSelectedEntity))
        {
            wa_playBuildingSelectionSound(context, newSelectedEntity);
        }
    }

    // if the new selected entity is the same one, don't clear the command, otherwise do
    if (newSelectedEntities->count == 1 && map->selectedEntities.count == 1)
    {
        WarEntity* newSelectedEntity = newSelectedEntities->items[0];
        WarEntityId selectedEntityId = map->selectedEntities.items[0];
        if (selectedEntityId != newSelectedEntity->id)
        {
            map->commandState.command.type = WAR_COMMAND_NONE;
        }
    }
    else
    {
        map->commandState.command.type = WAR_COMMAND_NONE;
    }

    // clear the current selection
    wmap_clearSelection(context);

    // and add the new selection
    s32 selectedEntitiesCount = MIN(newSelectedEntities->count, MAX_SELECTED_ENTITIES_COUNT);
    for (s32 i = 0; i < selectedEntitiesCount; i++)
    {
        WarEntity* entity = newSelectedEntities->items[i];
        wmap_addEntityToSelection(context, entity->id);
    }

    TracyCPlotI("SelectedEntities", (s64)map->selectedEntities.count);
    TracyCZoneEnd(ctx);
}

static void updateSelection(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    // if it was scrolling last frame, don't perform any selection this frame
    // also if the mouse is still dragging, don't perform any selection, the selection will be performed when the mouse will be released
    if (map->camera.wasScrolling || input->mapDragState.status != WAR_DRAG_RELEASED)
    {
        return;
    }

    if (map->commandState.suppressSelectionOnRelease)
    {
        map->commandState.suppressSelectionOnRelease = false;
        return;
    }

    WarEntityList newSelectedEntities;
    WarEntityListInit(&newSelectedEntities, wm_frameAllocator());

    if (isMapSelectionRect(input->mapDragState.rect))
    {
        rect pointerRect = wmap_screenToMapCoordinatesR(context, input->mapDragState.rect);

        WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
        for (s32 i = 0; i < units->count; i++)
        {
            WarEntity* entity = units->items[i];
            if (isEntitySelectable(context, map, entity))
            {
                rect unitRect = wu_getUnitRect(context, entity);
                if (rect_intersects(pointerRect, unitRect))
                {
                    WarEntityListAdd(&newSelectedEntities, entity);
                }
            }
        }
    }
    else
    {
        WarEntity* entityUnderCursor = we_findEntityUnderCursor(context, false, false);
        if (isEntitySelectable(context, map, entityUnderCursor))
        {
            WarEntityListAdd(&newSelectedEntities, entityUnderCursor);
        }
    }

    // include the already selected entities if the Ctrl key is pressed
    if (isKeyHeld(input, WAR_KEY_CTRL))
    {
        for (s32 i = 0; i < map->selectedEntities.count; i++)
        {
            WarEntity* entity = we_findEntity(context, map->selectedEntities.items[i]);
            if (entity)
                WarEntityListAdd(&newSelectedEntities, entity);
        }
    }

    updateSelectionFromList(context, &newSelectedEntities);
    WarEntityListFree(&newSelectedEntities);
}

static void updateDebugRenderShortcuts(WarContext* context)
{
    if (!context->cheatsEnabled)
        return;

    WarInput* input = &context->input;

    if (!isKeyHeld(input, WAR_KEY_CTRL) || !isKeyHeld(input, WAR_KEY_SHIFT))
        return;

    struct { WarKeys key; WarDebugRenderFlag flag; } shortcuts[] =
    {
        { WAR_KEY_G, WAR_DEBUG_RENDER_MAP_GRID         },
        { WAR_KEY_S, WAR_DEBUG_RENDER_SPATIAL_GRID      },
        { WAR_KEY_N, WAR_DEBUG_RENDER_NEAR_UNITS       },
        { WAR_KEY_P, WAR_DEBUG_RENDER_PASSABLE_INFO    },
        { WAR_KEY_I, WAR_DEBUG_RENDER_UNIT_INFO        },
        { WAR_KEY_U, WAR_DEBUG_RENDER_UNIT_ANIMATIONS  },
        { WAR_KEY_A, WAR_DEBUG_RENDER_MAP_ANIMATIONS   },
        { WAR_KEY_F, WAR_DEBUG_RENDER_FONT             },
        { WAR_KEY_J, WAR_DEBUG_RENDER_PROJECTILES      },
        { WAR_KEY_W, WAR_DEBUG_RENDER_FLOW_FIELD       },
        { WAR_KEY_R, WAR_DEBUG_RENDER_RVO              },
    };

    for (s32 i = 0; i < arrayLength(shortcuts); i++)
    {
        if (isKeyJustPressed(input, shortcuts[i].key))
        {
            context->debugRender.flags[shortcuts[i].flag] =
                !context->debugRender.flags[shortcuts[i].flag];
        }
    }
}

static void updateSelectionGroups(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateSelectionGroups", 1);

    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->status.cheatStatus.visible)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    for (s32 i = 0; i < MAX_SELECTION_GROUPS; i++)
    {
        WarKeys key = (WarKeys)(WAR_KEY_1 + i);
        if (!isKeyJustPressed(input, key))
            continue;

        WarEntityIdList* selectionGroup = &map->selectionGroups[i];
        if (isKeyHeld(input, WAR_KEY_CTRL))
        {
            WarEntityIdListClear(selectionGroup);

            s32 selectedEntitiesCount = MIN(map->selectedEntities.count, MAX_SELECTED_ENTITIES_COUNT);
            for (s32 j = 0; j < selectedEntitiesCount; j++)
            {
                WarEntityIdListAdd(selectionGroup, map->selectedEntities.items[j]);
            }

            TracyCPlotI("ControlGroupSize", (s64)selectionGroup->count);
            break;
        }

        if (selectionGroup->count == 0)
            break;

        WarEntityList newSelectedEntities;
        WarEntityListInit(&newSelectedEntities, wm_frameAllocator());

        for (s32 j = 0; j < selectionGroup->count; j++)
        {
            WarEntity* entity = we_findEntity(context, selectionGroup->items[j]);
            if (isEntitySelectable(context, map, entity))
                WarEntityListAdd(&newSelectedEntities, entity);
        }

        TracyCPlotI("ControlGroupRecallSize", (s64)newSelectedEntities.count);
        updateSelectionFromList(context, &newSelectedEntities);
        WarEntityListFree(&newSelectedEntities);
        break;
    }

    TracyCZoneEnd(ctx);
}

static void updateTreesEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->editing.mode != WAR_MAP_EDIT_MODE_TREES)
        return;

    if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
    {
        if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = wmap_screenToMapCoordinatesV(context, input->pos);
            pointerPos =  wmap_mapToTileCoordinatesV(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntityId entityId = getTileEntityId(&map->finder, x, y);
            WarEntity* entity = we_findEntity(context, entityId);
            if (!entity)
            {
                entity = map->editing.forest;

                we_plantTree(context, entity, x, y);
                we_determineAllTreeTiles(context);
            }
            else if (entity->type == WAR_ENTITY_TYPE_FOREST)
            {
                WarTree* tree = we_getTreeAtPosition(context, entity, x, y);
                if (tree)
                {
                    we_chopTree(context, entity, tree, TREE_MAX_WOOD);
                    we_determineAllTreeTiles(context);
                }
            }
        }
    }
}

void updateRoadsEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->editing.mode != WAR_MAP_EDIT_MODE_ROADS)
        return;

    if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
    {
        if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = wmap_screenToMapCoordinatesV(context, input->pos);
            pointerPos =  wmap_mapToTileCoordinatesV(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntity* road = map->editing.road;

            WarRoadPiece* piece = we_getRoadPieceAtPosition(context, road, x, y);
            if (!piece)
            {
                we_addRoadPiece(context, road, x, y, 0);
                we_determineRoadTypes(context, road);
            }
            else
            {
                we_removeRoadPiece(context, road, piece);
                we_determineRoadTypes(context, road);
            }
        }
    }
}

static void updateWallsEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->editing.mode != WAR_MAP_EDIT_MODE_WALLS)
        return;

    if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
    {
        if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = wmap_screenToMapCoordinatesV(context, input->pos);
            pointerPos =  wmap_mapToTileCoordinatesV(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntity* wall = map->editing.wall;

            WarWallPiece* piece = we_getWallPieceAtPosition(context, wall, x, y);
            if (!piece)
            {
                WarWallPiece* newPiece = we_addWallPiece(context, wall, x, y, 0);
                newPiece->hp = WAR_WALL_MAX_HP;
                newPiece->maxhp = WAR_WALL_MAX_HP;

                we_determineWallTypes(context, wall);
            }
            else
            {
                setFreeTiles(&map->finder, piece->tilex, piece->tiley, 1, 1);

                we_removeWallPiece(context, wall, piece);
                we_determineWallTypes(context, wall);
            }
        }
    }
}

void updateRuinsEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->editing.mode != WAR_MAP_EDIT_MODE_RUINS)
        return;

    if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
    {
        if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = wmap_screenToMapCoordinatesV(context, input->pos);
            pointerPos =  wmap_mapToTileCoordinatesV(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntity* ruin = map->editing.ruin;

            WarRuinPiece* piece = we_getRuinPieceAtPosition(context, ruin, x, y);
            if (!piece)
            {
                we_addRuinsPieces(context, ruin, x, y, 2);
                we_determineRuinTypes(context, ruin);
            }
            else
            {
                we_removeRuinPiece(context, ruin, piece);
                we_determineRuinTypes(context, ruin);
            }
        }
    }
}

static void updateRainOfFireEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->editing.mode != WAR_MAP_EDIT_MODE_RAIN_OF_FIRE)
        return;

    if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
    {
        if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
        {
            vec2 target = wmap_screenToMapCoordinatesV(context, input->pos);
            castDebugRainOfFire(context, wmap_mapToTileCoordinatesV(target));
        }

    }
}

static void updatePoisonCloudEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->editing.mode != WAR_MAP_EDIT_MODE_POISON_CLOUD)
        return;

    if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
    {
        if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
        {
            vec2 target = wmap_screenToMapCoordinatesV(context, input->pos);
            castDebugPoisonCloud(context, wmap_mapToTileCoordinatesV(target));
        }
    }
}

static void updateRaiseDeadEdit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->editing.mode != WAR_MAP_EDIT_MODE_RAISE_DEAD)
        return;

    if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
    {
        if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
        {
            vec2 target = wmap_screenToMapCoordinatesV(context, input->pos);
            castDebugRaiseDead(context, wmap_mapToTileCoordinatesV(target));
        }
    }
}

static void updateAddUnit(WarContext* context)
{
    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (map->editing.mode != WAR_MAP_EDIT_MODE_ADD_UNIT)
        return;

    if (isButtonJustPressed(input, WAR_MOUSE_LEFT))
    {
        if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
        {
            vec2 pointerPos = wmap_screenToMapCoordinatesV(context, input->pos);
            pointerPos =  wmap_mapToTileCoordinatesV(pointerPos);

            s32 x = (s32)pointerPos.x;
            s32 y = (s32)pointerPos.y;

            WarEntityId entityId = getTileEntityId(&map->finder, x, y);
            if (!entityId)
            {
                WarRace addingUnitRace = wu_getUnitTypeRace(map->editing.pendingUnitType);
                for (s32 i = 0; i < MAX_PLAYERS_COUNT; i++)
                {
                    if (map->players[i].race == addingUnitRace)
                    {
                        WarEntity* addedUnit = we_createUnit(context, CREATE_UNIT_ARGS_INIT(.type=map->editing.pendingUnitType, .x=x, .y=y, .player=map->players[i].index, .resourceKind=WAR_RESOURCE_NONE, .amount=0, .addToMap=true));
                        we_setInitialIdleState(context, addedUnit);
                        break;
                    }
                }
            }
        }
    }
}

static void updateFlowFieldDebug(WarContext* context)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_FLOW_FIELD])
        return;

    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (!isButtonJustPressed(input, WAR_MOUSE_LEFT))
        return;

    if (input->capturedUIButtonId)
        return;

    if (!rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
        return;

    vec2 pointerPos = wmap_screenToMapCoordinatesV(context, input->pos);
    pointerPos = wmap_mapToTileCoordinatesV(pointerPos);

    s32 x = (s32)pointerPos.x;
    s32 y = (s32)pointerPos.y;

    if (!wpath_isInside(x, y))
        return;

    map->debug.flowField = wpath_computeFlowField(&map->finder, x, y);
    map->debug.flowFieldX = x;
    map->debug.flowFieldY = y;
}

static void updateCommandButtons(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateCommandButtons", 1);

    WarMap* map = context->map;

    // Reset all IMGUI command panel state.
    for (s32 i = 0; i < 6; i++)
    {
        map->commandPanel.slotsActive[i]   = false;
        map->commandPanel.slots[i]        = (WarUnitCommandData){0};
    }

    for (s32 i = 0; i < 4; i++)
    {
        map->commandPanel.textsVisible[i]         = false;
        map->commandPanel.texts[i][0]            = '\0';
        map->commandPanel.textsHighlightIndex[i]  = NO_HIGHLIGHT;
        map->commandPanel.textsHighlightCount[i]  = 0;
    }

    s32 selectedEntitiesCount = map->selectedEntities.count;
    if (selectedEntitiesCount == 0)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    WarEntity* entity = we_findEntity(context, map->selectedEntities.items[0]);
    assert(entity && wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    // if the selected unit is a farm,
    // just show the text about the food consumtion
    //
    // FIX: this information shouldn't be visible if the selected unit is not friendly
    if (unit->type == WAR_UNIT_FARM_HUMANS || unit->type == WAR_UNIT_FARM_ORCS)
    {
        if (!unit->building)
        {
            s32 farmsCount = wu_getNumberOfBuildingsOfType(context, unit->player, unit->type, true);
            s32 dudesCount = wu_getTotalNumberOfDudes(context, unit->player);

            snprintf(map->commandPanel.texts[0], sizeof(map->commandPanel.texts[0]), "FOOD USAGE:");
            map->commandPanel.textsVisible[0] = true;
            snprintf(map->commandPanel.texts[1], sizeof(map->commandPanel.texts[1]), "GROWN %d", farmsCount * 4 + 1);
            map->commandPanel.textsVisible[1] = true;
            snprintf(map->commandPanel.texts[2], sizeof(map->commandPanel.texts[2]), " USED %d", dudesCount);
            map->commandPanel.textsVisible[2] = true;

            TracyCZoneEnd(ctx);
            return;
        }
    }

    // if the selected unit is a goldmine,
    // just add the text with the remaining gold
    if (unit->type == WAR_UNIT_GOLDMINE)
    {
        s32 gold = unit->amount;

        snprintf(map->commandPanel.texts[0], sizeof(map->commandPanel.texts[0]), "GOLD LEFT");
        map->commandPanel.textsVisible[0] = true;
        snprintf(map->commandPanel.texts[3], sizeof(map->commandPanel.texts[3]), "%d", gold);
        map->commandPanel.textsVisible[3] = true;

        TracyCZoneEnd(ctx);
        return;
    }

    // determine the commands for the selected unit(s)
    WarUnitCommandType commands[6] = {0};
    wu_getUnitCommands(context, entity, commands);

    if (selectedEntitiesCount > 1)
    {
        WarUnitCommandType selectedCommands[6] = {0};
        for (s32 i = 1; i < selectedEntitiesCount; i++)
        {
            WarEntity* selectedEntity = we_findEntity(context, map->selectedEntities.items[i]);
            assert(selectedEntity && wu_isUnit(selectedEntity));

            memset(selectedCommands, 0, sizeof(selectedCommands));
            wu_getUnitCommands(context, selectedEntity, selectedCommands);

            for (s32 j = 0; j < arrayLength(commands); j++)
            {
                if (commands[j] != selectedCommands[j])
                {
                    commands[j] = WAR_COMMAND_NONE;
                }
            }
        }
    }

    for (s32 i = 0; i < arrayLength(commands); i++)
    {
        if (commands[i] != WAR_COMMAND_NONE)
        {
            map->commandPanel.slots[i]       = wu_getUnitCommandData(context, entity, commands[i]);
            map->commandPanel.slotsActive[i]  = true;
        }
    }

    TracyCZoneEnd(ctx);
}

void updateCommandFromRightClick(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateCommandFromRightClick", 1);

    WarMap* map = context->map;
    WarUnitCommand* command = &map->commandState.command;
    WarInput* input = &context->input;

    if (isButtonJustPressed(input, WAR_MOUSE_RIGHT))
    {
        if (command->type == WAR_COMMAND_NONE)
        {
            s32 selEntitiesCount = map->selectedEntities.count;
            if (selEntitiesCount > 0)
            {
                // if the right click was on the map
                if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
                {
                    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                    WarEntityId targetEntityId = getTileEntityId(&map->finder, (s32)targetTile.x, (s32)targetTile.y);
                    WarEntity* targetEntity = we_findEntity(context, targetEntityId);
                    if (targetEntity)
                    {
                        if (wu_isUnitOfType(context, targetEntity, WAR_UNIT_GOLDMINE))
                        {
                            if (!wmap_isUnitUnknown(context, map, targetEntity))
                                wcmd_executeHarvestCommand(context, targetEntity, targetTile);
                            else
                                wcmd_executeMoveCommand(context, targetPoint);
                        }
                        else if (isEntityOfType(targetEntity, WAR_ENTITY_TYPE_FOREST))
                        {
                            if (wmap_isTileVisible(map, (s32)targetTile.x, (s32)targetTile.y))
                            {
                                wcmd_executeHarvestCommand(context, targetEntity, targetTile);
                            }
                            else
                            {
                                WarTree* tree = we_findAccesibleTree(context, targetEntity, targetTile);
                                if (tree)
                                {
                                    targetTile = vec2i(tree->tilex, tree->tiley);
                                    wcmd_executeHarvestCommand(context, targetEntity, targetTile);
                                }
                                else
                                {
                                    wcmd_executeMoveCommand(context, targetPoint);
                                }
                            }
                        }
                        else if (wu_isUnitOfType(context, targetEntity, WAR_UNIT_TOWNHALL_HUMANS) ||
                                 wu_isUnitOfType(context, targetEntity, WAR_UNIT_TOWNHALL_ORCS))
                        {
                            if (!wmap_isUnitUnknown(context, map, targetEntity))
                            {
                                if (wu_isEnemyUnit(context, targetEntity))
                                {
                                    wcmd_executeAttackCommand(context, targetEntity, targetTile);
                                }
                                else
                                {
                                    wcmd_executeDeliverCommand(context, targetEntity);
                                }
                            }
                            else
                            {
                                wcmd_executeMoveCommand(context, targetPoint);
                            }
                        }
                        else if (wu_isWall(targetEntity))
                        {
                            // it doesn't matter if the wall piece is visible or not,
                            // the unit will walk to it
                            wcmd_executeMoveCommand(context, targetPoint);
                        }
                        else
                        {
                            if (wu_isEnemyUnit(context, targetEntity))
                            {
                                wcmd_executeAttackCommand(context, targetEntity, targetTile);
                            }
                            else
                            {
                                wcmd_executeFollowCommand(context, targetEntity);
                            }
                        }
                    }
                    else
                    {
                        wcmd_executeMoveCommand(context, targetPoint);
                    }
                }
                // if the right click was on the minimap
                else if (rect_containsf(map->ui.minimapPanel, input->pos.x, input->pos.y))
                {
                    vec2 offset = wmap_screenToMinimapCoordinatesV(context, input->pos);
                    vec2 targetPoint = wmap_tileToMapCoordinatesV(offset, true);

                    wcmd_executeMoveCommand(context, targetPoint);
                }
            }
        }
        else
        {
            wcmd_cancel(context, NULL);
        }
    }

    TracyCZoneEnd(ctx);
}

static void updateStatus(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateStatus", 1);

    WarMap* map = context->map;
    WarInput* input = &context->input;
    WarMapStatus* status = &map->status;
    WarCheatStatus* cheatStatus = &status->cheatStatus;
    WarFlashStatus* flashStatus = &status->flashStatus;

    memset(status->statusLineText, 0, sizeof(status->statusLineText));
    status->statusLineHighlightIndex = NO_HIGHLIGHT;
    status->statusLineHighlightCount = 0;
    status->statusLineGold = 0;
    status->statusLineWood = 0;

    if (cheatStatus->enabled)
    {
        if (cheatStatus->feedback)
        {
            if (context->realTime >= cheatStatus->feedbackEndRealTime)
            {
                cheatStatus->feedback = false;
            }
        }

        if (cheatStatus->visible)
        {
            if (isKeyJustReleased(input, WAR_KEY_ESC) || isKeyJustReleased(input, WAR_KEY_ENTER))
            {
                if (isKeyJustReleased(input, WAR_KEY_ENTER))
                {
                    wcheat_applyCheat(context, wsv_fromString(&cheatStatus->text));
                }

                wcheatp_setCheatsPanelVisible(context, false);
                TracyCZoneEnd(ctx);
                return;
            }

            if (isKeyJustReleased(input, WAR_KEY_TAB))
            {
                s32 length = (s32)cheatStatus->text.length;
                if (TAB_WIDTH <= STATUS_TEXT_MAX_LENGTH - length)
                {
                    wstr_insert(&cheatStatus->text, cheatStatus->position, wsv_fromCString("\t"));
                    cheatStatus->position++;
                }
            }
            else if (isKeyJustReleased(input, WAR_KEY_BACKSPACE))
            {
                if (cheatStatus->position > 0)
                {
                    wstr_removeRange(&cheatStatus->text, cheatStatus->position - 1, 1);
                    cheatStatus->position--;
                }
            }
            else if (isKeyJustReleased(input, WAR_KEY_DELETE))
            {
                s32 length = (s32)cheatStatus->text.length;
                if (cheatStatus->position < length)
                {
                    wstr_removeRange(&cheatStatus->text, cheatStatus->position, 1);
                }
            }
            else if (isKeyJustReleased(input, WAR_KEY_RIGHT))
            {
                s32 length = (s32)cheatStatus->text.length;
                if (cheatStatus->position < length)
                {
                    cheatStatus->position++;
                }
            }
            else if (isKeyJustReleased(input, WAR_KEY_LEFT))
            {
                if (cheatStatus->position > 0)
                {
                    cheatStatus->position--;
                }
            }
            else if (isKeyJustReleased(input, WAR_KEY_HOME))
            {
                cheatStatus->position = 0;
            }
            else if (isKeyJustReleased(input, WAR_KEY_END))
            {
                s32 length = (s32)cheatStatus->text.length;
                cheatStatus->position = length;
            }

            StringView statusText = wsv_fromCStringFormat(status->statusLineText, arrayLength(status->statusLineText), "MSG: %.*s", (s32)cheatStatus->text.length, cheatStatus->text.data);
            WarFontParams params = { .fontSize = 6.0f, .fontData = getFontData(0) };
            vec2 statusTextSize = wfont_measureSingleSpriteText(statusText, (s32)statusText.length, params);

            // Store cursor X offset (relative to bottomPanel.x + 2) for renderHUD.
            cheatStatus->cursorX = 2.0f + statusTextSize.x;

            TracyCZoneEnd(ctx);
            return;
        }

        cheatStatus->cursorX = -1.0f; // sentinel: cursor not visible

        if (isKeyJustReleased(input, WAR_KEY_ENTER))
        {
            wcheatp_setCheatsPanelVisible(context, true);
        }
    }

    if (flashStatus->enabled)
    {
        if (context->realTime <= flashStatus->endRealTime)
        {
            wsv_copyToBuffer(wstr_view(&flashStatus->text), status->statusLineText, arrayLength(status->statusLineText));
            status->statusLineHighlightIndex = NO_HIGHLIGHT;
            status->statusLineHighlightCount = 0;
            status->statusLineGold = 0;
            status->statusLineWood = 0;
            TracyCZoneEnd(ctx);
            return;
        }

        // if the time for the flash status is over, just disabled it
        flashStatus->enabled = false;
    }

    if (map->selectedEntities.count > 0)
    {
        for (s32 i = 0; i < map->selectedEntities.count; i++)
        {
            WarEntityId selectedEntityId = map->selectedEntities.items[i];
            WarEntity* selectedEntity = we_findEntity(context, selectedEntityId);
            assert(selectedEntity);

            if (wu_isBuildingUnit(context, selectedEntity))
            {
                if (wst_isTraining(context, selectedEntity) || wst_isGoingToTrain(context, selectedEntity))
                {
                    WarStateTrain* trainState = wst_getTrainState(context, selectedEntity);
                    WarUnitType unitToBuild = trainState->unitToBuild;
                    const WarUnitCommandMapping* commandMapping = wu_getCommandMappingFromUnitType(unitToBuild);
                    const WarUnitCommandBaseData* commandData = wu_getCommandBaseData(commandMapping->type);

                    wsv_copyToBuffer(commandData->tooltip2, status->statusLineText, arrayLength(status->statusLineText));
                }
                else if (wst_isUpgrading(context, selectedEntity) || wst_isGoingToUpgrade(context, selectedEntity))
                {
                    WarStateUpgrade* upgradeState = wst_getUpgradeState(context, selectedEntity);
                    WarUpgradeType upgradeToBuild = upgradeState->upgradeToBuild;
                    const WarUnitCommandMapping* commandMapping = wu_getCommandMappingFromUpgradeType(upgradeToBuild);
                    const WarUnitCommandBaseData* commandData = wu_getCommandBaseData(commandMapping->type);

                    wsv_copyToBuffer(commandData->tooltip2, status->statusLineText, arrayLength(status->statusLineText));
                }
                else
                {
                    WarUnitComponent* selUnit = we_getUnitComponent(context, selectedEntity);
                    assert(selUnit);

                    s32 hp = selUnit->hp;
                    s32 maxhp = selUnit->maxhp;
                    if (hp < maxhp)
                    {
                        // to calculate the amount of wood and gold needed to repair a
                        // building I'm taking the 12% of the damage of the building,
                        // so for the a FARM if it has a damage of 200, the amount of
                        // wood and gold would be 200 * 0.12 = 24.
                        //
                        s32 repairCost = (s32)ceil((maxhp - hp) * 0.12f);
                        wsv_fromCStringFormat(status->statusLineText, arrayLength(status->statusLineText), "FULL REPAIRS WILL COST %d GOLD & LUMBER", repairCost);
                    }
                }
            }
            else if (wu_isWorkerUnit(context, selectedEntity))
            {
                if (wu_isCarryingResources(context, selectedEntity))
                {
                    WarUnitComponent* selUnit = we_getUnitComponent(context, selectedEntity);
                    assert(selUnit);

                    if (selUnit->resourceKind == WAR_RESOURCE_GOLD)
                    {
                        wsv_copyToBuffer(wsv_fromCString("CARRYING GOLD"), status->statusLineText, arrayLength(status->statusLineText));
                    }
                    else if (selUnit->resourceKind == WAR_RESOURCE_WOOD)
                    {
                        wsv_copyToBuffer(wsv_fromCString("CARRYING LUMBER"), status->statusLineText, arrayLength(status->statusLineText));
                    }
                }
            }
        }
    }

    WarEntityList* buttons = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_BUTTON);
    for(s32 i = 0; i < buttons->count; i++)
    {
        WarEntity* entity = buttons->items[i];
        if (entity)
        {
            if (we_isComponentEnabled(context, entity, COMP_UI) && we_isComponentEnabled(context, entity, COMP_BUTTON))
            {
                WarButtonComponent* button = we_getButtonComponent(context, entity);
                assert(button);

                if (button->interactive && button->hot)
                {
                    memset(status->statusLineText, 0, sizeof(status->statusLineText));
                    wsv_copyToBuffer(wstr_view(&button->tooltip), status->statusLineText, arrayLength(status->statusLineText));
                    status->statusLineHighlightIndex = button->highlightIndex;
                    status->statusLineHighlightCount = button->highlightCount;
                    status->statusLineGold = button->gold;
                    status->statusLineWood = button->wood;
                    break;
                }
            }
        }
    }

    TracyCZoneEnd(ctx);
}

static void updateMapCursor(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateMapCursor", 1);

    WarMap* map = context->map;
    WarInput* input = &context->input;

    if (!map->playing)
    {
        wui_changeCursorType(context, WAR_CURSOR_ARROW);
        TracyCZoneEnd(ctx);
        return;
    }

    if (isMapDragging(input))
    {
        wui_changeCursorType(context, WAR_CURSOR_GREEN_CROSSHAIR);
        TracyCZoneEnd(ctx);
        return;
    }

    if (rect_containsf(map->ui.mapPanel, input->pos.x, input->pos.y))
    {
        WarUnitCommand* command = &map->commandState.command;
        switch (command->type)
        {
            case WAR_COMMAND_ATTACK:
            case WAR_COMMAND_SPELL_RAIN_OF_FIRE:
            case WAR_COMMAND_SPELL_POISON_CLOUD:
            {
                wui_changeCursorType(context, WAR_CURSOR_RED_CROSSHAIR);
                break;
            }

            case WAR_COMMAND_MOVE:
            case WAR_COMMAND_STOP:
            case WAR_COMMAND_HARVEST:
            case WAR_COMMAND_DELIVER:
            case WAR_COMMAND_REPAIR:
            case WAR_COMMAND_SPELL_HEALING:
            case WAR_COMMAND_SPELL_FAR_SIGHT:
            case WAR_COMMAND_SPELL_INVISIBILITY:
            case WAR_COMMAND_SPELL_RAISE_DEAD:
            case WAR_COMMAND_SPELL_DARK_VISION:
            case WAR_COMMAND_SPELL_UNHOLY_ARMOR:
            {
                wui_changeCursorType(context, WAR_CURSOR_YELLOW_CROSSHAIR);
                break;
            }

            case WAR_COMMAND_BUILD_FARM_HUMANS:
            case WAR_COMMAND_BUILD_FARM_ORCS:
            case WAR_COMMAND_BUILD_BARRACKS_HUMANS:
            case WAR_COMMAND_BUILD_BARRACKS_ORCS:
            case WAR_COMMAND_BUILD_CHURCH:
            case WAR_COMMAND_BUILD_TEMPLE:
            case WAR_COMMAND_BUILD_TOWER_HUMANS:
            case WAR_COMMAND_BUILD_TOWER_ORCS:
            case WAR_COMMAND_BUILD_TOWNHALL_HUMANS:
            case WAR_COMMAND_BUILD_TOWNHALL_ORCS:
            case WAR_COMMAND_BUILD_LUMBERMILL_HUMANS:
            case WAR_COMMAND_BUILD_LUMBERMILL_ORCS:
            case WAR_COMMAND_BUILD_STABLE:
            case WAR_COMMAND_BUILD_KENNEL:
            case WAR_COMMAND_BUILD_BLACKSMITH_HUMANS:
            case WAR_COMMAND_BUILD_BLACKSMITH_ORCS:
            case WAR_COMMAND_BUILD_ROAD:
            case WAR_COMMAND_BUILD_WALL:
            {
                wui_changeCursorType(context, WAR_CURSOR_ARROW);
                break;
            }

            default:
            {
                vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
                vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

                WarEntity* entityUnderCursor = we_findEntityUnderCursor(context, true, true);
                if (!entityUnderCursor)
                {
                    wui_changeCursorType(context, WAR_CURSOR_ARROW);
                    break;
                }

                WarEntityIdList* selectedEntities = &map->selectedEntities;
                if (selectedEntities->count > 0)
                {
                    WarEntity* selectedEntity = we_findEntity(context, selectedEntities->items[0]);
                    if (selectedEntity &&
                        wu_isFriendlyUnit(context, selectedEntity) &&
                        wu_isDudeUnit(context, selectedEntity))
                    {
                        if (wu_isUnitOfType(context, entityUnderCursor, WAR_UNIT_GOLDMINE) &&
                            !wmap_isUnitUnknown(context, map, entityUnderCursor) &&
                            wu_isWorkerUnit(context, selectedEntity))
                        {
                            wui_changeCursorType(context, WAR_CURSOR_YELLOW_CROSSHAIR);
                        }
                        else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_FOREST) &&
                                 !wmap_isTileUnkown(map, (s32)targetTile.x, (s32)targetTile.y) &&
                                 wu_isWorkerUnit(context, selectedEntity))
                        {
                            wui_changeCursorType(context, WAR_CURSOR_YELLOW_CROSSHAIR);
                        }
                        else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_WALL) &&
                                 !wmap_isTileUnkown(map, (s32)targetTile.x, (s32)targetTile.y) &&
                                 wu_isWarriorUnit(context, selectedEntity) &&
                                 wu_canAttack(context, selectedEntity, entityUnderCursor))
                        {
                            wui_changeCursorType(context, WAR_CURSOR_RED_CROSSHAIR);
                        }
                        else if (!wu_isFriendlyUnit(context, entityUnderCursor) &&
                                 wu_isWarriorUnit(context, selectedEntity) &&
                                 wu_canAttack(context, selectedEntity, entityUnderCursor))
                        {
                            wui_changeCursorType(context, WAR_CURSOR_RED_CROSSHAIR);
                        }
                        else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_FOREST) ||
                                 isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_WALL))
                        {
                            wui_changeCursorType(context, WAR_CURSOR_ARROW);
                        }
                        else
                        {
                            wui_changeCursorType(context, WAR_CURSOR_MAGNIFYING_GLASS);
                        }
                    }
                    else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_FOREST) ||
                             isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_WALL))
                    {
                        wui_changeCursorType(context, WAR_CURSOR_ARROW);
                    }
                    else
                    {
                        wui_changeCursorType(context, WAR_CURSOR_MAGNIFYING_GLASS);
                    }
                }
                else if (isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_FOREST) ||
                         isEntityOfType(entityUnderCursor, WAR_ENTITY_TYPE_WALL))
                {
                    wui_changeCursorType(context, WAR_CURSOR_ARROW);
                }
                else
                {
                    wui_changeCursorType(context, WAR_CURSOR_MAGNIFYING_GLASS);
                }

                break;
            }
        }
    }
    else if (rect_containsf(map->ui.minimapPanel, input->pos.x, input->pos.y))
    {
        WarUnitCommand* command = &map->commandState.command;
        switch (command->type)
        {
            case WAR_COMMAND_ATTACK:
            case WAR_COMMAND_SPELL_RAIN_OF_FIRE:
            case WAR_COMMAND_SPELL_POISON_CLOUD:
            {
                wui_changeCursorType(context, WAR_CURSOR_RED_CROSSHAIR);
                break;
            }

            case WAR_COMMAND_MOVE:
            case WAR_COMMAND_SPELL_FAR_SIGHT:
            case WAR_COMMAND_SPELL_DARK_VISION:
            {
                wui_changeCursorType(context, WAR_CURSOR_YELLOW_CROSSHAIR);
                break;
            }

            default:
            {
                wui_changeCursorType(context, WAR_CURSOR_ARROW);
                break;
            }
        }
    }
    else
    {
        vec2 dir = wmap_getDirFromMousePos(context);
        if (dir.x < 0 && dir.y < 0)         // -1, -1
            wui_changeCursorType(context, WAR_CURSOR_ARROW_UP_LEFT);
        else if (dir.x < 0 && dir.y > 0)    // -1,  1
            wui_changeCursorType(context, WAR_CURSOR_ARROW_BOTTOM_LEFT);
        else if (dir.x > 0 && dir.y < 0)    //  1, -1
            wui_changeCursorType(context, WAR_CURSOR_ARROW_UP_RIGHT);
        else if (dir.x > 0 && dir.y > 0)    //  1,  1
            wui_changeCursorType(context, WAR_CURSOR_ARROW_BOTTOM_RIGHT);
        else if (dir.x < 0)                 // -1,  0
            wui_changeCursorType(context, WAR_CURSOR_ARROW_LEFT);
        else if (dir.x > 0)                 //  1,  0
            wui_changeCursorType(context, WAR_CURSOR_ARROW_RIGHT);
        else if (dir.y < 0)                 //  0, -1
            wui_changeCursorType(context, WAR_CURSOR_ARROW_UP);
        else if (dir.y > 0)                 //  0,  1
            wui_changeCursorType(context, WAR_CURSOR_ARROW_BOTTOM);
        else                                //  0,  0
            wui_changeCursorType(context, WAR_CURSOR_ARROW);
    }

    TracyCZoneEnd(ctx);
}

static void updateActions(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateActions", 1);

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            wact_updateAction(context, entity);
        }
    }

    TracyCZoneEnd(ctx);
}

static void updateProjectiles(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateProjectiles", 1);

    WarEntityList* projectiles = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_PROJECTILE);
    for (s32 i = 0; i < projectiles->count; i++)
    {
        WarEntity* entity = projectiles->items[i];
        if (entity)
        {
            wproj_updateProjectile(context, entity);
        }
    }

    TracyCZoneEnd(ctx);
}

static void updateMagic(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateMagic", 1);

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity && wu_isMagicUnit(context, entity))
        {
            if (wst_isDead(context, entity) || wst_isGoingToDie(context, entity))
                continue;

            WarUnitComponent* unit = we_getUnitComponent(context, entity);
            assert(unit);

            if (unit->manaTime <= 0)
            {
                if (wu_isSummonUnit(context, entity))
                {
                    unit->mana = MAX(unit->mana - 1, 0);

                    // when the mana runs out the summoned units will die
                    if (unit->mana == 0)
                    {
                        vec2 position = wu_getUnitCenterPosition(context, entity, false);

                        WarStateDeath* deathState = wst_createDeathState(context, entity);
                        wst_changeNextState(context, entity, (WarStateBase*)deathState, true);

                        if (unit->type == WAR_UNIT_SCORPION || unit->type == WAR_UNIT_SPIDER)
                        {
                            wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_DEAD_SPIDER_SCORPION, .position=position, .hasPosition=true, .loop=false));
                        }
                    }
                }
                else
                {
                    // the magic units have a mana regeneration rate of roughly 1 point/sec
                    // so a magic unit will spend almost 4 minutes to fill its mana when its rans out
                    unit->mana = MIN(unit->mana + 1, unit->maxMana);
                }

                unit->manaTime = 1.0f;
            }
            else
            {
                unit->manaTime -= context->gameDeltaTime;
            }
        }
    }

    TracyCZoneEnd(ctx);
}

static bool updatePoisonCloud(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "UpdatePoisonCloud", 1);

    WarPoisonCloudComponent* poisonCloud = we_getPoisonCloudComponent(context, entity);
    assert(poisonCloud);

    poisonCloud->time -= context->gameDeltaTime;
    poisonCloud->damageTime -= context->gameDeltaTime;

    if (poisonCloud->damageTime <= 0)
    {
        WarEntityList* nearUnits = (WarEntityList*)wm_allocFrame(sizeof(WarEntityList));
        WarEntityListInit(nearUnits, wm_frameAllocator());
        we_getNearUnits(context, poisonCloud->position, 2, nearUnits);

        for (s32 i = 0; i < nearUnits->count; i++)
        {
            WarEntity* targetEntity = nearUnits->items[i];
            if (targetEntity &&
                !wst_isDead(context, targetEntity) && !wst_isGoingToDie(context, targetEntity) &&
                !wst_isCollapsing(context, targetEntity) && !wst_isGoingToCollapse(context, targetEntity))
            {
                we_takeDamage(context, targetEntity, 0, POISON_CLOUD_DAMAGE);
            }
        }

        WarEntityListFree(nearUnits);

        poisonCloud->damageTime = 1.0f;
    }

    TracyCZoneEnd(ctx);

    return poisonCloud->time <= 0;
}

static bool updateSight(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "UpdateSight", 1);

    WarSightComponent* sight = we_getSightComponent(context, entity);
    assert(sight);

    sight->time -= context->gameDeltaTime;

    TracyCZoneEnd(ctx);

    return sight->time <= 0;
}

static void updateSpells(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateSpells", 1);

    WarEntityList* poisonCloudSpells = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_POISON_CLOUD);
    WarEntityList* sightSpells = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_SIGHT);

    s32 spellsToRemoveCount = 0;
    WarEntityId* spellsToRemove = (WarEntityId*)wm_allocFrame(sizeof(WarEntityId) * (poisonCloudSpells->count + sightSpells->count));

    for (s32 i = 0; i < poisonCloudSpells->count; i++)
    {
        WarEntity* entity = poisonCloudSpells->items[i];
        assert(entity);

        if (updatePoisonCloud(context, entity))
        {
            spellsToRemove[spellsToRemoveCount++] = entity->id;
        }
    }

    for (s32 i = 0; i < sightSpells->count; i++)
    {
        WarEntity* entity = sightSpells->items[i];
        assert(entity);

        if (updateSight(context, entity))
        {
            spellsToRemove[spellsToRemoveCount++] = entity->id;
        }
    }

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        assert(entity);

        WarUnitComponent* unit = we_getUnitComponent(context, entity);
        assert(unit);

        if (unit->invisible)
        {
            unit->invisibilityTime -= context->gameDeltaTime;
            if (unit->invisibilityTime <= 0)
            {
                unit->invisible = false;
                unit->invisibilityTime = 0;
            }
        }

        if (unit->invulnerable)
        {
            unit->invulnerabilityTime -= context->gameDeltaTime;
            if (unit->invulnerabilityTime <= 0)
            {
                unit->invulnerable = false;
                unit->invulnerabilityTime = 0;
            }
        }
    }

    for (s32 i = 0; i < spellsToRemoveCount; i++)
    {
        we_removeEntityById(context, spellsToRemove[i]);
    }

    TracyCZoneEnd(ctx);
}

void updateFoW(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateFoW", 1);

    WarMap* map = context->map;

    for (s32 i = 0; i < MAP_TILES_WIDTH * MAP_TILES_HEIGHT; i++)
    {
        WarMapTile* tile = &map->tiles[i];

        tile->type = WAR_FOG_PIECE_NONE;
        tile->boundary = WAR_FOG_BOUNDARY_NONE;
        if (tile->state == MAP_TILE_STATE_VISIBLE)
            tile->state = MAP_TILE_STATE_FOG;
    }

    // the Holy Sight and Dark Vision spells are the first entities that change FoW
    WarEntityList* sightSpells = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_SIGHT);
    for (s32 i = 0; i < sightSpells->count; i++)
    {
        WarEntity* entity = sightSpells->items[i];
        if (entity)
        {
            WarSightComponent* sight = we_getSightComponent(context, entity);
            assert(sight);

            rect r = rect_expand(rectv(sight->position, VEC2_ONE), 3, 3);
            wmap_setMapTileState(map, (s32)r.x, (s32)r.y, (s32)r.width, (s32)r.height, MAP_TILE_STATE_VISIBLE);
        }
    }

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);

    // do the update of the FoW for friendly units first
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity && wu_isFriendlyUnit(context, entity))
        {
            WarUnitComponent* unit = we_getUnitComponent(context, entity);
            assert(unit);

            vec2 position = wu_getUnitCenterPosition(context, entity, true);
            vec2 unitSize = wu_getUnitSize(context, entity);
            s32 sight = wu_getUnitSightRange(context, entity);

            rect unitRect = rectv(position, unitSize);
            unitRect = rect_expand(unitRect, (f32)sight, (f32)sight);

            if (wu_isBuildingUnit(context, entity))
            {
                // the friendly buildings are always seen by the player
                unit->hasBeenSeen = true;
            }

            // mark the tiles of the unit as visible
            wmap_setMapTileState(map, (s32)unitRect.x, (s32)unitRect.y, (s32)unitRect.width, (s32)unitRect.height, MAP_TILE_STATE_VISIBLE);

            // reveal the attack target of the unit
            WarEntity* targetEntity = we_getAttackTarget(context, entity);
            if (targetEntity)
            {
                const WarUnitStats* stats = wu_getUnitStats(unit->type);

                if (wu_isUnit(targetEntity))
                {
                    if (wu_unitInRange(context, entity, targetEntity, stats->range))
                    {
                        wmap_setUnitMapTileState(context, map, targetEntity, MAP_TILE_STATE_VISIBLE);
                    }
                }
                else if (wu_isWall(targetEntity))
                {
                    WarStateAttack* attackState = wst_getAttackState(context, entity);
                    vec2 targetTile = attackState->targetTile;

                    if (wu_tileInRange(context, entity, targetTile, stats->range))
                    {
                        WarWallPiece* piece = we_getWallPieceAtPosition(context, targetEntity, (s32)targetTile.x, (s32)targetTile.y);
                        if (piece)
                        {
                            wmap_setMapTileState(map, (s32)targetTile.x, (s32)targetTile.y, 1, 1, MAP_TILE_STATE_VISIBLE);
                        }
                    }
                }
            }

            // reveal the attacker
            WarEntity* attacker = we_getAttacker(context, entity);
            if (attacker)
            {
                // if the attacker is the same the unit is attacking to
                // don't change tile state because already happened above
                if (!targetEntity || attacker->id != targetEntity->id)
                {
                    wmap_setUnitMapTileState(context, map, attacker, MAP_TILE_STATE_VISIBLE);
                }
            }
        }
    }

    // and then do the update of the FoW for enemies and neutrals units
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity && !wu_isFriendlyUnit(context, entity))
        {
            if (!wmap_isUnitPartiallyVisible(context, map, entity))
            {
                // remove from selection enemy or neutral units that goes into fog
                wmap_removeEntityFromSelection(context, entity->id);
            }
            else if (wu_isBuildingUnit(context, entity))
            {
                WarUnitComponent* unit = we_getUnitComponent(context, entity);
                assert(unit);

                unit->hasBeenSeen = true;
            }
        }
    }

    TracyCZoneEnd(ctx);
}

void determineFoWTypes(WarContext* context)
{
    TracyCZoneN(ctx, "DetermineFoWTypes", 1);

    WarMap* map = context->map;

    if (!map->fowEnabled)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    const s32 dirC = 8;
    const s32 dirX[] = { -1,  0,  1, 1, 1, 0, -1, -1 };
    const s32 dirY[] = { -1, -1, -1, 0, 1, 1,  1,  0 };

    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            WarMapTile* tile = wmap_getMapTileState(map, x, y);
            if (tile->state == MAP_TILE_STATE_VISIBLE)
            {
                s32 index = 0;
                s32 unkownCount = 0;
                s32 fogCount = 0;

                for (s32 d = 0; d < dirC; d++)
                {
                    s32 xx = x + dirX[d];
                    s32 yy = y + dirY[d];

                    if (inRange(xx, 0, MAP_TILES_WIDTH) &&
                        inRange(yy, 0, MAP_TILES_HEIGHT))
                    {
                        WarMapTile* neighborTile = wmap_getMapTileState(map, xx, yy);
                        if (neighborTile->state == MAP_TILE_STATE_VISIBLE)
                            index = index | (1 << d);
                        else if (neighborTile->state == MAP_TILE_STATE_FOG)
                            fogCount++;
                        else
                            unkownCount++;
                    }
                }

                if (index != 0xFF)
                {
                    tile->type = fogTileTypeMap[index];
                }

                if (fogCount > 0)
                    tile->boundary = WAR_FOG_BOUNDARY_FOG;
                else if (unkownCount > 0)
                    tile->boundary = WAR_FOG_BOUNDARY_UNKOWN;
            }
            else if (tile->state == MAP_TILE_STATE_FOG)
            {
                s32 index = 0;
                s32 unkownCount = 0;

                for (s32 d = 0; d < dirC; d++)
                {
                    s32 xx = x + dirX[d];
                    s32 yy = y + dirY[d];

                    if (inRange(xx, 0, MAP_TILES_WIDTH) &&
                        inRange(yy, 0, MAP_TILES_HEIGHT))
                    {
                        WarMapTile* neighborTile = wmap_getMapTileState(map, xx, yy);
                        if (neighborTile->state == MAP_TILE_STATE_VISIBLE ||
                            neighborTile->state == MAP_TILE_STATE_FOG)
                        {
                            index = index | (1 << d);
                        }
                        else
                        {
                            unkownCount++;
                        }
                    }
                }

                if (index != 0xFF)
                {
                    tile->type = fogTileTypeMap[index];
                }

                if (unkownCount > 0)
                {
                    tile->boundary = WAR_FOG_BOUNDARY_UNKOWN;
                }
            }
            else
            {
                s32 index = 0;

                for (s32 d = 0; d < dirC; d++)
                {
                    s32 xx = x + dirX[d];
                    s32 yy = y + dirY[d];

                    if (inRange(xx, 0, MAP_TILES_WIDTH) &&
                        inRange(yy, 0, MAP_TILES_HEIGHT))
                    {
                        WarMapTile* neighborTile = wmap_getMapTileState(map, xx, yy);
                        if (neighborTile->state == MAP_TILE_STATE_VISIBLE ||
                            neighborTile->state == MAP_TILE_STATE_FOG)
                        {
                            index = index | (1 << d);
                        }
                    }
                }

                if (index == 0xFF)
                {
                    tile->type = fogTileTypeMap[index];
                }
            }
        }
    }

    TracyCZoneEnd(ctx);
}

WarCampaignMapType wmap_getCampaignMapTypeByLevelInfoIndex(s32 levelInfoIndex)
{
    return levelInfoIndex >= WAR_CAMPAIGN_HUMANS_01 && levelInfoIndex <= WAR_CAMPAIGN_ORCS_12
        ? (WarCampaignMapType)levelInfoIndex
        : WAR_CAMPAIGN_CUSTOM;
}

WarLevelResult checkObjectives(WarContext* context)
{
    WarMap* map = context->map;

    map->objectivesTime -= context->realDeltaTime;

    if (map->objectivesTime <= 0)
    {
        WarCampaignMapData data = wcamp_getCampaignData(
            wmap_getCampaignMapTypeByLevelInfoIndex(map->levelInfoIndex)
        );

        if (data.checkObjectivesFunc)
        {
            return data.checkObjectivesFunc(context);
        }

        map->objectivesTime = 1;
    }

    return WAR_LEVEL_RESULT_NONE;
}

void updateObjectives(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateObjectives", 1);

    WarMap* map = context->map;

    if (map->result == WAR_LEVEL_RESULT_NONE)
    {
        map->result = checkObjectives(context);
        TracyCZoneEnd(ctx);
        return;
    }

    map->playing = false;

    bool isLastLevel = map->levelInfoIndex == WAR_CAMPAIGN_HUMANS_02 ||
                        map->levelInfoIndex == WAR_CAMPAIGN_ORCS_02;

    if (map->result == WAR_LEVEL_RESULT_WIN && isLastLevel)
    {
        wmm_showDemoEndMenu(context, true);
    }
    else
    {
        wmm_showOrHideGameOverMenu(context, true);
    }

    TracyCZoneEnd(ctx);
}

void wmap_updateMap(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateMap", 1);

    WarMap* map = context->map;

    if (!map->playing)
    {
        WarInput* input = &context->input;

        input->mapDragState.status = WAR_DRAG_NONE;
        input->mapDragState.startPos = VEC2_ZERO;
        input->mapDragState.rect = RECT_EMPTY;

        wui_updateUIButtons(context, !map->status.cheatStatus.visible);
        TracyCZoneEnd(ctx);
        return;
    }

    clearNearUnitsDebug(context);

    updateViewport(context);
    updateDragRect(context);

    if (!wcmd_executeCommand(context))
    {
        // only update the selection if the current command doesn't get
        // executed or there is no command at all.
        // the reason is because some commands are executed by the left click
        // as well as the selection, and if a command get executed the current
        // selection shouldn't be lost
        updateSelection(context);
    }

    updateSelectionGroups(context);

    updateDebugRenderShortcuts(context);

    wai_updateAIPlayers(context);

    wst_processStateMachineTransitions(context);
    wst_updateIdleStates(context);
    wst_updateMoveStates(context);
    wst_updatePatrolStates(context);
    wst_updateFollowStates(context);
    wst_updateAttackStates(context);
    wst_updateGoldStates(context);
    wst_updateMiningStates(context);
    wst_updateWoodStates(context);
    wst_updateChoppingStates(context);
    wst_updateDeliverStates(context);
    wst_updateDeathStates(context);
    wst_updateCollapseStates(context);
    wst_updateTrainStates(context);
    wst_updateUpgradeStates(context);
    wst_updateBuildStates(context);
    wst_updateRepairStates(context);
    wst_updateRepairingStates(context);
    wst_updateCastStates(context);
    wst_updateWaitStates(context);
    updateActions(context);
    wanim_updateAnimations(context);
    updateProjectiles(context);
    updateMagic(context);
    updateSpells(context);

    updateFoW(context);
    determineFoWTypes(context);

    updateCommandButtons(context);

    updateCommandFromRightClick(context);
    updateStatus(context);

    updateTreesEdit(context);
    updateRoadsEdit(context);
    updateWallsEdit(context);
    updateRuinsEdit(context);
    updateRainOfFireEdit(context);
    updatePoisonCloudEdit(context);
    updateRaiseDeadEdit(context);
    updateAddUnit(context);

    refreshSelectedUnitNearUnitsDebug(context);

    updateFlowFieldDebug(context);

    updateObjectives(context);

    TracyCZoneEnd(ctx);
}

// Called each tick when context->paused is true.
// Handles only viewport scrolling, debug shortcuts, and the cheat panel so the
// player can review the map and enter cheats while the game simulation is frozen.
void wmap_updateMapPaused(WarContext* context)
{
    WarMap* map = context->map;

    if (!map->playing)
        return;

    updateViewport(context);
    updateDebugRenderShortcuts(context);
    updateStatus(context);
}

static void renderTerrain(WarContext* context)
{
    TracyCZoneN(ctx, "RenderTerrain", 1);

    WarMap *map = context->map;

    WarResource* levelInfo = wres_getOrCreateResource(context, map->levelInfoIndex);
    assert(levelInfo && levelInfo->type == WAR_RESOURCE_TYPE_LEVEL_INFO);

    WarResource* levelVisual = wres_getOrCreateResource(context, levelInfo->levelInfo.visualIndex);
    assert(levelVisual && levelVisual->type == WAR_RESOURCE_TYPE_LEVEL_VISUAL);

    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            WarMapTile* tile = wmap_getMapTileState(map, x, y);
            if (!map->fowEnabled ||
                tile->state == MAP_TILE_STATE_VISIBLE ||
                tile->state == MAP_TILE_STATE_FOG)
            {
                // index of the tile in the tilesheet
                u16 tileIndex = levelVisual->levelVisual.data[y * MAP_TILES_WIDTH + x];

                // coordinates in pixels of the terrain tile
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

                wr_save(context);
                wr_translate(context, (f32)(x * MEGA_TILE_WIDTH), (f32)(y * MEGA_TILE_HEIGHT));

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                wr_subImage(context, map->sprite.texture, rs, rd, VEC2_ONE);

                wr_restore(context);
            }
        }
    }

    TracyCZoneEnd(ctx);
}

static void renderFoW(WarContext* context)
{
    TracyCZoneN(ctx, "RenderFoW", 1);

    WarMap* map = context->map;

    if (!map->fowEnabled)
    {
        TracyCZoneEnd(ctx);
        return;
    }

    // Pass 1: render unknown boundary and unknown tiles at full opacity
    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            WarMapTile* tile = wmap_getMapTileState(map, x, y);

            if (tile->type != WAR_FOG_PIECE_NONE && tile->boundary == WAR_FOG_BOUNDARY_UNKOWN)
            {
                s32 tileIndex = (s32)tile->type;
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = (tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT;

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                wr_subImage(context, map->sprite.texture, rs, rd, VEC2_ONE);
            }

            if (tile->state == MAP_TILE_STATE_UNKOWN)
            {
                rect rs = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                wr_subImage(context, map->blackSprite.texture, rs, rd, VEC2_ONE);
            }
        }
    }

    // Pass 2: render fog boundary and fog tiles at half opacity
    wr_save(context);
    wr_globalAlpha(context, 0.5f);

    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            WarMapTile* tile = wmap_getMapTileState(map, x, y);

            if (tile->type != WAR_FOG_PIECE_NONE &&
                tile->state == MAP_TILE_STATE_VISIBLE &&
                tile->boundary == WAR_FOG_BOUNDARY_FOG)
            {
                s32 tileIndex = (s32)tile->type;
                s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
                s32 tilePixelY = (tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT;

                rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                wr_subImage(context, map->sprite.texture, rs, rd, VEC2_ONE);
            }

            if (tile->state == MAP_TILE_STATE_FOG)
            {
                rect rs = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                rect rd = recti(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                wr_subImage(context, map->blackSprite.texture, rs, rd, VEC2_ONE);
            }
        }
    }

    wr_restore(context);

    TracyCZoneEnd(ctx);
}

static void renderPassableInfo(WarContext* context)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_PASSABLE_INFO])
        return;

    WarMap *map = context->map;

    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            if (isStatic(&map->finder, x, y))
            {
                vec2 pos = vec2i(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);
                vec2 size = vec2i(MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                wr_fillRect(context, rectv(pos, size), WAR_COLOR_RGBA(255, 0, 0, 100));
            }
            else if(isDynamic(&map->finder, x, y))
            {
                vec2 pos = vec2i(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);
                vec2 size = vec2i(MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
                wr_fillRect(context, rectv(pos, size), WAR_COLOR_RGBA(255, 150, 100, 100));
            }
        }
    }
}

static void renderMapGrid(WarContext* context)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_MAP_GRID])
        return;

    for(s32 x = 1; x < MAP_TILES_WIDTH; x++)
    {
        vec2 p1 = vec2i(x * MEGA_TILE_WIDTH, 0);
        vec2 p2 = vec2i(x * MEGA_TILE_WIDTH, MAP_TILES_HEIGHT * MEGA_TILE_HEIGHT);
        wr_strokeLine(context, p1, p2, WAR_COLOR_WHITE);
    }

    for(s32 y = 1; y < MAP_TILES_HEIGHT; y++)
    {
        vec2 p1 = vec2i(0, y * MEGA_TILE_HEIGHT);
        vec2 p2 = vec2i(MAP_TILES_WIDTH * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT);
        wr_strokeLine(context, p1, p2, WAR_COLOR_WHITE);
    }
}

static void renderSpatialGrid(WarContext* context)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_SPATIAL_GRID])
        return;

    for (s32 x = 1; x < MAP_GRID_TILES_WIDTH; x++)
    {
        vec2 p1 = wmap_tileToMapCoordinatesV(vec2i(x * MAP_GRID_TILE_SIZE, 0), false);
        vec2 p2 = wmap_tileToMapCoordinatesV(vec2i(x * MAP_GRID_TILE_SIZE, MAP_TILES_HEIGHT), false);
        wr_strokeLine(context, p1, p2, WAR_COLOR_RGB(0, 255, 255));
    }

    for (s32 y = 1; y < MAP_GRID_TILES_HEIGHT; y++)
    {
        vec2 p1 = wmap_tileToMapCoordinatesV(vec2i(0, y * MAP_GRID_TILE_SIZE), false);
        vec2 p2 = wmap_tileToMapCoordinatesV(vec2i(MAP_TILES_WIDTH, y * MAP_GRID_TILE_SIZE), false);
        wr_strokeLine(context, p1, p2, WAR_COLOR_RGB(0, 255, 255));
    }
}

static void renderNearUnitsDebug(WarContext* context)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_NEAR_UNITS])
        return;

    TracyCZoneN(ctx, "RenderNearUnitsDebug", 1);

    WarMap* map = context->map;
    if (!map->debug.nearUnitsEnabled)
        return;

    vec2 targetTile = map->debug.nearUnitsTargetTile;
    s32 distance    = map->debug.nearUnitsDistance;

    // Draw the query bounding box (Chebyshev radius matches vec2_distanceInTiles semantics).
    vec2 center = wmap_tileToMapCoordinatesV(targetTile, true);
    vec2 size   = vec2i((distance * 2 + 1) * MEGA_TILE_WIDTH,
                        (distance * 2 + 1) * MEGA_TILE_HEIGHT);
    rect area   = rectf(center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x, size.y);
    wr_strokeRect(context, area, WAR_COLOR_YELLOW);

    if (context->debugRender.flags[WAR_DEBUG_RENDER_SPATIAL_GRID])
    {
        // Highlight exactly the grid cells that we_getNearUnits2 visits.
        // Uses the same floorf-based bounds as the query so the highlighted
        // cells match the yellow bounding box as tightly as possible.
        s32 gxMin = MAX(0,                     (s32)floorf((targetTile.x - (f32)distance) / MAP_GRID_TILE_SIZE));
        s32 gxMax = MIN(MAP_GRID_TILES_WIDTH  - 1, (s32)floorf((targetTile.x + (f32)distance) / MAP_GRID_TILE_SIZE));
        s32 gyMin = MAX(0,                     (s32)floorf((targetTile.y - (f32)distance) / MAP_GRID_TILE_SIZE));
        s32 gyMax = MIN(MAP_GRID_TILES_HEIGHT - 1, (s32)floorf((targetTile.y + (f32)distance) / MAP_GRID_TILE_SIZE));

        for (s32 gy = gyMin; gy <= gyMax; gy++)
        {
            for (s32 gx = gxMin; gx <= gxMax; gx++)
            {
                vec2 pos      = wmap_tileToMapCoordinatesV(vec2i(gx * MAP_GRID_TILE_SIZE, gy * MAP_GRID_TILE_SIZE), false);
                vec2 cellSize = vec2i(MAP_GRID_TILE_SIZE * MEGA_TILE_WIDTH, MAP_GRID_TILE_SIZE * MEGA_TILE_HEIGHT);
                rect cellRect = rectv(pos, cellSize);
                wr_fillRect(context, cellRect, WAR_COLOR_RGBA(0, 128, 255, 48));
                wr_strokeRect(context, cellRect, WAR_COLOR_RGB(0, 255, 255));
            }
        }
    }

    // Replay we_getNearUnits2 to get current results — no stored result arrays needed.
    WarEntityList nearUnits;
    WarEntityListInit(&nearUnits, wm_frameAllocator());
    we_getNearUnits(context, targetTile, distance, &nearUnits);

    for (s32 i = 0; i < nearUnits.count; i++)
    {
        WarEntity* entity = nearUnits.items[i];
        if (!entity || !wu_isUnit(entity))
            continue;

        rect unitRect = wu_getUnitRect(context, entity);
        wr_fillRect(context, unitRect, WAR_COLOR_RGBA(255, 255, 0, 48));
        wr_strokeRect(context, unitRect, WAR_COLOR_YELLOW);
    }

    WarEntityListFree(&nearUnits);

    TracyCZoneEnd(ctx);
}

static void renderFlowField(WarContext* context)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_FLOW_FIELD])
        return;

    WarMap* map = context->map;

    if (!map->debug.flowField)
        return;

    s32 startX = (s32)(map->camera.viewport.x / MEGA_TILE_WIDTH);
    s32 startY = (s32)(map->camera.viewport.y / MEGA_TILE_HEIGHT);
    s32 endX = (s32)((map->camera.viewport.x + map->camera.viewport.width) / MEGA_TILE_WIDTH) + 1;
    s32 endY = (s32)((map->camera.viewport.y + map->camera.viewport.height) / MEGA_TILE_HEIGHT) + 1;

    startX = MAX(startX, 0);
    startY = MAX(startY, 0);
    endX = MIN(endX, MAP_TILES_WIDTH);
    endY = MIN(endY, MAP_TILES_HEIGHT);

    static const s32 dirX[] = {  0,  1, 1, 1, 0, -1, -1, -1 };
    static const s32 dirY[] = { -1, -1, 0, 1, 1,  1,  0, -1 };

    WarColor arrowColor = WAR_COLOR_RGB(0, 255, 255);
    f32 shaftLen = MEGA_TILE_WIDTH * 0.35f;
    f32 headLen = 3.0f;

    for (s32 y = startY; y < endY; y++)
    {
        for (s32 x = startX; x < endX; x++)
        {
            if (x == map->debug.flowFieldX && y == map->debug.flowFieldY)
                continue;

            if (isStatic(&map->finder, x, y))
                continue;

            WarDirection dir = (WarDirection)map->debug.flowField->dirs[y * MAP_TILES_WIDTH + x];
            if (dir < WAR_DIRECTION_NORTH || dir >= WAR_DIRECTION_COUNT)
                continue;

            vec2 center = wmap_tileToMapCoordinatesV(vec2i(x, y), true);

            s32 dx = dirX[dir];
            s32 dy = dirY[dir];

            vec2 end = vec2f(center.x + dx * shaftLen, center.y + dy * shaftLen);

            wr_strokeLine(context, center, end, arrowColor);

            f32 angle = atan2f((f32)dy, (f32)dx);
            vec2 head1 = vec2f(
                end.x - headLen * cosf(angle - PI / 6.0f),
                end.y - headLen * sinf(angle - PI / 6.0f));
            vec2 head2 = vec2f(
                end.x - headLen * cosf(angle + PI / 6.0f),
                end.y - headLen * sinf(angle + PI / 6.0f));

            wr_strokeLine(context, end, head1, arrowColor);
            wr_strokeLine(context, end, head2, arrowColor);
        }
    }
}

static void renderRvoDebug(WarContext* context)
{
    if (!context->debugRender.flags[WAR_DEBUG_RENDER_RVO])
        return;

    TracyCZoneN(ctx, "RenderRvoDebug", 1);

#define CANDIDATE_DRAW_SCALE 1.0f
#define ARROW_HEAD_LEN 3.0f
#define DOT_RADIUS_PX 1.5f
#define BEST_DOT_RADIUS_PX 3.0f

    WarColor radiusColor   = WAR_COLOR_RGBA(255, 255, 255, 64);
    WarColor rvoVelColor   = WAR_COLOR_RGB(0, 255, 0);
    WarColor prefVelColor  = WAR_COLOR_RGB(255, 255, 0);
    WarColor cleanColor    = WAR_COLOR_RGB(255, 255, 255);
    WarColor penaltyColor  = WAR_COLOR_RGB(255, 0, 0);
    WarColor bestColor     = WAR_COLOR_RGB(0, 255, 0);

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (!entity || entity->id < 0) continue;

        WarStateMove* moveState = wst_getMoveState(context, entity);
        if (!moveState) continue;
        if (moveState->rvoNumCandidates <= 0) continue;

        vec2 pos    = moveState->rvoPosition;
        f32  radius = moveState->rvoRadius;

        // rect radiusRect = rectf(
        //     pos.x - radius, pos.y - radius,
        //     radius * 2.0f,  radius * 2.0f
        // );
        // wr_strokeRect(context, radiusRect, radiusColor);

        if (vec2_lengthSqr(moveState->rvoVelocity) > 0.01f)
        {
            vec2 rvoEnd = vec2f(
                pos.x + moveState->rvoVelocity.x * CANDIDATE_DRAW_SCALE,
                pos.y + moveState->rvoVelocity.y * CANDIDATE_DRAW_SCALE
            );
            wr_strokeLine(context, pos, rvoEnd, rvoVelColor);
            f32 angle = atan2f(moveState->rvoVelocity.y, moveState->rvoVelocity.x);
            vec2 h1 = vec2f(rvoEnd.x - ARROW_HEAD_LEN * cosf(angle - PI / 6.0f),
                            rvoEnd.y - ARROW_HEAD_LEN * sinf(angle - PI / 6.0f));
            vec2 h2 = vec2f(rvoEnd.x - ARROW_HEAD_LEN * cosf(angle + PI / 6.0f),
                            rvoEnd.y - ARROW_HEAD_LEN * sinf(angle + PI / 6.0f));
            wr_strokeLine(context, rvoEnd, h1, rvoVelColor);
            wr_strokeLine(context, rvoEnd, h2, rvoVelColor);
        }

        if (vec2_lengthSqr(moveState->rvoPreferredVelocity) > 0.01f)
        {
            vec2 prefEnd = vec2f(
                pos.x + moveState->rvoPreferredVelocity.x * CANDIDATE_DRAW_SCALE,
                pos.y + moveState->rvoPreferredVelocity.y * CANDIDATE_DRAW_SCALE
            );
            wr_strokeLine(context, pos, prefEnd, prefVelColor);
            f32 angle = atan2f(moveState->rvoPreferredVelocity.y, moveState->rvoPreferredVelocity.x);
            vec2 h1 = vec2f(prefEnd.x - ARROW_HEAD_LEN * cosf(angle - PI / 6.0f),
                            prefEnd.y - ARROW_HEAD_LEN * sinf(angle - PI / 6.0f));
            vec2 h2 = vec2f(prefEnd.x - ARROW_HEAD_LEN * cosf(angle + PI / 6.0f),
                            prefEnd.y - ARROW_HEAD_LEN * sinf(angle + PI / 6.0f));
            wr_strokeLine(context, prefEnd, h1, prefVelColor);
            wr_strokeLine(context, prefEnd, h2, prefVelColor);
        }

        // s32 n = moveState->rvoNumCandidates;
        // for (s32 c = 0; c < n; c++)
        // {
        //     vec2 cv = moveState->rvoCandidates[c];
        //     vec2 dot = vec2f(pos.x + cv.x * CANDIDATE_DRAW_SCALE,
        //                      pos.y + cv.y * CANDIDATE_DRAW_SCALE);
        //     rect r = rectf(dot.x - DOT_RADIUS_PX, dot.y - DOT_RADIUS_PX,
        //                    DOT_RADIUS_PX * 2.0f, DOT_RADIUS_PX * 2.0f);
        //     WarColor col = moveState->rvoCandidateHadCollision[c] ? penaltyColor : cleanColor;
        //     wr_fillRect(context, r, col);
        // }

        // if (moveState->rvoBestIndex >= 0 &&
        //     moveState->rvoBestIndex < n)
        // {
        //     vec2 cv = moveState->rvoCandidates[moveState->rvoBestIndex];
        //     vec2 dot = vec2f(pos.x + cv.x * CANDIDATE_DRAW_SCALE,
        //                      pos.y + cv.y * CANDIDATE_DRAW_SCALE);
        //     rect r = rectf(dot.x - BEST_DOT_RADIUS_PX, dot.y - BEST_DOT_RADIUS_PX,
        //                    BEST_DOT_RADIUS_PX * 2.0f, BEST_DOT_RADIUS_PX * 2.0f);
        //     wr_fillRect(context, r, bestColor);
        // }
    }

    TracyCZoneEnd(ctx);
}

static void renderMapPanel(WarContext *context)
{
    TracyCZoneN(ctx, "RenderMapPanel", 1);

    WarMap *map = context->map;

    wr_save(context);

    wr_translate(context, map->ui.mapPanel.x, map->ui.mapPanel.y);
    wr_translate(context, -map->camera.viewport.x, -map->camera.viewport.y);

    renderTerrain(context);
    we_renderEntitiesOfType(context, WAR_ENTITY_TYPE_RUIN);
    we_renderEntitiesOfType(context, WAR_ENTITY_TYPE_ROAD);
    we_renderEntitiesOfType(context, WAR_ENTITY_TYPE_WALL);
    we_renderEntitiesOfType(context, WAR_ENTITY_TYPE_FOREST);

    renderPassableInfo(context);
    renderMapGrid(context);
    renderSpatialGrid(context);
    renderFlowField(context);
    renderNearUnitsDebug(context);

    we_renderEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    we_renderUnitSelection(context);
    we_renderEntitiesOfType(context, WAR_ENTITY_TYPE_PROJECTILE);
    we_renderEntitiesOfType(context, WAR_ENTITY_TYPE_POISON_CLOUD);
    we_renderEntitiesOfType(context, WAR_ENTITY_TYPE_ANIMATION);

    renderRvoDebug(context);

    renderFoW(context);

    wr_restore(context);

    TracyCZoneEnd(ctx);
}

void wmap_renderMap(WarContext *context)
{
    TracyCZoneN(ctx, "RenderMap", 1);

    updateMapCursor(context);

    renderMapPanel(context);
    wmui_renderMapUI(context);

    TracyCZoneEnd(ctx);
}

