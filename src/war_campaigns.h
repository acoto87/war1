#pragma once

#include "war_audio.h"

struct _WarCampaignMapData
{
    WarCampaignMapType type;
    WarCheckObjectivesFunc checkObjectivesFunc;
    StringView objectives;
    WarAudioId briefingAudioId;
    StringView briefingText;
    f32 briefingDuration;
};

WarCampaignMapData wcamp_getCampaignData(WarCampaignMapType type);
bool wcamp_tryGetMapType(WarRace race, s32 mission, WarCampaignMapType* mapType);
