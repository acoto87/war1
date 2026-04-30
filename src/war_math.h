#pragma once

#include <stdbool.h>

#include "common.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define sign(x) ((x) < 0 ? -1 : 1)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp(x, a, b) (max(min(x, b), a))
#define abs(x) ((x) < 0 ? -(x) : (x))

#define halfi(x) ((x) / 2)
#define halff(x) ((x) * 0.5f)

#define percentf(x) ((x) * 100)
#define percenti(x) ((s32)percentf(x))
#define percentabf01(a, b) ((f32)(a)/(b))
#define percentabf(a, b) percentf(percentabf01(a, b))
#define percentabi(a, b) percenti(percentabf01(a, b))

#define PI 3.14159265358979323846264338327f

#define rad2Deg(x) ((x) * 180 / PI)
#define deg2Rad(x) ((x) * PI / 180)

/*
 * vec2 types and functions
*/
typedef struct
{
    f32 x, y;
} vec2;

#define VEC2_ZERO ((vec2){0.0f, 0.0f})
#define VEC2_ONE ((vec2){1.0f, 1.0f})
#define VEC2_LEFT ((vec2){-1.0f, 0.0 })
#define VEC2_UP ((vec2){0.0f, -1.0 })
#define VEC2_RIGHT ((vec2){1.0f, 0.0f})
#define VEC2_DOWN ((vec2){0.0f, 1.0f})

#define vec2IsZero(v) ((v).x == 0.0f && (v).y == 0.0f)
#define vec2IsOne(v) ((v).x == 1.0f && (v).y == 1.0f)

vec2 vec2f(f32 x, f32 y);
vec2 vec2i(s32 x, s32 y);
vec2 vec2Addv(vec2 a, vec2 b);
vec2 vec2Addi(vec2 v, s32 x);
vec2 vec2Addf(vec2 v, f32 x);
vec2 vec2Subv(vec2 a, vec2 b);
vec2 vec2Subi(vec2 v, s32 x);
vec2 vec2Subf(vec2 v, f32 x);
vec2 vec2Mulf(vec2 v, f32 a);
vec2 vec2Muli(vec2 v, s32 a);
vec2 vec2Mulv(vec2 a, vec2 b);
vec2 vec2Half(vec2 a);
vec2 vec2Translatef(vec2 v, f32 x, f32 y);
vec2 vec2Translatei(vec2 v, s32 x, s32 y);
vec2 vec2Scalef(vec2 v, f32 scale);
vec2 vec2Scalei(vec2 v, s32 scale);
vec2 vec2Scalev(vec2 v, vec2 scale);
vec2 vec2Inverse(vec2 v);
f32 vec2LengthSqr(vec2 v);
f32 vec2Length(vec2 v);
f32 vec2DistanceSqr(vec2 v1, vec2 v2);
f32 vec2Distance(vec2 v1, vec2 v2);
f32 vec2DistanceInTiles(vec2 v1, vec2 v2);
vec2 vec2Normalize(vec2 v);
f32 vec2Dot(vec2 v1, vec2 v2);
f32 vec2Determinant(vec2 v1, vec2 v2);
s32 vec2Orientation(vec2 v1, vec2 v2);
f32 vec2Angle(vec2 v1, vec2 v2);
f32 vec2ClockwiseAngle(vec2 v1, vec2 v2);
vec2 vec2Clampf(vec2 v, f32 a, f32 b);
vec2 vec2Clampi(vec2 v, s32 a, s32 b);
vec2 vec2Clampv(vec2 v, vec2 a, vec2 b);
vec2 vec2Floor(vec2 v);
vec2 vec2Ceil(vec2 v);
vec2 vec2Round(vec2 v);
void vec2Print(vec2 v);

/*
 * rect types and functions
*/
typedef struct
{
    f32 x, y;
    f32 width, height;
} rect;

#define RECT_EMPTY ((rect){0.0f, 0.0f, 0.0f, 0.0f})
#define rectTopLeft(r) vec2f(r.x, r.y)
#define recttopRight(r) vec2f(r.x + r.width, r.y)
#define rectBottomLeft(r) vec2f(r.x, r.y + r.height)
#define rectBottomRight(r) vec2f(r.x + r.width, r.y + r.height)
#define rectSize(r) vec2f(r.width, r.height)

rect rectf(f32 x, f32 y, f32 width, f32 height);
rect recti(s32 x, s32 y, s32 width, s32 height);
rect rectpf(f32 x1, f32 y1, f32 x2, f32 y2);
rect rectv(vec2 pos, vec2 size);
rect rects(vec2 size);
bool rectContainsf(rect r, f32 x, f32 y);
bool rectIntersects(rect r1, rect r2);
rect rectScalef(rect r, f32 scale);
rect rectTranslatef(rect r, f32 x, f32 y);
vec2 rectCenter(rect r);
rect rectExpand(rect r, f32 dx, f32 dy);
vec2 getClosestPointOnRect(vec2 p, rect r);
void rectPrint(rect r);

/*
 * shl list/map types
*/
#include "shl/list.h"
#include "shl/map.h"
#include "shl/wstr.h"

bool wt_equalsS32(const s32 a, const s32 b);
bool wt_compareS32(const s32 a, const s32 b);

shlDeclareList(s32List, s32)

#define s32ListDefaultOptions (s32ListOptions){0, wt_equalsS32, NULL}

bool wt_equalsVec2(const vec2 v1, const vec2 v2);

shlDeclareList(vec2List, vec2)

#define vec2ListDefaultOptions (vec2ListOptions){VEC2_ZERO, wt_equalsVec2, NULL}

bool wt_equalsRect(const rect r1, const rect r2);

shlDeclareList(rectList, rect)

#define rectListDefaultOptions (rectListOptions){RECT_EMPTY, wt_equalsRect, NULL}

shlDeclareMap(StringViewMap, StringView, String)

#define StringViewMapDefaultOptions (StringViewMapOptions){(String){0}, wsv_hashFNV32, wsv_equals, wstr_free}
