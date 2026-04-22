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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MEM_STATIC,                         // default block type, represent a normal block
    MEM_PURGE,                          // block type that can be reused when allocating, even if it's used
                                        // normally this type of block is created when free some STATIC block
    MEM_FIXED                           // block type that is fixed at the start of the app and will last the
                                        // entire app life-cycle and it can't be defragmented
} memtype_t;

typedef struct memblock_s
{
    size_t size;                        // size of the block
    memtype_t type;                     // the type of the block
    void* user;                         // a pointer to the pointer returned to the user
    struct memblock_s *next, *prev;     // pointers to next and prev in the block list
} memblock_t;

typedef struct
{
    size_t usedSize;                    // how much space is used without including blocks data
    size_t maxSize;                     // the max allowed size that can be allocated
    memblock_t* rover;                  // a pointer to a free block that is used when allocating
    memblock_t blockList;               // list of blocks, here is where the requested memory begins
} memzone_t;

memzone_t* mzInit(size_t maxSize);
void* mzAlloc(memzone_t* zone, size_t size);
void mzFree(memzone_t* zone, void* p);
int32_t mzGetNumberOfBlocks(memzone_t* zone);
size_t mzGetUsableFreeSize(memzone_t* zone);
float mzGetFragPercentage(memzone_t* zone);
// void mzDefrag(memzone_t* zone);
void mzPrint(memzone_t* zone, bool printBlocks, bool printMap);

#define mzIsBlockEmpty(block) ((block)->user == NULL)

#ifdef __cplusplus
}
#endif

#ifdef SHL_MEMORY_ZONE_IMPLEMENTATION

#define __pointerOffset(t, p, o) ((t*)((uint8_t*)(p) + (o)))

memzone_t* mzInit(size_t maxSize)
{
    if (maxSize < sizeof(memzone_t))
    {
        fprintf(stderr, "You need to allocate memory for at least %zu bytes.\n", sizeof(memzone_t));
        return NULL;
    }

    uint8_t* rawZone = (uint8_t*)malloc(maxSize);
    memzone_t* zone = (memzone_t*)rawZone;
    if (!zone)
    {
        fprintf(stderr, "The system couldn't allocate memory for %zu bytes.\n", maxSize);
        return NULL;
    }

    zone->usedSize = sizeof(memzone_t);
    zone->maxSize = maxSize;

    zone->blockList = (memblock_t){0};
    zone->blockList.size = maxSize - (sizeof(memzone_t) - sizeof(memblock_t));
    zone->blockList.type = MEM_STATIC;
    zone->blockList.user = NULL;
    zone->blockList.next = &zone->blockList;
    zone->blockList.prev = &zone->blockList;

    zone->rover = &zone->blockList;

    return zone;
}

void* mzAlloc(memzone_t* zone, size_t size)
{
    size_t sizeToAlloc = size + sizeof(memblock_t);
    if (sizeToAlloc > zone->maxSize - zone->usedSize)
    {
        fprintf(stderr, "Memory overflow: There is no more memory to alloc size %zu bytes.\n", size);
        return NULL;
    }

    memblock_t* rover = zone->rover;
    if (rover->user || rover->size < sizeToAlloc)
    {
        rover = rover->next;
        while (rover->user || rover->size < sizeToAlloc)
        {
            // if the rover pointer traversed the entire list and didn't
            // find any block to alloc the memory return null, maybe a defrag?
            if (rover == zone->rover)
            {
                fprintf(stderr, "There is total free memory to alloc size %zu bytes but there isn't a block big enough.\n", size);
                return NULL;
            }

            rover = rover->next;
        }
    }

    // if sizeToAlloc is less or equals to the block size,
    // allocate there and not split the block otherwise,
    // create a new block and split the current
    if (rover->size > sizeToAlloc)
    {
        // create a new empty block with the remaining free space
        memblock_t* newBlock = __pointerOffset(memblock_t, rover, sizeToAlloc);
        newBlock->size = rover->size - sizeToAlloc;
        newBlock->type = MEM_STATIC;
        newBlock->user = NULL;
        newBlock->prev = rover;
        newBlock->next = rover->next;

        // set the next block to point to the new one
        rover->next->prev = newBlock;
        rover->next = newBlock;
        rover->size = sizeToAlloc;

        // update the zone rover with the new free block created
        zone->rover = newBlock;
        zone->usedSize += sizeof(memblock_t);
    }

    zone->usedSize += size;

    rover->user = __pointerOffset(void, rover, sizeof(memblock_t));
    return rover->user;
}

void mzFree(memzone_t* zone, void* p)
{
    // this function doesn't need to iterate over the list
    // if it does this:
    // memblock_t* block = (memblock_t *) ( (byte *)p - sizeof(memblock_t));
    // because the pointers returned by `mzAlloc` are just the start of the block
    // plus the size of the memblock_t, so this function should expect that
    // those pointer are the ones to be free
    memblock_t* rover = &zone->blockList;
    if (!rover->user || rover->user != p)
    {
        rover = rover->next;
        while (!rover->user || rover->user != p)
        {
            // if the rover pointer traversed the entire list and didn't
            // find any block to de-alloc, just return
            if (rover == &zone->blockList)
            {
                return;
            }

            rover = rover->next;
        }
    }

    rover->user = NULL;
    zone->rover = rover;

    zone->usedSize -= rover->size - sizeof(memblock_t);

    // merge with next block if empty
    if (rover->next != rover && mzIsBlockEmpty(rover->next))
    {
        memblock_t* next = rover->next;
        rover->size += next->size;
        rover->next = next->next;
        rover->next->prev = rover;
        zone->usedSize -= sizeof(memblock_t);
        if (zone->rover == next)
            zone->rover = rover;
    }

    // merge with previous if empty
    if (rover->prev != rover && mzIsBlockEmpty(rover->prev))
    {
        memblock_t* prev = rover->prev;
        prev->size += rover->size;
        prev->next = rover->next;
        prev->next->prev = prev;
        zone->usedSize -= sizeof(memblock_t);
        zone->rover = prev;
    }
}

int32_t mzGetNumberOfBlocks(memzone_t* zone)
{
    int32_t numberOfBlocks = 0;

    memblock_t* rover = &zone->blockList;
    do
    {
        numberOfBlocks++;
        rover = rover->next;
    } while (rover != &zone->blockList);

    return numberOfBlocks;
}

size_t mzGetUsableFreeSize(memzone_t* zone)
{
    size_t usableFreeSize = 0;

    memblock_t* rover = &zone->blockList;
    do
    {
        if (!rover->user)
            usableFreeSize += rover->size - sizeof(memblock_t);

        rover = rover->next;
    } while (rover != &zone->blockList);

    return usableFreeSize;
}

float mzGetFragPercentage(memzone_t* zone)
{
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

    memblock_t* rover = &zone->blockList;
    do
    {
        if (!rover->user)
        {
            size_t freeSizeOnBlock = rover->size - sizeof(memblock_t);
            free += freeSizeOnBlock;

            if (freeSizeOnBlock > freeMax)
                freeMax = freeSizeOnBlock;
        }

        rover = rover->next;
    } while (rover != &zone->blockList);

    return free > 0 ? ((float)(free - freeMax) / free) * 100 : 0;
}

// void mzDefrag(memzone_t* zone)
// {
//     memblock_t* start = &zone->blockList;
//     do
//     {
//         if (!start->user)
//         {
//             size_t s = 0;

//             memblock_t* end = start;
//             while (!end->user)
//             {
//                 s += end->size;
//                 end = end->next;
//             }

//             if (end == &zone->blockList)
//             {
//                 start->size = s;
//                 start->next = end;

//                 end->prev = start;
//                 break;
//             }

//             memblock_t prevStartBlock = *start;
//             memblock_t prevEndBlock = *end;

//             memmove(start, end, end->size);

//             size_t currentStartSize = start->size;

//             memblock_t* newBlock = __pointerOffset(memblock_t, start, currentStartSize);
//             newBlock->size = prevStartBlock.size;
//             newBlock->type = MEM_STATIC;
//             newBlock->user = NULL;
//             newBlock->prev = start;
//             newBlock->next = prevEndBlock.next;

//             start->prev = prevStartBlock.prev;
//             start->next = newBlock;

//             newBlock->next->prev = newBlock;
//         }

//         start = start->next;
//     } while (start != &zone->blockList);
// }

void mzPrint(memzone_t* zone, bool printBlocks, bool printMap)
{
    printf("Zone: %p -> %p\n", (void*)zone, (void*)__pointerOffset(memzone_t, zone, zone->maxSize));
    printf("  rover: %p\n", (void*)zone->rover);
    printf("  maxSize: %zu\n", zone->maxSize);
    printf("  usedSize: %zu\n", zone->usedSize);
    printf("  freeSize: %zu\n", mzGetUsableFreeSize(zone));
    printf("  numberOfBlocks: %d\n", mzGetNumberOfBlocks(zone));
    printf("  fragmentation: %.2f%%\n", mzGetFragPercentage(zone));

    if (printBlocks)
    {
        printf("  Blocks:\n");
        memblock_t* rover = &zone->blockList;
        do
        {
            printf("    ----------\n");
            printf("    id: %p\n", (void*)rover);
            printf("    size: %zu\n", rover->size);
            printf("    type: %u\n", rover->type);
            printf("    user: %p\n", rover->user);
            printf("    prev: %p\n", (void*)rover->prev);
            printf("    next: %p\n", (void*)rover->next);
            printf("    ----------\n");

            rover = rover->next;
        } while (rover != &zone->blockList);
    }

    if (printMap)
    {
        printf("  Map:\n");

        char buffer[201];

        memblock_t* rover = &zone->blockList;

        int32_t numberOfBlocks = mzGetNumberOfBlocks(zone);
        int32_t lines = (int32_t)ceilf((float)numberOfBlocks / 200);
        while (lines--)
        {
            memset(buffer, 0, sizeof(buffer));

            for (int32_t i = 0; i < 200; i++)
            {
                buffer[i] = mzIsBlockEmpty(rover) ? '-' : '+';

                rover = rover->next;
                if (rover == &zone->blockList)
                    break;
            }

            printf("%s\n", buffer);
        }
    }
}

#endif // SHL_MEMORY_ZONE_IMPLEMENTATION
#endif // SHL_MEMORY_ZONE_H
