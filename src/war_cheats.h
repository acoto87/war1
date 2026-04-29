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

void wcheat_applyCheat(WarContext* context, StringView text);

#define cheatsEnabledAndVisible(mapOrScene) ((mapOrScene)->cheatStatus.enabled && (mapOrScene)->cheatStatus.visible)

// original cheats
void wcheat_applyGoldCheat(WarContext* context, StringView argument);
void wcheat_applySpellsCheat(WarContext* context, StringView argument);
void wcheat_applyUpgradesCheat(WarContext* context, StringView argument);
void wcheat_applyEndCheat(WarContext* context, StringView argument);
void wcheat_applyEnableCheat(WarContext* context, StringView argument);
void wcheat_applyGodModeCheat(WarContext* context, StringView argument);
void wcheat_applyWinCheat(WarContext* context, StringView argument);
void wcheat_applyLossCheat(WarContext* context, StringView argument);
void wcheat_applyFogOfWarCheat(WarContext* context, StringView argument);
void wcheat_applySkipHumanCheat(WarContext* context, StringView argument);
void wcheat_applySkipOrcCheat(WarContext* context, StringView argument);
void wcheat_applySpeedCheat(WarContext* context, StringView argument);

// custom cheats
void wcheat_applyMusicCheat(WarContext* context, StringView argument);
void wcheat_applySoundCheat(WarContext* context, StringView argument);
void wcheat_applyMusicVolCheat(WarContext* context, StringView argument);
void wcheat_applySoundVolCheat(WarContext* context, StringView argument);
void wcheat_applyGlobalScaleCheat(WarContext* context, StringView argument);
void wcheat_applyGlobalSpeedCheat(WarContext* context, StringView argument);
void wcheat_applyEditCheat(WarContext* context, StringView argument);
void wcheat_applyRainOfFireCheat(WarContext* context, StringView argument);
void wcheat_applyAddUnitCheat(WarContext* context, StringView argument);

// ui
void setCheatsPanelVisible(WarContext* context, bool visible);
void setCheatsFeedback(WarContext* context, String feedbackText);
void createCheatsPanel(WarContext* context);
void setCheatText(WarContext* context, String text);
void updateCheatsPanel(WarContext* context);
