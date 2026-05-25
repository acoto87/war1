#pragma once

#include "war_editor.h"

bool wecfg_load(WarEditorContext* ctx);
bool wecfg_save(WarEditorContext* ctx);
void wecfg_addRecentFile(WarEditorContext* ctx, const char* path);