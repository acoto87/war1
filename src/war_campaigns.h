#pragma once

#include "war_audio.h"

enum _WarCampaignMapType
{
    WAR_CAMPAIGN_HUMANS_01 = 117,
    WAR_CAMPAIGN_ORCS_01,
    WAR_CAMPAIGN_HUMANS_02,
    WAR_CAMPAIGN_ORCS_02,
    WAR_CAMPAIGN_HUMANS_03,
    WAR_CAMPAIGN_ORCS_03,
    WAR_CAMPAIGN_HUMANS_04,
    WAR_CAMPAIGN_ORCS_04,
    WAR_CAMPAIGN_HUMANS_05,
    WAR_CAMPAIGN_ORCS_05,
    WAR_CAMPAIGN_HUMANS_06,
    WAR_CAMPAIGN_ORCS_06,
    WAR_CAMPAIGN_HUMANS_07,
    WAR_CAMPAIGN_ORCS_07,
    WAR_CAMPAIGN_HUMANS_08,
    WAR_CAMPAIGN_ORCS_08,
    WAR_CAMPAIGN_HUMANS_09,
    WAR_CAMPAIGN_ORCS_09,
    WAR_CAMPAIGN_HUMANS_10,
    WAR_CAMPAIGN_ORCS_10,
    WAR_CAMPAIGN_HUMANS_11,
    WAR_CAMPAIGN_ORCS_11,
    WAR_CAMPAIGN_HUMANS_12,
    WAR_CAMPAIGN_ORCS_12,
    WAR_CAMPAIGN_CUSTOM
};

enum _WarLevelResult
{
    WAR_LEVEL_RESULT_NONE,
    WAR_LEVEL_RESULT_WIN,
    WAR_LEVEL_RESULT_LOSE
};

typedef WarLevelResult (*WarCheckObjectivesFunc)(struct _WarContext* context);

typedef struct
{
    WarCampaignMapType type;
    WarCheckObjectivesFunc checkObjectivesFunc;
    String objectives;
    WarAudioId briefingAudioId;
    String briefingText;
    f32 briefingDuration;
} WarCampaignMapData;

WarCampaignMapData wcamp_getCampaignData(WarCampaignMapType type);
