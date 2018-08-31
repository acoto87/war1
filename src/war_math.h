#define clamp(x, a, b) max(min(x, b),a)

/* 
 * This macro evaluate x twice, so for getting the sign of a value
 * you shouldn't do something like:
 * 
 * sign(a - b)
 * 
 * if a - b is an expensive operation, instead you should do it this way
 * 
 * c = a - b
 * c = sign(c)
*/
#define sign(x) (((x) > 0) - ((x) < 0))

#define absf(x) x

/*
 * vec2 types and functions
*/
typedef struct
{
    f32 x, y;
} vec2;

#define VEC2_ZERO (vec2){0.0f, 0.0f}
#define VEC2_ONE (vec2){1.0f, 1.0f}
#define VEC2_LEFT (vec2){-1.0f, 0.0f}
#define VEC2_UP (vec2){0.0f, -1.0f}
#define VEC2_RIGHT (vec2){1.0f, 0.0f}
#define VEC2_DOWN (vec2){0.0f, 1.0f}

inline vec2 vec2f(f32 x, f32 y)
{
    return (vec2){x, y};
}

inline vec2 vec2i(s32 x, s32 y)
{
    return (vec2){(f32)x, (f32)y};
}

inline vec2 vec2Addv(vec2 a, vec2 b)
{
    return (vec2){
        a.x + b.x,
        a.y + b.y
    };
}

inline vec2 vec2Subv(vec2 a, vec2 b)
{
    return (vec2){
        a.x - b.x,
        a.y - b.y
    };
}

inline vec2 vec2Mulf(vec2 v, f32 a)
{
    return (vec2){
        v.x * a,
        v.y * a
    };
}

inline vec2 vec2Muli(vec2 v, s32 a)
{
    return (vec2){
        v.x * (f32)a,
        v.y * (f32)a
    };
}

inline vec2 vec2Mulv(vec2 a, vec2 b)
{
    return (vec2){
        a.x * b.x,
        a.y * b.y
    };
}

inline vec2 vec2Translatef(vec2 v, f32 x, f32 y)
{
    return (vec2){v.x + x, v.y + y};
}

inline vec2 vec2Translatei(vec2 v, s32 x, s32 y)
{
    return (vec2){v.x + (f32)x, v.y + (f32)y};
}

inline vec2 vec2Scalef(vec2 v, f32 scale)
{
    return (vec2){v.x * scale, v.y * scale};
}

inline vec2 vec2Scalei(vec2 v, s32 scale)
{
    return (vec2){v.x * (f32)scale, v.y * (f32)scale};
}

inline vec2 vec2Inverse(vec2 v)
{
    return (vec2) { 
        -v.x, 
        -v.y 
    };
}

inline f32 vec2LengthSqr(vec2 v)
{
    return v.x * v.x + v.y * v.y;
}

inline f32 vec2Length(vec2 v)
{
    return sqrtf(vec2LengthSqr(v));
}

inline vec2 vec2Normalize(vec2 v)
{
    f32 len = vec2Length(v);
    if (len == 0) return VEC2_ZERO;
    return vec2Scalef(v, 1 / len);
}

inline vec2 vec2Clampf(vec2 v, f32 a, f32 b)
{
    return (vec2){
        clamp(v.x, a, b),
        clamp(v.y, a, b)
    };
}

inline vec2 vec2Clampi(vec2 v, s32 a, s32 b)
{
    return (vec2){
        clamp(v.x, (f32)a, (f32)b),
        clamp(v.y, (f32)a, (f32)b)
    };
}

inline vec2 vec2Clampv(vec2 v, vec2 a, vec2 b)
{
    return (vec2){
        clamp(v.x, a.x, b.x),
        clamp(v.y, a.y, b.y)
    };
}

inline void vec2Print(vec2 v)
{
    printf("(%f, %f)\n", v.x, v.y);
}

/*
 * rect types and functions
*/
typedef struct
{
    f32 x, y;
    f32 width, height;
} rect;

inline rect rectf(f32 x, f32 y, f32 width, f32 height)
{
    return (rect){x, y, width, height}; 
}

inline rect recti(s32 x, s32 y, s32 width, s32 height)
{
    return (rect){(f32)x, (f32)y, (f32)width, (f32)height}; 
}

inline rect rectpf(f32 x1, f32 y1, f32 x2, f32 y2)
{
    return rectf(
        fminf(x1, x2),
        fminf(y1, y2),
        fabsf(x1 - x2), 
        fabsf(y1 - y2));
}

inline bool rectContainsf(rect r, f32 x, f32 y)
{
    return x >= r.x && x <= r.x + r.width &&
           y >= r.y && y <= r.y + r.height;
}

inline bool rectIntersects(rect r1, rect r2)
{
    return !(r1.x + r1.width < r2.x || r1.x > r2.x + r2.width ||
             r1.y + r1.height < r2.y || r1.y > r2.y + r2.height);
}

inline rect rectScalef(rect r, f32 scale)
{
    r.x *= scale;
    r.y *= scale;
    r.width *= scale;
    r.height *= scale;
    return r;
}

inline rect rectTranslatef(rect r, f32 x, f32 y)
{
    r.x += x;
    r.y += y;
    return r;
}

inline void rectPrint(rect r)
{
    printf("(%f, %f, %f, %f)\n", r.x, r.y, r.width, r.height);
}