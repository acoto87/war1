#pragma once

#include "war_math.h"

#define STATUS_TEXT_MAX_LENGTH 40
#define CHEAT_TEXT_MAX_LENGTH 32
#define CHEAT_FEEDBACK_TEXT_MAX_LENGTH 50

enum _WarCheat
{
    WAR_CHEAT_NONE,

    // original cheats
    WAR_CHEAT_GOLD,
    WAR_CHEAT_SPELLS,
    WAR_CHEAT_UPGRADES,
    WAR_CHEAT_END,
    WAR_CHEAT_ENABLE,
    WAR_CHEAT_GOD_MODE,
    WAR_CHEAT_WIN,
    WAR_CHEAT_LOSS,
    WAR_CHEAT_FOG,
    WAR_CHEAT_SKIP_HUMAN,
    WAR_CHEAT_SKIP_ORC,
    WAR_CHEAT_SPEED,

    // custom cheats
    WAR_CHEAT_MUSIC,
    WAR_CHEAT_SOUND,
    WAR_CHEAT_MUSIC_VOL,
    WAR_CHEAT_SOUND_VOL,
    WAR_CHEAT_GLOBAL_SCALE,
    WAR_CHEAT_GLOBAL_SPEED,
    WAR_CHEAT_EDIT,
    WAR_CHEAT_ADD_UNIT,
    WAR_CHEAT_RAIN_OF_FIRE,

    WAR_CHEAT_COUNT
};

struct _WarFlashStatus
{
    bool enabled;
    f32 startTime;
    f32 duration;
    String text;
};

struct _WarCheatStatus
{
    bool enabled;
    bool visible;
    s32 position;
    String text;
    bool feedback;
    f32 feedbackTime;
    String feedbackText;
};

#define CHEAT_GOLD_INCREASE 10000
#define CHEAT_WOOD_INCREASE 5000
#define CHEAT_SPEED_UP_FACTOR 100.0f

#define CHEAT_FEEDBACK_WASCALLY_WABBIT "cheat enabled you wascally wabbit"

typedef void (*WarCheatFunc)(struct _WarContext* context, StringView argument);

typedef struct
{
    WarCheat cheat;
    char text[CHEAT_TEXT_MAX_LENGTH];
    bool argument;
    WarCheatFunc cheatFunc;
} WarCheatDescriptor;

void wcheat_applyCheat(struct _WarContext* context, StringView text);

#define cheatsEnabledAndVisible(mapOrScene) ((mapOrScene)->cheatStatus.enabled && (mapOrScene)->cheatStatus.visible)

// original cheats
void wcheat_applyGoldCheat(struct _WarContext* context, StringView argument);
void wcheat_applySpellsCheat(struct _WarContext* context, StringView argument);
void wcheat_applyUpgradesCheat(struct _WarContext* context, StringView argument);
void wcheat_applyEndCheat(struct _WarContext* context, StringView argument);
void wcheat_applyEnableCheat(struct _WarContext* context, StringView argument);
void wcheat_applyGodModeCheat(struct _WarContext* context, StringView argument);
void wcheat_applyWinCheat(struct _WarContext* context, StringView argument);
void wcheat_applyLossCheat(struct _WarContext* context, StringView argument);
void wcheat_applyFogOfWarCheat(struct _WarContext* context, StringView argument);
void wcheat_applySkipHumanCheat(struct _WarContext* context, StringView argument);
void wcheat_applySkipOrcCheat(struct _WarContext* context, StringView argument);
void wcheat_applySpeedCheat(struct _WarContext* context, StringView argument);

// custom cheats
void wcheat_applyMusicCheat(struct _WarContext* context, StringView argument);
void wcheat_applySoundCheat(struct _WarContext* context, StringView argument);
void wcheat_applyMusicVolCheat(struct _WarContext* context, StringView argument);
void wcheat_applySoundVolCheat(struct _WarContext* context, StringView argument);
void wcheat_applyGlobalScaleCheat(struct _WarContext* context, StringView argument);
void wcheat_applyGlobalSpeedCheat(struct _WarContext* context, StringView argument);
void wcheat_applyEditCheat(struct _WarContext* context, StringView argument);
void wcheat_applyRainOfFireCheat(struct _WarContext* context, StringView argument);
void wcheat_applyAddUnitCheat(struct _WarContext* context, StringView argument);

// ui
void wcheatp_setCheatsPanelVisible(struct _WarContext* context, bool visible);
void wcheatp_setCheatsFeedback(struct _WarContext* context, String feedbackText);
void wcheatp_createCheatsPanel(struct _WarContext* context);
void wcheatp_setCheatText(struct _WarContext* context, String text);
void wcheatp_updateCheatsPanel(struct _WarContext* context);
