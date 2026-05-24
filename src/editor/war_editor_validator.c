#include "war_editor_validator.h"

#include "war_editor_map.h"

static WarValidationResult s_results[WE_VALIDATION_MAX_RESULTS];
static s32 s_resultCount = 0;

static void wevalid_addResult(WarValidationResult* results,
                              s32* count,
                              WarValidationSeverity severity,
                              const char* message,
                              bool hasTile,
                              s32 tx,
                              s32 ty)
{
    if (!results || !count || *count >= WE_VALIDATION_MAX_RESULTS)
    {
        return;
    }

    WarValidationResult* r = &results[*count];
    r->severity = severity;
    r->hasTile = hasTile;
    r->tx = tx;
    r->ty = ty;
    SDL_strlcpy(r->message, message, sizeof(r->message));
    (*count)++;
}

static bool wevalid_isTownHall(WarUnitType t)
{
    return t == WAR_UNIT_TOWNHALL_HUMANS || t == WAR_UNIT_TOWNHALL_ORCS;
}

static u32 wevalid_countGoldmines(WarEditorMap* map)
{
    u32 count = map ? map->startGoldminesCount : 0u;

    if (!map)
    {
        return 0u;
    }

    for (u32 i = 0; i < map->startEntitiesCount; i++)
    {
        if (map->startEntities[i].type == WAR_UNIT_GOLDMINE)
        {
            count++;
        }
    }

    return count;
}

void wevalid_validate(WarEditorMap* map, WarValidationResult* results, s32* count)
{
    if (!count)
    {
        return;
    }

    *count = 0;
    if (!map || !results)
    {
        return;
    }

    for (u32 i = 0; i < map->startEntitiesCount; i++)
    {
        WarLevelUnit* lu = &map->startEntities[i];
        if (lu->x >= MAP_TILES_WIDTH || lu->y >= MAP_TILES_HEIGHT)
        {
            char msg[160];
            SDL_snprintf(msg, sizeof(msg), "Entity %u is out of bounds at (%d, %d).",
                         i, (s32)lu->x, (s32)lu->y);
            wevalid_addResult(results, count, WE_VALID_ERROR, msg, true, (s32)lu->x, (s32)lu->y);
        }
    }

    for (u32 i = 0; i < map->startGoldminesCount; i++)
    {
        WarLevelUnit* lu = &map->startGoldmines[i];
        if (lu->x >= MAP_TILES_WIDTH || lu->y >= MAP_TILES_HEIGHT)
        {
            char msg[160];
            SDL_snprintf(msg, sizeof(msg), "Goldmine %u is out of bounds at (%d, %d).",
                         i, (s32)lu->x, (s32)lu->y);
            wevalid_addResult(results, count, WE_VALID_ERROR, msg, true, (s32)lu->x, (s32)lu->y);
        }
    }

    for (u32 i = 0; i < map->startEntitiesCount; i++)
    {
        for (u32 j = i + 1; j < map->startEntitiesCount; j++)
        {
            if (map->startEntities[i].x == map->startEntities[j].x &&
                map->startEntities[i].y == map->startEntities[j].y)
            {
                char msg[160];
                SDL_snprintf(msg, sizeof(msg),
                             "Entities %u and %u overlap at (%d, %d).",
                             i, j,
                             (s32)map->startEntities[i].x,
                             (s32)map->startEntities[i].y);
                wevalid_addResult(results, count, WE_VALID_ERROR, msg, true,
                                  (s32)map->startEntities[i].x,
                                  (s32)map->startEntities[i].y);
            }
        }
    }

    bool hasPlayer0Hall = false;
    for (u32 i = 0; i < map->startEntitiesCount; i++)
    {
        WarLevelUnit* lu = &map->startEntities[i];
        if (lu->player == 0 && wevalid_isTownHall(lu->type))
        {
            hasPlayer0Hall = true;
            break;
        }
    }
    if (!hasPlayer0Hall)
    {
        wevalid_addResult(results, count, WE_VALID_WARNING,
                          "Player 0 has no Town Hall / Great Hall.",
                          false, 0, 0);
    }

    if (map->startX >= MAP_TILES_WIDTH || map->startY >= MAP_TILES_HEIGHT)
    {
        wevalid_addResult(results, count, WE_VALID_WARNING,
                          "Map start location exceeds map bounds.",
                          true, (s32)map->startX, (s32)map->startY);
    }

    if (wevalid_countGoldmines(map) == 0)
    {
        wevalid_addResult(results, count, WE_VALID_WARNING,
                          "No goldmines are placed.",
                          false, 0, 0);
    }

    if (map->startEntitiesCount > 800)
    {
        wevalid_addResult(results, count, WE_VALID_INFO,
                          "Entity count is above 800 (near MAX_ENTITIES_COUNT).",
                          false, 0, 0);
    }
}

void wevalid_run(WarEditorContext* ctx)
{
    if (!ctx || !ctx->map)
    {
        s_resultCount = 0;
        return;
    }

    wevalid_validate(ctx->map, s_results, &s_resultCount);
    SDL_snprintf(ctx->statusText, sizeof(ctx->statusText),
                 "Validation complete: %d issues", s_resultCount);
}

const WarValidationResult* wevalid_getResults(s32* outCount)
{
    if (outCount)
    {
        *outCount = s_resultCount;
    }
    return s_results;
}

static void wevalid_focusTile(WarEditorContext* ctx, s32 tx, s32 ty)
{
    f32 viewMapW = (f32)ctx->canvasPanelW / ctx->cameraZoom;
    f32 viewMapH = (f32)ctx->canvasPanelH / ctx->cameraZoom;

    ctx->cameraOffset.x = (f32)(tx * MEGA_TILE_WIDTH) - viewMapW * 0.5f;
    ctx->cameraOffset.y = (f32)(ty * MEGA_TILE_HEIGHT) - viewMapH * 0.5f;

    f32 maxX = (f32)(MAP_TILES_WIDTH * MEGA_TILE_WIDTH) - viewMapW;
    f32 maxY = (f32)(MAP_TILES_HEIGHT * MEGA_TILE_HEIGHT) - viewMapH;
    if (maxX < 0.0f) maxX = 0.0f;
    if (maxY < 0.0f) maxY = 0.0f;

    if (ctx->cameraOffset.x < 0.0f) ctx->cameraOffset.x = 0.0f;
    if (ctx->cameraOffset.y < 0.0f) ctx->cameraOffset.y = 0.0f;
    if (ctx->cameraOffset.x > maxX) ctx->cameraOffset.x = maxX;
    if (ctx->cameraOffset.y > maxY) ctx->cameraOffset.y = maxY;
}

void wevalid_drawPanel(WarEditorContext* ctx)
{
    if (!igBegin("Validation##validation", NULL, ImGuiWindowFlags_None))
    {
        igEnd();
        return;
    }

    if (s_resultCount == 0)
    {
        igTextDisabled("No validation results. Run Map > Validate Map.");
        igEnd();
        return;
    }

    for (s32 i = 0; i < s_resultCount; i++)
    {
        const WarValidationResult* r = &s_results[i];
        const char* sev = "INFO";
        ImVec4_c color = (ImVec4_c){ 0.45f, 0.65f, 1.0f, 1.0f };
        if (r->severity == WE_VALID_ERROR)
        {
            sev = "ERROR";
            color = (ImVec4_c){ 1.0f, 0.35f, 0.35f, 1.0f };
        }
        else if (r->severity == WE_VALID_WARNING)
        {
            sev = "WARN";
            color = (ImVec4_c){ 1.0f, 0.8f, 0.3f, 1.0f };
        }

        igPushStyleColor_Vec4(ImGuiCol_Text, color);
        igText("[%s]", sev);
        igPopStyleColor(1);
        igSameLine(0.0f, 6.0f);

        char label[220];
        SDL_snprintf(label, sizeof(label), "%03d %s", i + 1, r->message);
        if (igSelectable_Bool(label, false, 0, (ImVec2_c){ 0.0f, 0.0f }))
        {
            if (ctx && r->hasTile)
            {
                wevalid_focusTile(ctx, r->tx, r->ty);
            }
        }
    }

    igEnd();
}
