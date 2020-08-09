#pragma once

typedef struct
{
    WarCommandQueue commands;
} WarAICustomData;

typedef struct
{
    WarUnitType unitType;
    s32 count;
} WarSquadUnitRequest;

WarCommand* requestUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, bool checkExisting, bool waitForIdleWorker);

WarCommand* requestFootmanOrGrunt(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestPeasantOrPeon(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestCatapult(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestKnightOrRider(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestArcherOrSpearman(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestConjurerOrWarlock(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestClericOrNecrolyte(WarContext* context, WarPlayerInfo* aiPlayer);

WarCommand* requestFarm(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarCommand* requestBarracks(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarCommand* requestChurchOrTemple(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarCommand* requestTower(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarCommand* requestTownHall(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarCommand* requestLumbermill(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarCommand* requestStableOrKennel(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);
WarCommand* requestBlacksmith(WarContext* context, WarPlayerInfo* aiPlayer, bool checkExisting, bool waitForIdleWorker);

WarCommand* requestUpgrade(WarContext* context, WarPlayerInfo* aiPlayer, WarUpgradeType upgradeType);

WarCommand* requestUpgradeArrowsOrSpears(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestUpgradeSwordsOrAxes(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestUpgradeHorsesOrWolves(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestUpgradeScorpionsOrSpiders(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestUpgradeRainOfFireOrPoisonCloud(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestUpgradeWaterElementalOrDaemon(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestUpgradeHealingOrRaiseDead(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestUpgradeFarSightOrDarkVision(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestUpgradeInvisibilityOrUnholyArmor(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* requestUpgradeShields(WarContext* context, WarPlayerInfo* aiPlayer);

WarCommand* sendToGatherResource(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resource, s32 count, bool waitForIdleWorker);

#define sendToGatherGold(context, aiPlayer, count, waitForIdleWorker) sendToGatherResource(context, aiPlayer, WAR_RESOURCE_GOLD, count, waitForIdleWorker)
#define sendToGatherWood(context, aiPlayer, count, waitForIdleWorker) sendToGatherResource(context, aiPlayer, WAR_RESOURCE_WOOD, count, waitForIdleWorker)

WarCommand* waitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarCommandId commandId);
WarCommand* waitForResource(WarContext* context, WarPlayerInfo* aiPlayer, WarResourceKind resource, s32 amount);

#define waitForGold(context, aiPlayer, amount) waitForResource(context, aiPlayer, WAR_RESOURCE_GOLD, amount)
#define waitForWood(context, aiPlayer, amount) waitForResource(context, aiPlayer, WAR_RESOURCE_WOOD, amount)

WarCommand* requestSquad(WarContext* context, WarPlayerInfo* aiPlayer, WarSquadId id, s32 count, WarSquadUnitRequest requests[]);

WarSquadUnitRequest createSquadUnitRequest(WarUnitType unitType, s32 count);
WarSquadUnitRequest squadFootmanOrGruntRequest(WarPlayerInfo* aiPlayer, s32 count);
WarSquadUnitRequest squadCatapultRequest(WarPlayerInfo* aiPlayer, s32 count);
WarSquadUnitRequest squadKnightOrRiderRequest(WarPlayerInfo* aiPlayer, s32 count);
WarSquadUnitRequest squadArcherOrSpearmanRequest(WarPlayerInfo* aiPlayer, s32 count);
WarSquadUnitRequest squadConjurerOrWarlockRequest(WarPlayerInfo* aiPlayer, s32 count);
WarSquadUnitRequest squadClericOrNecrolyteRequest(WarPlayerInfo* aiPlayer, s32 count);

WarCommand* attackWithSquad(WarContext* context, WarPlayerInfo* aiPlayer, WarSquadId id);

void levelInitAI(WarContext* context, WarPlayerInfo* aiPlayer);
WarCommand* levelGetAICommand(WarContext* context, WarPlayerInfo* aiPlayer);
