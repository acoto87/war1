#pragma once

#include "common.h"
#include "war.h"
#include "war_math.h"
#include "war_units.h"

WarEntity* wproj_createProjectile(WarContext* context, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target);
