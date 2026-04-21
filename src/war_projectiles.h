#pragma once

#include "war_types.h"

WarEntity* createProjectile(WarContext* context, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target);
