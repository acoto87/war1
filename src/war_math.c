#pragma once

#include "war_math.h"

vec2 vec2f(f32 x, f32 y)
{
    return (vec2){x, y};
}

vec2 vec2i(s32 x, s32 y)
{
    return (vec2){(f32)x, (f32)y};
}

vec2 vec2_addv(vec2 a, vec2 b)
{
    return (vec2){a.x + b.x, a.y + b.y};
}

vec2 vec2_addi(vec2 v, s32 x)
{
    return (vec2){v.x + x, v.y + x};
}

vec2 vec2_addf(vec2 v, f32 x)
{
    return (vec2){v.x + x, v.y + x};
}

vec2 vec2_subv(vec2 a, vec2 b)
{
    return (vec2){a.x - b.x, a.y - b.y};
}

vec2 vec2_subi(vec2 v, s32 x)
{
    return (vec2){v.x - x, v.y - x};
}

vec2 vec2_subf(vec2 v, f32 x)
{
    return (vec2){v.x - x, v.y - x};
}

vec2 vec2_mulf(vec2 v, f32 a)
{
    return (vec2){v.x * a, v.y * a};
}

vec2 vec2_muli(vec2 v, s32 a)
{
    return (vec2){v.x * (f32)a, v.y * (f32)a};
}

vec2 vec2_mulv(vec2 a, vec2 b)
{
    return (vec2){a.x * b.x, a.y * b.y};
}

vec2 vec2_half(vec2 a)
{
    return (vec2){a.x * 0.5f, a.y * 0.5f};
}

vec2 vec2_translatef(vec2 v, f32 x, f32 y)
{
    return (vec2){v.x + x, v.y + y};
}

vec2 vec2_translatei(vec2 v, s32 x, s32 y)
{
    return (vec2){v.x + (f32)x, v.y + (f32)y};
}

vec2 vec2_scalef(vec2 v, f32 scale)
{
    return (vec2){v.x * scale, v.y * scale};
}

vec2 vec2_scalei(vec2 v, s32 scale)
{
    return (vec2){v.x * (f32)scale, v.y * (f32)scale};
}

vec2 vec2_scalev(vec2 v, vec2 scale)
{
    return (vec2){v.x * scale.x, v.y * scale.y};
}

vec2 vec2_inverse(vec2 v)
{
    return (vec2){-v.x, -v.y};
}

f32 vec2_lengthSqr(vec2 v)
{
    return v.x * v.x + v.y * v.y;
}

f32 vec2_length(vec2 v)
{
    return sqrtf(vec2_lengthSqr(v));
}

f32 vec2_distanceSqr(vec2 v1, vec2 v2)
{
    f32 xx = (v1.x - v2.x);
    f32 yy = (v1.y - v2.y);
    return xx * xx + yy * yy;
}

f32 vec2_distance(vec2 v1, vec2 v2)
{
    return sqrtf(vec2_distanceSqr(v1, v2));
}

f32 vec2_distanceInTiles(vec2 v1, vec2 v2)
{
    vec2 diff = vec2_subv(v1, v2);
    return MAX(ABS(diff.x), ABS(diff.y));
}

vec2 vec2_normalize(vec2 v)
{
    f32 len = vec2_length(v);
    return len != 0 ? vec2_scalef(v, 1 / len) : VEC2_ZERO;
}

f32 vec2_dot(vec2 v1, vec2 v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

f32 vec2_determinant(vec2 v1, vec2 v2)
{
    return v1.x * v2.x - v1.y * v2.y;
}

s32 vec2_orientation(vec2 v1, vec2 v2)
{
    return v1.x * v2.y - v1.y * v2.x >= 0 ? 1 : -1;
}

f32 vec2_angle(vec2 v1, vec2 v2)
{
    f32 v1Length = vec2_length(v1);
    f32 v2Length = vec2_length(v2);
    if (v1Length == 0 || v2Length == 0)
        return 0;

    f32 dot = vec2_dot(v1, v2);
    f32 angleRad = acosf(dot / (v1Length * v2Length));
    return RAD2DEG(angleRad);
}

f32 vec2_angleClockwise(vec2 v1, vec2 v2)
{
    f32 v1Length = vec2_length(v1);
    f32 v2Length = vec2_length(v2);
    if (v1Length == 0 || v2Length == 0)
        return 0;

    s32 orientation = vec2_orientation(v1, v2);

    f32 dot = vec2_dot(v1, v2);
    f32 angleRad = acosf(dot / (v1Length * v2Length));

    // if the two vectors are in counter-clockwise orientation
    // take the larger angle between the two vectors
    if (orientation < 0)
        angleRad = 2 * PI - angleRad;

    return RAD2DEG(angleRad);
}

vec2 vec2_clampf(vec2 v, f32 a, f32 b)
{
    return (vec2){CLAMP(v.x, a, b), CLAMP(v.y, a, b)};
}

vec2 vec2_clampi(vec2 v, s32 a, s32 b)
{
    return (vec2){CLAMP(v.x, (f32)a, (f32)b), CLAMP(v.y, (f32)a, (f32)b)};
}

vec2 vec2_clampv(vec2 v, vec2 a, vec2 b)
{
    return (vec2){CLAMP(v.x, a.x, b.x), CLAMP(v.y, a.y, b.y)};
}

vec2 vec2_floor(vec2 v)
{
    return (vec2){floorf(v.x), floorf(v.y)};
}

vec2 vec2_ceil(vec2 v)
{
    return (vec2){ceilf(v.x), ceilf(v.y)};
}

vec2 vec2_round(vec2 v)
{
    return (vec2){roundf(v.x), roundf(v.y)};
}

void vec2_print(vec2 v)
{
    logDebug("(%f, %f)", v.x, v.y);
}

rect rectf(f32 x, f32 y, f32 width, f32 height)
{
    return (rect){x, y, width, height};
}

rect recti(s32 x, s32 y, s32 width, s32 height)
{
    return (rect){(f32)x, (f32)y, (f32)width, (f32)height};
}

rect rectpf(f32 x1, f32 y1, f32 x2, f32 y2)
{
    return rectf(MIN(x1, x2), MIN(y1, y2), ABS(x1 - x2), ABS(y1 - y2));
}

rect rectv(vec2 pos, vec2 size)
{
    return (rect){pos.x, pos.y, size.x, size.y};
}

rect rects(vec2 size)
{
    return (rect){0.0f, 0.0f, size.x, size.y};
}

bool rect_containsf(rect r, f32 x, f32 y)
{
    return x >= r.x && x <= r.x + r.width &&
           y >= r.y && y <= r.y + r.height;
}

bool rect_intersects(rect r1, rect r2)
{
    return !(r1.x + r1.width < r2.x || r1.x > r2.x + r2.width ||
             r1.y + r1.height < r2.y || r1.y > r2.y + r2.height);
}

rect rect_scalef(rect r, f32 scale)
{
    r.x *= scale;
    r.y *= scale;
    r.width *= scale;
    r.height *= scale;
    return r;
}

rect rect_translatef(rect r, f32 x, f32 y)
{
    r.x += x;
    r.y += y;
    return r;
}

vec2 rect_center(rect r)
{
    return vec2f(r.x + 0.5f * r.width, r.y + 0.5f * r.height);
}

rect rect_expand(rect r, f32 dx, f32 dy)
{
    r.x -= dx;
    r.y -= dy;
    r.width += dx * 2;
    r.height += dy * 2;
    return r;
}

vec2 get_closestPointOnRect(vec2 p, rect r)
{
    f32 left = r.x;
    f32 top = r.y;
    f32 right = r.x + r.width - 1;
    f32 bottom = r.y + r.height - 1;

    // top-left
    if (p.x < left && p.y < top)
        return vec2f(left, top);

    // top-center
    if (p.x >= left && p.x <= right && p.y < top)
        return vec2f(p.x, top);

    // top-right
    if (p.x > right && p.y < top)
        return vec2f(right, top);

    // middle-right
    if (p.x > right && p.y >= top && p.y <= bottom)
        return vec2f(right, p.y);

    // bottom-right
    if (p.x > right && p.y > bottom)
        return vec2f(right, bottom);

    // bottom-center
    if (p.x >= left && p.x <= right && p.y > bottom)
        return vec2f(p.x, bottom);

    // bottom-left
    if (p.x < left && p.y > bottom)
        return vec2f(left, bottom);

    // middle-left
    if (p.x < left && p.y >= top && p.y <= bottom)
        return vec2f(left, p.y);

    // the point is inside the rect
    return p;
}

void rect_print(rect r)
{
    logDebug("(%f, %f, %f, %f)", r.x, r.y, r.width, r.height);
}

bool equalsS32(const s32 a, const s32 b)
{
    return a == b;
}

bool compareS32(const s32 a, const s32 b)
{
    return a - b;
}

shlDefineList(s32List, s32)

bool equalsVec2(const vec2 v1, const vec2 v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

shlDefineList(vec2List, vec2)

bool equalsRect(const rect r1, const rect r2)
{
    return r1.x == r2.x && r1.y == r2.y &&
           r1.width == r2.width && r1.height == r2.height;
}

shlDefineList(rectList, rect)

shlDefineMap(StringViewMap, StringView, String)
