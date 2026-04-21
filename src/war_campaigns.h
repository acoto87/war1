#pragma once

#include "war_types.h"

typedef struct
{
    WarCampaignMapType type;
    WarCheckObjectivesFunc checkObjectivesFunc;
    const char* objectives;
    WarAudioId briefingAudioId;
    const char* briefingText;
    f32 briefingDuration;
} WarCampaignMapData;

WarCampaignMapData getCampaignData(WarCampaignMapType type);
