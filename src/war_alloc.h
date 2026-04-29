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

void* wm_alloc(size_t sz);
void* wm_allocFrame(size_t sz);
void* wm_calloc(size_t n, size_t sz);
void* wm_realloc(void* p, size_t sz);
void  wm_free(void* p);
