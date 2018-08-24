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

typedef struct
{
    s32 x, y;
    s32 width, height;
} Rect;

bool rectContains(Rect rect, s32 x, s32 y)
{
    return x >= rect.x && x <= rect.x + rect.width &&
           y >= rect.y && y <= rect.y + rect.height;
}

Rect scaleRect(Rect rect, f32 scale)
{
    return (Rect){
        rect.x * scale, 
        rect.y * scale, 
        rect.width * scale, 
        rect.height * scale
    };
}