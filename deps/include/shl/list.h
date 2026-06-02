/*
    list.h - acoto87 (acoto87@gmail.com)

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

    Single-header macro library to declare and define strongly typed dynamic
    lists with indexed access plus search, sort, and range operations.

    USAGE
    Include this header in one or more C translation units, then use
    shlDeclareList(name, type) in a header and shlDefineList(name, type) in a
    single C file.

    ALLOCATION
    Pass an shl_allocator_t* to Init to control where the items buffer lives.
    Use shl_heap_alloc() for the default system heap.  To use a memzone_t,
    include memzone.h before this header and call shl_zone_alloc(zone).

    For a fixed-capacity list with no heap involvement at all, use InitFixed
    and supply a caller-owned buffer.  Free is a safe no-op on fixed lists.

    ITEM OWNERSHIP
    The list stores values by copy and does not manage the lifecycle of the
    items themselves.  The caller is responsible for freeing any resources
    owned by items before calling Remove, RemoveAt, RemoveAtRange, Clear, or
    Free.

    SEARCH
    IndexOf, Contains, and Remove require an explicit equality function at the
    call site rather than storing one in the list struct.

    OUT-OF-RANGE READS
    Get returns a zero-initialised value when the index is out of range.
*/

#ifndef SHL_LIST_H
#define SHL_LIST_H

#include "internal.h"

#define shlDeclareList(typeName, itemType) \
    typedef struct \
    { \
        int32_t count; \
        int32_t capacity; \
        shl_allocator_t* alloc; \
        itemType* items; \
    } typeName; \
    \
    void typeName ## Init(typeName* list, shl_allocator_t* alloc); \
    void typeName ## InitFixed(typeName* list, itemType* buffer, int32_t capacity); \
    void typeName ## Free(typeName* list); \
    void typeName ## Add(typeName* list, itemType value); \
    void typeName ## AddRange(typeName* list, int32_t count, itemType values[]); \
    void typeName ## Insert(typeName* list, int32_t index, itemType value); \
    void typeName ## InsertRange(typeName* list, int32_t index, int32_t count, itemType values[]); \
    int32_t typeName ## IndexOf(typeName* list, itemType value, bool (*equalsFn)(const itemType, const itemType)); \
    itemType typeName ## Get(typeName* list, int32_t index); \
    void typeName ## Set(typeName* list, int32_t index, itemType value); \
    bool typeName ## Contains(typeName* list, itemType value, bool (*equalsFn)(const itemType, const itemType)); \
    void typeName ## Remove(typeName* list, itemType value, bool (*equalsFn)(const itemType, const itemType)); \
    void typeName ## RemoveAt(typeName* list, int32_t index); \
    void typeName ## RemoveAtRange(typeName* list, int32_t index, int32_t count); \
    void typeName ## Clear(typeName* list); \
    void typeName ## Reverse(typeName* list); \
    void typeName ## Sort(typeName* list, int32_t (*compareFn)(const itemType item1, const itemType item2, void* userdata), void* userdata); \
    void typeName ## CopyTo(typeName* list, itemType array[], int32_t index); \
    itemType* typeName ## ToArray(typeName* list); \

#define shlDefineList(typeName, itemType) \
    void typeName ## __qsort(typeName* list, int32_t left, int32_t right, int32_t (*compareFn)(const itemType item1, const itemType item2, void* userdata), void* userdata) \
    { \
        if (left >= right) \
            return; \
        \
        int32_t middle = left + ((right - left) >> 1); \
        itemType p = list->items[middle]; \
        \
        int32_t i = left - 1; \
        int32_t j = right + 1; \
        \
        while (i < j) \
        { \
            do { i++; } while (compareFn(list->items[i], p, userdata) < 0); \
            do { j--; } while (compareFn(list->items[j], p, userdata) > 0); \
            \
            if (i >= j) \
                break; \
            \
            itemType tmp = list->items[i]; \
            list->items[i] = list->items[j]; \
            list->items[j] = tmp; \
        } \
        \
        typeName ## __qsort(list, left, j, compareFn, userdata); \
        typeName ## __qsort(list, j + 1, right, compareFn, userdata); \
    } \
    \
    void typeName ## Init(typeName* list, shl_allocator_t* alloc) \
    { \
        *list = (typeName){ 0 }; \
        if (!alloc) alloc = shl_heap_alloc(); \
        if (!alloc || !alloc->mallocFn) return; \
        list->alloc    = alloc; \
        list->capacity = SHL__INITIAL_CAPACITY; \
        list->count    = 0; \
        list->items    = (itemType*)alloc->mallocFn(alloc->ctx, (size_t)list->capacity * sizeof(itemType)); \
    } \
    \
    void typeName ## InitFixed(typeName* list, itemType* buffer, int32_t capacity) \
    { \
        list->alloc    = NULL; \
        list->capacity = capacity; \
        list->count    = 0; \
        list->items    = buffer; \
    } \
    \
    void typeName ## Free(typeName* list) \
    { \
        list->count = 0; \
        if (list->items && list->alloc && list->alloc->freeFn) \
            list->alloc->freeFn(list->alloc->ctx, list->items); \
        list->items = NULL; \
    } \
    \
    void typeName ## InsertRange(typeName* list, int32_t index, int32_t count, itemType values[]) \
    { \
        if (!list->items) \
            return; \
        \
        if (index < 0 || index > list->count) \
            return; \
        \
        if (list->count + count > list->capacity) \
        { \
            if (!shl__resizeArray((void**)&list->items, &list->capacity, list->count + count, sizeof(itemType), list->alloc)) \
                return; \
        } \
        \
        memmove(list->items + index + count, list->items + index, (list->count - index) * sizeof(itemType)); \
        memcpy(list->items + index, values, count * sizeof(itemType)); \
        list->count += count; \
    } \
    \
    void typeName ## Insert(typeName* list, int32_t index, itemType value) \
    { \
        typeName ## InsertRange(list, index, 1, &value); \
    } \
    \
    void typeName ## Add(typeName* list, itemType value) \
    { \
        typeName ## Insert(list, list->count, value); \
    } \
    \
    void typeName ## AddRange(typeName* list, int32_t count, itemType values[]) \
    { \
        typeName ## InsertRange(list, list->count, count, values); \
    } \
    \
    int32_t typeName ## IndexOf(typeName* list, itemType value, bool (*equalsFn)(const itemType, const itemType)) \
    { \
        if (!list->items || !equalsFn) \
            return -1; \
        \
        for (int32_t i = 0; i < list->count; i++) \
        { \
            if (equalsFn(list->items[i], value)) \
                return i; \
        } \
        \
        return -1; \
    } \
    \
    bool typeName ## Contains(typeName* list, itemType value, bool (*equalsFn)(const itemType, const itemType)) \
    { \
        return typeName ## IndexOf(list, value, equalsFn) >= 0; \
    } \
    \
    itemType typeName ## Get(typeName* list, int32_t index) \
    { \
        if (!list->items || index < 0 || index >= list->count) \
        { \
            itemType zero; \
            memset(&zero, 0, sizeof(itemType)); \
            return zero; \
        } \
        return list->items[index]; \
    } \
    \
    void typeName ## Set(typeName* list, int32_t index, itemType value) \
    { \
        if (!list->items || index < 0 || index >= list->count) \
            return; \
        list->items[index] = value; \
    } \
    \
    void typeName ## RemoveAtRange(typeName* list, int32_t index, int32_t count) \
    { \
        if (!list->items || index < 0 || index >= list->count) \
            return; \
        \
        if (index + count > list->count) \
            return; \
        \
        memmove(list->items + index, list->items + index + count, (list->count - index - count) * sizeof(itemType)); \
        list->count -= count; \
    } \
    \
    void typeName ## RemoveAt(typeName* list, int32_t index) \
    { \
        typeName ## RemoveAtRange(list, index, 1); \
    } \
    \
    void typeName ## Remove(typeName* list, itemType value, bool (*equalsFn)(const itemType, const itemType)) \
    { \
        int32_t index = typeName ## IndexOf(list, value, equalsFn); \
        if (index >= 0) \
            typeName ## RemoveAt(list, index); \
    } \
    \
    void typeName ## Clear(typeName* list) \
    { \
        list->count = 0; \
    } \
    \
    void typeName ## Reverse(typeName* list) \
    { \
        if (!list->items) \
            return; \
        \
        int32_t count = list->count; \
        for (int32_t i = 0; i < count / 2; i++) \
        { \
            itemType tmp = list->items[i]; \
            list->items[i] = list->items[count - i - 1]; \
            list->items[count - i - 1] = tmp; \
        } \
    } \
    \
    void typeName ## Sort(typeName* list, int32_t (*compareFn)(const itemType item1, const itemType item2, void* userdata), void* userdata) \
    { \
        if (!list->items || list->count < 2) \
            return; \
        typeName ## __qsort(list, 0, list->count - 1, compareFn, userdata); \
    } \
    \
    void typeName ## CopyTo(typeName* list, itemType array[], int32_t index) \
    { \
        if (!list->items) \
            return; \
        \
        if (index >= 0) \
            memcpy(array + index, list->items, list->count * sizeof(itemType)); \
    } \
    \
    itemType* typeName ## ToArray(typeName* list) \
    { \
        if (!list->items || !list->alloc || !list->alloc->mallocFn) return NULL; \
        itemType* array = (itemType*)list->alloc->mallocFn(list->alloc->ctx, list->count * sizeof(itemType)); \
        if (array) memcpy(array, list->items, list->count * sizeof(itemType)); \
        return array; \
    }

#endif // SHL_LIST_H
