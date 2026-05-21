#include "war_editor_inspector.h"

// ---------------------------------------------------------------------------
// Phase 10 — Inspector Panel
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
                                m->startEntities[idx].player = (u8)p;
                        }
                        ctx->unsavedChanges = true;
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
                        if (posX < 0) posX = 0;
                        if (posX >= MAP_TILES_WIDTH) posX = MAP_TILES_WIDTH - 1;
                        lu->x = (u8)posX;
                        ctx->unsavedChanges = true;
                    }

                    // Tile Y
                    s32 posY = (s32)lu->y;
                    weinspect_beginRow("Tile Y");
                    if (igInputInt("##tileY", &posY, 1, 10, 0))
                    {
                        if (posY < 0) posY = 0;
                        if (posY >= MAP_TILES_HEIGHT) posY = MAP_TILES_HEIGHT - 1;
                        lu->y = (u8)posY;
                        ctx->unsavedChanges = true;
                    }

                    // Player (not editable for goldmines)
                    if (!isGM)
                    {
                        s32 player = (s32)lu->player;
                        weinspect_beginRow("Player");
                        if (igInputInt("##player", &player, 1, 1, 0))
                        {
                            if (player < 0) player = 0;
                            if (player >= MAX_PLAYERS_COUNT)
                                player = MAX_PLAYERS_COUNT - 1;
                            lu->player = (u8)player;
                            ctx->unsavedChanges = true;
                        }
                    }

                    // Resource kind
                    s32 resKind = (s32)lu->resourceKind;
                    weinspect_beginRow("Resource");
                    if (igCombo_Str("##resKind", &resKind, s_resourceKindNames, 4))
                    {
                        lu->resourceKind = (WarResourceKind)resKind;
                        ctx->unsavedChanges = true;
                    }

                    // Amount
                    s32 amount = (s32)lu->amount;
                    weinspect_beginRow("Amount");
                    if (igInputInt("##amount", &amount, 100, 1000, 0))
                    {
                        if (amount < 0) amount = 0;
                        lu->amount = (u16)(amount > 65535 ? 65535 : amount);
                        ctx->unsavedChanges = true;
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
                            m->races[p] = (WarRace)race;
                            ctx->unsavedChanges = true;
                        }

                        // Gold
                        s32 gold = (s32)m->gold[p];
                        weinspect_beginRow("Gold");
                        if (igInputInt("##gold", &gold, 100, 1000, 0))
                        {
                            if (gold < 0) gold = 0;
                            m->gold[p] = (u32)gold;
                            ctx->unsavedChanges = true;
                        }

                        // Lumber
                        s32 lumber = (s32)m->lumber[p];
                        weinspect_beginRow("Lumber");
                        if (igInputInt("##lumber", &lumber, 100, 1000, 0))
                        {
                            if (lumber < 0) lumber = 0;
                            m->lumber[p] = (u32)lumber;
                            ctx->unsavedChanges = true;
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
                                m->allowedUpgrades[u][p] = enabled ? 1u : 0u;
                                ctx->unsavedChanges = true;
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
                if (igInputText("##mapname", ctx->mapName, sizeof(ctx->mapName), 0, NULL, NULL))
                    ctx->unsavedChanges = true;

                // Start camera position
                s32 startX = (s32)m->startX;
                weinspect_beginRow("Start X");
                if (igInputInt("##startX", &startX, 1, 10, 0))
                {
                    if (startX < 0) startX = 0;
                    if (startX >= MAP_TILES_WIDTH) startX = MAP_TILES_WIDTH - 1;
                    m->startX = (u16)startX;
                    ctx->unsavedChanges = true;
                }

                s32 startY = (s32)m->startY;
                weinspect_beginRow("Start Y");
                if (igInputInt("##startY", &startY, 1, 10, 0))
                {
                    if (startY < 0) startY = 0;
                    if (startY >= MAP_TILES_HEIGHT) startY = MAP_TILES_HEIGHT - 1;
                    m->startY = (u16)startY;
                    ctx->unsavedChanges = true;
                }

                // Objectives text
                igSeparatorText("Objectives");
                if (igInputTextMultiline("##obj", m->objectives,
                                         sizeof(m->objectives),
                                         (ImVec2_c){ -1.0f, 80.0f },
                                         0, NULL, NULL))
                    ctx->unsavedChanges = true;

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
                        m->allowedFeatures[f] = enabled ? 1u : 0u;
                        ctx->unsavedChanges = true;
                    }
                }
            }
        }
    }
    igEnd();
}
