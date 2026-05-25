#pragma once

#include "shl/wstr.h"

#include "war.h"
#include "war_resources.h"
#include "war_fwd.h"

struct _WarMapFileHeader
{
    u32 magic;
    u16 version;
    u16 flags;
    u32 reserved[2];
};

WarFile* wfile_loadWarFile(WarContext* context, StringView filePath);
bool wfile_loadWarMapFile(StringView filePath, WarResource* levelInfoRes, WarResource* visualInfoRes, WarResource* passableInfoRes);
