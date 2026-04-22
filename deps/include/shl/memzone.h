/*
    memzone.h - acoto87 (acoto87@gmail.com)

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
*/
#ifndef SHL_MEMORY_ZONE_H
#define SHL_MEMORY_ZONE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct memzone_s memzone_t;

typedef enum
{
    MZ_REPORT_ALLOCATION_FAILURE,
    MZ_REPORT_INVALID_FREE,
    MZ_REPORT_VALIDATION_FAILURE
} mz_report_t;

typedef void (*mz_reporter_t)(const memzone_t* zone, mz_report_t report, const void* context, const char* message, void* userData);

#if defined(MZ_PRIVATE_API)
typedef struct memblock_s
{
    size_t size;                        // size of the block
    void* user;                         // a pointer to the pointer returned to the user
    struct memblock_s *next, *prev;     // pointers to next and prev in the block list
} memblock_t;

struct memzone_s
{
    size_t usedSize;                    // how much space is used without including blocks data
    size_t maxSize;                     // the max allowed size that can be allocated
    memblock_t* rover;                  // a pointer to a free block that is used when allocating
    mz_reporter_t reporter;             // optional runtime diagnostics hook, defaults to stderr
    void* reporterUserData;             // user data passed to the diagnostics hook
    memblock_t blockList;               // list of blocks, here is where the requested memory begins
};
#endif

size_t mz_alignment(void);
size_t mz_maxSize(const memzone_t* zone);
size_t mz_usedSize(const memzone_t* zone);
memzone_t* mz_init(size_t maxSize);
void mz_destroy(memzone_t* zone);
void mz_reset(memzone_t* zone);
void* mz_alloc(memzone_t* zone, size_t size);
void* mz_allocAligned(memzone_t* zone, size_t size, size_t alignment);
void mz_setReporter(memzone_t* zone, mz_reporter_t reporter, void* userData);
void mz_free(memzone_t* zone, void* p);
bool mz_contains(const memzone_t* zone, const void* p);
size_t mz_allocationSize(const memzone_t* zone, const void* p);
bool mz_validate(const memzone_t* zone);
int32_t mz_blockCount(const memzone_t* zone);
size_t mz_usableFreeSize(const memzone_t* zone);
float mz_fragmentation(const memzone_t* zone);

#ifdef __cplusplus
}
#endif

#ifdef SHL_MEMORY_ZONE_IMPLEMENTATION

#ifndef MZ_MALLOC
#define MZ_MALLOC(sz) malloc(sz)
#endif

#ifndef MZ_FREE
#define MZ_FREE(p) free(p)
#endif

#if defined(MZ_DEBUG) && !defined(MZ_ASSERT)
#include <assert.h>
#define MZ_ASSERT(expr) assert(expr)
#endif

#ifndef MZ_ASSERT
#define MZ_ASSERT(expr) ((void)0)
#endif

#if !defined(MZ_PRIVATE_API)
typedef struct memblock_s
{
    size_t size;                        // size of the block
    void* user;                         // a pointer to the pointer returned to the user
    struct memblock_s *next, *prev;     // pointers to next and prev in the block list
} memblock_t;

struct memzone_s
{
    size_t usedSize;                    // how much space is used without including blocks data
    size_t maxSize;                     // the max allowed size that can be allocated
    memblock_t* rover;                  // a pointer to a free block that is used when allocating
    mz_reporter_t reporter;             // optional runtime diagnostics hook, defaults to stderr
    void* reporterUserData;             // user data passed to the diagnostics hook
    memblock_t blockList;               // list of blocks, here is where the requested memory begins
};
#endif

#define MZ__POINTER_OFFSET(t, p, o) ((t*)((uint8_t*)(p) + (o)))
#define MZ__ALIGNOF(type) offsetof(struct { char c; type value; }, value)
#define MZ__MAX(a, b) ((a) > (b) ? (a) : (b))
#define MZ__DEFAULT_ALIGNMENT MZ__MAX(MZ__ALIGNOF(memblock_t), MZ__MAX(MZ__ALIGNOF(void*), MZ__ALIGNOF(size_t)))
#define MZ__IS_BLOCK_EMPTY(block) ((block)->user == NULL)

static bool mz__alignUp(size_t value, size_t alignment, size_t* alignedValue)
{
    if (alignedValue == NULL)
    {
        return false;
    }

    if (alignment == 0)
    {
        *alignedValue = value;
        return true;
    }

    size_t remainder = value % alignment;
    if (remainder == 0)
    {
        *alignedValue = value;
        return true;
    }

    size_t padding = alignment - remainder;
    if (value > ((size_t)-1) - padding)
    {
        return false;
    }

    *alignedValue = value + padding;
    return true;
}

static bool mz__isPowerOfTwo(size_t value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

static bool mz__isSupportedAlignment(size_t alignment)
{
    if (!mz__isPowerOfTwo(alignment) || alignment < mz_alignment())
    {
        return false;
    }

    if ((alignment % mz_alignment()) != 0)
    {
        return false;
    }

    return alignment == mz_alignment() || alignment == 16 || alignment == 32 || alignment == 64;
}

static const char* mz__reportName(mz_report_t report)
{
    switch (report)
    {
        case MZ_REPORT_ALLOCATION_FAILURE: return "allocation failure";
        case MZ_REPORT_INVALID_FREE: return "invalid free";
        case MZ_REPORT_VALIDATION_FAILURE: return "validation failure";
        default: return "unknown report";
    }
}

static void mz__stderrReporter(const memzone_t* zone, mz_report_t report, const void* context, const char* message, void* userData)
{
    (void)userData;
    fprintf(stderr, "memzone %s", mz__reportName(report));

    if (zone != NULL)
    {
        fprintf(stderr, " [zone=%p]", (const void*)zone);
    }

    if (context != NULL)
    {
        fprintf(stderr, " [context=%p]", context);
    }

    if (message != NULL && message[0] != '\0')
    {
        fprintf(stderr, ": %s", message);
    }

    fputc('\n', stderr);
}

static void mz__report(const memzone_t* zone, mz_report_t report, const void* context, const char* message)
{
    if (zone == NULL || zone->reporter == NULL)
    {
        return;
    }

    zone->reporter(zone, report, context, message, zone->reporterUserData);
}

static bool mz__validationFailure(const memzone_t* zone, const void* context, const char* message)
{
    mz__report(zone, MZ_REPORT_VALIDATION_FAILURE, context, message);
    return false;
}

static void mz__debugAssertValid(const memzone_t* zone)
{
#if defined(MZ_DEBUG)
    MZ_ASSERT(mz_validate(zone));
#else
    (void)zone;
#endif
}

static size_t mz__headerSize(void)
{
    size_t headerSize = sizeof(memblock_t);
    size_t alignedHeaderSize = headerSize;

    (void)mz__alignUp(headerSize, mz_alignment(), &alignedHeaderSize);
    return alignedHeaderSize;
}

static size_t mz__zoneBaseSize(void)
{
    return offsetof(memzone_t, blockList) + mz__headerSize();
}

static size_t mz__payloadSize(const memblock_t* block)
{
    return block->size - mz__headerSize();
}

static void* mz__payloadPointer(memblock_t* block)
{
    return MZ__POINTER_OFFSET(void, block, mz__headerSize());
}

static size_t mz__allocationSize(const memblock_t* block)
{
    if (block == NULL || block->user == NULL)
    {
        return 0;
    }

    return (size_t)(((const uint8_t*)block + block->size) - (const uint8_t*)block->user);
}

static bool mz__isNextBlockAdjacent(const memblock_t* block)
{
    return MZ__POINTER_OFFSET(const uint8_t, block, block->size) == (const uint8_t*)block->next;
}

static bool mz__isPrevBlockAdjacent(const memblock_t* block)
{
    return MZ__POINTER_OFFSET(const uint8_t, block->prev, block->prev->size) == (const uint8_t*)block;
}

static const memblock_t* mz__findBlock(const memzone_t* zone, const void* p)
{
    if (zone == NULL || p == NULL)
    {
        return NULL;
    }

    const memblock_t* rover = &zone->blockList;
    do
    {
        if (rover->user == p)
        {
            return rover;
        }

        rover = rover->next;
    } while (rover != &zone->blockList);

    return NULL;
}

size_t mz_alignment(void)
{
    return MZ__DEFAULT_ALIGNMENT;
}

size_t mz_maxSize(const memzone_t* zone)
{
    return zone != NULL ? zone->maxSize : 0;
}

size_t mz_usedSize(const memzone_t* zone)
{
    return zone != NULL ? zone->usedSize : 0;
}

memzone_t* mz_init(size_t maxSize)
{
    if (maxSize < mz__zoneBaseSize())
    {
        fprintf(stderr, "You need to allocate memory for at least %llu bytes.\n", (unsigned long long)mz__zoneBaseSize());
        return NULL;
    }

    uint8_t* rawZone = (uint8_t*)MZ_MALLOC(maxSize);
    memzone_t* zone = (memzone_t*)rawZone;
    if (!zone)
    {
        fprintf(stderr, "The system couldn't allocate memory for %llu bytes.\n", (unsigned long long)maxSize);
        return NULL;
    }

    zone->usedSize = mz__zoneBaseSize();
    zone->maxSize = maxSize;
    zone->reporter = mz__stderrReporter;
    zone->reporterUserData = NULL;

    zone->blockList = (memblock_t){0};
    zone->blockList.size = maxSize - offsetof(memzone_t, blockList);
    zone->blockList.user = NULL;
    zone->blockList.next = &zone->blockList;
    zone->blockList.prev = &zone->blockList;

    zone->rover = &zone->blockList;
    mz__debugAssertValid(zone);

    return zone;
}

void mz_destroy(memzone_t* zone)
{
    MZ_FREE(zone);
}

void mz_reset(memzone_t* zone)
{
    if (zone == NULL)
    {
        return;
    }

    zone->usedSize = mz__zoneBaseSize();
    zone->blockList = (memblock_t){0};
    zone->blockList.size = zone->maxSize - offsetof(memzone_t, blockList);
    zone->blockList.user = NULL;
    zone->blockList.next = &zone->blockList;
    zone->blockList.prev = &zone->blockList;
    zone->rover = &zone->blockList;
    mz__debugAssertValid(zone);
}

void* mz_alloc(memzone_t* zone, size_t size)
{
    return mz_allocAligned(zone, size, mz_alignment());
}

void mz_setReporter(memzone_t* zone, mz_reporter_t reporter, void* userData)
{
    if (zone == NULL)
    {
        return;
    }

    zone->reporter = reporter;
    zone->reporterUserData = userData;
}

void* mz_allocAligned(memzone_t* zone, size_t size, size_t alignment)
{
    if (zone == NULL || size == 0)
    {
        return NULL;
    }

    if (!mz__isSupportedAlignment(alignment))
    {
        char message[160];
        snprintf(message, sizeof(message),
            "unsupported alignment %llu, expected %llu, 16, 32, or 64 bytes",
            (unsigned long long)alignment,
            (unsigned long long)mz_alignment());
        mz__report(zone, MZ_REPORT_ALLOCATION_FAILURE, NULL, message);
        return NULL;
    }

    size_t alignedSize = 0;
    if (!mz__alignUp(size, mz_alignment(), &alignedSize))
    {
        char message[160];
        snprintf(message, sizeof(message),
            "requested allocation size %llu bytes is too large",
            (unsigned long long)size);
        mz__report(zone, MZ_REPORT_ALLOCATION_FAILURE, NULL, message);
        return NULL;
    }

    size_t headerSize = mz__headerSize();
    if (alignedSize > zone->maxSize - zone->usedSize)
    {
        char message[160];
        snprintf(message, sizeof(message),
            "not enough free memory to allocate %llu bytes",
            (unsigned long long)size);
        mz__report(zone, MZ_REPORT_ALLOCATION_FAILURE, NULL, message);
        return NULL;
    }

    memblock_t* start = zone->rover != NULL ? zone->rover : &zone->blockList;
    memblock_t* rover = start;
    size_t padding = 0;
    size_t sizeToAlloc = 0;
    bool found = false;
    do
    {
        if (MZ__IS_BLOCK_EMPTY(rover))
        {
            uintptr_t payloadAddress = (uintptr_t)mz__payloadPointer(rover);
            size_t currentPadding = 0;
            size_t remainder = payloadAddress % alignment;
            if (remainder != 0)
            {
                currentPadding = alignment - remainder;
            }

            if (currentPadding <= mz__payloadSize(rover) && alignedSize <= mz__payloadSize(rover) - currentPadding)
            {
                size_t currentSizeToAlloc = headerSize + currentPadding;
                if (alignedSize <= ((size_t)-1) - currentSizeToAlloc)
                {
                    currentSizeToAlloc += alignedSize;
                    if (rover->size >= currentSizeToAlloc)
                    {
                        padding = currentPadding;
                        sizeToAlloc = currentSizeToAlloc;
                        found = true;
                        break;
                    }
                }
            }
        }

        rover = rover->next;
    } while (rover != start);

    if (!found)
    {
        char message[160];
        snprintf(message, sizeof(message),
            "free memory exists for %llu bytes but no suitably aligned block is large enough",
            (unsigned long long)size);
        mz__report(zone, MZ_REPORT_ALLOCATION_FAILURE, NULL, message);
        return NULL;
    }

    size_t usedPayloadSize = mz__payloadSize(rover);
    size_t remainingSize = rover->size - sizeToAlloc;
    if (remainingSize >= headerSize + mz_alignment())
    {
        // create a new empty block with the remaining free space
        memblock_t* newBlock = MZ__POINTER_OFFSET(memblock_t, rover, sizeToAlloc);
        newBlock->size = remainingSize;
        newBlock->user = NULL;
        newBlock->prev = rover;
        newBlock->next = rover->next;

        // set the next block to point to the new one
        rover->next->prev = newBlock;
        rover->next = newBlock;
        rover->size = sizeToAlloc;

        // update the zone rover with the new free block created
        zone->rover = newBlock;
        zone->usedSize += headerSize;
        usedPayloadSize = padding + alignedSize;
    }
    else
    {
        zone->rover = rover->next;
    }

    zone->usedSize += usedPayloadSize;

    rover->user = MZ__POINTER_OFFSET(void, mz__payloadPointer(rover), padding);
    mz__debugAssertValid(zone);
    return rover->user;
}

void mz_free(memzone_t* zone, void* p)
{
    if (zone == NULL || p == NULL)
    {
        return;
    }

    memblock_t* rover = (memblock_t*)mz__findBlock(zone, p);
    if (rover == NULL)
    {
        mz__report(zone, MZ_REPORT_INVALID_FREE, p, "pointer does not reference a live allocation in this zone");
        return;
    }

    rover->user = NULL;
    zone->rover = rover;

    zone->usedSize -= mz__payloadSize(rover);

    // merge with next block if empty
    if (rover->next != rover && MZ__IS_BLOCK_EMPTY(rover->next) && mz__isNextBlockAdjacent(rover))
    {
        memblock_t* next = rover->next;
        rover->size += next->size;
        rover->next = next->next;
        rover->next->prev = rover;
        zone->usedSize -= mz__headerSize();
        if (zone->rover == next)
        {
            zone->rover = rover;
        }
    }

    // merge with previous if empty
    if (rover->prev != rover && MZ__IS_BLOCK_EMPTY(rover->prev) && mz__isPrevBlockAdjacent(rover))
    {
        memblock_t* prev = rover->prev;
        prev->size += rover->size;
        prev->next = rover->next;
        prev->next->prev = prev;
        zone->usedSize -= mz__headerSize();
        zone->rover = prev;
    }

    mz__debugAssertValid(zone);
}

bool mz_contains(const memzone_t* zone, const void* p)
{
    return mz__findBlock(zone, p) != NULL;
}

size_t mz_allocationSize(const memzone_t* zone, const void* p)
{
    const memblock_t* block = mz__findBlock(zone, p);
    return mz__allocationSize(block);
}

bool mz_validate(const memzone_t* zone)
{
    if (zone == NULL || zone->rover == NULL)
    {
        return mz__validationFailure(zone, zone != NULL ? (const void*)zone->rover : NULL, "zone or rover pointer is null");
    }

    const uint8_t* zoneStart = (const uint8_t*)zone;
    const uint8_t* zoneEnd = zoneStart + zone->maxSize;
    const uint8_t* firstBlockStart = zoneStart + offsetof(memzone_t, blockList);
    const size_t headerSize = mz__headerSize();
    const uint8_t* expectedBlockStart = firstBlockStart;

    if (zone->maxSize < mz__zoneBaseSize())
    {
        return mz__validationFailure(zone, zone, "zone max size is smaller than allocator base size");
    }

    if (zone->usedSize > zone->maxSize)
    {
        return mz__validationFailure(zone, zone, "used size exceeds zone size");
    }

    size_t totalBlockSize = 0;
    size_t totalAllocatedPayload = 0;
    int32_t blockCount = 0;
    bool roverFound = false;
    const memblock_t* previousBlock = NULL;

    const memblock_t* rover = &zone->blockList;
    do
    {
        const uint8_t* blockStart = (const uint8_t*)rover;
        const uint8_t* blockEnd = blockStart + rover->size;

        if (blockStart != expectedBlockStart)
        {
            return mz__validationFailure(zone, rover, "there is a gap or overlap between consecutive blocks");
        }

        if (blockStart < firstBlockStart || blockEnd > zoneEnd || rover->size < headerSize)
        {
            return mz__validationFailure(zone, rover, "block bounds are outside the zone or smaller than the header");
        }

        if (rover->next == NULL || rover->prev == NULL)
        {
            return mz__validationFailure(zone, rover, "block links contain a null pointer");
        }

        if (previousBlock != NULL && rover->prev != previousBlock)
        {
            return mz__validationFailure(zone, rover, "block links are not bidirectionally consistent");
        }

        if (((uintptr_t)blockStart % mz_alignment()) != 0)
        {
            return mz__validationFailure(zone, rover, "block header is not aligned to the allocator alignment");
        }

        if (rover->next != &zone->blockList)
        {
            if (blockEnd != (const uint8_t*)rover->next)
            {
                return mz__validationFailure(zone, rover, "there is a gap or overlap between consecutive blocks");
            }

            if (MZ__IS_BLOCK_EMPTY(rover) && MZ__IS_BLOCK_EMPTY(rover->next))
            {
                return mz__validationFailure(zone, rover, "two adjacent free blocks should have been coalesced");
            }
        }

        if (!MZ__IS_BLOCK_EMPTY(rover))
        {
            const uint8_t* payloadStart = (const uint8_t*)mz__payloadPointer((memblock_t*)rover);
            if ((const uint8_t*)rover->user < payloadStart || (const uint8_t*)rover->user >= blockEnd)
            {
                return mz__validationFailure(zone, rover, "user pointer is outside the owning block payload range");
            }

            if (((uintptr_t)rover->user % mz_alignment()) != 0)
            {
                return mz__validationFailure(zone, rover, "user pointer is not aligned to the allocator alignment");
            }

            totalAllocatedPayload += mz__payloadSize(rover);
        }

        if (rover == zone->rover)
        {
            roverFound = true;
        }

        previousBlock = rover;
        totalBlockSize += rover->size;
        blockCount++;
        expectedBlockStart = blockEnd;
        rover = rover->next;
    } while (rover != &zone->blockList);

    if (zone->blockList.prev != previousBlock)
    {
        return mz__validationFailure(zone, &zone->blockList, "block links are not bidirectionally consistent");
    }

    if (!roverFound)
    {
        return mz__validationFailure(zone, zone->rover, "rover does not point at a block in the circular list");
    }

    if (totalBlockSize != zone->maxSize - offsetof(memzone_t, blockList))
    {
        return mz__validationFailure(zone, &zone->blockList, "total block size does not match the zone payload span");
    }

    if (expectedBlockStart != zoneEnd)
    {
        return mz__validationFailure(zone, &zone->blockList, "the block list does not cover the full zone payload span");
    }

    if (zone->usedSize != mz__zoneBaseSize() + totalAllocatedPayload + (size_t)(blockCount - 1) * headerSize)
    {
        return mz__validationFailure(zone, zone, "used size does not match allocated payload plus allocator metadata");
    }

    return true;
}

int32_t mz_blockCount(const memzone_t* zone)
{
    if (zone == NULL)
    {
        return 0;
    }

    int32_t numberOfBlocks = 0;

    const memblock_t* rover = &zone->blockList;
    do
    {
        numberOfBlocks++;
        rover = rover->next;
    } while (rover != &zone->blockList);

    return numberOfBlocks;
}

size_t mz_usableFreeSize(const memzone_t* zone)
{
    if (zone == NULL)
    {
        return 0;
    }

    size_t usableFreeSize = 0;

    const memblock_t* rover = &zone->blockList;
    do
    {
        if (!rover->user)
        {
            usableFreeSize += mz__payloadSize(rover);
        }

        rover = rover->next;
    } while (rover != &zone->blockList);

    return usableFreeSize;
}

float mz_fragmentation(const memzone_t* zone)
{
    if (zone == NULL)
    {
        return 0.0f;
    }

    /**
     * (free - freemax)
     * ----------------   x 100%    (or 100% for free=0)
     *      free
     * where
     * free     = total number of bytes free
     * freemax  = size of largest free block
     */

    size_t free = 0;
    size_t freeMax = 0;

    const memblock_t* rover = &zone->blockList;
    do
    {
        if (!rover->user)
        {
            size_t freeSizeOnBlock = mz__payloadSize(rover);
            free += freeSizeOnBlock;

            if (freeSizeOnBlock > freeMax)
                freeMax = freeSizeOnBlock;
        }

        rover = rover->next;
    } while (rover != &zone->blockList);

    return free > 0 ? ((float)(free - freeMax) / free) * 100 : 0;
}

#endif // SHL_MEMORY_ZONE_IMPLEMENTATION
#endif // SHL_MEMORY_ZONE_H
