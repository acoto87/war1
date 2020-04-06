WarLevelResult checkMap01Objectives(WarContext* context);
WarLevelResult checkMap02Objectives(WarContext* context);
WarLevelResult checkCustomMapObjectives(WarContext* context);

typedef struct
{
    WarCampaignMapType type;
    WarCheckObjectivesFunc checkObjectivesFunc;
    const char* objectives;
    WarAudioId briefingAudioId;
    const char* briefingText;
    f32 briefingDuration;
} WarCampaignMapData;

const WarCampaignMapData campaignsData[] =
{
    {
        WAR_CAMPAIGN_HUMANS_01,
        checkMap01Objectives,
        "Build:\n\t6 farms\n\t1 barracks",
        WAR_CAMPAIGNS_HUMAN_01_INTRO,
        "As a test of your abilities, the King has appointed\n"
        "you as Regent over a small parcel of land. Since\n"
        "we must keep our armies in the field well supplied,\n"
        "you are to build the town into a farming center of\n"
        "no less than 6 farms. Construction of a barracks\n"
        "for defense is also advised, as our scouts have\n"
        "reported Orc patrols in the area.",
        24.0f
    },
    {
        WAR_CAMPAIGN_ORCS_01,
        checkMap01Objectives,
        "Build:\n  6 farms\n  1 barracks",
        WAR_CAMPAIGNS_ORC_01_INTRO,
        "Blackhand has assigned you to an outpost in the\n"
        "Swamps of Sorrow.  Your task is simple enough\n"
        "that even the War Chief feels that you are capable\n"
        "of it. Construct at least 6 farms, so that we may\n"
        "keep our troops well fed and ready to do battle.\n"
        "Only a fool would leave his treasures unguarded,\n"
        "so you must also build a barracks for the defense\n"
        "of these farms.",
        28.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_02,
        checkMap02Objectives,
        "Defend town and\ndestroy orcs",
        WAR_CAMPAIGNS_HUMAN_02_INTRO,
        "The Orcs around Grand Hamlet are becoming\n"
        "increasingly brazen in their attacks, and our\n"
        "spies inform us that they are amassing a large\n"
        "army to march against the town.\n"
        "The King is sending you, along with a small\n"
        "detachment of troops, to rally the people and\n"
        "defend the town against all opposition.",
        20.0f
    },
    {
        WAR_CAMPAIGN_ORCS_02,
        checkMap02Objectives,
        "Crush all opposition",
        WAR_CAMPAIGNS_ORC_02_INTRO,
        "Like the stinging of a wasp, the attacks from the\n"
        "humans grow more and more bothersome.\n"
        "You have been assigned to a small outpost on the\n"
        "Borderlands of the Swamps of Sorrow.\n"
        "You are to defend our lands from the incursions\n"
        "of these ravenous dogs by crushing any opposition\n"
        "that you encounter.",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_03,
        NULL,
        "Destroy outpost",
        WAR_CAMPAIGNS_HUMAN_03_INTRO,
        "With Blackhand's raiding parties routed, now is the time for\n"
        "us to secure a lasting peace in the area around Grand Hamlet.\n"
        "You must seek out the Orcish outpost of Kyross that lies deep\n"
        "within the Swamps of Sorrow, and destroy it.",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_03,
        NULL,
        "Demolish Grand Hamlet",
        WAR_CAMPAIGNS_ORC_03_INTRO,
        "The Humans are growing strong in Grand Hamlet. An outpost will\n"
        "be placed under your dictatorship to use as you see fit. You must\n"
        "then prepare and lead a force to destroy Grand Hamlet and all that\n"
        "dwell there. Blackhand will brook no survivors - these Humans must\n"
        "be taught a hard lesson in the ways of humility.",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_04,
        NULL,
        "Find lothar,\nheal him and bring\nthem out",
        WAR_CAMPAIGNS_HUMAN_04_INTRO,
        "It has been some twenty months since Sir Lothar, one of the crown's\n"
        "greatest heroes, led an expedition into the Dead Mines to search for\n"
        "the Lost Tome of Divinity. They were never heard from again.\n"
        "However, the great knight has recently appeared to the Abbot of\n"
        "Northshire in a vision - battered and pleading for assistance.\n"
        "King Llane has ordered you to lead a detachment of warriors and healers\n"
        "into the mines in an attempt to find Sir Lothar, heal him, and bring him\n"
        "and any other survivors back alive.",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_04,
        NULL,
        "Kill griselda and\nher minions",
        WAR_CAMPAIGNS_ORC_04_INTRO,
        "You are wakened from your nights sleep by a runner from the War Chief.\n"
        "Blackhand's daughter Griselda has run off with the outlaw Turok's band of Ogres.\n"
        "Our wolfriders have tracked them to the dungeons hidden beneath the Dead Mines.\n"
        "Find Turok's band of rebellious pigs and kill them all --- including Griselda.\n"
        "She must not disobey the commands of her father... ever again.",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_05,
        NULL,
        "Destroy orcs",
        WAR_CAMPAIGNS_HUMAN_05_INTRO,
        "The Forest of Elwynn is a strategic key to securing the Borderlands.\n"
        "An outpost near the southeast edge of the forest will serve as your stronghold.\n"
        "The King has assigned one of his knights to aid you, so that your task of ridding\n"
        "the area of Blackhand's dark minions may be more readily completed.",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_05,
        NULL,
        "Save the outpost\nSlay all humans",
        WAR_CAMPAIGNS_ORC_05_INTRO,
        "On your return from the dungeon, you receive word from advance scouts that the\n"
        "recently established outpost near the Red Ridge Mountains is under siege.\n"
        "A group of raiders have been dispatched to assist you in taking back the\n"
        "outpost and crushing the Human opposition. Your secondary objective is to seek\n"
        "out and completely destroy their encampment, putting an end to this threat for good.",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_06,
        NULL,
        "Save the abbey and\ndestroy the traitors",
        WAR_CAMPAIGNS_HUMAN_06_INTRO,
        "The monks of Northshire Abbey are under siege by a band of warriors that have been\n"
        "convinced by enemy agents to fight against the crown. You will be given a complement\n"
        "of knights to lead to the Abbey, which is already under attack. Ride hard and fast,\n"
        "as you must prevent its destruction.  When you have secured the Abbey and beaten back\n"
        "these treacherous curs, you must then move to destroy the enemy at their source.",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_06,
        NULL,
        "Raze sunnyglade\nSave tower",
        WAR_CAMPAIGNS_ORC_06_INTRO,
        "The Humans of Sunnyglade have become fat and lazy with their prosperity.\n"
        "The town is like a ripe plum waiting to be plucked. You will march upon their\n"
        "weak Human armies and smash them to pieces. Somewhere in the town is a tower that\n"
        "you must keep intact so that we may study how their magiks are created. Fail me,\n"
        "and I will have your head on a pike at the gates of Black Rock Spire.",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_07,
        NULL,
        "Rescue peasants\nDemolish orcs",
        WAR_CAMPAIGNS_HUMAN_07_INTRO,
        "A raiding party has completely overrun the village of Sunnyglade.\n"
        "Our scouts report that the survivors have been taken to a hidden Orcish\n"
        "compound to serve as slaves. You must take a detachment of warriors and\n"
        "rescue the group of peasants that are imprisoned somewhere in the Orc camp.\n"
        "Our intelligence confirms that all of the prisoners are together, and that\n"
        "you must destroy the enclosure to open a path for their escape.\n"
        "The rebuilding of Sunnyglade is also of the utmost importance,\n"
        "as you will need their assistance in destroying the Orcish slavers.",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_07,
        NULL,
        "Kill Blackhand's troops",
        WAR_CAMPAIGNS_ORC_07_INTRO,
        "The time has come for you to seize control of the Orcish hordes for yourself.\n"
        "Blackhand has become foolish in the deployment of his personal troops, and has\n"
        "left an opening that you can now exploit. A key outpost in the Black Morass is\n"
        "the core of Blackhand's supply lines - not only to his foremost battle groups,\n"
        "but to his castle at Black Rock Spire, as well. The complete destruction of this\n"
        "outpost will disrupt his power base long enough for you to secure his overthrow.",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_08,
        NULL,
        "Slay Medivh and\nall minions",
        WAR_CAMPAIGNS_HUMAN_08_INTRO,
        "A new crisis has arisen that threatens to end the lives of all who would\n"
        "serve the King. The evil warlock Medivh has begun draining the soul of\n"
        "the land itself to increase his dark powers. You must take a party into\n"
        "his tower and destroy him before he summons enough energies to devastate\n"
        "all who would oppose him. Beware his mastery of the black arts, for\n"
        "legend speaks of his ability to command the daemons of Hell.",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_08,
        NULL,
        "Save garona\nDestroy the abbey",
        WAR_CAMPAIGNS_ORC_08_INTRO,
        "The destruction of Blackhand's outpost has left him in a weak position.\n"
        "The Shadow Council, sensing your rise in power, orders the assassination\n"
        "of Blackhand and elevates you to the position of War Chief.\n"
        "A wolfrider brings you news that our best spy, the half-orc Garona,\n"
        "has been discovered by the Humans of Northshire Abbey and imprisoned there.\n"
        "She has valuable information concerning new and powerful magiks that would\n"
        "aid you in the destruction of your counterpart - King Llane.\n"
        "Trusting no one to complete this vital mission in time, you must find her,\n"
        "and then completely destroy the Abbey to protect her secrets.",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_09,
        NULL,
        "Destroy all traces\nof the orcs",
        WAR_CAMPAIGNS_HUMAN_09_INTRO,
        "The time has come to take the battle into Blackhand's own domain.\n"
        "King Llane has ordered a full assault upon the Orcs, demanding\n"
        "that this plague that spreads across the kingdom be eradicated.\n"
        "To the east of the Borderlands lies the Black Morass where the\n"
        "Orcish hordes make their encampments. You are to lead an army into\n"
        "this foul region and destroy every trace of their dark presence.",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_09,
        NULL,
        "Crush the two human\noutposts",
        WAR_CAMPAIGNS_ORC_09_INTRO,
        "With your new found magiks, the time is ripe to burn the Human\n"
        "occupation from our lands. There are two Human outposts to the\n"
        "south that pose the greatest threat to our security.\n"
        "Reports from scouts near these towns show that the key to your\n"
        "success in this confrontation is to hold back the Human forces\n"
        "at their bridges while you strengthen your attack force.\n"
        "The glories of combat will be yours as you personally lead the\n"
        "armies that will reclaim your homelands.",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_10,
        NULL,
        "Destroy temple raze town\nKill orcs",
        WAR_CAMPAIGNS_HUMAN_10_INTRO,
        "Runners have arrived and informed you of grave news. King Llane\n"
        "lies dead this day, assassinated by the treacherous Garona, at\n"
        "Stormwind Keep.  His last command was that you should assume the\n"
        "mantle of War Leader, and end this battle that has drained the land\n"
        "of its resources, and now its king. Scouts report that deep within\n"
        "the Black Morass lies one of Blackhand's darkest seats of power ---\n"
        "the Temple of the Damned. No peasants dare approach the vile temple,\n"
        "and only the bravest of your soldiers have agreed to accompany you on\n"
        "this mission. You must strike boldly and without err, for there will\n"
        "be no reinforcements.",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_10,
        NULL,
        "Destroy the human camp",
        WAR_CAMPAIGNS_ORC_10_INTRO,
        "You have tasted victory, and the craving for more is upon you.\n"
        "It is clear that one decisive blow to the Humans will make the total\n"
        "and complete domination of this race a simple matter.\n"
        "Your spies have gathered intelligence that points to an encampment\n"
        "near the center of the Human lands where their knights and soldiers\n"
        "are sent to train. Although they will not be expecting an attack,\n"
        "they should prove a good fight. The destruction of this site would\n"
        "greatly weaken their forces, and etch your position as War Chief\n"
        "in stone. None shall survive!",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_11,
        NULL,
        "Destroy rockard\nDestroy stonard",
        WAR_CAMPAIGNS_HUMAN_11_INTRO,
        "Here beats the diseased and malevolent heart of Blackhand's plagued lands.\n"
        "The sister towns of Rockard and Stonard are all that stand between the\n"
        "forces of the kingdom and Blackhand's stronghold - Black Rock Spire.\n"
        "After conferring with your warchiefs, the path to victory lays clear.\n"
        "You must destroy Rockard and Stonard, thereby cutting off all lines of\n"
        "support and supplies, so that the final offensive can be made upon Black Rock Spire.",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_11,
        NULL,
        "Raze moonbrook\nRaze goldshire",
        WAR_CAMPAIGNS_ORC_11_INTRO,
        "The final march to King Llane's home, Stormwind Keep, is at hand.\n"
        "Only two pathetic settlements stand in the way of the awesome\n"
        "juggernaut your cruel leadership has created. The Humans have proved\n"
        "to be amusing opposition, but the hour of doom has come for them.\n"
        "The complete and utter demolition of the twin cities Goldshire and\n"
        "Moonbrook will sever the lifeline between the King and his people,\n"
        "making him a figurehead waiting to be lopped off.",
        22.0f
    },
    {
        WAR_CAMPAIGN_HUMANS_12,
        NULL,
        "Destroy Black Rock Spire\nand all orcs!",
        WAR_CAMPAIGNS_HUMAN_12_INTRO,
        "Black Rock Spire stands before us! The skies above the reeking swamp\n"
        "fill with the gathering thunderheads that spell doom for the loser in\n"
        "this final confrontation. Tension hangs like a heavy cloak on your\n"
        "shoulders as your troops prepare for the battle ahead. Above the din\n"
        "and chaos that swirls about the battlefield stands the Castle of Blackhand,\n"
        "its gaze sweeping down upon the battlefield where the destiny of the land\n"
        "will be decided. Destroy the stronghold and those who would seek to defend it,\n"
        "and Azeroth will be freed from Blackhand's poisoned grip forever!",
        22.0f
    },
    {
        WAR_CAMPAIGN_ORCS_12,
        NULL,
        "Destroy Stormwind Keep\nand all humans!",
        WAR_CAMPAIGNS_ORC_12_INTRO,
        "Stormwind Keep is ours to take! The Orcish hordes gather like buzzards to\n"
        "carrion, as the moment of destiny is close at hand. A low growl fills the\n"
        "air as your wolfriders whip their savage mounts into a frenzy. The earth\n"
        "shakes as catapults are loaded and moved into position. The fires of the\n"
        "burning rubble about you dance in your eyes as you gaze upon the pristine,\n"
        "white towers of Castle Stormwind. White that will soon be washed with the\n"
        "red of King Llane's blood.  With his fall, all of Azeroth will be yours!",
        22.0f
    },
    {
        WAR_CAMPAIGN_CUSTOM,
        checkCustomMapObjectives,
        "Destroy enemy",
        0,
        NULL,
        0.0f
    }
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

WarLevelResult checkMap01Objectives(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];

    WarUnitType farmType = isHumanPlayer(player)
        ? WAR_UNIT_FARM_HUMANS : WAR_UNIT_FARM_ORCS;
    WarUnitType barracksType = isHumanPlayer(player)
        ? WAR_UNIT_BARRACKS_HUMANS : WAR_UNIT_BARRACKS_ORCS;

    if (getNumberOfBuildingsOfType(context, player->index, farmType, true) >= 6 &&
        getNumberOfBuildingsOfType(context, player->index, barracksType, true) >= 1)
    {
        return WAR_LEVEL_RESULT_WIN;
    }

    if (getTotalNumberOfUnits(context, player->index) == 0)
    {
        return WAR_LEVEL_RESULT_LOSE;
    }

    return WAR_LEVEL_RESULT_NONE;
}

WarLevelResult checkMap02Objectives(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    WarPlayerInfo* enemy = &map->players[1];

    if (getTotalNumberOfDudes(context, enemy->index) == 0)
    {
        return WAR_LEVEL_RESULT_WIN;
    }

    if (getTotalNumberOfUnits(context, player->index) == 0)
    {
        return WAR_LEVEL_RESULT_LOSE;
    }

    return WAR_LEVEL_RESULT_NONE;
}

WarLevelResult checkCustomMapObjectives(WarContext* context)
{
    WarMap* map = context->map;
    WarPlayerInfo* player = &map->players[0];
    WarPlayerInfo* enemy = &map->players[1];

    if (getTotalNumberOfUnits(context, enemy->index) == 0)
    {
        return WAR_LEVEL_RESULT_WIN;
    }

    if (getTotalNumberOfUnits(context, player->index) == 0)
    {
        return WAR_LEVEL_RESULT_LOSE;
    }

    return WAR_LEVEL_RESULT_NONE;
}
