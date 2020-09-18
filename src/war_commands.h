#pragma once

WarCommand* createCommand(WarContext* context, WarPlayerInfo* player, WarCommandType type);

WarCommand* createTrainCommand(WarContext* context, WarPlayerInfo* player, WarUnitType unitType, WarEntityId buildingId);
WarCommand* createBuildCommand(WarContext* context, WarPlayerInfo* player, WarUnitType unitType, WarEntityId workerId, vec2 position);
WarCommand* createBuildWallCommand(WarContext* context, WarPlayerInfo* player, vec2 position);
WarCommand* createBuildRoadCommand(WarContext* context, WarPlayerInfo* player, vec2 position);
WarCommand* createUpgradeCommand(WarContext* context, WarPlayerInfo* player, WarUpgradeType upgradeType, WarEntityId buildingId);
WarCommand* createMoveCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, vec2 position);
WarCommand* createSquadMoveCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, vec2 position);
WarCommand* createFollowCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, WarEntityId targetEntityId);
WarCommand* createSquadFollowCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, WarEntityId targetEntityId);
WarCommand* createAttackEnemyCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, WarEntityId targetEntityId);
WarCommand* createAttackPositionCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, vec2 position);
WarCommand* createSquadAttackEnemyCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, WarEntityId targetEntityId);
WarCommand* createSquadAttackPositionCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, vec2 position);
WarCommand* createStopCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup);
WarCommand* createSquadStopCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId);
WarCommand* createGatherGoldCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, WarEntityId targetEntityId);
WarCommand* createGatherWoodCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, WarEntityId targetEntityId, vec2 targetTile);
WarCommand* createDeliverCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, WarEntityId targetEntityId);
WarCommand* createRepairCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, WarEntityId targetEntityId);
WarCommand* createCastCommand(WarContext* context, WarPlayerInfo* player, WarUnitGroup unitGroup, WarSpellType spellType, WarEntityId targetEntityId, vec2 position);
WarCommand* createSquadCastCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, WarSpellType spellType, WarEntityId targetEntityId, vec2 position);
WarCommand* createSquadCommand(WarContext* context, WarPlayerInfo* player, WarSquadId squadId, WarUnitGroup unitGroup);
WarCommand* createWaitCommand(WarContext* context, WarPlayerInfo* player, WarCommandId commandId, WarResourceKind resource, s32 amount);
WarCommand* createSleepCommand(WarContext* context, WarPlayerInfo* player, f32 time);
WarCommand* createCancelCommand(WarContext* context, WarPlayerInfo* player, WarEntityId targetEntityId);

WarCommandStatus canExecuteCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command);
WarCommandStatus executeCommand(WarContext* context, WarPlayerInfo* player, WarCommand* command);

char* commandTypeToString(WarCommandType type);
