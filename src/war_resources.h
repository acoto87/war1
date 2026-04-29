#pragma once

#include "war_types.h"

#include "war_database.h"

WarResource* wres_getOrCreateResource(WarContext* context, s32 index);
void wres_loadResource(WarContext* context, DatabaseEntry* entry);
