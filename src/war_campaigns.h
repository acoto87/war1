#pragma once

#include "war_audio.h"

struct _WarCampaignMapData
{
    WarCampaignMapType type;
    WarCheckObjectivesFunc checkObjectivesFunc;
    String objectives;
    WarAudioId briefingAudioId;
    String briefingText;
    f32 briefingDuration;
};

WarCampaignMapData wcamp_getCampaignData(WarCampaignMapType type);
