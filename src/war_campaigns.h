#pragma once

#include "war_types.h"

typedef struct
{
    WarCampaignMapType type;
    WarCheckObjectivesFunc checkObjectivesFunc;
    String objectives;
    WarAudioId briefingAudioId;
    String briefingText;
    f32 briefingDuration;
} WarCampaignMapData;

WarCampaignMapData getCampaignData(WarCampaignMapType type);
