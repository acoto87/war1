#pragma once

#include "war_types.h"

bool wcomm_executeCommand(WarContext* context);

// train units
void wcomm_trainFootman(WarContext* context, WarEntity* entity);
void wcomm_trainGrunt(WarContext* context, WarEntity* entity);
void wcomm_trainPeasant(WarContext* context, WarEntity* entity);
void wcomm_trainPeon(WarContext* context, WarEntity* entity);
void wcomm_trainHumanCatapult(WarContext* context, WarEntity* entity);
void wcomm_trainOrcCatapult(WarContext* context, WarEntity* entity);
void wcomm_trainKnight(WarContext* context, WarEntity* entity);
void wcomm_trainRaider(WarContext* context, WarEntity* entity);
void wcomm_trainArcher(WarContext* context, WarEntity* entity);
void wcomm_trainSpearman(WarContext* context, WarEntity* entity);
void wcomm_trainConjurer(WarContext* context, WarEntity* entity);
void wcomm_trainWarlock(WarContext* context, WarEntity* entity);
void wcomm_trainCleric(WarContext* context, WarEntity* entity);
void wcomm_trainNecrolyte(WarContext* context, WarEntity* entity);

// upgrades
void wcomm_upgradeSwords(WarContext* context, WarEntity* entity);
void wcomm_upgradeAxes(WarContext* context, WarEntity* entity);
void wcomm_upgradeHumanShields(WarContext* context, WarEntity* entity);
void wcomm_upgradeOrcsShields(WarContext* context, WarEntity* entity);
void wcomm_upgradeArrows(WarContext* context, WarEntity* entity);
void wcomm_upgradeSpears(WarContext* context, WarEntity* entity);
void wcomm_upgradeHorses(WarContext* context, WarEntity* entity);
void wcomm_upgradeWolves(WarContext* context, WarEntity* entity);
void wcomm_upgradeScorpions(WarContext* context, WarEntity* entity);
void wcomm_upgradeSpiders(WarContext* context, WarEntity* entity);
void wcomm_upgradeRainOfFire(WarContext* context, WarEntity* entity);
void wcomm_upgradePoisonCloud(WarContext* context, WarEntity* entity);
void wcomm_upgradeWaterElemental(WarContext* context, WarEntity* entity);
void wcomm_upgradeDaemon(WarContext* context, WarEntity* entity);
void wcomm_upgradeHealing(WarContext* context, WarEntity* entity);
void wcomm_upgradeRaiseDead(WarContext* context, WarEntity* entity);
void wcomm_upgradeFarSight(WarContext* context, WarEntity* entity);
void wcomm_upgradeDarkVision(WarContext* context, WarEntity* entity);
void wcomm_upgradeInvisibility(WarContext* context, WarEntity* entity);
void wcomm_upgradeUnholyArmor(WarContext* context, WarEntity* entity);

// wcomm_cancel
void wcomm_cancel(WarContext* context, WarEntity* entity);

// basic
void wcomm_move(WarContext* context, WarEntity* entity);
void wcomm_stop(WarContext* context, WarEntity* entity);
void wcomm_harvest(WarContext* context, WarEntity* entity);
void wcomm_deliver(WarContext* context, WarEntity* entity);
void wcomm_repair(WarContext* context, WarEntity* entity);
void wcomm_attack(WarContext* context, WarEntity* entity);
void wcomm_buildBasic(WarContext* context, WarEntity* entity);
void wcomm_buildAdvanced(WarContext* context, WarEntity* entity);

// build buildings
void wcomm_buildFarmHumans(WarContext* context, WarEntity* entity);
void wcomm_buildFarmOrcs(WarContext* context, WarEntity* entity);
void wcomm_buildBarracksHumans(WarContext* context, WarEntity* entity);
void wcomm_buildBarracksOrcs(WarContext* context, WarEntity* entity);
void wcomm_buildChurch(WarContext* context, WarEntity* entity);
void wcomm_buildTemple(WarContext* context, WarEntity* entity);
void wcomm_buildTowerHumans(WarContext* context, WarEntity* entity);
void wcomm_buildTowerOrcs(WarContext* context, WarEntity* entity);
void wcomm_buildTownHallHumans(WarContext* context, WarEntity* entity);
void wcomm_buildTownHallOrcs(WarContext* context, WarEntity* entity);
void wcomm_buildLumbermillHumans(WarContext* context, WarEntity* entity);
void wcomm_buildLumbermillOrcs(WarContext* context, WarEntity* entity);
void wcomm_buildStable(WarContext* context, WarEntity* entity);
void wcomm_buildKennel(WarContext* context, WarEntity* entity);
void wcomm_buildBlacksmithHumans(WarContext* context, WarEntity* entity);
void wcomm_buildBlacksmithOrcs(WarContext* context, WarEntity* entity);
void wcomm_buildWall(WarContext* context, WarEntity* entity);
void wcomm_buildRoad(WarContext* context, WarEntity* entity);

// spells
void wcomm_castRainOfFire(WarContext* context, WarEntity* entity);
void wcomm_castPoisonCloud(WarContext* context, WarEntity* entity);
void wcomm_castHeal(WarContext* context, WarEntity* entity);
void wcomm_castFarSight(WarContext* context, WarEntity* entity);
void wcomm_castDarkVision(WarContext* context, WarEntity* entity);
void wcomm_castInvisibility(WarContext* context, WarEntity* entity);
void wcomm_castUnHolyArmor(WarContext* context, WarEntity* entity);
void wcomm_castRaiseDead(WarContext* context, WarEntity* entity);

// summons
void wcomm_summonSpider(WarContext* context, WarEntity* entity);
void wcomm_summonScorpion(WarContext* context, WarEntity* entity);
void wcomm_summonDaemon(WarContext* context, WarEntity* entity);
void wcomm_summonWaterElemental(WarContext* context, WarEntity* entity);
