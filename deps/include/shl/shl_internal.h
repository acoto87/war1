/*
    shl_internal.h - shared internal helpers for SHL collection headers.
    This file is not part of the public API surface.
*/

#ifndef SHL_INTERNAL_H
#define SHL_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef SHL_MALLOC
#define SHL_MALLOC(sz) malloc(sz)
#endif
#ifndef SHL_CALLOC
#define SHL_CALLOC(n, sz) calloc((n), (sz))
#endif
#ifndef SHL_REALLOC
#define SHL_REALLOC(ptr, sz) realloc((ptr), (sz))
#endif
#ifndef SHL_FREE
#define SHL_FREE(ptr) free(ptr)
#endif

#define SHL__INITIAL_CAPACITY 8
#define SHL__INITIAL_HASH_SHIFT 29
#define SHL__INITIAL_HASH_LOAD_FACTOR 6

static inline int32_t shl__grownCapacity(int32_t currentCapacity, int32_t minSize)
{
    int32_t newCapacity = currentCapacity > 0 ? (currentCapacity << 1) : SHL__INITIAL_CAPACITY;

    if (newCapacity < minSize)
        newCapacity = minSize;

    return newCapacity;
}

static inline void shl__resizeArray(void** items, int32_t* capacity, int32_t minSize, size_t itemSize)
{
    *capacity = shl__grownCapacity(*capacity, minSize);
    *items = SHL_REALLOC(*items, (size_t)(*capacity) * itemSize);
}

static inline void shl__resizeCircularArray(void** items, int32_t* capacity, int32_t* head, int32_t* tail, int32_t count, size_t itemSize)
{
    int32_t oldCapacity = *capacity;
    unsigned char* oldItems = (unsigned char*)*items;
    unsigned char* newItems;

    *capacity = shl__grownCapacity(*capacity, *capacity + 1);
    newItems = (unsigned char*)SHL_CALLOC((size_t)(*capacity), itemSize);

    if (count > 0)
    {
        if (*head >= *tail)
        {
            size_t firstCopySize = (size_t)(oldCapacity - *head) * itemSize;
            size_t secondCopySize = (size_t)((*head + count) % oldCapacity) * itemSize;

            memcpy(newItems, oldItems + (size_t)(*head) * itemSize, firstCopySize);
            memcpy(newItems + firstCopySize, oldItems, secondCopySize);
        }
        else
        {
            memcpy(newItems, oldItems + (size_t)(*head) * itemSize, (size_t)count * itemSize);
        }
    }

    *head = 0;
    *tail = count;
    SHL_FREE(*items);
    *items = newItems;
}

static inline int32_t shl__fibHash(uint32_t hash, int32_t shift)
{
    const uint32_t hashConstant = 2654435769u;
    return (int32_t)((hash * hashConstant) >> shift);
}

static inline int32_t shl__findEmptyBucket(const void* entries, int32_t capacity, int32_t startIndex, size_t entrySize, size_t activeOffset)
{
    const unsigned char* bytes = (const unsigned char*)entries;

    for (int32_t i = 0; i < capacity; i++)
    {
        int32_t currentIndex = (startIndex + i) % capacity;
        bool active = false;

        memcpy(&active, bytes + (size_t)currentIndex * entrySize + activeOffset, sizeof(active));
        if (!active)
            return currentIndex;
    }

    return -1;
}

#endif // SHL_INTERNAL_H
