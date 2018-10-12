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

#define mini(a, b) min(a, b)
#define maxi(a, b) max(a, b)

#define minf(a, b) fminf(a, b)
#define maxf(a, b) fmaxf(a, b)

#define absi(x) abs(x)
#define absf(x) fabsf(x)
#define halfi(x) ((x)/2)
#define halff(x) ((x)*0.5f)

//
// From the linux kernel
//
// /**
//  * abs - return absolute value of an argument
//  * @x: the value.  If it is unsigned type, it is converted to signed type first.
//  *     char is treated as if it was signed (regardless of whether it really is)
//  *     but the macro's return type is preserved as char.
//  *
//  * Return: an absolute value of x.
//  */
// #define abs(x)	__abs_choose_expr(x, long long,				\
// 		__abs_choose_expr(x, long,				\
// 		__abs_choose_expr(x, int,				\
// 		__abs_choose_expr(x, short,				\
// 		__abs_choose_expr(x, char,				\
// 		__builtin_choose_expr(					\
// 			__builtin_types_compatible_p(typeof(x), char),	\
// 			(char)({ signed char __x = (x); __x<0?-__x:__x; }), \
// 			((void)0)))))))

// #define __abs_choose_expr(x, type, other) __builtin_choose_expr(	\
// 	__builtin_types_compatible_p(typeof(x),   signed type) ||	\
// 	__builtin_types_compatible_p(typeof(x), unsigned type),		\
// 	({ signed type __x = (x); __x < 0 ? -__x : __x; }), other)

// /*
//  * min()/max()/clamp() macros must accomplish three things:
//  *
//  * - avoid multiple evaluations of the arguments (so side-effects like
//  *   "x++" happen only once) when non-constant.
//  * - perform strict type-checking (to generate warnings instead of
//  *   nasty runtime surprises). See the "unnecessary" pointer comparison
//  *   in __typecheck().
//  * - retain result as a constant expressions when called with only
//  *   constant expressions (to avoid tripping VLA warnings in stack
//  *   allocation usage).
//  */
// #define __typecheck(x, y) \
// 		(!!(sizeof((typeof(x) *)1 == (typeof(y) *)1)))

// /*
//  * This returns a constant expression while determining if an argument is
//  * a constant expression, most importantly without evaluating the argument.
//  * Glory to Martin Uecker <Martin.Uecker@med.uni-goettingen.de>
//  */
// #define __is_constexpr(x) \
// 	(sizeof(int) == sizeof(*(8 ? ((void *)((long)(x) * 0l)) : (int *)8)))

// #define __no_side_effects(x, y) \
// 		(__is_constexpr(x) && __is_constexpr(y))

// #define __safe_cmp(x, y) \
// 		(__typecheck(x, y) && __no_side_effects(x, y))

// #define __cmp(x, y, op)	((x) op (y) ? (x) : (y))

// #define __cmp_once(x, y, unique_x, unique_y, op) ({	\
// 		typeof(x) unique_x = (x);		\
// 		typeof(y) unique_y = (y);		\
// 		__cmp(unique_x, unique_y, op); })

// #define __careful_cmp(x, y, op) \
// 	__builtin_choose_expr(__safe_cmp(x, y), \
// 		__cmp(x, y, op), \
// 		__cmp_once(x, y, __UNIQUE_ID(__x), __UNIQUE_ID(__y), op))

// /**
//  * min - return minimum of two values of the same or compatible types
//  * @x: first value
//  * @y: second value
//  */
// #define min(x, y)	__careful_cmp(x, y, <)

// /**
//  * max - return maximum of two values of the same or compatible types
//  * @x: first value
//  * @y: second value
//  */
// #define max(x, y)	__careful_cmp(x, y, >)

// /**
//  * min3 - return minimum of three values
//  * @x: first value
//  * @y: second value
//  * @z: third value
//  */
// #define min3(x, y, z) min((typeof(x))min(x, y), z)

// /**
//  * max3 - return maximum of three values
//  * @x: first value
//  * @y: second value
//  * @z: third value
//  */
// #define max3(x, y, z) max((typeof(x))max(x, y), z)

// /**
//  * min_not_zero - return the minimum that is _not_ zero, unless both are zero
//  * @x: value1
//  * @y: value2
//  */
// #define min_not_zero(x, y) ({			\
// 	typeof(x) __x = (x);			\
// 	typeof(y) __y = (y);			\
// 	__x == 0 ? __y : ((__y == 0) ? __x : min(__x, __y)); })

// /**
//  * clamp - return a value clamped to a given range with strict typechecking
//  * @val: current value
//  * @lo: lowest allowable value
//  * @hi: highest allowable value
//  *
//  * This macro does strict typechecking of @lo/@hi to make sure they are of the
//  * same type as @val.  See the unnecessary pointer comparisons.
//  */
// #define clamp(val, lo, hi) min((typeof(val))max(val, lo), hi)

// /*
//  * ..and if you can't take the strict
//  * types, you can specify one yourself.
//  *
//  * Or not use min/max/clamp at all, of course.
//  */

// /**
//  * min_t - return minimum of two values, using the specified type
//  * @type: data type to use
//  * @x: first value
//  * @y: second value
//  */
// #define min_t(type, x, y)	__careful_cmp((type)(x), (type)(y), <)

// /**
//  * max_t - return maximum of two values, using the specified type
//  * @type: data type to use
//  * @x: first value
//  * @y: second value
//  */
// #define max_t(type, x, y)	__careful_cmp((type)(x), (type)(y), >)

// /**
//  * clamp_t - return a value clamped to a given range using a given type
//  * @type: the type of variable to use
//  * @val: current value
//  * @lo: minimum allowable value
//  * @hi: maximum allowable value
//  *
//  * This macro does no typechecking and uses temporary variables of type
//  * @type to make all the comparisons.
//  */
// #define clamp_t(type, val, lo, hi) min_t(type, max_t(type, val, lo), hi)

// /**
//  * clamp_val - return a value clamped to a given range using val's type
//  * @val: current value
//  * @lo: minimum allowable value
//  * @hi: maximum allowable value
//  *
//  * This macro does no typechecking and uses temporary variables of whatever
//  * type the input argument @val is.  This is useful when @val is an unsigned
//  * type and @lo and @hi are literals that will otherwise be assigned a signed
//  * integer type.
//  */
// #define clamp_val(val, lo, hi) clamp_t(typeof(val), val, lo, hi)


// /**
//  * swap - swap values of @a and @b
//  * @a: first value
//  * @b: second value
//  */
// #define swap(a, b) \
// 	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

// /* This counts to 12. Any more, it will return 13th argument. */
// #define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _n, X...) _n
// #define COUNT_ARGS(X...) __COUNT_ARGS(, ##X, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// #define __CONCAT(a, b) a ## b
// #define CONCATENATE(a, b) __CONCAT(a, b)

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

inline vec2 vec2Addi(vec2 v, s32 x)
{
    return (vec2){
        v.x + x,
        v.y + x
    };
}

inline vec2 vec2Addf(vec2 v, f32 x)
{
    return (vec2){
        v.x + x,
        v.y + x
    };
}

inline vec2 vec2Subv(vec2 a, vec2 b)
{
    return (vec2){
        a.x - b.x,
        a.y - b.y
    };
}

inline vec2 vec2Subi(vec2 v, s32 x)
{
    return (vec2){
        v.x - x,
        v.y - x
    };
}

inline vec2 vec2Subf(vec2 v, f32 x)
{
    return (vec2){
        v.x - x,
        v.y - x
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

inline vec2 vec2Scalev(vec2 v, vec2 scale)
{
    return (vec2){v.x * scale.x, v.y * scale.y};
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

inline f32 vec2DistanceSqr(vec2 v1, vec2 v2)
{
    f32 xx = (v1.x - v2.x);
    f32 yy = (v1.y - v2.y);
    return xx * xx + yy * yy;
}

inline f32 vec2Distance(vec2 v1, vec2 v2)
{
    return sqrtf(vec2DistanceSqr(v1, v2));
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

inline vec2 vec2Floor(vec2 v)
{
    return (vec2){
        floorf(v.x),
        floorf(v.y)
    };
}

inline vec2 vec2Ceil(vec2 v)
{
    return (vec2){
        ceilf(v.x),
        ceilf(v.y)
    };
}

inline vec2 vec2Round(vec2 v)
{
    return (vec2){
        roundf(v.x),
        roundf(v.y)
    };
}

inline void vec2Print(vec2 v)
{
    logDebug("(%f, %f)", v.x, v.y);
}

/*
 * rect types and functions
*/
typedef struct
{
    f32 x, y;
    f32 width, height;
} rect;

#define RECT_EMPTY (rect){0.0f, 0.0f, 0.0f, 0.0f}

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
        minf(x1, x2),
        minf(y1, y2),
        absf(x1 - x2), 
        absf(y1 - y2));
}

inline rect rectv(vec2 pos, vec2 size)
{
    return (rect){pos.x, pos.y, size.x, size.y}; 
}

inline rect rects(vec2 size)
{
    return (rect){0.0f, 0.0f, size.x, size.y}; 
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