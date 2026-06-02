/*
    alloc.h - per-instance allocator interface for SHL collections.

    MIT License

    Copyright (c) 2018 Alejandro Coto Gutiérrez

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    Self-contained single-header with no SHL dependencies.  Include it
    directly if you only need the allocator type, or include any SHL collection
    header which pulls it in via internal.h.

    USAGE
    Use shl_heap_alloc() for the default system-heap allocator.

    For a memzone_t-backed allocator, include memzone.h BEFORE this header
    (or before any SHL collection header) and call shl_zone_alloc(zone):

        #include "memzone.h"   // must precede alloc.h / any SHL header
        #include "list.h"

        memzone_t*      zone  = mz_init(1 << 20);
        shl_allocator_t alloc = shl_zone_alloc(zone);
        IntListInit(&list, &alloc);

    'zone' must outlive every collection that holds a pointer to 'alloc'.

    A NULL shl_allocator_t* stored in a collection means the collection owns
    a fixed, externally-provided buffer and will never allocate or free memory.
*/

#ifndef SHL_ALLOC_H
#define SHL_ALLOC_H

#include <stddef.h>
#include <stdlib.h>

/* ---------------------------------------------------------------------------
   shl_allocator_t — per-instance allocator interface

   A single shared instance can back multiple collections simultaneously.
   NULL function pointers are not permitted; use shl_heap_alloc() for the
   default system heap.
   --------------------------------------------------------------------------- */

typedef struct shl_allocator_s
{
    void* ctx;
    void* (*mallocFn)(void* ctx, size_t sz);
    void* (*reallocFn)(void* ctx, void* ptr, size_t sz);
    void  (*freeFn)(void* ctx, void* ptr);
} shl_allocator_t;

static inline void* shl__heap_malloc_fn(void* ctx, size_t sz)             { (void)ctx; return malloc(sz); }
static inline void* shl__heap_realloc_fn(void* ctx, void* ptr, size_t sz) { (void)ctx; return realloc(ptr, sz); }
static inline void  shl__heap_free_fn(void* ctx, void* ptr)               { (void)ctx; free(ptr); }

/* Returns a pointer to a stable shl_allocator_t backed by the system heap
   (malloc / realloc / free). The returned pointer is valid for the lifetime
   of the program and may be shared freely across collections and threads. */
static inline shl_allocator_t* shl_heap_alloc(void)
{
    static shl_allocator_t heap = { NULL, shl__heap_malloc_fn, shl__heap_realloc_fn, shl__heap_free_fn };
    return &heap;
}

/* ---------------------------------------------------------------------------
   Optional memzone.h bridge

   Compiled only when memzone.h has been included before this header.
   --------------------------------------------------------------------------- */

#ifdef SHL_MZ_H

static inline void* shl__mz_malloc_fn(void* ctx, size_t sz)             { return mz_alloc((memzone_t*)ctx, sz); }
static inline void* shl__mz_realloc_fn(void* ctx, void* ptr, size_t sz) { return mz_realloc((memzone_t*)ctx, ptr, sz); }
static inline void shl__mz_free_fn(void* ctx, void* ptr)                { mz_free((memzone_t*)ctx, ptr); }

/* Returns an shl_allocator_t value backed by zone.
   Store the returned value and pass its address to collection Init functions. */
static inline shl_allocator_t shl_zone_alloc(memzone_t* zone)
{
    shl_allocator_t a;
    a.ctx       = zone;
    a.mallocFn  = shl__mz_malloc_fn;
    a.reallocFn = shl__mz_realloc_fn;
    a.freeFn    = shl__mz_free_fn;
    return a;
}

#endif /* SHL_MZ_H */

#endif /* SHL_ALLOC_H */
