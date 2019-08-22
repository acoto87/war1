bool checkMap01Objectives(WarContext* context);
bool checkMap02Objectives(WarContext* context);

typedef struct
{
    WarCampaignMapType type;
    WarCheckObjectivesFunc checkObjectivesFunc;
    const char* objectives;
    WarAudioId briefingAudioId;
    const char* briefingText;
} WarCampaignMapData;

const WarCampaignMapData campaignsData[] =
{
    { WAR_CAMPAIGN_HUMANS_01,   checkMap01Objectives,   "Build:\n  6 farms\n  1 barracks",              WAR_CAMPAIGNS_HUMAN_01_INTRO,   "As a test of your abilities, the King has appointed\nyou as Regent over a small parcel of land. Since\nwe must keep our armies in the field well supplied,\nyou are to build the town into a farming center of\nno less than 6 farms. Construction of a barracks\nfor defense is also advised, as our scouts have\nreported Orc patrols in the area." },
    { WAR_CAMPAIGN_ORCS_01,     checkMap01Objectives,   "Build:\n  6 farms\n  1 barracks",              WAR_CAMPAIGNS_ORC_01_INTRO,     "Blackhand has assigned you to an outpost in the\nSwamps of Sorrow.  Your task is simple enough\nthat even the War Chief feels that you are capable\nof it. Construct at least 6 farms, so that we may\nkeep our troops well fed and ready to do battle.\nOnly a fool would leave his treasures unguarded,\nso you must also build a barracks for the defense\nof these farms."   },
    { WAR_CAMPAIGN_HUMANS_02,   checkMap02Objectives,   "Defend town and\ndestroy orcs",                WAR_CAMPAIGNS_HUMAN_02_INTRO,   "The Orcs around Grand Hamlet are becoming increasingly brazen in their attacks, and our spies inform us that they are amassing a large army to march against the town.  The King is sending you, along with a small detachment of troops, to rally the people and defend the town against all opposition." },
    { WAR_CAMPAIGN_ORCS_02,     checkMap02Objectives,   "Crush all opposition",                         WAR_CAMPAIGNS_ORC_02_INTRO,     "Like the stinging of a wasp, the attacks from the humans grow more and more bothersome.  You have been assigned to a small outpost on the Borderlands of the Swamps of Sorrow.  You are to defend our lands from the incursions of these ravenous dogs by crushing any opposition that you encounter."   },
    { WAR_CAMPAIGN_HUMANS_03,   NULL,                   "Destroy outpost",                              0,                              NULL },
    { WAR_CAMPAIGN_ORCS_03,     NULL,                   "Demolish Grand Hamlet",                        0,                              NULL },
    { WAR_CAMPAIGN_HUMANS_04,   NULL,                   "Find lothar,\nheal him and bring\nthem out",   0,                              NULL },
    { WAR_CAMPAIGN_ORCS_04,     NULL,                   "Kill griselda and\nher minions",               0,                              NULL },
    { WAR_CAMPAIGN_HUMANS_05,   NULL,                   "Destroy orcs",                                 0,                              NULL },
    { WAR_CAMPAIGN_ORCS_05,     NULL,                   "Save the outpost\nSlay all humans",            0,                              NULL },
    { WAR_CAMPAIGN_HUMANS_06,   NULL,                   "Save the abbey and\ndestroy the traitors",     0,                              NULL },
    { WAR_CAMPAIGN_ORCS_06,     NULL,                   "Raze sunnyglade\nSave tower",                  0,                              NULL },
    { WAR_CAMPAIGN_HUMANS_07,   NULL,                   "Rescue peasants\nDemolish orcs",               0,                              NULL },
    { WAR_CAMPAIGN_ORCS_07,     NULL,                   "Kill Blackhand's troops",                      0,                              NULL },
    { WAR_CAMPAIGN_HUMANS_08,   NULL,                   "Slay Medivh and\nall minions",                 0,                              NULL },
    { WAR_CAMPAIGN_ORCS_08,     NULL,                   "Save garona\nDestroy the abbey",               0,                              NULL },
    { WAR_CAMPAIGN_HUMANS_09,   NULL,                   "Destroy all traces\nof the orcs",              0,                              NULL },
    { WAR_CAMPAIGN_ORCS_09,     NULL,                   "Crush the two human\noutposts",                0,                              NULL },
    { WAR_CAMPAIGN_HUMANS_10,   NULL,                   "Destroy temple raze town\nKill orcs",          0,                              NULL },
    { WAR_CAMPAIGN_ORCS_10,     NULL,                   "Destroy the human camp",                       0,                              NULL },
    { WAR_CAMPAIGN_HUMANS_11,   NULL,                   "Destroy rockard\nDestroy stonard",             0,                              NULL },
    { WAR_CAMPAIGN_ORCS_11,     NULL,                   "Raze moonbrook\nRaze goldshire",               0,                              NULL },
    { WAR_CAMPAIGN_HUMANS_12,   NULL,                   "Destroy Black Rock Spire\nand all orcs!",      0,                              NULL },
    { WAR_CAMPAIGN_ORCS_12,     NULL,                   "Destroy Stormwind Keep\nand all humans!",      0,                              NULL },
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
