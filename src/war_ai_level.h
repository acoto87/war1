#pragma once

typedef struct
{
    s32 index;
    WarAICommandQueue commands;
    WarAICommandList runningCommands;
} WarAICustomData;

// WarAICommand* createUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool checkExisting, bool waitForIdleWorker)
// {
//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarAICustomData* customData = (WarAICustomData*)ai->customData;
//     assert(customData);

//     WarAICommand* command = createRequestAICommand(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
//     WarAICommandQueuePush(&customData->commands, command);

//     return command;
// }

// WarAICommand* createUnitAndWait(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool checkExisting, bool waitForIdleWorker)
// {
//     WarAICommand* command = createUnit(context, aiPlayer, unitType, checkExisting, waitForIdleWorker);
//     command->wait = true;
//     return command;
// }

// #define createFootman(context, aiPlayer, wait) wait \
//     ? createUnitAndWait(context, aiPlayer, WAR_UNIT_FOOTMAN, false, false) \
//     : createUnit(context, aiPlayer, WAR_UNIT_FOOTMAN, false, false)
// #define createGrunt(context, aiPlayer, wait) wait \
//     ? createUnitAndWait(context, aiPlayer, WAR_UNIT_GRUNT, false, false) \
//     : createUnit(context, aiPlayer, WAR_UNIT_GRUNT, false, false)
// #define createFootmanOrGrunt(context, aiPlayer, wait) wait \
//     ? createUnitAndWait(context, aiPlayer, getUnitTypeForRace(WAR_UNIT_FOOTMAN, aiPlayer->race), false, false)
//     : createUnit(context, aiPlayer, getUnitTypeForRace(WAR_UNIT_FOOTMAN, aiPlayer->race), false, false)

// WAR_UNIT_PEASANT,
// WAR_UNIT_PEON,
// WAR_UNIT_CATAPULT_HUMANS,
// WAR_UNIT_CATAPULT_ORCS,
// WAR_UNIT_KNIGHT,
// WAR_UNIT_RAIDER,
// WAR_UNIT_ARCHER,
// WAR_UNIT_SPEARMAN,
// WAR_UNIT_CONJURER,
// WAR_UNIT_WARLOCK,
// WAR_UNIT_CLERIC,
// WAR_UNIT_NECROLYTE,
// WAR_UNIT_MEDIVH,
// WAR_UNIT_LOTHAR,
// WAR_UNIT_WOUNDED,
// WAR_UNIT_GRIZELDA,
// WAR_UNIT_GARONA,
// WAR_UNIT_OGRE,
// WAR_UNIT_SPIDER,
// WAR_UNIT_SLIME,
// WAR_UNIT_FIRE_ELEMENTAL,
// WAR_UNIT_SCORPION,
// WAR_UNIT_BRIGAND,
// WAR_UNIT_THE_DEAD,
// WAR_UNIT_SKELETON,
// WAR_UNIT_DAEMON,
// WAR_UNIT_WATER_ELEMENTAL,
// WAR_UNIT_DRAGON_CYCLOPS_GIANT,
// WAR_UNIT_26,
// WAR_UNIT_30,

// // buildings
// WAR_UNIT_FARM_HUMANS,
// WAR_UNIT_FARM_ORCS,
// WAR_UNIT_BARRACKS_HUMANS,
// WAR_UNIT_BARRACKS_ORCS,
// WAR_UNIT_CHURCH,
// WAR_UNIT_TEMPLE,
// WAR_UNIT_TOWER_HUMANS,
// WAR_UNIT_TOWER_ORCS,
// WAR_UNIT_TOWNHALL_HUMANS,
// WAR_UNIT_TOWNHALL_ORCS,
// WAR_UNIT_LUMBERMILL_HUMANS,
// WAR_UNIT_LUMBERMILL_ORCS,
// WAR_UNIT_STABLE,
// WAR_UNIT_KENNEL,
// WAR_UNIT_BLACKSMITH_HUMANS,
// WAR_UNIT_BLACKSMITH_ORCS,
// WAR_UNIT_STORMWIND,
// WAR_UNIT_BLACKROCK,

// // neutral
// WAR_UNIT_GOLDMINE,

// WAR_UNIT_51,

// // others
// WAR_UNIT_HUMAN_CORPSE,
// WAR_UNIT_ORC_CORPSE,

// WAR_UNIT_COUNT

void levelInitAI(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* levelGetAICommand(WarContext* context, WarPlayerInfo* aiPlayer);
void levelExecutedAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command);