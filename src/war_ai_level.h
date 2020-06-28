#pragma once

typedef struct
{
    s32 index;
    WarAICommandQueue commands;
    WarAICommandList runningCommands;
} WarAICustomData;

WarAICommand* requestUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool checkExisting, bool waitForIdleWorker);

WarAICommand* requestFootmanOrGrunt(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestPeasantOrPeon(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestCatapult(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestKnightOrRider(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestArcherOrSpearman(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestConjurerOrWarlock(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestClericOrNecrolyte(WarContext* context, WarPlayerInfo* aiPlayer);

WarAICommand* requestFarm(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarAICommand* requestBarracks(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarAICommand* requestChurchOrTemple(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarAICommand* requestTower(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarAICommand* requestTownHall(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarAICommand* requestLumbermill(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarAICommand* requestStableOrKennel(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarAICommand* requestBlacksmith(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);

WarAICommand* requestUpgrade(WarContext* context, WarPlayerInfo* aiPlayer, WarUpgradeType upgradeType);

WarAICommand* requestUpgradeArrowsOrSpears(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestUpgradeSwordsOrAxes(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestUpgradeHorsesOrWolves(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestUpgradeScorpionsOrSpiders(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestUpgradeRainOfFireOrPoisonCloud(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestUpgradeWaterElementalOrDaemon(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestUpgradeHealingOrRaiseDead(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestUpgradeFarSightOrDarkVision(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestUpgradeInvisibilityOrUnholyArmor(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* requestUpgradeShields(WarContext* context, WarPlayerInfo* aiPlayer);

WarAICommand* sendToGatherResource(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resource, s32 count, bool waitForIdleWorker);

#define sendToGatherGold(context, aiPlayer, count, waitForIdleWorker) sendToGatherResource(context, aiPlayer, WAR_RESOURCE_GOLD, count, waitForIdleWorker)
#define sendToGatherWood(context, aiPlayer, count, waitForIdleWorker) sendToGatherResource(context, aiPlayer, WAR_RESOURCE_WOOD, count, waitForIdleWorker)

WarAICommand* waitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandId commandId);
WarAICommand* waitForResource(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resource, s32 amount);

#define waitForGold(context, aiPlayer, amount) waitForResource(context, aiPlayer, WAR_RESOURCE_GOLD, amount)
#define waitForWood(context, aiPlayer, amount) waitForResource(context, aiPlayer, WAR_RESOURCE_WOOD, amount)

void levelInitAI(WarContext* context, WarPlayerInfo* aiPlayer);
WarAICommand* levelGetAICommand(WarContext* context, WarPlayerInfo* aiPlayer);
void levelExecutedAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command);