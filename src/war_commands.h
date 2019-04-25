bool executeCommand(WarContext* context);

// train units
void trainFootman(WarContext* context, WarEntity* entity);
void trainGrunt(WarContext* context, WarEntity* entity);
void trainPeasant(WarContext* context, WarEntity* entity);
void trainPeon(WarContext* context, WarEntity* entity);
void trainHumanCatapult(WarContext* context, WarEntity* entity);
void trainOrcCatapult(WarContext* context, WarEntity* entity);
void trainKnight(WarContext* context, WarEntity* entity);
void trainRaider(WarContext* context, WarEntity* entity);
void trainArcher(WarContext* context, WarEntity* entity);
void trainSpearman(WarContext* context, WarEntity* entity);
void trainConjurer(WarContext* context, WarEntity* entity);
void trainWarlock(WarContext* context, WarEntity* entity);
void trainCleric(WarContext* context, WarEntity* entity);
void trainNecrolyte(WarContext* context, WarEntity* entity);

// upgrades
void upgradeSwords(WarContext* context, WarEntity* entity);
void upgradeAxes(WarContext* context, WarEntity* entity);
void upgradeHumanShields(WarContext* context, WarEntity* entity);
void upgradeOrcsShields(WarContext* context, WarEntity* entity);
void upgradeArrows(WarContext* context, WarEntity* entity);
void upgradeSpears(WarContext* context, WarEntity* entity);
void upgradeHorses(WarContext* context, WarEntity* entity);
void upgradeWolves(WarContext* context, WarEntity* entity);
void upgradeScorpions(WarContext* context, WarEntity* entity);
void upgradeSpiders(WarContext* context, WarEntity* entity);
void upgradeRainOfFire(WarContext* context, WarEntity* entity);
void upgradePoisonCloud(WarContext* context, WarEntity* entity);
void upgradeWaterElemental(WarContext* context, WarEntity* entity);
void upgradeDaemon(WarContext* context, WarEntity* entity);
void upgradeHealing(WarContext* context, WarEntity* entity);
void upgradeRaiseDead(WarContext* context, WarEntity* entity);
void upgradeFarSight(WarContext* context, WarEntity* entity);
void upgradeDarkVision(WarContext* context, WarEntity* entity);
void upgradeInvisibility(WarContext* context, WarEntity* entity);
void upgradeUnholyArmor(WarContext* context, WarEntity* entity);

// cancel
void cancelTrainOrUpgrade(WarContext* context, WarEntity* entity);

// basic
void move(WarContext* context, WarEntity* entity);
void stop(WarContext* context, WarEntity* entity);
void harvest(WarContext* context, WarEntity* entity);
void deliver(WarContext* context, WarEntity* entity);
void attack(WarContext* context, WarEntity* entity);
void buildBasic(WarContext* context, WarEntity* entity);
void buildAdvanced(WarContext* context, WarEntity* entity);

// build buildings
void buildFarmHumans(WarContext* context, WarEntity* entity);
void buildFarmOrcs(WarContext* context, WarEntity* entity);
void buildBarracksHumans(WarContext* context, WarEntity* entity);
void buildBarracksOrcs(WarContext* context, WarEntity* entity);
void buildChurch(WarContext* context, WarEntity* entity);
void buildTemple(WarContext* context, WarEntity* entity);
void buildTowerHumans(WarContext* context, WarEntity* entity);
void buildTowerOrcs(WarContext* context, WarEntity* entity);
void buildTownHallHumans(WarContext* context, WarEntity* entity);
void buildTownHallOrcs(WarContext* context, WarEntity* entity);
void buildLumbermillHumans(WarContext* context, WarEntity* entity);
void buildLumbermillOrcs(WarContext* context, WarEntity* entity);
void buildStable(WarContext* context, WarEntity* entity);
void buildKennel(WarContext* context, WarEntity* entity);
void buildBlacksmithHumans(WarContext* context, WarEntity* entity);
void buildBlacksmithOrcs(WarContext* context, WarEntity* entity);
