/*
    binary_heap.h - acoto87 (acoto87@gmail.com)

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

    Single-header macro library to declare and define strongly typed binary
    heaps backed by a dynamically resized array.

    USAGE
    Declare a heap type with shlDeclareBinaryHeap(name, type), then define it
    once with shlDefineBinaryHeap(name, type) in a C source file.

    ALLOCATION
    Pass an shl_allocator_t* to Init to control where the items buffer lives.
    Use shl_heap_alloc() for the default system heap.  To use a memzone_t,
    include memzone.h before this header and call shl_zone_alloc(zone).

    ITEM OWNERSHIP
    The heap stores values by copy and does not manage the lifecycle of
    items.  The caller is responsible for freeing any resources owned by
    items before calling Clear or Free.

    SEARCH
    IndexOf and Contains require an explicit equality function at the call
    site rather than storing one in the heap struct.

    OUT-OF-RANGE READS
    Peek and Pop return a zero-initialised value when the heap is empty.

    NOTES
    This implementation behaves as a min-heap according to compareFn.  Push,
    Pop, and Update all restore heap order automatically.
*/

#ifndef SHL_HEAP_H
#define SHL_HEAP_H

#include "internal.h"

#define shlDeclareBinaryHeap(typeName, itemType) \
    typedef struct \
    { \
        int32_t count; \
        int32_t capacity; \
        shl_allocator_t* alloc; \
        int32_t (*compareFn)(const itemType item1, const itemType item2); \
        itemType* items; \
    } typeName; \
    \
    void typeName ## Init(typeName* heap, shl_allocator_t* alloc, int32_t (*compareFn)(const itemType item1, const itemType item2)); \
    void typeName ## Free(typeName* heap); \
    void typeName ## Push(typeName* heap, itemType value); \
    itemType typeName ## Peek(typeName* heap); \
    itemType typeName ## Pop(typeName* heap); \
    int32_t typeName ## IndexOf(typeName* heap, itemType value, bool (*equalsFn)(const itemType, const itemType)); \
    bool typeName ## Contains(typeName* heap, itemType value, bool (*equalsFn)(const itemType, const itemType)); \
    void typeName ## Update(typeName* heap, int32_t index, itemType newValue); \
    void typeName ## Clear(typeName* heap);

#define shlDefineBinaryHeap(typeName, itemType) \
    void typeName ## __heapUp(typeName* heap, int32_t index) \
    { \
        int32_t pindex = (index - 1) >> 1; \
        while (index > 0 && heap->compareFn(heap->items[index], heap->items[pindex]) < 0) \
        { \
            itemType tmp = heap->items[index]; \
            heap->items[index] = heap->items[pindex]; \
            heap->items[pindex] = tmp; \
            \
            index = pindex; \
            pindex = (index - 1) >> 1; \
        } \
    } \
    \
    void typeName ## __heapDown(typeName* heap, int32_t index) \
    { \
        while (index < heap->count) \
        { \
            itemType value = heap->items[index]; \
            \
            int32_t leftIndex = 2 * index + 1; \
            if (leftIndex >= heap->count) \
                break; \
            \
            int32_t minIndex = leftIndex; \
            itemType minValue = heap->items[minIndex]; \
            \
            int32_t rightIndex = 2 * index + 2; \
            if (rightIndex < heap->count) \
            { \
                itemType rightValue = heap->items[rightIndex]; \
                if (heap->compareFn(rightValue, minValue) < 0) \
                { \
                    minIndex = rightIndex; \
                    minValue = rightValue; \
                } \
            } \
            \
            if (heap->compareFn(minValue, value) >= 0) \
                break; \
            \
            itemType tmp = heap->items[index]; \
            heap->items[index] = heap->items[minIndex]; \
            heap->items[minIndex] = tmp; \
            \
            index = minIndex; \
        } \
    } \
    \
    void typeName ## Init(typeName* heap, shl_allocator_t* alloc, int32_t (*compareFn)(const itemType item1, const itemType item2)) \
    { \
        *heap = (typeName){ 0 }; \
        if (!alloc) alloc = shl_heap_alloc(); \
        if (!alloc || !alloc->mallocFn || !compareFn) return; \
        heap->alloc     = alloc; \
        heap->compareFn = compareFn; \
        heap->capacity  = SHL__INITIAL_CAPACITY; \
        heap->count     = 0; \
        heap->items     = (itemType*)alloc->mallocFn(alloc->ctx, (size_t)heap->capacity * sizeof(itemType)); \
    } \
    \
    void typeName ## Free(typeName* heap) \
    { \
        heap->count = 0; \
        if (heap->items && heap->alloc && heap->alloc->freeFn) \
            heap->alloc->freeFn(heap->alloc->ctx, heap->items); \
        heap->items = NULL; \
    } \
    \
    void typeName ## Push(typeName* heap, itemType value) \
    { \
        if (!heap->items) \
            return; \
        \
        if (heap->count + 1 >= heap->capacity) { \
            if (!shl__resizeArray((void**)&heap->items, &heap->capacity, heap->count + 1, sizeof(itemType), heap->alloc)) \
                return; \
        } \
        \
        int32_t index = heap->count; \
        heap->items[index] = value; \
        \
        typeName ## __heapUp(heap, index); \
        heap->count++; \
    } \
    \
    itemType typeName ## Peek(typeName* heap) \
    { \
        if (!heap->items || heap->count == 0) \
        { \
            itemType zero; \
            memset(&zero, 0, sizeof(itemType)); \
            return zero; \
        } \
        return heap->items[0]; \
    } \
    \
    itemType typeName ## Pop(typeName* heap) \
    { \
        if (!heap->items || heap->count == 0) \
        { \
            itemType zero; \
            memset(&zero, 0, sizeof(itemType)); \
            return zero; \
        } \
        \
        itemType returnValue = heap->items[0]; \
        \
        heap->items[0] = heap->items[heap->count - 1]; \
        heap->count--; \
        typeName ## __heapDown(heap, 0); \
        \
        return returnValue; \
    } \
    \
    int32_t typeName ## IndexOf(typeName* heap, itemType value, bool (*equalsFn)(const itemType, const itemType)) \
    { \
        if (!heap->items || !equalsFn) \
            return -1; \
        \
        for (int32_t i = 0; i < heap->count; i++) \
        { \
            if (equalsFn(heap->items[i], value)) \
                return i; \
        } \
        \
        return -1; \
    } \
    \
    bool typeName ## Contains(typeName* heap, itemType value, bool (*equalsFn)(const itemType, const itemType)) \
    { \
        return typeName ## IndexOf(heap, value, equalsFn) >= 0; \
    } \
    \
    void typeName ## Update(typeName* heap, int32_t index, itemType newValue) \
    { \
        if (!heap->items || !heap->compareFn) \
            return; \
        \
        if (index < 0 || index >= heap->count) \
            return; \
        \
        itemType oldValue = heap->items[index]; \
        heap->items[index] = newValue; \
        int32_t cmpValue = heap->compareFn(newValue, oldValue); \
        if (cmpValue < 0) \
            typeName ## __heapUp(heap, index); \
        else if (cmpValue > 0) \
            typeName ## __heapDown(heap, index); \
    } \
    \
    void typeName ## Clear(typeName* heap) \
    { \
        heap->count = 0; \
    }

#endif // SHL_HEAP_H
