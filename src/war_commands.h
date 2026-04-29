#pragma once

#include "war_types.h"

bool wcmd_executeCommand(WarContext* context);

// train units
void wcmd_trainFootman(WarContext* context, WarEntity* entity);
void wcmd_trainGrunt(WarContext* context, WarEntity* entity);
void wcmd_trainPeasant(WarContext* context, WarEntity* entity);
void wcmd_trainPeon(WarContext* context, WarEntity* entity);
void wcmd_trainHumanCatapult(WarContext* context, WarEntity* entity);
void wcmd_trainOrcCatapult(WarContext* context, WarEntity* entity);
void wcmd_trainKnight(WarContext* context, WarEntity* entity);
void wcmd_trainRaider(WarContext* context, WarEntity* entity);
void wcmd_trainArcher(WarContext* context, WarEntity* entity);
void wcmd_trainSpearman(WarContext* context, WarEntity* entity);
void wcmd_trainConjurer(WarContext* context, WarEntity* entity);
void wcmd_trainWarlock(WarContext* context, WarEntity* entity);
void wcmd_trainCleric(WarContext* context, WarEntity* entity);
void wcmd_trainNecrolyte(WarContext* context, WarEntity* entity);

// upgrades
void wcmd_upgradeSwords(WarContext* context, WarEntity* entity);
void wcmd_upgradeAxes(WarContext* context, WarEntity* entity);
void wcmd_upgradeHumanShields(WarContext* context, WarEntity* entity);
void wcmd_upgradeOrcsShields(WarContext* context, WarEntity* entity);
void wcmd_upgradeArrows(WarContext* context, WarEntity* entity);
void wcmd_upgradeSpears(WarContext* context, WarEntity* entity);
void wcmd_upgradeHorses(WarContext* context, WarEntity* entity);
void wcmd_upgradeWolves(WarContext* context, WarEntity* entity);
void wcmd_upgradeScorpions(WarContext* context, WarEntity* entity);
void wcmd_upgradeSpiders(WarContext* context, WarEntity* entity);
void wcmd_upgradeRainOfFire(WarContext* context, WarEntity* entity);
void wcmd_upgradePoisonCloud(WarContext* context, WarEntity* entity);
void wcmd_upgradeWaterElemental(WarContext* context, WarEntity* entity);
void wcmd_upgradeDaemon(WarContext* context, WarEntity* entity);
void wcmd_upgradeHealing(WarContext* context, WarEntity* entity);
void wcmd_upgradeRaiseDead(WarContext* context, WarEntity* entity);
void wcmd_upgradeFarSight(WarContext* context, WarEntity* entity);
void wcmd_upgradeDarkVision(WarContext* context, WarEntity* entity);
void wcmd_upgradeInvisibility(WarContext* context, WarEntity* entity);
void wcmd_upgradeUnholyArmor(WarContext* context, WarEntity* entity);

// wcmd_cancel
void wcmd_cancel(WarContext* context, WarEntity* entity);

// basic
void wcmd_move(WarContext* context, WarEntity* entity);
void wcmd_stop(WarContext* context, WarEntity* entity);
void wcmd_harvest(WarContext* context, WarEntity* entity);
void wcmd_deliver(WarContext* context, WarEntity* entity);
void wcmd_repair(WarContext* context, WarEntity* entity);
void wcmd_attack(WarContext* context, WarEntity* entity);
void wcmd_buildBasic(WarContext* context, WarEntity* entity);
void wcmd_buildAdvanced(WarContext* context, WarEntity* entity);

// build buildings
void wcmd_buildFarmHumans(WarContext* context, WarEntity* entity);
void wcmd_buildFarmOrcs(WarContext* context, WarEntity* entity);
void wcmd_buildBarracksHumans(WarContext* context, WarEntity* entity);
void wcmd_buildBarracksOrcs(WarContext* context, WarEntity* entity);
void wcmd_buildChurch(WarContext* context, WarEntity* entity);
void wcmd_buildTemple(WarContext* context, WarEntity* entity);
void wcmd_buildTowerHumans(WarContext* context, WarEntity* entity);
void wcmd_buildTowerOrcs(WarContext* context, WarEntity* entity);
void wcmd_buildTownHallHumans(WarContext* context, WarEntity* entity);
void wcmd_buildTownHallOrcs(WarContext* context, WarEntity* entity);
void wcmd_buildLumbermillHumans(WarContext* context, WarEntity* entity);
void wcmd_buildLumbermillOrcs(WarContext* context, WarEntity* entity);
void wcmd_buildStable(WarContext* context, WarEntity* entity);
void wcmd_buildKennel(WarContext* context, WarEntity* entity);
void wcmd_buildBlacksmithHumans(WarContext* context, WarEntity* entity);
void wcmd_buildBlacksmithOrcs(WarContext* context, WarEntity* entity);
void wcmd_buildWall(WarContext* context, WarEntity* entity);
void wcmd_buildRoad(WarContext* context, WarEntity* entity);

// spells
void wcmd_castRainOfFire(WarContext* context, WarEntity* entity);
void wcmd_castPoisonCloud(WarContext* context, WarEntity* entity);
void wcmd_castHeal(WarContext* context, WarEntity* entity);
void wcmd_castFarSight(WarContext* context, WarEntity* entity);
void wcmd_castDarkVision(WarContext* context, WarEntity* entity);
void wcmd_castInvisibility(WarContext* context, WarEntity* entity);
void wcmd_castUnHolyArmor(WarContext* context, WarEntity* entity);
void wcmd_castRaiseDead(WarContext* context, WarEntity* entity);

// summons
void wcmd_summonSpider(WarContext* context, WarEntity* entity);
void wcmd_summonScorpion(WarContext* context, WarEntity* entity);
void wcmd_summonDaemon(WarContext* context, WarEntity* entity);
void wcmd_summonWaterElemental(WarContext* context, WarEntity* entity);
