#pragma once

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

typedef union {
    u8 rgba[4];
    struct { u8 r, g, b, a; };
} u8Color;

#define U8COLOR_TRANSPARENT ((u8Color){{0, 0, 0, 0}})
#define U8COLOR_WHITE ((u8Color){{255, 255, 255, 255}})
#define U8COLOR_BLACK ((u8Color){{0, 0, 0, 255}})
#define U8COLOR_RED ((u8Color){{199, 0, 0, 255}})
#define U8COLOR_GREEN ((u8Color){{0, 199, 0, 255}})
#define U8COLOR_BLUE ((u8Color){{0, 0, 199, 255}})
#define U8COLOR_YELLOW ((u8Color){{199, 199, 0, 255}})
#define u8RgbaColor(r, g, b, a) ((u8Color){{r, g, b, a}})
#define u8RgbColor(r, g, b) u8RgbaColor(r, g, b, 255)

#ifndef NULL
#define NULL ((void*)0)
#endif

#define internal static
#define global static
#define local static

#define DEFAULT_BUFFER_SIZE 1024

#define NOT_IMPLEMENTED() fprintf(stderr, "Not implemented at %s (%d)", __FILE__, __LINE__)
#define NOT_USED(expr) do { (void)(expr); } while (0)

#define getBitFlag(v, f) ((v) & (1 << (f)) ? 1 : 0)
#define setBitFlag(v, f) (*(v) = (*(v) | (1 << (f))))
#define clearBitFlag(v, f) (*(v) = *(v) ^ (*(v) & (1 << (f))))

#define arrayInit(type, ...) ((type[]){__VA_ARGS__})
#define arrayArg(type, ...) arrayInit(type, __VA_ARGS__)
#define createArray(type, count, reset) (reset ? (type *)xcalloc(count, sizeof(type)) : (type *)xmalloc(count * sizeof(type)))
#define arrayLength(arr) (sizeof(arr) / sizeof((arr)[0]))
#define chance(p) ((rand() % 100) < p)
#define chancef01(p) ((rand() / RAND_MAX) < p)
#define randomi(a, b) ((a) + (rand() % ((b) - (a))))
#define randomf(a, b) ((a) + ((f32)rand() / RAND_MAX) * ((b) - (a)))
#define inRange(x, a, b) ((x) >= (a) && (x) < (b))
#define debugVar(x) printf("variable "#x" = %d\n", x)

#define readu8(arr, index) (arr[index])
#define reads16(arr, index) (*(s16*)((arr) + (index)))
#define readu16(arr, index) (*(u16*)((arr) + (index)))
#define reads32(arr, index) (*(s32*)((arr) + (index)))
#define readu32(arr, index) (*(u32*)((arr) + (index)))

bool strEquals(const char* str1, const char* str2)
{
    return strcmp(str1, str2) == 0;
}

bool strCaseEquals(const char* str1, const char* str2, bool ignoreCase)
{
    return ignoreCase
        ? strcasecmp(str1, str2) == 0
        : strcmp(str1, str2) == 0;
}

uint32_t strHashFNV32(const char* data)
{
#define FNV_PRIME_32 0x01000193
#define FNV_OFFSET_32 0x811c9dc5

    uint32_t hash = FNV_OFFSET_32;
    while(*data != 0)
        hash = (*data++ ^ hash) * FNV_PRIME_32;

    return hash;
}

void strFree(char* str)
{
    free((void*)str);
}

void strInsertAt(char* str, s32 index, char c)
{
    s32 length = strlen(str) + 1; // count the \0
    memmove(str + index + 1, str + index, length - index);
    str[index] = c;
}

void strRemoveAt(char* str, s32 index)
{
    s32 length = strlen(str) + 1; // count the \0
    memmove(str + index, str + index + 1, length - index - 1);
}

bool strStartsWith(const char* str1, const char* str2)
{
    return strncmp(str1, str2, strlen(str2)) == 0;
}

bool strCaseStartsWith(const char* str1, const char* str2, bool ignoreCase)
{
    return ignoreCase
        ? strncasecmp(str1, str2, strlen(str2)) == 0
        : strncmp(str1, str2, strlen(str2)) == 0;
}

const char* strSkipUntil(const char* str1, const char* str2)
{
    char* str = (char*)str1;
    while (strchr(str2, *str))
    {
        str++;
    }

    return str;
}

s32 strParseS32(const char* str)
{
    return strtol(str, NULL, 0);
}

bool strTryParseS32(const char* str, s32* value)
{
    char* following;
    *value = strtol(str, &following, 0);
    return following > str;
}

s64 strParseS64(const char* str)
{
    return strtoll(str, NULL, 0);
}

bool strTryParseS64(const char* str, s64* value)
{
    char* following;
    *value = strtoll(str, &following, 0);
    return following > str;
}

void* xmalloc(size32 size, char *file, s32 line)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        perror("xmalloc failed!");
        if (file)
        {
            fprintf(stderr, "  at file %s (%d)\n", file, line);
        }
        exit(1);
    }

    return ptr;
}

void* xcalloc(size32 count, size32 size, char *file, s32 line)
{
    void *ptr = calloc(count, size);
    if (!ptr)
    {
        perror("xcalloc failed!");
        if (file)
        {
            fprintf(stderr, "  at file %s (%d)\n", file, line);
        }
        exit(1);
    }

    return ptr;
}

void* xrealloc(void *ptr, size32 size, char *file, s32 line)
{
    ptr = realloc(ptr, size);
    if (!ptr)
    {
        perror("xrealloc failed!");
        if (file)
        {
            fprintf(stderr, "  at file %s (%d)\n", file, line);
        }
        exit(1);
    }

    return ptr;
}

#define xmalloc(size) xmalloc(size, __FILE__, __LINE__)
#define xcalloc(count, size) xcalloc(count, size, __FILE__, __LINE__)
#define xrealloc(ptr, size) xrealloc(ptr, size, __FILE__, __LINE__)

void msleep(s32 milliseconds) // cross-platform sleep function
{
    if (milliseconds <= 0)
        return;

#ifdef WIN32
    // windows.h need to be include for this
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    // this is the posix call, _POSIX_C_SOURCE need to be defined
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    // unistd.h need to be include for this
    usleep(milliseconds * 1000);
#endif
}

#if __DEBUG__
/* Obtain a backtrace and print it to stdout. */
void printTrace()
{
#define MAX_STACKTRACE 50

  void *buffer[MAX_STACKTRACE];
  size32 size = backtrace(buffer, MAX_STACKTRACE);
  char** strings = backtrace_symbols(buffer, size);

  printf("Obtained %zd stack frames.\n", size);

  for (s32 i = 0; i < size; i++)
     printf ("%s\n", strings[i]);

  free (strings);
}

#endif
