/*
    set.h - acoto87 (acoto87@gmail.com)

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

    Single-header macro library to declare and define strongly typed hash sets.
    Callers provide hash and equality hooks for the generated item type.

    USAGE
    Declare a set type with shlDeclareSet(name, type), then define it once
    with shlDefineSet(name, type) in a C source file.

    CUSTOMISATION
    Provide a hash function and equality function for the item type, plus a
    default value and optional free function for owned items. Items are stored
    by copy.

    NOTES
    This set uses hash buckets with collision chains stored inside the entry
    array. Add returns false when the item is already present. Call Free to
    release internal storage.
*/

#ifndef SHL_SET_H
#define SHL_SET_H

#include "shl_internal.h"

#define shlDeclareSet(typeName, itemType) \
    typedef struct \
    { \
        itemType defaultValue; \
        uint32_t (*hashFn)(const itemType item); \
        bool (*equalsFn)(const itemType item1, const itemType item2); \
        void (*freeFn)(itemType item); \
    } typeName ## Options; \
    \
    typedef struct { \
        bool active; \
        uint32_t hash; \
        int32_t next; \
        itemType item; \
    } typeName ## __Entry__; \
    \
    typedef struct { \
        int32_t count; \
        int32_t capacity; \
        int32_t loadFactor; \
        int32_t shift; \
        uint32_t (*hashFn)(const itemType item); \
        bool (*equalsFn)(const itemType item1, const itemType item2); \
        void (*freeFn)(itemType item); \
        itemType defaultValue; \
        typeName ## __Entry__* entries; \
    } typeName; \
    \
    void typeName ## Init(typeName* map, typeName ## Options options); \
    void typeName ## Free(typeName* map); \
    bool typeName ## Add(typeName* set, itemType item); \
    bool typeName ## Contains(typeName* set, itemType item); \
    void typeName ## Remove(typeName* set, itemType item); \
    void typeName ## Clear(typeName* set); \

#define shlDefineSet(typeName, itemType) \
    static void typeName ## __resize(typeName* set); \
    \
    static void typeName ## __resize(typeName* set) \
    { \
        int32_t oldCapacity = set->capacity; \
        typeName ## __Entry__* old = set->entries; \
        \
        set->loadFactor = oldCapacity; \
        set->capacity = 1 << (32 - (--set->shift)); \
        set->entries = (typeName ## __Entry__*)calloc(set->capacity, sizeof(typeName ## __Entry__)); \
        set->count = 0; \
        \
        for(int32_t i = 0; i < oldCapacity; i++) \
        { \
            if(old[i].active) \
                typeName ## Add(set, old[i].item); \
        } \
        SHL_FREE(old); \
    } \
    \
    void typeName ## Init(typeName* set, typeName ## Options options) \
    { \
        set->defaultValue = options.defaultValue; \
        set->hashFn = options.hashFn; \
        set->equalsFn = options.equalsFn; \
        set->freeFn = options.freeFn; \
        set->shift = SHL__INITIAL_HASH_SHIFT; \
        set->capacity = SHL__INITIAL_CAPACITY; \
        set->loadFactor = SHL__INITIAL_HASH_LOAD_FACTOR; \
        set->count = 0; \
        set->entries = (typeName ## __Entry__ *)SHL_CALLOC((size_t)set->capacity, sizeof(typeName ## __Entry__)); \
    } \
    \
    void typeName ## Free(typeName* set) \
    { \
        if (!set->entries) \
            return; \
        \
        typeName ## Clear(set); \
        \
        SHL_FREE(set->entries); \
        set->entries = 0; \
    } \
    \
    bool typeName ## Add(typeName* set, itemType item) \
    { \
        if (!set->entries) \
            return false; \
        \
        if(set->count == set->loadFactor) \
            typeName ## __resize(set); \
        \
        uint32_t hash; \
        int32_t index; \
        int32_t next; \
        hash = index = shl__fibHash(set->hashFn(item), set->shift); \
        \
        while (set->entries[index].active) \
        { \
            if(set->entries[index].hash == hash && set->equalsFn(set->entries[index].item, item)) \
                return false; \
            \
            if (set->entries[index].next < 0) \
                break; \
            \
            index = set->entries[index].next; \
        } \
        \
        next = shl__findEmptyBucket(set->entries, set->capacity, index, sizeof(typeName ## __Entry__), offsetof(typeName ## __Entry__, active)); \
        if (next < 0) \
        { \
            typeName ## __resize(set); \
            return typeName ## Add(set, item); \
        } \
        if (index != next) \
            set->entries[index].next = next; \
        \
        set->entries[next].active = true; \
        set->entries[next].item = item; \
        set->entries[next].hash = hash; \
        set->entries[next].next = -1; \
        set->count++; \
        return true; \
    } \
    \
    bool typeName ## Contains(typeName* set, itemType item) \
    { \
        if (!set->entries) \
            return false; \
        \
        int32_t index; \
        uint32_t hash; \
        hash = index = shl__fibHash(set->hashFn(item), set->shift); \
        \
        bool found = false; \
        \
        while (set->entries[index].active) \
        { \
            if(set->entries[index].hash == hash && set->equalsFn(set->entries[index].item, item)) \
            { \
                found = true; \
                break; \
            } \
            \
            if (set->entries[index].next < 0) \
                break; \
            \
            index = set->entries[index].next; \
        } \
        \
        return found; \
    } \
    \
    void typeName ## Remove(typeName* set, itemType item) \
    { \
        if (!set->entries) \
            return; \
        \
        int32_t prevIndex, index; \
        uint32_t hash; \
        hash = prevIndex = index = shl__fibHash(set->hashFn(item), set->shift); \
        \
        while (set->entries[index].active) \
        { \
            if(set->entries[index].hash == hash && set->equalsFn(set->entries[index].item, item)) \
            { \
                itemType oldItem = set->entries[index].item; \
                int32_t nextIndex = set->entries[index].next; \
                if (nextIndex >= 0) \
                { \
                    set->entries[index] = set->entries[nextIndex]; \
                    set->entries[nextIndex].item = set->defaultValue; \
                    set->entries[nextIndex].next = -1; \
                    set->entries[nextIndex].active = false; \
                } \
                else \
                { \
                    if (prevIndex != index) \
                        set->entries[prevIndex].next = -1; \
                    set->entries[index].item = set->defaultValue; \
                    set->entries[index].next = -1; \
                    set->entries[index].active = false; \
                } \
                \
                if (set->freeFn) \
                    set->freeFn(oldItem); \
                \
                set->count--; \
                \
                break; \
            } \
            \
            if (set->entries[index].next < 0) \
                break; \
            \
            prevIndex = index; \
            index = set->entries[index].next; \
        } \
    } \
    \
    void typeName ## Clear(typeName* set) \
    { \
        if (!set->entries) \
            return; \
        \
        for(int32_t i = 0; i < set->capacity; i++) \
        { \
            if (set->entries[i].active) \
            { \
                if (set->freeFn) \
                    set->freeFn(set->entries[i].item); \
                \
                set->entries[i].item = set->defaultValue; \
                set->entries[i].next = -1; \
                set->entries[i].active = false; \
            } \
        } \
        \
        set->count = 0; \
    }

#endif //SHL_SET_H
