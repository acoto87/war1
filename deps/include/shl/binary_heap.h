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

    This is a single header file with macros to declare and define a strongly typed heap of objects.
*/
#ifndef SHL_HEAP_H
#define SHL_HEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define shlDeclareBinaryHeap(typeName, itemType) \
    typedef struct \
    { \
        itemType defaultValue; \
        bool (*equalsFn)(const itemType item1, const itemType item2); \
        int32_t (*compareFn)(const itemType item1, const itemType item2); \
        void (*freeFn)(itemType item); \
    } typeName ## Options; \
    \
    typedef struct \
    { \
        uint32_t count; \
        uint32_t capacity; \
        bool (*equalsFn)(const itemType item1, const itemType item2); \
        int32_t (*compareFn)(const itemType item1, const itemType item2); \
        void (*freeFn)(itemType item); \
        itemType defaultValue; \
        itemType* items; \
    } typeName; \
    \
    void typeName ## Init(typeName* heap, typeName ## Options options); \
    void typeName ## Free(typeName* heap); \
    void typeName ## Push(typeName* heap, itemType value); \
    itemType typeName ## Peek(typeName* heap); \
    itemType typeName ## Pop(typeName* heap); \
    int32_t typeName ## IndexOf(typeName* heap, itemType value); \
    bool typeName ## Contains(typeName* heap, itemType value); \
    void typeName ## Update(typeName* heap, int32_t index, itemType newValue); \
    void typeName ## Clear(typeName* heap);

#define shlDefineBinaryHeap(typeName, itemType) \
    void typeName ## __resize(typeName* heap, int32_t minSize) \
    { \
        heap->capacity = heap->capacity << 1; \
        if (heap->capacity < minSize) \
            heap->capacity = minSize; \
        \
        heap->items = (itemType *)realloc(heap->items, heap->capacity * sizeof(itemType)); \
    } \
    \
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
    void typeName ## Init(typeName* heap, typeName ## Options options) \
    { \
        heap->capacity = 8; \
        heap->defaultValue = options.defaultValue; \
        heap->equalsFn = options.equalsFn; \
        heap->compareFn = options.compareFn; \
        heap->freeFn = options.freeFn; \
        heap->count = 0; \
        heap->items = (itemType *)malloc(heap->capacity * sizeof(itemType)); \
    } \
    \
    void typeName ## Free(typeName* heap) \
    { \
        if (!heap->items) \
            return; \
        \
        typeName ## Clear(heap); \
        \
        free(heap->items); \
        heap->items = 0; \
    } \
     \
    void typeName ## Push(typeName* heap, itemType value) \
    { \
        if (!heap->items) \
            return; \
        \
        if (heap->count + 1 >= heap->capacity) \
            typeName ## __resize(heap, heap->count + 1); \
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
        if (!heap->items) \
            return heap->defaultValue; \
        \
        if (heap->count == 0) \
            return heap->defaultValue; \
        \
        return heap->items[0]; \
    } \
    \
    itemType typeName ## Pop(typeName* heap) \
    { \
        if (!heap->items) \
            return heap->defaultValue; \
        \
        if (heap->count == 0) \
            return heap->defaultValue; \
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
    int32_t typeName ## IndexOf(typeName* heap, itemType value) \
    { \
        if (!heap->items) \
            return -1; \
        \
        if (!heap->equalsFn) \
            return -1; \
        \
        for(int32_t i = 0; i < heap->count; i++) \
        { \
            if (heap->equalsFn(heap->items[i], value)) \
                return i; \
        } \
        \
        return -1; \
    } \
    \
    bool typeName ## Contains(typeName* heap, itemType value) \
    { \
        return typeName ## IndexOf(heap, value) >= 0; \
    } \
    \
    void typeName ## Update(typeName* heap, int32_t index, itemType newValue) \
    { \
        if (!heap->items) \
            return; \
        \
        if (!heap->compareFn) \
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
        if (!heap->items) \
            return; \
        \
        if (heap->freeFn) \
        { \
            for(int32_t i = 0; i < heap->count; i++) \
                heap->freeFn(heap->items[i]); \
        } \
        \
        heap->count = 0; \
    }

#endif // SHL_HEAP_H