#define sign(x) \
  ({ __typeof__(x) _x = (x); \
     (__typeof__(x))(_x < 0 ? -1 : 1); })

#define min(a, b) \
  ({ __typeof__(a) _a = (a); \
     __typeof__(b) _b = (b); \
     _a < _b ? _a : _b; })

#define max(a, b) \
  ({ __typeof__(a) _a = (a); \
     __typeof__(b) _b = (b); \
     _a > _b ? _a : _b; })

#define clamp(x, a, b) \
    max(min(x, b), a)

#define abs(x) \
	({ __typeof__(x) _x = (x); \
       _x < 0 ? -_x : _x; })

#define halfi(x) ((x) / 2)
#define halff(x) ((x) * 0.5f)

#define percentf(x) ((x) * 100)
#define percenti(x) ((s32)percentf(x))
#define percentabf01(a, b) ((f32)(a)/(b))
#define percentabf(a, b) percentf(percentabf01(a, b))
#define percentabi(a, b) percenti(percentabf01(a, b))

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

vec2 vec2f(f32 x, f32 y)
{
    return (vec2){x, y};
}

vec2 vec2i(s32 x, s32 y)
{
    return (vec2){(f32)x, (f32)y};
}

vec2 vec2Addv(vec2 a, vec2 b)
{
    return (vec2){a.x + b.x, a.y + b.y};
}

vec2 vec2Addi(vec2 v, s32 x)
{
    return (vec2){v.x + x, v.y + x};
}

vec2 vec2Addf(vec2 v, f32 x)
{
    return (vec2){v.x + x, v.y + x};
}

vec2 vec2Subv(vec2 a, vec2 b)
{
    return (vec2){a.x - b.x, a.y - b.y};
}

vec2 vec2Subi(vec2 v, s32 x)
{
    return (vec2){v.x - x, v.y - x};
}

vec2 vec2Subf(vec2 v, f32 x)
{
    return (vec2){v.x - x, v.y - x};
}

vec2 vec2Mulf(vec2 v, f32 a)
{
    return (vec2){v.x * a, v.y * a};
}

vec2 vec2Muli(vec2 v, s32 a)
{
    return (vec2){v.x * (f32)a, v.y * (f32)a};
}

vec2 vec2Mulv(vec2 a, vec2 b)
{
    return (vec2){a.x * b.x, a.y * b.y};
}

vec2 vec2Half(vec2 a)
{
    return (vec2){a.x * 0.5f, a.y * 0.5f};
}

vec2 vec2Translatef(vec2 v, f32 x, f32 y)
{
    return (vec2){v.x + x, v.y + y};
}

vec2 vec2Translatei(vec2 v, s32 x, s32 y)
{
    return (vec2){v.x + (f32)x, v.y + (f32)y};
}

vec2 vec2Scalef(vec2 v, f32 scale)
{
    return (vec2){v.x * scale, v.y * scale};
}

vec2 vec2Scalei(vec2 v, s32 scale)
{
    return (vec2){v.x * (f32)scale, v.y * (f32)scale};
}

vec2 vec2Scalev(vec2 v, vec2 scale)
{
    return (vec2){v.x * scale.x, v.y * scale.y};
}

vec2 vec2Inverse(vec2 v)
{
    return (vec2){-v.x, -v.y};
}

f32 vec2LengthSqr(vec2 v)
{
    return v.x * v.x + v.y * v.y;
}

f32 vec2Length(vec2 v)
{
    return sqrtf(vec2LengthSqr(v));
}

f32 vec2DistanceSqr(vec2 v1, vec2 v2)
{
    f32 xx = (v1.x - v2.x);
    f32 yy = (v1.y - v2.y);
    return xx * xx + yy * yy;
}

f32 vec2Distance(vec2 v1, vec2 v2)
{
    return sqrtf(vec2DistanceSqr(v1, v2));
}

f32 vec2DistanceInTiles(vec2 v1, vec2 v2)
{
    vec2 diff = vec2Subv(v1, v2);
    return max(abs(diff.x), abs(diff.y));
}

vec2 vec2Normalize(vec2 v)
{
    f32 len = vec2Length(v);
    return len != 0 ? vec2Scalef(v, 1 / len) : VEC2_ZERO;
}

vec2 vec2Clampf(vec2 v, f32 a, f32 b)
{
    return (vec2){clamp(v.x, a, b), clamp(v.y, a, b)};
}

vec2 vec2Clampi(vec2 v, s32 a, s32 b)
{
    return (vec2){clamp(v.x, (f32)a, (f32)b), clamp(v.y, (f32)a, (f32)b)};
}

vec2 vec2Clampv(vec2 v, vec2 a, vec2 b)
{
    return (vec2){clamp(v.x, a.x, b.x), clamp(v.y, a.y, b.y)};
}

vec2 vec2Floor(vec2 v)
{
    return (vec2){floorf(v.x), floorf(v.y)};
}

vec2 vec2Ceil(vec2 v)
{
    return (vec2){ceilf(v.x), ceilf(v.y)};
}

vec2 vec2Round(vec2 v)
{
    return (vec2){roundf(v.x), roundf(v.y)};
}

void vec2Print(vec2 v)
{
    logDebug("(%f, %f)\n", v.x, v.y);
}

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
    return rectf(min(x1, x2), min(y1, y2), abs(x1 - x2), abs(y1 - y2));
}

rect rectv(vec2 pos, vec2 size)
{
    return (rect){pos.x, pos.y, size.x, size.y};
}

rect rects(vec2 size)
{
    return (rect){0.0f, 0.0f, size.x, size.y};
}

bool rectContainsf(rect r, f32 x, f32 y)
{
    return x >= r.x && x <= r.x + r.width &&
           y >= r.y && y <= r.y + r.height;
}

bool rectIntersects(rect r1, rect r2)
{
    return !(r1.x + r1.width < r2.x || r1.x > r2.x + r2.width ||
             r1.y + r1.height < r2.y || r1.y > r2.y + r2.height);
}

rect rectScalef(rect r, f32 scale)
{
    r.x *= scale;
    r.y *= scale;
    r.width *= scale;
    r.height *= scale;
    return r;
}

rect rectTranslatef(rect r, f32 x, f32 y)
{
    r.x += x;
    r.y += y;
    return r;
}

vec2 getClosestPointOnRect(vec2 p, rect r)
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

void rectPrint(rect r)
{
    printf("(%f, %f, %f, %f)\n", r.x, r.y, r.width, r.height);
}