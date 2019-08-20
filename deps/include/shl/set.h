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

    This is a single header file with macros to declare and define a strongly typed list of objects that can be accessed by index. 
    Provides methods to search, sort, and manipulate lists.
*/

#ifndef SHL_SET_H
#define SHL_SET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

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
        uint32_t count; \
        uint32_t capacity; \
        uint32_t loadFactor; \
        uint32_t shift; \
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
    static uint32_t typeName ## __fibHash(uint32_t hash, uint32_t shift) \
    { \
        const uint32_t hashConstant = 2654435769u; \
        return (hash * hashConstant) >> shift; \
    } \
    \
    static uint32_t typeName ## __findEmptyBucket(typeName* set, uint32_t index) \
    { \
        for(int32_t i = 0; i < set->capacity; i++) \
        { \
            if (!set->entries[(index + i) % set->capacity].active) \
                return (index + i) % set->capacity; \
        } \
        \
        return -1; \
    } \
    \
    static void typeName ## __resize(typeName* set) \
    { \
        uint32_t oldCapacity = set->capacity; \
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
        free(old); \
    } \
    \
    void typeName ## Init(typeName* set, typeName ## Options options) \
    { \
        set->defaultValue = options.defaultValue; \
        set->hashFn = options.hashFn; \
        set->equalsFn = options.equalsFn; \
        set->freeFn = options.freeFn; \
        set->shift = 29; \
        set->capacity = 8; \
        set->loadFactor = 6; \
        set->count = 0; \
        set->entries = (typeName ## __Entry__ *)calloc(set->capacity, sizeof(typeName ## __Entry__)); \
    } \
    \
    void typeName ## Free(typeName* set) \
    { \
        if (!set->entries) \
            return; \
        \
        typeName ## Clear(set); \
        \
        free(set->entries); \
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
        uint32_t hash, index, next; \
        hash = index = typeName ## __fibHash(set->hashFn(item), set->shift); \
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
        next = typeName ## __findEmptyBucket(set, index); \
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
        uint32_t index, hash; \
        hash = index = typeName ## __fibHash(set->hashFn(item), set->shift); \
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
        uint32_t prevIndex, index, hash; \
        hash = prevIndex = index = typeName ## __fibHash(set->hashFn(item), set->shift); \
        \
        while (set->entries[index].active) \
        { \
            if(set->entries[index].hash == hash && set->equalsFn(set->entries[index].item, item)) \
            { \
                itemType item = set->entries[index].item; \
                \
                set->entries[prevIndex].next = set->entries[index].next; \
                set->entries[index].item = set->defaultValue; \
                set->entries[index].active = false; \
                \
                if (set->freeFn) \
                    set->freeFn(item); \
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
        set->count--; \
    } \
    \
    void typeName ## Clear(typeName* set) \
    { \
        if (!set->entries) \
            return; \
        \
        for(int32_t i = 0; i < set->count; i++) \
        { \
            if (set->entries[i].active) \
            { \
                if (set->freeFn) \
                    set->freeFn(set->entries[i].item); \
                \
                set->entries[i].active = false; \
            } \
        } \
        \
        set->count = 0; \
    }

#endif //SHL_SET_H