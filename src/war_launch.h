#pragma once

#include <stdbool.h>

#include "war_common.h"
#include "war_enums.h"

#define WAR_LAUNCH_MAP_VALUE_CAPACITY 512

typedef enum _WarLaunchMode
{
    WAR_LAUNCH_DEFAULT,
    WAR_LAUNCH_CAMPAIGN,
    WAR_LAUNCH_PREDEFINED_CUSTOM,
    WAR_LAUNCH_MAP_FILE
} WarLaunchMode;

typedef struct _WarCustomGameOptions WarCustomGameOptions;
typedef struct _WarLaunchConfig WarLaunchConfig;

struct _WarCustomGameOptions
{
    bool enabled;
    WarRace playerRace;
    WarRace enemyRace;
    bool hasGold;
    s32 gold;
    bool hasWood;
    s32 wood;
    bool hasSeed;
    u64 seed;
    s32 startConfigurationIndex;
};

struct _WarLaunchConfig
{
    WarLaunchMode mode;
    bool skipIntro;
    WarRace campaignRace;
    s32 campaignMission;
    s32 customMapIndex;
    char mapValue[WAR_LAUNCH_MAP_VALUE_CAPACITY];
    WarCustomGameOptions customGame;
};
