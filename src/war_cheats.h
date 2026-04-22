#pragma once

#include "war_types.h"

#define CHEAT_GOLD_INCREASE 10000
#define CHEAT_WOOD_INCREASE 5000
#define CHEAT_SPEED_UP_FACTOR 100.0f

#define CHEAT_FEEDBACK_WASCALLY_WABBIT "cheat enabled you wascally wabbit"

typedef void (*WarCheatFunc)(WarContext* context, StringView argument);

typedef struct
{
    WarCheat cheat;
    char text[CHEAT_TEXT_MAX_LENGTH];
    bool argument;
    WarCheatFunc cheatFunc;
} WarCheatDescriptor;

void applyCheat(WarContext* context, StringView text);

#define cheatsEnabledAndVisible(mapOrScene) ((mapOrScene)->cheatStatus.enabled && (mapOrScene)->cheatStatus.visible)

// original cheats
void applyGoldCheat(WarContext* context, StringView argument);
void applySpellsCheat(WarContext* context, StringView argument);
void applyUpgradesCheat(WarContext* context, StringView argument);
void applyEndCheat(WarContext* context, StringView argument);
void applyEnableCheat(WarContext* context, StringView argument);
void applyGodModeCheat(WarContext* context, StringView argument);
void applyWinCheat(WarContext* context, StringView argument);
void applyLossCheat(WarContext* context, StringView argument);
void applyFogOfWarCheat(WarContext* context, StringView argument);
void applySkipHumanCheat(WarContext* context, StringView argument);
void applySkipOrcCheat(WarContext* context, StringView argument);
void applySpeedCheat(WarContext* context, StringView argument);

// custom cheats
void applyMusicCheat(WarContext* context, StringView argument);
void applySoundCheat(WarContext* context, StringView argument);
void applyMusicVolCheat(WarContext* context, StringView argument);
void applySoundVolCheat(WarContext* context, StringView argument);
void applyGlobalScaleCheat(WarContext* context, StringView argument);
void applyGlobalSpeedCheat(WarContext* context, StringView argument);
void applyEditCheat(WarContext* context, StringView argument);
void applyRainOfFireCheat(WarContext* context, StringView argument);
void applyAddUnitCheat(WarContext* context, StringView argument);

// ui
void setCheatsPanelVisible(WarContext* context, bool visible);
void setCheatsFeedback(WarContext* context, String feedbackText);
void createCheatsPanel(WarContext* context);
void setCheatText(WarContext* context, char* text, ...);
void updateCheatsPanel(WarContext* context);
