#pragma once

#include "war_types.h"

#include "war_database.h"

WarResource* getOrCreateResource(WarContext* context, s32 index);
void loadResource(WarContext* context, DatabaseEntry* entry);
