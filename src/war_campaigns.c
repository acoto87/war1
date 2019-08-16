bool checkMap01Objectives(WarContext* context);
bool checkMap02Objectives(WarContext* context);

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
    const char* objectives;
} WarCampaignMapData;

const WarCampaignMapData campaignsData[] =
{
    { WAR_CAMPAIGN_HUMANS_01,   checkMap01Objectives,   "Build:\n  6 farms\n  1 barracks" },
    { WAR_CAMPAIGN_ORCS_01,     checkMap01Objectives,   "Build:\n  6 farms\n  1 barracks" },
    { WAR_CAMPAIGN_HUMANS_02,   checkMap02Objectives,   "Defend town and\ndestroy orcs" },
    { WAR_CAMPAIGN_ORCS_02,     checkMap02Objectives,   "Crush all opposition" },
    { WAR_CAMPAIGN_HUMANS_03,   NULL,                   "Destroy outpost" },
    { WAR_CAMPAIGN_ORCS_03,     NULL,                   "Demolish Grand Hamlet" },
    { WAR_CAMPAIGN_HUMANS_04,   NULL,                   "Find lothar,\nheal him and bring\nthem out" },
    { WAR_CAMPAIGN_ORCS_04,     NULL,                   "Kill griselda and\nher minions" },
    { WAR_CAMPAIGN_HUMANS_05,   NULL,                   "Destroy orcs" },
    { WAR_CAMPAIGN_ORCS_05,     NULL,                   "Save the outpost\nSlay all humans" },
    { WAR_CAMPAIGN_HUMANS_06,   NULL,                   "Save the abbey and\ndestroy the traitors" },
    { WAR_CAMPAIGN_ORCS_06,     NULL,                   "Raze sunnyglade\nSave tower" },
    { WAR_CAMPAIGN_HUMANS_07,   NULL,                   "Rescue peasants\nDemolish orcs" },
    { WAR_CAMPAIGN_ORCS_07,     NULL,                   "Kill Blackhand's troops" },
    { WAR_CAMPAIGN_HUMANS_08,   NULL,                   "Slay Medivh and\nall minions" },
    { WAR_CAMPAIGN_ORCS_08,     NULL,                   "Save garona\nDestroy the abbey" },
    { WAR_CAMPAIGN_HUMANS_09,   NULL,                   "Destroy all traces\nof the orcs" },
    { WAR_CAMPAIGN_ORCS_09,     NULL,                   "Crush the two human\noutposts" },
    { WAR_CAMPAIGN_HUMANS_10,   NULL,                   "Destroy temple raze town\nKill orcs" },
    { WAR_CAMPAIGN_ORCS_10,     NULL,                   "Destroy the human camp" },
    { WAR_CAMPAIGN_HUMANS_11,   NULL,                   "Destroy rockard\nDestroy stonard" },
    { WAR_CAMPAIGN_ORCS_11,     NULL,                   "Raze moonbrook\nRaze goldshire" },
    { WAR_CAMPAIGN_HUMANS_12,   NULL,                   "Destroy Black Rock Spire\nand all orcs!" },
    { WAR_CAMPAIGN_ORCS_12,     NULL,                   "Destroy Stormwind Keep\nand all humans!" },
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

bool checkMap02Objectives(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* enemy = &map->players[1];

    return getTotalNumberOfDudes(context, enemy->index) == 0;
}
