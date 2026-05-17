#pragma once

#include <stddef.h>
#include <stdbool.h>

// Forward declare memzone_t
typedef struct memzone_s memzone_t;

// Global allocators
extern memzone_t* globalZone;
extern memzone_t* frameZone;
extern memzone_t* audioZone;

bool wm_allocInit(size_t globalSize, size_t frameSize, size_t audioSize);
void wm_allocFree(void);

// Internal macro targets; prefer the wm_* macros below.
void* wm__alloc(size_t sz, const char* file, int line);
void* wm__allocFrame(size_t sz, const char* file, int line);
void* wm__allocAudio(size_t sz, const char* file, int line);
void* wm__realloc(void* p, size_t sz, const char* file, int line);
void* wm__reallocAudio(void* p, size_t sz, const char* file, int line);
void  wm__free(void* p, const char* file, int line);

#define wm_alloc(sz)          wm__alloc((sz), __FILE__, __LINE__)
#define wm_allocFrame(sz)     wm__allocFrame((sz), __FILE__, __LINE__)
#define wm_allocAudio(sz)     wm__allocAudio((sz), __FILE__, __LINE__)
#define wm_realloc(p, sz)     wm__realloc((p), (sz), __FILE__, __LINE__)
#define wm_reallocAudio(p, sz) wm__reallocAudio((p), (sz), __FILE__, __LINE__)
#define wm_free(p)            wm__free((p), __FILE__, __LINE__)
