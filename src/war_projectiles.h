#pragma once

#include "war_types.h"

WarEntity* wproj_createProjectile(WarContext* context, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target);
