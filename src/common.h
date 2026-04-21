#pragma once

#include <stdint.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef unsigned char uchar;

typedef size_t size32;

#define NO_ARG_STR ""

#ifndef NULL
#define NULL ((void*)0)
#endif

#define NOT_IMPLEMENTED() fprintf(stderr, "Not implemented at %s (%d)", __FILE__, __LINE__)
#define NOT_USED(expr) do { (void)(expr); } while (0)

#define arrayInit(type, ...) ((type[]){__VA_ARGS__})
#define arrayArg(type, ...) arrayInit(type, __VA_ARGS__)
#define arrayLength(arr) (sizeof(arr) / sizeof((arr)[0]))
#define chance(p) ((rand() % 100) < p)
#define chancef01(p) ((rand() / RAND_MAX) < p)
#define randomi(a, b) ((a) + (rand() % ((b) - (a))))
#define randomf(a, b) ((a) + ((f32)rand() / RAND_MAX) * ((b) - (a)))
#define inRange(x, a, b) ((x) >= (a) && (x) < (b))
