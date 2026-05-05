#pragma once

#include <stddef.h>
#include <stdbool.h>

// Forward declare memzone_t
typedef struct memzone_s memzone_t;

// Global allocators
extern memzone_t* permanentZone;
extern memzone_t* frameZone;
extern memzone_t* currentZone;

bool wm_allocInit(size_t permSize, size_t frameSize);
void wm_allocFree(void);

// Temporary set current zone, useful for frame-specific allocations
void wm_setZone(memzone_t* zone);
void wm_resetZone(void); // restores to permanentZone

// Internal macro targets; prefer the wm_* macros below.
void* wm__alloc(size_t sz, const char* file, int line);
void* wm__allocFrame(size_t sz, const char* file, int line);
void* wm__calloc(size_t n, size_t sz, const char* file, int line);
void* wm__realloc(void* p, size_t sz, const char* file, int line);
void  wm__free(void* p, const char* file, int line);

#define wm_alloc(sz)          wm__alloc((sz), __FILE__, __LINE__)
#define wm_allocFrame(sz)     wm__allocFrame((sz), __FILE__, __LINE__)
#define wm_calloc(n, sz)      wm__calloc((n), (sz), __FILE__, __LINE__)
#define wm_realloc(p, sz)     wm__realloc((p), (sz), __FILE__, __LINE__)
#define wm_free(p)            wm__free((p), __FILE__, __LINE__)
