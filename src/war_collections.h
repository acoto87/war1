#pragma once

#include "shl/list.h"
#include "shl/map.h"
#include "shl/wstr.h"

#include "war_common.h"
#include "war_math.h"

bool equalsS32(const s32 a, const s32 b);
bool compareS32(const s32 a, const s32 b);
bool equalsVec2(const vec2 v1, const vec2 v2);

shlDeclareList(S32List, s32)
shlDeclareList(Vec2List, vec2)
shlDeclareMap(StringViewMap, StringView, String)
