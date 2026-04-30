#pragma once

#include "common.h"
#include "war_fwd.h"
#include "war_units.h"

enum _WarUnitCommandType
{
    WAR_COMMAND_NONE, // 0

    // unit commands
    WAR_COMMAND_MOVE, // 1
    WAR_COMMAND_STOP,
    WAR_COMMAND_HARVEST,
    WAR_COMMAND_DELIVER,
    WAR_COMMAND_REPAIR,
    WAR_COMMAND_ATTACK,

    // train commands
    WAR_COMMAND_TRAIN_FOOTMAN, // 7
    WAR_COMMAND_TRAIN_GRUNT,
    WAR_COMMAND_TRAIN_PEASANT,
    WAR_COMMAND_TRAIN_PEON,
    WAR_COMMAND_TRAIN_CATAPULT_HUMANS,
    WAR_COMMAND_TRAIN_CATAPULT_ORCS,
    WAR_COMMAND_TRAIN_KNIGHT,
    WAR_COMMAND_TRAIN_RAIDER,
    WAR_COMMAND_TRAIN_ARCHER,
    WAR_COMMAND_TRAIN_SPEARMAN,
    WAR_COMMAND_TRAIN_CONJURER,
    WAR_COMMAND_TRAIN_WARLOCK,
    WAR_COMMAND_TRAIN_CLERIC,
    WAR_COMMAND_TRAIN_NECROLYTE,

    // spell commands
    WAR_COMMAND_SPELL_HEALING, // 21
    WAR_COMMAND_SPELL_FAR_SIGHT,
    WAR_COMMAND_SPELL_INVISIBILITY,
    WAR_COMMAND_SPELL_RAIN_OF_FIRE,
    WAR_COMMAND_SPELL_POISON_CLOUD,
    WAR_COMMAND_SPELL_RAISE_DEAD,
    WAR_COMMAND_SPELL_DARK_VISION,
    WAR_COMMAND_SPELL_UNHOLY_ARMOR,

    // summons
    WAR_COMMAND_SUMMON_SPIDER, // 29
    WAR_COMMAND_SUMMON_SCORPION,
    WAR_COMMAND_SUMMON_DAEMON,
    WAR_COMMAND_SUMMON_WATER_ELEMENTAL,

    // build commands
    WAR_COMMAND_BUILD_BASIC, // 33
    WAR_COMMAND_BUILD_ADVANCED,
    WAR_COMMAND_BUILD_FARM_HUMANS,
    WAR_COMMAND_BUILD_FARM_ORCS,
    WAR_COMMAND_BUILD_BARRACKS_HUMANS,
    WAR_COMMAND_BUILD_BARRACKS_ORCS,
    WAR_COMMAND_BUILD_CHURCH,
    WAR_COMMAND_BUILD_TEMPLE,
    WAR_COMMAND_BUILD_TOWER_HUMANS,
    WAR_COMMAND_BUILD_TOWER_ORCS,
    WAR_COMMAND_BUILD_TOWNHALL_HUMANS,
    WAR_COMMAND_BUILD_TOWNHALL_ORCS,
    WAR_COMMAND_BUILD_LUMBERMILL_HUMANS,
    WAR_COMMAND_BUILD_LUMBERMILL_ORCS,
    WAR_COMMAND_BUILD_STABLE,
    WAR_COMMAND_BUILD_KENNEL,
    WAR_COMMAND_BUILD_BLACKSMITH_HUMANS,
    WAR_COMMAND_BUILD_BLACKSMITH_ORCS,
    WAR_COMMAND_BUILD_ROAD,
    WAR_COMMAND_BUILD_WALL,

    // upgrades
    WAR_COMMAND_UPGRADE_SWORDS, // 53
    WAR_COMMAND_UPGRADE_AXES,
    WAR_COMMAND_UPGRADE_SHIELD_HUMANS,
    WAR_COMMAND_UPGRADE_SHIELD_ORCS,
    WAR_COMMAND_UPGRADE_ARROWS,
    WAR_COMMAND_UPGRADE_SPEARS,
    WAR_COMMAND_UPGRADE_HORSES,
    WAR_COMMAND_UPGRADE_WOLVES,
    WAR_COMMAND_UPGRADE_SCORPION,
    WAR_COMMAND_UPGRADE_SPIDER,
    WAR_COMMAND_UPGRADE_RAIN_OF_FIRE,
    WAR_COMMAND_UPGRADE_POISON_CLOUD,
    WAR_COMMAND_UPGRADE_WATER_ELEMENTAL,
    WAR_COMMAND_UPGRADE_DAEMON,
    WAR_COMMAND_UPGRADE_HEALING,
    WAR_COMMAND_UPGRADE_RAISE_DEAD,
    WAR_COMMAND_UPGRADE_FAR_SIGHT,
    WAR_COMMAND_UPGRADE_DARK_VISION,
    WAR_COMMAND_UPGRADE_INVISIBILITY,
    WAR_COMMAND_UPGRADE_UNHOLY_ARMOR,

    // wcmd_cancel
    WAR_COMMAND_CANCEL // 73
};

struct _WarUnitCommand
{
    WarUnitCommandType type;

    union
    {
        struct
        {
            WarUnitType unitToTrain;
            WarUnitType buildingUnit;
        } train;

        struct
        {
            WarUpgradeType upgradeToBuild;
            WarUnitType buildingUnit;
        } upgrade;

        struct
        {
            WarUnitType buildingToBuild;
        } build;
    };
};

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
void move(WarContext* context, WarEntity* entity);
void wcmd_stop(WarContext* context, WarEntity* entity);
void wcmd_harvest(WarContext* context, WarEntity* entity);
void deliver(WarContext* context, WarEntity* entity);
void repair(WarContext* context, WarEntity* entity);
void attack(WarContext* context, WarEntity* entity);
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
