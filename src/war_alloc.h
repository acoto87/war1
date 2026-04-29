#pragma once

#include <stddef.h>
#include <stdbool.h>

// Forward declare memzone_t
typedef struct memzone_s memzone_t;

// Global allocators
extern memzone_t* permanentZone;
extern memzone_t* frameZone;
extern memzone_t* currentZone;

bool war_alloc_init(size_t permSize, size_t frameSize);
void war_alloc_free(void);

// Temporary set current zone, useful for frame-specific allocations
void war_set_zone(memzone_t* zone);
void war_reset_zone(void); // restores to permanentZone

void* war_malloc(size_t sz);
void* war_malloc_frame(size_t sz);
void* war_calloc(size_t n, size_t sz);
void* war_realloc(void* p, size_t sz);
void  war_free(void* p);
