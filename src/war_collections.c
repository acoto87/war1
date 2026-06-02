#include "war_collections.h"

bool equalsS32(const s32 a, const s32 b)
{
    return a == b;
}

bool compareS32(const s32 a, const s32 b)
{
    return a - b;
}

bool equalsVec2(const vec2 v1, const vec2 v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

shlDefineList(S32List, s32)
shlDefineList(Vec2List, vec2)
shlDefineMap(StringViewMap, StringView, String)
