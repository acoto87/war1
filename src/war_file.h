#pragma once

#include "shl/wstr.h"
#include "war.h"
#include "war_resources.h"

WarFile* wfile_loadWarFile(WarContext* context, StringView filePath);
