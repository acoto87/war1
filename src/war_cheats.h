#pragma once

#include "war_math.h"
#include "war_fwd.h"

#define STATUS_TEXT_MAX_LENGTH 40
#define CHEAT_TEXT_MAX_LENGTH 32
#define CHEAT_FEEDBACK_TEXT_MAX_LENGTH 50

struct _WarFlashStatus
{
    bool enabled;
    f64 endRealTime;
    String text;
};

struct _WarCheatStatus
{
    bool enabled;
    bool visible;
    s32 position;
    String text;
    bool feedback;
    f64 feedbackEndRealTime;
    String feedbackText;
    bool debugTextActive;
    String debugText;
    // X offset (relative to panel.x + 2) of the blinking cursor rect.
    // Written by the update function; read by the render function.
    // Negative value means cursor is not visible.
    f32 cursorX;
};

#define CHEAT_GOLD_INCREASE 10000
#define CHEAT_WOOD_INCREASE 5000
#define CHEAT_SPEED_UP_FACTOR 100.0f

#define CHEAT_FEEDBACK_WASCALLY_WABBIT "cheat enabled you wascally wabbit"

struct _WarCheatDescriptor
{
    WarCheat cheat;
    char text[CHEAT_TEXT_MAX_LENGTH];
    bool argument;
    WarCheatFunc cheatFunc;
};

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
void wcheat_applyPoisonCloudCheat(WarContext* context, StringView argument);
void wcheat_applyRaiseDeadCheat(WarContext* context, StringView argument);
void wcheat_applyAddUnitCheat(WarContext* context, StringView argument);
void wcheat_applyDebugRenderCheat(WarContext* context, StringView argument);

// ui
void wcheatp_setCheatsPanelVisible(WarContext* context, bool visible);
void wcheatp_setCheatsFeedback(WarContext* context, String feedbackText);
void wcheatp_setDebugText(WarContext* context, StringView text);
void wcheatp_createCheatsPanel(WarContext* context);
void wcheatp_setCheatText(WarContext* context, String text);
void wcheatp_updateCheatsPanel(WarContext* context);
void wcheatp_renderCheatsPanel(WarContext* context);
