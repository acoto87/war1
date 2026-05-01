#pragma once

#include <stdbool.h>

#include "shl/list.h"
#include "shl/map.h"

#include "common.h"

#define SIGN(x) ((x) < 0 ? -1 : 1)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, a, b) (MAX(MIN(x, b), a))
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define PERCENTF(x) ((x) * 100)
#define PERCENTI(x) ((s32)PERCENTF(x))
#define PERCENTF01(a, b) ((f32)(a)/(b))
#define PERCENTABF(a, b) PERCENTF(PERCENTF01(a, b))
#define PERCENTABI(a, b) PERCENTI(PERCENTF01(a, b))

#define PI 3.14159265358979323846264338327f

#define RAD2DEG(x) ((x) * 180 / PI)
#define DEG2RAD(x) ((x) * PI / 180)

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

#define VEC2_IS_ZERO(v) ((v).x == 0.0f && (v).y == 0.0f)
#define VEC2_IS_ONE(v) ((v).x == 1.0f && (v).y == 1.0f)

vec2 vec2f(f32 x, f32 y);
vec2 vec2i(s32 x, s32 y);
vec2 vec2_addv(vec2 a, vec2 b);
vec2 vec2_addi(vec2 v, s32 x);
vec2 vec2_addf(vec2 v, f32 x);
vec2 vec2_subv(vec2 a, vec2 b);
vec2 vec2_subi(vec2 v, s32 x);
vec2 vec2_subf(vec2 v, f32 x);
vec2 vec2_mulf(vec2 v, f32 a);
vec2 vec2_muli(vec2 v, s32 a);
vec2 vec2_mulv(vec2 a, vec2 b);
vec2 vec2_half(vec2 a);
vec2 vec2_translatef(vec2 v, f32 x, f32 y);
vec2 vec2_translatei(vec2 v, s32 x, s32 y);
vec2 vec2_scalef(vec2 v, f32 scale);
vec2 vec2_scalei(vec2 v, s32 scale);
vec2 vec2_scalev(vec2 v, vec2 scale);
vec2 vec2_inverse(vec2 v);
f32 vec2_lengthSqr(vec2 v);
f32 vec2_length(vec2 v);
f32 vec2_distanceSqr(vec2 v1, vec2 v2);
f32 vec2_distance(vec2 v1, vec2 v2);
f32 vec2_distanceInTiles(vec2 v1, vec2 v2);
vec2 vec2_normalize(vec2 v);
f32 vec2_dot(vec2 v1, vec2 v2);
f32 vec2_determinant(vec2 v1, vec2 v2);
s32 vec2_orientation(vec2 v1, vec2 v2);
f32 vec2_angle(vec2 v1, vec2 v2);
f32 vec2_angleClockwise(vec2 v1, vec2 v2);
vec2 vec2_clampf(vec2 v, f32 a, f32 b);
vec2 vec2_clampi(vec2 v, s32 a, s32 b);
vec2 vec2_clampv(vec2 v, vec2 a, vec2 b);
vec2 vec2_floor(vec2 v);
vec2 vec2_ceil(vec2 v);
vec2 vec2_round(vec2 v);
void vec2_print(vec2 v);

/*
 * rect types and functions
*/
typedef struct
{
    f32 x, y;
    f32 width, height;
} rect;

#define RECT_EMPTY ((rect){0.0f, 0.0f, 0.0f, 0.0f})
#define RECT_TOP_LEFT(r) vec2f(r.x, r.y)
#define RECT_TOP_RIGHT(r) vec2f(r.x + r.width, r.y)
#define RECT_BOTTOM_LEFT(r) vec2f(r.x, r.y + r.height)
#define RECT_BOTTOM_RIGHT(r) vec2f(r.x + r.width, r.y + r.height)
#define RECT_SIZE(r) vec2f(r.width, r.height)

rect rectf(f32 x, f32 y, f32 width, f32 height);
rect recti(s32 x, s32 y, s32 width, s32 height);
rect rectpf(f32 x1, f32 y1, f32 x2, f32 y2);
rect rectv(vec2 pos, vec2 size);
rect rects(vec2 size);
bool rect_containsf(rect r, f32 x, f32 y);
bool rect_intersects(rect r1, rect r2);
rect rect_scalef(rect r, f32 scale);
rect rect_translatef(rect r, f32 x, f32 y);
vec2 rect_center(rect r);
rect rect_expand(rect r, f32 dx, f32 dy);
vec2 get_closestPointOnRect(vec2 p, rect r);
void rect_print(rect r);

/*
 * shl list/map types
*/
#include "shl/list.h"
#include "shl/map.h"
#include "shl/wstr.h"

bool equalsS32(const s32 a, const s32 b);
bool compareS32(const s32 a, const s32 b);
bool equalsVec2(const vec2 v1, const vec2 v2);
bool equalsRect(const rect r1, const rect r2);

shlDeclareList(s32List, s32)
shlDeclareList(vec2List, vec2)
shlDeclareList(rectList, rect)
shlDeclareMap(StringViewMap, StringView, String)

#define s32ListDefaultOptions (s32ListOptions){0, equalsS32, NULL}
#define vec2ListDefaultOptions (vec2ListOptions){VEC2_ZERO, equalsVec2, NULL}
#define rectListDefaultOptions (rectListOptions){RECT_EMPTY, equalsRect, NULL}
#define StringViewMapDefaultOptions (StringViewMapOptions){(String){0}, wsv_hashFNV32, wsv_equals, wstr_free}
