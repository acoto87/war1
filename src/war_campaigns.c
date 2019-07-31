bool checkMap01Objectives(WarContext* context);

typedef enum
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
    WAR_CAMPAIGN_ORCS_12
} WarCampaignMapType;

typedef struct
{
    WarCampaignMapType type;
    WarCheckObjectivesFunc checkObjectivesFunc;
} WarCampaignMapData;

const WarCampaignMapData campaignsData[] =
{
    { WAR_CAMPAIGN_HUMANS_01,   checkMap01Objectives },
    { WAR_CAMPAIGN_ORCS_01,     checkMap01Objectives },
    { WAR_CAMPAIGN_HUMANS_02,   NULL },
    { WAR_CAMPAIGN_ORCS_02,     NULL },
    { WAR_CAMPAIGN_HUMANS_03,   NULL },
    { WAR_CAMPAIGN_ORCS_03,     NULL },
    { WAR_CAMPAIGN_HUMANS_04,   NULL },
    { WAR_CAMPAIGN_ORCS_04,     NULL },
    { WAR_CAMPAIGN_HUMANS_05,   NULL },
    { WAR_CAMPAIGN_ORCS_05,     NULL },
    { WAR_CAMPAIGN_HUMANS_06,   NULL },
    { WAR_CAMPAIGN_ORCS_06,     NULL },
    { WAR_CAMPAIGN_HUMANS_07,   NULL },
    { WAR_CAMPAIGN_ORCS_07,     NULL },
    { WAR_CAMPAIGN_HUMANS_08,   NULL },
    { WAR_CAMPAIGN_ORCS_08,     NULL },
    { WAR_CAMPAIGN_HUMANS_09,   NULL },
    { WAR_CAMPAIGN_ORCS_09,     NULL },
    { WAR_CAMPAIGN_HUMANS_10,   NULL },
    { WAR_CAMPAIGN_ORCS_10,     NULL },
    { WAR_CAMPAIGN_HUMANS_11,   NULL },
    { WAR_CAMPAIGN_ORCS_11,     NULL },
    { WAR_CAMPAIGN_HUMANS_12,   NULL },
    { WAR_CAMPAIGN_ORCS_12,     NULL },
};

WarCampaignMapData getCampaignData(WarCampaignMapType type)
{
    s32 index = 0;
    s32 length = arrayLength(campaignsData);
    while (index < length && campaignsData[index].type != type)
        index++;

    assert(index < length);
    return campaignsData[index];
}

bool checkMap01Objectives(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    
    WarUnitType farmType = isHumanPlayer(player) 
        ? WAR_UNIT_FARM_HUMANS : WAR_UNIT_FARM_ORCS;
    WarUnitType barracksType = isHumanPlayer(player) 
        ? WAR_UNIT_BARRACKS_HUMANS : WAR_UNIT_BARRACKS_ORCS;

    return getNumberOfBuildingsOfType(context, player->index, farmType, true) >= 6 &&
           getNumberOfBuildingsOfType(context, player->index, barracksType, true) >= 1;
}