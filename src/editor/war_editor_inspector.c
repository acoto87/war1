#include "war_editor_inspector.h"
#include "war_editor_history.h"

static bool weinspect_rectsOverlap(s32 ax, s32 ay, s32 aw, s32 ah,
                                   s32 bx, s32 by, s32 bw, s32 bh)
{
    return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}

static bool weinspect_canEntityOccupy(WarEditorContext* ctx,
                                      s32 index,
                                      bool isGoldmine,
                                      s32 x,
                                      s32 y)
{
    WarEditorMap* m = ctx->map;
    if (!m)
    {
        return false;
    }

    WarLevelUnit* lu = NULL;
    if (isGoldmine)
    {
        if (index < 0 || (u32)index >= m->startGoldminesCount)
        {
            return false;
        }
        lu = &m->startGoldmines[index];
    }
    else
    {
        if (index < 0 || (u32)index >= m->startEntitiesCount)
        {
            return false;
        }
        lu = &m->startEntities[index];
    }

    const WarUnitData* ud = wu_getUnitData(lu->type);
    if (!ud)
    {
        return false;
    }

    if (x < 0 || y < 0 ||
        x + ud->sizex > MAP_TILES_WIDTH ||
        y + ud->sizey > MAP_TILES_HEIGHT)
    {
        return false;
    }

    for (s32 fy = y; fy < y + ud->sizey; fy++)
    {
        for (s32 fx = x; fx < x + ud->sizex; fx++)
        {
            if (m->passableData[fy * MAP_TILES_WIDTH + fx] != 0)
            {
                return false;
            }
        }
    }

    for (u32 i = 0; i < m->startEntitiesCount; i++)
    {
        if (!isGoldmine && (s32)i == index)
        {
            continue;
        }

        WarLevelUnit* other = &m->startEntities[i];
        const WarUnitData* oud = wu_getUnitData(other->type);
        if (!oud)
        {
            continue;
        }

        if (weinspect_rectsOverlap(x, y, ud->sizex, ud->sizey,
                                   (s32)other->x, (s32)other->y,
                                   oud->sizex, oud->sizey))
        {
            return false;
        }
    }

    for (u32 i = 0; i < m->startGoldminesCount; i++)
    {
        if (isGoldmine && (s32)i == index)
        {
            continue;
        }

        WarLevelUnit* other = &m->startGoldmines[i];
        const WarUnitData* oud = wu_getUnitData(other->type);
        if (!oud)
        {
            continue;
        }

        if (weinspect_rectsOverlap(x, y, ud->sizex, ud->sizey,
                                   (s32)other->x, (s32)other->y,
                                   oud->sizex, oud->sizey))
        {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------
// — Inspector Panel
// ---------------------------------------------------------------------------

// Helper: resource kind names for the entity inspector combo.
static const char* s_resourceKindNames = "None\0Gold\0Wood\0";

// Helper: race names for the per-player combo.
static const char* s_raceNames = "Neutral\0Humans\0Orcs\0";

// Width of the label column in the two-column inspector layout.
#define WEINSPECT_LABEL_W 90.0f

// Begin a two-column inspector row: left column shows a text label aligned
// with the widget frame, right column hosts the widget.
// Caller must immediately call an input widget after this.
static void weinspect_beginRow(const char* label)
{
    igAlignTextToFramePadding();
    igText("%s", label);
    igSameLine(WEINSPECT_LABEL_W, 0.0f);
    igSetNextItemWidth(-1.0f);
}

static void weinspect_pushEntityEdit(WarEditorContext* ctx,
                                     s32 index,
                                     bool isGoldmine,
                                     const WarLevelUnit* oldEntity,
                                     const WarLevelUnit* newEntity)
{
    if (!ctx->history)
    {
        return;
    }

    WarEditorOp op;
    memset(&op, 0, sizeof(op));
    op.type                  = WE_OP_EDIT_ENTITY;
    op.editEntity.index      = index;
    op.editEntity.isGoldmine = isGoldmine;
    op.editEntity.oldEntity  = *oldEntity;
    op.editEntity.newEntity  = *newEntity;
    wehist_push(ctx->history, op);
}

static void weinspect_pushStartLocation(WarEditorContext* ctx,
                                        u16 oldX, u16 oldY,
                                        u16 newX, u16 newY)
{
    if (!ctx->history)
    {
        return;
    }

    WarEditorOp op;
    memset(&op, 0, sizeof(op));
    op.type          = WE_OP_SET_START;
    op.setStart.oldX = oldX;
    op.setStart.oldY = oldY;
    op.setStart.newX = newX;
    op.setStart.newY = newY;
    wehist_push(ctx->history, op);
}

static void weinspect_pushMapEdit(WarEditorContext* ctx,
                                  WarEditorMap* m,
                                  const void* fieldPtr,
                                  u16 size,
                                  const void* oldData,
                                  const void* newData)
{
    if (!ctx->history || !m || !fieldPtr || size == 0 ||
        size > WE_HISTORY_MAP_EDIT_MAX_BYTES)
    {
        return;
    }

    u32 offset = (u32)((const u8*)fieldPtr - (const u8*)m);
    if (offset + size > sizeof(*m))
    {
        return;
    }

    WarEditorOp op;
    memset(&op, 0, sizeof(op));
    op.type           = WE_OP_EDIT_MAP;
    op.editMap.offset = offset;
    op.editMap.size   = size;
    memcpy(op.editMap.oldData, oldData, size);
    memcpy(op.editMap.newData, newData, size);
    wehist_push(ctx->history, op);
}

static void weinspect_pushMapNameEdit(WarEditorContext* ctx,
                                      const char* oldName,
                                      const char* newName)
{
    if (!ctx->history)
    {
        return;
    }

    WarEditorOp op;
    memset(&op, 0, sizeof(op));
    op.type = WE_OP_EDIT_MAP_NAME;
    SDL_strlcpy(op.editMapName.oldName, oldName, sizeof(op.editMapName.oldName));
    SDL_strlcpy(op.editMapName.newName, newName, sizeof(op.editMapName.newName));
    wehist_push(ctx->history, op);
}

void weinspect_drawPanel(WarEditorContext* ctx)
{
    WarEditorMap* m = ctx->map;

    if (igBegin("Inspector##inspector", NULL, ImGuiWindowFlags_None))
    {
        // -------------------------------------------------------------------
        // Section 1: Entity fields
        // Only shown when exactly one entity is selected.
        // -------------------------------------------------------------------
        if (igCollapsingHeader_TreeNodeFlags("Entity##sect",
                                             ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (!m || ctx->selectedEntities.count == 0)
            {
                igTextDisabled("No entity selected.");
            }
            else if (ctx->selectedEntities.count > 1)
            {
                igTextDisabled("%d entities selected.", ctx->selectedEntities.count);
                igTextDisabled("Select one to edit its fields.");

                // Multi-select: allow changing player for all selected entities.
                igSeparator();
                igAlignTextToFramePadding();
                igText("Apply to all:");
                for (s32 p = 0; p < MAX_PLAYERS_COUNT; p++)
                {
                    static const ImVec4_c s_pCol[5] =
                    {
                        { 0.00f, 0.00f, 0.78f, 1.0f },
                        { 0.78f, 0.00f, 0.00f, 1.0f },
                        { 0.00f, 0.78f, 0.00f, 1.0f },
                        { 0.78f, 0.78f, 0.00f, 1.0f },
                        { 0.78f, 0.78f, 0.78f, 1.0f },
                    };
                    ImVec4_c col = s_pCol[p];
                    ImVec4_c hov = { fminf(col.x + 0.2f, 1.0f),
                                     fminf(col.y + 0.2f, 1.0f),
                                     fminf(col.z + 0.2f, 1.0f),
                                     1.0f };
                    igPushStyleColor_Vec4(ImGuiCol_Button,        col);
                    igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, hov);
                    char lbl[16];
                    SDL_snprintf(lbl, sizeof(lbl), "P%d##mp%d", p, p);
                    if (igButton(lbl, (ImVec2_c){ 28.0f, 0.0f }))
                    {
                        for (s32 i = 0; i < ctx->selectedEntities.count; i++)
                        {
                            WarEntityId id = ctx->selectedEntities.items[i];
                            if (id & 0x8000u) continue; // goldmines have no player
                            u32 idx = (u32)(id - 1u);
                            if (idx < m->startEntitiesCount)
                            {
                                WarLevelUnit oldEntity = m->startEntities[idx];
                                m->startEntities[idx].player = (u8)p;
                                if (oldEntity.player != (u8)p)
                                {
                                    weinspect_pushEntityEdit(ctx, (s32)idx, false,
                                                             &oldEntity,
                                                             &m->startEntities[idx]);
                                }
                            }
                        }
                        ctx->unsavedChanges = true;
                        ctx->minimapDirty = true;
                    }
                    igPopStyleColor(2);
                    if (p < MAX_PLAYERS_COUNT - 1)
                        igSameLine(0.0f, 2.0f);
                }
            }
            else
            {
                WarEntityId id = ctx->selectedEntities.items[0];

                WarLevelUnit* lu   = NULL;
                bool          isGM = false;

                if (id & 0x8000u)
                {
                    u32 idx = (u32)(id & 0x7FFFu);
                    if (idx < m->startGoldminesCount)
                    {
                        lu   = &m->startGoldmines[idx];
                        isGM = true;
                    }
                }
                else
                {
                    u32 idx = (u32)(id - 1u);
                    if (idx < m->startEntitiesCount)
                        lu = &m->startEntities[idx];
                }

                s32 entityIndex = -1;
                if (lu)
                {
                    entityIndex = isGM ? (s32)(id & 0x7FFFu) : (s32)(id - 1u);
                }

                if (!lu)
                {
                    igTextDisabled("Invalid entity.");
                }
                else
                {
                    // Unit type (read-only)
                    const WarUnitData* ud = wu_getUnitData(lu->type);
                    weinspect_beginRow("Type");
                    if (ud)
                        igText("%.*s", (int)ud->name.length, ud->name.data);
                    else
                        igText("%d", (s32)lu->type);

                    // Tile X
                    s32 posX = (s32)lu->x;
                    weinspect_beginRow("Tile X");
                    if (igInputInt("##tileX", &posX, 1, 10, 0))
                    {
                        WarLevelUnit oldEntity = *lu;
                        if (posX < 0) posX = 0;
                        if (posX >= MAP_TILES_WIDTH) posX = MAP_TILES_WIDTH - 1;
                        if (oldEntity.x != (u8)posX)
                        {
                            if (weinspect_canEntityOccupy(ctx, entityIndex, isGM, posX, (s32)lu->y))
                            {
                                lu->x = (u8)posX;
                                weinspect_pushEntityEdit(ctx, entityIndex, isGM, &oldEntity, lu);
                                ctx->unsavedChanges = true;
                                ctx->minimapDirty = true;
                            }
                            else
                            {
                                SDL_strlcpy(ctx->statusText,
                                            "Inspector move blocked: tile is non-passable or occupied.",
                                            sizeof(ctx->statusText));
                            }
                        }
                    }

                    // Tile Y
                    s32 posY = (s32)lu->y;
                    weinspect_beginRow("Tile Y");
                    if (igInputInt("##tileY", &posY, 1, 10, 0))
                    {
                        WarLevelUnit oldEntity = *lu;
                        if (posY < 0) posY = 0;
                        if (posY >= MAP_TILES_HEIGHT) posY = MAP_TILES_HEIGHT - 1;
                        if (oldEntity.y != (u8)posY)
                        {
                            if (weinspect_canEntityOccupy(ctx, entityIndex, isGM, (s32)lu->x, posY))
                            {
                                lu->y = (u8)posY;
                                weinspect_pushEntityEdit(ctx, entityIndex, isGM, &oldEntity, lu);
                                ctx->unsavedChanges = true;
                                ctx->minimapDirty = true;
                            }
                            else
                            {
                                SDL_strlcpy(ctx->statusText,
                                            "Inspector move blocked: tile is non-passable or occupied.",
                                            sizeof(ctx->statusText));
                            }
                        }
                    }

                    // Player (not editable for goldmines)
                    if (!isGM)
                    {
                        s32 player = (s32)lu->player;
                        weinspect_beginRow("Player");
                        if (igInputInt("##player", &player, 1, 1, 0))
                        {
                            WarLevelUnit oldEntity = *lu;
                            if (player < 0) player = 0;
                            if (player >= MAX_PLAYERS_COUNT)
                                player = MAX_PLAYERS_COUNT - 1;
                            lu->player = (u8)player;
                            if (oldEntity.player != lu->player)
                            {
                                weinspect_pushEntityEdit(ctx, entityIndex, isGM, &oldEntity, lu);
                                ctx->unsavedChanges = true;
                                ctx->minimapDirty = true;
                            }
                        }
                    }

                    // Resource kind
                    s32 resKind = (s32)lu->resourceKind;
                    weinspect_beginRow("Resource");
                    if (igCombo_Str("##resKind", &resKind, s_resourceKindNames, 4))
                    {
                        WarLevelUnit oldEntity = *lu;
                        lu->resourceKind = (WarResourceKind)resKind;
                        if (oldEntity.resourceKind != lu->resourceKind)
                        {
                            weinspect_pushEntityEdit(ctx, entityIndex, isGM, &oldEntity, lu);
                            ctx->unsavedChanges = true;
                            ctx->minimapDirty = true;
                        }
                    }

                    // Amount
                    s32 amount = (s32)lu->amount;
                    weinspect_beginRow("Amount");
                    if (igInputInt("##amount", &amount, 100, 1000, 0))
                    {
                        WarLevelUnit oldEntity = *lu;
                        if (amount < 0) amount = 0;
                        lu->amount = (u16)(amount > 65535 ? 65535 : amount);
                        if (oldEntity.amount != lu->amount)
                        {
                            weinspect_pushEntityEdit(ctx, entityIndex, isGM, &oldEntity, lu);
                            ctx->unsavedChanges = true;
                            ctx->minimapDirty = true;
                        }
                    }
                }
            }
        }

        // -------------------------------------------------------------------
        // Section 2: Per-player (race, gold, lumber, upgrades)
        // -------------------------------------------------------------------
        if (igCollapsingHeader_TreeNodeFlags("Players##sect",
                                             ImGuiTreeNodeFlags_None))
        {
            if (!m)
            {
                igTextDisabled("No map loaded.");
            }
            else
            {
                for (s32 p = 0; p < MAX_PLAYERS_COUNT; p++)
                {
                    char hdr[32];
                    SDL_snprintf(hdr, sizeof(hdr), "Player %d##pp%d", p, p);

                    igPushID_Int(p);

                    if (igCollapsingHeader_TreeNodeFlags(hdr, ImGuiTreeNodeFlags_None))
                    {
                        // Race
                        s32 race = (s32)m->races[p];
                        weinspect_beginRow("Race");
                        if (igCombo_Str("##race", &race, s_raceNames, 4))
                        {
                            WarRace oldRace = m->races[p];
                            WarRace newRace = (WarRace)race;
                            if (oldRace != newRace)
                            {
                                m->races[p] = newRace;
                                weinspect_pushMapEdit(ctx, m, &m->races[p], sizeof(m->races[p]),
                                                      &oldRace, &newRace);
                                ctx->unsavedChanges = true;
                            }
                        }

                        // Gold
                        s32 gold = (s32)m->gold[p];
                        weinspect_beginRow("Gold");
                        if (igInputInt("##gold", &gold, 100, 1000, 0))
                        {
                            u32 oldGold = m->gold[p];
                            if (gold < 0) gold = 0;
                            u32 newGold = (u32)gold;
                            if (oldGold != newGold)
                            {
                                m->gold[p] = newGold;
                                weinspect_pushMapEdit(ctx, m, &m->gold[p], sizeof(m->gold[p]),
                                                      &oldGold, &newGold);
                                ctx->unsavedChanges = true;
                            }
                        }

                        // Lumber
                        s32 lumber = (s32)m->lumber[p];
                        weinspect_beginRow("Lumber");
                        if (igInputInt("##lumber", &lumber, 100, 1000, 0))
                        {
                            u32 oldLumber = m->lumber[p];
                            if (lumber < 0) lumber = 0;
                            u32 newLumber = (u32)lumber;
                            if (oldLumber != newLumber)
                            {
                                m->lumber[p] = newLumber;
                                weinspect_pushMapEdit(ctx, m, &m->lumber[p], sizeof(m->lumber[p]),
                                                      &oldLumber, &newLumber);
                                ctx->unsavedChanges = true;
                            }
                        }

                        // Upgrades
                        igSeparatorText("Upgrades");
                        for (s32 u = 0; u < MAX_UPGRADES_COUNT; u++)
                        {
                            bool enabled = (m->allowedUpgrades[u][p] != 0);
                            const StringView* uname = &upgradeNames[u];

                            char uchk[64];
                            SDL_snprintf(uchk, sizeof(uchk), "%.*s##u%d",
                                         (int)uname->length, uname->data, u);

                            if (igCheckbox(uchk, &enabled))
                            {
                                u8 oldVal = m->allowedUpgrades[u][p];
                                u8 newVal = enabled ? 1u : 0u;
                                if (oldVal != newVal)
                                {
                                    m->allowedUpgrades[u][p] = newVal;
                                    weinspect_pushMapEdit(ctx, m, &m->allowedUpgrades[u][p],
                                                          sizeof(m->allowedUpgrades[u][p]),
                                                          &oldVal, &newVal);
                                    ctx->unsavedChanges = true;
                                }
                            }
                        }
                    }

                    igPopID();
                }
            }
        }

        // -------------------------------------------------------------------
        // Section 3: Map meta (name, objectives, start position, features)
        // -------------------------------------------------------------------
        if (igCollapsingHeader_TreeNodeFlags("Map##sect", ImGuiTreeNodeFlags_None))
        {
            if (!m)
            {
                igTextDisabled("No map loaded.");
            }
            else
            {
                // Map name
                weinspect_beginRow("Name");
                {
                    char oldName[sizeof(ctx->mapName)];
                    SDL_strlcpy(oldName, ctx->mapName, sizeof(oldName));
                    if (igInputText("##mapname", ctx->mapName, sizeof(ctx->mapName), 0, NULL, NULL))
                    {
                        if (strcmp(oldName, ctx->mapName) != 0)
                        {
                            weinspect_pushMapNameEdit(ctx, oldName, ctx->mapName);
                            ctx->unsavedChanges = true;
                        }
                    }
                }

                // Start camera position
                s32 startX = (s32)m->startX;
                weinspect_beginRow("Start X");
                if (igInputInt("##startX", &startX, 1, 10, 0))
                {
                    u16 oldX = m->startX;
                    u16 oldY = m->startY;
                    if (startX < 0) startX = 0;
                    if (startX >= MAP_TILES_WIDTH) startX = MAP_TILES_WIDTH - 1;
                    m->startX = (u16)startX;
                    if (oldX != m->startX)
                    {
                        weinspect_pushStartLocation(ctx, oldX, oldY, m->startX, m->startY);
                        ctx->unsavedChanges = true;
                    }
                }

                s32 startY = (s32)m->startY;
                weinspect_beginRow("Start Y");
                if (igInputInt("##startY", &startY, 1, 10, 0))
                {
                    u16 oldX = m->startX;
                    u16 oldY = m->startY;
                    if (startY < 0) startY = 0;
                    if (startY >= MAP_TILES_HEIGHT) startY = MAP_TILES_HEIGHT - 1;
                    m->startY = (u16)startY;
                    if (oldY != m->startY)
                    {
                        weinspect_pushStartLocation(ctx, oldX, oldY, m->startX, m->startY);
                        ctx->unsavedChanges = true;
                    }
                }

                // Objectives text
                igSeparatorText("Objectives");
                char oldObj[sizeof(m->objectives)];
                SDL_strlcpy(oldObj, m->objectives, sizeof(oldObj));
                if (igInputTextMultiline("##obj", m->objectives,
                                         sizeof(m->objectives),
                                         (ImVec2_c){ -1.0f, 80.0f },
                                         0, NULL, NULL))
                {
                    if (strcmp(oldObj, m->objectives) != 0)
                    {
                        weinspect_pushMapEdit(ctx, m, m->objectives,
                                              (u16)sizeof(m->objectives),
                                              oldObj, m->objectives);
                        ctx->unsavedChanges = true;
                    }
                }

                // Allowed features — human name at [f*2], orc name at [f*2+1]
                igSeparatorText("Allowed Features");
                for (s32 f = 0; f < MAX_FEATURES_COUNT; f++)
                {
                    bool enabled = (m->allowedFeatures[f] != 0);
                    const StringView* fname = &features[f * 2];

                    char fchk[96];
                    SDL_snprintf(fchk, sizeof(fchk), "%.*s##f%d",
                                 (int)fname->length, fname->data, f);

                    if (igCheckbox(fchk, &enabled))
                    {
                        u8 oldVal = m->allowedFeatures[f];
                        u8 newVal = enabled ? 1u : 0u;
                        if (oldVal != newVal)
                        {
                            m->allowedFeatures[f] = newVal;
                            weinspect_pushMapEdit(ctx, m, &m->allowedFeatures[f],
                                                  sizeof(m->allowedFeatures[f]),
                                                  &oldVal, &newVal);
                            ctx->unsavedChanges = true;
                        }
                    }
                }
            }
        }
    }
    igEnd();
}
